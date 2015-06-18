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

class EksisterendeOrdninger_KI_OSV_25_49 : public RuleEngineInitialiser {
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

TEST_F(EksisterendeOrdninger_KI_OSV_25_49, EksisterendeOrdninger_POSITIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kEksisterendeOrdning_MK, false);

	auto r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;

	ta.setValue(kEksisterendeOrdning_MK, true);
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	//	if (!r.isAllOk())
		cout << r;
	EXPECT_EQ(4, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kPensionsudbyder, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kIndtraedelseSenere_MK, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kOblDaekningSammeStoerrelse_MK, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kHelbred_leverandorskift, kProductElementRequired));


	ta.setValue(kPensionsudbyder, "ok garbage");
	ta.setValue(kIndtraedelseSenere_MK, false);
	ta.setValue(kOblDaekningSammeStoerrelse_MK, false);
	ta.setValue(kHelbred_leverandorskift, re.getDefaultValue(kHelbred_leverandorskift)->stringValue());
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	//	if (!r.isAllOk())
		cout << r;


	ta.setValue(kIndtraedelseSenere_MK, true);
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	//	if (!r.isAllOk())
		cout << r;
	EXPECT_EQ(1, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kIndtraedelsesdato, kProductElementRequired));

	ta.setValue(kIndtraedelsesdato, (long) 20150701);
	re.printVariablesInParser();
	r = re.validate(ta, false);
	//	if (!r.isAllOk())
		cout << r;
	EXPECT_TRUE(r.isAllOk());

}

TEST_F(EksisterendeOrdninger_KI_OSV_25_49, Helbred_Leverandorskift_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kEksisterendeOrdning_MK, false);
	ta.setValue(kFusion_MK, false);
	ta.setValue(kHelbred_leverandorskift, "FP7");

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;

	EXPECT_TRUE(r.hasMessages(kHelbred_leverandorskift, kValueNotAllowed));

}


