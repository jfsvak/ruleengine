/*
 * TA.cpp
 *
 *  Created on: 08/05/2015
 *      Author: jfsvak
 */

#include "TA_sbx.h"

namespace sbx {

TA::TA()
{
	// TODO Auto-generated constructor stub

}

TA::TA(const std::map<unsigned short, sbx::ProductElementValue>& peValues)
		: _peValuesMap {peValues}
{
}

TA::TA(const sbx::TA& otherTA)
		: _peValuesMap {otherTA._peValuesMap}
{
}

void TA::addValue(unsigned short productElementOid, const std::string& value)
{
	ProductElementValue pev {productElementOid, value};
	_peValuesMap.insert( {productElementOid, pev } );
}

const std::map<unsigned short, sbx::ProductElementValue>& TA::getValues() const
{
	return _peValuesMap;
}

const sbx::ProductElementValue& TA::getValue(unsigned short productElementOid) const
{
	if (_peValuesMap.find(productElementOid) != _peValuesMap.cend())
	{
		return _peValuesMap.at(productElementOid);
	}

	// return empty value if not found for this ta.
	return {productElementOid, ""};
}

TA::~TA()
{
	// TODO Auto-generated destructor stub
}

} /* namespace sbx */
