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
#include "Utils.h"

namespace sbx {

class ValidationResults {
public:
	friend std::ostream &operator<<(std::ostream & output, const ValidationResults& res);

	ValidationResults();

	sbx::ValidationResults& addValidationResult(const sbx::ValidationResult& validationResult);
	sbx::ValidationResults& addValidationResults(const std::vector<sbx::ValidationResult>& validationResults);
	sbx::ValidationResults& addWarning(const sbx::ValidationResult& validationResult);

	void merge(const sbx::ValidationResults&);
	const std::multimap<sbx::productelement_oid, sbx::ValidationResult>& getValidationResults() const;
	const std::multimap<sbx::productelement_oid, sbx::ValidationResult>& getWarnings() const;

	std::vector<sbx::ValidationResult> getValidationResults(sbx::productelement_oid);
	std::vector<sbx::ValidationResult> getWarnings(sbx::productelement_oid);

	int sizeValidationResults() const;
	int sizeWarnings() const;

	bool isAllOk() const;
	bool hasWarnings() const;

	bool hasMessages(sbx::productelement_oid) const;
	bool hasMessages(sbx::productelement_oid, sbx::ValidationCode) const;
	bool hasWarnings(sbx::productelement_oid, sbx::ValidationCode) const;

	virtual ~ValidationResults();
private:
	void updateState(const sbx::ValidationResult& result);
	bool _hasWarnings;
	bool _allOk;

	// map of product element oid to ValidationResult
	std::multimap<sbx::productelement_oid, sbx::ValidationResult> _validationResultMap;

	// map of warnings indexed by peOid
	std::multimap<sbx::productelement_oid, sbx::ValidationResult> _warningsMap;

	std::vector<sbx::ValidationResult> _getSubSet(const std::multimap<sbx::productelement_oid, sbx::ValidationResult>& map, sbx::productelement_oid);
};

} /* namespace sbx */

#endif /* RULEENGINE_VALIDATIONRESULTS_H_ */
