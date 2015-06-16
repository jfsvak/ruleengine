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

class Boernerente_KI_OSV_25_49 : public ::testing::Test  {
protected:
    virtual void SetUp() {
    	RuleEngine::_printDebugAtValidation = false;
    	re = RuleEngine();
        re.initConstants(get_file_contents("basedata-ruleconstants.json"));
        re.initKoncepts(get_file_contents("koncepts.json"));
        re.parseRuleCatalogueJSON(get_file_contents("rule-catalogue.json"));

        KonceptInfo ki {4, 30, 0, // UnderkonceptOid:OSV 25-49
        	{ {11, "true"}, // Parameter-Basis
        	  {1, "true"}, // Solidarisk faellestarif
			  {15, "true"}, // FG span
			  {6, "true"} // SEB Firmapensionspulje
        	} };
        re.initContext(ki, OUTSIDE);
    }

    RuleEngine re;
};

TEST_F(Boernerente_KI_OSV_25_49, Boernerente_Pristal_POSITIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kBoernerente_Reguleringstype, "Pristal");
	ta.setValue(kBoerneUdloebsalder, (long) 24);
	ta.setValue(kBoerneRenteBlMin, (long) 5000);
	ta.setValue(kBoerneRenteBlMax, (long) 5000);
	ta.setValue(kBoerneRenteSoliMax, "Obligatorisk maks");

	auto r = re.validate(ta, false);

	EXPECT_TRUE(r.isAllOk());

//	if (!r.isAllOk())
		cout << r;
}

TEST_F(Boernerente_KI_OSV_25_49, Boernerente_Pristal_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kBoernerente_Reguleringstype, "Pristal");

	auto r = re.validate(ta, false);
//	if (!r.isAllOk())
		cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_EQ(3, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kBoerneRenteBlMin, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kBoerneRenteBlMax, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kBoerneRenteSpaendBl, kProductElementRequired));

	ta.setValue(kBoerneRenteBlMin, -1);
	ta.setValue(kBoerneRenteBlMax, 55000);
	r = re.validate(ta, {kBoernerente_Reguleringstype, kBoerneRenteBlMin, kBoerneRenteBlMax, kBoerneRenteSpaendBl});
//	if (!r.isAllOk())
		cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_EQ(3, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kBoerneRenteBlMin, kValueUnderLimit));
	EXPECT_TRUE(r.hasMessages(kBoerneRenteBlMax, kValueOverLimit));
	EXPECT_TRUE(r.hasMessages(kBoerneRenteSpaendBl, kValueOverLimit));

	ta.setValue(kBoerneRenteBlMin, 15000);
	ta.setValue(kBoerneRenteBlMax, 25000);
	r = re.validate(ta, {kBoernerente_Reguleringstype, kBoerneRenteBlMin, kBoerneRenteBlMax, kBoerneRenteSpaendBl} );
//	if (!r.isAllOk())
		cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_EQ(1, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kBoerneRenteSpaendBl, kValueOverLimit));

	ta.setValue(kBoerneRenteBlMax, 15000);
	r = re.validate(ta, false);
//	if (!r.isAllOk())
		cout << r;
	EXPECT_TRUE(r.isAllOk());
}

TEST_F(Boernerente_KI_OSV_25_49, Boernerente_Gage_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kBoernerente_Reguleringstype, "Gage");

	auto r = re.validate(ta, false);
//	if (!r.isAllOk())
		cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_EQ(3, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kBoerneRentePctMin, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kBoerneRentePctMax, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kBoerneRenteSpaendPct, kProductElementRequired));

	ta.setValue(kBoerneRentePctMin, 5);
	ta.setValue(kBoerneRentePctMax, 15);
	r = re.validate(ta, false);
//	if (!r.isAllOk())
		cout << r;
	EXPECT_TRUE(r.isAllOk());
}

