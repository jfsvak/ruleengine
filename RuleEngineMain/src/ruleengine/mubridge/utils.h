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

#include "../Rule.h"
#include "../ValidationResult.h"
#include "../ValidationResults.h"

namespace sbx {

namespace mubridge {

void handle(const mup::ParserError& e);
void handle(const mup::ParserError& e, sbx::Rule* rule, std::vector<unsigned short> peOidsBeingValidated, sbx::ValidationResults& valResult);

} // namespace mubridge

} // namespace sbx


#endif /* RULEENGINE_MUBRIDGE_UTILS_H_ */
