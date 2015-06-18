#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <locale>
#include <ctime>
#include <iomanip>

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

class LatestSignDate_CONTEXT_KI_OSV_25_50 : public RuleEngineInitialiser {
protected:
    virtual void SetUp() {
    	RuleEngineInitialiser::SetUp();

        KonceptInfo ki {4, 30, 0, // UnderkonceptOid:OSV 25-49
        	{ { 1, "true"}, // Solidarisk faellestarif
			  { 6, "true"}, // SEB Firmapensionspulje
        	  {11, "true"}, // Parameter-Basis
			  {15, "true"}, // FG-Spaend
        	} };

        re.initContext(ki, OUTSIDE);
    }
};


// Test kIndmeldelsesalder and getting options prior to that
TEST_F(LatestSignDate_CONTEXT_KI_OSV_25_50, LatestSignDate_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kSeneste_Underskriftsdato, 20150601);

	auto r = re.validate(ta, (unsigned short) kIndmeldelsesAlder);
	if (!r.isAllOk()) cout << r;



}

TEST_F(LatestSignDate_CONTEXT_KI_OSV_25_50, DateParsing) {
    std::istringstream is("01.07.2015");
    char delimiter = '.';
    int d, m, y;
    struct tm *time;

    if (is >> d >> delimiter >> m >> delimiter >> y) {
        struct tm t = {0};
        t.tm_mday = d;
        t.tm_mon = m - 1;
        t.tm_year = y - 1900;
        t.tm_isdst = -1;

        // normalize:
        time_t when = mktime(&t);
        time = localtime(&when);
    }
//    std::wstring input = L"2011-Februar-18 23:12:34";
//    std::tm t;
//	std::wistringstream ss(input);
//	ss.imbue(std::locale("de_DE"));
//	ss >> std::get_time(&t, L"%Y-%b-%d %H:%M:%S"); // uses std::time_get<wchar_t>
	cout << std::asctime(time);
//	cout << std::put_time();

}
