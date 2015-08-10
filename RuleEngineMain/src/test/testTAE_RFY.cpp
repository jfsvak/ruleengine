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

class TAE_RFY_KI_OSV_25_49 : public RuleEngineInitialiser {
protected:
    virtual void SetUp() {
    	RuleEngineInitialiser::SetUp();

		KonceptInfo ki {4, 30, 0, // UnderkonceptOid:OSV 25-49
			{ {11, "true"}, // Parameter-Basis
			  {1, "true"}, // Solidarisk faellestarif
			  {6, "true"}, // SEB Firmapensionspulje
			  {29, "0"} // Insurance Condition Version, 0 == all versions
			} };
		re.initContext(ki, OUTSIDE);
    }
};

TEST_F(TAE_RFY_KI_OSV_25_49, InsuranceCondition_1) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040" };
	ta.setValue(kInsuranceConditionVersion, "1");
	ta.setValue(kTAE_RFY, re.getDefaultValue(kTAE_RFY)->stringValue());

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;

	r.hasMessages(kTAE_RFY, ValidationCode::kValueNotAllowed);

	ta.setValue(kTAE_RFY, "Ingen");
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk()); // Ingen is allowed for insurance condition version 1
	cout << r;
}

TEST_F(TAE_RFY_KI_OSV_25_49, InsuranceCondition_2) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040" };
	ta.setValue(kInsuranceConditionVersion, "2");
	ta.setValue(kTAE_RFY, re.getDefaultValue(kTAE_RFY)->stringValue());

	auto r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk()); // Ingen is allowed for insurance condition version 1
}

