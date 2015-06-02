/*
 * SubtractMonths.h
 *
 *  Created on: 02/06/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_MUBRIDGE_SUBTRACTMONTHS_H_
#define RULEENGINE_MUBRIDGE_SUBTRACTMONTHS_H_

#include "../muParser/mpICallback.h"
#include "../muParser/mpTypes.h"

namespace sbx {

class SubtractMonths : public mup::ICallback {
public:
	SubtractMonths();

	virtual void Eval(mup::ptr_val_type &ret, const mup::ptr_val_type *a_pArg, int a_iArgc);
	const mup::char_type* GetDesc() const;
	mup::IToken* Clone() const;

	virtual ~SubtractMonths();
};

} /* namespace sbx */

#endif /* RULEENGINE_MUBRIDGE_SUBTRACTMONTHS_H_ */
