//============================================================================
// Name        : sbxTypes.cpp
// Author      : xjes
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "sbxTypes.h"

#include <algorithm>
#include <string>
#include <iostream>

namespace sbx {

std::string toUpper(const std::string& s)
{
	std::string upperS = s;
	std::transform(upperS.begin(), upperS.end(), upperS.begin(), toupper);
	return upperS;
}

bool toBool(const std::string& s)
{

	return (sbx::toUpper(s) == "TRUE" || s == "1");
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

} //namespace sbx
