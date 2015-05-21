/*
 * Rule.h
 *
 *  Created on: 16/05/2015
 *      Author: jespe_000
 */

#ifndef RULEENGINE_RULE_H_
#define RULEENGINE_RULE_H_

#include <string>
#include <vector>
#include <memory>

namespace sbx {
class RuleCatalogue;

class Rule
{
public:
	Rule();
	Rule(const std::string& ruleId, const std::string& expr, sbx::Rule*, const std::string& _positiveMessage, const std::string& _negativeMessage);

	const std::string& getRuleId() const;
	const std::string& getExpr() const;
	const std::string& getPositiveMessage() const;
	const std::string& getNegativeMessage() const;

	std::shared_ptr<sbx::Rule> getRequiredIfRule();
	void setIsRequiredIfTrue(std::shared_ptr<sbx::Rule>);
	void addProductElementOid(unsigned short productElementOid);
	const std::vector<unsigned short>& getProductElementOids() const;

	void setRequiredExpr(const std::string& requiredExpr);
	void setPositiveRuleCatalogue(sbx::RuleCatalogue*);
	void setNegativeRuleCatalogue(sbx::RuleCatalogue*);

	sbx::RuleCatalogue* getPositiveRuleCatalogue();
	sbx::RuleCatalogue* getNegativeRuleCatalogue();

	virtual ~Rule();

private:
	std::string _ruleId;
	std::string _expr;
	std::vector<unsigned short> _productElementOids;
	std::string _positiveMessage;
	std::string _negativeMessage;

	std::shared_ptr<sbx::Rule> _requiredIfRule;

	sbx::RuleCatalogue* _positiveRuleCatalogue;
	sbx::RuleCatalogue* _negativeRuleCatalogue;
};

} /* namespace sbx */

#endif /* RULEENGINE_RULE_H_ */
