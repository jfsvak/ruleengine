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

#include "muParser/mpParser.h"
#include "muParser/mpTypes.h"
#include "muParser/mpValue.h"

#include "Constant_sbx.h"
#include "KonceptInfo_sbx.h"
#include "ProductElement_sbx.h"
#include "Rule.h"
#include "RuleCatalogue.h"
#include "RuleConstantContainer_sbx.h"
#include "sbxTypes.h"
#include "TA_sbx.h"
#include "ValidationResults.h"

namespace sbx {

class RuleEngine
{
public:
	static bool _printDebug;
	static const short int VALID = 1;
	static const short int INVALID = -1;
	static std::string comparisonTypeName(const sbx::ComparisonTypes& comparisonType);


	RuleEngine();
	RuleEngine(const sbx::RuleEngine&); // copy constructor. Handle proper copy of pointers in map
	virtual ~RuleEngine(); // Handler proper delete of pointers

	void initConstants(const std::vector<Constant>& globalConstants);
	void initConstants(const std::string& jsonContents);
	void initContext(const sbx::KonceptInfo& ki);
	void parseRuleCatalogueJSON(const std::string& jsonContents);

	std::vector<std::string> getOptions(sbx::ProductElementOid productElement);
	const std::vector<std::shared_ptr<Constant>>& getOptionsList(sbx::ProductElementOid productElement);
	const std::shared_ptr<sbx::Constant>& getDefaultValue(sbx::ProductElementOid productElementOid);
	std::shared_ptr<sbx::Constant> getConstant(sbx::ProductElementOid productElement, sbx::ComparisonTypes comparisonType);

	sbx::ValidationResults validate(const TA&, unsigned short peOidToValidate); // single product element validation
	sbx::ValidationResults validate(const TA&, const std::vector<unsigned short>& peOidToValidate); // multiple product element validation, using single pe validation
	int validate(const sbx::KonceptInfo&, const sbx::TA& ta); // Full TA validation

	sbx::RuleCatalogue& getRuleCatalogue();
	const sbx::RuleConstantContainer& getContainer() const;

	// -- util methods for printing
	void printRuleCatalogue(sbx::RuleCatalogue&, int depth);
	void printRule(std::shared_ptr<sbx::Rule>, int depth);
	void printVariables();
	void printConstants();
	void printExpressionVariables();

private:

	// -- initialisation methods
	void initRuleCatalogue(sbx::RuleCatalogue*, const Json::Value& ruleCatalogues);
	void initParserWithProductElementConstants(unsigned short peOid);
	template <typename T> void defineVariable(const std::string& name, const T& value);

	void defineConstant(const std::string& name, double constant);
	void executeRule(unsigned short peOidToValidate, std::shared_ptr<sbx::Rule> rule, sbx::ValidationResults& valResult);
	void loadTAValues(const TA& ta);

	sbx::ProductElement _pe(unsigned short peOid);

	sbx::KonceptInfo _ki;
	sbx::RuleConstantContainer _container;
	sbx::RuleCatalogue _ruleCatalogue;

	/**
	 * multimap of product element oids to list of rules that relates to the oid.
	 * Each peOid can point to more than one rule
	 * index: peOid
	 * value(s): sbx::Rule*
	 */
    std::multimap<unsigned short, std::shared_ptr<sbx::Rule>> _peOidToRules;


	// ParserX is initialised when the KonceptInfo has been parsed in for initialisation
	mup::ParserX _parser { mup::pckALL_NON_COMPLEX };

	/**
	 * Map of product element oids to mup::Values pointers
	 * Index: peOid
	 * Value: mup::Value*
	 */
	std::map<std::string, mup::Value*> _mupValueMap;

	// -- util methods for printing etc.
	void _printVariables(mup::ParserX& p);
	void _printExpressionVariables(mup::ParserX& p);
	void _printConstants(mup::ParserX& p);
	std::string _indent(unsigned short depth);
	std::string constructRCName(const sbx::ProductElement&, const sbx::ComparisonTypes&);
};

} // sbx namespace end

#endif // RULEENGINE_RULEENGINE_H_
