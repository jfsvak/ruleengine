#include <string>
#include <vector>
#include "gtest/gtest.h"

#include "../ruleengine/KonceptInfo_sbx.h"
#include "../ruleengine/RuleEngine_sbx.h"
#include "../ruleengine/TA_sbx.h"
#include "../ruleengine/ValidationResult.h"
#include "../ruleengine/ValidationResults.h"

#include "ruleenginetestutils.h"
#include "testRuleEngineInitialiser.cpp"

using namespace std;
using namespace sbx;

class FravalgRisiko_CONTEXT_KI_OSV_25_50 : public RuleEngineInitialiser {
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

// Test
// PE:
//   FravalgRisiko_MK - PE(230)-P(129)
// Allowed values: true | false
TEST_F(FravalgRisiko_CONTEXT_KI_OSV_25_50, FravalgRisiko_MK_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kFravalgRisiko_MK, true);

	RuleEngine::_printDebugAtValidation = true;

	auto r = re.validate(ta, (unsigned short) kFravalgRisiko_MK);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kFravalgRisiko_MK, kValueNotAllowed));

	ta.setValue(kFravalgRisiko_MK, false);
	r = re.validate(ta, (unsigned short) kFravalgRisiko_MK);
	EXPECT_TRUE(r.isAllOk());
}

// Test
// PE:
//   FravalgRisiko_MK - PE(230)-P(129)
//   FravalgRisikoAlder - PE(231)-P(129)
// Allowed values:
//   FravalgRisikoAlder between 40-67
TEST_F(FravalgRisiko_CONTEXT_KI_OSV_25_50, FravalgRisikoAlder_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kFravalgRisiko_MK, true);
	ta.setValue(kFravalgRisikoAlder, (long) 40); // allowed

	auto r = re.validate(ta, (unsigned short) kFravalgRisikoAlder);
	EXPECT_TRUE(r.isAllOk());

	ta.setValue(kFravalgRisikoAlder, (long) 50); // not allowed
	r = re.validate(ta, (unsigned short) kFravalgRisikoAlder);
	EXPECT_TRUE(r.isAllOk());

	ta.setValue(kFravalgRisikoAlder, (long) 67); // not allowed
	r = re.validate(ta, (unsigned short) kFravalgRisikoAlder);
	EXPECT_TRUE(r.isAllOk());
}

// Test
// PE:
//   FravalgRisiko_MK - PE(230)-P(129)
//   FravalgRisikoAlder - PE(231)-P(129)
// Allowed values:
//   FravalgRisikoAlder between 40-67
TEST_F(FravalgRisiko_CONTEXT_KI_OSV_25_50, FravalgRisikoAlder_NEGATIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kFravalgRisiko_MK, true);

	ta.setValue(kFravalgRisikoAlder, (long) 1);

	auto r = re.validate(ta, (unsigned short) kFravalgRisikoAlder);
	EXPECT_FALSE(r.isAllOk());

	ta.setValue(kFravalgRisikoAlder, (long) 39);
	r = re.validate(ta, (unsigned short) kFravalgRisikoAlder);
	EXPECT_FALSE(r.isAllOk());

	ta.setValue(kFravalgRisikoAlder, (long) 68);
	r = re.validate(ta, (unsigned short) kFravalgRisikoAlder);
	EXPECT_FALSE(r.isAllOk());

	ta.setValue(kFravalgRisikoAlder, (long) 10000);
	r = re.validate(ta, (unsigned short) kFravalgRisikoAlder);
	EXPECT_FALSE(r.isAllOk());
}

// Test
// PE:
//   FravalgRisiko_MK - PE(230)-P(129)
//   FravalgRisikoAlder - PE(231)-P(129)
// Allowed values:
//   FravalgRisikoAlder not allowed as FravalgRisiko_MK is false
TEST_F(FravalgRisiko_CONTEXT_KI_OSV_25_50, FravalgRisikoAlder_FravalgRisiko_MK_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kFravalgRisiko_MK, false);
	ta.setValue(kFravalgRisikoAlder, (long) 60);

	auto r = re.validate(ta, (unsigned short) kFravalgRisikoAlder);
	EXPECT_FALSE(r.isAllOk());
	if (r.isAllOk())
		cout << r;

	auto v = r.getValidationResults(kFravalgRisikoAlder);
	ASSERT_EQ(1, v.size());

	EXPECT_EQ(kProductElementNotAllowed, v.at(0).getValidationCode());

	// Set FravalgRisiko_MK to true and now the FravalgRisikoAlder is allowed and the value is OK
	ta.setValue(kFravalgRisiko_MK, true);
	r = re.validate(ta, (unsigned short) kFravalgRisikoAlder);
	EXPECT_TRUE(r.isAllOk());

	if (!r.isAllOk())
		cout << r;
}

TEST_F(FravalgRisiko_CONTEXT_KI_OSV_25_50, FravalgRisikoAlder_TooBig_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kUdlobsalder_Pension, (long) 65);
	ta.setValue(kFravalgRisiko_MK, true);
	ta.setValue(kFravalgRisikoAlder, (long) 66);

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_TRUE(r.hasMessages(kFravalgRisikoAlder, kValueOverLimit));

	ta.setValue(kUdlobsalder_Pension, (long) 67);

	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_TRUE(r.hasMessages(kFravalgRisiko_MK, kValueNotAllowed));
	EXPECT_FALSE(r.hasMessages(kFravalgRisikoAlder));

	ta.setValue(kUdlobsalder_Pension, (long) 65);
	ta.setValue(kFravalgRisikoAlder, (long) 60);

	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_TRUE(r.hasMessages(kFravalgRisiko_MK, kValueNotAllowed));
	EXPECT_FALSE(r.hasMessages(kFravalgRisikoAlder));
}

