/*
 * SubtractMonths.cpp
 *
 *  Created on: 02/06/2015
 *      Author: jfsvak
 */

#include "SubtractMonths.h"

#include <iostream>

#include "../muParser/mpValue.h"

using namespace mup;
using namespace std;

namespace sbx {

SubtractMonths::SubtractMonths()
		: ICallback(cmFUNC, _T("subtract_months"), 2)
{}

void SubtractMonths::Eval(mup::ptr_val_type &ret, const mup::ptr_val_type *a_pArg, int a_iArgc)
{
	// Get the argument from the argument input vector
	mup::int_type date = a_pArg[0]->GetInteger();
	mup::int_type months = a_pArg[1]->GetInteger();

	cout << "Date : " << date << ", " << months << endl;

//	// The return value is passed by writing it to the reference ret
	*ret = (date - months);
}

const mup::char_type* SubtractMonths::GetDesc() const
{
	return _T("subtract_months(date_as_int, no_of_months) - A custom months subtract function");
}


mup::IToken* SubtractMonths::Clone() const
{
	return new SubtractMonths ( *this );
}

} /* namespace sbx */
