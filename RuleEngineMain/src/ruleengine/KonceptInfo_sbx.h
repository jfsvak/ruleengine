/*
 * sbxFirma.h
 *
 *  Created on: 15/12/2014
 *      Author: xjes
 */

#ifndef RULEENGINE_KONCEPTINFO_H_
#define RULEENGINE_KONCEPTINFO_H_

#include <map>
#include <string>

#include "Utils.h"

namespace sbx {

class KonceptInfo {
public:
	KonceptInfo();
	KonceptInfo(sbx::koncept_oid konceptOid, sbx::number_of_employees numberOfEmployees, sbx::number_of_employees numberOfRiskClassC, const std::map<sbx::parameter_oid, std::string>& parameterValues);

	void addParameterValue(sbx::parameter_oid parameterOid, std::string value);
	void addParameterValue(sbx::parameter_oid parameterOid, bool selected);
	const std::map<sbx::parameter_oid, std::string>& getParameterValues() const;
	unsigned short getNumberOfEmployees() const;
	unsigned short getNumberOfRiskGroupC() const;
	sbx::koncept_oid getKonceptOid() const;
	bool isParameterSelected(sbx::parameter_oid parameterOid) const;

	virtual ~KonceptInfo();
private:
	// Number of employees in this koncept
	sbx::number_of_employees _numberOfEmployees;

	// Number of employees in the risk group C
	sbx::number_of_employees _numberOfRiskGroupC;

	// oid from basedata for koncept
	sbx::koncept_oid _konceptOid;

	/**
	 *  map of parameter oid to value (string)
	 *  first: parameterOid
	 *  second: value
	 */
	std::map<sbx::parameter_oid, std::string> _parameterValues;
};

} /* namespace sbx */

#endif /* RULEENGINE_KONCEPTINFO_H_ */
