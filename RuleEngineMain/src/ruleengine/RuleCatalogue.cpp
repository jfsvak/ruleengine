/*
 * RuleCatalogue.cpp
 *
 *  Created on: 16/05/2015
 *      Author: jespe_000
 */

#include "RuleCatalogue.h"

#include <vector>

namespace sbx {

RuleCatalogue::RuleCatalogue()
		: _parent { nullptr }
{}

RuleCatalogue::RuleCatalogue(sbx::Rule* parent)
		: _parent { parent }
{
}

std::vector<sbx::Rule*> RuleCatalogue::getRules()
{
	return _rules;
}

void RuleCatalogue::addRule(sbx::Rule* rule)
{
	_rules.push_back(rule);
}

sbx::Rule* RuleCatalogue::getParent()
{
	return _parent;
}

void RuleCatalogue::setParent(sbx::Rule* parent)
{
	_parent = parent;
}

RuleCatalogue::~RuleCatalogue()
{
	// clearing the rule instances
	for (std::vector<sbx::Rule*>::iterator it = _rules.begin(); it < _rules.end(); it++)
	{
		delete *it;
	}

	delete _parent;
}

} /* namespace sbx */
