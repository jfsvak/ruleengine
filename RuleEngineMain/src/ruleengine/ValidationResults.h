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
	sbx::ValidationResults& addValidationResults(const std::vector<sbx::ValidationResult>& validationResults);
	sbx::ValidationResults& addWarning(const sbx::ValidationResult& validationResult);

	void merge(const sbx::ValidationResults&);
	const std::multimap<unsigned short, sbx::ValidationResult>& getValidationResults() const;
	const std::multimap<unsigned short, sbx::ValidationResult>& getWarnings() const;

	std::vector<sbx::ValidationResult> getValidationResults(unsigned short peOid);
	std::vector<sbx::ValidationResult> getWarnings(unsigned short peOid);


	int sizeValidationResults() const;
	int sizeWarnings() const;

	bool isAllOk() const;
	bool hasWarnings() const;

	bool hasMessages(unsigned short peOid, sbx::ValidationCode) const;
	bool hasWarnings(unsigned short peOid, sbx::ValidationCode) const;

	virtual ~ValidationResults();
private:
	void updateState(const sbx::ValidationResult& result);
	bool _hasWarnings;
	bool _allOk;

	// map of product element oid to ValidationResult
	std::multimap<unsigned short, sbx::ValidationResult> _validationResultMap;

	// map of warnings indexed by peOid
	std::multimap<unsigned short, sbx::ValidationResult> _warningsMap;

	std::vector<sbx::ValidationResult> _getSubSet(const std::multimap<unsigned short, sbx::ValidationResult>& map, unsigned short peOid);
};

} /* namespace sbx */

#endif /* RULEENGINE_VALIDATIONRESULTS_H_ */
