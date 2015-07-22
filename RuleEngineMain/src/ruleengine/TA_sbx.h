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

#include "ContributionStep_sbx.h"
#include "EmploymentGroup_sbx.h"
#include "ProductElementValue_sbx.h"

namespace sbx {

class TA {
public:
	TA();
	explicit TA(const sbx::TA& otherTA); // copy constructor - cannot be invoked implicitly
	TA(const std::string& cvr);
	TA(const std::string& cvr, const std::map<unsigned short, sbx::ProductElementValue>& peValues);
	// delete the two constructors with konceptOid when ipad is ready
//	TA(const std::string& cvr, unsigned short konceptOid);
//	TA(const std::string& cvr, unsigned short konceptOid, const std::map<unsigned short, sbx::ProductElementValue>& peValues);

	TA& setCvr(const std::string& cvr);
	TA& setValue(unsigned short productElementOid, const char* value);
	TA& setValue(unsigned short productElementOid, bool value);
	TA& setValue(unsigned short productElementOid, const std::string& value);
	TA& setValue(unsigned short productElementOid, double value);
	TA& setValue(unsigned short productElementOid, long value);
	TA& setValue(unsigned short productElementOid, int value);
	TA& addContributionStep(const sbx::ContributionStep&);
	TA& setContributionSteps(const std::vector<sbx::ContributionStep>&);
	TA& removeContributionStep(const sbx::ContributionStep& step);
	TA& removeContributionSteps();

	std::multiset<sbx::ContributionStep> getContributionLadder() const;

	const std::string& getCVR() const;
	sbx::ProductElementValue& getValue(unsigned short productElementOid);
	sbx::ProductElementValue getValue(unsigned short productElementOid) const;
	const std::map<unsigned short, sbx::ProductElementValue>& getValues() const;
	const std::string& getInceptionDate() const;

	void remove(unsigned short peOid);
	bool hasValue(unsigned short productElementOid) const;


	virtual ~TA();
//	sbx::UnionAgreementRelationship getUar() const;
//	TA& setUar(sbx::UnionAgreementRelationship uar);
//	unsigned short getUnionAgreementOid() const;
//	TA& setUnionAgreementOid(unsigned short unionAgreementOid);

private:
	std::string _cvr;
	// map of productElement oid to productElementValues
	std::map<unsigned short, sbx::ProductElementValue> _peValuesMap;
	std::multiset<sbx::ContributionStep> _ladder;
};

} /* namespace sbx */

#endif /* RULEENGINE_TA_H_ */
