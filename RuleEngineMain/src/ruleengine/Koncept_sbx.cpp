/*
 * Koncept_sbx.cpp
 *
 *  Created on: 12/06/2015
 *      Author: jfsvak
 */

#include "Koncept_sbx.h"

using namespace std;

namespace sbx {

Koncept::Koncept(sbx::koncept_oid oid, const std::string& name)
		: _oid {oid},
		  _name {name}
{}

void Koncept::addSubKoncept(const sbx::Subkoncept& subkoncept)
{
	_subkoncepts.insert(std::make_pair(subkoncept.getMaxNumberOfEmployees(), subkoncept));
}

const sbx::Subkoncept& Koncept::getSubkoncept(sbx::number_of_employees noOfEmployees) const
{
	for ( auto& entry : _subkoncepts )
	{
		if (entry.first > noOfEmployees )
			return entry.second;
	}
	stringstream ss{};
	ss << noOfEmployees;
	throw domain_error("Subkoncept oid [" + ss.str() + "] not found!");
}

const std::map<sbx::max_number_of_employees, sbx::Subkoncept>& Koncept::getSubkoncepts() const
{
	return _subkoncepts;
}

sbx::koncept_oid Koncept::getOid() const
{
	return _oid;
}

const std::string& Koncept::getName() const
{
	return _name;
}

Koncept::~Koncept()
{
	// TODO Auto-generated destructor stub
}

} /* namespace sbx */
