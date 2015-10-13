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

class FagspecificMaxEnrollmentAge : public RuleEngineInitialiser {
protected:
    virtual void SetUp() {
    	RuleEngineInitialiser::SetUp();
    }
};

TEST_F(FagspecificMaxEnrollmentAge, Advokatpakken_3_9) {
	RuleEngine::_printDebugAtValidation = true;
	KonceptInfo ki {ADVOKATPAKKEN, 5, 0,
			{ {11, "true"}, // Parameter-Basis
			  {1, "true"}, // Solidarisk faellestarif
			  {6, "true"}, // SEB Firmapensionspulje
			  {29, "1"} // Insurance Condition Version, 1 == old version
		    }};
	re.initContext(ki, OUTSIDE);

	TA ta { "15124040" };
	ta.setValue(kFagspec_mk, 0);
	ta.setValue(kFagspecificMaxAgeEnrolled_mk, 0);
	auto r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kFagspecificMaxAgeEnrolled_mk, kProductElementNotAllowed));

	ta.setValue(kFagspec_mk, 0);
	ta.setValue(kFagspecificMaxAgeEnrolled_mk, 1);
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kFagspecificMaxAgeEnrolled_mk, kProductElementNotAllowed));

	ta.setValue(kFagspec_mk, 0);
	ta.remove(kFagspecificMaxAgeEnrolled_mk);
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk());

	ta.setValue(kFagspec_mk, 1);
	ta.setValue(kFagspecificMaxAgeEnrolled_mk, 0);
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kFagspecificMaxAgeEnrolled_mk, kValueNotAllowed));

	ta.setValue(kFagspec_mk, 1);
	ta.setValue(kFagspecificMaxAgeEnrolled_mk, 1);
	r = re.validate(ta, true);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kFagspecificMaxAgeEnrolledAge, kProductElementRequired));

	ta.setValue(kFagspec_mk, 1);
	ta.setValue(kFagspecificMaxAgeEnrolled_mk, 1);
	ta.setValue(kFagspecificMaxAgeEnrolledAge, 59);
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kFagspecificMaxAgeEnrolledAge, kValueNotAllowed));

	ta.setValue(kFagspec_mk, 1);
	ta.setValue(kFagspecificMaxAgeEnrolled_mk, 1);
	ta.setValue(kFagspecificMaxAgeEnrolledAge, 60);
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk());
}

TEST_F(FagspecificMaxEnrollmentAge, OSV_24_49) {
	RuleEngine::_printDebugAtValidation = true;
	KonceptInfo ki {OSV, 40, 0,
			{ {11, "true"}, // Parameter-Basis
			  {1, "true"}, // Solidarisk faellestarif
			  {6, "true"}, // SEB Firmapensionspulje
			  {29, "1"} // Insurance Condition Version, 1 == old version
		    }};
	re.initContext(ki, OUTSIDE);

	TA ta { "15124040" };
	ta.setValue(kFagspec_mk, 1);
	ta.setValue(kFagspecificMaxAgeEnrolled_mk, 0);
	auto r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kFagspec_mk, kValueNotAllowed));

	ta.setValue(kFagspec_mk, 1);
	ta.setValue(kFagspecificMaxAgeEnrolled_mk, 1);
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kFagspec_mk, kValueNotAllowed));
	EXPECT_TRUE(r.hasMessages(kFagspecificMaxAgeEnrolled_mk, kValueNotAllowed));

	ta.setValue(kFagspec_mk, 1);
	ta.remove(kFagspecificMaxAgeEnrolled_mk);
	r = re.validate(ta, true);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kFagspec_mk, kValueNotAllowed));
	EXPECT_TRUE(r.hasMessages(kFagspecificMaxAgeEnrolled_mk, kProductElementRequired));


	ta.setValue(kFagspec_mk, 0);
	ta.setValue(kFagspecificMaxAgeEnrolled_mk, 0);
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kFagspecificMaxAgeEnrolled_mk, kProductElementNotAllowed));


	ta.setValue(kFagspec_mk, 1);
	ta.setValue(kFagspecificMaxAgeEnrolled_mk, 1);
	r = re.validate(ta, true);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kFagspec_mk, kValueNotAllowed));
	EXPECT_TRUE(r.hasMessages(kFagspecificMaxAgeEnrolledAge, kProductElementRequired));


	ta.setValue(kFagspec_mk, 1);
	ta.setValue(kFagspecificMaxAgeEnrolled_mk, 1);
	ta.setValue(kFagspecificMaxAgeEnrolledAge, 59);
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kFagspec_mk, kValueNotAllowed));
	EXPECT_TRUE(r.hasMessages(kFagspecificMaxAgeEnrolledAge, kValueNotAllowed));


	ta.setValue(kFagspec_mk, 1);
	ta.setValue(kFagspecificMaxAgeEnrolled_mk, 1);
	ta.setValue(kFagspecificMaxAgeEnrolledAge, 60);
	r = re.validate(ta, false);
	re.printConstantsInParser();
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kFagspec_mk, kValueNotAllowed));
	EXPECT_TRUE(r.hasMessages(kFagspecificMaxAgeEnrolled_mk, kValueNotAllowed));
//	EXPECT_TRUE(r.hasMessages(kFagspecificMaxAgeEnrolledAge, kValueNotAllowed)); // this should fail, but due to no enum limitation list in rule constants, a min/max validation is done for range 0-2147483647
}


