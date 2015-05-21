/*
 * Utils.cpp
 *
 *  Created on: 21/05/2015
 *      Author: jfsvak
 */

#include "Utils.h"

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


} // namespace utils
} /* namespace sbx */
