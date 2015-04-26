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
int testRCJsonLoad();
int testJsonLoad();
std::string get_file_contents(const char *filename);
void PrintJSONValue( const Json::Value &val );
bool PrintJSONTree(Json::Value &root, unsigned short depth);
void testFoo();
void testFoo2(FooHolder& fh);

const int NO_OF_DUMMY_CONSTANTS = 15;

int main() {

	try {
//		cout << "Hello world" << endl;
//		cout << "Another Hello world" << endl;
		testFoo();
//		return testRCJsonLoad();
	} catch (exception &e) {
		cout << "Exception while testing: " << e.what() << endl;
	}

	return -1;
}

void testFoo() {
	cout << "  create vector  " << endl;
	FooHolder fh;
	fh.createVector();
	cout << "  getFoo  " << endl;
	const Foo& f = fh.getFoo(1);
	cout << "  call getFoo  " << endl;
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
	cout << "  end of testFoo2 " << endl;
}

int testRCJsonLoad() {
//	Constant::_printDebug = true;
//	RuleConstantContainer::_printDebug = true;
	RuleEngine::_printDebug = true;
	RuleEngine re {  };
	string json = get_file_contents("basedata-ruleconstants.json");// rc-small.json");
	re.initConstants(json);
//	cout << "Number of constants loaded [" << re.getContainer().size() << "]" << endl;
//	re.getContainer().printContainerOverview();
//	re.getContainer().printContainerOverview(5);

	re.initContext(9, 0);
//	re.getContainer().printContainerOverview(9, sbx::kEquals);

	ProductElementValue pev {ProductElementOid::kLoenDefinition, ProductElementTypes::kText, "Min forkerte løn"};
	try {
		re.validate(pev);
	} catch (exception &e) {
		cout << "Expecting domain error: " << e.what() << endl;
	}

	try {
		re.validate(pev);
	} catch (exception &e) {
		cout << "Not Expecting domain error: " << e.what() << endl;
	}

	return 0;
}

int testJsonLoad() {
	cout << "Load file: " << "rc-small.json" << endl;
	string json = get_file_contents("rc-small.json");

//	cout << "Loading Json from string:" << endl << json << endl;

	Json::Value root;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(json, root);

	if (parsingSuccessful)
	{
//		PrintJSONTree(root["data"], 0);

//		Json::Value data = root["data"];

		Json::Value ruleConstants = root["data"].get("ruleConstant", 0);

		cout << "Printing RC " << endl;
		PrintJSONValue(ruleConstants);
		cout << "\nRC size: " << ruleConstants.size() << endl;

		Json::Value rc0 = ruleConstants.get(0u, 0);
		cout << "RC0 type: " << rc0.type() << " size: " << rc0.size() << endl;

		Json::Value::Members rcMembers = rc0.getMemberNames();

		for ( unsigned int i=0; i<rcMembers.size();++i) {
			string member = rcMembers[i];
			Json::Value value = rc0[member];
			cout << "  " << member << "=" << value.asString() << endl;
		}

		for (Json::ValueIterator ruleConstantElement = ruleConstants.begin(); ruleConstantElement != ruleConstants.end(); ruleConstantElement++) {
			cout << "Key:" << ruleConstantElement.key().asInt() << "Type:" << ruleConstantElement->type() << "Size:" << ruleConstantElement->size() << endl;

			int peOid = ruleConstantElement->get("productElementOid", 0).asInt();
			cout << "ProductElement:" << peOid << endl;

//			for (Json::ValueIterator elementItr = rcItr->begin(); elementItr != rcItr->end(); elementItr++) {
//				cout << "  Key[" << elementItr.key().asString() << "] Value:";
//				PrintJSONValue(*elementItr);
//				cout << endl;
//
//			}

		}
//		for ( Json::ValueIterator itr = data.begin(); itr != data.end(); itr++) {
//			PrintJSONValue(itr.key());
//
//			for (Json::ValueIterator itr2 = itr->begin(); itr2 != itr->end(); itr2++) {
//				cout << "Key:" << itr2.key().asInt() << "Size:" << itr2->size() << endl;
////				PrintJSONValue();
//			}
//		}

//		Json::Value::Members members = root["data"].getMemberNames();
//		cout << "Members found in data: " << members.size() << endl;
//
//		for (unsigned int i = 0; i<members.size();++i) {
//			string name = members[i];
//			Json::Value value = root[name];
//			cout<<"Key: " << name <<endl;
//			cout<<"Value: " << value.toStyledString() <<endl;
//			cout<<"type: " << value.type() <<endl;
//		}

	}
	else {
		cout<<"Failed to parse JSON"<< endl <<reader.getFormattedErrorMessages() << endl;
	}
	return 0;
}

bool PrintJSONTree(Json::Value &root, unsigned short depth) {
	depth += 1;
	cout << " {type=["<< root.type()<< "], size="<< root.size() << "}";
	if( root.size() > 0 ) {
		cout << "\n";

		for(Json::ValueIterator itr = root.begin(); itr != root.end(); itr++) {

			for( int tab = 0 ; tab < depth; tab++) {
				cout << "-";
			}
			cout << " ";
			PrintJSONValue(itr.key());
			cout << " -";
			PrintJSONTree( *itr, depth);
		}
		return true;
	} else {
		cout << " ";
		PrintJSONValue(root);
		cout << "\n" ;
	}
	return true;
}

void PrintJSONValue( const Json::Value &val )
{
    if( val.isString() ) {
        cout << "string("<< val.asString().c_str() << ")";
    } else if( val.isBool() ) {
    	cout << "bool(" << val.asBool() << ")";
    } else if( val.isInt() ) {
        cout << "int(" << val.asInt() << ")";
    } else if( val.isUInt() ) {
        cout << "uint(" << val.asUInt() << ")";
    } else if( val.isDouble() ) {
        cout << "double(" << val.asDouble() << ")";
    }
    else
    {
        cout << "unknown type=[" << val.type() << "]";
    }
}

std::string get_file_contents(const char *filename)
{
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in)
  {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return(contents);
  }
  throw(1);
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
