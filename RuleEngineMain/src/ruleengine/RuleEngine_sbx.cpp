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
#include <locale>
#include <set>
#include <map>
#include <string>
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
#include "mubridge/DefinedInParser.h"
#include "mubridge/NSEquals.h"

using namespace std;

namespace sbx {

bool RuleEngine::_printDebug {false};
bool RuleEngine::_printDebugAtValidation {false};
bool RuleEngine::_printMuParserErrorInfo {false};

sbx::RuleEngine::RuleEngine()
{
	_parser.DefineFun(new DefinedInParser {this});
	_parser.DefineFun(new NSEquals {this});
}

sbx::RuleEngine::RuleEngine(const sbx::RuleEngine& other)
		: _container {other._container}
{
	// TODO !!!!! proper copy/cloning of pointers in map
	cerr << " Copy constructor for RuleEngine was called! The behaviour of copying the RuleEngine instance is not defined" << endl;
	_parser.DefineFun(new DefinedInParser {this});
	_parser.DefineFun(new NSEquals {this});
}

void RuleEngine::initialiseAll(const std::string& jsonContents)
{
	_container.initConstants(jsonContents);
	_container.initKoncepts(jsonContents);
	_container.initUnionAgreements(jsonContents);
//	this->initConstants(jsonContents);
//	this->initKoncepts(jsonContents);
//	this->initUnionAgreements(jsonContents);
}

void RuleEngine::initConstants(const std::string& jsonContents)
{
	_container.initConstants(jsonContents);
}

//void RuleEngine::initUAContributionSteps(const std::map<unsigned short, std::vector<sbx::ContributionStep>>& uaLadders)
//{
//	_container.initUAContributionSteps(uaLadders);
//}

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

void RuleEngine::initKoncepts(const std::string& jsonContents)
{
	_container.initKoncepts(jsonContents);
}

void RuleEngine::initUnionAgreements(const std::string& jsonContents)
{
	_container.initUnionAgreements(jsonContents);
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
			string requiredifexpr = valueIterator->get("requiredif", "").asString();
			string evaluateExprIf = valueIterator->get("evaluateExprIf", "").asString();
			string notallowedifexpr = valueIterator->get("notallowedif", "").asString();
			string positiveMessage = valueIterator->get("positiveMessage", "").asString();
			string negativeMessage = valueIterator->get("negativeMessage", "").asString();
			int negativeValCode = valueIterator->get("negativeValCode", 0).asInt();
			int positiveValCode = valueIterator->get("positiveValCode", 0).asInt();

			if (RuleEngine::_printDebug) { cout << "  Creating Rule [" << id << "], expr [" << expr << "]" << endl; }


			std::shared_ptr<sbx::Rule> rule = make_shared<sbx::Rule>(
					id,
					expr,
					((requiredifexpr == "") ? nullptr : ((requiredifexpr == "#parent#") ? ruleCatalogue->getParent() : make_shared<sbx::Rule>(id + ".R", requiredifexpr, nullptr, "", ""))),
					positiveMessage,
					negativeMessage,
					preCalcExpr);

			rule->setNotAllowedIfRule( (notallowedifexpr == "") ? nullptr : ((notallowedifexpr == "#parent#") ? ruleCatalogue->getParent() : make_shared<sbx::Rule>(id + ".NA", notallowedifexpr, nullptr, "", "")));
			rule->setEvaluateExprIfRule( (evaluateExprIf == "") ? nullptr : ((evaluateExprIf == "#parent#") ? ruleCatalogue->getParent() : make_shared<sbx::Rule>(id + ".EVEXP", evaluateExprIf, nullptr, "", "")));
			rule->setNegativeValCode(negativeValCode);
			rule->setPositiveValCode(positiveValCode);


			Json::Value requiredPEOids = valueIterator->get("preCalcRequiredPEOids", "");
			if (requiredPEOids.size() > 0)
				for (Json::ValueIterator requiredPEOidIterator = requiredPEOids.begin(); requiredPEOidIterator != requiredPEOids.end(); ++requiredPEOidIterator)
					rule->addPreCalcRequiredPEOid(requiredPEOidIterator->asInt());


			Json::Value negativeMessageParameters = valueIterator->get("negativeMessageParameters", "");
			if (negativeMessageParameters.size() > 0)
				for (Json::ValueIterator negativeMessageParametersIterator = negativeMessageParameters.begin(); negativeMessageParametersIterator != negativeMessageParameters.end(); ++negativeMessageParametersIterator)
					rule->addNegativeMessageParameter(negativeMessageParametersIterator->asString());

			Json::Value positiveMessageParameters = valueIterator->get("positiveMessageParameters", "");
			if (positiveMessageParameters.size() > 0)
				for (Json::ValueIterator positiveMessageParametersIterator = positiveMessageParameters.begin(); positiveMessageParametersIterator != positiveMessageParameters.end(); ++positiveMessageParametersIterator)
					rule->addPositiveMessageParameter(positiveMessageParametersIterator->asString());

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
 * Initialises local context that is used for subsequent operations on RuleEngine
 */
void RuleEngine::initContext(const sbx::KonceptInfo& ki, sbx::UnionAgreementRelationship uar, sbx::unionagreement_oid uaOid)
{
	// Clear any previous context and parser values
	_container.clearContext();
	this->_clearParserValues();

	// Check union agreement
	if (uar != sbx::UnionAgreementRelationship::OUTSIDE && (uaOid == sbx::undefined_oid || !_container.isUnionAgreementLoaded(uaOid)) )
	{
		stringstream ss {};
		ss << "Invalid context! Cannot set FOLLOWS or INCLUDED with invalid union agreement oid[" << uaOid << "]";
		throw domain_error(ss.str());
	}

	// Check koncept
	if (_container.getKoncepts().find(ki.getKonceptOid()) == _container.getKoncepts().cend())
	{
		stringstream ss {};
		ss << ki.getKonceptOid();
		throw domain_error("No valid koncept with oid [" + ss.str() + "]");
	}

	// Find subkoncept
    Koncept k = _container.getKoncepts().at(ki.getKonceptOid());
   	Subkoncept sk = k.getSubkoncept(ki.getNumberOfEmployees());

	_container.initContext(sk, ki, uar, uaOid);

	// init ParserX with constants for context by going through each parameter and get the product elements for that parameter
	const std::set<sbx::productelement_oid>& peOids = _container.getAllowedPEOids();

	for (auto& peOid : peOids)
		// get min and max values and initialise parser with that
		_initParserWithProductElementConstants(peOid);
}

void RuleEngine::_clearParserValues()
{
	// clear parser for variables and consts
	_parser.ClearVar();
	_parser.ClearConst();
	_mupValueMap.clear();
}

void RuleEngine::_initParserWithProductElementConstants(sbx::productelement_oid peOid)
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

void RuleEngine::initUAContributionSteps(const std::map<sbx::unionagreement_oid, std::vector<sbx::ContributionStep>>& uaLadders)
{
	// not doing anything, just here for backward compatibility
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
				_parser.EnableAutoCreateVar(false);
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
std::vector<string> RuleEngine::getOptions(ProductElementOid productElement)
{
	return _container.getOptions(productElement);
}

/**
 * Delegate call to RuleConstantContainer::getOptionsList(...)
 */
std::vector<std::shared_ptr<Constant>> RuleEngine::getOptionsList(sbx::ProductElementOid productElement)
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
void RuleEngine::_loadParser(TA& ta, ValidationResults& valResults)
{
	// clear all vars in parser, and
	_parser.ClearVar();

	// set ua + uar on ta for pe value initialisation into the parser
	ta.setValue(kUnionAgreementRelationship, sbx::utils::convertUAR(_container.getUnionAgreementRelationship()));

	if (_container.getUnionAgreementRelationship() != OUTSIDE)
		ta.setValue(kUnionAgreementOid, _container.getUnionAgreementOid());

	// initialise all PE values for TA into parser
	for (auto& item : ta.getValues())
	{
		const sbx::ProductElementValue& pev = item.second;
		const sbx::ProductElement pe = _container.getProductElement(item.first);

		switch (pe.getElementType())
		{
		case kDate:
			// TODO: how to validate date??
			_defineVariable<int>(pe.getVariableName(), pev.longValue());
			break;
		case kBool:
			_defineVariable<bool>(pe.getVariableName(), pev.boolValue());
			break;
		case kText:
			_defineVariable<std::string>(pe.getVariableName(), pev.stringValue());
			break;
		case kLong: // fall throughs down to long
		case kMonth:
		case kYear:
			_defineVariable<int>(pe.getVariableName(), pev.longValue());
			break;
		case kCurr: // fall throughs down to double
		case kPercent:
			_defineVariable<double>(pe.getVariableName(), pev.doubleValue());
			break;
		case kUnknownPEType:
			// TODO: return or throw exception? agree with iOS
			break;
		}
	}

	// set union agreement ladder employer pct and total pct
	_loadUAContributionStep(ta, valResults);
	_loadLadder(ta);

	// execute all precalc rules to update the parser
	_executePreCalcRules();
}

/**
 * Creating a 3 x n (y,x) matrix to hold contributions
 *
 * y = row, idx (Ingen, Age, Senority, Year (20150612)
 * x = column
 *
 * Senority:
 *
 *  idx | empPct | compPct
 * -----|--------|--------
 *   0  |   3.5  |  4.5
 *   1  |   4.5  |  5.5
 *   2  |   5.5  |  6.5
 *
 * Age:
 *
 *  idx | empPct | compPct
 * -----|--------|--------
 *   18 |   3.5  |  4.5
 *   20 |   4.5  |  5.5
 *   25 |   5.5  |  6.5
 *
 * so (1,2) = compPct == 6.5
 */
void RuleEngine::_loadLadder(const TA& ta)
{
	// if the variable is already in the parser, remove it
	if (_parser.IsVarDefined(sbx::kBidragstrappe_VARNAME)) {
		_parser.RemoveVar(sbx::kBidragstrappe_VARNAME);
	}

	// if the ladder is already in the value map, then remove it
	if (_mupValueMap.find(sbx::kBidragstrappe_VARNAME) != _mupValueMap.cend()) {
		_mupValueMap.erase(sbx::kBidragstrappe_VARNAME);
	}

	auto n = ta.getContributionLadder().size();

	if (n > 0) {
		std::shared_ptr<mup::Value> ladderMatrix = make_shared<mup::Value>(n, 3, 0);
		// if not already there, create and insert a new value
		_mupValueMap.insert( std::pair<std::string, std::shared_ptr<mup::Value>> (sbx::kBidragstrappe_VARNAME, ladderMatrix));

		int i {0};
		// initialise contribution ladder matrix
		for (auto it : ta.getContributionLadder())
		{
			ladderMatrix->At(i, 0) = it.index();
			ladderMatrix->At(i, 1) = it.employeePct();
			ladderMatrix->At(i, 2) = it.companyPct();

			i++;
		}

		// Put matrix into parser
		_parser.DefineVar(sbx::kBidragstrappe_VARNAME, mup::Variable(_mupValueMap[sbx::kBidragstrappe_VARNAME].get()));
	}
}

void RuleEngine::_loadUAContributionStep(const TA& ta, ValidationResults& valResults)
{
	if (_parser.IsConstDefined(sbx::kUnionAgreementEmployerPct1stStep))
		_parser.RemoveConst(sbx::kUnionAgreementEmployerPct1stStep);

	if (_parser.IsConstDefined(sbx::kUnionAgreementTotalPct1stStep))
		_parser.RemoveConst(sbx::kUnionAgreementTotalPct1stStep);

	if (_container.getUnionAgreementRelationship() == sbx::FOLLOWS || _container.getUnionAgreementRelationship() == sbx::INCLUDED)
	{
		sbx::unionagreement_oid uaOid = _container.getUnionAgreementOid();

		// if the ua oid is unknown, the set message for the union agreement oid
		if (uaOid == sbx::undefined_oid)
			valResults.addValidationResult( sbx::ValidationResult(sbx::kValueNotAllowed, kUnionAgreementOid, kUnionAgreementOid_VARNAME, "Vælg venligst en gyldig overenskomst!") );
		else
		{
            if (ta.hasValue(kAftaleIkraftdato))
            {
                int aftaleIkraftdato = ta.getValue(kAftaleIkraftdato).longValue();
                
                try {
                    Date inceptionDate(aftaleIkraftdato);
                    sbx::UnionAgreementContributionStep step = _container.getUAContributionStep(uaOid, inceptionDate);
                    _parser.DefineConst(sbx::kUnionAgreementEmployerPct1stStep, step.totalPct() - step.employeePct());
                    _parser.DefineConst(sbx::kUnionAgreementTotalPct1stStep, step.totalPct());
                } catch(const invalid_argument& e) {
                    stringstream ss{};
                    ss << "Ugyldig ikraftdato [" << aftaleIkraftdato << "] - " << e.what() << "!";
                    valResults.addValidationResult( sbx::ValidationResult(sbx::kValueNotAllowed, kAftaleIkraftdato, _VAR_NAME(kAftaleIkraftdato), ss.str()) );
                }
            }
            else
                valResults.addValidationResult( sbx::ValidationResult(sbx::kProductElementRequired, kAftaleIkraftdato, _VAR_NAME(kAftaleIkraftdato), "Ingen ikraftdato sat" ));

		}
	}
}

/**
 * See sbx::ValidationResults RuleEngine::validate(const TA& ta, const std::vector<unsigned short>& peOidsToValidate)
 */
sbx::ValidationResults RuleEngine::validate(sbx::TA& ta, sbx::productelement_oid peOidToValidate)
{
	return this->validate(ta, std::vector<sbx::productelement_oid> {peOidToValidate});
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
sbx::ValidationResults RuleEngine::validate(sbx::TA& ta, const std::vector<sbx::productelement_oid>& peOidsToValidate)
{
	sbx::ValidationResults valResults{};

	// Check to see if we need to refresh parser values before validating
	if (_refreshParserValues)
		_loadParser(ta, valResults);

//	if (RuleEngine::_printDebugAtValidation) printVariablesInParser();

	for (auto& peOid : peOidsToValidate) {

		// If the value about to be validated is on the TA, then do value validation
		if ( ta.hasValue(peOid) )
		{
			// Validate value of the pe using constant container for option-allowed checking and dynamic rule expr's
			//  using in muParser for min/max validation. Does not run rules from rule catalogue
			_validateValue(ta.getValue(peOid), valResults);
		}
		else if ( !_parser.IsVarDefined(_VAR_NAME(peOid)) ) {
			// If the variable is not defined in the parser, then check if it's required by running required rules,
			//   found in rule catalogue on the peOid
			// If it's required and missing, then add msg
			std::shared_ptr<sbx::Rule> requiredIfRule = _isRequired(peOid, valResults);
			if ( requiredIfRule != nullptr && !valResults.hasWarnings(peOid, sbx::ValidationCode::kProductElementRequired))
				valResults.addWarning( sbx::ValidationResult(sbx::ValidationCode::kProductElementRequired, peOid, _VAR_NAME(peOid), "Værdi for [" + _GUI_NAME(peOid) + "] ikke angivet", requiredIfRule->getRuleId(), requiredIfRule->getExpr()) );
		}

		// Even if the value doesn't exists on the TA, we still run custom rules, as there might be
		//   custom calculations to run and validate, such as DoedSpaendPct etc.
		_validateCustomRules(peOid, valResults);

		if (peOid == sbx::ProductElementOid::kBidragstrappe)
		{
			double previousStepTotal {0};
			int previousIndex {-1};

			// loop through the steps and validate each step expecting increasing totals
			for (auto& step : ta.getContributionLadder()) {

				double total {step.employeePct() + step.companyPct()};

				std::stringstream ss {};
				ss << step.index();

				// if the total procentage is not increasing, then add message
				if (total < previousStepTotal && !valResults.hasMessages(peOid, sbx::kValueUnderLimit))
					valResults.addValidationResult(sbx::ValidationResult(sbx::kValueUnderLimit, peOid, _VAR_NAME(peOid) + "_" + ss.str(), "Den totale bidragsprocent skal være stigende"));

				// if this step is similar to previous step, then add message. The steps are sorted by index, so it's sufficient to just check against the previous step
				if (step.index() == previousIndex && !valResults.hasMessages(peOid, sbx::kValueNotAllowed))
					valResults.addValidationResult(sbx::ValidationResult(sbx::kValueNotAllowed, peOid, _VAR_NAME(peOid) + "_" + ss.str(), "Dette trin findes allerede"));

				if (total > 100)
					valResults.addValidationResult(sbx::ValidationResult(sbx::kValueOverLimit, peOid, _VAR_NAME(peOid) + "_" + ss.str(), "Den totale bidragsprocent mmå ikke overstige 100"));

				previousStepTotal = total;
				previousIndex = step.index();
			}
		}
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

		if (!result.GetBool() && !r.hasMessages(peOid, kValueUnderLimit)) {
			// value was not greater or equal to min
			stringstream ss {};
			ss << "Værdien [" << sbx::utils::formatValue(pev.longValue()) << "] for [" << _GUI_NAME(peOid) << "] må ikke være mindre end [" << this->_getConstFromParser(sbx::utils::constructRCName(_PE(peOid), sbx::ComparisonTypes::kMin)) << "]";
			r.addValidationResult( sbx::ValidationResult(sbx::ValidationCode::kValueUnderLimit, peOid, _VAR_NAME(peOid), ss.str(), "", expr) );
		}
	} catch (const mup::ParserError& e) {
		sbx::mubridge::handle(e, r, _container);
	}

	expr = sbx::utils::constructMinMaxExpr(_PE(peOid), sbx::ComparisonTypes::kMax);

	try {
		mup::Value result = _execute(expr, "Max-expr");

		if (!result.GetBool() && !r.hasMessages(peOid, kValueOverLimit))
		{
			stringstream ss {};
			ss << "Værdien [" << sbx::utils::formatValue(pev.longValue()) << "] for [" << _GUI_NAME(peOid) << "] må ikke overstige [" << this->_getConstFromParser(sbx::utils::constructRCName(_PE(peOid), sbx::ComparisonTypes::kMax)) << "]";
			// value was not lesser or equal to max
			r.addValidationResult( sbx::ValidationResult(sbx::ValidationCode::kValueOverLimit, peOid, _VAR_NAME(peOid), ss.str(), "", expr) );
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
			stringstream optionsString {};

			const auto& options = _container.getOptionsList(pev.getProductElementOid());

            for (auto option : options)
            {
                optionsString << _getFormattedValue(option);
				optionsString << ", ";
            }

			stringstream msg {};
			msg << "Værdien [" << getFormattedValue(pev) << "] er ikke tilladt! Tilladte værdier er : [" << optionsString.str().substr(0, optionsString.str().length()-2) << "]";

			valResult.addValidationResult( sbx::ValidationResult(sbx::ValidationCode::kValueNotAllowed, pev.getProductElementOid(), _VAR_NAME(pev.getProductElementOid()), msg.str()) );
		}
	}
}

/**
 * Run all custom rules relating to the peOid
 * Also run any requiredif rules, to validate requiredness
 */
void RuleEngine::_validateCustomRules(sbx::productelement_oid peOid, sbx::ValidationResults& valResults)
{
	// if there are specific rules to run, then run them
	if (_peOidToRules.find(peOid) != _peOidToRules.end())
	{
		const auto& rangeOfRules = _peOidToRules.equal_range(peOid);

		// for each rule, check the evaluateExprIf rule. If its true, then continue to validate this rule and add messages to valResult
		for (auto it = rangeOfRules.first; it != rangeOfRules.second; it++)
		{
			std::shared_ptr<sbx::Rule> rule = it->second;

			bool evaluate {true};

			if ( (rule->getEvaluateExprIfRule() != nullptr) && (rule->getEvaluateExprIfRule()->getExpr() != "") )
			{
				try {
					mup::Value evaluateExprIfResult = _execute(rule->getEvaluateExprIfRule()->getExpr(), rule->getEvaluateExprIfRule()->getRuleId());

					if ( evaluateExprIfResult.GetType() == 'b' )
						evaluate = evaluateExprIfResult.GetBool();
					else
						cout << "EvaluateExprIf [" << rule->getEvaluateExprIfRule()->getExpr() << "] didn't evaluate to a boolean. Result : " << evaluateExprIfResult << "]" << endl;
				} catch (const mup::ParserError& e) {
					sbx::mubridge::handle(e, valResults, this->getContainer());
					evaluate = false;
				}
			}

			// if there is an expr and we should evaluate it
			if (rule->getExpr() != "" && evaluate) {
				try	{
					_evaluateRule( peOid, rule, valResults );
				} catch (const mup::ParserError& e) {
					sbx::mubridge::handle(e, valResults, this->getContainer());
				}
			}


			// if there is a notallowedif rule, then evaluate it
			if (rule->getNotAllowedIfRule() != nullptr && (rule->getNotAllowedIfRule()->getExpr() != "") )
			{
				try {
					mup::Value notAllowedIfResult = _execute(rule->getNotAllowedIfRule()->getExpr(), rule->getNotAllowedIfRule()->getRuleId());

					// if the peOid is not allowed and it's value is set in the parser, set message that pe is not allowed, if the msg is not already there
					if ( notAllowedIfResult.GetType() == 'b' && notAllowedIfResult.GetBool() && _parser.IsVarDefined(_VAR_NAME(peOid)) && !valResults.hasMessages(peOid, kProductElementNotAllowed))
					{
						valResults.addValidationResult( sbx::ValidationResult(
								sbx::ValidationCode::kProductElementNotAllowed,
								peOid,
								_VAR_NAME(peOid),
								"Der skal ikke angives værdi for [" + _GUI_NAME(peOid) + "]",
								rule->getNotAllowedIfRule()->getRuleId(),
								rule->getNotAllowedIfRule()->getExpr(),
								"Not allowedRule check in validateCustomRules") );
					}
					else if ( notAllowedIfResult.GetType() != 'b' )
 					{
						cout << "notAllowedIf expr [" << rule->getNotAllowedIfRule()->getExpr() << "] didn't evaluate to a boolean. Result : [" << notAllowedIfResult << "]" << endl;
					}
				} catch (const mup::ParserError& e) {
					sbx::mubridge::handle(e, valResults, this->getContainer());
				}
			}
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
std::shared_ptr<sbx::Rule> RuleEngine::_isRequired(sbx::productelement_oid peOid, sbx::ValidationResults& valResults, bool fullValidation)
{
	// run through all specific rules
	if (_peOidToRules.find(peOid) != _peOidToRules.end()) {
		const auto& rangeOfRules = _peOidToRules.equal_range(peOid); // std::pair <std::multimap<unsigned short, std::shared_ptr<sbx::Rule>::iterator, std::multimap<unsigned short, std::shared_ptr<sbx::Rule>::iterator> range = _peOidToRules.equal_range(peOidToValidate);

		for (auto it = rangeOfRules.first; it != rangeOfRules.second; it++) { //	for (std::multimap<unsigned short, std::shared_ptr<sbx::Rule>::iterator it = range.first; it != range.second; it++)
			std::shared_ptr<sbx::Rule> rule = it->second;

			// if there is a requiredif-rule, then run it to check if this productelement is positively required to be there
			if ((rule->getRequiredIfRule() != nullptr) && rule->getRequiredIfRule()->getExpr() != "") {

				try {
					// If the requiredif rule evaluates to true, we know deterministically that its required and can therefore safely return the rule that requires the element without continuing evaluating more rules
					mup::Value r = _execute(rule->getRequiredIfRule()->getExpr(), rule->getRequiredIfRule()->getRuleId());

					if ( r.GetType() == 'b' && r.GetBool() ) {
						return rule->getRequiredIfRule();
					}
				} catch (const mup::ParserError& e) {
					sbx::mubridge::handle(e, valResults, _container);

					// In case of a missing token, and we are doing full validation, we assume the pe is required and therefore return the requiredif rule
					if (e.GetCode() == mup::ecUNASSIGNABLE_TOKEN && fullValidation)
						return rule->getRequiredIfRule();
				}

				// if the requiredif-rule was false, we continue executing other potential rules to see if they make the pe required
			}

		}
	}

	// if no rules specifically said that the peOid is required, we assume it's not required and return null
	return nullptr;
}

/**
 * Run through specific rules to see if any requiredif rule validates to true.
 * All requiredif rules has to positively evaluate to false.
 * If just one requiredif rule validates to true, the pe is not optional.
 *
 * If all requiredif rules evaluate to false,
 * we know that the pe is actually optional (not required by any rule)
 * and therefore answer true in the end.
 *
 * In case of missing token or other ParserError, we answer false, as we cannot positively confirm
 * that the pe is not required by the requiredif rule or not, so we assume not optional.
 */
void RuleEngine::_isOptional(sbx::productelement_oid peOid, sbx::ValidationResults& valResults)
{
	if (_peOidToRules.find(peOid) != _peOidToRules.end()) {
		// run through all specific rules
		const auto& rangeOfRules = _peOidToRules.equal_range(peOid); // std::pair <std::multimap<unsigned short, std::shared_ptr<sbx::Rule>::iterator, std::multimap<unsigned short, std::shared_ptr<sbx::Rule>::iterator> range = _peOidToRules.equal_range(peOidToValidate);

		bool requiredIfRulesExists {false};

		for (auto it = rangeOfRules.first; it != rangeOfRules.second; it++) { //	for (std::multimap<unsigned short, std::shared_ptr<sbx::Rule>::iterator it = range.first; it != range.second; it++)
			std::shared_ptr<sbx::Rule> rule = it->second;

			// if there is a requiredif-rule, then run it
			if ((rule->getRequiredIfRule() != nullptr) && rule->getRequiredIfRule()->getExpr() != "") {
				requiredIfRulesExists = true;
				bool grandParentRequired {false};

				std::shared_ptr<sbx::Rule> requiredIfRule = rule->getRequiredIfRule();
				std::shared_ptr<sbx::Rule> grandParentRule = requiredIfRule->getRequiredIfRule();

				// if there is a grand parent rule, then run that first
				if (grandParentRule != nullptr && grandParentRule->getExpr() != "")
				{
					try {
						mup::Value r = _execute(grandParentRule->getExpr(), grandParentRule->getRuleId());

						// if the grandparent requiredif rule evaluates to true, we will also try to evaluate the parent rule, otherwise there is no need
						if ( r.GetType() == 'b' && r.GetBool() ) {
							grandParentRequired = true;
						}
					} catch (const mup::ParserError& e) {
						sbx::mubridge::handle(e, valResults, _container);

						// in case of exception, we cannot reliably deduct that the requiredif rule is false, so we assume that the pe is not optional,
						//  and straight away return false
						if ( !valResults.hasMessages(peOid, sbx::ValidationCode::kProductElementRequired))
							valResults.addValidationResult(sbx::ValidationResult(sbx::ValidationCode::kProductElementRequired,
																				 peOid,
																				 _VAR_NAME(peOid),
																				 "Værdi for [" + _GUI_NAME(peOid) + "] skal angives!",
																				 grandParentRule->getRuleId(),
																				 "",
																				 "Caused by full validation - Grandparent rule failed"));

					}
				}

				// if no grandparent or grandparentrule == true
				if (grandParentRule == nullptr || grandParentRequired) {
					try {
						mup::Value r = _execute(requiredIfRule->getExpr(), requiredIfRule->getRuleId());

						// if the requiredif rule evaluates to true, we know positively that the pe is not optional, and can straight away answer false
						if ( r.GetType() == 'b' && r.GetBool() ) {
							if ( !valResults.hasMessages(peOid, sbx::ValidationCode::kProductElementRequired))
								valResults.addValidationResult(sbx::ValidationResult(sbx::ValidationCode::kProductElementRequired,
																					 peOid,
																					 _VAR_NAME(peOid),
																					 "Værdi for [" + _GUI_NAME(peOid) + "] skal angives!",
																					 requiredIfRule->getRuleId(),
																					 "",
																					 "Caused by full validation - Required rule == true"));
						}
					} catch (const mup::ParserError& e) {
						sbx::mubridge::handle(e, valResults, _container);

						// in case of exception, we cannot reliably deduct that the requiredif rule is false, so we assume that the pe is not optional,
						//  and straight away return false
						if ( !valResults.hasMessages(peOid, sbx::ValidationCode::kProductElementRequired))
							valResults.addValidationResult(sbx::ValidationResult(sbx::ValidationCode::kProductElementRequired,
																				 peOid,
																				 _VAR_NAME(peOid),
																				 "Værdi for [" + _GUI_NAME(peOid) + "] skal angives!",
																				 requiredIfRule->getRuleId(),
																				 "",
																				 "Caused by full validation - Required rule failed"));
					}
				}
			}
		} // for loop

		if (!requiredIfRulesExists)
			// if no requiredIf rules existed, we assume the pe is not optional
			if ( !valResults.hasMessages(peOid, sbx::ValidationCode::kProductElementRequired))
				valResults.addValidationResult(sbx::ValidationResult(sbx::ValidationCode::kProductElementRequired,
																	 peOid,
																	 _VAR_NAME(peOid),
																	 "Værdi for [" + _GUI_NAME(peOid) + "] skal angives!",
																	 "",
																	 "",
																	 "Caused by full validation - no required rules exisits"));
	}
	else
		// if no pe-specific rules existed, we assume the pe is not optional
		if ( !valResults.hasMessages(peOid, sbx::ValidationCode::kProductElementRequired))
			valResults.addValidationResult(sbx::ValidationResult(sbx::ValidationCode::kProductElementRequired,
																 peOid,
																 _VAR_NAME(peOid),
																 "Værdi for [" + _GUI_NAME(peOid) + "] skal angives!",
																 "",
																 "",
																 "Caused by full validation - no specific rules exists"));}

/**
 */
void RuleEngine::_evaluateRule(sbx::productelement_oid peOidBeingValidated, std::shared_ptr<sbx::Rule> rule, sbx::ValidationResults& valResult, sbx::ValidationCode negativeValCode) {
	try {
		mup::Value result = _execute(rule->getExpr(), rule->getRuleId());

		if (result.GetBool())
		{
			// only add message, if it's not empty
			if (rule->getPositiveMessage() != "")
			{
				sbx::ValidationCode positiveValCode = sbx::utils::toValCode(rule->getPositiveValCode(), sbx::ValidationCode::kFail);

				// add the validation message, if its not already there
				if (!valResult.hasMessages(peOidBeingValidated, positiveValCode))
				{
					auto p = _getParametersFromParser(rule->getPositiveMessageParameters());
					string msg = sbx::utils::formatMessage(rule->getPositiveMessage(), p);
					valResult.addValidationResult( sbx::ValidationResult(positiveValCode,
																		 peOidBeingValidated,
																		 _VAR_NAME(peOidBeingValidated),
																		 msg,
																		 rule->getRuleId(),
																		 rule->getExpr(),
																		 "Evaluating rule - positive") );
				}
			}
		}
		else
		{
			// If no negative message, we don't consider the negative outcome as a failure
			if (rule->getNegativeMessage() != "")
			{
				sbx::ValidationCode negativeValCode = sbx::utils::toValCode(rule->getNegativeValCode(), sbx::ValidationCode::kFail);

				// add the validation message, if its not already there
				if (!valResult.hasMessages(peOidBeingValidated, negativeValCode))
				{
					auto p = _getParametersFromParser(rule->getNegativeMessageParameters());
					string msg = sbx::utils::formatMessage(rule->getNegativeMessage(), p);

					valResult.addValidationResult( sbx::ValidationResult(negativeValCode,
																		 peOidBeingValidated,
																		 _VAR_NAME(peOidBeingValidated),
																		 msg,
																		 rule->getRuleId(),
																		 rule->getExpr(),
																		 "Evaluating rule - negative") );
				}
			}
		}
	}
	catch (const mup::ParserError& e) {
		sbx::mubridge::handle(e, valResult, _container);
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
		if (RuleEngine::_printDebugAtValidation) { cout << "=[" << boolalpha << result << "]" << endl;}
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
sbx::ValidationResults RuleEngine::validate(sbx::TA& ta, bool full)
{
	sbx::ValidationResults valResults {};
	_loadParser(ta, valResults);
	_refreshParserValues = false;

    if (RuleEngine::_printDebugAtValidation) printVariablesInParser();

	try {
		std::set<sbx::productelement_oid, less<sbx::productelement_oid>> allowedProductElementOids { _container.getAllowedPEOids() };

		// run through all available values on the TA and check to see if they are OK
		//   check if they are allowed according to the konceptinfo
		for (auto& item : ta.getValues()) {
			const sbx::ProductElementValue& pev = item.second;
			unsigned short peOid = pev.getProductElementOid();

			// if peOid is not found in the allowed list of pe's allowed on this konceptinfo, then add a validation message
			if (allowedProductElementOids.find(peOid) == allowedProductElementOids.cend()) {
				stringstream msg {};
				msg << "Product element oid [" << peOid << "] not allowed for this konceptInfo";
				valResults.addValidationResult(sbx::ValidationResult(sbx::ValidationCode::kProductElementNotAllowed,
																	 peOid,
																	 _VAR_NAME(peOid), // varname
																	 msg.str(), // msg
																	 "", // ruleId
																	 "", // expr
																	 "Caused by konceptinfo not allowing this product element. No rule was executed for this") );
			}
			else
			{
				// If element is allowed on the konceptinfo, then validate it.
				//   This could result in the pe not being allowed on the TA due to other values on the ta
				valResults.merge(this->validate(ta, vector<productelement_oid> ({peOid}) ));

				// if partial validation, then run through all values on the TA and see if the make other pe's required.
				//  TODO do this by evaluating expr's and if true, then run positiveRuleCatalogue for that rule?
				//       If a sub rule has requiredif rule, then those peoids are required, so set kProductElementRequired
				unsigned short peOid = item.first;

				if (_peOidToRules.find(peOid) != _peOidToRules.end())
				{
					const auto& rangeOfRules = _peOidToRules.equal_range(peOid); // std::pair <std::multimap<unsigned short, sbx::Rule*>::iterator, std::multimap<unsigned short, sbx::Rule*>::iterator> range = _peOidToRules.equal_range(peOidToValidate);

					for (auto it = rangeOfRules.first; it != rangeOfRules.second; it++) //	for (std::multimap<unsigned short, sbx::Rule*>::iterator it = range.first; it != range.second; it++)
					{
						std::shared_ptr<sbx::Rule> parentRule = it->second;

						// if the rule has an expression AND a rule catalogue, then execute the expr
						if (parentRule->getExpr() != "" && parentRule->getPositiveRuleCatalogue() != nullptr)
						{
							try
							{
								mup::Value result = _execute(parentRule->getExpr(), parentRule->getRuleId());

								// if the rule == true, then check for
								if ( result.GetType() == 'b' && result.GetBool() )
								{
									// for all rules in the positive rule catalogue
									for ( auto ruleFromPositiveCatalogue : parentRule->getPositiveRuleCatalogue()->getRules())
									{
										// run the ruleFromPositiveCatalogue
										for (auto oid : ruleFromPositiveCatalogue->getProductElementOids())
										{
											if ( ruleFromPositiveCatalogue->getExpr() != "" )
												_evaluateRule(oid, ruleFromPositiveCatalogue, valResults);

											// if the productelement oid is allowed
											if (allowedProductElementOids.find(oid) != allowedProductElementOids.cend())
												this->_checkRequiredness(oid, ruleFromPositiveCatalogue, parentRule, valResults);
										}
									}
								}
								else
								{
									if (result.GetType() !='b')
										cout << "Parent Rule expr [" << parentRule->getExpr() << "] not evaluating to a boolean, so assuming not required." << endl;
								}
							} catch (const mup::ParserError& e) {
								if (RuleEngine::_printMuParserErrorInfo)
									cout << "Evaluation of parentRule failed: " << endl;
								sbx::mubridge::handle(e);
							}
						}
					}
				}
			}
		}

		// if full validation, run through all the expected/allowed pe's
		//  and check for missing required pe's.
		if (full)
		{
			// run through all the allowed product elements and check if its on the TA. If its not, then check if its optional. If its not optional, add a message stating its required
			for (auto& peOid : allowedProductElementOids)
			{
				bool definedInParser = _parser.IsVarDefined(_VAR_NAME(peOid));
				if (!definedInParser)
					_isOptional(peOid, valResults);
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


void RuleEngine::_checkRequiredness(sbx::productelement_oid _peOid, std::shared_ptr<Rule> ruleFromPositiveCatalogue, std::shared_ptr<sbx::Rule> parentRule, sbx::ValidationResults& valResults)
{
	// if there is a requiredif rule and it has an expr...
	if ( ruleFromPositiveCatalogue->getRequiredIfRule() != nullptr && ruleFromPositiveCatalogue->getRequiredIfRule()->getExpr() != "" )
	{
		std::shared_ptr<sbx::Rule> requiredIfRule = ruleFromPositiveCatalogue->getRequiredIfRule();

		bool required { false };

		// if the requiredIf-rule is the same as the parent rule with the positiveRuleCatalogue
		if ( requiredIfRule->getRuleId() == parentRule->getRuleId() )
		{
			// since we have determined that the topLevelRule is already true, this is also true
			required = true;
		}
		else // requiredIf-rule is different from parent, so evaluate it seperately
		{
			try {
				// evaluate the requiredif rule
				mup::Value r = _execute(requiredIfRule->getExpr(), requiredIfRule->getRuleId() );

				required = (r.GetType() == 'b' && r.GetBool());

				if (r.GetType() != 'b') {
					cout << "Required Rule expr [" << requiredIfRule->getExpr() << "] not evaluating to a boolean. Setting required == false" << endl;
				}
			} catch (const mup::ParserError& e) {
				cout << "Evaluation of requiredIfRule failed: " << endl;
				sbx::mubridge::handle(e);
			}
		}

		if (required)
		{
			// if required, then go through peOids on the ruleFromPositiveCatalogue and see if they are in the parser
			for (auto peOid : ruleFromPositiveCatalogue->getProductElementOids())
			{
				// if the peOid doesn't have a value in the parser, then add kProductElementRequired if that messages isn't already there
				if ( !_parser.IsVarDefined(_VAR_NAME(peOid)) && !valResults.hasMessages(peOid, sbx::ValidationCode::kProductElementRequired))
				{
						valResults.addValidationResult(
								sbx::ValidationResult(  sbx::ValidationCode::kProductElementRequired,
														peOid,
														_VAR_NAME(peOid),
														"Værdi for [" + _GUI_NAME(peOid) + "] ikke angivet. Værdi er påkrævet når : " + requiredIfRule->getExpr(),
														requiredIfRule->getRuleId(),
														requiredIfRule->getExpr(),
														"CheckRequiredness") );
				}  /* TODO anything if ta.hasValue(peOid) == true ??? */
			}
		}
	}
}

std::shared_ptr<sbx::Constant> RuleEngine::getDefaultValue(sbx::ProductElementOid productElement)
{
	// look up the productElement oid and see in which internal map to find the constant
	// for now just assume that the product element is found in equals/enum map
	const vector<shared_ptr<Constant>>& options = _container.getOptionsList(productElement);

	if (options.size() <= 0) {
		ostringstream s;
		s << "No default value for product element oid [" << static_cast<unsigned short>(productElement) << "]!";
		cout << s.str();
		return nullptr;
	}

	for (const auto& option : options) {
		if (option->isDefault()) {
			return option;
		}
	}

	return options.back();
}

bool RuleEngine::isVarDefined(const std::string& variable) const
{
	return _parser.IsVarDefined(variable);
}

const mup::ParserX& RuleEngine::getParser() const
{
	return _parser;
}

std::string RuleEngine::_getConstFromParser(const std::string& constName)
{
	try {
		mup::var_maptype vmap = _parser.GetConst();
		for (mup::var_maptype::iterator item = vmap.begin(); item != vmap.end(); ++item) {
			if (item->first == constName) {
				mup::Value& mupConst = (mup::Value&) (*(item->second));
				return sbx::utils::formatValue(mupConst);
			}
		}
	}
	catch (const mup::ParserError &e) {
		sbx::mubridge::handle(e);
	}

	// no value found, so return dummy string
	return "<ingen værdi>";
}

std::string RuleEngine::_getVarFromParser(const std::string& constName)
{
	try {
		mup::var_maptype vmap = _parser.GetVar();
		for (mup::var_maptype::iterator item = vmap.begin(); item != vmap.end(); ++item) {
			if (item->first == constName) {
				mup::Variable& mupVar = (mup::Variable&) (*(item->second));
				return sbx::utils::formatValue(mupVar);
			}
		}
	}
	catch (const mup::ParserError &e) {
		sbx::mubridge::handle(e);
	}

	// no value found, so return dummy string
	return "<ingen værdi>";
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


bool RuleEngine::isProductElementAllowed(sbx::productelement_oid peOid) const
{
	const auto& allowedPEs = _container.getAllowedPEOids();
	return (allowedPEs.find(peOid) != allowedPEs.cend());
}


std::vector<std::string> RuleEngine::_getParametersFromParser(const std::vector<std::string>& parameters)
{
	std::vector<std::string> formattedParameters {};

	for (auto& parameterIt : parameters )
	{
		string parameter {"not found"};

		if (_parser.IsConstDefined(parameterIt))
			parameter = this->_getConstFromParser(parameterIt);
		else if (_parser.IsVarDefined(parameterIt))
			parameter = this->_getVarFromParser(parameterIt);

		formattedParameters.push_back(parameter);
	}

	return formattedParameters;
}

void RuleEngine::_printExpressionVariables(mup::ParserX& p)
{
	cout << "------------- Expressions BEGIN ------------" << endl;
	mup::var_maptype vmap = p.GetExprVar();

	for (mup::var_maptype::iterator item = vmap.begin(); item != vmap.end(); ++item)
	{
		cout << item->first << "=";
		mup::Variable var = (mup::Variable&) (*(item->second));
		cout << var << "\n";
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
    
std::string RuleEngine::_getFormattedValue(const std::shared_ptr<sbx::Constant>& c)
{
    switch(_PE(c->getProductElement()).getElementType())
    {
        case kBool:
            return sbx::utils::formatValue(c->boolValue());
            break;
        case kCurr:
        case kPercent:
            return sbx::utils::formatValue(c->doubleValue());
            break;
        case kLong:
        case kYear:
        case kMonth:
            return sbx::utils::formatValue(c->longValue());
            break;
        case kText:
            return c->stringValue();
            break;
        default:
            return c->stringValue();
    }
    
}

std::string RuleEngine::getFormattedValue(const sbx::ProductElementValue& pev)
{
    switch(_PE(pev.getProductElementOid()).getElementType())
    {
        case kBool:
            return sbx::utils::formatValue(pev.boolValue());
            break;
        case kCurr:
        case kPercent:
            return sbx::utils::formatValue(pev.doubleValue());
            break;
        case kLong:
        case kYear:
        case kMonth:
            return sbx::utils::formatValue(pev.longValue());
            break;
        case kText:
            return pev.stringValue();
            break;
        default:
            return pev.stringValue();
    }
}

sbx::ProductElement RuleEngine::_PE(sbx::productelement_oid peOid)
{
	try {
		return _container.getProductElement(peOid);
	} catch (const domain_error& e) {
		// unsigned short oid, const std::string& variableName, const std::string& guiName, sbx::ProductElementTypes productElementType, unsigned short productOid
		return sbx::ProductElement(peOid, "<unknown>", "<unknown>", sbx::ProductElementTypes::kUnknownPEType, 0);
	}
}

std::string RuleEngine::_VAR_NAME(sbx::productelement_oid peOid)
{
	try {
		return _container.getProductElement(peOid).getVariableName();
	} catch (const domain_error& e) {
		return std::string ("");
	}
}

std::string RuleEngine::_GUI_NAME(sbx::productelement_oid peOid)
{
	try {
		return _container.getProductElement(peOid).getGuiName();
	} catch (const domain_error& e) {
		return std::string ("");
	}
}

std::string RuleEngine::_indent(unsigned short depth) { return std::string(depth, ' '); }

const Koncept& sbx::RuleEngine::getKoncept(sbx::koncept_oid konceptOid)
{
	if (_container.getKoncepts().find(konceptOid) == _container.getKoncepts().cend())
		throw domain_error("Koncepts not initialised");

	return _container.getKoncepts().at(konceptOid);
}

RuleEngine::~RuleEngine()
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

