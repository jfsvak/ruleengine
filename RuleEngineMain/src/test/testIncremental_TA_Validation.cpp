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

class Incremental_TA_CONTEXT_KI_OSV_25_50 : public ::testing::Test  {
protected:
    virtual void SetUp() {
        re = RuleEngine();
        re.initConstants(get_file_contents("basedata-ruleconstants.json"));
        re.parseRuleCatalogueJSON(get_file_contents("rule-catalogue.json"));

        KonceptInfo ki {17, // UnderkonceptOid:OSV 25-49
        	{ {  1, "true" }, // Solidarisk faellestarif
        	  {  6, "true" }, // SEB Firmapensionspulje
        	  { 11, "true" }, // Parameter-Basis
			  { 15, "true" } // FG Span
        	} };
        re.initContext(ki);
    }

    RuleEngine re;
};


TEST_F(Incremental_TA_CONTEXT_KI_OSV_25_50, Incremental_TA_POSITIVE) {
	RuleEngine::_printDebugAtValidation = true;

	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kUdlobsalder_Pension, (long) 67);

	auto r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk());

	ta.setValue(kFravalgRisiko_MK, true);
	r = re.validate(ta, false);
	cout << r;

//	EXPECT_FALSE(r.isAllOk());
//	EXPECT_EQ(1, r.getValidationResults(kFravalgRisikoAlder).size());
//	EXPECT_EQ(sbx::ValidationCode::kProductElementRequired, r.getValidationResults(kFravalgRisikoAlder).at(0).getValidationCode());
}
