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
#include <iterator>
#include <string>
#include <memory>
#include <numeric>
#include <set>

#include "RuleEngine.h"
#include "ProductElement.h"
#include "sbxTypes.h"
#include "muParser/mpParser.h"
#include "../json/json.h"
#include "KonceptInfo.h"
#include "PrintUtils.h"
#include "TA.h"

using namespace std;
using namespace mup;

namespace sbx {

bool RuleEngine::_printDebug = false;

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
 * Initialises local context that is used for following operations on RuleEngine
 */
void RuleEngine::initContext(short underkoncept_oid, short unionagreement_oid)
{
	_container.initContext(underkoncept_oid, unionagreement_oid);
}

/**
 * Initialises local context that is used for following operations on RuleEngine
 */
void sbx::RuleEngine::initContext(const sbx::KonceptInfo& ki)
{
	_ki = ki;
	_container.initContext(ki.getUnderkonceptOid(), 0);
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
 *
 */
void RuleEngine::loadRuleCatalogue()
{
	_ruleCatalogue[kTaeSpaendBl] = {	"rc_taespaendbl_max >= (a-b)"};
	_ruleCatalogue[kTaeBlGrMin] = {	"taeblgrmin <= taebloblmax"};
}

/**
 *
 */
void RuleEngine::loadRuleConstants(ParserX &p)
{
	// load rule constants - for now, set dummy values
	ProductElement pe { kTaeBlGrMin, "taeblgrmin", "gui", kCurr, sbx::ProductOid::kP_194 };

//	rc_cont["rc_taeblgrmin_min"] = MakeRuleConstant(pe, kMin, 100000);
//	rc_cont["rc_taeblgrmin_max"] = MakeRuleConstant(pe, kMax, 800000);
//	rc_cont["rc_taebloblmax_min"] = MakeRuleConstant(pe, kMin, 100000);
//	rc_cont["rc_taebloblmax_max"] = MakeRuleConstant(pe, kMax, 800000);

	pe =
	{	kTaeSpaendBl, "taespaendbl", "gui", kCurr, sbx::ProductOid::kP_194};
//	  rc_cont["rc_taespaendbl_max"] = MakeRuleConstant(pe, kMax, "200000");

	try
	{
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
	} catch (ParserError &e)
	{
		cout << "Exception: [" << e.GetCode() << "]-[" << e.GetMsg() << "]-[" << e.GetExpr() << "]" << endl;
	}
}

/*
 * Validates a single productElementValue using rules specific for the productElementType
 */
int RuleEngine::validate(const sbx::ProductElementValue& peValue)
{
	switch (_container.getProductElement(peValue.getProductElementOid()).getElementType())
	{
	case kText:
	{
		// check in optionsList if this value is allowed
		const vector<string>& v = _container.getOptions(peValue.getProductElementOid());
		cout << "Options for [" << peValue.getProductElementOid() << "] - ";
		printVector(v);
		cout << endl;
		const auto& result = find(v.cbegin(), v.cend(), peValue.stringValue());

		if (result != v.cend())
			cout << "Found at " << (result - v.cbegin()) << endl;
		else
			cout << "Not Found" << endl;

		// if we are not at the end, we found it, so we return 1
		short int found = (result != v.cend()) ? RuleEngine::VALID : RuleEngine::INVALID;

		if (RuleEngine::_printDebug)
		{
			// print if its not valid, and print valid options for this context
			if (found == RuleEngine::VALID)
			{
				cout << "Value[" << peValue.stringValue() << "] is allowed! " << endl;
			} else
			{
				ostringstream s { };
				ostream_iterator<string> strOutput(s, ", ");
				copy(v.cbegin(), v.cend(), strOutput);
				cout << "Value[" << peValue.stringValue() << "] is not allowed! Allowed values are: [" << s.str() << "]" << endl;
			}
		}

		return found;
	}
	case kLong:
	case kCurr:
	case kPercent:
	{
		// Initialise a muParser and run the necessary evals.
		cout << "Will do double validation" << endl;
		ParserX p;
		p.EnableAutoCreateVar(true);
		// setup constants
		std::ostringstream sstream;
		sstream << _container.getConstant(peValue.getProductElementOid(), kMin)->doubleValue();
		std::string varAsString = sstream.str();
		p.SetExpr("min_constant=" + varAsString);
		p.Eval();
		sstream.str("");

		sstream << _container.getConstant(peValue.getProductElementOid(), kMax)->doubleValue();
		varAsString = sstream.str();
		p.SetExpr("max_constant=" + varAsString);
		p.Eval();
		sstream.str("");

		sstream << peValue.doubleValue();
		varAsString = sstream.str();
		p.SetExpr("variable=" + varAsString);
		p.Eval();
		sstream.str("");

		printVariables(p);
		printExpressions(p);

		string minExpr = "variable >= min_constant";
		p.SetExpr(minExpr);
		Value result = p.Eval();
		cout << "Result of min-value validation : [" << result << "]" << endl;

		string maxExpr = "variable <= max_constant";
		p.SetExpr(maxExpr);
		result = p.Eval();
		cout << "Result of max-value validation : [" << result << "]" << endl;

		break;
	}
	default:
		break;
	}

	return 0;
}

/*
 * Validates a single product element using the specific rules for that product element
 */
int RuleEngine::validate(sbx::ProductElementOid p_element, const std::vector<std::pair<std::string, long>> &p_operands)
{
	cout << "The Start of Validate" << endl;

	try
	{
		ParserX p;
		p.EnableAutoCreateVar(true);
		loadRuleConstants(p);

		for (auto & element : p_operands)
		{
			stringstream sstm;
			sstm << element.first << "=" << element.second;
			string s = sstm.str();
			cout << "Will add [" << s << "]" << endl;
			p.SetExpr(s);
			Value result = p.Eval();
			cout << "Result of adding it: [" << result << "]" << endl;
		}

		printVariables(p);

		int no_of_failures;

		for (auto & rule : _ruleCatalogue[p_element])
		{
			p.SetExpr(rule);
			Value result = p.Eval();
			printExpressions(p);
			if (!result)
			{
				no_of_failures++;
			}
		}

		cout << "The End of Validate" << endl;

		return no_of_failures;
	} catch (ParserError &e)
	{
		cout << "Exception: [" << e.GetCode() << "]-[" << e.GetMsg() << "]-[" << e.GetExpr() << "]" << endl;
	}

	return -1;
}

/*
 * Validates the generic rules for the ComparisonType
 */
int RuleEngine::validate(sbx::ComparisonTypes comparisonType) const
{
	return -1;
}

/*
 * Validates the generic rules for the ComparisonType
 */
int RuleEngine::validate(const sbx::KonceptInfo& konceptInfo, const sbx::TA& ta)
{
	// 1. check allowed products elements for the konceptinfo
	std::set<unsigned short, less<unsigned short>> allowedProductElementOids { };

	// insert all allowed product oids into one vector
	for (auto& parameterIt : konceptInfo.getParameterValues())
	{
		const std::set<unsigned short>& productElementOids = _container.getProductElementOids(parameterIt.first);
		allowedProductElementOids.insert(productElementOids.cbegin(), productElementOids.cend());
	}

	for (auto& item : ta.getValues())
	{
		//check if this product element is allowed
		const sbx::ProductElementValue& pev = item.second;

		if (allowedProductElementOids.find(pev.getProductElementOid()) == allowedProductElementOids.cend())
		{
			cout << "Product element oid [" << pev.getProductElementOid() << "] not allowed for this konceptInfo" << endl;
		}
		try
		{
			int result = this->validate(pev);
			cout << "Result of validation of pev[" << result << "]" << endl;
		} catch (exception& e)
		{
			cout << "Validation of pev failed with exception: [" << e.what() << "]" << endl;
		}
	}

	// 2. check necessary related product element values
	//    2.a. check related values
	// 3. check min values

	// 4. check max values
	// 5. check custom rules

	return 0;
}

const std::shared_ptr<sbx::Constant>& RuleEngine::getDefaultValue(sbx::ProductElementOid productElement)
{
	// look up the productElement oid and see in which internal map to find the constant
	// for now just assume that the product element is found in equals/enum map
	const vector<shared_ptr<Constant>>& options = _container.getOptionsList(productElement);

	if (options.size() <= 0)
	{
		ostringstream s;
		s << "No default value for product element oid [" << static_cast<unsigned short>(productElement) << "]!";
		throw domain_error(s.str());
	}

	for (const auto& option : options)
	{
		if (option->isDefault())
		{
			return option;
		}
	}

	return options.back();
}

void RuleEngine::printVariables(ParserX p)
{
	cout << "------------- Variables initialised BEGIN ------------" << endl;
	var_maptype vmap = p.GetVar();
	for (var_maptype::iterator item = vmap.begin(); item != vmap.end(); ++item)
	{
		cout << item->first << "=" << (Variable&) (*(item->second)) << endl;
	}
	cout << "------------- Variables initialised END ------------" << endl << endl;
}

void RuleEngine::printExpressions(ParserX p)
{
	cout << "------------- Expressions BEGIN ------------" << endl;
	var_maptype vmap = p.GetExprVar();
	for (var_maptype::iterator item = vmap.begin(); item != vmap.end(); ++item)
		cout << "  " << item->first << " =  " << (Variable&) (*(item->second)) << "\n";
	cout << "------------- Expressions END ------------" << endl;
}

} // sbx namespace end

