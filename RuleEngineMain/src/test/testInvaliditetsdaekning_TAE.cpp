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

class Invaliditetsdaekning_KI_OSV_25_49 : public ::testing::Test  {
protected:
    virtual void SetUp() {
    	RuleEngine::_printDebugAtValidation = false;
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

TEST_F(Invaliditetsdaekning_KI_OSV_25_49, Invaliditetsdaekning_I_Kr_Ingen) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kTAEReguleringskode, "Ingen");

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;
	EXPECT_EQ(2, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kTAEBlGrMin, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kTAEBlOblMax, kProductElementRequired));

	ta.setValue(kTAEBlGrMin, 100000);
	ta.setValue(kTAEBlOblMax, 200000); // now set amounts and it should be happy
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk()); //

}

TEST_F(Invaliditetsdaekning_KI_OSV_25_49, Invaliditetsdaekning_I_Kr_Ingen_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kTAEReguleringskode, "Ingen");

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;
	EXPECT_EQ(2, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kTAEBlGrMin, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kTAEBlOblMax, kProductElementRequired));

	ta.setValue(kTAEBlGrMin, 200000); // the span is too big, so should complain
	ta.setValue(kTAEBlOblMax, 900000); // now set max amounts to more than allowed
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk()); //

	EXPECT_EQ(3, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kTAEBlOblMax, kValueOverLimit));
}


TEST_F(Invaliditetsdaekning_KI_OSV_25_49, Invaliditetsdaekning_I_Kr_Pristal) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kTAEReguleringskode, "Pristal");

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;
	EXPECT_EQ(2, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kTAEBlGrMin, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kTAEBlOblMax, kProductElementRequired));

	ta.setValue(kTAEBlGrMin, 100000);
	ta.setValue(kTAEBlOblMax, 200000); // now set amounts and it should be happy
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk()); //

}

TEST_F(Invaliditetsdaekning_KI_OSV_25_49, Invaliditetsdaekning_I_Kr_Pristal_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kTAEReguleringskode, "Pristal");

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;
	EXPECT_EQ(2, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kTAEBlGrMin, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kTAEBlOblMax, kProductElementRequired));

	ta.setValue(kTAEBlGrMin, 200000);
	ta.setValue(kTAEBlOblMax, 900000); // now set amounts and it should be happy
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk()); //

	EXPECT_EQ(3, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kTAEBlOblMax, kValueOverLimit));

}

TEST_F(Invaliditetsdaekning_KI_OSV_25_49, Invaliditetsdaekning_I_PCT_Gage) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kTAEReguleringskode, "Gage");

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;
	EXPECT_EQ(3, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kTAEPctGrMin, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kTAEPctOblMax, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kTAEBlGrMin, kProductElementRequired));

	ta.setValue(kTAEPctGrMin, 40);
	ta.setValue(kTAEPctOblMax, 50); // now set pct and it should be only complain about missing amount
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk()); //
	EXPECT_EQ(1, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kTAEBlGrMin, kProductElementRequired));

	ta.setValue(kTAEBlGrMin, 100000); // now set the min amount and execpt it to be ok
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk()); // expect to find TAEBlGrMin
}

TEST_F(Invaliditetsdaekning_KI_OSV_25_49, Invaliditetsdaekning_I_PCT_Gage_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kTAEReguleringskode, "Gage");
	ta.setValue(kTAEPctGrMin, 10); // 20 is min
	ta.setValue(kTAEPctOblMax, 90); // 80 is max

	auto r = re.validate(ta, { kTAEReguleringskode, kTAEPctGrMin, kTAEPctOblMax, kTAESpaendPct});
	EXPECT_FALSE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;
	EXPECT_EQ(5, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kTAEPctGrMin, kValueUnderLimit));
	EXPECT_TRUE(r.hasMessages(kTAEPctOblMax, kValueOverLimit));
	EXPECT_TRUE(r.hasMessages(kTAESpaendPct, kValueOverLimit));

	ta.setValue(kTAEPctGrMin, 60);
	ta.setValue(kTAEPctOblMax, 70); // now set allowed pct and it should be only complain about missing amount
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk()); // expect to find TAEBlGrMin message
	EXPECT_EQ(1, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kTAEBlGrMin, kProductElementRequired));

	ta.setValue(kTAEBlGrMin, 100000); // now set the min amount and execpt it to be ok
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk()); // expect to find TAEBlGrMin
}

TEST_F(Invaliditetsdaekning_KI_OSV_25_49, Invaliditetsdaekning_I_PCT_Gage_NEGATIVE_Partial) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kTAEReguleringskode, "Gage");
	ta.setValue(kTAEPctGrMin, 10); // 20 is min
	ta.setValue(kTAEPctOblMax, 90); // 80 is max

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;
	EXPECT_EQ(5, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kTAEPctGrMin, kValueUnderLimit));
	EXPECT_TRUE(r.hasMessages(kTAEPctOblMax, kValueOverLimit));
	EXPECT_TRUE(r.hasMessages(kTAEBlGrMin, kProductElementRequired));

	ta.setValue(kTAEPctGrMin, 60);
	ta.setValue(kTAEPctOblMax, 70); // now set vallowed pct and it should be happy
	ta.setValue(kTAEBlGrMin, 200000); // now set vallowed pct and it should be happy
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk()); //
}

TEST_F(Invaliditetsdaekning_KI_OSV_25_49, Invaliditetsdaekning_I_PCT_Ingen_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kTAEReguleringskode, "Ingen");
	ta.setValue(kTAEBlGrMin, -1); // 0 is min
	ta.setValue(kTAEBlOblMax, 800001); // 800000 is max

	auto r = re.validate(ta, { kTAEReguleringskode, kTAEBlGrMin, kTAEBlOblMax, kTAESpaendBl});
	EXPECT_FALSE(r.isAllOk());
//	if (!r.isAllOk())
		cout << r;
	EXPECT_EQ(5, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kTAEBlGrMin, kValueUnderLimit));
	EXPECT_TRUE(r.hasMessages(kTAEBlOblMax, kValueOverLimit));
	EXPECT_TRUE(r.hasMessages(kTAESpaendBl, kValueOverLimit));

	ta.setValue(kTAEBlGrMin, 600000);
	ta.setValue(kTAEBlOblMax, 700000); // now set allowed amount and it should be happy
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk()); //
}

TEST_F(Invaliditetsdaekning_KI_OSV_25_49, Invaliditetsdaekning_I_PCT_Ingen_NEGATIVE_Partial) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kTAEReguleringskode, "Ingen");
	ta.setValue(kTAEBlGrMin, -1); // 20 is min
	ta.setValue(kTAEBlOblMax, 800001); // 80 is max

	auto r = re.validate(ta, false);
//	if (!r.isAllOk())
		cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_EQ(4, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kTAEBlGrMin, kValueUnderLimit));
	EXPECT_TRUE(r.hasMessages(kTAEBlOblMax, kValueOverLimit));

	ta.setValue(kTAEBlGrMin, 600000);
	ta.setValue(kTAEBlOblMax, 700000); // now set allowed amount and it should be happy
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk()); //
}

TEST_F(Invaliditetsdaekning_KI_OSV_25_49, Invaliditetsdaekning_SoliMax_TAEDaekningstype) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kTAESoliMax, "Tegningsmaks");

	auto r = re.validate(ta, false);
//	if (!r.isAllOk())
		cout << r;
	EXPECT_TRUE(r.isAllOk()); //

	ta.setValue(kTAESoliMax, "Obligatorisk maks");
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_EQ(1, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kTAEDaekningstype, kProductElementRequired));

	ta.setValue(kTAEDaekningstype, re.getDefaultValue(kTAEDaekningstype)->stringValue());
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk());

	ta.setValue(kTAESoliMax, "Tegningsmaks");
	r = re.validate(ta, false);
	cout << r;
	EXPECT_EQ(1, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kTAEDaekningstype, kProductElementNotAllowed));

	r = re.validate(ta, false);
	cout << r;

	ta.remove(kTAEDaekningstype);
	ta.setValue(kTAESoliMax, "Ingen");
	r = re.validate(ta, false);
	cout << r;
	EXPECT_EQ(1, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kTAEDaekningstype, kProductElementRequired));
}

TEST_F(Invaliditetsdaekning_KI_OSV_25_49, Invaliditetsdaekning_Kortvarig_Overbygning_Frivillig) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kKortTAE_Daekning, "Frivillig");

	auto r = re.validate(ta, false);
//	if (!r.isAllOk())
		cout << r;
	EXPECT_FALSE(r.isAllOk()); // should be missing both KortTAE_Min_obl_faktor and KortTAE_Daekningsperiode
	EXPECT_EQ(2, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kKortTAE_Min_obl_faktor, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kKortTAE_Daekningsperiode, kProductElementRequired));

	ta.setValue(kKortTAE_Min_obl_faktor, "1");
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_EQ(2, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kKortTAE_Min_obl_faktor, kValueNotAllowed));
	EXPECT_TRUE(r.hasMessages(kKortTAE_Daekningsperiode, kProductElementRequired));

	ta.setValue(kKortTAE_Min_obl_faktor, "0");
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_EQ(1, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kKortTAE_Daekningsperiode, kProductElementRequired));

	ta.setValue(kKortTAE_Daekningsperiode, "6");
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk());
}

TEST_F(Invaliditetsdaekning_KI_OSV_25_49, Invaliditetsdaekning_Kortvarig_Overbygning_Obligatorisk) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kKortTAE_Daekning, "Obligatorisk");

	auto r = re.validate(ta, false);
//	if (!r.isAllOk())
		cout << r;
	EXPECT_FALSE(r.isAllOk()); // should be missing both KortTAE_Min_obl_faktor and KortTAE_Daekningsperiode
	EXPECT_EQ(2, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kKortTAE_Min_obl_faktor, kProductElementRequired));
	EXPECT_TRUE(r.hasMessages(kKortTAE_Daekningsperiode, kProductElementRequired));

	ta.setValue(kKortTAE_Min_obl_faktor, "2");
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_EQ(1, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kKortTAE_Daekningsperiode, kProductElementRequired));

	ta.setValue(kKortTAE_Daekningsperiode, "6");
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk());

	ta.setValue(kKortTAE_Min_obl_faktor, "4");
	re.printConstantsInParser();
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_EQ(1, r.getValidationResults().size());
	EXPECT_TRUE(r.hasMessages(kKortTAE_Min_obl_faktor, kValueNotAllowed));

}










