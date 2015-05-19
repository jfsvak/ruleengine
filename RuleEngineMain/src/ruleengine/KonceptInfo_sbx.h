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

namespace sbx {

class KonceptInfo {
public:
	KonceptInfo();
	explicit KonceptInfo(unsigned short underkonceptOid);
	KonceptInfo(unsigned short underkonceptOid, const std::map<unsigned short, std::string>& parameterValues);

	void addParameterValue(unsigned short parameterOid, std::string value);
	void addParameterValue(unsigned short parameterOid, bool selected);
	unsigned short getUnderkonceptOid() const;
	const std::map<unsigned short, std::string>& getParameterValues() const;
	unsigned short getNumberOfEmployees() const;
	unsigned short getNumberOfRiskGroupC() const;
	bool isParameterSelected(unsigned short parameterOid) const;

	virtual ~KonceptInfo();
private:
	// Number of employees in this koncept
	unsigned short _numberOfEmployees;

	// Number of employees in the risk group C
	unsigned short _numberOfRiskGroupC;

	// oid from basedata for subkoncept
	unsigned short _underkonceptOid;

	/**
	 *  map of parameter oid to value (string)
	 *  first: parameterOid
	 *  second: value
	 */
	std::map<unsigned short, std::string> _parameterValues;
};

} /* namespace sbx */

#endif /* RULEENGINE_KONCEPTINFO_H_ */
