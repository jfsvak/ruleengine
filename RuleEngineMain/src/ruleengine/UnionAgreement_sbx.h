/*
 * UnionAgreement.h
 *
 *  Created on: 12/06/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_UNIONAGREEMENT_SBX_H_
#define RULEENGINE_UNIONAGREEMENT_SBX_H_

#include <string>
#include <vector>

#include "UnionAgreementContributionStep_sbx.h"
#include "Utils.h"

namespace sbx {

class UnionAgreement {
public:
	UnionAgreement();
	UnionAgreement(sbx::unionagreement_oid oid, int unionAgreementNumber, const std::string& name, const std::string& name2);

	void addContributionStep(const UnionAgreementContributionStep&);

	sbx::unionagreement_oid getOid() const;;
	int getUnionAgreementNumber() const;
	const std::string& getName() const;
	const std::string& getName2() const;
	const std::vector<sbx::UnionAgreementContributionStep>& getContributionSteps() const;

	virtual ~UnionAgreement();
private:
	sbx::unionagreement_oid _oid;
	int _unionAgreementNumber;
	std::string _name;
	std::string _name2;

	std::vector<sbx::UnionAgreementContributionStep> _uaLadder;
};

} /* namespace sbx */

#endif /* RULEENGINE_UNIONAGREEMENT_SBX_H_ */
