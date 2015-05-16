/*
 * TA.cpp
 *
 *  Created on: 08/05/2015
 *      Author: jfsvak
 */

#include "TA_sbx.h"
#include <sstream>

namespace sbx {

TA::TA()
{
	// TODO Auto-generated constructor stub

}

TA::TA(const std::map<unsigned short, sbx::ProductElementValue>& peValues)
		: _peValuesMap {peValues},
		  _cvr {""},
		  _konceptOid {0}
{
}

TA::TA(const sbx::TA& otherTA)
		: _peValuesMap {otherTA._peValuesMap},
		  _cvr {otherTA._cvr},
		  _konceptOid {otherTA._konceptOid}
{
}

sbx::TA& TA::addValue(unsigned short productElementOid, const std::string& value)
{
	ProductElementValue pev {productElementOid, value};
	_peValuesMap.insert( {productElementOid, pev } );
	return *this;
}

sbx::TA& TA::addValue(unsigned short productElementOid, const double value)
{
	std::stringstream s {};
	s << value;
	return this->addValue(productElementOid, s.str());
}

sbx::TA& TA::addValue(unsigned short productElementOid, const long value)
{
	std::stringstream s {};
	s << value;
	return this->addValue(productElementOid, s.str());
}

const std::map<unsigned short, sbx::ProductElementValue>& TA::getValues() const
{
	return _peValuesMap;
}

sbx::ProductElementValue TA::getValue(unsigned short productElementOid) const
{
	if (_peValuesMap.find(productElementOid) != _peValuesMap.cend())
	{
		return _peValuesMap.at(productElementOid);
	}

	// return empty value if not found for this ta.
	return {productElementOid, ""};
}

sbx::TA& TA::setCVR(const std::string& cvr)
{
	_cvr = cvr;
	return *this;
}

const std::string& TA::getCVR() const
{
	return _cvr;
}

sbx::TA& TA::setKonceptOid(unsigned short konceptOid)
{
	_konceptOid = konceptOid;
	return *this;
}

unsigned short TA::getKonceptOid() const
{
	return _konceptOid;
}

TA::~TA()
{
	// TODO Auto-generated destructor stub
}

} /* namespace sbx */
