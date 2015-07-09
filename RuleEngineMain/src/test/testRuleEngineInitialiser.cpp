#include "gtest/gtest.h"

#include "ruleenginetestutils.h"

#include "../ruleengine/RuleEngine_sbx.h"

using namespace std;
using namespace sbx;

class RuleEngineInitialiser : public ::testing::Test  {
protected:
    void SetUp() {
    	RuleEngine::_printDebugAtValidation = false;
    	RuleEngine::_printDebug = false;
    	RuleConstantContainer::_printDebug = false;

        re.initConstants(get_file_contents("basedata-all.json"));
        re.initKoncepts(get_file_contents("basedata-all.json"));
        re.initUnionAgreements(get_file_contents("basedata-all.json"));
        re.parseRuleCatalogueJSON(get_file_contents("rule-catalogue.json"));
    }

    RuleEngine re;
    KonceptInfo _ki;
};







