/*
 * Utils.h
 *
 *  Created on: 21/05/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_UTILS_H_
#define RULEENGINE_UTILS_H_

#include <algorithm>
#include <string>

#include "ProductElement_sbx.h"
#include "sbxTypes.h"

namespace sbx {
const std::string kBidragstrappe_VARNAME = "Bidragstrappe";

namespace utils {

std::string toUpper(const std::string& s);
bool toBool(const std::string& s);
sbx::ValidationCode toValCode(unsigned short valCode, sbx::ValidationCode defaultValCode = sbx::ValidationCode::kFail);

std::string comparisonTypeName(const sbx::ComparisonTypes& comparisonType);
std::string constructRCName(const sbx::ProductElement& pe, const sbx::ComparisonTypes& ct);
std::string constructMinMaxExpr(const sbx::ProductElement& pe, const sbx::ComparisonTypes& ct);

}

} /* namespace sbx */

#endif /* RULEENGINE_UTILS_H_ */
