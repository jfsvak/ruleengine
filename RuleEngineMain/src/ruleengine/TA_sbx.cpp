/*
 * TA.cpp
 *
 *  Created on: 08/05/2015
 *      Author: jfsvak
 */

#include "TA_sbx.h"

#include <iomanip>
#include <map>
#include <stdlib.h>
#include <string>
#include <sstream>

#include "ContributionStep_sbx.h"
#include "ProductElementValue_sbx.h"
#include "Utils.h"


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

TA& TA::setValue(unsigned short productElementOid, int value)
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

TA& TA::setValue(unsigned short productElementOid, const char* value)
{
	return this->setValue(productElementOid, std::string(value));
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

TA& TA::setKonceptOid(unsigned short konceptOid)
{
	_konceptOid = konceptOid;
	return *this;
}

/**
 * Removes the productElementValue for the peOid from this TA, so there is no entry in the map of values.
 */
void TA::remove(unsigned short peOid)
{
	_peValuesMap.erase(peOid);
}

// allow duplicate steps to be added to the ladder
// During validation, these duplicate steps will be validated and error messages returned
TA& TA::addContributionStep(const sbx::ContributionStep& step)
{

	// insert step
	_ladder.insert(step);

	// if this is the first step to be added, then set a 'fake' placeholder value for the peoid 999==kBidragstrappe
	if (_ladder.size() == 1)
		this->setValue(kBidragstrappe, (double) 0);

	return *this;
}

// allow duplicate steps to be added to the ladder
// During validation, these duplicate steps will be validated and error messages returned
TA& TA::removeContributionStep(const sbx::ContributionStep& step)
{
	// insert step
	_ladder.erase(step);

	// if not more steps in the ladder, remove the 'fake' placeholder pevalue
	if (_ladder.size() == 0)
		this->remove(kBidragstrappe);

	return *this;
}

std::multiset<sbx::ContributionStep> TA::getContributionLadder() const
{
	return _ladder;
}

const std::map<unsigned short, sbx::ProductElementValue>& TA::getValues() const { return _peValuesMap; }
const std::string& TA::getCVR() const { return _cvr; }
unsigned short TA::getKonceptOid() const { return _konceptOid; }
bool TA::hasValue(unsigned short productElementOid) const { return (_peValuesMap.find(productElementOid) != _peValuesMap.cend()); }
TA& TA::setCvr(const std::string& cvr) { _cvr = cvr; return *this; }

TA& TA::setContributionSteps(const std::vector<sbx::ContributionStep>& ladder)
{
	_ladder.clear();

	for (const auto& step : ladder) {
		this->addContributionStep(step);
	}

	return *this;
}

TA& TA::removeContributionSteps()
{
	_ladder.clear();
	return *this;
}

TA::~TA() {}

sbx::UnionAgreementRelationship TA::getUar() const
{
	if (this->getValue(kUnionAgreementRelationship).stringValue() == sbx::kFOLLOWS)
		return sbx::UnionAgreementRelationship::FOLLOWS;
	else if (this->getValue(kUnionAgreementRelationship).stringValue() == sbx::kOUTSIDE)
		return sbx::UnionAgreementRelationship::OUTSIDE;
	else if (this->getValue(kUnionAgreementRelationship).stringValue() == sbx::kINCLUDED)
		return sbx::UnionAgreementRelationship::INCLUDED;

	return sbx::UnionAgreementRelationship::OUTSIDE;
}

TA& TA::setUar(sbx::UnionAgreementRelationship uar)
{
	switch(uar)
	{
	case FOLLOWS:
		this->setValue(sbx::ProductElementOid::kUnionAgreementRelationship, sbx::kFOLLOWS);
		break;
	case OUTSIDE:
		this->setValue(sbx::ProductElementOid::kUnionAgreementRelationship, sbx::kOUTSIDE);
		break;
	case INCLUDED:
		this->setValue(sbx::ProductElementOid::kUnionAgreementRelationship, sbx::kINCLUDED);
		break;
	}

	return *this;
}

unsigned short TA::getUnionAgreementOid() const
{
	return this->getValue(kUnionAgreementOid).longValue();
}

TA& TA::setUnionAgreementOid(unsigned short unionAgreementOid)
{
	this->setValue(sbx::ProductElementOid::kUnionAgreementOid, (long) unionAgreementOid);
	return *this;
}

} /* namespace sbx */
