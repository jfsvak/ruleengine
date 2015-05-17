/*
 * GetTAValue.cpp
 *
 *  Created on: 17/05/2015
 *      Author: jespe_000
 */

#include "GetTAValue.h"

namespace sbx {

GetTAValue::GetTAValue() : ICallback(mup::cmFUNC, "ta_value", 1) {}

void GetTAValue::Eval(mup::ptr_val_type& ret, const mup::ptr_val_type* a_pArg, int a_iArgc)
{

}

const mup::char_type* GetTAValue::GetDesc() const
{
	return "ta_value(x) - A custom function to get a ta value";
}

mup::IToken* GetTAValue::Clone() const
{
	return new GetTAValue(*this);
}

GetTAValue::~GetTAValue()
{
	// TODO Auto-generated destructor stub
}

} /* namespace sbx */
