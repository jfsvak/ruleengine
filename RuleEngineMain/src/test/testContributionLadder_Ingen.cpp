#include <string>
#include <vector>
#include <iostream>

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

class ContributionLadder_Ingen_CONTEXT_KI_OSV_25_50: public RuleEngineInitialiser {
protected:
    virtual void SetUp() {
    	RuleEngineInitialiser::SetUp();

		KonceptInfo ki { 4, 30, 0, // UnderkonceptOid:OSV 25-49
				{ { 1, "true" }, // Solidarisk faellestarif
						{ 6, "true" }, // SEB Firmapensionspulje
						{ 11, "true" }, // Parameter-Basis
						{ 15, "true" } // FG-Spaend
				} };
		re.initContext(ki, OUTSIDE);
	}
};

// Test kIndmeldelsesalder and getting options prior to that
TEST_F(ContributionLadder_Ingen_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Ingen_No_Hospitalsdaekning)
{
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4 }; // KonceptOid 4 - OSV
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Ingen");
	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kHospitalsdaekning_MK, false);

	// expecting complains about missing step
	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;
	ASSERT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_EQ(kProductElementRequired, r.getValidationResults(kBidragstrappe).at(0).getValidationCode());

	r = re.validate(ta, (unsigned short) kBidragsstigningsform);
	EXPECT_TRUE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;

	r = re.validate(ta, { kBidragsstigningsform, kBidragstrappe });
	EXPECT_TRUE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;
	EXPECT_EQ(2, r.getWarnings(kBidragstrappe).size());
	EXPECT_TRUE(r.hasWarnings(kBidragstrappe, kProductElementRequired));

	//  now add one step, but let it start from 3 (!=0), and it should fail
	ta.addContributionStep( { 3, 3.4, 5.3 });
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueNotAllowed));

	//  now remove that step, and add one that starts from 0 and no complains
	ta.removeContributionStep( { 3, 0, 0 });
	ta.addContributionStep( { 0, 3.4, 5.3 });
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
//	if (!r.isAllOk())
	cout << r;

	//  then add one more and it should complain again, that only 1 step is allowed, when "Ingen" is selected in Bidragsstigningsform
	ta.addContributionStep( { 1, 4, 6 });
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueNotAllowed));

	//  now remove the last step (index!=0), and everything should be fine again
	ta.removeContributionStep( { 1, 0, 0 });
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
//	if (!r.isAllOk())
	cout << r;
}

// Test kIndmeldelsesalder and getting options prior to that
TEST_F(ContributionLadder_Ingen_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Ingen_Hospitalsdaekning)
{
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040", 4 }; // KonceptOid 4 - OSV
	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Ingen");
	ta.setValue(kHospitalsdaekning_MK, true);
	ta.setValue(kHospitalsdaekningLeverandoer, "Codan");
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);

	// expecting complains about missing step
	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;
	ASSERT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_EQ(kProductElementRequired, r.getValidationResults(kBidragstrappe).at(0).getValidationCode());

	r = re.validate(ta, (unsigned short) kBidragsstigningsform);
	EXPECT_TRUE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;

	r = re.validate(ta, { kBidragsstigningsform, kBidragstrappe });
	EXPECT_TRUE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;
	EXPECT_EQ(2, r.getWarnings(kBidragstrappe).size());
	EXPECT_TRUE(r.hasWarnings(kBidragstrappe, kProductElementRequired));

	//  now add one step, but let it start from 3 (!=0), and it should fail
	ta.addContributionStep( { 3, 3.4, 5.3 });
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueNotAllowed));

	//  now remove that step, and add one that starts from 0 and no complains
	ta.removeContributionStep( { 3, 0, 0 });
	ta.addContributionStep( { 0, 3.4, 5.3 });
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
//	if (!r.isAllOk())
	cout << r;

	//  then add one more and it should complain again, that only 1 step is allowed, when "Ingen" is selected in Bidragsstigningsform
	ta.addContributionStep( { 1, 4, 6 });
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
	cout << r;
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueNotAllowed));

	//  now remove the last step (index!=0), and everything should be fine again
	ta.removeContributionStep( { 1, 0, 0 });
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
//	if (!r.isAllOk())
	cout << r;
}
