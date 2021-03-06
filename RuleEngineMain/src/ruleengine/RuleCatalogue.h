/*
 * RuleCatalogue.h
 *
 *  Created on: 16/05/2015
 *      Author: jespe_000
 */

#ifndef RULEENGINE_RULECATALOGUE_H_
#define RULEENGINE_RULECATALOGUE_H_

#include <vector>
#include <memory>

namespace sbx {
class Rule;

class RuleCatalogue {
public:
	RuleCatalogue();
    RuleCatalogue(std::shared_ptr<sbx::Rule> parent);

	const std::vector<std::shared_ptr<sbx::Rule>>& getRules() const;
	void addRule(std::shared_ptr<sbx::Rule>);

	const std::shared_ptr<sbx::Rule>& getParent() const;
	void setParent(std::shared_ptr<sbx::Rule> parent);

	virtual ~RuleCatalogue();

private:
    std::vector<std::shared_ptr<sbx::Rule>> _rules;
    std::shared_ptr<sbx::Rule> _parent;
};

} /* namespace sbx */

#endif /* RULEENGINE_RULECATALOGUE_H_ */
