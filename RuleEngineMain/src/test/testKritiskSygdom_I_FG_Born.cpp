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

class KritiskSygdom_I_FG_CONTEXT_KI_OSV_25_50 : public ::testing::Test  {
protected:
    virtual void SetUp() {
        re = RuleEngine();
        re.initConstants(get_file_contents("basedata-ruleconstants.json"));
        re.initKoncepts(get_file_contents("koncepts.json"));
        re.parseRuleCatalogueJSON(get_file_contents("rule-catalogue.json"));

        KonceptInfo ki {4, 30, 0, // UnderkonceptOid:OSV 25-49
        	{ {11, "true"}, // Parameter-Basis
        	  {1, "true"}, // Solidarisk faellestarif
        	  {15, "true"}, // FG Span - always selected on the KI
			  {6, "true"} // SEB Firmapensionspulje
        	} };
        re.initContext(ki, OUTSIDE);
    }

    RuleEngine re;
};


TEST_F(KritiskSygdom_I_FG_CONTEXT_KI_OSV_25_50, KritiskSygdom_I_FG_Boernesum_Pristal_Ingen_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kKritiskSygBornesum_i_FG_mk, true);
	ta.setValue(kKritiskSygBornesumReguleringskode, "Pristal");
	ta.setValue(kKritiskSygBornesumBlMin, (long) 0);
	ta.setValue(kKritiskSygBornesumBlMax, (long) 20000);


	auto r = re.validate(ta, {
			kKritiskSygBornesum_i_FG_mk,
			kKritiskSygBornesumReguleringskode,
			kKritiskSygBornesumBlMin,
			kKritiskSygBornesumBlMax,
			kKritiskSygBornesumSpaendBl
	});

	cout << r;
	EXPECT_FALSE(r.isAllOk());
	auto v = r.getValidationResults(kKritiskSygBornesum_i_FG_mk);
	ASSERT_EQ(1, v.size());
	// expect that true is not allowed for kKritiskSygBornesum_i_FG_mk, but other values should still validate to ok
	EXPECT_EQ(sbx::ValidationCode::kValueNotAllowed, v.at(0).getValidationCode());

	ta.setValue(kKritiskSygBornesumReguleringskode, "Ingen");

	re.validate(ta, {
				kKritiskSygBornesum_i_FG_mk,
				kKritiskSygBornesumReguleringskode,
				kKritiskSygBornesumBlMin,
				kKritiskSygBornesumBlMax,
				kKritiskSygBornesumSpaendBl
	});

	cout << r;
	EXPECT_FALSE(r.isAllOk());
	v = r.getValidationResults(kKritiskSygBornesum_i_FG_mk);
	ASSERT_EQ(1, v.size());
	// expect that true is not allowed for kKritiskSygBornesum_i_FG_mk, but other values should still validate to ok
	EXPECT_EQ(sbx::ValidationCode::kValueNotAllowed, v.at(0).getValidationCode());
}

TEST_F(KritiskSygdom_I_FG_CONTEXT_KI_OSV_25_50, KritiskSygdom_I_FG_Boernesum_Pristal_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kKritiskSygBornesum_i_FG_mk, true);
	ta.setValue(kKritiskSygBornesumReguleringskode, "Pristal");
	ta.setValue(kKritiskSygBornesumBlMin, (long) 20000);
	ta.setValue(kKritiskSygBornesumBlMax, (long) 70000);
	ta.setValue(kKritiskSygBornesumSuppldaekn_mk, false);
	ta.setValue(kKritiskSygBornesumSkattekode, re.getDefaultValue(kKritiskSygBornesumSkattekode)->stringValue());

	auto r = re.validate(ta, {
			kKritiskSygBornesum_i_FG_mk,
			kKritiskSygBornesumReguleringskode,
			kKritiskSygBornesumBlMin,
			kKritiskSygBornesumBlMax,
			kKritiskSygBornesumSpaendBl,
			kKritiskSygBornesumSuppldaekn_mk,
			kKritiskSygBornesumSkattekode
	});

	cout << r;
	EXPECT_FALSE(r.isAllOk());
	auto v = r.getValidationResults(kKritiskSygBornesum_i_FG_mk);
	ASSERT_EQ(1, v.size());
	// expect that true is not allowed for kKritiskSygBornesum_i_FG_mk, but other values should still validate to ok
	EXPECT_EQ(sbx::ValidationCode::kValueNotAllowed, v.at(0).getValidationCode());
}


//////////////////////////////////////////////////////////
// NEGATIVE SCENARIOS
//////////////////////////////////////////////////////////

TEST_F(KritiskSygdom_I_FG_CONTEXT_KI_OSV_25_50, KritiskSygdom_I_FG_Boernesum_Pristal_NEGATIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kKritiskSygBornesum_i_FG_mk, true);
	ta.setValue(kKritiskSygBornesumReguleringskode, "Pristal");
	ta.setValue(kKritiskSygBornesumBlMin, (long) -1);
	ta.setValue(kKritiskSygBornesumBlMax, (long) 110000);
	ta.setValue(kKritiskSygBornesumSuppldaekn_mk, false);
	ta.setValue(kKritiskSygBornesumSkattekode, re.getDefaultValue(kKritiskSygBornesumSkattekode)->stringValue());

	auto r = re.validate(ta, {
			kKritiskSygBornesum_i_FG_mk,
			kKritiskSygBornesumReguleringskode,
			kKritiskSygBornesumBlMin,
			kKritiskSygBornesumBlMax,
			kKritiskSygBornesumSpaendBl,
			kKritiskSygBornesumSuppldaekn_mk,
			kKritiskSygBornesumSkattekode
	});

	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_EQ(1, r.getValidationResults(kKritiskSygBornesum_i_FG_mk).size());
	if (r.getValidationResults(kKritiskSygBornesum_i_FG_mk).size() == 1)
		EXPECT_EQ(sbx::ValidationCode::kValueNotAllowed, r.getValidationResults(kKritiskSygBornesum_i_FG_mk).at(0).getValidationCode());

	EXPECT_EQ(1, r.getValidationResults(kKritiskSygBornesumBlMin).size());
	if (r.getValidationResults(kKritiskSygBornesumBlMin).size() == 1)
		EXPECT_EQ(sbx::ValidationCode::kValueUnderLimit, r.getValidationResults(kKritiskSygBornesumBlMin).at(0).getValidationCode());

	EXPECT_EQ(1, r.getValidationResults(kKritiskSygBornesumBlMax).size());
	if (r.getValidationResults(kKritiskSygBornesumBlMax).size() == 1)
		EXPECT_EQ(sbx::ValidationCode::kValueOverLimit, r.getValidationResults(kKritiskSygBornesumBlMax).at(0).getValidationCode());

	EXPECT_EQ(1, r.getValidationResults(kKritiskSygBornesumSpaendBl).size());
	// expect that true is not allowed for kKritiskSygBornesum_i_FG_mk, but other values should still validate to ok
	if (r.getValidationResults(kKritiskSygBornesumSpaendBl).size() == 1)
		EXPECT_EQ(sbx::ValidationCode::kValueOverLimit, r.getValidationResults(kKritiskSygBornesumSpaendBl).at(0).getValidationCode());
}
