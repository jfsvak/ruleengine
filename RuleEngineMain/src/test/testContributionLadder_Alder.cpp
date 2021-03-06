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

class ContributionLadder_Alder_CONTEXT_KI_OSV_25_50 : public RuleEngineInitialiser {
protected:
    virtual void SetUp() {
    	RuleEngineInitialiser::SetUp();

        KonceptInfo ki {4, 30, 0, // UnderkonceptOid:OSV 25-49
        	{ { 1, "true"}, // Solidarisk faellestarif
//			  { 6, "true"}, // SEB Firmapensionspulje
        	  {11, "true"}, // Parameter-Basis
			  {15, "true"} // FG-Spaend
        	} };
        re.initContext(ki, OUTSIDE);
    }

};


TEST_F(ContributionLadder_Alder_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Alder_POSITIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Alder" );
	ta.setValue(kHospitalsdaekning_MK, false);


	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
		cout << r;
	EXPECT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kProductElementRequired));

	ta.addContributionStep( {18,2.5, 5} );
}

TEST_F(ContributionLadder_Alder_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Alder_1_Step_POSITIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Alder" );
	ta.setValue(kHospitalsdaekning_MK, false);
	ta.setValue(kIndmeldelsesAlder, 20);


	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
		cout << r;
	EXPECT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kProductElementRequired));

	ta.addContributionStep( {19, 1, 1} );
	re.printConstantsInParser();
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
		cout << r;
	EXPECT_EQ(3, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueNotAllowed));
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueUnderLimit));

	ta.removeContributionStep( {19, 1, 1} );
	ta.addContributionStep( {20, 3, 3} );
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
//	if (r.isAllOk())
		cout << r;
}

TEST_F(ContributionLadder_Alder_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Alder_1_Step_Ekskl_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Alder" );
	ta.setValue(kHospitalsdaekning_MK, false);
	ta.setValue(kIndmeldelsesAlder, 18);

	ta.addContributionStep( {18, 2, 1} ); // == 3 which is below BidragEksklHispdaekningPctMin == 3
	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueUnderLimit));
	EXPECT_EQ("33.1.2.1", r.getValidationResults(kBidragstrappe).at(0).getRuleId());

	ta.removeContributionStep( {18, 2, 1} ); // remove and set new step that equals to 4
	ta.addContributionStep( {18, 2, 4} );
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;
}

TEST_F(ContributionLadder_Alder_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Alder_1_Step_Inkl_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Alder" );
	ta.setValue(kHospitalsdaekning_MK, true);
	ta.setValue(kHospitalsdaekningLeverandoer, "Codan");
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);
	ta.setValue(kIndmeldelsesAlder, 18);

	ta.addContributionStep( {18, 2, 2} ); // == 4 which is below BidragInklHispdaekningPctMin == 5
	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueUnderLimit));
	EXPECT_EQ("33.1.1.1", r.getValidationResults(kBidragstrappe).at(0).getRuleId());

	ta.removeContributionStep( {18, 2, 2} ); // remove and set new step that equals to 5
	ta.addContributionStep( {18, 2, 3} );
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;
}

TEST_F(ContributionLadder_Alder_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Alder_3_Steps_Inkl_POSITIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Alder" );
	ta.setValue(kHospitalsdaekning_MK, true);
	ta.setValue(kHospitalsdaekningLeverandoer, "Codan");
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);
	ta.setValue(kIndmeldelsesAlder, 18);

	ta.addContributionStep( {18, 2.6, 3} );
	auto r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

	ta.addContributionStep( {22, 3, 4} );
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

	ta.addContributionStep( {23, 4, 3.2} );
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;
}

TEST_F(ContributionLadder_Alder_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Alder_2_Steps_Inkl_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Alder" );
	ta.setValue(kHospitalsdaekning_MK, true);
	ta.setValue(kHospitalsdaekningLeverandoer, "Codan");
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);
	ta.setValue(kIndmeldelsesAlder, 18);

	ta.addContributionStep( {18, 2, 3} );
	auto r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

	ta.addContributionStep( {22, 1, 1} );
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueUnderLimit));
}

TEST_F(ContributionLadder_Alder_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Alder_Before_Indmeldelsesalder_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Alder" );
	ta.setValue(kHospitalsdaekning_MK, true);
	ta.setValue(kHospitalsdaekningLeverandoer, "Codan");
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);
	ta.setValue(kIndmeldelsesAlder, 18);

	ta.addContributionStep( {15, 2, 3} );
	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;

	EXPECT_EQ(2, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueNotAllowed));
}




TEST_F(ContributionLadder_Alder_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Alder_AddAll_POSITIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Alder" );
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kHospitalsdaekning_MK, true);
	ta.setValue(kHospitalsdaekningLeverandoer, "Codan");
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);
	ta.setValue(kIndmeldelsesAlder, 18);

	std::vector<sbx::ContributionStep> v { {18,1,1}, {22,4,4}, {25,4,4} };
	ta.setContributionSteps(v);

	auto r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());
	EXPECT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueUnderLimit));

	ta.setContributionSteps({ {18,3,3}, {25,4,4}, {24,3,4} });
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk());
}

TEST_F(ContributionLadder_Alder_CONTEXT_KI_OSV_25_50, Duplicate_Alder_Index_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kAftaleIkraftdato, (long) 20150601);
	ta.setValue(kBidragsstigningsform, "Alder" );
	ta.setValue(kHospitalsdaekning_MK, true);
	ta.setValue(kHospitalsdaekningLeverandoer, "Codan");
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);
	ta.setValue(kIndmeldelsesAlder, 18);

	ta.setContributionSteps({ {18,3,3}, {25,4,4}, {24,3,4}, {24,3,4} });
	auto r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());

	EXPECT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueNotAllowed));

}

TEST_F(ContributionLadder_Alder_CONTEXT_KI_OSV_25_50, Alder_Index_Over100_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kAftaleIkraftdato, (long) 20150601);
	ta.setValue(kBidragsstigningsform, "Alder" );
	ta.setValue(kHospitalsdaekning_MK, true);
	ta.setValue(kHospitalsdaekningLeverandoer, "Codan");
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);
	ta.setValue(kIndmeldelsesAlder, 18);

	ta.setContributionSteps({ {18, 5, 5},
							  {24, 99, 2},
		                      {25, 4, 4} });
	auto r = re.validate(ta, false);
	cout << r;
	EXPECT_FALSE(r.isAllOk());

	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueOverLimit));

}






