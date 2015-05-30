//============================================================================
// Name        : sbxTypes.cpp
// Author      : xjes
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#ifndef RULEENGINE_SBXTYPES_H_
#define RULEENGINE_SBXTYPES_H_

#ifdef __cplusplus
namespace sbx {
#endif

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

			kbagud_mk								= 39,

			kIndmeldelsesAlder						= 57,
			kAnciennitet							= 59,

			kGrpElever								= 61,
			kGrpStuderende							= 62,
			kGrpVikarer								= 63,
			kGrpLaerlinge							= 64,
			kGrpSeniorMedarbejdere					= 65,
			kEjIndmeldelseOverAlder					= 66,
			kIkraftDato								= 68,

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
			kKritiskSygdom_MK						= 96,
			kHospitalsdaekning_MK					= 97,

			kAnciennitetTidlArbejdsplads			= 109,

			kDoedDaekningstype						= 139,

			kLoenDefinition							= 146,
			kLoenRegulering							= 147,
			kLoenreguleringsfrekvens				= 148,
			kBidragsstigningsform					= 149,
			kIndslusning_MK							= 150,
			kTAE_Udlobsforskellig_MK				= 151,
			kBidragEjFoesteTrin_MK					= 152,
			kAar									= 153,
			kAttestGrFaktor							= 155,
			kBidragInklHospdaekningPctMin			= 156,
			kBidragsEksklHospdaekningPctMin			= 157,



			kHelbredsopl_frivilligbidrag_indenfor_spaend	= 158,

			kOmtegning_MK							= 160,

			kUdbetalingsperiode						= 174,
			kMaxAndelLinkBidragPCT					= 184,
			kUdbetalingsform						= 192,

			kBoerneRenteSoliMax						= 183,

			kMaxAndelMarkedspensionBidragPct		= 208,
			kLinkKunForEgetBidrag_MK				= 209,
			kTidspensionUdenGaranti_MK 				= 210,
			kFrivilligAftaleMinPct					= 211,
			kFrivilligAftaleMinKr					= 212,
			kMinAndelTraditionelPct					= 213,
			kMinAndelTraditionelPctType				= 214,
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
			kInvalidesumReguleringskode				= 226,
			kInvalidesumSoliMax						= 228,
			kBoernerente_Reguleringstype			= 229,
			kFravalgRisiko_MK						= 230,
			kFravalgRisikoAlder						= 231,
			kFravalgLivrente_mk						= 232,
			kEjIndmeldelseEleverUnderAlder			= 233,
			kGrpEleverRisikoPakke					= 234,
			kGrpSkatPrivateIndehaver				= 235,
			kOmtegndato								= 236,
			kOmtegnperiode							= 237,

			kModetype								= 248,
			kModearrangor							= 249,

			kPrivate_Taxed_MK						= 260,
			kPrivate_Premium_BL						= 261,
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
			kSeneste_Underskriftsdato				= 273,


			// artificial pe
			kAftaleIkraftdato						= 998,
			kBidragstrappe							= 999
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


enum UnionAgreementRelationship {
	OUTSIDE,
	FOLLOWS,
	INCLUDED
};

#ifdef __cplusplus
} // namespace sbx
#endif

#endif // RULEENGINE_SBXTYPES_H_
