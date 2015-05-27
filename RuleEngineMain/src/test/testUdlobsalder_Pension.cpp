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


// Test Udlobsalder_Pension - PE(71)-P(13)
// Allowed values, 60, 62, 65, 67
TEST_F(RuleEngine_CONTEXT_KI_OSV_25_50, UdloebsalderPension_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	auto options = re.getOptionsList(kUdlobsalder_Pension);
	ta.setValue(kUdlobsalder_Pension, (long) 60 );

	auto r = re.validate(ta, (unsigned short) kUdlobsalder_Pension);
	EXPECT_EQ(true, r.isAllOk());
}


// Test Udlobsalder_Pension - PE(71)-P(13)
// Allowed values, 60, 62, 65, 67
TEST_F(RuleEngine_CONTEXT_KI_OSV_25_50, UdloebsalderPension_NEGATIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kUdlobsalder_Pension, (long) 61); // value not allowed

	RuleEngine::_printDebugAtValidation = true;

	auto r = re.validate(ta, (unsigned short) kUdlobsalder_Pension);
	EXPECT_FALSE(r.isAllOk());

	auto v = r.getValidationResults(kUdlobsalder_Pension);
	EXPECT_EQ(kValueNotAllowed, v.at(0).getValidationCode());
}
