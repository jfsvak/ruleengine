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

using namespace std;
using namespace sbx;

class KritiskSygdom_I_FG_CONTEXT_KI_OSV_25_50_NO_FG_SPAN : public ::testing::Test  {
protected:
    virtual void SetUp() {
        re = RuleEngine();
        re.initConstants(get_file_contents("basedata-ruleconstants.json"));
        re.parseRuleCatalogueJSON(get_file_contents("rule-catalogue.json"));

        KonceptInfo ki {17, // UnderkonceptOid:OSV 25-49
        	{ {11, "true"}, // Parameter-Basis
        	  {1, "true"}, // Solidarisk faellestarif
		//    {15, "true"}, // No FG_SPAN parameter set
			  {6, "true"} // SEB Firmapensionspulje
        	} };
        re.initContext(ki);
    }

    RuleEngine re;
};


TEST_F(KritiskSygdom_I_FG_CONTEXT_KI_OSV_25_50_NO_FG_SPAN, KritiskSygdom_I_FG_Missing_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV

	auto r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk());

	ASSERT_EQ(0, r.getValidationResults(kKritiskSygdom_i_FG_mk).size());
}

TEST_F(KritiskSygdom_I_FG_CONTEXT_KI_OSV_25_50_NO_FG_SPAN, KritiskSygdom_I_FG_True_NEGATIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kKritiskSygdom_i_FG_mk, true);

	auto r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());

	ASSERT_EQ(1, r.getValidationResults(kKritiskSygdom_i_FG_mk).size());
	auto v = r.getValidationResults(kKritiskSygdom_i_FG_mk);
	EXPECT_EQ(sbx::ValidationCode::kProductElementNotAllowed, v.at(0).getValidationCode());
}


TEST_F(KritiskSygdom_I_FG_CONTEXT_KI_OSV_25_50_NO_FG_SPAN, KritiskSygdom_I_FG_False_NEGATIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kKritiskSygdom_i_FG_mk, false); // value not allowed

	RuleEngine::_printDebugAtValidation = true;

	auto r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());

	ASSERT_EQ(1, r.getValidationResults(kKritiskSygdom_i_FG_mk).size());
	auto v = r.getValidationResults(kKritiskSygdom_i_FG_mk);
	EXPECT_EQ(sbx::ValidationCode::kProductElementNotAllowed, v.at(0).getValidationCode());
}
