/*
 * Parameter.h
 *
 *  Created on: 07/05/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_PARAMETER_H_
#define RULEENGINE_PARAMETER_H_
#include <string>
#include "sbxTypes.h"

namespace sbx {

class Parameter {
public:
	Parameter();
	Parameter(const sbx::Parameter&); // copy constructor
	Parameter(unsigned short oid, const std::string& name, const std::string& type, sbx::ProductElementTypes);
	virtual ~Parameter();
	unsigned short getOid() const;
	const std::string& getName() const;
	const std::string& getType() const;
	sbx::ProductElementTypes getElementType() const;
private:
	unsigned short _oid;
	std::string _name;
	std::string _type;
	sbx::ProductElementTypes _elementType;
};

}

#endif /* RULEENGINE_PARAMETER_H_ */
