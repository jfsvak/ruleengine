/*
 * Utils.cpp
 *
 *  Created on: 21/05/2015
 *      Author: jfsvak
 */

#include "Utils.h"

#include <iostream>
#include <iomanip>
#include <cmath>
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

std::string formatValue(const mup::Value& val)
{
	std::stringstream ss {};
	std::locale dk_locale(std::locale(), new sbx::utils::dk_numpunct { });
	ss.imbue(dk_locale);

	switch(val.GetType())
	{
	case 'i': return formatValue(val.GetInteger());
	case 'f': return formatValue(val.GetFloat());
	case 'b': return formatValue(val.GetBool());
	default:
		ss << val;
	}

	return ss.str();
}

std::string formatValue(string v) {
	std::stringstream ss {};
	std::locale dk_locale(std::locale(), new sbx::utils::dk_numpunct { });
	ss.imbue(dk_locale);
	ss << boolalpha << v;
	return ss.str();
}

std::string formatValue(long v) {
	std::stringstream ss {};
	std::locale dk_locale(std::locale(), new sbx::utils::dk_numpunct { });
	ss.imbue(dk_locale);
	ss << std::setprecision(0) << fixed << v;
	return ss.str();
}

std::string formatValue(int v) {
	std::stringstream ss {};
	std::locale dk_locale(std::locale(), new sbx::utils::dk_numpunct { });
	ss.imbue(dk_locale);
	ss << std::setprecision(0) << fixed << v;
	return ss.str();
}

std::string formatValue(bool v) {
	std::stringstream ss {};
	std::locale dk_locale(std::locale(), new sbx::utils::dk_numpunct { });
	ss.imbue(dk_locale);
	ss << boolalpha << v;
	return ss.str();
}

std::string formatValue(double v) {
	std::stringstream ss {};
	std::locale dk_locale(std::locale(), new sbx::utils::dk_numpunct { });
	ss.imbue(dk_locale);
	ss << std::setprecision(2) << fixed << v;
	return ss.str();
}

std::string formatMessage(std::string msg, const std::vector<std::string>& parameters)
{
	try
	{
		int i { 1 };

		for (auto& parameter : parameters) {
			stringstream ss{};
			ss << "%" << i;
			int pos = msg.find(ss.str());

			if (pos > -1)
				msg.replace(pos, 2, parameter);
			else
				cout << "[" << ss.str() << "] not found in msg: " << msg << endl;

			i++;
		}
	}
	catch (const exception& e) {
		cerr << "Exception during string formatting: " << e.what() << endl;
	}

	return msg;
}

std::string int2string(long i) {
	std::stringstream ss {};
	ss << i;
	return ss.str();
}

std::string int2string(int i) {
	return int2string((long) i);
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

const std::string& convertUAR(sbx::UnionAgreementRelationship uar)
{
	switch(uar)
	{
	case FOLLOWS:
		return sbx::kFOLLOWS;
	case INCLUDED:
		return sbx::kINCLUDED;
	case OUTSIDE:
		return sbx::kOUTSIDE;
	}
}

inline bool isEqual(double x, double y)
{
  const double epsilon {0.000001}; /* some small number such as 1e-5 */;
  return std::abs(x - y) <= epsilon * std::abs(x);
  // see Knuth section 4.2.2 pages 217-218
}

char dk_numpunct::do_thousands_sep() const { return '.'; }
std::string dk_numpunct::do_grouping() const { return "\03"; }
char dk_numpunct::do_decimal_point() const { return ','; };
std::string dk_numpunct::do_truename() const { return "Ja"; };
std::string dk_numpunct::do_falsename() const { return "Nej"; };


} // namespace utils
} /* namespace sbx */
