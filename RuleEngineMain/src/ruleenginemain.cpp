//============================================================================
// Name        : ruleenginemain.cpp?
// Author      : xjes
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>

#include "ruleenginetestutils.h"
#include "ruleengine/Constant_sbx.h"
#include "ruleengine/ValidationResult.h"
#include "ruleengine/RuleEngine_sbx.h"
#include "ruleengine/TA_sbx.h"
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
int testValidatePEV(RuleEngine&);
int testGetProducts(RuleEngine&);
int testValidateDeathCoverageTA(RuleEngine&);

const int NO_OF_DUMMY_CONSTANTS = 15;

int main() {

	try {
		RuleEngine re = testRCJsonLoad();
//		testDefaultValue(re);
//		testGetProducts(re);
		testValidateDeathCoverageTA(re);
	} catch (exception &e) {
		cout << "Exception while testing: " << e.what() << endl;
		return -1;
	}

	return 0;
}

int testValidateDeathCoverageTA(RuleEngine& re)
{
	re.getContainer().printContainerOverview(0);
	re.getContainer().printProducts();
	using sbx::ProductElementOid;

	KonceptInfo ki {27, { {11, "true"}, {6, "true"} }};
	TA ta { {{static_cast<unsigned short>(kTaeBlGrMin), {kTaeBlGrMin, "100000"}}, {static_cast<unsigned short>(kLoenDefinition), {kLoenDefinition, "Gage"}}} };
	ta.setCVR("15124040")
			.setKonceptOid(4)
			.addValue(kDoedReguleringskode, "Gage")
			.addValue(kDoedPctGrMin, "200")
			.addValue(kDoedPctOblMax, "200")
			.addValue(kDoedBlGrMin, "100000");
	re.initContext(ki.getUnderkonceptOid(), 0);
	re.validate(ki, ta);

	return 0;
}

int testGetProducts(RuleEngine& re) {
	re.initContext(6,0);
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

int testValidatePEV(RuleEngine& re) {
	try {
		// Wrong lon type
		ProductElementValue pev {ProductElementOid::kLoenDefinition, "Min forkerte lon"};
		re.validate(pev);
	} catch (exception &e) {
		cout << "Expecting domain error: " << e.what() << endl;
	}

	try {
		// allowed lon type
		ProductElementValue pev {ProductElementOid::kLoenDefinition, "I henhold til kontrakt"};
		re.validate(pev);
	} catch (exception &e) {
		cout << "Not Expecting domain error: " << e.what() << endl;
	}
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


RuleEngine testRCJsonLoad() {
//	Constant::_printDebug = true;
//	Product::_printDebug = true;
//	RuleConstantContainer::_printDebug = true;
//	RuleEngine::_printDebug = true;
	RuleEngine re {  };
    string json = get_file_contents("basedata-ruleconstants.json");
    
	re.initConstants(json);
	cout << "Number of constants loaded [" << re.getContainer().size() << "]" << endl;
	re.initContext(9, 0);
//	re.getContainer().printContainerOverview(0);
	
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
	ProductElementValue pevString {sbx::kLoenDefinition, "Løn 1"};
	sbx::ValidationResult r =  re.validate(pevString);
	cout << "ValidationResult [" << r << "]" << endl;

	re.initContext(2, 0);
	r = re.validate(pevString);
	cout << "ValidationResult [" << r << "]" << endl;

	cout << "\nValidate double value" << endl;
	ProductElementValue pevDouble {sbx::ProductElementOid::kTaeBlGrMin, "100000"};
	r = re.validate(pevDouble);
	cout << "ValidationResult [" << r << "]" << endl;

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

