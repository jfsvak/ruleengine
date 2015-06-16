/*
 * Subkoncept_sbx.h
 *
 *  Created on: 12/06/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_SUBKONCEPT_SBX_H_
#define RULEENGINE_SUBKONCEPT_SBX_H_

#include <set>

#include "Utils.h"

namespace sbx {

class Subkoncept {
public:
	Subkoncept();
	Subkoncept(sbx::subkoncept_oid oid, const std::string& name, sbx::number_of_employees minNumberOfEmployees, sbx::number_of_employees maxNumberOfEmployees, sbx::subkoncept_oid relatedSubkonceptOid);

	void addAllowedParameter(sbx::parameter_oid);

	sbx::subkoncept_oid getOid() const;
	const std::string& getName() const;
	sbx::number_of_employees getMinNumberOfEmployees() const;
	sbx::max_number_of_employees getMaxNumberOfEmployees() const;
	sbx::subkoncept_oid getRelatedSubkonceptOid() const;
	const std::set<sbx::parameter_oid>& getAllowedParameters() const;

	virtual ~Subkoncept();
private:
	sbx::subkoncept_oid _oid;
	std::string _name;
	sbx::number_of_employees _minNumberOfEmployees;
	sbx::number_of_employees _maxNumberOfEmployees;
	sbx::subkoncept_oid _relatedSubkonceptOid;

	std::set<sbx::parameter_oid> _allowedParameters;
};

} /* namespace sbx */

#endif /* RULEENGINE_SUBKONCEPT_SBX_H_ */
