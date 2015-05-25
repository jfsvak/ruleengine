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

using namespace std;
using namespace sbx;

class Doedsfaldsdaekning_KI_OSV_25_49 : public ::testing::Test  {
protected:
    virtual void SetUp() {
    	RuleEngine::_printDebugAtValidation = false;
    	re = RuleEngine();
        re.initConstants(get_file_contents("basedata-ruleconstants.json"));
        re.parseRuleCatalogueJSON(get_file_contents("rule-catalogue.json"));

        KonceptInfo ki {17, // UnderkonceptOid:OSV 25-49
        	{ {11, "true"}, // Parameter-Basis
        	  {1, "true"}, // Solidarisk faellestarif
			  {6, "true"} // SEB Firmapensionspulje
        	} };
        re.initContext(ki);
    }

    RuleEngine re;
};


// Test DoedBlGrMin
// Allowed values: [0 - 5,000,000]
// Expected result:
//   Validation of value should be ok
//   But a warning should be set that says the DoedReguleringskode is not set in the parser
TEST_F(Doedsfaldsdaekning_KI_OSV_25_49, DoedBlGrMin_Single_Value_OK_With_Warning) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kDoedBlGrMin, (long) 100000);
	RuleEngine::_printDebugAtValidation = true;

	auto r = re.validate(ta, (unsigned short) kDoedBlGrMin);
	EXPECT_TRUE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;

	auto v = r.getWarnings(kDoedReguleringskode);
	ASSERT_EQ(1, v.size());
	EXPECT_EQ(kTokenNotDefined, v.at(0).getValidationCode());
}

// Test DoedBlGrMin
// Allowed values: [0 - 5,000,000]
// Expected result:
//   Validation of value for DoedBlGrMin should be ok
//   But a warning should be set that says the DoedReguleringskode is not set in the parser
TEST_F(Doedsfaldsdaekning_KI_OSV_25_49, DoedBlGrMin_Single_Value_OK_With_Warning2) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kDoedBlGrMin, (long) 100000);
	RuleEngine::_printDebugAtValidation = true;

	auto r = re.validate(ta, {kDoedBlGrMin, kDoedReguleringskode});
	EXPECT_TRUE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;

	auto v = r.getWarnings(kDoedReguleringskode);

	EXPECT_EQ(kTokenNotDefined, v.at(0).getValidationCode());
}

/**
 * Short description:
 *   Testing the whole section of Dodsfaldsdaekning incl. Boernerente
 *
 * Product Elements being tested:
 *   DoedReguleringskode = Gage,
 *   DoedPctGrMin, DoedPctOblMax, DoedSpaendPct
 *   DoedBlGrMin
 *   DoedSoliMax
 *   DoedDaekningstype
 *   DoedSkattekode
 *   BoerneUdloebsalder
 *   Boernerente_Reguleringstype = Gage,
 *   BoerneRenteUdloebsalder
 *   BoerneRenteBlMin
 *   BoerneRenteSoliMax
 *   BoerneSumBlMin
 *
 * Expected results:
 *   isAllOk() == true, i.e. no validation messages
 *
 * Expected warnings:
 *  no warnings expected
 *
 */
TEST_F(Doedsfaldsdaekning_KI_OSV_25_49, Doedfaldsdaekning_Whole_Section_GAGE_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kDoedReguleringskode, "Gage");
	ta.setValue(kDoedPctGrMin, (long) 200);
	ta.setValue(kDoedPctOblMax, (long) 300);
	ta.setValue(kDoedBlGrMin, (long) 200000);

	ta.setValue(kDoedSoliMax, "Tegningsmaks");
	ta.setValue(kDoedDaekningstype, "115 DoeD Gennemsnitspraemie");
	ta.setValue(kDoedSkattekode, "Skattefri daekning");

	ta.setValue(kBoernerente_Reguleringstype, "Gage");
	ta.setValue(kBoerneUdloebsalder, (long) 24);
	ta.setValue(kBoerneRenteBlMin, (long) 5);

	ta.setValue(kBoerneRenteSoliMax, "Obligatorisk maks");
	ta.setValue(kBoerneSumBlMin, (long) 25000);

	RuleEngine::_printDebugAtValidation = true;

	re.getContainer().printConstants(17, 5);
	re.getContainer().printConstants(17, 139);

	auto r = re.validate(ta,
			{
			kDoedReguleringskode,
			kDoedPctGrMin,
			kDoedPctOblMax,
			kDoedSpaendPct,
			kDoedBlGrMin,
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

//	if (!r.isAllOk())
		cout << r;

	EXPECT_EQ(8, r.getWarnings().size());
}

/**
 * Short description:
 *   Testing the DodReguleringstype = Gage
 *
 * Product Elements being tested:
 *   DoedReguleringskode = Gage
 *   DoedPctGrMin, DoedPctOblMax, DoedSpaendPct
 *   DoedBlGrMin
 *
 * Expected results:
 *   isAllOk() == true, i.e. no validation messages
 *
 * Expected warnings:
 *  no warnings expected
 *
 */
TEST_F(Doedsfaldsdaekning_KI_OSV_25_49, DoedReguleringstype_GAGE_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kDoedReguleringskode, "Gage");
	ta.setValue(kDoedPctGrMin, (long) 200);
	ta.setValue(kDoedPctOblMax, (long) 300);
	ta.setValue(kDoedBlGrMin, (long) 200000);

	RuleEngine::_printDebugAtValidation = true;

	auto r = re.validate(ta,
			{
			kDoedReguleringskode,
			kDoedPctGrMin,
			kDoedPctOblMax,
			kDoedSpaendPct,
			kDoedBlGrMin
	});

	EXPECT_TRUE(r.isAllOk());

//	if (!r.isAllOk())
		cout << r;

	EXPECT_EQ(0, r.getWarnings().size());
}

//////////////////////////////////////////////////////////
// -----------------------------
//    NEGATIVE SCENARIOS BELOW
// ----------------------------
//////////////////////////////////////////////////////////


// Test Gage with max amount, which should fail
// Expected:
//    No value allowed for DoedBlOblMax as DoedReguleringskode == Gage
TEST_F(Doedsfaldsdaekning_KI_OSV_25_49, DoedBlOblMax_Single_Value_NOT_OK) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kDoedReguleringskode, "Gage");
	ta.setValue(kDoedBlOblMax, (long) 700000);
	RuleEngine::_printDebugAtValidation = true;

	auto r = re.validate(ta, (unsigned short) kDoedBlOblMax);
	cout << r;
	EXPECT_FALSE(r.isAllOk());

	std::vector<sbx::ValidationResult> v = r.getValidationResults(kDoedBlOblMax);
	ASSERT_EQ(1, v.size());
	EXPECT_EQ(sbx::ValidationCode::kProductElementNotAllowed, v.at(0).getValidationCode());

	if (sbx::ValidationCode::kProductElementNotAllowed != v.at(0).getValidationCode())
		cout << r;
}

// Test missing value on TA, DoedBlOblMax
// Allowed values:
// Expected:
//    Warnings should contain a warning say that the product element being validated is missing on the TA
TEST_F(Doedsfaldsdaekning_KI_OSV_25_49, DoedBlGrMin_ValidateNonExistingToken_NOT_OK) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kDoedReguleringskode, "Pristal");

	// set MIN value, DoedBlGrMin
	ta.setValue(kDoedBlGrMin, (long) 100000);
	RuleEngine::_printDebugAtValidation = true;

	// ... but validate MAX, DoedBlOblMax
	auto r = re.validate(ta, (unsigned short) kDoedBlOblMax);

	EXPECT_TRUE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;

	ASSERT_EQ(1, r.sizeWarnings());
	std::vector<sbx::ValidationResult> v = r.getWarnings(kDoedBlOblMax);

	EXPECT_EQ(sbx::ValidationCode::kProductElementNotDefined, v.at(0).getValidationCode());
}
