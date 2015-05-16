/*
 * TA.h
 *
 *  Created on: 08/05/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_TA_H_
#define RULEENGINE_TA_H_

#include <map>

#include "ProductElementValue_sbx.h"

namespace sbx {

class TA {
public:
	TA();
	TA(const std::map<unsigned short, sbx::ProductElementValue>& peValues);
	TA(const sbx::TA& otherTA);

	TA& setCVR(const std::string& cvr);
	const std::string& getCVR() const;
	TA& setKonceptOid(unsigned short konceptOid);
	unsigned short getKonceptOid() const;

	TA& addValue(unsigned short productElementOid, const std::string& value);
	TA& addValue(unsigned short productElementOid, double value);
	TA& addValue(unsigned short productElementOid, long value);

	sbx::ProductElementValue getValue(unsigned short productElementOid) const;
	const std::map<unsigned short, sbx::ProductElementValue>& getValues() const;

	virtual ~TA();
private:
	std::string _cvr;
	unsigned short _konceptOid;
	// map of productElement oid to productElementValues
	std::map<unsigned short, sbx::ProductElementValue> _peValuesMap;
};

} /* namespace sbx */

#endif /* RULEENGINE_TA_H_ */
