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

class LatestSignDate_CONTEXT_KI_OSV_25_50 : public ::testing::Test  {
protected:
    virtual void SetUp() {
        re = RuleEngine();
        re.initConstants(get_file_contents("basedata-ruleconstants.json"));
        re.parseRuleCatalogueJSON(get_file_contents("rule-catalogue.json"));

        KonceptInfo ki {17, // UnderkonceptOid:OSV 25-49
        	{ { 1, "true"}, // Solidarisk faellestarif
			  { 6, "true"}, // SEB Firmapensionspulje
        	  {11, "true"}, // Parameter-Basis
			  {15, "true"}, // FG-Spaend
        	} };

        re.initContext(ki);
    }

    RuleEngine re;
};


// Test kIndmeldelsesalder and getting options prior to that
TEST_F(LatestSignDate_CONTEXT_KI_OSV_25_50, LatestSignDate_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV

	ta.setValue(kSeneste_Underskriftsdato, 20150601);

	auto r = re.validate(ta, (unsigned short) kIndmeldelsesAlder);
	if (!r.isAllOk()) cout << r;
}
