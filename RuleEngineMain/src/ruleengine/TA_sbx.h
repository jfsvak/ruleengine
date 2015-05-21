/*
 * TA.h
 *
 *  Created on: 08/05/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_TA_H_
#define RULEENGINE_TA_H_

#include <map>
#include <string>

#include "ProductElementValue_sbx.h"

namespace sbx {

class TA {
public:
	TA();
	explicit TA(const sbx::TA& otherTA); // copy constructor - cannot be invoked implicitly
	TA(const std::string& cvr, unsigned short konceptOid);
	TA(const std::string& cvr, unsigned short konceptOid, const std::map<unsigned short, sbx::ProductElementValue>& peValues);

	TA& setCVR(const std::string& cvr);
	TA& setKonceptOid(unsigned short konceptOid);

	TA& setValue(unsigned short productElementOid, const std::string& value);
	TA& setValue(unsigned short productElementOid, double value);
	TA& setValue(unsigned short productElementOid, long value);
	TA& setValue(unsigned short productElementOid, bool value);

	const std::string& getCVR() const;
	unsigned short getKonceptOid() const;
	sbx::ProductElementValue& getValue(unsigned short productElementOid);
	sbx::ProductElementValue getValue(unsigned short productElementOid) const;
	const std::map<unsigned short, sbx::ProductElementValue>& getValues() const;
	bool hasValue(unsigned short productElementOid) const;

	virtual ~TA();
private:
	std::string _cvr;
	unsigned short _konceptOid;
	// map of productElement oid to productElementValues
	std::map<unsigned short, sbx::ProductElementValue> _peValuesMap;
};

} /* namespace sbx */

#endif /* RULEENGINE_TA_H_ */
