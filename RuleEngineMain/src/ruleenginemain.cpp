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

#include "ruleengine/Constant.cpp"
#include "ruleengine/RuleEngine.h"

using namespace std;

void printVector(vector<string>);

int main() {
	cout << "RuleEngineMain start..." << endl;
//	sbx::Constant* c_ptr;
//	std::vector<Constant> global_constants;
//	sbx::ConstantTemplate<std::string> c {"hello"};
//	cout << "String:" << c.value() << endl;

//	sbx::StringConstant sc {"another hello"};
//	cout << "String:" << sc.value() << endl;


//	sbx::RuleEngine re {};
//	re.Init();
//
//	vector<string> options = re.GetOptionsList(sbx::kLoenRegulering);
//	printVector(options);
//
	cout << "RuleEngineMain End!" << endl;
	return 0;
}

int getValuesFromCIN() {
	sbx::RuleEngine re;
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

void printVector(vector<string> v) {
	cout << "------------- Vector Content BEGIN ------------" << endl;

	for (string option : v) {
		cout << "Option=" << option << endl;
	}
	cout << "------------- Vector Content END ------------" << endl;
}
