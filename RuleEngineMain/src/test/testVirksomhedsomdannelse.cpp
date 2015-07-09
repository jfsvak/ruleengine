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

class Virksomhedsomdannelse_KI_OSV_25_49 : public RuleEngineInitialiser {
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


TEST_F(Virksomhedsomdannelse_KI_OSV_25_49, Virksomhedsomdannelse_POSITIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
	ta.setValue(kFusion_MK, false);

	auto r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;

	ta.setValue(kFusion_MK, true);
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	//	if (!r.isAllOk())
		cout << r;
	EXPECT_EQ(4, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kFusionIndtraedelsesdato, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kFusionTidligerePensionsordning_MK, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kFusionOblDaekningSammeStoerrelse_MK, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kHelbred_leverandorskift, kProductElementRequired));

	ta.setValue(kFusionIndtraedelsesdato, (long) 20150701);
	ta.setValue(kFusionTidligerePensionsordning_MK, false);
	ta.setValue(kFusionOblDaekningSammeStoerrelse_MK, false);
	ta.setValue(kHelbred_leverandorskift, re.getDefaultValue(kHelbred_leverandorskift)->stringValue());
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	//	if (!r.isAllOk())
		cout << r;


	ta.setValue(kFusionTidligerePensionsordning_MK, true);
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	//	if (!r.isAllOk())
		cout << r;
	EXPECT_EQ(1, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kFusionPensionsselskab, kProductElementRequired));

	ta.setValue(kFusionPensionsselskab, "ok garbage");
	r = re.validate(ta, false);
	//	if (!r.isAllOk())
		cout << r;
	EXPECT_TRUE(r.isAllOk());

}


