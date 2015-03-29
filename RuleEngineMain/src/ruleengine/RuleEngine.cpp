//============================================================================
// Name        : helloworld2.cpp
// Author      : xjes
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <sstream>
#include <vector>
#include <iostream>

#include "RuleEngine.h"
#include "sbxTypes.h"
#include "muparserx/mpParser.h"

using namespace std;
using namespace mup;

namespace sbx {

RuleConstant MakeRuleConstant(const ProductElement, const ComparisonTypes, string);
void PrintVariables(ParserX p);
void PrintExpressions(ParserX p);

void RuleEngine::Init() {
//	container.Init();
	LoadRuleCatalogue();
}

void RuleEngine::LoadRuleConstants(ParserX &p) {
	// load rule constants - for now, set dummy values
	ProductElement pe = {kTaeBlGrMin, "taeblgrmin", kCurr};

//	rc_cont["rc_taeblgrmin_min"] = MakeRuleConstant(pe, kMin, 100000);
//	rc_cont["rc_taeblgrmin_max"] = MakeRuleConstant(pe, kMax, 800000);
//	rc_cont["rc_taebloblmax_min"] = MakeRuleConstant(pe, kMin, 100000);
//	rc_cont["rc_taebloblmax_max"] = MakeRuleConstant(pe, kMax, 800000);

	pe = {kTaeSpaendBl, "taespaendbl", kCurr};
	rc_cont["rc_taespaendbl_max"] = MakeRuleConstant(pe, kMax, "200000");

	try {
		p.EnableAutoCreateVar(true);
		p.SetExpr("rc_taeblgrmin_min=100000");
		p.Eval();
		p.SetExpr("rc_taeblgrmin_max=800000");
		p.Eval();
		p.SetExpr("rc_taebloblmax_min=100000");
		p.Eval();
		p.SetExpr("rc_taebloblmax_max=800000");
		p.Eval();
		p.SetExpr("rc_taespaendbl_max=200000");
		p.Eval();
	} catch(ParserError &e) {
		cout << "Exception: [" << e.GetCode() << "]-[" << e.GetMsg() << "]-[" << e.GetExpr() << "]" << endl;
	}
}

void RuleEngine::LoadRuleCatalogue() {
	rule_catalogue[kTaeSpaendBl] = {"rc_taespaendbl_max >= (a-b)" };
	rule_catalogue[kTaeBlGrMin] = {"taeblgrmin <= taebloblmax" };
}

/*
 * Validates a single product element using the specific rules for that product element
 */
int RuleEngine::Validate(sbx::ProductElementNames p_element, std::vector<std::pair<std::string,long>> p_operands) {
	cout << "The Start of Validate" << endl;

	try {
		ParserX p;
		p.EnableAutoCreateVar(true);
		LoadRuleConstants(p);

		for (auto & element : p_operands) {
			stringstream sstm;
			sstm << element.first << "=" << element.second;
		    string s = sstm.str();
		    cout << "Will add [" << s << "]" << endl;
		    p.SetExpr(s);
		    Value result = p.Eval();
		    cout << "Result of adding it: [" << result << "]" << endl;
		}

		PrintVariables(p);

		int no_of_failures;

		for (auto & rule : rule_catalogue[p_element]) {
			p.SetExpr(rule);
			Value result = p.Eval();
			PrintExpressions(p);
			if (!result) {
				no_of_failures++;
			}
		}

		cout << "The End of Validate" << endl;

		return no_of_failures;
	} catch(ParserError &e) {
		cout << "Exception: [" << e.GetCode() << "]-[" << e.GetMsg() << "]-[" << e.GetExpr() << "]" << endl;
	}

	return -1;
}

/*
 * Validates the generic rules for the ComparisonType
 */
int RuleEngine::Validate(sbx::ComparisonTypes) {
	return -1;
}

vector<string> RuleEngine::GetOptionsList(sbx::ProductElementNames product_element_name) {
	return container.GetOptions(product_element_name);
}

RuleConstant MakeRuleConstant(ProductElement product_element, ComparisonTypes ct, string s) {
	RuleConstant rc = {1, 0, product_element, ct, s, true};
	return rc;
}

void PrintVariables(ParserX p) {
	cout << "------------- Variables initialised BEGIN ------------" << endl;
	var_maptype vmap = p.GetVar();
	for (var_maptype::iterator item = vmap.begin();item!=vmap.end(); ++item) {
		cout << item->first << "=" << (Variable&)(*(item->second)) << endl;
	}
	cout << "------------- Variables initialised END ------------" << endl << endl;
}

void PrintExpressions(ParserX p) {
	cout << "------------- Expressions BEGIN ------------" << endl;
	var_maptype vmap = p.GetExprVar();
	for (var_maptype::iterator item = vmap.begin(); item!=vmap.end(); ++item)
	  cout << "  " << item->first << " =  " << (Variable&)(*(item->second)) << "\n";
	cout << "------------- Expressions END ------------" << endl;
}

} // sbx namespace end
