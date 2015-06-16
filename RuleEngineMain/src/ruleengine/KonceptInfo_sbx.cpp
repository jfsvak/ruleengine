/*
 * sbxFirma.cpp
 *
 *  Created on: 15/12/2014
 *      Author: xjes
 */

#include "KonceptInfo_sbx.h"

namespace sbx {

KonceptInfo::KonceptInfo()
		: _konceptOid {undefined_oid},
		  _numberOfEmployees {0},
		  _numberOfRiskGroupC {0}
{}

KonceptInfo::KonceptInfo(sbx::koncept_oid konceptOid, sbx::number_of_employees numberOfEmployees, sbx::number_of_employees numberOfRiskClassC, const std::map<unsigned short, std::string>& parameterValues)
		: _konceptOid {konceptOid},
		  _numberOfEmployees {numberOfEmployees},
		  _numberOfRiskGroupC {numberOfRiskClassC},
		  _parameterValues {parameterValues}
{}

void KonceptInfo::addParameterValue(sbx::parameter_oid parameterOid, std::string value)
{
	_parameterValues.insert( {parameterOid, value} );
}

const std::map<sbx::parameter_oid, std::string>& KonceptInfo::getParameterValues() const
{
	return _parameterValues;
}

bool KonceptInfo::isParameterSelected(sbx::parameter_oid parameterOid) const
{
	return (_parameterValues.find(parameterOid) != _parameterValues.cend());
}

void sbx::KonceptInfo::addParameterValue(sbx::parameter_oid parameterOid, bool selected)
{
	_parameterValues.insert( {parameterOid, (selected) ? "true" : "false"});
}

sbx::number_of_employees sbx::KonceptInfo::getNumberOfEmployees() const
{
	return _numberOfEmployees;
}

sbx::number_of_employees sbx::KonceptInfo::getNumberOfRiskGroupC() const
{
	return _numberOfRiskGroupC;
}

sbx::koncept_oid KonceptInfo::getKonceptOid() const
{
	return _konceptOid;
}

KonceptInfo::~KonceptInfo() { }

} /* namespace sbx */

