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
#include <iostream>

namespace sbx {

enum ComparisonTypes : unsigned short {
	kUnknown	= 0,
	kMin		= 1,
	kMax		= 2,
	kEnum		= 3,
	kEquals		= 4 // equals == 4 to match SBX value
};

enum ProductElementTypes : unsigned short {
	kUnknownPEType	= 0,
	kLong			= 1,
	kText 			= 2,
	kMonth 			= 3,
	kBool 			= 4,
	kCurr			= 5,
	kDate 			= 6,
	kYear			= 7,
	kPercent		= 8
};

enum ProductOid : unsigned short {
	kUnknownProduct		 	= 0,
	kP_194					= 194,
	kP_217					= 217,
	kP_218					= 218
};

enum ProductElementOid : unsigned short {
			kUnknownProductElement 	= 0,
			kDoedBlGrMin			= 1,
			kDoedBlOblMax			= 2,
			kDoedSpaendBl			= 3,
			kDoedSoliMax			= 4,
			kDoedPctGrMin			= 6,
			kDoedPctOblMax			= 7,
			kTaeBlGrMin				= 14,
			kTaeBlOblMax			= 15,
			kTaeSpaendBl			= 16,
			kBoerneUdloebsalder		= 67,
			kDoedReguleringskode	= 82,
			kBoerneRentePctMin		= 85,
			kBoerneRenteBlMin		= 89,
			kBoerneSumBlMin			= 91,
			kBoernerenteReguleringstype	= 128,
			kDoedDaekningstype		= 139,
			kLoenDefinition			= 146,
			kLoenRegulering			= 147,
			kBoerneRenteSoliMax		= 183,
			k194					= 194,
			k217					= 217,
			k218					= 218
};

struct ConstantKey {
	sbx::ProductElementOid pe;
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
