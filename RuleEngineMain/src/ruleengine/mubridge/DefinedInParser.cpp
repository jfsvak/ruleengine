/*
 * DefinedInParser.cpp
 *
 *  Created on: 24/06/2015
 *      Author: jfsvak
 */

#include "DefinedInParser.h"

#include "../muParser/mpValue.h"

using namespace mup;
using namespace std;

namespace sbx {

DefinedInParser::DefinedInParser(sbx::RuleEngine* ruleEngine)
		: ICallback(cmFUNC, _T("is_var_defined"), 1)
{
	_ruleEngine = ruleEngine;
}

void DefinedInParser::Eval(mup::ptr_val_type &ret, const mup::ptr_val_type *a_pArg, int a_iArgc)
{
	// Get the argument from the argument input vector
	mup::string_type variable = a_pArg[0]->GetString();
	cout << "is_var_defined called with var[" << variable << "]" << endl;

	// The return value is passed by writing it to the reference ret
	*ret = _ruleEngine->isVarDefined(variable);
//	*ret = (date - months);
}

const mup::char_type* DefinedInParser::GetDesc() const
{
	return _T("is_var_defined(variable_name) - A function to check if a variable is defined in the parser");
}

mup::IToken* DefinedInParser::Clone() const
{
	return new DefinedInParser { *this };
}

} /* namespace sbx */
