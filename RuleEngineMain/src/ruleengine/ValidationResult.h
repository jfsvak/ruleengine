/*
 * ValidationResult.h
 *
 *  Created on: 19/05/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_VALIDATIONRESULT_H_
#define RULEENGINE_VALIDATIONRESULT_H_

#include <iostream>
#include <string>

#include "sbxTypes.h"
#include "Utils.h"

namespace sbx {

class ValidationResult {
public:
	friend std::ostream& operator << (std::ostream&, const ValidationResult&);

	ValidationResult();
	explicit ValidationResult(unsigned short peOid);
	ValidationResult(sbx::ValidationCode code, sbx::productelement_oid peOid, const std::string& variableName = "", const std::string& message = "", const std::string& ruleId = "", const std::string& expr = "", const std::string& from = "");

	sbx::ValidationCode getValidationCode() const;
	unsigned short getProductElementOid() const;
	const std::string& getRuleId() const;
	const std::string& getMessage() const;
	const std::string& getVariableName() const;
	const std::string& getExpr() const;
	const std::string& getFrom() const;

	ValidationResult& setValidationCode(sbx::ValidationCode validationCode);
	ValidationResult& setProductElementOid(sbx::productelement_oid productElementOid);
	ValidationResult& setRuleId(const std::string& ruleId);
	ValidationResult& setMessage(const std::string& message);
	ValidationResult& setVariableName(const std::string& variableName);
	ValidationResult& setExpr(const std::string& expr);
	ValidationResult& setFrom(const std::string& from);

	virtual ~ValidationResult();

private:
	sbx::ValidationCode _code;
	unsigned short _productElementOid;
	std::string _variableName;
	// TODO rename to causedBy
	std::string _ruleId;
	// TODO rename to causedByExpr
	std::string _expr;
	std::string _message;
	std::string _from;
};

} /* namespace sbx */

#endif /* RULEENGINE_VALIDATIONRESULT_H_ */
