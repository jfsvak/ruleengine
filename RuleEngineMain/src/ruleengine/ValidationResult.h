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

namespace sbx {

enum class ValidationCode : unsigned short {
	kUnknown,
	kFail,
	kOK,
	kValueMissing,
	kValueNotRequired
};

class ValidationResult {
public:
	friend std::ostream& operator << (std::ostream&, const ValidationResult&);

	ValidationResult();
	explicit ValidationResult(unsigned short peOid);
//	ValidationResult(const ValidationResult&); // copy constructor
	ValidationResult(sbx::ValidationCode code, unsigned short peOid, const std::string& variableName, const std::string& ruleId, const std::string& message);

	sbx::ValidationCode getValidationCode() const;
	unsigned short getProductElementOid() const;
	const std::string& getRuleId() const;
	const std::string& getMessage() const;

	ValidationResult& setValidationCode(sbx::ValidationCode validationCode);
	ValidationResult& setProductElementOid(unsigned short productElementOid);
	ValidationResult& setRuleId(const std::string& ruleId);
	ValidationResult& setMessage(const std::string& message);

	virtual ~ValidationResult();
	const std::string& getVariableName() const;
	void setVariableName(const std::string& variableName);

private:
	sbx::ValidationCode _code;
	unsigned short _productElementOid;
	std::string _variableName;
	std::string _ruleId;
	std::string _message;
};

} /* namespace sbx */

#endif /* RULEENGINE_VALIDATIONRESULT_H_ */
