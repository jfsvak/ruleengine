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

class Opsparingsproduct_RuleEngine_CONTEXT_KI_OSV_25_50 : public RuleEngineInitialiser {
protected:
    virtual void SetUp() {
    	RuleEngineInitialiser::SetUp();

        KonceptInfo ki {4, 30, 0, // UnderkonceptOid:OSV 25-49
        	{ {11, "true"}, // Parameter-Basis
        	  {1, "true"}, // Solidarisk faellestarif
			  {6, "true"} // SEB Firmapensionspulje
        	} };
        re.initContext(ki, OUTSIDE);
    }
};


// Test Opsparingsprodukter - PE(72, 73, 74, 75, 201)-P(60)
// Allowed values: true false
TEST_F(Opsparingsproduct_RuleEngine_CONTEXT_KI_OSV_25_50, Opsparingsprodukt_ALL_SELECTED_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kMarkedspension_MK, true )
			.setValue(kTraditionel_MK, true)
			.setValue(kTidspensionMedGaranti_MK, true)
			.setValue(kTidspensionUdenGaranti_MK, true)
			.setValue(kLink_MK, true);

	auto r = re.validate(ta, {kMarkedspension_MK, kTraditionel_MK, kTidspensionMedGaranti_MK, kTidspensionUdenGaranti_MK, kLink_MK} );
	EXPECT_TRUE(r.isAllOk());
	cout << r;
}

// Test Opsparingsprodukter - PE(72, 73, 74, 75, 201)-P(60)
// Allowed values: true false
TEST_F(Opsparingsproduct_RuleEngine_CONTEXT_KI_OSV_25_50, Opsparingsprodukt_SOME_SELECTED_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kMarkedspension_MK, true )
			.setValue(kTraditionel_MK, true)
			.setValue(kTidspensionMedGaranti_MK, false)
			.setValue(kLink_MK, false);

	auto r = re.validate(ta, {kMarkedspension_MK, kTraditionel_MK, kTidspensionMedGaranti_MK, kTidspensionUdenGaranti_MK, kLink_MK} );
	EXPECT_TRUE(r.isAllOk());

//	if (!r.isAllOk())
		cout << r;
}

// Test Opsparingsprodukter - PE(72, 73, 74, 75, 201)-P(60)
// Allowed values: true false
// Expected result:
//   Should fail due to Traditionel is selected as Standardprodukt but Traditionel_MK == false
//
TEST_F(Opsparingsproduct_RuleEngine_CONTEXT_KI_OSV_25_50, Opsparingsprodukt_NEGATIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kMarkedspension_MK, true);
	ta.setValue(kTraditionel_MK, false);

	ta.setValue(kStandardProduct, "Traditionel_MK");

	auto r = re.validate(ta, std::vector<unsigned short> {kStandardProduct, kMarkedspension_MK, kTraditionel_MK} );
	EXPECT_FALSE(r.isAllOk());

//	if (r.isAllOk())
		cout << r;

	auto v = r.getValidationResults(kStandardProduct);
	EXPECT_GE(v.size(), 1);

	EXPECT_EQ(kFail, v.at(0).getValidationCode());
}

// Test Opsparingsprodukter - PE(72, 73, 74, 75, 201)-P(60)
// Allowed values: true false
// Expected result:
//   Should be OK as the selected StandardProdukt is also selected on the TA
//
TEST_F(Opsparingsproduct_RuleEngine_CONTEXT_KI_OSV_25_50, Opsparingsprodukt_OK) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kMarkedspension_MK, true);
	ta.setValue(kTraditionel_MK, true);
	ta.setValue(kStandardProduct, "Traditionel_MK");

	auto r = re.validate(ta, std::vector<unsigned short> {kStandardProduct} );
	EXPECT_TRUE(r.isAllOk());

//	if (!r.isAllOk())
		cout << r;
}

// Test Opsparingsprodukter - PE(72, 73, 74, 75, 201)-P(60)
// Allowed values:
// Expected result:
//   Should be OK as the selected StandardProdukt is also selected on the TA
//
TEST_F(Opsparingsproduct_RuleEngine_CONTEXT_KI_OSV_25_50, Opsparingsprodukt_RemoveFromTA_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kMarkedspension_MK, true);
	ta.setValue(kTraditionel_MK, true);

	ta.setValue(kStandardProduct, "Traditionel_MK");

	auto r = re.validate(ta, (unsigned short) kStandardProduct );
	EXPECT_TRUE(r.isAllOk());

//	if (!r.isAllOk())
		cout << r;

	// now set the traditional to false and validate again, now it should give a an error saying that the traditional cannot be selected as standard produkt
	//  as it hasn't been selected on the ta
	ta.setValue(kTraditionel_MK, false);
	r = re.validate(ta, (unsigned short) kStandardProduct );
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
		cout << r;
}

TEST_F(Opsparingsproduct_RuleEngine_CONTEXT_KI_OSV_25_50, Opsparingsprodukt_MinAndelTraditionel_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	ta.setValue(kMarkedspension_MK, true);
	ta.setValue(kTraditionel_MK, true);
	ta.setValue(kTidspensionMedGaranti_MK, true);
	ta.setValue(kTidspensionUdenGaranti_MK, true);

	ta.setValue(kStandardProduct, "Traditionel_MK");

	RuleEngine::_printDebugAtValidation = true;
	RuleEngine::_printDebug = true;
	auto r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

	ta.setValue(kTidspensionMedGaranti_MK, false);
	ta.setValue(kTidspensionUdenGaranti_MK, false);
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk());

	ta.setValue(kMinAndelTraditionelPct, (long) 6);
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_EQ(1, r.getValidationResults(kMinAndelTraditionelPctType).size());
	EXPECT_TRUE(r.hasMessages(kMinAndelTraditionelPctType, sbx::ValidationCode::kProductElementRequired));


	ta.setValue(kMinAndelTraditionelPctType, "garbabe");
	r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_TRUE(r.hasMessages(kMinAndelTraditionelPctType, sbx::ValidationCode::kValueNotAllowed));

	ta.setValue(kMinAndelTraditionelPctType, "GAGE");
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk());

	ta.setValue(kTidspensionMedGaranti_MK, true);
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk());

}

