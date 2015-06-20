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
		cout << "Undefined token:" << " Token[" << e.GetToken() << "], code[" << e.GetCode() << "], expr[" << e.GetExpr() << "]" << endl;
		break;
	default:
		cout << "Unhandled ParserError caught:" << " Token[" << e.GetToken() << "], code[" << e.GetCode() << "], msg[" << e.GetMsg() << "], expr[" << e.GetExpr() << "]" << endl;
		break;
	}
}

void handle(const mup::ParserError& e, sbx::ValidationResults& valResult, const sbx::RuleConstantContainer& rcc, bool handleAsError) {
	sbx::ValidationResult r {sbx::ValidationCode::kUnknownValidationCode, rcc.getProductElementOid(e.GetToken()), e.GetToken(), "", "", e.GetExpr()};

	stringstream s {};
	bool handled {false};

	switch(e.GetCode())
	{
	case mup::ecUNASSIGNABLE_TOKEN:
		handled = true;
		r.setValidationCode( (handleAsError) ? sbx::ValidationCode::kProductElementRequired : sbx::ValidationCode::kTokenNotDefined );
		s << "Token '" << e.GetToken() << "' not found on in parser: Msg[" << e.GetMsg() << "] ( from sbx::mubridge::handle(...) )";
		break;
	default:
		s << "Unhandled ParserError caught:" << " Token[" << e.GetToken() << "], code[" << e.GetCode() << "], msg[" << e.GetMsg() << "], expr[" << e.GetExpr() << "]";
		break;
	}

	r.setMessage(s.str());

	if (!handled) {
		cout << "**** " << s.str() << endl << endl;
	}

	if (handleAsError && !valResult.hasMessages(r.getProductElementOid(), r.getValidationCode()))
		valResult.addValidationResult(r);
	else if (!valResult.hasWarnings(r.getProductElementOid(), r.getValidationCode()))
		valResult.addWarning(r);
}

} // namespace mubridge

} // namespace sbx
