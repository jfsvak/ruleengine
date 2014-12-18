//============================================================================
// Name        : ruleenginemain.cpp
// Author      : xjes
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sstream>
#include <vector>
#include "ruleengine/ruleengine.h"

using namespace std;

int main() {
	sbx::RuleEngine re;
	re.Init();
	// (taeblgrmax-taebloblmax) >= rc.taespaendbl.max

	string s;
	std::vector<std::pair<std::string, long>> operands;
	do {
		int v;
		s.clear();
		cout << "Enter product element: ";
		cin >> s;
		if (s == "exit")
			break;
		cout << "Enter value :" << endl;
		cin >> v;
		cout << "Product Element: " << s << " Value: " << v << endl;
		pair<string, long> p(s,v);
		operands.push_back(p);
	} while (s != "exit");

	for (auto & element : operands) {
		stringstream sstm;
		sstm << element.first << "=" << element.second;
		string s = sstm.str();
		cout << "Added " << s << endl;
	}

	int result = re.Validate(sbx::kTaeSpaendBl, operands);
	cout << "No of validation exceptions: [" << result << "]" << endl;

	return 0;
}
