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
	KonceptInfo(const sbx::KonceptInfo& otherKonceptInfo); // copy constructor
	void addParameterValue(unsigned short parameterOid, std::string value);
	unsigned short getUnderkonceptOid() const;
	const std::map<unsigned short, std::string>& getParameterValues() const;
	bool isParameterSelected(unsigned short parameterOid) const;
	virtual ~KonceptInfo();
private:
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
