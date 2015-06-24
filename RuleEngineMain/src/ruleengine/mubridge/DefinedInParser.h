/*
 * DefinedInParser.h
 *
 *  Created on: 24/06/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_MUBRIDGE_DEFINEDINPARSER_H_
#define RULEENGINE_MUBRIDGE_DEFINEDINPARSER_H_

#include "../muParser/mpICallback.h"

#include "../RuleEngine_sbx.h"

namespace sbx {

class DefinedInParser : public mup::ICallback {
public:

	DefinedInParser(sbx::RuleEngine* ruleEngine);
	virtual void Eval(mup::ptr_val_type &ret, const mup::ptr_val_type *a_pArg, int a_iArgc) override;
	virtual const mup::char_type* GetDesc() const override;
	virtual mup::IToken* Clone() const override;
private:
	sbx::RuleEngine* _ruleEngine;
};

} /* namespace sbx */

#endif /* RULEENGINE_MUBRIDGE_DEFINEDINPARSER_H_ */
