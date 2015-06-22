/*
 * ContributionStep.cpp
 *
 *  Created on: 28/05/2015
 *      Author: jfsvak
 */

#include "UnionAgreementContributionStep_sbx.h"

namespace sbx {

std::ostream& operator << (std::ostream& output, const UnionAgreementContributionStep& step)
{
	std::stringstream ss{};
	ss << "oid[" << step.oid() << "],";
	ss << "Type[" << step.type() << "]";
	ss << "Year[" << step.year() << "]";
	ss << "Total[" << step.totalPct() << "]";
	ss << "Employee[" << step.employeePct() << "]";
	ss << "StartDate[" << step.startDate() << "]";
	output << ss.str();
	return output;
}

UnionAgreementContributionStep::UnionAgreementContributionStep(int oid)
		: _oid{oid}
{
}

UnionAgreementContributionStep::UnionAgreementContributionStep()
		: _oid {0},
		  _employeePct {0},
		  _totalPct {0},
		  _year {0},
		  _type {""},
		  _startDate{"01.01.2008"}
{}

UnionAgreementContributionStep::UnionAgreementContributionStep(int oid, double employeePct, double totalPct, int year, const std::string& type, const sbx::Date& startDate)
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
const sbx::Date& UnionAgreementContributionStep::startDate() const { return _startDate;}

// Comparator operator overload
bool UnionAgreementContributionStep::operator < (const sbx::UnionAgreementContributionStep& otherStep) const
{
	if (this != &otherStep) {
		return this->_oid < otherStep._oid;
	}
	return false;
}


UnionAgreementContributionStep::~UnionAgreementContributionStep()
{}

} /* namespace sbx */
