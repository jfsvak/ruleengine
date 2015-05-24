#include <string>
#include <vector>
#include "gtest/gtest.h"

#include "ruleenginetestutils.h"

#include "../ruleengine/KonceptInfo_sbx.h"
#include "../ruleengine/RuleEngine_sbx.h"
#include "../ruleengine/TA_sbx.h"
#include "../ruleengine/ValidationResult.h"
#include "../ruleengine/ValidationResults.h"


using namespace std;
using namespace sbx;

class RuleEngine_CONTEXT_KI_OSV_25_50 : public ::testing::Test  {
protected:
    virtual void SetUp() {
        re = RuleEngine();
        re.initConstants(get_file_contents("basedata-ruleconstants.json"));
        re.parseRuleCatalogueJSON(get_file_contents("rule-catalogue.json"));

        KonceptInfo ki {17, // UnderkonceptOid:OSV 25-49
        	{ {11, "true"}, // Parameter-Basis
        	  {1, "true"}, // Solidarisk faellestarif
			  {6, "true"} // SEB Firmapensionspulje
        	} };
        re.initContext(ki);
    }

    RuleEngine re;
};

// Test
// PE:
//   FravalgRisiko_MK - PE(230)-P(129)
// Allowed values: true | false
TEST_F(RuleEngine_CONTEXT_KI_OSV_25_50, FravalgRisiko_MK_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kFravalgRisiko_MK, true);

	RuleEngine::_printDebugAtValidation = true;

	auto r = re.validate(ta, (unsigned short) kFravalgRisiko_MK);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

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
TEST_F(RuleEngine_CONTEXT_KI_OSV_25_50, FravalgRisikoAlder_POSITIVE) {
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
TEST_F(RuleEngine_CONTEXT_KI_OSV_25_50, FravalgRisikoAlder_NEGATIVE) {
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
TEST_F(RuleEngine_CONTEXT_KI_OSV_25_50, FravalgRisikoAlder_FravalgRisiko_MK_NEGATIVE) {
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
