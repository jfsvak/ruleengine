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
	// TODO !!!!! proper copy/cloning of pointers in map
}

void RuleEngine::initConstants(const std::vector<sbx::Constant> &allConstants)
{
	_container.initGlobalConstants(allConstants);
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
	Json::Value root {};

	bool parsingSuccessful = reader.parse(jsonContents, root);

	if (parsingSuccessful)
	{
		// if we can parse the json string, then get the list of rules
		sbx::RuleCatalogue* ruleCataloguePtr = &_ruleCatalogue;
		_initRuleCatalogue(ruleCataloguePtr, root["data"].get("ruleCatalogue", 0));
	}
	else
	{
		cerr << "Could not parse json string with RuleCatalogues" << endl;
		throw invalid_argument(reader.getFormattedErrorMessages());
	}
}

/**
 * Recursive initialiser method of the custom rules catalogue.
 * Is recursively parsing in pointers to nested ruleCatalogues loaded from
 */
void RuleEngine::_initRuleCatalogue(sbx::RuleCatalogue* ruleCatalogue, const Json::Value& ruleJSON)
{
	if (ruleJSON.size() > 0)
	{
		if (RuleEngine::_printDebug) { cout << "\n  Looping over [" << ruleJSON.size() << "] json RuleCatalogues to load" << endl; }

		// iterate over the list of rules and create Ruleobjects to put into the RuleConstantContainer
		for (Json::ValueIterator valueIterator = ruleJSON.begin(); valueIterator != ruleJSON.end(); ++valueIterator)
		{
			string id = valueIterator->get("id", "").asString();
			string expr = valueIterator->get("expr", "").asString();
			string preCalcExpr = valueIterator->get("preCalcExpr", "").asString();
			string requiredif = valueIterator->get("requiredif", "").asString();
			string positiveMessage = valueIterator->get("positiveMessage", "").asString();
			string negativeMessage = valueIterator->get("negativeMessage", "").asString();

			if (RuleEngine::_printDebug) { cout << "  Creating Rule [" << id << "], expr [" << expr << "]" << endl; }

			// TODO implement handling of requiredif != #parent#

			std::shared_ptr<sbx::Rule> parent;


			if (requiredif == "#parent#")
				parent = ruleCatalogue->getParent();
			else
				parent = make_shared<sbx::Rule>(id + "p", requiredif, nullptr, "", "");

			std::shared_ptr<sbx::Rule> rule = make_shared<sbx::Rule>(id, expr, parent, positiveMessage, negativeMessage, preCalcExpr);

			Json::Value requiredPEOids = valueIterator->get("preCalcRequiredPEOids", "");

			if (requiredPEOids.size() > 0)
				for (Json::ValueIterator requiredPEOidIterator = requiredPEOids.begin(); requiredPEOidIterator != requiredPEOids.end(); ++requiredPEOidIterator)
					rule->addPreCalcRequiredPEOid(requiredPEOidIterator->asInt());

			Json::Value peOids = valueIterator->get("productElementOids", "");

			if (peOids.size() > 0)
			{
				// run through and set peOids on rule and add rule to peOid multimap or preCalcExprMap
				for (Json::ValueIterator peOidIterator = peOids.begin(); peOidIterator != peOids.end(); ++peOidIterator)
				{
					int peOid = peOidIterator->asInt();
					rule->addProductElementOid(peOid);
					_peOidToRules.insert(make_pair(peOid, rule));

					if (preCalcExpr != "")
						_preCalcExprMap.insert( make_pair(peOid, rule) );
				}
			}

			rule->setPositiveRuleCatalogue(new RuleCatalogue{rule});
			rule->setNegativeRuleCatalogue(new RuleCatalogue{rule});
			ruleCatalogue->addRule(rule);

			// recursively load the positive rule catalogue
			if (RuleEngine::_printDebug) { cout << "    Initialising positive Catalogue..."; }
			this->_initRuleCatalogue(rule->getPositiveRuleCatalogue(), valueIterator->get("positiveRuleCatalogue", ""));

			// recursively load the positive rule catalogue
			if (RuleEngine::_printDebug) { cout << "    Initialising Negative Catalogue...";}
			this->_initRuleCatalogue(rule->getNegativeRuleCatalogue(), valueIterator->get("negativeRuleCatalogue", ""));
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
	this->_clearContext();

	_ki = ki;
	_container.initContext(ki.getUnderkonceptOid(), 0);

	// init ParserX with constants for context by going through each parameter and get the product elements for that parameter
	for (auto& parameterIt : ki.getParameterValues()) {
		const std::set<unsigned short>& peOids = _container.getProductElementOids(parameterIt.first);

		for (auto& peOid : peOids) {
			// get min and max values and initialise parser with that
			_initParserWithProductElementConstants(peOid);
		}
	}
}

void RuleEngine::_clearContext()
{
	// clear parser for variables and consts
	_parser.ClearVar();
	_parser.ClearConst();
	_mupValueMap.clear();
}

void RuleEngine::_initParserWithProductElementConstants(unsigned short peOid)
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
			_defineConstant(minName, cMin->longValue());
		}

		// get max value and set to rc_<peoidvariablename>_max
		if (!cMax->isAutoCreated()) {
			_defineConstant(maxName, cMax->longValue());
		}
		break;
	}
	case kPercent:
	case kCurr:
	{
		// get min value and set to rc_<peoidvariablename>_min
		if (!cMin->isAutoCreated()) {
			_defineConstant(minName, cMin->doubleValue());
		}

		// get max value and set to rc_<peoidvariablename>_max
		if (!cMax->isAutoCreated()) {
			_defineConstant(maxName, cMax->doubleValue());
		}
		break;
	}
	default:
		break;
	}
}

void RuleEngine::_executePreCalcRules()
{
	// for each precalc rule execute it
	for (auto ruleIt = _preCalcExprMap.cbegin(); ruleIt != _preCalcExprMap.cend(); ruleIt++)
	{
		bool missingPE {false};

		for (auto& requiredPEOid : ruleIt->second->getPreCalcRequiredPEOids()) {
			if (!_parser.IsVarDefined(_VAR_NAME(requiredPEOid)) )
				missingPE = true;
		}

		// if no pe oids were missing, we continue to do precalc
		if (!missingPE) {
			try {
				_parser.EnableAutoCreateVar(true);
				_execute(ruleIt->second->getPreCalcExpr(), ruleIt->second->getRuleId());
				_parser.EnableAutoCreateVar(false);
			} catch (const mup::ParserError& e) {
				// already handled inside execute
			}
		}
	}
}

template <typename T>
void RuleEngine::_defineVariable(const std::string& name, const T& value)
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
	} catch (const mup::ParserError& e) {
		sbx::mubridge::handle(e);
	}
}

void RuleEngine::_defineConstant(const std::string& name, double constant)
{
	try {
		// if const was already define, remove first to set new value
		if (_parser.IsConstDefined(name)) {
			_parser.RemoveConst(name);
		}

		_parser.DefineConst(name, constant);
	} catch (const mup::ParserError& e) {
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
void RuleEngine::_loadParser(const TA& ta)
{
	// clear all vars in parser, and
	_parser.ClearVar();

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
			_defineVariable<bool>(pe.getVariableName(), pev.boolValue());
			break;
		case kText:
			_defineVariable<std::string>(pe.getVariableName(), pev.stringValue());
			break;
		case kLong: // fall throughs down to double
		case kMonth:
		case kYear:
		case kCurr:
		case kPercent:
			_defineVariable<double>(pe.getVariableName(), pev.doubleValue());
			break;
		case kUnknownPEType:
			// TODO: return or throw exception? agree with iOS
			break;
		}
	}

	// execute all precalc rules to update the parser
	_executePreCalcRules();
}

/**
 * See sbx::ValidationResults RuleEngine::validate(const TA& ta, const std::vector<unsigned short>& peOidsToValidate)
 */
sbx::ValidationResults RuleEngine::validate(const TA& ta, unsigned short peOidToValidate)
{
	return this->validate(ta, std::vector<unsigned short> {peOidToValidate});
}

/**
 * Validates a list of pe's
 *
 * Validations carried out are full validation of each peOid:
 *   1. pure value validation
 *   	- eg. min/max or "option allowed"
 *   2. custom rules from rule catalogue validation
 *     - custom rule expressions for the pe
 *     - run any requireif rules
 */
sbx::ValidationResults RuleEngine::validate(const TA& ta, const std::vector<unsigned short>& peOidsToValidate)
{
	sbx::ValidationResults valResults{};

	// Check to see if we need to refresh parser values before validating
	if (_refreshParserValues)
		_loadParser(ta);

	if (RuleEngine::_printDebugAtValidation) printVariablesInParser();

	for (auto& peOid : peOidsToValidate) {

		// If the value about to be validated is on the TA, then do value validation
		if ( ta.hasValue(peOid) )
		{
			// Validate value of the pe using constant container for option-allowed checking and dynamic rule expr's in muParser for min/max validation
			_validateValue(ta.getValue(peOid), valResults);
		}
		else if ( !_parser.IsVarDefined(_VAR_NAME(peOid)) ) {
			// If the variable is not defined in the parser, then check if it's required by running required rules. If it's required and missing, then add msg
			if ( _isRequired(peOid, valResults) )
				valResults.addWarning( sbx::ValidationResult(sbx::ValidationCode::kProductElementNotDefined, peOid, _VAR_NAME(peOid), "Value not found on TA") );
		}

		// Even if the value doesn't exists on the TA, we still run custom rules, as there might be
		//   custom calculations to run and validate, such as DoedSpaendPct etc.
		// Validate custom rules uses muParser and rules loaded from rule catalogue. It also runs requiredif-rule
		//   It's safe to run the requiredif rules again, in addition to the above _isRequired(),
		//   since we in the below case of _validateCustomRules assume that the values are on the TA.
		//   If they are not, the requiredif rule will not evaluate but throw an exception and will be handled as a warning,
		//   not a validation error
		_validateCustomRules(peOid, valResults);
	}

	return valResults;
}

/**
 * Validates the value of one product element.
 *
 * Does not look into related product element values, and does not consider whether the peValue is allowed on this TA or not.
 */
void RuleEngine::_validateValue(const sbx::ProductElementValue& peValue, sbx::ValidationResults& valResult)
{
	unsigned short int peOid = peValue.getProductElementOid();

	switch (_PE(peOid).getElementType())
	{
	case kDate:
		// TODO figure out how to validate kDate?
		valResult.addWarning( sbx::ValidationResult(sbx::ValidationCode::kWarning, peOid, _VAR_NAME(peOid), "Validation of DATE's not implemented") );
		break;
	case kBool: // fall through
	case kText:
		// validate booleans and texts as "option allowed"
		_validateOptionAllowed(peValue, valResult);
		break;
	case kMonth: // fall through
	case kYear: // fall through
	case kPercent: // fall through
	case kCurr: // fall through
	case kLong:
	{
		// Check to see if we have this rule constant as enum/equals, then we do "option allowed" check
		if (_container.existsAs(peOid, sbx::ComparisonTypes::kEnum))
			_validateOptionAllowed(peValue, valResult);
		else
			// If the constant is not a enum/equals we do generic min/max validation
			_validateMinMax(peValue, valResult);
		break;
	}
	case kUnknownPEType: // fall through
	default:
		valResult.addWarning( sbx::ValidationResult(sbx::ValidationCode::kWarning, peOid, _VAR_NAME(peOid), "Unknown PE Type") );
		break;
	}
}

void RuleEngine::_validateMinMax(const sbx::ProductElementValue& pev, sbx::ValidationResults& r)
{
	unsigned short peOid = pev.getProductElementOid();
	std::string expr = sbx::utils::constructMinMaxExpr(_PE(peOid), sbx::ComparisonTypes::kMin);

	try {
		mup::Value result = _execute(expr, "Min-Expr");

		if (!result.GetBool()) {
			// value was not greater or equal to min
			r.addValidationResult( sbx::ValidationResult(sbx::ValidationCode::kValueUnderLimit, peOid, _VAR_NAME(peOid), "Value is under the limit", "", expr) );
		}
	} catch (const mup::ParserError& e) {
		sbx::mubridge::handle(e, r, _container);
	}

	expr = sbx::utils::constructMinMaxExpr(_PE(peOid), sbx::ComparisonTypes::kMax);

	try {
		mup::Value result = _execute(expr, "Max-expr");

		if (!result.GetBool()) {
			// value was not lesser or equal to max
			r.addValidationResult( sbx::ValidationResult(sbx::ValidationCode::kValueOverLimit, peOid, _VAR_NAME(peOid), "Value is over the limit", "", expr) );
		}
	} catch (const mup::ParserError& e) {
		sbx::mubridge::handle(e, r, _container);
	}
}

void RuleEngine::_validateOptionAllowed(const sbx::ProductElementValue& pev, sbx::ValidationResults& valResult)
{
	if (_container.existsAs(pev.getProductElementOid(), sbx::ComparisonTypes::kEnum))
	{
		// if it exists as an enum/equals constant, then check if the value is allowed
		if ( !this->_isOptionAllowed(pev) )
		{
			valResult.addValidationResult( sbx::ValidationResult(sbx::ValidationCode::kValueNotAllowed, pev.getProductElementOid(), _VAR_NAME(pev.getProductElementOid()), "Value not allowed") );
		}
	}
}

/**
 * Run all custom rules relating to the peOid
 * Also run any requiredif rules, to validate requiredness
 */
void RuleEngine::_validateCustomRules(unsigned short peOid, sbx::ValidationResults& valResults)
{
	// if there are specific rules to run, then run them
	if (_peOidToRules.find(peOid) != _peOidToRules.end())
	{
		const auto& rangeOfRules = _peOidToRules.equal_range(peOid); // std::pair <std::multimap<unsigned short, sbx::Rule*>::iterator, std::multimap<unsigned short, sbx::Rule*>::iterator> range = _peOidToRules.equal_range(peOidToValidate);

		// for each rule, validate and add messages to valResult
		for (auto it = rangeOfRules.first; it != rangeOfRules.second; it++) //	for (std::multimap<unsigned short, sbx::Rule*>::iterator it = range.first; it != range.second; it++)
		{
			std::shared_ptr<sbx::Rule> rule = it->second;

			if (rule->getExpr() != "")
				_executeRule( peOid, rule, valResults );

			// if there is a requiredif-rule, then validate it to check if this productelement is required to be there or not
			if ((rule->getRequiredIfRule() != nullptr) && rule->getRequiredIfRule()->getExpr() != "")
				_executeRequiredIfRule(peOid, rule->getRequiredIfRule(), valResults);
		}
	}
	else
		// if no rules to run, then add a warning
		valResults.addWarning( sbx::ValidationResult(sbx::ValidationCode::kWarning, peOid, _VAR_NAME(peOid), "No rules to run for this ProductElement") );
}

bool RuleEngine::_isOptionAllowed(const sbx::ProductElementValue& pev)
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

/**
 * Validate requiredif-rules and determine if this pev is required on the TA according to other related
 * values on the TA (that are assumed to have been loaded into the parser).
 *
 * If we cannot determine positively that the peOid is required on the TA, we assume it's not required.
 *
 * In case of full validation, if a token is missing, we assume that the product element is required,
 * since the requiredif expr couldn't positively be verified to be either true or false
 */
bool RuleEngine::_isRequired(unsigned short peOid, sbx::ValidationResults& valResults, bool fullValidation)
{
	// run through all specific rules
	if (_peOidToRules.find(peOid) != _peOidToRules.end()) {
		const auto& rangeOfRules = _peOidToRules.equal_range(peOid); // std::pair <std::multimap<unsigned short, std::shared_ptr<sbx::Rule>::iterator, std::multimap<unsigned short, std::shared_ptr<sbx::Rule>::iterator> range = _peOidToRules.equal_range(peOidToValidate);

		for (auto it = rangeOfRules.first; it != rangeOfRules.second; it++) { //	for (std::multimap<unsigned short, std::shared_ptr<sbx::Rule>::iterator it = range.first; it != range.second; it++)
			std::shared_ptr<sbx::Rule> rule = it->second;

			// if there is a requiredif-rule, then run it to check if this productelement is positively required to be there
			if ((rule->getRequiredIfRule() != nullptr) && rule->getRequiredIfRule()->getExpr() != "") {

				try {
					// If the requiredif rule evaluates to true, we know deterministically that its required and can therefore safely return true without continuing evaluating more rules
					// There should only be one requiredif rule for one pe, so its safe to assume that if that validates to true, the pe is required
					if ( _execute(rule->getRequiredIfRule()->getExpr(), rule->getRequiredIfRule()->getRuleId()).GetBool() ) {
						return true;
					}
				} catch (const mup::ParserError& e) {
					sbx::mubridge::handle(e, valResults, _container);

					// In case of a missing token, and we are doing full validation, we assume the pe is required and therefore return true
					if (e.GetCode() == mup::ecUNASSIGNABLE_TOKEN && fullValidation)
						return true;
				}

				// if the requiredif-rule was false, we continue executing other potential rules to see if they make the pe required
			}
		}
	}

	// if no rules specifically said that the peOid is required, we assume it's not required and return false
	return false;
}

/**
 * Run through specific rules to see if requiredif rules validates to false.
 * If it does, we assume the pe is optional and return true. We can do this,
 * because we assume there is only one requiredif rule per pe.
 *
 * If no requiredif rule (should only be one) states that the pe is optional (ie. by evaluating to false),
 * we assume that the pe is required, and therefore return false in the end
 *
 * In case of missing token, we continue to look for a positive confirmation of optionality by a 'negative' requiredif rule.
 * A warning of the missing token is added though
 */
bool RuleEngine::_isOptional(unsigned short peOid, sbx::ValidationResults& valResults)
{
	if (_peOidToRules.find(peOid) != _peOidToRules.end()) {
		// run through all specific rules
		const auto& rangeOfRules = _peOidToRules.equal_range(peOid); // std::pair <std::multimap<unsigned short, std::shared_ptr<sbx::Rule>::iterator, std::multimap<unsigned short, std::shared_ptr<sbx::Rule>::iterator> range = _peOidToRules.equal_range(peOidToValidate);

		for (auto it = rangeOfRules.first; it != rangeOfRules.second; it++) { //	for (std::multimap<unsigned short, std::shared_ptr<sbx::Rule>::iterator it = range.first; it != range.second; it++)
			std::shared_ptr<sbx::Rule> rule = it->second;

			// if there is a requiredif-rule, then run it
			if ((rule->getRequiredIfRule() != nullptr) && rule->getRequiredIfRule()->getExpr() != "") {

				try {
					// if the requiredif rule evaluates to false, we assume the pe is optional as there should only be one requiredif rule per pe
					if ( !_execute(rule->getRequiredIfRule()->getExpr(), rule->getRequiredIfRule()->getRuleId()).GetBool() ) {
						return true;
					}
				} catch (const mup::ParserError& e) {
					sbx::mubridge::handle(e, valResults, _container);
				}

				// if the requiredif-rule was false, we continue executing other potential rules to see if they make the pe required
			}
		}
	}

	// if no rules specifically said that the peOid is optional, we assume it's required by answering false
	return false;
}

/**
 */
void RuleEngine::_executeRule(unsigned short peOidBeingValidated, std::shared_ptr<sbx::Rule> rule, sbx::ValidationResults& valResult, sbx::ValidationCode negativeValCode) {
	try {
		mup::Value result = _execute(rule->getExpr(), rule->getRuleId());

		if (result.GetBool()) {
			// only add message, if it's not empty
			if (rule->getPositiveMessage() != "") {
				valResult.addValidationResult( sbx::ValidationResult(sbx::ValidationCode::kFail, peOidBeingValidated, _VAR_NAME(peOidBeingValidated), rule->getPositiveMessage(), rule->getRuleId(), rule->getExpr()) );
			}
		}
		else {
			// If no negative message, we don't consider the negative outcome as a failure
			if (rule->getNegativeMessage() != "") {
				valResult.addValidationResult( sbx::ValidationResult(negativeValCode, peOidBeingValidated, _VAR_NAME(peOidBeingValidated), rule->getNegativeMessage(), rule->getRuleId(), rule->getExpr()) );
			}
		}
	}
	catch (const mup::ParserError& e) {
		// TODO if full validation, then handle as failure, otherwise as warning
		sbx::mubridge::handle(e, valResult, _container);
	}
}

/**
 * Validates the rule as a requiredif rule, meaning that the peOidBeingValidated
 * is only allowed on the TA if the requiredIf rule validates to true.
 * If it's false, we add a message saying the pe is not allowed.
 *
 * We assume there is only one requiredif rule per pe, so its safe to assume that if that evaluates to false, the element is not allowed
 */
void RuleEngine::_executeRequiredIfRule(unsigned short peOidBeingValidated, std::shared_ptr<sbx::Rule> requiredIfRule, sbx::ValidationResults& valResults) {
	try {
		mup::Value result = _execute(requiredIfRule->getExpr(), requiredIfRule->getRuleId());

		// If the evaluation is false, we add a failure to the child PE saying it's not allowed
		if ( !result.GetBool() )
			valResults.addValidationResult( sbx::ValidationResult(sbx::ValidationCode::kProductElementNotAllowed, peOidBeingValidated, _VAR_NAME(peOidBeingValidated), "Product Element is not allowed", requiredIfRule->getRuleId(), requiredIfRule->getExpr()) );

		// TODO consider adding handling else case, i.e. infoValidations
	}
	catch (const mup::ParserError& e) {
		sbx::mubridge::handle(e, valResults, _container);
	}
}

/**
 * Executes the expression and prints info about the expression being evaluated.
 * Also handles ParserError nicely, but rethrows for the caller to handle individually
 */
mup::Value RuleEngine::_execute(const std::string& expr, const std::string& ruleId)
{
	mup::Value result {};

	try {
		_parser.SetExpr(expr);
		if (RuleEngine::_printDebugAtValidation) { cout << "Executing ruleId[" << ruleId << "], expr[" << expr << "]...";}
		result = _parser.Eval();
		if (RuleEngine::_printDebugAtValidation) { cout << "=[" << boolalpha << result.GetBool() << "]" << endl;}
	} catch (const mup::ParserError& e)	{
		if (RuleEngine::_printDebugAtValidation) { cout << " ... ! Exception evaluating the expression..." << endl;}
		sbx::mubridge::handle(e);
		throw;
	}

	return result;
}

/**
 * Validates the partial or full TA
 *
 * full:
 * 	 true (default)		: validates values set on the TA plus validates whether all required values are set on the TA.
 * 	 false				: Only validates the values put on the ta already. Does not take into account values that are not set, but might be required
 *
 */
sbx::ValidationResults RuleEngine::validate(const sbx::TA& ta, bool full)
{
	_loadParser(ta);
	_refreshParserValues = false;
	sbx::ValidationResults valResults {};

	try {
		std::set<unsigned short, less<unsigned short>> allowedProductElementOids {_getAllowedPEOids()};

		// run through all available values on the TA and check to see if they are OK
		//   1. check if they are allowed according to the konceptinfo
		for (auto& item : ta.getValues()) {
			const sbx::ProductElementValue& pev = item.second;
			unsigned short peOid = pev.getProductElementOid();

			// if peOid was not found in the allowed list of pe's allowed on this konceptinfo, then add a validation message
			if (allowedProductElementOids.find(peOid) == allowedProductElementOids.cend()) {
				stringstream msg {};
				msg << "Product element oid [" << peOid << "] not allowed for this konceptInfo";
				valResults.addValidationResult( sbx::ValidationResult(sbx::ValidationCode::kProductElementNotAllowed, peOid, _VAR_NAME(peOid), msg.str()) );
			}
			else {
				// If element is allowed on the konceptinfo, then validate it.
				//   This could result in the pe not being allowed on the TA due to other values on the ta
				valResults.merge(this->validate(ta, peOid));
			}
		}

		// if also full validation, run through all the expected/allowed pe's and see if they have a value and if they are required if they are missing
		if (full)
		{
			// run through all the allowed product elements and check if its on the TA. If its not, then check if its optional. If its not optional, add a message stating its required
			for (auto& peOid : allowedProductElementOids)
			{
//				if ( _isRequired(peOid, valResults, full) ) {
//					valResults.merge(this->validate(ta, peOid));
//				}

				// If it's not on the TA and it is not optional, tell that the pe is required
				if ( !ta.hasValue(peOid) && !_isOptional(peOid, valResults) )
					valResults.addValidationResult( sbx::ValidationResult(sbx::ValidationCode::kProductElementRequired, peOid, _VAR_NAME(peOid), "Value is missing") );
			}
		}

	} catch (const std::exception &e) {
		cerr << e.what() << endl;
		// if any exception occurred, then reset _refresParserValues and rethrow
		_refreshParserValues = true;
		throw;
	}

	_refreshParserValues = true;
	return valResults;
}

std::set<unsigned short, std::less<unsigned short>> RuleEngine::_getAllowedPEOids()
{
	std::set<unsigned short, less<unsigned short>> allowedProductElementOids {};

	for (auto& parameterIt : _ki.getParameterValues()) {
		const std::set<unsigned short>& productElementOids = _container.getProductElementOids(parameterIt.first);
		allowedProductElementOids.insert(productElementOids.cbegin(), productElementOids.cend());
	}

	return allowedProductElementOids;
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
			cout << varName << "=" << var << " (type=" << var.GetType() << ")" << endl;
		}
	}
	catch (const mup::ParserError &e) {
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
	catch (const mup::ParserError &e) {
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
	} catch (const domain_error& e) {
		// unsigned short oid, const std::string& variableName, const std::string& guiName, sbx::ProductElementTypes productElementType, unsigned short productOid
		return sbx::ProductElement(peOid, "<unknown>", "<unknown>", sbx::ProductElementTypes::kUnknownPEType, 0);
	}
}

std::string RuleEngine::_VAR_NAME(unsigned short peOid)
{
	try {
		return _container.getProductElement(peOid).getVariableName();
	} catch (const domain_error& e) {
		return std::string ("");
	}
}

std::string RuleEngine::_indent(unsigned short depth) { return std::string(depth, ' '); }

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
