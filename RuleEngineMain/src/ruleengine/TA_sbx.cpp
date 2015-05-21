/*
 * TA.cpp
 *
 *  Created on: 08/05/2015
 *      Author: jfsvak
 */

#include "TA_sbx.h"

#include <iomanip>
#include <map>
#include <string>
#include <sstream>

#include "ProductElementValue_sbx.h"

namespace sbx {

TA::TA()
		: _cvr {""},
		  _konceptOid {0}
{}

TA::TA(const std::string& cvr, unsigned short konceptOid)
		:  _cvr {cvr},
		   _konceptOid {konceptOid}
{}

TA::TA(const std::string& cvr, unsigned short konceptOid, const std::map<unsigned short, sbx::ProductElementValue>& peValues)
		: _cvr {cvr},
		  _konceptOid {konceptOid},
		  _peValuesMap {peValues}
{}

TA::TA(const sbx::TA& otherTA)
		: _cvr {otherTA._cvr},
		  _konceptOid {otherTA._konceptOid},
		  _peValuesMap {otherTA._peValuesMap}
{}

TA& TA::setValue(unsigned short productElementOid, const std::string& value)
{
	_peValuesMap[productElementOid] = sbx::ProductElementValue {productElementOid, value};
	return *this;
}

TA& TA::setValue(unsigned short productElementOid, double value)
{
	// TODO fix precision conversion of double - or find better way to convert long to string
	std::stringstream s {};
	s << value;
	return this->setValue(productElementOid, s.str());
}

TA& TA::setValue(unsigned short productElementOid, long value)
{
	// TODO xjes find better way to convert long to string
	std::stringstream s {};
	s << value;
	return this->setValue(productElementOid, s.str());
}

TA& TA::setValue(unsigned short productElementOid, bool value)
{
	std::stringstream s {};
	s << std::boolalpha << value;
	return this->setValue(productElementOid, s.str());
}

/**
 * Gets a reference to the product element in this ta (not-copy).
 * Can be used to update value inside the product element.
 * If no value was found for the peOid, a new value is created and returned.
 */
sbx::ProductElementValue& TA::getValue(unsigned short productElementOid)
{
	// if not found in the map, insert a new element into the map
	if (_peValuesMap.find(productElementOid) == _peValuesMap.cend()) {
		_peValuesMap.insert( std::pair<unsigned short, sbx::ProductElementValue>(productElementOid, {productElementOid, ""} ));
	}

	return _peValuesMap[productElementOid];
}

/**
 * Gets the productElementValue for the oid from this ta (copy).
 * If no value was found for the peOid, a new dummy value is created.
 */
sbx::ProductElementValue TA::getValue(unsigned short productElementOid) const
{
	if (_peValuesMap.find(productElementOid) != _peValuesMap.cend()) {
		return _peValuesMap.at(productElementOid);
	}

	return sbx::ProductElementValue(productElementOid, "");
}

TA& TA::setCVR(const std::string& cvr)
{
	_cvr = cvr;
	return *this;
}

TA& TA::setKonceptOid(unsigned short konceptOid)
{
	_konceptOid = konceptOid;
	return *this;
}

const std::map<unsigned short, sbx::ProductElementValue>& TA::getValues() const { return _peValuesMap; }
const std::string& TA::getCVR() const { return _cvr; }
unsigned short TA::getKonceptOid() const { return _konceptOid; }
bool sbx::TA::hasValue(unsigned short productElementOid) const { return (_peValuesMap.find(productElementOid) != _peValuesMap.cend()); }

TA::~TA() {}

} /* namespace sbx */
