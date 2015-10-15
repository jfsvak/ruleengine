#include <iostream>
#include <vector>
#include <map>

#include "../ruleengine/ContributionStep_sbx.h"
#include "gtest/gtest.h"
#include "gtest/internal/gtest-internal.h"
//#include "../../googletestlib/include/gtest/gtest.h"
//#include "../../googletestlib/include/gtest/internal/gtest-internal.h"
#include "../ruleengine/KonceptInfo_sbx.h"
#include "../ruleengine/RuleEngine_sbx.h"
#include "../ruleengine/sbxTypes.h"
#include "../ruleengine/TA_sbx.h"
#include "../ruleengine/Utils.h"
#include "../ruleengine/ValidationResults.h"

#include "ruleenginetestutils.h"
#include "testRuleEngineInitialiser.cpp"

using namespace std;
using namespace sbx;

class ContributionLadder_Follows_CONTEXT_KI_OSV_25_50 : public RuleEngineInitialiser {
protected:
    virtual void SetUp() {
    	try {
			RuleEngineInitialiser::SetUp();

			_ki = {OSV, 30, 0, // UnderkonceptOid:OSV 25-49
				{ { 1, "true"}, // Solidarisk faellestarif
				  { 6, "true"}, // SEB Firmapensionspulje
				  {11, "true"}, // Parameter-Basis
				  {15, "true"} // FG-Spaend
				} };
			re.initContext(_ki, FOLLOWS, 6);
    	} catch (const exception& e) {
    		cerr << "Exception: " << e.what() << endl;
    	}
    }

};


TEST_F(ContributionLadder_Follows_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_POSITIVE) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040" }; // KonceptOid 4 - OSV
//	ta.setValue(kUnionAgreementOid, 6);
//	ta.setValue(kUnionAgreementRelationship, sbx::kFOLLOWS);
	ta.setValue(kAftaleIkraftdato, 20080801);

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_EQ(1, r.getValidationResults(kBidragEjFoesteTrin_MK).size());
	EXPECT_TRUE(r.hasMessages(kBidragEjFoesteTrin_MK, kProductElementRequired));
//	ta.remove(kUnionAgreementOid);
//	ta.remove(kUnionAgreementRelationship);
//	ta.setUar(sbx::UnionAgreementRelationship::FOLLOWS);
//	ta.setUnionAgreementOid(6);
	re.initContext(_ki, FOLLOWS, 6);

	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_EQ(1, r.getValidationResults(kBidragEjFoesteTrin_MK).size());
	EXPECT_TRUE(r.hasMessages(kBidragEjFoesteTrin_MK, kProductElementRequired));

	ta.setValue(kBidragEjFoesteTrin_MK, false);
	ta.setValue(kAar, 2015);
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_EQ(1, r.getValidationResults(kAar).size());
	EXPECT_TRUE(r.hasMessages(kAar, kProductElementNotAllowed));


	ta.remove(kAar);
	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

	ta.addContributionStep( {0, 4, 2} );
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kProductElementNotAllowed));

	ta.setValue(kBidragEjFoesteTrin_MK, true);
	ta.removeContributionSteps();
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_EQ(1, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kProductElementRequired));

	ta.addContributionStep( {0, 4, 2} );
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_EQ(2, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueUnderLimit));

	ta.removeContributionSteps();
	ta.addContributionStep( {0, 1, 4} );
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_EQ(2, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueUnderLimit));

	ta.removeContributionSteps();
	ta.remove(kBidragsstigningsform);
	ta.addContributionStep( {0, 4, 8.1} );
	r = re.validate(ta, false);
	cout << r;
	EXPECT_TRUE(r.isAllOk());

	cout << r;
	ta.addContributionStep( {2, 5, 4} );
	r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_EQ(3, r.getValidationResults(kBidragstrappe).size());
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueNotAllowed));

}


TEST_F(ContributionLadder_Follows_CONTEXT_KI_OSV_25_50, Bidragsstigningsform_Alder_When_Follows) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040" }; // KonceptOid 4 - OSV
	ta.setValue(kAftaleIkraftdato, 20080801);
//	ta.setValue(kUnionAgreementOid, 6);
//	ta.setValue(kUnionAgreementRelationship, sbx::kFOLLOWS);
	ta.setValue(kBidragEjFoesteTrin_MK, true);
	ta.setValue(kBidragsstigningsform, "Alder");
	ta.addContributionStep({18, 4.5, 8.1});

	auto r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

}

TEST_F(ContributionLadder_Follows_CONTEXT_KI_OSV_25_50, Bidragstrappe_When_Follows_Ingen) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
	ta.setValue(kAftaleIkraftdato, 20080801);
//	ta.setValue(kUnionAgreementOid, 6);
//	ta.setValue(kUnionAgreementRelationship, sbx::kFOLLOWS);
	ta.setValue(kBidragEjFoesteTrin_MK, true);
	ta.addContributionStep({0, 4.5, 8.5});
	ta.setValue(kBidragsstigningsform, "Ingen");

	auto r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;
}


TEST_F(ContributionLadder_Follows_CONTEXT_KI_OSV_25_50, Bidragstrappe_When_Follows_Dato) {
	RuleEngine::_printDebugAtValidation = true;
	TA ta { "15124040"}; // KonceptOid 4 - OSV
	ta.setValue(kAftaleIkraftdato, 20080801);
//	ta.setValue(kUnionAgreementOid, 6);
//	ta.setValue(kUnionAgreementRelationship, sbx::kFOLLOWS);
	ta.setValue(kBidragEjFoesteTrin_MK, true);
	ta.addContributionStep({20080801, 4.5, 8.5});
	ta.setValue(kBidragsstigningsform, "Dato");

	auto r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;
}

TEST_F(ContributionLadder_Follows_CONTEXT_KI_OSV_25_50, Bidragstrappe_When_Included_Ingen) {
	RuleEngine::_printDebugAtValidation = true;
	KonceptInfo ki {OSV, 30, 0, // UnderkonceptOid:OSV 25-49
		{ { 1, "true"}, // Solidarisk faellestarif
		  { 6, "true"}, // SEB Firmapensionspulje
		  {11, "true"}, // Parameter-Basis
		  {15, "true"} // FG-Spaend
		} };
	re.initContext(ki, INCLUDED, 6);

	TA ta { "15124040" }; // KonceptOid 4 - OSV
	ta.setValue(kAftaleIkraftdato, 20080801);
//	ta.setValue(kUnionAgreementOid, 6);
//	ta.setValue(kUnionAgreementRelationship, sbx::kINCLUDED);
	ta.setValue(kBidragEjFoesteTrin_MK, true);
	ta.addContributionStep({0, 4.5, 8.5});
	ta.setValue(kBidragsstigningsform, "Ingen");

	auto r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

	ta.setValue(kBidragEjFoesteTrin_MK, false);
	ta.removeContributionSteps();

	r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

}

TEST_F(ContributionLadder_Follows_CONTEXT_KI_OSV_25_50, Bidragstrappe_When_Included_Dato) {
	RuleEngine::_printDebugAtValidation = true;
	KonceptInfo ki {OSV, 30, 0, // UnderkonceptOid:OSV 25-49
		{ { 1, "true"}, // Solidarisk faellestarif
		  { 6, "true"}, // SEB Firmapensionspulje
		  {11, "true"}, // Parameter-Basis
		  {15, "true"} // FG-Spaend
		} };
	re.initContext(ki, INCLUDED, 6);

	TA ta { "15124040" };
	ta.setValue(kAftaleIkraftdato, 20080801);
//	ta.setValue(kUnionAgreementOid, 6);
//	ta.setValue(kUnionAgreementRelationship, sbx::kINCLUDED);
	ta.setValue(kBidragEjFoesteTrin_MK, true);
	ta.addContributionStep({0, 4.5, 8.5});
	ta.setValue(kBidragsstigningsform, "Dato");

	auto r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

}

TEST_F(ContributionLadder_Follows_CONTEXT_KI_OSV_25_50, Bidragstrappe_Follows_UASTeps_NotInitialised) {
	RuleEngine::_printDebugAtValidation = true;
	KonceptInfo ki {OSV, 30, 0, // UnderkonceptOid:OSV 25-49
					{ { 1, "true"}, // Solidarisk faellestarif
					  { 6, "true"}, // SEB Firmapensionspulje
					  {11, "true"}, // Parameter-Basis
					  {15, "true"} // FG-Spaend
					} };
	re.initContext(ki, FOLLOWS, 4);

	TA ta { "15124040" };
	ta.setValue(kAftaleIkraftdato, 20150701);
//	ta.setValue(kUnionAgreementOid, 4);
//	ta.setValue(kUnionAgreementRelationship, sbx::kFOLLOWS);
	ta.setValue(kBidragEjFoesteTrin_MK, true);
	ta.addContributionStep({20150701, 5.0, 10.1});
	ta.setValue(kBidragsstigningsform, "Dato");

	auto r = re.validate(ta, false);
	EXPECT_TRUE(r.isAllOk());
	cout << r;

}

TEST_F(ContributionLadder_Follows_CONTEXT_KI_OSV_25_50, Bidragstrappe_Follows_Over100_NEGATIVE) {
	RuleEngine::_printDebugAtValidation = true;
	KonceptInfo ki {OSV, 30, 0, // UnderkonceptOid:OSV 25-49
					{ { 1, "true"}, // Solidarisk faellestarif
					  { 6, "true"}, // SEB Firmapensionspulje
					  {11, "true"}, // Parameter-Basis
					  {15, "true"} // FG-Spaend
					} };
	re.initContext(ki, FOLLOWS, 4);

	TA ta { "15124040" };
	ta.setValue(kAftaleIkraftdato, 20150701);
//	ta.setValue(kUnionAgreementOid, 4);
//	ta.setValue(kUnionAgreementRelationship, sbx::kFOLLOWS);
	ta.setValue(kBidragEjFoesteTrin_MK, true);
	ta.addContributionStep({20150701, 99, 1.1});

	auto r = re.validate(ta, false);
	EXPECT_FALSE(r.isAllOk());
	cout << r;
	EXPECT_TRUE(r.hasMessages(kBidragstrappe, kValueOverLimit));
}





