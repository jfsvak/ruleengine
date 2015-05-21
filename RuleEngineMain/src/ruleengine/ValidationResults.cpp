/*
 * ValidateResult.cpp
 *
 *  Created on: 15/05/2015
 *      Author: jfsvak
 */
#include "ValidationResults.h"

#include <iostream>
#include <map>
#include <vector>
#include <string>

#include "sbxTypes.h"
#include "ValidationResult.h"

namespace sbx {

std::ostream& operator<<(std::ostream& output, const sbx::ValidationResults& res)
{
	output << "\nValidation results [" << res._validationResultMap.size() << "]:" << std::endl;
	for (auto& item : res._validationResultMap)
	{
		output << "  " << item.second << std::endl;
	}

	output << "Warning [" << res._warningsMap.size() << "]:" << std::endl;
	for (auto& item : res._warningsMap)
	{
		output << "  " << item.second << std::endl;
	}

	output << std::endl;

	return output;
}

ValidationResults::ValidationResults()
		: _allOk {true},
		  _hasWarnings { false },
		  _validationResultMap {}
{}

sbx::ValidationResults& ValidationResults::addValidationResult(const sbx::ValidationResult& result)
{
	updateState(result);

	_validationResultMap.insert({result.getProductElementOid(), result});
	return *this;
}

sbx::ValidationResults& ValidationResults::addValidationResults(const std::vector<sbx::ValidationResult>& results)
{
	for (auto& result : results) {
		updateState(result);
		_validationResultMap.insert({result.getProductElementOid(), result});
	}

	return *this;
}

std::vector<sbx::ValidationResult> ValidationResults::getValidationResults(unsigned short peOid)
{
	return _getSubSet(_validationResultMap, peOid);
}

const std::multimap<unsigned short, sbx::ValidationResult>& ValidationResults::getValidationResults() const
{
	return _validationResultMap;
}

bool ValidationResults::isAllOk() const { return _allOk;}
bool ValidationResults::hasWarnings() const { return _hasWarnings; }

sbx::ValidationResults& ValidationResults::addWarning(const sbx::ValidationResult& validationResult)
{
	_hasWarnings = true;
	_warningsMap.insert( {validationResult.getProductElementOid(), validationResult} );
	return *this;
}

const std::multimap<unsigned short, sbx::ValidationResult>& ValidationResults::getWarnings() const
{
	return _warningsMap;
}

std::vector<sbx::ValidationResult> ValidationResults::getWarnings(unsigned short peOid)
{
	return _getSubSet(_warningsMap, peOid);
}

// merges all the ValidationResults from other into this ValidationResults
void ValidationResults::merge(const sbx::ValidationResults& other)
{
	// merge validation results
	for (auto it = other.getValidationResults().cbegin(); it != other.getValidationResults().cend(); it++) {
		this->addValidationResult(it->second);
	}

	// merge warnings
	for (auto it = other.getWarnings().cbegin(); it != other.getWarnings().cend(); it++) 	{
		this->addWarning(it->second);
	}
}

void ValidationResults::updateState(const sbx::ValidationResult& result)
{
	if (_allOk && result.getValidationCode() != sbx::ValidationCode::kOK) {
		_allOk = false;
	}
}

// private method for getting a subset of ValidationResult's in a vector
std::vector<sbx::ValidationResult> ValidationResults::_getSubSet(const std::multimap<unsigned short, sbx::ValidationResult>& map, unsigned short peOid)
{
	std::vector<sbx::ValidationResult> v {};

	const auto& pairs = map.equal_range(peOid); //	std::pair <std::multimap<unsigned short,std::string>::iterator, std::multimap<unsigned short,std::string>::iterator> pairs = _validationResults.equal_range(peOid);

	for ( auto it = pairs.first; it != pairs.second; it++ )
	{
		v.push_back(it->second);
	}

	return v;
}

ValidationResults::~ValidationResults()
{}

} /* namespace sbx */
