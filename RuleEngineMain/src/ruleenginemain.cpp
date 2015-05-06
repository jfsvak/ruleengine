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
#include <memory>
#include <fstream>
#include <sstream>

#include "ruleenginetestutils.h"
#include "ruleengine/Constant.h"
#include "ruleengine/RuleEngine.h"
#include "json/json.h"
#include "Foo.h"
#include "FooHolder.h"

using namespace std;
using namespace sbx;

int testRuleEngine(void);
int testRuleEngineValidateAllowedOption(void);
int testRuleConstantContainer(void);
int testRuleConstantContainerPrint(void);
int testRuleConstantContainerWithException(void);
RuleEngine testRCJsonLoad();
void testFoo();
void testFoo2(FooHolder& fh);
int testDefaultValue(RuleEngine&);

const int NO_OF_DUMMY_CONSTANTS = 15;

int main() {

	try {
//        testRuleEngineValidateAllowedOption();
		RuleEngine re = testRCJsonLoad();
		testDefaultValue(re);
	} catch (exception &e) {
		cout << "Exception while testing: " << e.what() << endl;
		return -1;
	}

	return 0;
}

int testDefaultValue(RuleEngine& re) {
	re.initContext(13,0);
//	re.getContainer().printConstants();
	re.getContainer().printConstants(13, sbx::ProductElementOid::kLoenDefinition);
	re.getContainer().printConstants(13, sbx::ProductElementOid::k217);
	re.getContainer().printConstants(13, sbx::ProductElementOid::k218);
	re.getContainer().printProducts();

	const Constant& c1 = re.getDefaultValue(sbx::ProductElementOid::kLoenDefinition);
	cout << "Default value for " << ProductElementOid::kLoenDefinition << " is [" << c1.stringValue() << "]" << endl;

	const Constant& c2 = re.getDefaultValue(sbx::ProductElementOid::kTaeBlGrMin);
	cout << "Default value for " << ProductElementOid::kTaeBlGrMin << " is [" << c2.doubleValue() << "]" << endl;
	return 0;
}


RuleEngine testRCJsonLoad() {
//	Constant::_printDebug = true;
	RuleConstantContainer::_printDebug = true;
	RuleEngine::_printDebug = true;
	RuleEngine re {  };
    string json = get_file_contents("basedata-ruleconstants.json");
    
	re.initConstants(json);
	cout << "Number of constants loaded [" << re.getContainer().size() << "]" << endl;
	re.initContext(9, 0);
	re.getContainer().printContainerOverview(0);
	
	try {
        // Wring lon type
        ProductElementValue pev {ProductElementOid::kLoenDefinition, ProductElementTypes::kText, "Min forkerte lon"};
    	re.validate(pev);
	} catch (exception &e) {
		cout << "Expecting domain error: " << e.what() << endl;
	}
    
    try {
        // allowed lon type
        ProductElementValue pev {ProductElementOid::kLoenDefinition, ProductElementTypes::kText, "I henhold til kontrakt"};
		re.validate(pev);
	} catch (exception &e) {
		cout << "Not Expecting domain error: " << e.what() << endl;
	}

	return re;
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
	ProductElementValue pevDouble {sbx::ProductElementOid::kTaeBlGrMin, sbx::ProductElementTypes::kCurr, "100000"};
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

void testFoo() {
	cout << "  create vector  " << endl;
	FooHolder fh;
	fh.createVector();
	cout << "  getFoo  " << endl;
	const Foo& f = fh.getFoo(1);
	cout << "  " << f.getVal() << " call getFoo  " << endl;
	testFoo2(fh);
    
	cout << "  call getFoosByRef  " << endl;
	const vector<Foo>& foos = fh.getFoosByRef();
	cout << "  list content of getFoosRef  " << endl;
	for (auto& foo : foos) {
		cout << "  Foo value:" << foo.getValInspect() << endl;
	}
    
	cout << "  call getFoosByVal  " << endl;
	vector<Foo> foosVal = fh.getFoosByValue();
	cout << "  list content of getFoosVal  " << endl;
	for (auto& foo : foosVal) {
		cout << "  Foo value:" << foo.getVal() << endl;
	}
    
	cout << "  the end" << endl;
}

void  testFoo2(FooHolder& fh) {
	cout << "  getFoo in testFoo2 " << endl;
	const Foo& f = fh.getFoo(1);
	cout << "  " << f.getVal() << " end of testFoo2 " << endl;
}

