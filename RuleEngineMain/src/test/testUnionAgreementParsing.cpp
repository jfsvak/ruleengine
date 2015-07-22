#include "gtest/gtest.h"

#include <memory>

#include "../ruleengine/KonceptInfo_sbx.h"
#include "../ruleengine/Date_sbx.h"

#include "testRuleEngineInitialiser.cpp"

using namespace std;
using namespace sbx;

class UnionAgreementParsing : public RuleEngineInitialiser {
protected:
    virtual void SetUp() {
    	RuleEngineInitialiser::SetUp();

        KonceptInfo ki {PROSA, 5, 0,
        	{ {11, "true"}, // Parameter-Basis
        	  {1, "true"}, // Solidarisk faellestarif
			  {15, "true"}, // FG span
			  {6, "true"} // SEB Firmapensionspulje
        	} };
        re.initContext(ki, INCLUDED, 6);
    }
};

TEST_F(UnionAgreementParsing, Parsing_UnionAgreements) {
	RuleEngine::_printDebug = true;
	RuleEngine::_printDebugAtValidation = true;

	re.getContainer().printUnionAgreements();
	Date d{"01.07.2010"};
	UnionAgreementContributionStep step = re.getContainer().getUAContributionStep((unsigned short) 3, d);
	cout << step;

}

TEST_F(UnionAgreementParsing, ReadingUAContributionSteps) {
	KonceptInfo ki {PROSA, 5, 0,
	        	{ {11, "true"}, // Parameter-Basis
	        	  {1, "true"}, // Solidarisk faellestarif
				  {15, "true"}, // FG span
				  {6, "true"} // SEB Firmapensionspulje
	        	} };
	re.initContext(ki, INCLUDED, 6);
	RuleEngine::_printDebug = true;
	RuleEngine::_printDebugAtValidation = true;

	re.getContainer().printUnionAgreements();
	Date d{"01.07.2010"};
	UnionAgreementContributionStep step = re.getContainer().getUAContributionStep((unsigned short) 3, d);
	cout << step;

}

TEST_F(UnionAgreementParsing, ReadingConstants_UnionAgreements) {
	KonceptInfo ki {PROSA, 5, 0,
	        	{ {11, "true"}, // Parameter-Basis
	        	  {1, "true"}, // Solidarisk faellestarif
				  {15, "true"}, // FG span
				  {6, "true"} // SEB Firmapensionspulje
	        	} };
	re.initContext(ki, INCLUDED, 6);
	RuleEngine::_printDebug = true;
	RuleEngine::_printDebugAtValidation = true;

	re.getContainer().printSubkonceptConstants(18);
	re.getContainer().printUnionAgreementConstants();
	re.getContainer().printConstantsFromContext(kUdbetalingsperiode);

}

TEST_F(UnionAgreementParsing, UAIncluded_Udbetalingsperiode) {
	KonceptInfo ki {PROSA, 5, 0,
	        	{ {11, "true"}, // Parameter-Basis
	        	  {1, "true"}, // Solidarisk faellestarif
				  {15, "true"}, // FG span
				  {6, "true"} // SEB Firmapensionspulje
	        	} };
	re.initContext(ki, OUTSIDE);
	RuleEngine::_printDebug = true;
	RuleEngine::_printDebugAtValidation = true;

	re.getContainer().printConstantsFromContext(kUdbetalingsperiode);

	TA ta {"20247940"};
	ta.setValue(kUdbetalingsperiode, 13);
	ta.setValue(kAftaleIkraftdato, 20150701);
	ta.setValue(kBidragEjFoesteTrin_MK, false);

	auto r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk());

	re.initContext(ki, INCLUDED, 6);

	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kUdbetalingsperiode, kValueNotAllowed));

	ta.setValue(kUdbetalingsperiode, 25);
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk());
}

TEST_F(UnionAgreementParsing, UAIncluded_DoedBlGrMin) {
	KonceptInfo ki {OSV, 50, 0,
	        	{ {11, "true"}, // Parameter-Basis
	        	  {1, "true"}, // Solidarisk faellestarif
				  {15, "true"}, // FG span
				  {6, "true"} // SEB Firmapensionspulje
	        	} };
	re.initContext(ki, INCLUDED, 4);
	RuleEngine::_printDebug = true;
	RuleEngine::_printDebugAtValidation = true;

	re.getContainer().printKoncepts();
	re.getContainer().printSubkonceptConstants(18);
	re.getContainer().printUnionAgreementConstants(4);

	TA ta {"20247940"};
	ta.setValue(kDoedReguleringskode, "Ingen");
	ta.setValue(kDoedBlGrMin, 50000);
	ta.setValue(kDoedBlOblMax, 100000);
	ta.setValue(kAftaleIkraftdato, 20150701);
	ta.setValue(kBidragEjFoesteTrin_MK, false);

	auto r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kDoedBlGrMin, kValueUnderLimit));

	ta.setValue(kDoedBlGrMin, 100000);
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk());
}

TEST_F(UnionAgreementParsing, UAIncluded_MaxAndelLinkBidragPCT) {
	KonceptInfo ki {OSV, 50, 0,
	        	{ {11, "true"}, // Parameter-Basis
	        	  {1, "true"}, // Solidarisk faellestarif
				  {15, "true"}, // FG span
				  {6, "true"} // SEB Firmapensionspulje
	        	} };
	re.initContext(ki, INCLUDED, 4);
	RuleEngine::_printDebug = true;
	RuleEngine::_printDebugAtValidation = true;

	re.getContainer().printKoncepts();
	re.getContainer().printSubkonceptConstants(18);
	re.getContainer().printUnionAgreementConstants(4);

	TA ta {"20247940"};
	ta.setValue(kMaxAndelLinkBidragPCT, 35);
	ta.setValue(kAftaleIkraftdato, 20150701);
	ta.setValue(kBidragEjFoesteTrin_MK, false);

	auto r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kMaxAndelLinkBidragPCT, kValueOverLimit));

	ta.setValue(kMaxAndelLinkBidragPCT, 30);
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk());
}







