//============================================================================
// Name        : helloworld2.cpp
// Author      : xjes
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#ifndef RULEENGINE_RULEENGINE_H_
#define RULEENGINE_RULEENGINE_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../json/json.h"
#include "Constant_sbx.h"
#include "KonceptInfo_sbx.h"
#include "muParser/mpParser.h"
#include "muParser/mpTypes.h"
#include "ProductElement_sbx.h"
#include "Rule.h"
#include "RuleCatalogue.h"
#include "RuleConstantContainer_sbx.h"
#include "sbxTypes.h"
#include "TA_sbx.h"
#include "ValidationResult.h"

namespace sbx {

class RuleEngine
{
public:
	static bool _printDebug;
	static const short int VALID = 1;
	static const short int INVALID = -1;
	static std::string comparisonTypeName(const sbx::ComparisonTypes& comparisonType);

	void initConstants(const std::vector<Constant>& globalConstants);
	void initConstants(const std::string& jsonContents);
	void initContext(const sbx::KonceptInfo& ki);
	void parseRuleCatalogueJSON(const std::string& jsonContents);

	std::vector<std::string> getOptions(sbx::ProductElementOid productElement);
	const std::vector<std::shared_ptr<Constant>>& getOptionsList(sbx::ProductElementOid productElement);
	const std::shared_ptr<sbx::Constant>& getDefaultValue(sbx::ProductElementOid productElementOid);
	std::shared_ptr<sbx::Constant> getConstant(sbx::ProductElementOid productElement, sbx::ComparisonTypes comparisonType);

	sbx::ValidationResult validate(const TA&, unsigned short peOidToValidate); // single product element validation
	int validate(const sbx::KonceptInfo&, const sbx::TA& ta); // Full TA validation

	sbx::RuleCatalogue& getRuleCatalogue();
	const sbx::RuleConstantContainer& getContainer() const;

	// -- util methods for printing
	void printRuleCatalogue(sbx::RuleCatalogue&, int depth);
	void printRule(sbx::Rule*, int depth);
	void printVariables();
	void printConstants();
	void printExpressionVariables();

private:

	// -- initialisation methods
	void initRuleCatalogue(sbx::RuleCatalogue*, const Json::Value& ruleCatalogues);
	void initParserWithProductElementConstants(unsigned short peOid);
	void defineVariable(const std::string& name, double value);
	void defineVariable(const std::string& name, const std::string& value);
	void defineConstant(const std::string& name, double constant);
	void executeRule(const std::vector<unsigned short>& peOidToValidate, sbx::Rule* rule, sbx::ValidationResult& valResult);

	const sbx::ProductElement& _pe(unsigned short peOid);

	sbx::KonceptInfo _ki;
	sbx::RuleConstantContainer _container;

	std::map<sbx::ProductElementOid, std::vector<std::string>> _ruleMap;
	sbx::RuleCatalogue _ruleCatalogue;

	/**
	 * multimap of product element oids to list of rules that relates to the oid.
	 * Each peOid can point to more than one rule
	 * index: peOid
	 * value(s): sbx::Rule*
	 */
	std::multimap<unsigned short, sbx::Rule*> _peOidToRules;

	std::string constructRCName(const sbx::ProductElement&, const sbx::ComparisonTypes&);

	// ParserX is initialised when the KonceptInfo has been parsed in for initialisation
	mup::ParserX _parser { mup::pckALL_NON_COMPLEX };

	// -- util methods for printing etc.
	void _printVariables(mup::ParserX& p);
	void _printExpressionVariables(mup::ParserX& p);
	void _printConstants(mup::ParserX& p);
	std::string _indent(unsigned short depth);
	void loadTAValues(const TA& ta);
};

} // sbx namespace end

#endif // RULEENGINE_RULEENGINE_H_
