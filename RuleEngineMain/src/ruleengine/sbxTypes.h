//============================================================================
// Name        : sbxTypes.cpp
// Author      : xjes
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#ifndef RULEENGINE_SBXTYPES_H_
#define RULEENGINE_SBXTYPES_H_

#include <algorithm>
#include <string>
#include <iostream>


namespace sbx {

std::string toUpper(const std::string& s);
bool toBool(const std::string& s);

enum ComparisonTypes : unsigned short {
	kUnknown	= 0,
	kMin		= 1,
	kMax		= 2,
	kEnum		= 3,
	kEquals		= 4
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
			kUnknownProductElement 					= 0,
			kDoedBlGrMin							= 1,
			kDoedBlOblMax							= 2,
			kDoedSpaendBl							= 3,
			kDoedSoliMax							= 4,
			kDoedSkattekode							= 5,
			kDoedPctGrMin							= 6,
			kDoedPctOblMax							= 7,
			kDoedSpaendPct							= 9,
			kTaeBlGrMin								= 14,
			kTaeBlOblMax							= 15,
			kTaeSpaendBl							= 16,
			kUdlobsalder_Pension					= 71,
			kTraditionel_MK							= 72,
			kLink_MK								= 73,
			kMarkedspension_MK						= 74,
			kTidspensionMedGaranti_MK				= 75,
			kDoedReguleringskode					= 82,
			kBoerneRentePctMin						= 85,
			kBoerneRenteBlMin						= 88,
			kBoerneSumBlMin							= 91,
			kBoerneSumBlMax							= 92,
			kBoerneSumSpaendBl						= 93,
			kBoerneUdloebsalder						= 94,
			kStandardProduct						= 95,
			kDoedDaekningstype						= 139,
			kLoenDefinition							= 146,
			kLoenRegulering							= 147,

			kHelbredsopl_frivilligbidrag_indenfor_spaend	= 158,

			kBoerneRenteSoliMax						= 183,
			kTidspensionUdenGaranti_MK 				= 210,
			kKritiskSygBlMin						= 215,
			kKritiskSygBlMax						= 216,
			kKritiskSygSpaendBl						= 217,
			kKritiskSygPctMin						= 218,
			kKritiskSygPctMax						= 219,
			kKritiskSygspaendpc						= 220,
			kKritiskSygReguleringskode				= 221,
			kKritiskSygSkattekode					= 222,
			kKritiskSygTegningsmaxBl				= 223,
			kKritiskSygSuppldaekn_mk				= 224,
			kKritiskSygdom_i_FG_mk					= 225,


			kBoernerente_Reguleringstype			= 229,
			kFravalgRisiko_MK						= 230,
			kFravalgRisikoAlder						= 231,
			kKritiskSygBornesumBlMax				= 262,
			kKritiskSygBornesumBlMin				= 263,
			kKritiskSygBornesum_i_FG_mk				= 264,
			kKritiskSygBornesumPctMax				= 265,
			kKritiskSygBornesumPctMin				= 266,
			kKritiskSygBornesumReguleringskode		= 267,
			kKritiskSygBornesumSkattekode			= 268,
			kKritiskSygBornesumSpaendBl				= 269,
			kKritiskSygBornesumspaendpc				= 270,
			kKritiskSygBornesumSuppldaekn_mk		= 271,
			kKritiskSygBornesumTegningsmaxBl		= 272,
			kSeneste_Underskriftsdato				= 273
};


enum ValidationCode : unsigned short {
	kUnknownValidationCode			= 0,		// If everything goes wrong, this code is used
	// General validation codes
	kOK 							= 1,		// General OK code
	kWarning 						= 2,		// General warning code
	kFail 							= 3,		// General fail code
	kProductElementNotDefined		= 4,		// If trying to validate a pe that hasn't been defined/set on the TA

	// Validation Codes for values (inside pe's)
	kValueNotAllowed 				= 101,		// value is not allowed i.e. not found in options list
	kValueUnderLimit 				= 102,		// Value is under the allowed limit
	kValueOverLimit 				= 103,		// Value is over the allowed limit

	// Validation Codes for product elements, not the actual value is being validated
	kProductElementNotAllowed		= 201,		// Product element shouldn't be on the TA (and therefore not have a value either)
	kProductElementRequired			= 202, 		// Product element is required to be on the TA (and have a value)

	// muParser Validation Codes
	kTokenNotDefined				= 301		// If an expression (maybe custom) contains a token that hasn't been loaded into the parser
};

sbx::ValidationCode toValCode(unsigned short valCode, sbx::ValidationCode defaultValCode = sbx::ValidationCode::kFail);
} // namespace sbx

#endif // RULEENGINE_SBXTYPES_H_
