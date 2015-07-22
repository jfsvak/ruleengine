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

class RelatedSubkoncepts : public RuleEngineInitialiser {
protected:
    virtual void SetUp() {
    	RuleEngineInitialiser::SetUp();

        KonceptInfo ki {PROSA, 5, 0, // UnderkonceptOid:OSV 25-49
        	{ {11, "true"}, // Parameter-Basis
        	  {1, "true"}, // Solidarisk faellestarif
			  {15, "true"}, // FG span
			  {6, "true"} // SEB Firmapensionspulje
        	} };
        re.initContext(ki, OUTSIDE);
    }
};

TEST_F(RelatedSubkoncepts, Udlobsalder_Pension_Allowed_Values) {
	RuleEngine::_printDebug = true;
	RuleEngine::_printDebugAtValidation = true;
	re.getContainer().printKoncepts();

	re.getContainer().printSubkonceptConstants(18);
	re.getContainer().printSubkonceptConstants(29);
	re.getContainer().printParametersToProducts(29);

	TA ta {"20247940"};

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

TEST_F(RelatedSubkoncepts, Private_Taxed_MK_NotAllowed) {
	RuleEngine::_printDebug = true;
	RuleEngine::_printDebugAtValidation = true;
	re.getContainer().printKoncepts();

	TA ta {"20247940"};

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

TEST_F(RelatedSubkoncepts, ContextSwitch) {
	RuleEngine::_printDebug = true;
	RuleEngine::_printDebugAtValidation = true;
	RuleEngine re2{};
	re2.initialiseAll(get_file_contents("basedata-all.json"));
	re2.parseRuleCatalogueJSON(get_file_contents("rule-catalogue.json"));

    KonceptInfo kiOSV {OSV, 55, 0, //
    	{ {11, "true"}, // Parameter-Basis
    	  {1, "true"}, // Solidarisk faellestarif
		  {15, "true"}, // FG span
		  {6, "true"} // SEB Firmapensionspulje
    	} };

    re2.initContext(kiOSV, OUTSIDE);

	re2.getContainer().printSubkonceptConstants(18, 216);

	TA ta {"20247940"};

	ta.setValue(kDoedReguleringskode, "Pristal");
	ta.setValue(kDoedBlGrMin, (long) 50000);
	ta.setValue(kDoedBlOblMax, (long) 200000);

	auto r = re2.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

    KonceptInfo kiITPension {ITPENSION, 15, 0, //
    	{ {11, "true"}, // Parameter-Basis
    	  {1, "true"}, // Solidarisk faellestarif
		  {15, "true"}, // FG span
		  {6, "true"} // SEB Firmapensionspulje
    	} };
    re2.initContext(kiITPension, OUTSIDE);

	r = re2.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_TRUE(r.hasMessages(kDoedBlGrMin, kValueUnderLimit));

	ta.setValue(kDoedBlGrMin, (long) 100000);
	r = re2.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

}





