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

void handle(const mup::ParserError& e, sbx::Rule* rule, std::vector<unsigned short> peOidsBeingValidated, sbx::ValidationResult& valResult) {
	stringstream s {};

	bool handled {true};

	switch(e.GetCode())
	{
	case mup::ecUNASSIGNABLE_TOKEN:
		s << "Value for [" << e.GetToken() << "] not found on ta";
		break;
	default:
		s << "Unhandled ParserError caught:" << " Token[" << e.GetToken() << "], code[" << e.GetCode() << "], msg[" << e.GetMsg() << "], expr[" << e.GetExpr() << "]" << endl;
		handled = false;
		break;
	}

	if (!handled) {
		cerr << s.str() << endl;
	}

	valResult.addValidationResult(peOidsBeingValidated, s.str());
}

} // namespace mubridge

} // namespace sbx
