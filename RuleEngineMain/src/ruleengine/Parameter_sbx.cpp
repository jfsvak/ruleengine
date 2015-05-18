/*
 * Parameter.cpp
 *
 *  Created on: 07/05/2015
 *      Author: jfsvak
 */
#include <string>

#include "Parameter_sbx.h"

namespace sbx {

Parameter::Parameter()
		:  _oid {0},
		   _name {""},
		   _type {""},
		   _elementType {sbx::ProductElementTypes::kUnknownPEType}
{

}

Parameter::Parameter(unsigned short oid, const std::string& name, const std::string& type, sbx::ProductElementTypes elementType)
		:  _oid {oid},
		   _name {name},
		   _type {type},
		   _elementType {elementType}
{

}

Parameter::Parameter(const sbx::Parameter& otherParameter)
		:  _oid {otherParameter._oid},
		   _name {otherParameter._name},
		   _type {otherParameter._type},
		   _elementType {otherParameter._elementType}
{

}

unsigned short Parameter::getOid() const {
	return _oid;
}

const std::string& Parameter::getName() const
{
	return _name;
}

const std::string& Parameter::getType() const
{
	return _type;
}

sbx::ProductElementTypes Parameter::getElementType() const
{
	return _elementType;
}

Parameter::~Parameter() {
}
}
