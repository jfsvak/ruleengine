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

#include "mubridge/utils.h"

#include "muParser/mpError.h"
#include "muParser/mpIToken.h"
#include "muParser/mpIValue.h"
#include "muParser/mpTypes.h"
#include "muParser/mpValue.h"
#include "muParser/mpVariable.h"

#include "PrintUtils_sbx.h"
#include "ProductElement_sbx.h"
#include "ProductElementValue_sbx.h"
#include "Rule.h"
#include "RuleCatalogue.h"
#include "Utils.h"
#include "ValidationResults.h"

using namespace std;

namespace sbx {

bool RuleEngine::_printDebug {false};
bool RuleEngine::_printDebugAtValidation {false};

sbx::RuleEngine::RuleEngine()
{}

sbx::RuleEngine::RuleEngine(const sbx::RuleEngine& other)
		: _container {other._container}
{
	// TODO proper copy/cloning of pointers in map
}

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

			std::shared_ptr<sbx::Rule> rule = make_shared<sbx::Rule>(id, expr, (requiredRule == "#parent#") ? ruleCatalogue->getParent() : nullptr, positiveMessage, negativeMessage);

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
	// first clear any previous context
	this->clearContext();

	_ki = ki;
	_container.initContext(ki.getUnderkonceptOid(), 0);

	// init ParserX with constants for context by going through each parameter and get the product elements for that parameter
	for (auto& parameterIt : ki.getParameterValues()) {
		const std::set<unsigned short>& peOids = _container.getProductElementOids(parameterIt.first);

		for (auto& peOid : peOids) {
			// get min and max values and initialise parser with that
			initParserWithProductElementConstants(peOid);
		}
	}
}

void RuleEngine::clearContext()
{
	// clear parser for variables and consts
	_parser.ClearVar();
	_parser.ClearConst();
	_mupValueMap.clear();
}

void RuleEngine::initParserWithProductElementConstants(unsigned short peOid)
{
	const sbx::ProductElement pe = _container.getProductElement(peOid);
	std::shared_ptr<Constant> cMin = _container.getConstant(peOid, sbx::ComparisonTypes::kMin);
	std::shared_ptr<Constant> cMax = _container.getConstant(peOid, sbx::ComparisonTypes::kMax);
	string minName = sbx::utils::constructRCName(pe, sbx::ComparisonTypes::kMin);
	string maxName = sbx::utils::constructRCName(pe, sbx::ComparisonTypes::kMax);
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

template <typename T>
void RuleEngine::defineVariable(const std::string& name, const T& value)
{
	try {
		// if the value is already in the map, then just update the value
		if (_mupValueMap.find(name) != _mupValueMap.cend()) {
			*_mupValueMap.at(name) = value;
		}
		else {
			// if not already there, create and insert a new value
			_mupValueMap.insert( std::pair<std::string, std::shared_ptr<mup::Value>> (name, make_shared<mup::Value>(value)));
		}

		// if the variable has not been defined in the parser, define it, otherwise just leave it
		if (!_parser.IsVarDefined(name)) {
			_parser.DefineVar(name, mup::Variable(_mupValueMap[name].get()));
		}
	} catch (mup::ParserError& e) {
		sbx::mubridge::handle(e);
	}
}

void RuleEngine::defineConstant(const std::string& name, double constant)
{
	try {
		// if const was already define, remove first to set new value
		if (_parser.IsConstDefined(name)) {
			_parser.RemoveConst(name);
		}

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
 * When setting values, it checks to see if the value/variable is already there. If it is, then the value is just updated
 */
void RuleEngine::loadTAValues(const TA& ta)
{
	// initialise all values for TA into parser
	for (auto& item : ta.getValues())
	{
		const sbx::ProductElementValue& pev = item.second;
		const sbx::ProductElement pe = _container.getProductElement(item.first);

		switch (pe.getElementType())
		{
		case kDate:
			// TODO: how to validate date??
			break;
		case kBool:
			// TODO how to handle too?
			defineVariable<bool>(pe.getVariableName(), pev.boolValue());
			break;
		case kText:
			defineVariable<std::string>(pe.getVariableName(), pev.stringValue());
			break;
		case kLong: // fall throughs down to double
		case kMonth:
		case kYear:
		case kCurr:
		case kPercent:
			defineVariable<double>(pe.getVariableName(), pev.doubleValue());
			break;
		case kUnknownPEType:
			// TODO: return or throw exception? agree with iOS
			break;
		}
	}
}

sbx::ValidationResults RuleEngine::validate(const TA& ta, unsigned short peOidToValidate)
{
	return this->validate(ta, std::vector<unsigned short> {peOidToValidate});
}

/**
 * 1. initialised TA values into parser
 * 2. Find rules for the product oid in the rule catalogue (pointer into the rule catalogue)
 * 3. Validate each rule 1-by-1
 *    3.1 validate parent (if its there) to see if the product element is required or not (false on parent rule eval makes it not required, and shouldn't be there
 * 4. return the validation result with messages
 */
sbx::ValidationResults RuleEngine::validate(const TA& ta, const std::vector<unsigned short>& peOidsToValidate)
{
	sbx::ValidationResults valResult{};

	// initialise all values for TA into parser
	loadTAValues(ta);

	if (RuleEngine::_printDebugAtValidation) printVariablesInParser();

	for (auto& peOid : peOidsToValidate) {

		switch(_PE(peOid).getElementType()) {
		case kDate:
			// TODO how to validate?
			valResult.addWarning( {sbx::ValidationCode::kWarning, peOid, _VAR_NAME(peOid), "", "Validation of DATE's not implemented"} );
			break;
		case kBool:  		// fall through
		case kText:
		{
			if ( !this->isValueAllowed(ta.getValue(peOid)) )
			{
				valResult.addValidationResult( {sbx::ValidationCode::kValueNotAllowed, peOid, _VAR_NAME(peOid), "", "Value [" + ta.getValue(peOid).stringValue() + "] not allowed"} );
			}
			break;
		}
		case kMonth: 		// fall through
		case kYear: 		// fall through
		case kPercent:		// fall through
		case kCurr:			// fall through
		case kLong:
		{
			// check to see if we have this rule constant as enum/equals
			if (_container.existsAs(peOid, sbx::ComparisonTypes::kEnum))
			{
				// if it exists as an enum/equals constant, then check if the value is allowed
				if ( !this->isValueAllowed(ta.getValue(peOid)) )
				{
					valResult.addValidationResult( {sbx::ValidationCode::kValueNotAllowed, peOid, _VAR_NAME(peOid), "", "Value not allowed"} );
				}
			}

			// Generic validation of min/max values
			valResult.merge(validateMinMax(ta.getValue(peOid)));

			// if there are specific rules to run, then run them
			if (_peOidToRules.find(peOid) != _peOidToRules.end())
			{
				const auto& rangeOfRules = _peOidToRules.equal_range(peOid); // std::pair <std::multimap<unsigned short, sbx::Rule*>::iterator, std::multimap<unsigned short, sbx::Rule*>::iterator> range = _peOidToRules.equal_range(peOidToValidate);

				// for each rule, validate and add messages to valResult
				for (auto it = rangeOfRules.first; it != rangeOfRules.second; it++) //	for (std::multimap<unsigned short, sbx::Rule*>::iterator it = range.first; it != range.second; it++)
				{
					std::shared_ptr<sbx::Rule> rule = it->second;

					if (rule->getExpr() != "")
						executeRule( peOid, rule, valResult );

					// if there is a parent rule, then validate it to check if this productelement is supposed to be here or not
					if ((rule->getRequiredIfRule() != nullptr) && rule->getRequiredIfRule()->getExpr() != "")
						executeRule(peOid, rule->getRequiredIfRule(), valResult);
				}
			}
			else
			{
				// if no rules to run, then add a warning
				valResult.addWarning( {sbx::ValidationCode::kWarning, peOid, _container.getProductElement(peOid).getVariableName(), "", "No rules to run for this ProductElement"} );
			}

			break;
		}
		case kUnknownPEType:	// fall through
		default:
			valResult.addWarning( {sbx::ValidationCode::kWarning, peOid, _VAR_NAME(peOid), "", "Unknown PE Type"} );
			break;
		}
	}

	return valResult;
}

/*
 * Validates the entire TA
 */
sbx::ValidationResults RuleEngine::validate(const sbx::TA& ta, bool partial)
{
	sbx::ValidationResults valResults {};

	// 1. check allowed products elements for the konceptinfo
	std::set<unsigned short, less<unsigned short>> allowedProductElementOids {};

	// insert all allowed product oids into one vector
	for (auto& parameterIt : _ki.getParameterValues()) {
		const std::set<unsigned short>& productElementOids = _container.getProductElementOids(parameterIt.first);
		allowedProductElementOids.insert(productElementOids.cbegin(), productElementOids.cend());
	}

	// validate the available values for the TA using the rules from rules catalogue
	for (auto& item : ta.getValues()) {
		// check if this product element is allowed
		const sbx::ProductElementValue& pev = item.second;
		unsigned short peOid = pev.getProductElementOid();

		// if product element was not found in the allowed list
		if (allowedProductElementOids.find(peOid) == allowedProductElementOids.cend()) {
			stringstream msg {};
			msg << "Product element oid [" << peOid << "] not allowed for this konceptInfo";
			valResults.addValidationResult( sbx::ValidationResult {sbx::ValidationCode::kValueNotAllowed, peOid, _VAR_NAME(peOid), "", msg.str()} );
		}
		else {
			// if element is allowed, then validate it and merge the results into valResults
			valResults.merge(this->validate(ta, {peOid}));
		}
	}

	if (!partial) {
		// run through all the required product elements and add a ValidationResult to valResult
		for (auto& peOid : allowedProductElementOids)
		{
			if (!ta.hasValue(peOid)) {
				valResults.addValidationResult( sbx::ValidationResult {sbx::ValidationCode::kValueMissing, peOid, _VAR_NAME(peOid), "", "Value is missing"} );
			}
		}
	}

	// 2. check necessary related product element values
	//    2.a. check related values
	// 3. check min values

	// 4. check max values
	// 5. check custom rules

	return valResults;
}

sbx::ValidationResults RuleEngine::validateMinMax(const sbx::ProductElementValue& pev)
{
	sbx::ValidationResults r {};
	unsigned short peOid = pev.getProductElementOid();
	std::string expr = sbx::utils::constructMinMaxExpr(_PE(peOid), sbx::ComparisonTypes::kMin);

	if (RuleEngine::_printDebugAtValidation) cout << "Min expr: " << expr << endl;

	_parser.SetExpr(expr);
	mup::Value result = _parser.Eval();

	if (!result.GetBool()) {
		// value was not greater or equal to min
		r.addValidationResult( {sbx::ValidationCode::kValueTooSmall, peOid, _VAR_NAME(peOid), "", "Value too small", expr} );
	}

	expr = sbx::utils::constructMinMaxExpr(_PE(peOid), sbx::ComparisonTypes::kMax);

	if (RuleEngine::_printDebugAtValidation) cout << "Max expr: " << expr << endl;

	_parser.SetExpr(expr);
	result = _parser.Eval();

	if (!result.GetBool()) {
		// value was not lesser or equal to max
		r.addValidationResult( {sbx::ValidationCode::kValueTooBig, peOid, _VAR_NAME(peOid), "", "Value too big", expr} );
	}

	return r;
}

bool RuleEngine::isValueRequired(const sbx::TA&, unsigned short peOid)
{
	// TODO implement this
	return false;
}

bool RuleEngine::isValueAllowed(const sbx::ProductElementValue& pev)
{
	for (auto& constant : _container.getOptionsList(pev.getProductElementOid()))
	{
		switch(_PE(pev.getProductElementOid()).getElementType())
		{
		case kBool:
			if (constant->boolValue() == pev.boolValue())
				return true;
			break;

		default:
			if (constant->stringValue() == pev.stringValue())
				return true;
		}
	}

	return false;
}

void RuleEngine::executeRule(unsigned short peOidBeingValidated, std::shared_ptr<sbx::Rule> rule, sbx::ValidationResults& valResult) {
	try {
		_parser.SetExpr(rule->getExpr());

		if (RuleEngine::_printDebugAtValidation) { cout << "Executing rule [" << rule->getRuleId() << "]...";}

		mup::Value singleRuleResult = _parser.Eval();

		if (RuleEngine::_printDebugAtValidation) { cout << "\tresult for expr[" << rule->getExpr() << "]=[" << boolalpha << singleRuleResult.GetBool() << "]" << endl;}

		if (singleRuleResult.GetBool()) {
			// only add message, if it's not empty
			if (rule->getPositiveMessage() != "") {
				valResult.addValidationResult( {sbx::ValidationCode::kOK, peOidBeingValidated, _VAR_NAME(peOidBeingValidated), rule->getRuleId(), rule->getPositiveMessage(), rule->getExpr()} );
			}
		}
		else {
			valResult.addValidationResult( {sbx::ValidationCode::kFail, peOidBeingValidated, _VAR_NAME(peOidBeingValidated), rule->getRuleId(), rule->getNegativeMessage(), rule->getExpr()} );
		}
	}
	catch (mup::ParserError& e)	{
		sbx::mubridge::handle(e);
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

void RuleEngine::printVariablesInParser() { _printVariablesInParser(_parser); }
void RuleEngine::printConstantsInParser() { _printConstantsInParser(_parser); }
void RuleEngine::printExpressionVariables() { _printExpressionVariables(_parser); }

void RuleEngine::_printVariablesInParser(mup::ParserX& p)
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

void RuleEngine::_printConstantsInParser(mup::ParserX& p)
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

	for (std::shared_ptr<sbx::Rule> rulePtr : ruleCatalogue.getRules())
	{
		printRule(rulePtr, depth);
		cout << _indent(depth) << "Positive case for rule id[" << rulePtr->getRuleId() << "]:" << endl;

		printRuleCatalogue(*rulePtr->getPositiveRuleCatalogue(), depth);

		cout << _indent(depth) << "Negative case for rule[" << rulePtr->getRuleId() << "]:" << endl;
		printRuleCatalogue(*rulePtr->getNegativeRuleCatalogue(), depth);
		cout << _indent(depth) << "----------\n\n";
	}
}

void RuleEngine::printRule(std::shared_ptr<sbx::Rule> rule, int depth)
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

sbx::ProductElement RuleEngine::_PE(unsigned short peOid)
{
	try {
		return _container.getProductElement(peOid);
	} catch (domain_error& e) {
		// unsigned short oid, const std::string& variableName, const std::string& guiName, sbx::ProductElementTypes productElementType, unsigned short productOid
		return sbx::ProductElement(peOid, "<unknown>", "<unknown>", sbx::ProductElementTypes::kUnknownPEType, 0);
	}
}

std::string RuleEngine::_VAR_NAME(unsigned short peOid)
{
	try {
		return _container.getProductElement(peOid).getVariableName();
	} catch (domain_error& e) {
		return std::string ("");
	}
}

std::string RuleEngine::_indent(unsigned short depth)
{
	return std::string(depth, ' ');
}

sbx::RuleEngine::~RuleEngine()
{
//	for (auto it = _peOidToRules.end(); it != _peOidToRules.begin(); it--) {
//		delete it->second;
//		_peOidToRules.erase(it->first);
//	}

//	for (auto it = _mupValueMap.begin();it != _mupValueMap.end(); it++) {
//		delete it->second;
//		_mupValueMap.erase(it->first);
//	}

}

} // sbx namespace end
