#include <string>
#include <vector>
#include "gtest/gtest.h"

#include "../ruleengine/KonceptInfo_sbx.h"
#include "../ruleengine/RuleEngine_sbx.h"
#include "../ruleengine/TA_sbx.h"
#include "../ruleengine/ValidationResult.h"
#include "../ruleengine/ValidationResults.h"

#include "ruleenginetestutils.h"
#include "testRuleEngineInitialiser.cpp"

using namespace std;
using namespace sbx;

class Incremental_TA_CONTEXT_KI_OSV_25_50 : public RuleEngineInitialiser {
protected:
    virtual void SetUp() {
    	RuleEngineInitialiser::SetUp();

        KonceptInfo ki {4, 30, 0, // UnderkonceptOid:OSV 25-49
        	{ {  1, "true" }, // Solidarisk faellestarif
        	  {  6, "true" }, // SEB Firmapensionspulje
        	  { 11, "true" }, // Parameter-Basis
			  { 15, "true" } // FG Span
        	} };
        re.initContext(ki, OUTSIDE);
    }
};


TEST_F(Incremental_TA_CONTEXT_KI_OSV_25_50, Incremental_TA_POSITIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);

	bool full {false};
	auto r = re.validate(ta, full);
	EXPECT_TRUE(r.isAllOk());
	if (!r.isAllOk())
		cout << r;

	//************************************************************
	// GENERAL PAGE
	//************************************************************
	//

	//
	// ***** Date
	//
	// need to calculate Ikraftdato minus rc_Ikraftdato_min number of months and see if todays date is before that.
	// same with rc_Ikraftdato_max
	ta.setValue(kIkraftDato, (long) 11);
//	r = re.validate(ta, full);
//	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk()) cout << r;
//	ASSERT_EQ(1, r.getValidationResults(kIkraftDato).size());
//	EXPECT_EQ(sbx::ValidationCode::kProductElementRequired, r.getValidationResults(kIkraftDato).at(0).getValidationCode());


	//
	// ***** Indmeldelseskriterier, age, anciennitet
	//
	ta.setValue(kIndmeldelsesAlder, (long) 5);
	r = re.validate(ta, full);
	EXPECT_FALSE(r.isAllOk());
	if (!r.isAllOk()) cout << r;
	ASSERT_EQ(1, r.getValidationResults(kIndmeldelsesAlder).size());
	EXPECT_EQ(sbx::ValidationCode::kValueUnderLimit, r.getValidationResults(kIndmeldelsesAlder).at(0).getValidationCode());

	ta.setValue(kIndmeldelsesAlder, (long) 31);
	r = re.validate(ta, full);

	EXPECT_FALSE(r.isAllOk());
	if (r.isAllOk()) cout << r;
	ASSERT_EQ(1, r.getValidationResults(kIndmeldelsesAlder).size());
	EXPECT_EQ(sbx::ValidationCode::kValueOverLimit, r.getValidationResults(kIndmeldelsesAlder).at(0).getValidationCode());

	ta.setValue(kIndmeldelsesAlder, (long) 20);
	r = re.validate(ta, full);
	EXPECT_TRUE(r.isAllOk());
	if (!r.isAllOk()) cout << r;


	ta.setValue(kAnciennitet, (long) 30);
	r = re.validate(ta, full);
	ASSERT_EQ(1, r.getValidationResults(kAnciennitet).size());
	EXPECT_EQ(sbx::ValidationCode::kValueOverLimit, r.getValidationResults(kAnciennitet).at(0).getValidationCode());
	ta.setValue(kAnciennitet, (long) 12);
	r = re.validate(ta, full);
	EXPECT_TRUE(r.isAllOk());
	if (!r.isAllOk()) cout << r;


	ta.setValue(kAnciennitetTidlArbejdsplads, true);
	r = re.validate(ta, full);
	EXPECT_TRUE(r.isAllOk());
	if (!r.isAllOk()) cout << r;

	//
	// ***** Fravalg grupper
	//
	ta.setValue(kGrpElever, true);
	ta.setValue(kGrpLaerlinge, true);
	ta.setValue(kGrpStuderende, true);
	r = re.validate(ta, full);
	EXPECT_TRUE(r.isAllOk());
	if (!r.isAllOk()) cout << r;


	//
	//************************************************************
	// PRODUKT PAGE
	//************************************************************
	//
	//
	// ****** Udlobsalder
	//
	ta.setValue(kUdlobsalder_Pension, (long) 67);
	r = re.validate(ta, full);
	EXPECT_TRUE(r.isAllOk());
	if (!r.isAllOk())
		cout << r;


	//
	// ******* Fravalg af risikodaekning
	//
	ta.setValue(kFravalgRisiko_MK, true);
	r = re.validate(ta, full);
	cout << r;

	EXPECT_FALSE(r.isAllOk());
	EXPECT_EQ(1, r.getValidationResults(kFravalgRisikoAlder).size());
	EXPECT_EQ(1, r.getValidationResults(kFravalgRisiko_MK).size());
	EXPECT_EQ(sbx::ValidationCode::kProductElementRequired, r.getValidationResults(kFravalgRisikoAlder).at(0).getValidationCode());
	EXPECT_EQ(sbx::ValidationCode::kValueNotAllowed, r.getValidationResults(kFravalgRisiko_MK).at(0).getValidationCode());
	ta.setValue(kFravalgRisikoAlder, (long) 45);

	// now age is set, so all should be ok
	r = re.validate(ta, full);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_EQ(1, r.getValidationResults(kFravalgRisiko_MK).size());
	EXPECT_EQ(sbx::ValidationCode::kValueNotAllowed, r.getValidationResults(kFravalgRisiko_MK).at(0).getValidationCode());

	ta.setValue(kFravalgRisiko_MK, false);
	ta.remove(kFravalgRisikoAlder);
	r = re.validate(ta, full);
	cout << r;
	EXPECT_TRUE(r.isAllOk());

	//
	// ****** Opsparingsprodukter
	//
	ta.setValue(kStandardProduct, "Traditionel_MK");
	ta.setValue(kTraditionel_MK, false);
	//    expecting to fail as the standardprodukt hasn't been set on the ta
	r = re.validate(ta, full);
	EXPECT_FALSE(r.isAllOk());
	if (r.isAllOk()) cout << r;

	ta.setValue(kTraditionel_MK, true);
	ta.setValue(kLink_MK, true);
	ta.setValue(kMarkedspension_MK, true);
	ta.setValue(kTidspensionMedGaranti_MK, true);
	ta.setValue(kTidspensionUdenGaranti_MK, true);

	//    now savings products has been set, so everything should be fine
	r = re.validate(ta, full);
	EXPECT_TRUE(r.isAllOk());
	if (!r.isAllOk()) cout << r;

	ta.setValue(kUdbetalingsform, re.getDefaultValue(kUdbetalingsform)->stringValue());
	r = re.validate(ta, full);
	EXPECT_TRUE(r.isAllOk());
	if (!r.isAllOk()) cout << r;

	ta.setValue(kUdbetalingsperiode, re.getDefaultValue(kUdbetalingsperiode)->longValue());
	r = re.validate(ta, full);
	EXPECT_TRUE(r.isAllOk());
	if (!r.isAllOk()) cout << r;

	ta.setValue(kMaxAndelLinkBidragPCT, (double) 5.5);
	r = re.validate(ta, full);
	EXPECT_TRUE(r.isAllOk());
	if (!r.isAllOk()) cout << r;

	ta.setValue(kMaxAndelMarkedspensionBidragPct, (double) 6.1);
	r = re.validate(ta, full);
	EXPECT_TRUE(r.isAllOk());
	if (!r.isAllOk()) cout << r;

	ta.setValue(kLinkKunForEgetBidrag_MK, true);
	r = re.validate(ta, full);
	EXPECT_TRUE(r.isAllOk());
	if (!r.isAllOk()) cout << r;

	//
	// Invaliditetsdaekning
	//
//	ta.setValue(kInvalidesum);



	//
	//************************************************************
	// SALARY PAGE
	//************************************************************
	//
	//
	// ***** Lon og betaling
	//
	ta.setValue(kLoenDefinition, "garbage");
	r = re.validate(ta, full);
	EXPECT_FALSE(r.isAllOk());
	if (r.isAllOk()) cout << r;
	ASSERT_EQ(1, r.getValidationResults(kLoenDefinition).size());
	EXPECT_EQ(sbx::ValidationCode::kValueNotAllowed, r.getValidationResults(kLoenDefinition).at(0).getValidationCode());
	ta.setValue(kLoenDefinition, re.getDefaultValue(kLoenDefinition)->stringValue());

	ta.setValue(kLoenRegulering, "garbage");
	r = re.validate(ta, full);
	EXPECT_FALSE(r.isAllOk());
	if (r.isAllOk()) cout << r;
	ASSERT_EQ(1, r.getValidationResults(kLoenRegulering).size());
	EXPECT_EQ(sbx::ValidationCode::kValueNotAllowed, r.getValidationResults(kLoenRegulering).at(0).getValidationCode());
	ta.setValue(kLoenRegulering, "januar");

	ta.setValue(kLoenreguleringsfrekvens, "garbage");
	r = re.validate(ta, full);
	EXPECT_FALSE(r.isAllOk());
	if (r.isAllOk()) cout << r;
	ASSERT_EQ(1, r.getValidationResults(kLoenreguleringsfrekvens).size());
	EXPECT_EQ(sbx::ValidationCode::kValueNotAllowed, r.getValidationResults(kLoenreguleringsfrekvens).at(0).getValidationCode());
	ta.setValue(kLoenreguleringsfrekvens, re.getDefaultValue(kLoenreguleringsfrekvens)->stringValue());

	ta.setValue(kBidragsstigningsform, "garbage");
	r = re.validate(ta, full);
	EXPECT_FALSE(r.isAllOk());
	if (r.isAllOk()) cout << r;
	ASSERT_EQ(1, r.getValidationResults(kBidragsstigningsform).size());
	EXPECT_EQ(sbx::ValidationCode::kValueNotAllowed, r.getValidationResults(kBidragsstigningsform).at(0).getValidationCode());
	ta.setValue(kBidragsstigningsform, "Ingen");
	ta.addContributionStep({0, 2, 2});

	ta.setValue(kPrivate_Taxed_MK, true); // set privately taxed but no value for premium
	r = re.validate(ta, full); // ...and expect to fail
	EXPECT_FALSE(r.isAllOk());
	if (r.isAllOk()) cout << r;
	ASSERT_EQ(1, r.getValidationResults(kPrivate_Premium_BL).size());
	EXPECT_EQ(sbx::ValidationCode::kProductElementRequired, r.getValidationResults(kPrivate_Premium_BL).at(0).getValidationCode());

	ta.setValue(kPrivate_Premium_BL, (double) 150234.5); // set a premium ...
	ta.setValue(kHospitalsdaekning_MK, false);
	r = re.validate(ta, full); // ... and expect not to fail
	EXPECT_TRUE(r.isAllOk());
	if (!r.isAllOk()) cout << r;

	ta.setValue(kBidragEjFoesteTrin_MK, false); // set the
	r = re.validate(ta, full);
	EXPECT_TRUE(r.isAllOk());
	if (!r.isAllOk()) cout << r;

}
