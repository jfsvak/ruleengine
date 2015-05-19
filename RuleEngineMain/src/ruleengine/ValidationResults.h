/*
 * ValidateResult.h
 *
 *  Created on: 15/05/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_VALIDATIONRESULTS_H_
#define RULEENGINE_VALIDATIONRESULTS_H_

#include <map>
#include <vector>
#include <string>

#include "sbxTypes.h"
#include "ValidationResult.h"

namespace sbx {

class ValidationResults {
public:
	friend std::ostream &operator<<(std::ostream & output, const ValidationResults& res);

	ValidationResults();

	sbx::ValidationResults& addValidationResult(const sbx::ValidationResult& validationResult);

	const std::multimap<unsigned short, sbx::ValidationResult>& getValidationResults() const;
	std::vector<sbx::ValidationResult> getValidationResults(unsigned short peOid);

	virtual ~ValidationResults();
private:
	// map of product element oid to ValidationResult
	std::multimap<unsigned short, sbx::ValidationResult> _validationResultMap;
};

} /* namespace sbx */

#endif /* RULEENGINE_VALIDATIONRESULTS_H_ */
