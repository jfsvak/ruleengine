/*
 * ValidateResult.h
 *
 *  Created on: 15/05/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_VALIDATIONRESULT_H_
#define RULEENGINE_VALIDATIONRESULT_H_

#include <map>
#include <vector>
#include <string>

#include "sbxTypes.h"

namespace sbx {

class ValidationResult {
public:
	friend std::ostream &operator<<(std::ostream & output, const ValidationResult& res);
	ValidationResult();
	ValidationResult &addValidationResult(sbx::ProductElementOid peOid, const std::string& result);
	ValidationResult &addValidationResult(unsigned short peOid, const std::string& result);
	std::vector<std::string> getValidationResults(sbx::ProductElementOid peOid);
	std::vector<std::string> getValidationResults(unsigned short peOid);
	virtual ~ValidationResult();
private:
	std::multimap<unsigned short, std::string> _validationResults;
};

} /* namespace sbx */

#endif /* RULEENGINE_VALIDATIONRESULT_H_ */
