/*
 * GetTAValue.h
 *
 *  Created on: 17/05/2015
 *      Author: jespe_000
 */

#ifndef RULEENGINE_MUBRIDGE_GETTAVALUE_H_
#define RULEENGINE_MUBRIDGE_GETTAVALUE_H_

#include "../muParser/mpICallback.h"

namespace sbx {

class GetTAValue: public mup::ICallback
{
public:
	GetTAValue();
	virtual void Eval(mup::ptr_val_type &ret, const mup::ptr_val_type *a_pArg, int a_iArgc);
	const mup::char_type* GetDesc() const;
	mup::IToken* Clone() const;
	virtual ~GetTAValue();
};

} /* namespace sbx */

#endif /* RULEENGINE_MUBRIDGE_GETTAVALUE_H_ */
