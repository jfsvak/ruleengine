//============================================================================
// Name        : ruleenginetestutils.cpp
// Author      : xjes
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#include <iostream>
#include <sstream>
#include <vector>
#include "ruleenginetestutils.h"
#include "ruleengine/Constant.h"

using namespace std;
using namespace sbx;

void makeDummyConstants(vector<Constant> &constants) {
	//	  sbx::Constant *c1 = nullptr, *c2 = nullptr, *c3 = nullptr, *c4 = nullptr, *c5 = nullptr;
	//	  c1 = new Constant { 1, 0, sbx::kLoenRegulering, sbx::kEquals, "Løn 1" };
	//	  c2 = new Constant { 1, 0, sbx::kLoenRegulering, sbx::kEquals, "Løn 2" };
	//	  c3 = new Constant { 1, 0, sbx::kLoenRegulering, sbx::kEquals, "Løn 3" };
	//	  c4 = new Constant { 2, 0, sbx::kLoenRegulering, sbx::kEquals, "Løn 4" };
	//	  c5 = new Constant { 2, 0, sbx::kLoenRegulering, sbx::kEquals, "Løn 5" };
	//	  Constant *c1 = new  Constant { 1, 0, sbx::kLoenRegulering, sbx::kEquals, "Løn 1" };
	constants[0] = *new  Constant { 1, 0, sbx::kLoenDefinition, sbx::kEquals, "Løn 1" };
	constants[1] = *new  Constant { 1, 0, sbx::kLoenDefinition, sbx::kEquals, "Løn 2", true };
	constants[2] = *new  Constant { 1, 0, sbx::kLoenDefinition, sbx::kEquals, "Løn 3" };
	constants[5] = *new  Constant { 1, 0, sbx::kTaeBlGrMin, sbx::kMin, "100000" };
	constants[13] = *new  Constant { 1, 0, sbx::kTaeBlGrMin, sbx::kMax, "800000" };
	constants[7] = *new  Constant { 1, 0, sbx::kTaeBlOblMax, sbx::kMax, "750000" };
	constants[9] = *new  Constant { 1, 0, sbx::kTaeSpaendBl, sbx::kMin, "0" };
	constants[11] = *new  Constant { 1, 0, sbx::kTaeSpaendBl, sbx::kMax, "100000" };
	constants[6] = *new  Constant { 2, 0, sbx::kTaeBlGrMin, sbx::kMin, "150000" };
	constants[14] = *new  Constant { 2, 0, sbx::kTaeBlGrMin, sbx::kMax, "750000" };
	constants[8] = *new  Constant { 2, 0, sbx::kTaeBlOblMax, sbx::kMax, "800000" };
	constants[10] = *new  Constant { 2, 0, sbx::kTaeSpaendBl, sbx::kMin, "0" };
	constants[12] = *new  Constant { 2, 0, sbx::kTaeSpaendBl, sbx::kMax, "200000" };
	constants[3] = *new  Constant { 2, 0, sbx::kLoenDefinition, sbx::kEquals, "Løn 4", true };
	constants[4] = *new  Constant { 2, 0, sbx::kLoenDefinition, sbx::kEquals, "Løn 5" };
	cout << "Dummy constants created and placed in vector" << endl;
}

int testConstant(void) {
	//	sbx::Constant* c_ptr;
	//	std::vector<Constant> global_constants;
	//	sbx::ConstantTemplate<std::string> c {"hello"};
	sbx::Constant c { 1, 0, sbx::kTaeBlGrMin, sbx::kMin, "123.45" };
	cout << "String: " << c.stringValue() << endl;
	cout << "Long: " << c.longValue() << endl;
	cout << "Double: " << c.doubleValue()*2 << endl;
	return 0;
}

void printVector(const vector<string> &v) {
	cout << "------------- Vector Content BEGIN ------------" << endl;

	for (const string option : v) {
	  cout << "Option=" << option << endl;
	}
	cout << "------------- Vector Content END ------------" << endl;
}

void printVector(const vector<shared_ptr<Constant>> &v) {
	cout << "------------- Vector Content BEGIN ------------" << endl;

	for (const shared_ptr<Constant> constant : v) {
		constant->printValues();
	}

	cout << "------------- Vector Content END ------------" << endl;
}

int getValuesFromCIN() {
//	  sbx::RuleEngine re;
	  //	re.Init();

	  string s;
	  //	std::vector<std::pair<std::string, long>> operands;
//
//	do {
//		int v;
//		s.clear();
//		cout << "Enter product element: ";
//		cin >> s;
//		if (s == "exit")
//			break;
//		cout << "Enter value :" << endl;
//		cin >> v;
//		cout << "Product Element: " << s << " Value: " << v << endl;
//		pair<string, long> p(s,v);
//		operands.push_back(p);
//	} while (s != "exit");
//
//	for (auto & element : operands) {
//		stringstream sstm;
//		sstm << element.first << "=" << element.second;
//		string s = sstm.str();
//		cout << "Added " << s << endl;
//	}
//
//	int result = re.Validate(sbx::kTaeSpaendBl, operands);
//	cout << "No of validation exceptions: [" << result << "]" << endl;

	  return 0;
}
