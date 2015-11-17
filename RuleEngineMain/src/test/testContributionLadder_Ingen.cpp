#include <string>
#include <vector>
#include <iostream>

#include "gtest/gtest.h"

#include "../ruleengine/Constant_sbx.h"
#include "../ruleengine/KonceptInfo_sbx.h"
#include "../ruleengine/ProductElementValue_sbx.h"
#include "../ruleengine/RuleConstantContainer_sbx.h"
#include "../ruleengine/RuleEngine_sbx.h"
#include "../ruleengine/RuleCatalogue.h"
#include "../ruleengine/Rule.h"
#include "../ruleengine/sbxTypes.h"
#include "../ruleengine/TA_sbx.h"
#include "../ruleengine/ValidationResult.h"

#include "ruleenginetestutils.h"
#include "testRuleEngineInitialiser.cpp"

using namespace std;
using namespace sbx;

class ContributionLadder_Ingen_CONTEXT_KI_OSV_25_50: public RuleEngineInitialiser {
protected:
    virtual void SetUp() {
    	RuleEngineInitialiser::SetUp();

		KonceptInfo ki { 4, 30, 0, // UnderkonceptOid:OSV 25-49
				{ { 1, "true" }, // Solidarisk faellestarif
						{ 6, "true" }, // SEB Firmapensionspulje
						{ 11, "true" }, // Parameter-Basis
						{ 15, "true" } // FG-Spaend
				} };
		re.initContext(ki, OUTSIDE);
	}
};

// Test kIndmeldelsesalder and getting options prior to that
TEST_F(ContributionLadder_Ingen_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Ingen_No_Hospitalsdaekning)
{
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Ingen");
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kHospitalsdaekning_MK, false);

	// expecting complains about missing step
	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;
	EXPECT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kProductElementRequired));

	r = re.validate(ta, (unsigned short) kBidragsstigningsform);
	EXPECT_TRUE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;

	r = re.validate(ta, { kBidragsstigningsform, kBidragstrappe });
	EXPECT_TRUE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;
	EXPECT_EQ(2, r.getWarnings(kBidragstrappe).size());
	EXPECT_TRUE(r.hasWarnings(kBidragstrappe, kProductElementRequired));

	//  now add one step, but let it start from 3 (!=0), and it should fail
	ta.addContributionStep( { 3, 3.4, 5.3 });
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueNotAllowed));

	//  now remove that step, and add one that starts from 0 and no complains
	ta.removeContributionStep( { 3, 0, 0 });
	ta.addContributionStep( { 0, 3.4, 5.3 });
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
//	if (!r.isAllOk())
	cout << r;

	//  then add one more and it should complain again, that only 1 step is allowed, when "Ingen" is selected in Bidragsstigningsform
	ta.addContributionStep( { 1, 4, 6 });
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueNotAllowed));

	//  now remove the last step (index!=0), and everything should be fine again
	ta.removeContributionStep( { 1, 0, 0 });
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
//	if (!r.isAllOk())
	cout << r;
}

// Test kIndmeldelsesalder and getting options prior to that
TEST_F(ContributionLadder_Ingen_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Ingen_Hospitalsdaekning)
{
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Ingen");
	ta.setValue(kHospitalsdaekning_MK, true);
	ta.setValue(kHospitalsdaekningLeverandoer, "Codan");
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);

	// expecting complains about missing step
	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;
	ASSERT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_EQ(kProductElementRequired, r.getValidationResults(kBidragstrappe).at(0).getValidationCode());

	r = re.validate(ta, (unsigned short) kBidragsstigningsform);
	EXPECT_TRUE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;

	r = re.validate(ta, { kBidragsstigningsform, kBidragstrappe });
	EXPECT_TRUE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;
	EXPECT_EQ(2, r.getWarnings(kBidragstrappe).size());
	EXPECT_TRUE(r.hasWarnings(kBidragstrappe, kProductElementRequired));

	//  now add one step, but let it start from 3 (!=0), and it should fail
	ta.addContributionStep( { 3, 3.4, 5.3 });
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueNotAllowed));

	//  now remove that step, and add one that starts from 0 and no complains
	ta.removeContributionStep( { 3, 0, 0 });
	ta.addContributionStep( { 0, 3.4, 5.3 });
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
//	if (!r.isAllOk())
	cout << r;

	//  then add one more and it should complain again, that only 1 step is allowed, when "Ingen" is selected in Bidragsstigningsform
	ta.addContributionStep( { 1, 4, 6 });
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueNotAllowed));

	//  now remove the last step (index!=0), and everything should be fine again
	ta.removeContributionStep( { 1, 0, 0 });
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
//	if (!r.isAllOk())
	cout << r;
}

TEST_F(ContributionLadder_Ingen_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Ingen_PrivateTaxedMK)
{
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
	ta.setValue(kAftaleIkraftdato, 20150701);
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kBidragEjFoesteTrin_MK, false);
	ta.setValue(kBidragsstigningsform, "Ingen");
	ta.setValue(kHospitalsdaekning_MK, false);
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);
	ta.setValue(kTAEReguleringskode, "Ingen");
	ta.setValue(kDoedReguleringskode, "Ingen");
	ta.setValue(kPrivate_Taxed_MK, true);
	ta.setValue(kPrivate_Taxed_Reguleringskode, "Ingen");
	ta.setValue(kPrivate_Premium_BL, 50000);
	ta.setValue(kPrivate_Taxed_SA_Administrated_MK, true);

	// expecting complains about missing step
	auto r = re.validate(ta, true);
	cout << r;
	EXPECT_FALSE(r.hasMessages(kBidragstrappe, kProductElementRequired));
}


TEST_F(ContributionLadder_Ingen_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Ingen_PrivateTaxedMK_Pristal_NEGATIVE)
{
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
	ta.setValue(kAftaleIkraftdato, 20150701);
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kBidragEjFoesteTrin_MK, false);
	ta.setValue(kBidragsstigningsform, "Ingen");
	ta.setValue(kHospitalsdaekning_MK, false);
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);
	ta.setValue(kTAEReguleringskode, "Pristal");
	ta.setValue(kDoedReguleringskode, "Pristal");
	ta.setValue(kPrivate_Taxed_MK, true);

	ta.setValue(kPrivate_Premium_BL, 0);
	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_TRUE(r.hasMessages(kPrivate_Premium_BL, kValueUnderLimit));
	EXPECT_TRUE(r.hasMessages(kPrivate_Taxed_Reguleringskode, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kPrivate_Taxed_SA_Administrated_MK, kProductElementRequired));

	ta.setValue(kPrivate_Taxed_Reguleringskode, "Pristal");
	ta.setValue(kPrivate_Taxed_SA_Administrated_MK, true);
	ta.setValue(kPrivate_Premium_BL, 49999);
	r = re.validate(ta, true);
	cout << r;
	EXPECT_TRUE(r.hasMessages(kPrivate_Premium_BL, kValueUnderLimit));

	ta.setValue(kPrivate_Premium_BL, 50000);
	ta.setValue(kPrivate_Premium_pct, 5);
	r = re.validate(ta, true);
	cout << r;
	EXPECT_FALSE(r.hasMessages(kPrivate_Premium_BL, kValueUnderLimit));
	EXPECT_TRUE(r.hasMessages(kPrivate_Premium_pct, kProductElementNotAllowed));

	ta.remove(kPrivate_Premium_pct);

	r = re.validate(ta, true);
	cout << r;
	EXPECT_FALSE(r.hasMessages(kPrivate_Premium_pct, kProductElementNotAllowed));
}

TEST_F(ContributionLadder_Ingen_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Ingen_PrivateTaxedMK_Ingen_NEGATIVE)
{
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
	ta.setValue(kAftaleIkraftdato, 20150701);
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kBidragEjFoesteTrin_MK, false);
	ta.setValue(kBidragsstigningsform, "Ingen");
	ta.setValue(kHospitalsdaekning_MK, false);
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);
	ta.setValue(kTAEReguleringskode, "Ingen");
	ta.setValue(kDoedReguleringskode, "Ingen");
	ta.setValue(kPrivate_Taxed_MK, true);

	ta.setValue(kPrivate_Premium_BL, 0);
	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_TRUE(r.hasMessages(kPrivate_Premium_BL, kValueUnderLimit));
	EXPECT_TRUE(r.hasMessages(kPrivate_Taxed_Reguleringskode, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kPrivate_Taxed_SA_Administrated_MK, kProductElementRequired));

	ta.setValue(kPrivate_Taxed_Reguleringskode, "Ingen");
	ta.setValue(kPrivate_Taxed_SA_Administrated_MK, true);
	ta.setValue(kPrivate_Premium_BL, 49999);
	r = re.validate(ta, true);
	cout << r;
	EXPECT_TRUE(r.hasMessages(kPrivate_Premium_BL, kValueUnderLimit));

	ta.setValue(kPrivate_Premium_BL, 50000);
	r = re.validate(ta, true);
	cout << r;
	EXPECT_FALSE(r.hasMessages(kPrivate_Premium_BL, kValueUnderLimit));
}

TEST_F(ContributionLadder_Ingen_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Ingen_PrivateTaxedMK_Gage_NEGATIVE)
{
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
	ta.setValue(kAftaleIkraftdato, 20150701);
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kBidragEjFoesteTrin_MK, false);
	ta.setValue(kBidragsstigningsform, "Ingen");
	ta.setValue(kHospitalsdaekning_MK, false);
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);
	ta.setValue(kTAEReguleringskode, "Gage");
	ta.setValue(kDoedReguleringskode, "Gage");
	ta.setValue(kPrivate_Taxed_MK, true);
	ta.setValue(kPrivate_Taxed_Reguleringskode, "Gage");
	ta.setValue(kPrivate_Taxed_SA_Administrated_MK, true);
	ta.setValue(kPrivate_Premium_pct, 3);
	ta.setValue(kPrivate_Premium_BL, 40000);

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kPrivate_Premium_pct, kValueUnderLimit));
	EXPECT_TRUE(r.hasMessages(kPrivate_Premium_BL, kProductElementNotAllowed));
	cout << r;

	ta.setValue(kPrivate_Premium_pct, 5);
	ta.remove(kPrivate_Premium_BL);
	r = re.validate(ta, true);
	EXPECT_FALSE(r.hasMessages(kPrivate_Premium_pct, kValueUnderLimit));
	EXPECT_FALSE(r.hasMessages(kPrivate_Premium_BL, kProductElementNotAllowed));
	cout << r;
}

TEST_F(ContributionLadder_Ingen_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Ingen_PrivateTaxedMK_Hospitalsdaekning_NEGATIVE)
{
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
	ta.setValue(kAftaleIkraftdato, 20150701);
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kBidragEjFoesteTrin_MK, false);
	ta.setValue(kBidragsstigningsform, "Ingen");
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);
	ta.setValue(kTAEReguleringskode, "Gage");
	ta.setValue(kDoedReguleringskode, "Gage");
	ta.setValue(kPrivate_Taxed_MK, true);
	ta.setValue(kPrivate_Taxed_Reguleringskode, "Gage");
	ta.setValue(kPrivate_Taxed_SA_Administrated_MK, true);
	ta.setValue(kPrivate_Premium_pct, 5);
	ta.setValue(kHospitalsdaekning_MK, true);

	auto r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kHospitalsdaekning_MK, kValueNotAllowed));

	ta.setValue(kHospitalsdaekning_MK, false);
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.hasMessages(kHospitalsdaekning_MK, kValueNotAllowed));
}

TEST_F(ContributionLadder_Ingen_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Ingen_PrivateTaxedMK_NEGATIVE)
{
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
	ta.setValue(kAftaleIkraftdato, 20150701);
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kTAEReguleringskode, "Gage");
	ta.setValue(kDoedReguleringskode, "Gage");
	ta.setValue(kBidragEjFoesteTrin_MK, false);
	ta.setValue(kBidragsstigningsform, "Ingen");
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);
	ta.setValue(kPrivate_Taxed_MK, true);
	ta.setValue(kPrivate_Taxed_Reguleringskode, "Gage");
	ta.setValue(kPrivate_Taxed_SA_Administrated_MK, true);
	ta.setValue(kPrivate_Premium_pct, 5);
	ta.setValue(kHospitalsdaekning_MK, true);

	auto r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kHospitalsdaekning_MK, kValueNotAllowed));

	ta.setValue(kHospitalsdaekning_MK, false);
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.hasMessages(kHospitalsdaekning_MK, kValueNotAllowed));
}


TEST_F(ContributionLadder_Ingen_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Ingen_PrivateTaxedMK_Reguleringskode1_NEGATIVE)
{
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
	ta.setValue(kAftaleIkraftdato, 20150701);
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kTAEReguleringskode, "Ingen");
	ta.setValue(kDoedReguleringskode, "Gage");
	ta.setValue(kBidragEjFoesteTrin_MK, false);
	ta.setValue(kBidragsstigningsform, "Ingen");
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);
	ta.setValue(kPrivate_Taxed_MK, true);
	ta.setValue(kPrivate_Taxed_Reguleringskode, "Gage");
	ta.setValue(kPrivate_Taxed_SA_Administrated_MK, true);
	ta.setValue(kPrivate_Premium_pct, 5);
	ta.setValue(kHospitalsdaekning_MK, false);

	auto r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kDoedReguleringskode, kValueNotAllowed));
	EXPECT_TRUE(r.hasMessages(kTAEReguleringskode, kValueNotAllowed));

	ta.setValue(kTAEReguleringskode, "Gage");
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.hasMessages(kDoedReguleringskode, kValueNotAllowed));
	EXPECT_FALSE(r.hasMessages(kTAEReguleringskode, kValueNotAllowed));
}

TEST_F(ContributionLadder_Ingen_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Ingen_PrivateTaxedMK_Reguleringskode2_NEGATIVE)
{
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
	ta.setValue(kAftaleIkraftdato, 20150701);
	ta.setValue(kTAEReguleringskode, "Ingen");
	ta.setValue(kDoedReguleringskode, "Pristal");
	ta.setValue(kBidragEjFoesteTrin_MK, false);
	ta.setValue(kBidragsstigningsform, "Ingen");
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);
	ta.setValue(kPrivate_Taxed_MK, true);
	ta.setValue(kPrivate_Taxed_Reguleringskode, "Ingen");
	ta.setValue(kPrivate_Taxed_SA_Administrated_MK, true);
	ta.setValue(kPrivate_Premium_pct, 5);
	ta.setValue(kHospitalsdaekning_MK, false);

	auto r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kDoedReguleringskode, kValueNotAllowed));
	EXPECT_TRUE(r.hasMessages(kTAEReguleringskode, kValueNotAllowed));

	ta.setValue(kTAEReguleringskode, "Pristal");
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.hasMessages(kDoedReguleringskode, kValueNotAllowed));
	EXPECT_FALSE(r.hasMessages(kTAEReguleringskode, kValueNotAllowed));
}

TEST_F(ContributionLadder_Ingen_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Ingen_Over100_NEGATIVE)
{
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
	ta.setValue(kAftaleIkraftdato, 20150701);
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kBidragEjFoesteTrin_MK, false);
	ta.setValue(kBidragsstigningsform, "Ingen");
	ta.setValue(kHospitalsdaekning_MK, false);
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);
	ta.setValue(kPrivate_Taxed_MK, false);

	ta.addContributionStep( {0, 90, 11} );

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueOverLimit));

	ta.addContributionStep( {0, 90, 9} );
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
}

TEST_F(ContributionLadder_Ingen_CONTEXT_KI_OSV_25_50, PrivateTaxedMK_TAETilfirmaMK_NEGATIVE)
{
    RuleEngine::_printDebugAtValidation = true;
    TA ta { "15124040"}; // KonceptOid 4 - OSV
    ta.setValue(kAftaleIkraftdato, 20150701);
    ta.setValue(kPrivate_Taxed_MK, true);
    ta.setValue(kTAETilfirma_MK, true);
    
    auto r = re.validate(ta, false);
    cout << r;
    EXPECT_FALSE(r.isAllOk());
    EXPECT_TRUE(r.hasMessages(kTAETilfirma_MK, kValueNotAllowed));
    
    ta.setValue(kTAETilfirma_MK, false);
    r = re.validate(ta, false);
    cout << r;
    EXPECT_FALSE(r.hasMessages(kPrivate_Taxed_MK, kValueNotAllowed));
    EXPECT_FALSE(r.hasMessages(kTAETilfirma_MK, kValueNotAllowed));
}
