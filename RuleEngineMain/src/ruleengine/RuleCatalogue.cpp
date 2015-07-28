/*
 * RuleCatalogue.cpp
 *
 *  Created on: 16/05/2015
 *      Author: jespe_000
 */

#include "RuleCatalogue.h"

#include <vector>
#include <memory>


namespace sbx {

RuleCatalogue::RuleCatalogue()
		: _parent { nullptr }
{}

RuleCatalogue::RuleCatalogue(std::shared_ptr<sbx::Rule> parent)
		: _parent { parent }
{
}

const std::vector<std::shared_ptr<sbx::Rule>>& RuleCatalogue::getRules() const
{
	return _rules;
}

void RuleCatalogue::addRule(std::shared_ptr<sbx::Rule> rule)
{
	_rules.push_back(rule);
}

const std::shared_ptr<sbx::Rule>& RuleCatalogue::getParent() const
{
	return _parent;
}

void RuleCatalogue::setParent(std::shared_ptr<sbx::Rule> parent)
{
	_parent = parent;
}

RuleCatalogue::~RuleCatalogue()
{
}

} /* namespace sbx */
