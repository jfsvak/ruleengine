/*
 * ContributionStep.h
 *
 *  Created on: 28/05/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_UNIONAGREEMENTCONTRIBUTIONSTEP_SBX_H_
#define RULEENGINE_UNIONAGREEMENTCONTRIBUTIONSTEP_SBX_H_

#include <string>

#include "Date_sbx.h"
#include "Utils.h"

namespace sbx {

class UnionAgreementContributionStep {
	friend std::ostream& operator << (std::ostream&, const sbx::UnionAgreementContributionStep& step);
public:
	UnionAgreementContributionStep();
	explicit UnionAgreementContributionStep(int oid);
	UnionAgreementContributionStep(int oid, double employeePct, double totalPct, int year, const std::string& type, const sbx::Date& startDate);

	bool operator < (const sbx::UnionAgreementContributionStep& otherStep) const;
	int oid() const;
	double employeePct() const;
	double totalPct() const;
	int year() const;
	const std::string& type() const;
	const sbx::Date& startDate() const;

	virtual ~UnionAgreementContributionStep();
private:
	int _oid;
	double _employeePct;
	double _totalPct;
	int _year;
	std::string _type;
	sbx::Date _startDate;
};

} /* namespace sbx */

#endif /* RULEENGINE_UNIONAGREEMENTCONTRIBUTIONSTEP_SBX_H_ */
