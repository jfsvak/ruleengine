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
			_negativeMessage { "" }
{
}

Rule::Rule(const std::string& ruleId, const std::string& expr, std::shared_ptr<sbx::Rule> requiredifRule, const std::string& positiveMessage, const std::string& negativeMessage)
		: 	_ruleId { ruleId },
			_expr { expr },
			_requiredIfRule { requiredifRule },
			_positiveMessage { positiveMessage },
			_negativeMessage { negativeMessage }
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

void Rule::setIsRequiredIfTrue(std::shared_ptr<sbx::Rule> rule)
{
	_requiredIfRule = rule;
}

void Rule::addProductElementOid(unsigned short productElementOid)
{
	_productElementOids.push_back(productElementOid);
}

Rule::~Rule()
{
	delete _positiveRuleCatalogue;
	delete _negativeRuleCatalogue;
}

} /* namespace sbx */
