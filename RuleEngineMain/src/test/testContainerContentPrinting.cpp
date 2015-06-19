#include "gtest/gtest.h"

#include "../ruleengine/KonceptInfo_sbx.h"

#include "testRuleEngineInitialiser.cpp"

using namespace std;
using namespace sbx;

class ContainerContentPrinting : public RuleEngineInitialiser {
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

TEST_F(ContainerContentPrinting, PrintingConstants) {
	RuleEngine::_printDebug = true;
	RuleEngine::_printDebugAtValidation = true;

	re.getContainer().printParameters();
	re.getContainer().printParametersToProducts();
}






