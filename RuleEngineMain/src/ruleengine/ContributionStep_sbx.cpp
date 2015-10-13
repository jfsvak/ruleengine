/*
 * ContributionStep.cpp
 *
 *  Created on: 28/05/2015
 *      Author: jfsvak
 */

#include "ContributionStep_sbx.h"

namespace sbx {

ContributionStep::ContributionStep()
		: _index {0},
		  _employeePct {0},
		  _companyPct {0}
{}

ContributionStep::ContributionStep(int index, double employeePct, double companyPct)
		: _index {index},
		  _employeePct {employeePct},
		  _companyPct {companyPct}
{}

int ContributionStep::index() const { return _index;}
double ContributionStep::employeePct() const{ return _employeePct; }
double ContributionStep::companyPct() const { return _companyPct; }
double ContributionStep::totalPct() const { return _employeePct + _companyPct; }
// Comparator operator overload
bool ContributionStep::operator < (const sbx::ContributionStep& otherStep) const
{
	return this->_index < otherStep.index();
}


ContributionStep::~ContributionStep()
{}

} /* namespace sbx */
