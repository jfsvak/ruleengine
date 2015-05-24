/*
 * utils.h
 *
 *  Created on: 18/05/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_MUBRIDGE_UTILS_H_
#define RULEENGINE_MUBRIDGE_UTILS_H_

#include <iostream>
#include <sstream>

#include "../muParser/mpError.h"

#include "../ValidationResults.h"
#include "../RuleConstantContainer_sbx.h"

namespace sbx {

namespace mubridge {
//typedef unsigned short (sbx::RuleConstantContainer::*FuncPtr)(const std::string& varName);

void handle(const mup::ParserError& e);
void handleAsWarning(const mup::ParserError& e, sbx::ValidationResults& valResult, const sbx::RuleConstantContainer&);
//void handleAsWarning(const mup::ParserError& e, sbx::ValidationResults& valResult, FuncPtr myFunc);
} // namespace mubridge

} // namespace sbx


#endif /* RULEENGINE_MUBRIDGE_UTILS_H_ */
