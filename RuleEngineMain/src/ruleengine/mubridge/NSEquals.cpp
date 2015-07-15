/*
 * DefinedInParser.cpp
 *
 *  Created on: 24/06/2015
 *      Author: jfsvak
 */

#include "NSEquals.h"

#include "../muParser/mpValue.h"

using namespace mup;
using namespace std;

namespace sbx {

NSEquals::NSEquals(sbx::RuleEngine* ruleEngine)
		: ICallback(cmFUNC, _T("ns_equals"), 3)
{
	_ruleEngine = ruleEngine;
}

void NSEquals::Eval(mup::ptr_val_type &ret, const mup::ptr_val_type *a_pArg, int a_iArgc)
{
	// Get the argument from the argument input vector
	mup::string_type variableName = a_pArg[0]->GetString();
	mup::ptr_val_type compareTOValue = a_pArg[1];
	mup::bool_type nullResponse = a_pArg[2]->GetBool();

	// The return value is passed by writing it to the reference ret
	if (_ruleEngine->isVarDefined(variableName))
	{
		mup::ParserX parser = _ruleEngine->getParser();

		try {
			mup::var_maptype vmap = parser.GetVar();

			for (mup::var_maptype::iterator item = vmap.begin(); item != vmap.end(); ++item) {

				if (item->first == variableName) {
					mup::Variable& mupVar = (mup::Variable&) (*(item->second));

					if (mupVar.GetType() == 'i' )
						*ret = (compareTOValue->GetInteger() == mupVar.GetInteger());
					else if (mupVar.GetType() == 's')
						*ret = (compareTOValue->GetString() == mupVar.GetString());
					else if (mupVar.GetType() == 'b')
						*ret = (compareTOValue->GetBool() == mupVar.GetBool());
					else if (mupVar.GetType() == 'f')
						*ret = (compareTOValue->GetFloat() == mupVar.GetFloat());
					return;
				}
			}
		}
		catch (const mup::ParserError &e) {
			cout << "Exception in ns_equals: " << e.GetMsg() << endl;
		}
	}
	else
		*ret = nullResponse;
}

const mup::char_type* NSEquals::GetDesc() const
{
	return _T("ns_equals(variable_name, value, null_response) - A function to check if a variable is equal to a certain value. Is null safe by putting in expected response in null_response");
}

mup::IToken* NSEquals::Clone() const
{
	return new NSEquals { *this };
}

} /* namespace sbx */
