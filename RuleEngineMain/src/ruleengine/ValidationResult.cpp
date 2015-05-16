/*
 * ValidateResult.cpp
 *
 *  Created on: 15/05/2015
 *      Author: jfsvak
 */
#include "ValidationResult.h"

#include <map>
#include <vector>
#include <string>

#include "sbxTypes.h"

namespace sbx {

std::ostream& operator<<(std::ostream& output, const sbx::ValidationResult& res)
{
	output << "No of validation results [" << res._validationResults.size() << "]";
	return output;
}

ValidationResult::ValidationResult()
{
	// TODO Auto-generated constructor stub

}

ValidationResult& ValidationResult::addValidationResult(sbx::ProductElementOid peOid, const std::string& result)
{
	_validationResults.insert({peOid, result});
	return *this;
}

ValidationResult& ValidationResult::addValidationResult(unsigned short peOid, const std::string& result)
{
	_validationResults.insert({peOid, result});
	return *this;
}

std::vector<std::string> ValidationResult::getValidationResults(unsigned short peOid)
{
	std::vector<std::string> v;

	std::pair <std::multimap<unsigned short,std::string>::iterator, std::multimap<unsigned short,std::string>::iterator> pairs = _validationResults.equal_range(peOid);
//	const auto& pairs = _validationResults.equal_range(peOid);

	for ( std::multimap<unsigned short, std::string>::iterator it = pairs.first; it != pairs.second; it++ )
	{
		v.push_back(it->second);
	}

	return v;
}

std::vector<std::string> ValidationResult::getValidationResults(sbx::ProductElementOid peOid)
{
	return this->getValidationResults(static_cast<unsigned short>(peOid));
}

ValidationResult::~ValidationResult()
{
	// TODO Auto-generated destructor stub
}
} /* namespace sbx */