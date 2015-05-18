//============================================================================
// Name        : ruleenginemain.cpp?
// Author      : xjes
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <exception>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "ruleengine/Constant_sbx.h"
#include "ruleengine/KonceptInfo_sbx.h"
#include "ruleengine/ProductElementValue_sbx.h"
#include "ruleengine/RuleConstantContainer_sbx.h"
#include "ruleengine/RuleEngine_sbx.h"
#include "ruleengine/RuleCatalogue.h"
#include "ruleengine/Rule.h"
#include "ruleengine/sbxTypes.h"
#include "ruleengine/TA_sbx.h"
#include "ruleengine/ValidationResult.h"
#include "ruleenginetestutils.h"

using namespace std;
using namespace sbx;

int testRuleEngine(void);
int testRuleConstantContainer(void);
int testRuleConstantContainerPrint(void);
int testRuleConstantContainerWithException(void);
int testDefaultValue(RuleEngine&);
int testGetProducts(RuleEngine&);
int testValidateDeathCoverageTA(RuleEngine&);
int testSinglePEValidation();

const int NO_OF_DUMMY_CONSTANTS {15};

int main() {

	try {
//		RuleEngine re = testRCJsonLoad();
//		testDefaultValue(re);
//		testGetProducts(re);
//		testValidateDeathCoverageTA(re);
		testSinglePEValidation();
	} catch (runtime_error &e) {
		cout << "Exception while testing: " << e.what() << endl;
		return -1;
	}

	return 0;
}

int testSinglePEValidation()
{
//	RuleEngine::_printDebug = true;
	RuleEngine re {  };
	re.initConstants(get_file_contents("basedata-ruleconstants.json"));
    re.parseRuleCatalogueJSON(get_file_contents("rule-catalogue.json"));

    KonceptInfo ki {27, { {11, "true"} }};
    re.initContext(ki);
    re.printVariables();
    re.printConstants();

    // OK scenario
    TA ta { "15124040", 4};
	ta.setValue(kDoedReguleringskode, std::string {"Gage"})
			.setValue(kDoedPctGrMin, (long) 200)
			.setValue(kDoedPctOblMax, (long) 300);


    sbx::ValidationResult result = re.validate(ta, kDoedPctGrMin);

	re.printVariables();

    if (result.getValidationResults().size() > 0) {
    	cerr << "Got [" << result.getValidationResults().size() << " validation results, expected [0] - Messages:" << endl;

		for (auto& pair : result.getValidationResults()) {
			cout << "  PE[" << pair.first << "], msg[" << pair.second<< "]" << endl;
		}
    }

    //
    // NOT OK scenario - pe value is over the limit
    //
    //
    ta.getValue(kDoedPctGrMin).setValue("700");
	ta.getValue(kDoedPctOblMax).setValue("801");

	result = re.validate(ta, kDoedPctGrMin);

	if (result.getValidationResults().size() == 0) {
		cerr << "Expected validation messages. Got 0" << endl;
	}
	else
	{
		cout << "Messages (expected):" << endl;

		for (auto& pair : result.getValidationResults()) {
			cout << "  PE[" << pair.first << "], msg[" << pair.second<< "]" << endl;
		}
	}

	//
    // NOT OK Scenario - related pe value (spaend is over the limit)
	//
	//
	ta.getValue(kDoedPctGrMin).setValue("100");
	ta.getValue(kDoedPctOblMax).setValue("700");
	result = re.validate(ta, 1);

	if (result.getValidationResults().size() == 0) {
		cerr << "Expected validation messages. Got 0" << endl;
	}
	else
	{
		cout << "Messages (expected):" << endl;

		for (auto& pair : result.getValidationResults()) {
			cout << "  PE[" << pair.first << "], msg[" << pair.second<< "]" << endl;
		}
	}

	//
    // NOT OK scenario - pe value should not be there - DoedBlGrMin (amount) when DoedReguleringskode == 'Gage' (expects DoedPctGrMin)
	//
	//



    return 0;
}

int testValidateDeathCoverageTA(RuleEngine& re)
{
	re.getContainer().printContainerOverview(0);
	re.getContainer().printProducts();
	using sbx::ProductElementOid;

	KonceptInfo ki {27, { {11, "true"}, {6, "true"} }};
	TA ta { "15124040", 4 };
	ta.setValue(kDoedReguleringskode, "Gage")
			.setValue(kDoedPctGrMin, "200")
			.setValue(kDoedPctOblMax, "200")
			.setValue(kDoedBlGrMin, "100000");
	re.initContext(ki);
	re.validate(ki, ta);

	return 0;
}

int testGetProducts(RuleEngine& re) {
	re.initContext( {6, {} } );
	//	re.getContainer().printConstants();
	//	re.getContainer().printConstants(13, sbx::ProductElementOid::kLoenDefinition);
	//	re.getContainer().printConstants(13, sbx::ProductElementOid::k217);
	//	re.getContainer().printConstants(13, sbx::ProductElementOid::k218);
	re.getContainer().printParametersToProducts(6);
	re.getContainer().printProducts();
	re.getContainer().printParameters();
	re.getContainer().printParametersToProducts(6);

	auto& productOids = re.getContainer().getProductOids(15);
	cout << "Size of products for underkoncept 6, parameter 15[" << productOids.size() << "]\n";
	return 0;
}

int testDefaultValue(RuleEngine& re) {
	re.initContext( {17, { {11, "true"}, {16, "true"} }} );

	ostringstream s { };
	for (auto& c : re.getOptionsList(sbx::ProductElementOid::kLoenDefinition)) {
		s << "\"" << c->stringValue() << "\", ";
	}
	cout << "Allowed options for are LoenDefinition: [" << s.str() << "]" << endl;

	const Constant& c1 = re.getDefaultValue(sbx::ProductElementOid::kLoenDefinition);
	cout << "Default value for " << (unsigned short int)ProductElementOid::kLoenDefinition << " is [" << c1.stringValue() << "]" << endl;

	const Constant& c2 = re.getDefaultValue(sbx::ProductElementOid::kTaeBlGrMin);
	cout << "Default value for " << (unsigned short int)ProductElementOid::kTaeBlGrMin << " is [" << c2.doubleValue() << "]" << endl;
	return 0;
}

int testRuleConstantContainer(void) {
	cout << "testRuleConstantContainer start..." << endl;
	// get array of dummy constants
	// initialise RuleEngine with constants
	// set context for rule engine
	// get values in the context and check that they are the correct ones
	RuleConstantContainer rcc {  };
	cout << "RuleConstantContainer declared" << endl;
	vector<Constant> cVec(NO_OF_DUMMY_CONSTANTS);
	makeDummyConstants(cVec);
	rcc.initGlobalConstants(cVec);
	cout << "Initialising context" << endl;
	rcc.initContext(1, 0);
	cout << "Getting options as strings" << endl;
	vector<string> options = rcc.getOptions(sbx::kLoenRegulering);
	printVector(options);

	cout << "Getting options as shared_ptr" << endl;
	vector<shared_ptr<Constant>> optionsConstant = rcc.getOptionsList(sbx::kLoenRegulering);
	printVector(optionsConstant);

	rcc.getConstant(kTaeBlGrMin, kMin)->printValues();
	rcc.getConstant(kTaeBlOblMax, kMax)->printValues();

	cout << "testRuleConstantContainer End!" << endl;
	return 0;
}

int testRuleConstantContainerPrint(void) {
	cout << "testRuleConstantContainer start..." << endl;
	// get array of dummy constants
	// initialise RuleEngine with constants
	// set context for rule engine
	// get values in the context and check that they are the correct ones
	RuleConstantContainer rcc {  };
	cout << "RuleConstantContainer declared" << endl;
	vector<Constant> cVec(NO_OF_DUMMY_CONSTANTS);
	makeDummyConstants(cVec);
	rcc.initGlobalConstants(cVec);
	cout << "Initialising context" << endl;
	rcc.initContext(1, 0);
	rcc.printConstants();
	return 0;
}

int testRuleEngine(void) {
	cout << "testRuleEngine start..." << endl;
	// get array of dummy constants
	// initialise RuleEngine with constants
	// set context for rule engine
	// get values in the context and check that they are the correct ones
	RuleEngine re {  };
	cout << "RuleEngine declared" << endl;
	vector<Constant> cVec(NO_OF_DUMMY_CONSTANTS);
	makeDummyConstants(cVec);
	re.initConstants(cVec);
	cout << "Initialising context" << endl;
	re.initContext( {1, {} } );
	cout << "Getting options" << endl;
	vector<string> options = re.getOptions(sbx::kLoenRegulering);
	printVector(options);

	re.getConstant(kTaeBlGrMin, kMin)->printValues();
	re.getConstant(kTaeBlOblMax, kMax)->printValues();

	cout << "testRuleEngineMain End!" << endl;
	return 0;
}

int testRuleConstantContainerWithException(void) {
	cout << "testRuleConstantContainerWithException start..." << endl;
	// get array of dummy constants
	// initialise RuleEngine with constants
	// set context for rule engine
	// get values in the context and check that they are the correct ones
	RuleConstantContainer rcc {  };
	vector<Constant> cVec(NO_OF_DUMMY_CONSTANTS);
	makeDummyConstants(cVec);
	rcc.initGlobalConstants(cVec);
	rcc.getConstant(kTaeBlGrMin, kMin);
	cout << "testRuleConstantContainerWithException End!" << endl;
	return 0;
}
