//============================================================================
// Name        : ruleenginemain.cpp?
// Author      : xjes
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sstream>
#include <vector>

#include "ruleenginetestutils.h"
#include "ruleengine/Constant.h"
#include "ruleengine/RuleEngine.h"

using namespace std;
using namespace sbx;

int testRuleEngine(void);
int testRuleEngineValidateAllowedOption(void);
int testRuleConstantContainer(void);
int testRuleConstantContainerPrint(void);
int testRuleConstantContainerWithException(void);

const int NO_OF_DUMMY_CONSTANTS = 15;

int main() {

	try {
		cout << "Hello world" << endl;
		cout << "Another Hello world" << endl;
		return testRuleEngineValidateAllowedOption();
	} catch (exception &e) {
		cout << "Exception while testing: " << e.what() << endl;
	}

	return -1;
}

int testRuleEngineValidateAllowedOption(void) {
	cout << "testRuleEngine start..." << endl;
	// get array of dummy constants
	// initialise RuleEngine with constants
	// set context for rule engine
	// get values in the context and check that they are the correct ones
	RuleEngine re {  };
	vector<Constant> cVec(NO_OF_DUMMY_CONSTANTS);
	makeDummyConstants(cVec);
	re.initConstants(cVec);
	re.initContext(1, 0);

	cout << "Validate allowed string" << endl;
	ProductElementValue pevString {sbx::kLoenDefinition, sbx::kText, "Løn 1"};
	int validationResult = re.validate(pevString);
	cout << "ValidationResult [" << validationResult << "]" << endl;

	re.initContext(2, 0);
	validationResult = re.validate(pevString);
	cout << "ValidationResult [" << validationResult << "]" << endl;

	cout << "\nValidate double value" << endl;
	ProductElementValue pevDouble {sbx::ProductElementNames::kTaeBlGrMin, sbx::ProductElementTypes::kCurr, "100000"};
	validationResult = re.validate(pevDouble);
	cout << "ValidationResult [" << validationResult << "]" << endl;

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
	re.initContext(1, 0);
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
