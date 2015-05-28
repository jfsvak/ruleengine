/*
 * ContributionStep.h
 *
 *  Created on: 28/05/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_CONTRIBUTIONSTEP_H_
#define RULEENGINE_CONTRIBUTIONSTEP_H_

namespace sbx {

class ContributionStep {
public:
	ContributionStep();
	ContributionStep(int index, double employeePct, double companyPct);

	bool operator < (const sbx::ContributionStep& otherStep) const;

	double employeePct() const;
	double companyPct() const;
	int index() const;

	virtual ~ContributionStep();
private:
	int _index;
	double _employeePct;
	double _companyPct;
};

} /* namespace sbx */

#endif /* RULEENGINE_CONTRIBUTIONSTEP_H_ */
