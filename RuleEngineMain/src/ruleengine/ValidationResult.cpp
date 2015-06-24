/*
 * ValidationResult.cpp
 *
 *  Created on: 19/05/2015
 *      Author: jfsvak
 */
#include "ValidationResult.h"

#include <iostream>

namespace sbx {

std::ostream& operator << (std::ostream& output, const ValidationResult& valResult) {
	output << "ValCode[" << (int) valResult.getValidationCode() << "], "
			"Variable[" << valResult.getVariableName() << " (" << valResult.getProductElementOid() << ")], " <<
			"expr[" << valResult.getExpr() << "], " <<
			"RuleId[" << valResult.getRuleId() << "], " <<
			"Msg[" << valResult.getMessage() << "] "	<<
			"From[" << valResult.getFrom() << "]";
	return output;
}

ValidationResult::ValidationResult()
		: _code { sbx::ValidationCode::kUnknownValidationCode },
		  _productElementOid {0},
		  _ruleId {""},
		  _message {""},
		  _expr {""},
		  _from {""}
{}

ValidationResult::ValidationResult(unsigned short peOid)
		: _code { sbx::ValidationCode::kUnknownValidationCode },
		  _productElementOid {peOid},
		  _ruleId {""},
		  _message {""},
		  _expr {""},
		  _from {""}
{}

/*ValidationResult::ValidationResult(const sbx::ValidationResult& other)
		: _code { other._code },
		  _productElementOid { other._productElementOid },
		  _variableName {other._variableName},
		  _ruleId {other._ruleId},
		  _message { other._message }
{}
*/
ValidationResult::ValidationResult(	sbx::ValidationCode code,
									sbx::productelement_oid peOid,
									const std::string& variableName,
									const std::string& message,
									const std::string& ruleId,
									const std::string& expr,
									const std::string& from)
		: _code { code },
		  _productElementOid {peOid},
		  _variableName {variableName},
		  _ruleId {ruleId},
		  _message {message},
		  _expr {expr},
		  _from {from}
{}


sbx::ValidationCode ValidationResult::getValidationCode() const { return _code; }
unsigned short ValidationResult::getProductElementOid() const { return _productElementOid; }
const std::string& ValidationResult::getRuleId() const { return _ruleId; }
const std::string& ValidationResult::getMessage() const { return _message; }
const std::string& ValidationResult::getVariableName() const { return _variableName; }
const std::string& ValidationResult::getExpr() const { return _expr; }
const std::string& ValidationResult::getFrom() const { return _from; }

ValidationResult& ValidationResult::setValidationCode(sbx::ValidationCode validationCode)
{
	_code = validationCode;
	return *this;
}

ValidationResult& ValidationResult::setProductElementOid(unsigned short productElementOid)
{
	_productElementOid = productElementOid;
	return *this;
}

ValidationResult& ValidationResult::setRuleId(const std::string& ruleId)
{
	_ruleId = ruleId;
	return *this;
}

ValidationResult& ValidationResult::setMessage(const std::string& message)
{
	_message = message;
	return *this;
}

sbx::ValidationResult& ValidationResult::setVariableName(const std::string& variableName)
{
	_variableName = variableName;
	return *this;
}



sbx::ValidationResult& ValidationResult::setExpr(const std::string& expr)
{
	_expr = expr;
	return *this;
}

sbx::ValidationResult& ValidationResult::setFrom(const std::string& from)
{
	_from = from;
	return *this;
}

ValidationResult::~ValidationResult()
{}

} /* namespace sbx */

