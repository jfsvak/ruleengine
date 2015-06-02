/*
 * TA.h
 *
 *  Created on: 08/05/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_TA_H_
#define RULEENGINE_TA_H_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "EmploymentGroup.h"
#include "ContributionStep.h"
#include "ProductElementValue_sbx.h"

namespace sbx {

class TA {
public:
	TA();
	explicit TA(const sbx::TA& otherTA); // copy constructor - cannot be invoked implicitly
	TA(const std::string& cvr, unsigned short konceptOid);
	TA(const std::string& cvr, unsigned short konceptOid, const std::map<unsigned short, sbx::ProductElementValue>& peValues);

	TA& setCvr(const std::string& cvr);
	TA& setKonceptOid(unsigned short konceptOid);
	TA& setValue(unsigned short productElementOid, const char* value);
	TA& setValue(unsigned short productElementOid, bool value);
	TA& setValue(unsigned short productElementOid, const std::string& value);
	TA& setValue(unsigned short productElementOid, double value);
	TA& setValue(unsigned short productElementOid, long value);
	TA& setValue(unsigned short productElementOid, int value);
	TA& setUar(sbx::UnionAgreementRelationship uar);
	TA& setUnionAgreementOid(unsigned short unionAgreementOid);
	TA& addContributionStep(const sbx::ContributionStep&);
	TA& setContributionSteps(const std::vector<sbx::ContributionStep>&);
	TA& removeContributionStep(const sbx::ContributionStep& step);
	TA& removeContributionSteps();

	std::multiset<sbx::ContributionStep> getContributionLadder() const;

	const std::string& getCVR() const;
	unsigned short getKonceptOid() const;
	sbx::ProductElementValue& getValue(unsigned short productElementOid);
	sbx::ProductElementValue getValue(unsigned short productElementOid) const;
	const std::map<unsigned short, sbx::ProductElementValue>& getValues() const;

	void remove(unsigned short peOid);
	bool hasValue(unsigned short productElementOid) const;

	sbx::UnionAgreementRelationship getUar() const;
	unsigned short getUnionAgreementOid() const;
	const std::string& getInceptionDate() const;

	virtual ~TA();

private:
	std::string _cvr;
	unsigned short _konceptOid;
	// map of productElement oid to productElementValues
	std::map<unsigned short, sbx::ProductElementValue> _peValuesMap;

	unsigned short _unionAgreementOid;
	sbx::UnionAgreementRelationship _uar;

	std::multiset<sbx::ContributionStep> _ladder;
};

} /* namespace sbx */

#endif /* RULEENGINE_TA_H_ */
