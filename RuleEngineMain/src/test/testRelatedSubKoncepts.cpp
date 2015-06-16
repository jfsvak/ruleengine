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

class RelatedSubkoncepts_Prosa : public ::testing::Test  {
protected:
    virtual void SetUp() {
    	RuleEngine::_printDebugAtValidation = false;
    	re = RuleEngine();
        re.initConstants(get_file_contents("basedata-ruleconstants.json"));
        re.initKoncepts(get_file_contents("koncepts.json"));
        re.parseRuleCatalogueJSON(get_file_contents("rule-catalogue.json"));

        KonceptInfo ki {PROSA, 5, 0, // UnderkonceptOid:OSV 25-49
        	{ {11, "true"}, // Parameter-Basis
        	  {1, "true"}, // Solidarisk faellestarif
			  {15, "true"}, // FG span
			  {6, "true"} // SEB Firmapensionspulje
        	} };
        re.initContext(ki, OUTSIDE);
    }

    RuleEngine re;
};

TEST_F(RelatedSubkoncepts_Prosa, Udlobsalder_Pension_Allowed_Values) {
	RuleEngine::_printDebug = true;
	RuleEngine::_printDebugAtValidation = true;
	re.getContainer().printKoncepts();

	re.getContainer().printConstants(18);
	re.getContainer().printConstants(29);
	re.getContainer().printParametersToProducts(29);

	TA ta {"20247940", PROSA};

	ta.setValue(kUdlobsalder_Pension, (long) 65);

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kUdlobsalder_Pension, kValueNotAllowed));
	cout << r;

	ta.setValue(kUdlobsalder_Pension, (long) 67);
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;
}

TEST_F(RelatedSubkoncepts_Prosa, Private_Taxed_MK_NotAllowed) {
	RuleEngine::_printDebug = true;
	RuleEngine::_printDebugAtValidation = true;
	re.getContainer().printKoncepts();

	TA ta {"20247940", PROSA};

	ta.setValue(kPrivate_Taxed_MK, true);

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kPrivate_Taxed_MK, kValueNotAllowed));
	cout << r;

	ta.setValue(kPrivate_Taxed_MK, false);
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;
}





