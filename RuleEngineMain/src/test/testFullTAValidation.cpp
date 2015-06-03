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

class Full_TA_CONTEXT_KI_OSV_25_50 : public ::testing::Test  {
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


TEST_F(Full_TA_CONTEXT_KI_OSV_25_50, Full_TA_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kUdlobsalder_Pension, (long) 67)
			.setValue(kFravalgRisiko_MK, true)
			.setValue(kTraditionel_MK, true)
			.setValue(kTidspensionMedGaranti_MK, true)
			.setValue(kLink_MK, true);

	RuleEngine::_printDebugAtValidation = true;

	auto r = re.validate(ta);
	EXPECT_FALSE(r.isAllOk());
	// total is 187, so should be 187 minus the number of pe's set above
	EXPECT_EQ(37, r.sizeValidationResults());
	cout << r;

	ta.setValue(kDoedReguleringskode, "Ingen"); // should make DoedPctGrMin, DoedPctOblMax, DoedSpaendPct notRequired and therefore lower the amount of missing pe's to 181-4=177
	r = re.validate(ta);
	EXPECT_EQ(34, r.sizeValidationResults());
//	if (r.sizeValidationResults() != 181)
		cout << r;
}

TEST_F(Full_TA_CONTEXT_KI_OSV_25_50, Full_TA_NUMBER_OF_VALIDATIONRESULTS) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV

	ValidationResults r {};
	RuleEngine::_printDebugAtValidation = true;

	r = re.validate(ta);
	EXPECT_FALSE(r.isAllOk());
	EXPECT_EQ(37, r.sizeValidationResults());

//	cout << r;
}
