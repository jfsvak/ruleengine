/*
 * ContributionStep.h
 *
 *  Created on: 28/05/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_UNIONAGREEMENTCONTRIBUTIONSTEP_H_
#define RULEENGINE_UNIONAGREEMENTCONTRIBUTIONSTEP_H_

#include <string>

#include "Utils.h"

namespace sbx {

class UnionAgreementContributionStep {
public:
	UnionAgreementContributionStep();
	UnionAgreementContributionStep(int oid, double employeePct, double totalPct, int year, const std::string& type, const std::string& startDate);

	bool operator < (const sbx::UnionAgreementContributionStep& otherStep) const;
	int oid() const;
	double employeePct() const;
	double totalPct() const;
	int year() const;
	const std::string& type() const;
	const std::string& startDate() const;

	virtual ~UnionAgreementContributionStep();
private:
	int _oid;
	double _employeePct;
	double _totalPct;
	int _year;
	std::string _type;
	std::string _startDate;
};

} /* namespace sbx */

#endif /* RULEENGINE_UNIONAGREEMENTCONTRIBUTIONSTEP_H_ */
