/*
 * UnionAgreement.cpp
 *
 *  Created on: 12/06/2015
 *      Author: jfsvak
 */

#include "UnionAgreement_sbx.h"

namespace sbx {

UnionAgreement::UnionAgreement()
		: _oid {undefined_oid},
		  _unionAgreementNumber{0},
		  _name{""},
		  _name2{""}
{
	// TODO Auto-generated constructor stub

}

UnionAgreement::UnionAgreement(sbx::unionagreement_oid oid, int unionAgreementNumber, const std::string& name, const std::string& name2)
		: _oid {oid},
		  _unionAgreementNumber{unionAgreementNumber},
		  _name{name},
		  _name2{name2}
{
}

void UnionAgreement::addContributionStep(const UnionAgreementContributionStep& step)
{
	_uaLadder.insert(step);
}

sbx::unionagreement_oid UnionAgreement::getOid() const
{
	return _oid;
}

int UnionAgreement::getUnionAgreementNumber() const
{
	return _unionAgreementNumber;
}

const std::string& UnionAgreement::getName() const
{
	return _name;
}

const std::string& UnionAgreement::getName2() const
{
	return _name2;
}

const std::set<sbx::UnionAgreementContributionStep>& UnionAgreement::getContributionSteps() const
{
	return _uaLadder;
}

UnionAgreement::~UnionAgreement()
{
	// TODO Auto-generated destructor stub
}

} /* namespace sbx */
