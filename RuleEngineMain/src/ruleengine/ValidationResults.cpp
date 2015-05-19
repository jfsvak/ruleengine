/*
 * ValidateResult.cpp
 *
 *  Created on: 15/05/2015
 *      Author: jfsvak
 */
#include "ValidationResults.h"
#include <map>
#include <vector>
#include <string>

#include "sbxTypes.h"
#include "ValidationResult.h"

namespace sbx {

std::ostream& operator<<(std::ostream& output, const sbx::ValidationResults& res)
{
	output << "No of validation results [" << res._validationResultMap.size() << "]";
	return output;
}

ValidationResults::ValidationResults()
		: _validationResultMap {}
{}

sbx::ValidationResults& ValidationResults::addValidationResult(const sbx::ValidationResult& result)
{
	_validationResultMap.insert({result.getProductElementOid(), result});
	return *this;
}

std::vector<sbx::ValidationResult> ValidationResults::getValidationResults(unsigned short peOid)
{
	std::vector<sbx::ValidationResult> v;

	const auto& pairs = _validationResultMap.equal_range(peOid); //	std::pair <std::multimap<unsigned short,std::string>::iterator, std::multimap<unsigned short,std::string>::iterator> pairs = _validationResults.equal_range(peOid);

	for ( std::multimap<unsigned short, sbx::ValidationResult>::iterator it = pairs.first; it != pairs.second; it++ )
	{
		v.push_back(it->second);
	}

	return v;
}

const std::multimap<unsigned short, sbx::ValidationResult>& ValidationResults::getValidationResults() const
{
	return _validationResultMap;
}

ValidationResults::~ValidationResults()
{
}

} /* namespace sbx */

