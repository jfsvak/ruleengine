//============================================================================
// Name        : sbxTypes.cpp
// Author      : xjes
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#ifndef RULEENGINE_SBXTYPES_H_
#define RULEENGINE_SBXTYPES_H_

#include <string>

namespace sbx {

enum ComparisonTypes : int {
	kMin	= 1,
	kMax	= 2,
	kEquals	= 4 // equals == 4 to match SBX value
};

enum ProductElementTypes : int {
	kLong		= 1,
	kText 		= 2,
	kMonth 		= 3,
	kBool 		= 4,
	kCurr		= 5,
	kDate 		= 6,
	kYear		= 7,
	kPercent	= 8
};

enum ProductElementNames : int {
	kTaeBlGrMin			= 1,
	kTaeBlOblMax		,
	kTaeSpaendBl		,
	kLoenDefinition		,
	kLoenRegulering
};

struct ProductElement {
	sbx::ProductElementNames product_element_name;
	std::string name;
	ProductElementTypes product_element_type;
};

struct ConstantKey {
	sbx::ProductElementNames pe;
	sbx::ComparisonTypes type;

	bool operator<(ConstantKey const& other) const {
		if (pe < other.pe) return true;
		if (pe > other.pe) return false;
		// pe is equal, so look at next part of the composite key
		if (type < other.type) return true;
		if (type > other.type) return false;

		// both pe and type are equal...
		return false;
	}
};

} // namespace sbx

#endif // RULEENGINE_SBXTYPES_H_
