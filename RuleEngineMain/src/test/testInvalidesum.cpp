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

class Invalidesum_KI_OSV_25_49 : public RuleEngineInitialiser {
protected:
    virtual void SetUp() {
    	RuleEngineInitialiser::SetUp();

        KonceptInfo ki {4, 30, 0, // UnderkonceptOid:OSV 25-49
        	{ {11, "true"}, // Parameter-Basis
        	  {1, "true"}, // Solidarisk faellestarif
			  {6, "true"} // SEB Firmapensionspulje
        	} };
        re.initContext(ki, OUTSIDE);
    }
};

TEST_F(Invalidesum_KI_OSV_25_49, Invalidesum_I_Kr_Ingen_POSITIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
	ta.setValue(kInvalidesumReguleringskode, "Ingen");

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;
	EXPECT_EQ(3, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kInvalidesumBlMin, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kInvalidesumBlMax, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kInvalidesumSpaendBl, kProductElementRequired));

	ta.setValue(kInvalidesumBlMin, 20000);
	ta.setValue(kInvalidesumBlMax, 20000); // now set amounts and it should be happy
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk()); //

}

TEST_F(Invalidesum_KI_OSV_25_49, Invalidesum_I_Kr_Ingen_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
	ta.setValue(kInvalidesumReguleringskode, "Ingen");
	ta.setValue(kInvalidesumBlMin, -1);
	ta.setValue(kInvalidesumBlMax, 1300000);

	auto r = re.validate(ta, {kInvalidesumReguleringskode, kInvalidesumBlMin, kInvalidesumBlMax, kInvalidesumSpaendBl});
	EXPECT_FALSE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;
	EXPECT_EQ(3, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kInvalidesumBlMin, kValueUnderLimit));
	EXPECT_TRUE(r.hasMessages(kInvalidesumSpaendBl, kValueOverLimit));
	EXPECT_TRUE(r.hasMessages(kInvalidesumBlMax, kValueOverLimit));

	ta.setValue(kInvalidesumBlMin, 100000);
	ta.setValue(kInvalidesumBlMax, 200000); // now set allowed values, but with span (span==10000 > 0)
	r = re.validate(ta, {kInvalidesumReguleringskode, kInvalidesumBlMin, kInvalidesumBlMax, kInvalidesumSpaendBl});
	cout << r;
	EXPECT_FALSE(r.isAllOk()); //
	EXPECT_EQ(1, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kInvalidesumSpaendBl, kValueOverLimit));

}


TEST_F(Invalidesum_KI_OSV_25_49, Invalidesum_I_Pct_POSITIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
	ta.setValue(kInvalidesumReguleringskode, "Gage");

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;
	EXPECT_EQ(3, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kInvalidesumPctMin, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kInvalidesumPctMax, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kInvalidesumSpaendPct, kProductElementRequired));

	ta.setValue(kInvalidesumPctMin, 40);
	ta.setValue(kInvalidesumPctMax, 40); // now set amounts and it should be happy
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk()); //

}

TEST_F(Invalidesum_KI_OSV_25_49, Invalidesum_I_Pct_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
	ta.setValue(kInvalidesumReguleringskode, "Gage");
	ta.setValue(kInvalidesumPctMin, -1);
	ta.setValue(kInvalidesumPctMax, 101);

	auto r = re.validate(ta, {kInvalidesumReguleringskode, kInvalidesumPctMin, kInvalidesumPctMax, kInvalidesumSpaendPct});
	EXPECT_FALSE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;
	EXPECT_EQ(3, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kInvalidesumPctMin, kValueUnderLimit));
	EXPECT_TRUE(r.hasMessages(kInvalidesumPctMax, kValueOverLimit));

	ta.setValue(kInvalidesumPctMin, 10);
	ta.setValue(kInvalidesumPctMax, 50); // now set allowed values, but with span (span==40 > 0)
	r = re.validate(ta, {kInvalidesumReguleringskode, kInvalidesumPctMin, kInvalidesumPctMin, kInvalidesumSpaendPct});
	cout << r;
	EXPECT_FALSE(r.isAllOk()); //
	EXPECT_EQ(1, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kInvalidesumSpaendPct, kValueOverLimit));

	ta.setValue(kInvalidesumPctMin, 50); // now set allowed values, but with span (span==10000 > 0)
	r = re.validate(ta, {kInvalidesumReguleringskode, kInvalidesumPctMin, kInvalidesumPctMin, kInvalidesumSpaendPct});
	cout << r;
	EXPECT_TRUE(r.isAllOk()); //

}

TEST_F(Invalidesum_KI_OSV_25_49, InvalideSumPctMin_OverLimit) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kInvalidesumReguleringskode, "Gage");
	ta.setValue(kInvalidesumPctMin, 101);
	ta.setValue(kInvalidesumPctMax, 101);

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_TRUE(r.hasMessages(kInvalidesumPctMin, kValueOverLimit));

	ta.setValue(kInvalidesumPctMin, 60);
	ta.setValue(kInvalidesumPctMax, 60);
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

}

TEST_F(Invalidesum_KI_OSV_25_49, InvalideSumBlMin_OverLimit) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kInvalidesumReguleringskode, "Pristal");
	ta.setValue(kInvalidesumBlMin, 1215001);
	ta.setValue(kInvalidesumBlMax, 1500000);

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
		cout << r;
	EXPECT_TRUE(r.hasMessages(kInvalidesumBlMin, kValueOverLimit));

	ta.setValue(kInvalidesumBlMin, 1215000);
	ta.setValue(kInvalidesumBlMax, 1215000);
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

	ta.setValue(kInvalidesumBlMin, 1214999);
	ta.setValue(kInvalidesumBlMax, 1214999);
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;
}




