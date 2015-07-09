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

class ContributionLadder_Anciennitet_CONTEXT_KI_OSV_25_50 : public RuleEngineInitialiser {
protected:
    virtual void SetUp() {
    	RuleEngineInitialiser::SetUp();

        KonceptInfo ki {4, 30, 0, // UnderkonceptOid:OSV 25-49
        	{ { 1, "true"}, // Solidarisk faellestarif
			  { 6, "true"}, // SEB Firmapensionspulje
        	  {11, "true"}, // Parameter-Basis
			  {15, "true"} // FG-Spaend
        	} };
        re.initContext(ki, OUTSIDE);
    }

};


TEST_F(ContributionLadder_Anciennitet_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Anciennitet_POSITIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Anciennitet" );
	ta.setValue(kHospitalsdaekning_MK, false);

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
		cout << r;
	EXPECT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kProductElementRequired));

	ta.addContributionStep( {18,2.5, 5} );
}

TEST_F(ContributionLadder_Anciennitet_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Anciennitet_1_Step_POSITIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Anciennitet" );
	ta.setValue(kHospitalsdaekning_MK, false);


	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
		cout << r;
	EXPECT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kProductElementRequired));

	ta.addContributionStep( {0, 1, 1} );
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
//	if (r.isAllOk())
		cout << r;
	EXPECT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueUnderLimit));

	ta.removeContributionStep( {0, 1, 1} );
	ta.addContributionStep( {0, 3, 3} );
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
//	if (r.isAllOk())
		cout << r;
}

TEST_F(ContributionLadder_Anciennitet_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Anciennitet_1_Step_Ekskl_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Anciennitet" );
	ta.setValue(kHospitalsdaekning_MK, false);

	ta.addContributionStep( {0, 2, 1} ); // == 3 which is below BidragEksklHispdaekningPctMin == 3
	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueUnderLimit));
	EXPECT_EQ("33.1.2.1", r.getValidationResults(kBidragstrappe).at(0).getRuleId());

	ta.removeContributionStep( {0, 2, 1} ); // remove and set new step that equals to 6
	ta.addContributionStep( {0, 2, 4} );
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;
}

TEST_F(ContributionLadder_Anciennitet_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Anciennitet_1_Step_Inkl_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Anciennitet" );
	ta.setValue(kHospitalsdaekning_MK, true);
	ta.setValue(kHospitalsdaekningLeverandoer, "Codan");
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);

	ta.addContributionStep( {0, 2, 2} ); // == 4 which is below BidragInklHispdaekningPctMin == 5
	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueUnderLimit));
	EXPECT_EQ("33.1.1.1", r.getValidationResults(kBidragstrappe).at(0).getRuleId());

	ta.removeContributionStep( {0, 2, 2} ); // remove and set new step that equals to 5
	ta.addContributionStep( {0, 2, 3} );
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;
}

TEST_F(ContributionLadder_Anciennitet_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Anciennitet_3_Steps_Inkl_POSITIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Anciennitet" );
	ta.setValue(kHospitalsdaekning_MK, true);
	ta.setValue(kHospitalsdaekningLeverandoer, "Codan");
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);

	ta.addContributionStep( {0, 2.6, 3} );
	auto r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

	ta.addContributionStep( {2, 3, 4} );
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

	ta.addContributionStep( {5, 4, 3.2} );
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;
}

TEST_F(ContributionLadder_Anciennitet_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Anciennitet_3_Steps_Inkl_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Anciennitet" );
	ta.setValue(kHospitalsdaekning_MK, true);
	ta.setValue(kHospitalsdaekningLeverandoer, "Codan");
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);

	ta.addContributionStep( {0, 2, 3} );
	auto r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

	ta.addContributionStep( {2, 1, 1} ); // add a step where the total is not increasing 1+1 < 2+3
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueUnderLimit));
}

TEST_F(ContributionLadder_Anciennitet_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Anciennitet_Over100_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
//	ta.setValue(kUnionAgreementRelationship, kOUTSIDE);
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.setValue(kBidragsstigningsform, "Anciennitet" );
	ta.setValue(kHospitalsdaekning_MK, true);
	ta.setValue(kHospitalsdaekningLeverandoer, "Codan");
	ta.setValue(kHospitalsdaekningFrivillig_MK, false);

	ta.addContributionStep( {0, 2, 3} );
	auto r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

	ta.addContributionStep( {2, 99, 2} ); // add a step where the total is not increasing 1+1 < 2+3
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueOverLimit));
}












