/*
 * RuleCatalogue.h
 *
 *  Created on: 16/05/2015
 *      Author: jespe_000
 */

#ifndef RULEENGINE_RULECATALOGUE_H_
#define RULEENGINE_RULECATALOGUE_H_

#include <vector>

namespace sbx {
class Rule;

class RuleCatalogue {
public:
	RuleCatalogue();
	RuleCatalogue(sbx::Rule* parent);

	std::vector<sbx::Rule*> getRules();
	void addRule(sbx::Rule*);

	sbx::Rule* getParent();
	void setParent(sbx::Rule* parent);

	virtual ~RuleCatalogue();

private:
	std::vector<sbx::Rule*> _rules;
	sbx::Rule* _parent;
};

} /* namespace sbx */

#endif /* RULEENGINE_RULECATALOGUE_H_ */
