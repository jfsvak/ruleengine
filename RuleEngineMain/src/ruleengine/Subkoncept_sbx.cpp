/*
 * Subkoncept_sbx.cpp
 *
 *  Created on: 12/06/2015
 *      Author: jfsvak
 */

#include "Subkoncept_sbx.h"

#include "Utils.h"

using namespace std;

namespace sbx {

Subkoncept::Subkoncept()
			: _oid {undefined_oid},
			  _name {""},
			  _minNumberOfEmployees {0},
			  _maxNumberOfEmployees {0},
			  _relatedSubkonceptOid {undefined_oid}
{}

Subkoncept::Subkoncept(sbx::subkoncept_oid oid, const string& name, sbx::number_of_employees minNumberOfEmployees, sbx::number_of_employees maxNumberOfEmployees, sbx::subkoncept_oid relatedSubkonceptOid)
			: _oid {oid},
			  _name {name},
			  _minNumberOfEmployees {minNumberOfEmployees},
			  _maxNumberOfEmployees {maxNumberOfEmployees},
			  _relatedSubkonceptOid {relatedSubkonceptOid}
{}

void Subkoncept::addAllowedParameter(sbx::parameter_oid parameterOid)
{
	_allowedParameters.insert(parameterOid);
}

sbx::subkoncept_oid Subkoncept::getOid() const
{
	return _oid;
}

const string& Subkoncept::getName() const
{
	return _name;
}

sbx::number_of_employees Subkoncept::getMinNumberOfEmployees() const
{
	return _minNumberOfEmployees;
}

sbx::max_number_of_employees Subkoncept::getMaxNumberOfEmployees() const
{
	return _maxNumberOfEmployees;
}

sbx::subkoncept_oid Subkoncept::getRelatedSubkonceptOid() const
{
	return _relatedSubkonceptOid;
}

const std::set<sbx::parameter_oid>& Subkoncept::getAllowedParameters() const
{
	return _allowedParameters;
}

Subkoncept::~Subkoncept()
{
	// TODO Auto-generated destructor stub
}

} /* namespace sbx */
