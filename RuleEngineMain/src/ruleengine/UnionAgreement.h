/*
 * UnionAgreement.h
 *
 *  Created on: 12/06/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_UNIONAGREEMENT_H_
#define RULEENGINE_UNIONAGREEMENT_H_

#include <string>
#include <vector>

#include "Utils.h"
#include "UnionAgreementContributionStep.h"

namespace sbx {

class UnionAgreement {
public:
	UnionAgreement();
	UnionAgreement(sbx::unionagreement_oid oid, int unionAgreementNumber, const std::string& name1, const std::string& name2);

	void addContributionStep();

	sbx::unionagreement_oid getOid() const;;
	int getUnionAgreementNumber() const;
	const std::string& getName1() const;
	const std::string& getName2() const;

	virtual ~UnionAgreement();
private:
	sbx::unionagreement_oid _oid;
	int _unionAgreementNumber;
	std::string _name1;
	std::string _name2;

	std::vector<sbx::UnionAgreementContributionStep> _uaLadder;
};

} /* namespace sbx */

#endif /* RULEENGINE_UNIONAGREEMENT_H_ */
