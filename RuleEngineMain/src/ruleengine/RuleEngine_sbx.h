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
#include <set>
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
	static bool _printDebugAtValidation;

	RuleEngine();
	RuleEngine(const sbx::RuleEngine&); // copy constructor. Handle proper copy of pointers in map
	virtual ~RuleEngine(); // Handler proper delete of pointers

	void initConstants(const std::vector<Constant>& globalConstants);
	void initConstants(const std::string& jsonContents);
	void initContext(const sbx::KonceptInfo& ki);

	void parseRuleCatalogueJSON(const std::string& jsonContents);

	std::vector<std::string> getOptions(sbx::ProductElementOid productElement);
	std::vector<std::shared_ptr<Constant>> getOptionsList(sbx::ProductElementOid productElement);
	std::shared_ptr<sbx::Constant> getDefaultValue(sbx::ProductElementOid productElementOid);
	std::shared_ptr<sbx::Constant> getConstant(sbx::ProductElementOid productElement, sbx::ComparisonTypes comparisonType);

	sbx::ValidationResults validate(const TA&, unsigned short peOidToValidate); // simple delegate method to vector-method
	sbx::ValidationResults validate(const TA&, const std::vector<unsigned short>& peOidToValidate); // multiple product element validation, using single pe validation

	// Full ta validation
	sbx::ValidationResults validate(const sbx::TA& ta, bool full = true); // Full TA validation

	sbx::RuleCatalogue& getRuleCatalogue();
	const sbx::RuleConstantContainer& getContainer() const;

	// -- util methods for printing
	void printRuleCatalogue(sbx::RuleCatalogue&, int depth);
	void printRule(std::shared_ptr<sbx::Rule>, int depth);
	void printVariablesInParser();
	void printConstantsInParser();
	void printExpressionVariables();

private:

	// Method to validate the value of a product element
	void _validateValue(const sbx::ProductElementValue&, sbx::ValidationResults&);


	//
	// Methods for pure value validations
	//
	void _validateMinMax(const sbx::ProductElementValue&, sbx::ValidationResults&);
	void _validateOptionAllowed(const sbx::ProductElementValue&, sbx::ValidationResults&);
	//   checks if the supplied value is an allowed option (found in the allowed list in the rule constant container
	bool _isOptionAllowed(const sbx::ProductElementValue&);


	//
	// Methods for related pe validations/checks
	//
	void _validateCustomRules(unsigned short peOid, sbx::ValidationResults&);
	// checks if the product element is required to be the TA for the context and other values on ta's. Uses custom rules to validate this requiredness
	bool _isRequired(unsigned short peOid, sbx::ValidationResults&, bool fullValidation = false);
	bool _isOptional(unsigned short peOid, sbx::ValidationResults&);

	void _executeRule(unsigned short peOidToValidate, std::shared_ptr<sbx::Rule>, sbx::ValidationResults&, sbx::ValidationCode negativeValCode = sbx::ValidationCode::kFail);
	void _executeRequiredIfRule(unsigned short peOidToValidate, std::shared_ptr<sbx::Rule>, sbx::ValidationResults&);
	mup::Value _execute(const std::string& expr, const std::string& ruleId);
	void _executePreCalcRules();

	// -- initialisation methods
	void _initRuleCatalogue(sbx::RuleCatalogue*, const Json::Value& ruleCatalogues);
	void _initParserWithProductElementConstants(unsigned short peOid);
	template <typename T> void _defineVariable(const std::string& name, const T& value);
	void _clearContext();

	std::set<unsigned short, std::less<unsigned short>> _getAllowedPEOids();

	void _defineConstant(const std::string& name, double constant);

	void _loadParser(const TA& ta);

	sbx::ProductElement _PE(unsigned short peOid);
	std::string _VAR_NAME(unsigned short peOid);

	sbx::KonceptInfo _ki;
	sbx::RuleConstantContainer _container;
	sbx::RuleCatalogue _ruleCatalogue;
	std::multimap<unsigned short, std::shared_ptr<sbx::Rule>> _preCalcExprMap;

	/**
	 * multimap of product element oids to list of rules that relates to the oid.
	 * Each peOid can point to more than one rule
	 * index: peOid
	 * value(s): sbx::Rule*
	 */
    std::multimap<unsigned short, std::shared_ptr<sbx::Rule>> _peOidToRules;


	// ParserX is initialised when the KonceptInfo has been parsed in for initialisation
	mup::ParserX _parser { mup::pckALL_NON_COMPLEX };
	bool _refreshParserValues = true;

	/**
	 * Map of product element oids to mup::Values pointers
	 * Index: peOid
	 * Value: mup::Value*
	 */
	std::map<std::string, std::shared_ptr<mup::Value>> _mupValueMap;

	// -- util methods for printing etc.
	void _printVariablesInParser(mup::ParserX& p);
	void _printExpressionVariables(mup::ParserX& p);
	void _printConstantsInParser(mup::ParserX& p);
	std::string _indent(unsigned short depth);
};

} // sbx namespace end

#endif // RULEENGINE_RULEENGINE_H_
