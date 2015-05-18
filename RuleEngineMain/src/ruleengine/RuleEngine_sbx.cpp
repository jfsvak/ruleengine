//============================================================================
// Name        : helloworld2.cpp
// Author      : xjes
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "RuleEngine_sbx.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <set>
#include <map>
#include <sstream>
#include <stdexcept>

#include "muParser/mpError.h"
#include "muParser/mpIToken.h"
#include "muParser/mpIValue.h"
#include "muParser/mpTypes.h"
#include "muParser/mpValue.h"
#include "muParser/mpVariable.h"

#include "mubridge/utils.h"

#include "PrintUtils_sbx.h"
#include "ProductElement_sbx.h"
#include "ProductElementValue_sbx.h"
#include "ValidationResult.h"
#include "Rule.h"
#include "RuleCatalogue.h"

using namespace std;

namespace sbx {

bool RuleEngine::_printDebug {false};

void RuleEngine::initConstants(const std::vector<sbx::Constant> &allConstants)
{
	_container.initGlobalConstants(allConstants);
//	  loadRuleCatalogue();
}

void RuleEngine::initConstants(const std::string& jsonContents)
{
	_container.initConstants(jsonContents);
}

/**
 *
 */
void RuleEngine::parseRuleCatalogueJSON(const std::string& jsonContents)
{
	Json::Reader reader{Json::Features{}};
	Json::Value root;

	bool parsingSuccessful = reader.parse(jsonContents, root);

	if (parsingSuccessful)
	{
		// if we can parse the json string, then get the list of rules
		sbx::RuleCatalogue* ruleCataloguePtr = &_ruleCatalogue;
		initRuleCatalogue(ruleCataloguePtr, root["data"].get("ruleCatalogue", 0));
	}
	else
	{
		cerr << "Could not parse json string with RuleCatalogues" << endl;
		throw invalid_argument(reader.getFormattedErrorMessages());
	}
}

void RuleEngine::initRuleCatalogue(sbx::RuleCatalogue* ruleCatalogue, const Json::Value& ruleJSON)
{
	if (ruleJSON.size() > 0)
	{
		if (RuleEngine::_printDebug) { cout << "\n  Looping over [" << ruleJSON.size() << "] json RuleCatalogues to load" << endl; }

		// iterate over the list of rules and create Ruleobjects to put into the RuleConstantContainer
		for (Json::ValueIterator valueIterator = ruleJSON.begin(); valueIterator != ruleJSON.end(); ++valueIterator)
		{
			string id = valueIterator->get("id", "").asString();
			string expr = valueIterator->get("expr", "").asString();
			string requiredRule = valueIterator->get("requiredRule", "").asString();
			string positiveMessage = valueIterator->get("positiveMessage", "").asString();
			string negativeMessage = valueIterator->get("negativeMessage", "").asString();

			if (RuleEngine::_printDebug) { cout << "  Creating Rule [" << id << "], expr [" << expr << "]" << endl; }

			Rule* rule = new Rule{id, expr, (requiredRule == "parent") ? ruleCatalogue->getParent() : nullptr, positiveMessage, negativeMessage};

			Json::Value peOids = valueIterator->get("productElementOids", "");

			if (peOids.size() > 0)
			{
				// run through and set peOids on rule and add rule to peOid multimap
				for (Json::ValueIterator peOidIterator = peOids.begin(); peOidIterator != peOids.end(); ++peOidIterator)
				{
					int peOid = peOidIterator->asInt();
					rule->addProductElementOid(peOid);
					_peOidToRules.insert(make_pair(peOid, rule));
				}
			}

			rule->setPositiveRuleCatalogue(new RuleCatalogue{rule});
			rule->setNegativeRuleCatalogue(new RuleCatalogue{rule});
			ruleCatalogue->addRule(rule);

			// recursively load the positive rule catalogue
			if (RuleEngine::_printDebug) { cout << "    Initialising positive Catalogue..."; }
			this->initRuleCatalogue(rule->getPositiveRuleCatalogue(), valueIterator->get("positiveRuleCatalogue", ""));

			// recursively load the positive rule catalogue
			if (RuleEngine::_printDebug) { cout << "    Initialising Negative Catalogue...";}
			this->initRuleCatalogue(rule->getNegativeRuleCatalogue(), valueIterator->get("negativeRuleCatalogue", ""));
		}
	}
	else
	{
		if (RuleEngine::_printDebug) { cout << "Empty RuleCatalogue...nothing to load" << endl; }
	}
}

/**
 * Initialises local context that is used for following operations on RuleEngine
 */
void RuleEngine::initContext(const sbx::KonceptInfo& ki)
{
	_ki = ki;
	_container.initContext(ki.getUnderkonceptOid(), 0);

	// clear parser for variables and consts
	_parser.ClearVar();
	_parser.ClearConst();

	// init ParserX with constants for context
	for (auto& parameterIt : ki.getParameterValues()) {
		const std::set<unsigned short>& peOids = _container.getProductElementOids(parameterIt.first);

		for (auto& peOid : peOids) {
			// get min and max values and initialise parser with that
			initParserWithProductElementConstants(peOid);
		}
	}
}

void RuleEngine::initParserWithProductElementConstants(unsigned short peOid)
{
	ProductElement pe = _container.getProductElement(peOid);
	std::shared_ptr<Constant> cMin = _container.getConstant(peOid, sbx::ComparisonTypes::kMin);
	std::shared_ptr<Constant> cMax = _container.getConstant(peOid, sbx::ComparisonTypes::kMax);
	string minName = constructRCName(pe, sbx::ComparisonTypes::kMin);
	string maxName = constructRCName(pe, sbx::ComparisonTypes::kMax);
//	cout << "adding constant [" << minName << "], value [" << cMin->stringValue() << "] as [" << cMin->longValue() << "]" << endl;
//	cout << "adding constant [" << maxName << "], value [" << cMax->stringValue() << "] as [" << cMax->longValue() << "]" << endl;

	// TODO figure out how to send in function pointer, to simply the next 4 calls
	switch (pe.getElementType())
	{
	case kLong:
	case kMonth:
	{
		// get min value and set to rc_<peoidvariablename>_min
		if (!cMin->isAutoCreated()) {
			defineConstant(minName, cMin->longValue());
		}

		// get max value and set to rc_<peoidvariablename>_max
		if (!cMax->isAutoCreated()) {
			defineConstant(maxName, cMax->longValue());
		}
		break;
	}
	case kPercent:
	case kCurr:
	{
		// get min value and set to rc_<peoidvariablename>_min
		if (!cMin->isAutoCreated()) {
			defineConstant(minName, cMin->doubleValue());
		}

		// get max value and set to rc_<peoidvariablename>_max
		if (!cMax->isAutoCreated()) {
			defineConstant(maxName, cMax->doubleValue());
		}
		break;
	}
	default:
		break;
	}
}

void RuleEngine::defineVariable(const std::string& name, double value)
{
	// TODO if we redefine a variable, we should not create a new one, but get the mup::Value& and set the value inside it
	try {
		mup::Value* val = new mup::Value(value);
		_parser.DefineVar(name, mup::Variable(val));
	} catch (mup::ParserError& e) {
		cerr << "Exception defining variable : [" << e.GetCode() << "]-[" << e.GetMsg() << "]-[" << e.GetToken() << "]" << endl;
	}
}

void sbx::RuleEngine::defineVariable(const std::string& name, const std::string& value)
{
	// TODO Check c++11 validation on desktop is working
	try {
		mup::Value* val = new mup::Value(value);
		_parser.DefineVar(name, mup::Variable(val));
	} catch (mup::ParserError& e) {
		cerr << "Exception defining variable : [" << e.GetCode() << "]-[" << e.GetMsg() << "]-[" << e.GetToken() << "]" << endl;
	}
}

void RuleEngine::defineConstant(const std::string& name, double constant)
{
	try {
		_parser.DefineConst(name, constant);
	} catch (mup::ParserError& e) {
		cerr << "Exception setting constant: [" << e.GetCode() << "]-[" << e.GetMsg() << "]-[" << e.GetToken() << "]" << endl;
	}
}

/**
 * Delegate call to RuleConstantContainer::getOptionsList(...)
 */
vector<string> RuleEngine::getOptions(ProductElementOid productElement)
{
	return _container.getOptions(productElement);
}

/**
 * Delegate call to RuleConstantContainer::getOptionsList(...)
 */
const std::vector<std::shared_ptr<Constant>>& RuleEngine::getOptionsList(sbx::ProductElementOid productElement)
{
	return _container.getOptionsList(productElement);
}

/**
 * Delegate call to RuleConstantContainer::getConstant(...)
 */
std::shared_ptr<sbx::Constant> RuleEngine::getConstant(sbx::ProductElementOid productElement, sbx::ComparisonTypes comparisonType)
{
	return _container.getConstant(productElement, comparisonType);
}

/**
 * Gets the RuleConstantContainer
 */
const sbx::RuleConstantContainer& RuleEngine::getContainer() const
{
	return _container;
}

/**
 * Loads all ta values into the parser as variables.
 * When setting values, it checks to see if the value/variable is already there. If it is, the
 */
void RuleEngine::loadTAValues(const TA& ta)
{
	// initialise all values for TA into parser
	for (auto& item : ta.getValues())
	{
		const unsigned short peOid = item.first;
		const sbx::ProductElementValue& pev = item.second;
		ProductElement pe = _container.getProductElement(peOid);

		switch (pe.getElementType())
		{
		case kDate:
			// TODO: how to validate date??
			break;
		case kBool:
			// TODO: check in the constant container that the value is allowed
			break;
		case kText:
			defineVariable(pe.getVariableName(), pev.stringValue());
			break;
		case kLong:
		case kMonth:
		case kYear:
		case kCurr:
		case kPercent:
			defineVariable(pe.getVariableName(), pev.doubleValue());
			break;
		case kUnknownPEType:
			// TODO: return or throw exception? agree with iOS
			break;
		}
	}
}

/**
 * 1. initialised TA values into parser
 * 2. Find rules for the product oid in the rule catalogue (pointer into the rule catalogue)
 * 3. Validate each rule 1-by-1
 *    3.1 validate parent (if its there) to see if the product element is required or not (false on parent rule eval makes it not required, and shouldn't be there
 * 4. return the validation result with messages
 */
sbx::ValidationResult RuleEngine::validate(const TA& ta, unsigned short peOidToValidate)
{
	sbx::ValidationResult valResult{};

	// initialise all values for TA into parser
	loadTAValues(ta);

	// get rules for the product element
	const auto& range = _peOidToRules.equal_range(peOidToValidate); // std::pair <std::multimap<unsigned short, sbx::Rule*>::iterator, std::multimap<unsigned short, sbx::Rule*>::iterator> range = _peOidToRules.equal_range(peOidToValidate);

	// for each rule, validate and add messages to valResult
	for (auto it = range.first; it != range.second; it++) //	for (std::multimap<unsigned short, sbx::Rule*>::iterator it = range.first; it != range.second; it++)
	{
		sbx::Rule* rule = it->second;

		executeRule( {peOidToValidate}, rule, valResult );

		// if there is a parent rule, then validate it to check if this productelement is supposed to be here or not
		if (rule->getRequiredIfRule()) {
			executeRule(rule->getRequiredIfRule()->getProductElementOids(), rule->getRequiredIfRule(), valResult);
		}
	}

	return valResult;
}

/*
 * TODO: Implement this correct, go throuh the logic for validating an entire TA
 * Validates the generic rules for the ComparisonType
 */
int RuleEngine::validate(const sbx::KonceptInfo& konceptInfo, const sbx::TA& ta)
{
	// 1. check allowed products elements for the konceptinfo
	std::set<unsigned short, less<unsigned short>> allowedProductElementOids {};

	// insert all allowed product oids into one vector
	for (auto& parameterIt : konceptInfo.getParameterValues()) {
		const std::set<unsigned short>& productElementOids = _container.getProductElementOids(parameterIt.first);
		allowedProductElementOids.insert(productElementOids.cbegin(), productElementOids.cend());
	}

	for (auto& item : ta.getValues()) {
		// check if this product element is allowed
		const sbx::ProductElementValue& pev = item.second;
		unsigned short peOid = pev.getProductElementOid();

		if (allowedProductElementOids.find(peOid) == allowedProductElementOids.cend()) {
			cout << "Product element oid [" << peOid << "] not allowed for this konceptInfo" << endl;
		}

		try {
			sbx::ValidationResult r = this->validate(ta, peOid);
			std::vector<std::string> v = r.getValidationResults(peOid);
			ostringstream s {};
			ostream_iterator<string> strOutput(s, ", ");
			copy(v.cbegin(), v.cend(), strOutput);
			cerr << "Result of validation of pev[" << s.str() << "]" << endl;
		}
		catch (exception& e) {
			cerr << "Validation of pev failed with exception: [" << e.what() << "]" << endl;
			throw;
		}
	}

	// 2. check necessary related product element values
	//    2.a. check related values
	// 3. check min values

	// 4. check max values
	// 5. check custom rules

	return 0;
}

void RuleEngine::executeRule(const std::vector<unsigned short>& peOidsBeingValidated, sbx::Rule* rule, sbx::ValidationResult& valResult) {
	try {
		_parser.SetExpr(rule->getExpr());
		if (RuleEngine::_printDebug) { cout << "Executing rule [" << rule->getRuleId() << "]...";}
		mup::Value singleRuleResult = _parser.Eval();
		if (RuleEngine::_printDebug) { cout << "\tresult for expr[" << rule->getExpr() << "]=[" << boolalpha << singleRuleResult.GetBool() << "]" << endl;}

		if (singleRuleResult.GetBool()) {
			valResult.addValidationResult(peOidsBeingValidated, std::string {rule->getRuleId() + "-" + rule->getPositiveMessage()});
		}
		else {
			valResult.addValidationResult(peOidsBeingValidated, std::string {rule->getRuleId() + "-" + rule->getNegativeMessage()});
		}
	}
	catch (mup::ParserError& e)	{
		sbx::mubridge::handle(e, rule, peOidsBeingValidated, valResult);
	}
}

const std::shared_ptr<sbx::Constant>& RuleEngine::getDefaultValue(sbx::ProductElementOid productElement)
{
	// look up the productElement oid and see in which internal map to find the constant
	// for now just assume that the product element is found in equals/enum map
	const vector<shared_ptr<Constant>>& options = _container.getOptionsList(productElement);

	if (options.size() <= 0) {
		ostringstream s;
		s << "No default value for product element oid [" << static_cast<unsigned short>(productElement) << "]!";
		throw domain_error(s.str());
	}

	for (const auto& option : options) {
		if (option->isDefault()) {
			return option;
		}
	}

	return options.back();
}

void RuleEngine::printVariables() { _printVariables(_parser); }
void RuleEngine::printConstants() { _printConstants(_parser); }
void RuleEngine::printExpressionVariables() { _printExpressionVariables(_parser); }

void RuleEngine::_printVariables(mup::ParserX& p)
{
	cout << "------------- VARIABLES initialised BEGIN ------------" << endl;
	try {
		mup::var_maptype vmap = p.GetVar();
		for (mup::var_maptype::iterator item = vmap.begin(); item != vmap.end(); ++item) {
			std::string varName = item->first;
			mup::Variable var = (mup::Variable&) (*(item->second));
			cout << varName << "=" << var << endl;
		}
	}
	catch (mup::ParserError &e) {
		sbx::mubridge::handle(e);
	}
	cout << "------------- VARIABLES initialised END ------------\n\n";
	cout.flush();
}

void RuleEngine::_printConstants(mup::ParserX& p)
{
	cout << "------------- CONSTANTS initialised BEGIN ------------" << endl;
	try {
		mup::var_maptype vmap = p.GetConst();
		for (mup::var_maptype::iterator item = vmap.begin(); item != vmap.end(); ++item) {
			cout << item->first << "=" << (mup::Value&) (*(item->second)) << endl;
		}
	}
	catch (mup::ParserError &e) {
		sbx::mubridge::handle(e);
	}

	cout << "------------- CONSTANTS initialised END ------------\n\n";
	cout.flush();
}

sbx::RuleCatalogue& RuleEngine::getRuleCatalogue()
{
	return _ruleCatalogue;
}

void RuleEngine::_printExpressionVariables(mup::ParserX& p)
{
	cout << "------------- Expressions BEGIN ------------" << endl;
	mup::var_maptype vmap = p.GetExprVar();

	for (mup::var_maptype::iterator item = vmap.begin(); item != vmap.end(); ++item)
	{
		cout << item->first << "=";
//		mup::Variable var = (mup::Variable&) (*(item->second));
//		cout << var << "\n";
		cout << endl;
	}
	cout << "------------- Expressions END ------------" << endl;
	cout.flush();
}

void RuleEngine::printRuleCatalogue(sbx::RuleCatalogue& ruleCatalogue, int depth)
{
	depth += 2;

	for (sbx::Rule* rulePtr : ruleCatalogue.getRules())
	{
		printRule(rulePtr, depth);
		cout << _indent(depth) << "Positive case for rule id[" << rulePtr->getRuleId() << "]:" << endl;

		printRuleCatalogue(*rulePtr->getPositiveRuleCatalogue(), depth);

		cout << _indent(depth) << "Negative case for rule[" << rulePtr->getRuleId() << "]:" << endl;
		printRuleCatalogue(*rulePtr->getNegativeRuleCatalogue(), depth);
		cout << _indent(depth) << "----------\n\n";
	}
}

void RuleEngine::printRule(sbx::Rule* rule, int depth)
{
	cout << _indent(depth) << "ID=[" << rule->getRuleId() << "], expr=[" << rule->getExpr() << "], ";

	if (rule->getRequiredIfRule() != nullptr)
	{
		cout << "requiredIfRuleID=[" << rule->getRequiredIfRule()->getRuleId() << "], ";
	}

	cout << "peOids=[";
	for (auto peOid : rule->getProductElementOids())
	{
		cout << peOid << ", ";
	}

	cout << "], ";

	cout << "posMsg=[" << rule->getPositiveMessage() << "], negMsg=[" << rule->getNegativeMessage() << "]" << endl;
}

std::string RuleEngine::comparisonTypeName(const sbx::ComparisonTypes& comparisonType)
{
	switch (comparisonType)
	{
	case kMin:
		return "min";
	case kMax:
		return "max";
	case kEquals:
		return "equal";
	case kEnum:
		return "enum";
	default:
		return "unknown";
	}
}

std::string RuleEngine::constructRCName(const sbx::ProductElement& pe, const sbx::ComparisonTypes& ct)
{
	stringstream s {};
	s << "rc_" << pe.getVariableName() << "_" << RuleEngine::comparisonTypeName(ct);
	return s.str();
}

const sbx::ProductElement& RuleEngine::_pe(unsigned short peOid)
{
	// TODO fix this temp stuff
	return _container.getProductElement(peOid);
}

std::string RuleEngine::_indent(unsigned short depth)
{
	return std::string(depth, ' ');
}

} // sbx namespace end
