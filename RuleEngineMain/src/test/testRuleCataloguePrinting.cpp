#include "gtest/gtest.h"

#include <memory>

#include "../ruleengine/KonceptInfo_sbx.h"
#include "../ruleengine/Date_sbx.h"

#include "testRuleEngineInitialiser.cpp"

using namespace std;
using namespace sbx;

class RuleCataloguePrinting : public RuleEngineInitialiser {
protected:
    virtual void SetUp() {
    	RuleEngineInitialiser::SetUp();
    }
};

TEST_F(RuleCataloguePrinting, PrintRuleCatalogue) {
	RuleEngine::_printDebug = true;
	RuleEngine::_printDebugAtValidation = true;

	re.printRuleCatalogue();
}




