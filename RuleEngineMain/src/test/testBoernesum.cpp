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

class Boernesum_KI_OSV_25_49 : public RuleEngineInitialiser {
protected:
    virtual void SetUp() {
    	RuleEngineInitialiser::SetUp();

        KonceptInfo ki {4, 30, 0, // UnderkonceptOid:OSV 25-49
        	{ {11, "true"}, // Parameter-Basis
        	  {1, "true"}, // Solidarisk faellestarif
			  {15, "true"}, // FG span
			  {6, "true"} // SEB Firmapensionspulje
        	} };
        re.initContext(ki, OUTSIDE);
    }

};

TEST_F(Boernesum_KI_OSV_25_49, Boernesum_POSITIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"};

	auto r = re.validate(ta, {kBoerneSumBlMin, kBoerneSumBlMax});
	EXPECT_TRUE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;

	ta.setValue(kBoerneSumBlMin, 5000);
	ta.setValue(kBoerneSumBlMax, 6000);
	r = re.validate(ta, { kBoerneSumBlMin, kBoerneSumBlMax, kBoerneSumSpaendBl} );
	EXPECT_FALSE(r.isAllOk());
	//	if (!r.isAllOk())
		cout << r;
	EXPECT_EQ(1, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kBoerneSumSpaendBl, kValueOverLimit));
}

TEST_F(Boernesum_KI_OSV_25_49, BoernesumBlMin_OverLimit) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040" };
	ta.setValue(kBoerneSumBlMin, 50001);
	ta.setValue(kBoerneSumBlMax, 50001);

	auto r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kBoerneSumBlMin, kValueOverLimit));

	ta.setValue(kBoerneSumBlMin, 50000);
	ta.setValue(kBoerneSumBlMax, 50000);
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk());


	ta.setValue(kBoerneSumBlMin, 49999);
	ta.setValue(kBoerneSumBlMax, 49999);
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk());
}


