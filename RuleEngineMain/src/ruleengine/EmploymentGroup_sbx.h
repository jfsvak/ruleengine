/*
 * EmploymentGroup.h
 *
 *  Created on: 27/05/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_EMPLOYMENTGROUP_SBX_H_
#define RULEENGINE_EMPLOYMENTGROUP_SBX_H_

#include <string>

namespace sbx {

class EmploymentGroup {
public:
	EmploymentGroup();
	virtual ~EmploymentGroup();
private:
	unsigned short _oid;
	std::string _name;
};

} /* namespace sbx */

#endif /* RULEENGINE_EMPLOYMENTGROUP_SBX_H_ */
