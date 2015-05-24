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
		cout << "\n**** Undefined token ParserError caught:" << " Token[" << e.GetToken() << "], code[" << e.GetCode() << "], msg[" << e.GetMsg() << "], expr[" << e.GetExpr() << "]" << endl;
		break;
	default:
		cout << "\n**** Unhandled ParserError caught:" << " Token[" << e.GetToken() << "], code[" << e.GetCode() << "], msg[" << e.GetMsg() << "], expr[" << e.GetExpr() << "]" << endl;
		break;
	}
}

void handleAsWarning(const mup::ParserError& e, sbx::ValidationResults& valResult, const sbx::RuleConstantContainer& rcc) {
	//sbx::ValidationCode code, unsigned short peOid, const std::string& variableName = "", const std::string& message = "", const std::string& ruleId = "", const std::string& expr = ""
	sbx::ValidationResult r {sbx::ValidationCode::kUnknownValidationCode, rcc.getProductElementOid(e.GetToken()), e.GetToken(), "", "", e.GetExpr()};

	stringstream s {};
	bool handled = true;

	switch(e.GetCode())
	{
	case mup::ecUNASSIGNABLE_TOKEN:
		r.setValidationCode(sbx::ValidationCode::kTokenNotDefined);
		s << "Token '" << e.GetToken() << "' not found on in parser: Msg[" << e.GetMsg() << "]";
		break;
	default:
		s << "Unhandled ParserError caught:" << " Token[" << e.GetToken() << "], code[" << e.GetCode() << "], msg[" << e.GetMsg() << "], expr[" << e.GetExpr() << "]" << endl;
		handled = false;
		break;
	}

	r.setMessage(s.str());

	if (!handled) {
		cout << "\n**** " << s.str() << endl;
	}

	valResult.addWarning(r);
}

} // namespace mubridge

} // namespace sbx
