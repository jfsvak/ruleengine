/*
 * ContributionStep.cpp
 *
 *  Created on: 28/05/2015
 *      Author: jfsvak
 */

#include "UnionAgreementContributionStep.h"

namespace sbx {

UnionAgreementContributionStep::UnionAgreementContributionStep()
		: _oid {0},
		  _employeePct {0},
		  _totalPct {0},
		  _year {0},
		  _type {""},
		  _startDate {""}
{}

UnionAgreementContributionStep::UnionAgreementContributionStep(int oid, double employeePct, double totalPct, int year, const std::string& type, const std::string& startDate)
		: _oid {oid},
		  _employeePct {employeePct},
		  _totalPct {totalPct},
		  _year {year},
		  _type {type},
		  _startDate {startDate}
{}

int UnionAgreementContributionStep::oid() const { return _oid;}
double UnionAgreementContributionStep::employeePct() const{ return _employeePct; }
double UnionAgreementContributionStep::totalPct() const { return _totalPct; }
int UnionAgreementContributionStep::year() const { return _year;}
const std::string& UnionAgreementContributionStep::type() const { return _type;}
const std::string& UnionAgreementContributionStep::startDate() const { return _startDate;}

// Comparator operator overload
bool UnionAgreementContributionStep::operator < (const sbx::UnionAgreementContributionStep& otherStep) const
{
	return this->_oid < otherStep.oid();
}


UnionAgreementContributionStep::~UnionAgreementContributionStep()
{}

} /* namespace sbx */
