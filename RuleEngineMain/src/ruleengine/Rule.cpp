/*
 * Rule.cpp
 *
 *  Created on: 16/05/2015
 *      Author: jespe_000
 */

#include "Rule.h"

#include <string>
#include <vector>

namespace sbx {

Rule::Rule()
		: 	_ruleId { "" },
			_expr { "" },
			_requiredIfRule { nullptr },
			_positiveMessage { "" },
			_negativeMessage { "" },
			_preCalcExpr { "" }
{
}

Rule::Rule(const std::string& ruleId, const std::string& expr, std::shared_ptr<sbx::Rule> requiredifRule, const std::string& positiveMessage, const std::string& negativeMessage, const std::string& preCalcExpr)
		: 	_ruleId { ruleId },
			_expr { expr },
			_requiredIfRule { requiredifRule },
			_positiveMessage { positiveMessage },
			_negativeMessage { negativeMessage },
			_preCalcExpr { preCalcExpr }
{
}

const std::string& Rule::getRuleId() const
{
	return _ruleId;
}

const std::string& Rule::getExpr() const
{
	return _expr;
}

const std::string& Rule::getPreCalcExpr() const
{
	return _preCalcExpr;
}

const std::string& Rule::getNegativeMessage() const
{
	return _negativeMessage;
}

const std::string& Rule::getPositiveMessage() const
{
	return _positiveMessage;
}

sbx::RuleCatalogue* Rule::getNegativeRuleCatalogue()
{
	return _negativeRuleCatalogue;
}

sbx::RuleCatalogue* Rule::getPositiveRuleCatalogue()
{
	return _positiveRuleCatalogue;
}

void Rule::setPositiveRuleCatalogue(sbx::RuleCatalogue* ruleCatalogue)
{
	_positiveRuleCatalogue = ruleCatalogue;
}

void Rule::setNegativeRuleCatalogue(sbx::RuleCatalogue* ruleCatalogue)
{
	_negativeRuleCatalogue = ruleCatalogue;
}

const std::vector<unsigned short>& Rule::getProductElementOids() const
{
	return _productElementOids;
}

std::shared_ptr<sbx::Rule> Rule::getRequiredIfRule()
{
	return _requiredIfRule;
}

//void Rule::setIsRequiredIfTrue(std::shared_ptr<sbx::Rule> rule)
//{
//	_requiredIfRule = rule;
//}

void Rule::addProductElementOid(unsigned short productElementOid)
{
	_productElementOids.push_back(productElementOid);
}

void Rule::addPreCalcRequiredPEOid(unsigned short requiredPEOid)
{
	_preCalcRequiredPEOids.push_back(requiredPEOid);
}

const std::vector<unsigned short>& Rule::getPreCalcRequiredPEOids() const
{
	return _preCalcRequiredPEOids;
}

Rule::~Rule()
{
	delete _positiveRuleCatalogue;
	delete _negativeRuleCatalogue;
}

unsigned short Rule::getNegativeValCode() const
{
	return _negativeValCode;
}

std::shared_ptr<sbx::Rule> Rule::getNotAllowedIfRule()
{
	return _notAllowedIfRule;
}

void Rule::setNotAllowedIfRule(std::shared_ptr<sbx::Rule> notAllowedIfRule)
{
	_notAllowedIfRule = notAllowedIfRule;
}

void Rule::setNegativeValCode(unsigned short negativeValCode)
{
	_negativeValCode = negativeValCode;
}

unsigned short Rule::getPositiveValCode() const
{
	return _positiveValCode;
}

void Rule::setPositiveValCode(unsigned short positiveValCode)
{
	_positiveValCode = positiveValCode;
}

const std::vector<std::string>& Rule::getNegativeMessageParameters() const
{
	return _negativeMessageParameters;
}

void Rule::setNegativeMessageParameters(const std::vector<std::string>& negativeMessageParameters)
{
	_negativeMessageParameters = negativeMessageParameters;
}

void Rule::addNegativeMessageParameter(const std::string& parameter)
{
	_negativeMessageParameters.push_back(parameter);
}

void Rule::setPositiveMessageParameters(const std::vector<std::string>& positiveMessageParameters)
{
	_positiveMessageParameters = positiveMessageParameters;
}

const std::vector<std::string>& Rule::getPositiveMessageParameters() const
{
	return _positiveMessageParameters;
}

void Rule::addPositiveMessageParameter(const std::string& parameter)
{
	_positiveMessageParameters.push_back(parameter);
}

std::shared_ptr<sbx::Rule> Rule::getEvaluateExprIfRule() const
{
	return _evaluateExprIfRule;
}

void Rule::setEvaluateExprIfRule(std::shared_ptr<sbx::Rule> evaluateExprIfRule)
{
	_evaluateExprIfRule = evaluateExprIfRule;
}

} /* namespace sbx */
