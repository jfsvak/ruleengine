/*
 * Koncept_sbx.h
 *
 *  Created on: 12/06/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_KONCEPT_SBX_H_
#define RULEENGINE_KONCEPT_SBX_H_

#include <map>
#include <string>

#include "Subkoncept_sbx.h"

namespace sbx {

class Koncept {
public:
	Koncept();
	Koncept(sbx::koncept_oid oid, const std::string& name);

	void addSubKoncept(const sbx::Subkoncept&);

	const sbx::Subkoncept& getSubkoncept(sbx::number_of_employees noOfEmployees) const;
	const std::map<sbx::max_number_of_employees, sbx::Subkoncept>& getSubkoncepts() const;
	sbx::koncept_oid getOid() const;
	const std::string& getName() const;

	virtual ~Koncept();
private:
	sbx::koncept_oid _oid;
	std::string _name;
	std::map<sbx::max_number_of_employees, sbx::Subkoncept> _subkoncepts;
};

} /* namespace sbx */

#endif /* RULEENGINE_KONCEPT_SBX_H_ */
