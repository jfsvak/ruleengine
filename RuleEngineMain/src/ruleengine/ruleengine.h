//============================================================================
// Name        : helloworld2.cpp
// Author      : xjes
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#ifndef RULEENGINE_RULEENGINE_H_
#define RULEENGINE_RULEENGINE_H_

#include <string>
#include <vector>
#include <map>
#include "muparserx/mpParser.h"
#include "sbxTypes.h"

namespace sbx {

class RuleEngine {
//	mup::ParserX p;
	std::map<std::string, sbx::RuleConstant> rc_cont;
	std::map<sbx::ProductElementNames, std::vector<std::string>> rule_catalogue;
private:
	void LoadRuleConstants(mup::ParserX&);
	void LoadRuleCatalogue();
public:
	void Init();
	int Validate(sbx::ProductElementNames, std::vector<std::pair<std::string,long>>);
	int Validate(sbx::ComparisonTypes);
};

}

#endif // RULEENGINE_RULEENGINE_H_
