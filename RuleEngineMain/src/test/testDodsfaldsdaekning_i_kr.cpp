#include <string>
#include <vector>
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

class Doedsfaldsdaekning_i_kr_KI_OSV_25_49 : public RuleEngineInitialiser {
protected:
    virtual void SetUp() {
    	RuleEngineInitialiser::SetUp();

        KonceptInfo ki {4, 30, 0, // UnderkonceptOid:OSV 25-49
        	{ {11, "true"}, // Parameter-Basis
        	  {1, "true"}, // Solidarisk faellestarif
			  {6, "true"} // SEB Firmapensionspulje
        	} };
        re.initContext(ki, OUTSIDE);
    }
};

class Doedsfaldsdaekning_i_kr_KI_OSV_Over_99 : public ::testing::Test  {
protected:
    virtual void SetUp() {
    	RuleEngine::_printDebugAtValidation = false;
    	re = RuleEngine();
        re.initConstants(get_file_contents("basedata-ruleconstants.json"));
        re.initKoncepts(get_file_contents("koncepts.json"));
        re.parseRuleCatalogueJSON(get_file_contents("rule-catalogue.json"));

        KonceptInfo ki {4, 100, 0, // UnderkonceptOid:OSV >99
        	{ {11, "true"}, // Parameter-Basis
        	  {1, "true"}, // Solidarisk faellestarif
			  {6, "true"} // SEB Firmapensionspulje
        	} };
        re.initContext(ki, OUTSIDE);
    }

    RuleEngine re;
};

TEST_F(Doedsfaldsdaekning_i_kr_KI_OSV_25_49, Doedfaldsdaekning_Whole_Section_Pristal_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kDoedReguleringskode, "Pristal");
	ta.setValue(kDoedBlGrMin, (long) 400000);
	ta.setValue(kDoedBlOblMax, (long) 500000);

	ta.setValue(kDoedSoliMax, "Tegningsmaks");
	ta.setValue(kDoedDaekningstype, "115 DØD Gennemsnitspræmie");
	ta.setValue(kDoedSkattekode, "Skattefri dækning");

	ta.setValue(kBoernerente_Reguleringstype, "Gage");
	ta.setValue(kBoerneUdloebsalder, (long) 24);
	ta.setValue(kBoerneRenteBlMin, (long) 5);

	ta.setValue(kBoerneRenteSoliMax, "Obligatorisk maks");
	ta.setValue(kBoerneSumBlMin, (long) 25000);

	RuleEngine::_printDebugAtValidation = true;

	re.printConstantsInParser();
	const auto& r = re.validate(ta,
			{
			kDoedReguleringskode,
			kDoedBlGrMin,
			kDoedBlOblMax,
			kDoedSpaendBl,
			kDoedSoliMax,
			kDoedDaekningstype,
			kDoedSkattekode,
			kBoernerente_Reguleringstype,
			kBoerneUdloebsalder,
			kBoerneRenteBlMin,
			kBoerneRenteSoliMax,
			kBoerneSumBlMin
	});

	EXPECT_TRUE(r.isAllOk());

	if (!r.isAllOk())
		cout << r;
}

TEST_F(Doedsfaldsdaekning_i_kr_KI_OSV_25_49, DoedReguleringstype_Ingen_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kDoedReguleringskode, "Ingen");
	ta.setValue(kDoedBlGrMin, (long) 400000);
	ta.setValue(kDoedBlOblMax, (long) 600000);

	RuleEngine::_printDebugAtValidation = true;

	re.printConstantsInParser();
	auto r = re.validate(ta,
			{
			kDoedReguleringskode,
			kDoedBlGrMin,
			kDoedBlOblMax,
			kDoedSpaendBl
	});

	EXPECT_TRUE(r.isAllOk());

//	if (!r.isAllOk())
		cout << r;
}

TEST_F(Doedsfaldsdaekning_i_kr_KI_OSV_25_49, DoedReguleringstype_Ingen_SpanOverLimit_Negative) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kDoedReguleringskode, "Ingen");
	ta.setValue(kDoedBlGrMin, (long) 200000);
	ta.setValue(kDoedBlOblMax, (long) 500000);

	RuleEngine::_printDebugAtValidation = true;

	re.printConstantsInParser();
	auto r = re.validate(ta,
			{
			kDoedReguleringskode,
			kDoedBlGrMin,
			kDoedBlOblMax,
			kDoedSpaendBl
	});

	EXPECT_FALSE(r.isAllOk());

//	if (r.isAllOk())
		cout << r;

	auto v = r.getValidationResults(kDoedSpaendBl);
	ASSERT_EQ(1, v.size());
	EXPECT_EQ(sbx::ValidationCode::kValueOverLimit, v.at(0).getValidationCode());
}


TEST_F(Doedsfaldsdaekning_i_kr_KI_OSV_Over_99, DoedReguleringstype_Ingen_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kDoedReguleringskode, "Ingen");
	ta.setValue(kDoedBlGrMin, (long) 200000);
	ta.setValue(kDoedBlOblMax, (long) 500000);

	RuleEngine::_printDebugAtValidation = true;

	re.printConstantsInParser();
	auto r = re.validate(ta,
			{
			kDoedReguleringskode,
			kDoedBlGrMin,
			kDoedBlOblMax,
			kDoedSpaendBl
	});

	EXPECT_TRUE(r.isAllOk());

//	if (!r.isAllOk())
		cout << r;
}


TEST_F(Doedsfaldsdaekning_i_kr_KI_OSV_Over_99, DoedReguleringstype_Ingen_NEGATIVE_OverLimit) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kDoedReguleringskode, "Ingen");
	ta.setValue(kDoedBlGrMin, (long) 300000);
	ta.setValue(kDoedBlOblMax, (long) 750000);

	RuleEngine::_printDebugAtValidation = true;

	re.printConstantsInParser();
	auto r = re.validate(ta,
			{
			kDoedReguleringskode,
			kDoedBlGrMin,
			kDoedBlOblMax,
			kDoedSpaendBl
	});

	EXPECT_FALSE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;
	EXPECT_EQ(2, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kDoedBlOblMax, kValueOverLimit));
	EXPECT_TRUE(r.hasMessages(kDoedSpaendBl, kValueOverLimit));

}
