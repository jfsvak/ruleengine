/*
 * sbxFirma.cpp
 *
 *  Created on: 15/12/2014
 *      Author: xjes
 */

#include "KonceptInfo.h"

namespace sbx {

KonceptInfo::KonceptInfo()
		: _underkonceptOid {0}
{

}

KonceptInfo::KonceptInfo(unsigned short underkonceptOid)
		: _underkonceptOid {underkonceptOid}
{
}

KonceptInfo::KonceptInfo(unsigned short underkonceptOid, const std::map<unsigned short, std::string>& parameterValues)
		: _underkonceptOid {underkonceptOid},
		  _parameterValues {parameterValues}
{
}

KonceptInfo::KonceptInfo(const sbx::KonceptInfo& otherKonceptInfo)
		: _underkonceptOid {otherKonceptInfo._underkonceptOid},
		  _parameterValues {otherKonceptInfo._parameterValues}
{
}

void KonceptInfo::addParameterValue(unsigned short parameterOid, std::string value)
{
	_parameterValues.insert( {parameterOid, value} );
}

unsigned short KonceptInfo::getUnderkonceptOid() const
{
	return _underkonceptOid;
}

const std::map<unsigned short, std::string>& KonceptInfo::getParameterValues() const
{
	return _parameterValues;
}

bool KonceptInfo::isParameterSelected(unsigned short parameterOid) const
{
	return (_parameterValues.find(parameterOid) != _parameterValues.cend());
}

KonceptInfo::~KonceptInfo()
{
}

} /* namespace sbx */
