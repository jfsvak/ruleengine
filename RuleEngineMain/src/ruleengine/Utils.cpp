/*
 * Utils.cpp
 *
 *  Created on: 21/05/2015
 *      Author: jfsvak
 */

#include "Utils.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

namespace sbx {
namespace utils {

std::string comparisonTypeName(const sbx::ComparisonTypes& comparisonType)
{
	switch (comparisonType)
	{
	case kMin:
		return "min";
	case kMax:
		return "max";
	case kEquals:
		return "equal";
	case kEnum:
		return "enum";
	default:
		return "unknown";
	}
}

std::string constructRCName(const sbx::ProductElement& pe, const sbx::ComparisonTypes& ct)
{
	stringstream s {};
	s << "rc_" << pe.getVariableName() << "_" << sbx::utils::comparisonTypeName(ct);
	return s.str();
}

std::string constructMinMaxExpr(const sbx::ProductElement& pe, const sbx::ComparisonTypes& ct)
{
	stringstream exp {};

	std::string positiveOperation {};

	switch (ct) {
	case kMin:
		positiveOperation = ">=";
		break;
	case kMax:
		positiveOperation = "<=";
		break;
	default:
		positiveOperation = "=";
		break;
	}

	exp << pe.getVariableName() << positiveOperation << sbx::utils::constructRCName(pe, ct);

	return exp.str();
}

std::string toUpper(const std::string& s)
{
	std::string upperS = s;
	std::transform(upperS.begin(), upperS.end(), upperS.begin(), ::toupper);
	return upperS;
}

bool toBool(const std::string& s)
{

	return (sbx::utils::toUpper(s) == "TRUE" || s == "1");
}

sbx::ValidationCode toValCode(unsigned short valCode, sbx::ValidationCode defaultValCode)
{
	if (       (valCode >= (unsigned short) sbx::ValidationCode::kOK && valCode <= sbx::ValidationCode::kProductElementNotDefined)
			|| (valCode >= (unsigned short) sbx::ValidationCode::kValueNotAllowed && valCode <= sbx::ValidationCode::kValueOverLimit)
			|| (valCode >= (unsigned short) sbx::ValidationCode::kProductElementNotAllowed && valCode <= sbx::ValidationCode::kProductElementRequired)
			|| (valCode >= (unsigned short) sbx::ValidationCode::kTokenNotDefined && valCode <= sbx::ValidationCode::kTokenNotDefined) )
		return static_cast<sbx::ValidationCode>(valCode);

	return defaultValCode;
}

} // namespace utils
} /* namespace sbx */
