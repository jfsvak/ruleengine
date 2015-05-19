/*
 * utils.cpp
 *
 *  Created on: 18/05/2015
 *      Author: jfsvak
 */
#include "utils.h"

using namespace std;

namespace sbx {
namespace mubridge {

void handle(const mup::ParserError& e) {

	switch(e.GetCode()) {
	case mup::ecUNASSIGNABLE_TOKEN:
		// TODO maybe do something fancy if we know the error code
		cerr << "Unhandled ParserError caught:" << " Token[" << e.GetToken() << "], code[" << e.GetCode() << "], msg[" << e.GetMsg() << "], expr[" << e.GetExpr() << "]" << endl;
		break;
	default:
		cerr << "Unhandled ParserError caught:" << " Token[" << e.GetToken() << "], code[" << e.GetCode() << "], msg[" << e.GetMsg() << "], expr[" << e.GetExpr() << "]" << endl;
		break;
	}
}

void handle(const mup::ParserError& e, sbx::Rule* rule, unsigned short peOidBeingValidated, const std::string& variableName, sbx::ValidationResults& valResult) {
	sbx::ValidationResult r {peOidBeingValidated};
	r.setRuleId(rule->getRuleId());
	r.setVariableName(variableName);

	stringstream s {};
	bool handled {true};

	switch(e.GetCode())
	{
	case mup::ecUNASSIGNABLE_TOKEN:
		r.setValidationCode(sbx::ValidationCode::kValueMissing);
		s << "Value for [" << e.GetToken() << "] not found on ta";
		break;
	default:
		s << "Unhandled ParserError caught:" << " Token[" << e.GetToken() << "], code[" << e.GetCode() << "], msg[" << e.GetMsg() << "], expr[" << e.GetExpr() << "]" << endl;
		handled = false;
		break;
	}

	r.setMessage(s.str());

	if (!handled) {
		cerr << s.str() << endl;
	}

	valResult.addValidationResult(r);
}

} // namespace mubridge

} // namespace sbx
