/*
 * Utils.h
 *
 *  Created on: 21/05/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_UTILS_H_
#define RULEENGINE_UTILS_H_

#include <string>

#include "ProductElement_sbx.h"
#include "sbxTypes.h"

namespace sbx {
namespace utils {

std::string comparisonTypeName(const sbx::ComparisonTypes& comparisonType);
std::string constructRCName(const sbx::ProductElement& pe, const sbx::ComparisonTypes& ct);
std::string constructMinMaxExpr(const sbx::ProductElement& pe, const sbx::ComparisonTypes& ct);

}

} /* namespace sbx */

#endif /* RULEENGINE_UTILS_H_ */
