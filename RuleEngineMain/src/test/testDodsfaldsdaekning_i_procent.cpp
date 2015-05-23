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

class RuleEngine_CONTEXT_KI_OSV_25_50 : public ::testing::Test  {
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
//
TEST_F(RuleEngine_CONTEXT_KI_OSV_25_50, DoedBlGrMin_Single_Value_OK) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kDoedBlGrMin, (long) 100000);
	RuleEngine::_printDebugAtValidation = true;

	auto r = re.validate(ta, (unsigned short) kDoedBlGrMin);
	EXPECT_TRUE(r.isAllOk());
	if (!r.isAllOk())
		cout << r;
}

// Test DoedBlGrMin
// Allowed values: [0 - 5,000,000]
// Expected:
//    No value allowed for DoedBlGrMin as DoedReguleringskode == Gage
TEST_F(RuleEngine_CONTEXT_KI_OSV_25_50, DoedBlGrMin_Single_Value_NOT_OK) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kDoedReguleringskode, "Gage");
	ta.setValue(kDoedBlGrMin, (long) 100000);

	auto r = re.validate(ta, (unsigned short) kDoedBlGrMin);

	EXPECT_FALSE(r.isAllOk());

	std::vector<sbx::ValidationResult> v = r.getValidationResults(kDoedBlGrMin);
	ASSERT_EQ(1, v.size());
	EXPECT_EQ(sbx::ValidationCode::kProductElementNotAllowed, v.at(0).getValidationCode());

	if (sbx::ValidationCode::kProductElementNotAllowed != v.at(0).getValidationCode())
		cout << r;
}

// Test missing value on TA, DoedBlOblMax
// Allowed values:
// Expected:
//    Warnings should contain a warning say that the product element being validated is missing on the TA
TEST_F(RuleEngine_CONTEXT_KI_OSV_25_50, DoedBlGrMin_ValidateNonExistingToken_NOT_OK) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kDoedReguleringskode, "Pristal");

	// set MIN value, DoedBlGrMin
	ta.setValue(kDoedBlGrMin, (long) 100000);
	RuleEngine::_printDebugAtValidation = true;

	// ... but validate MAX, DoedBlOblMax
	auto r = re.validate(ta, (unsigned short) kDoedBlOblMax);

	EXPECT_TRUE(r.isAllOk());
	if (!r.isAllOk())
		cout << r;

	ASSERT_EQ(1, r.sizeWarnings());
	std::vector<sbx::ValidationResult> v = r.getWarnings(kDoedBlOblMax);

	EXPECT_EQ(sbx::ValidationCode::kProductElementNotDefined, v.at(0).getValidationCode());
}
