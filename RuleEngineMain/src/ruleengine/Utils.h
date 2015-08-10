/*
 * Utils.h
 *
 *  Created on: 21/05/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_UTILS_H_
#define RULEENGINE_UTILS_H_

#include <algorithm>
#include <locale>
#include <string>

#include "muParser/mpValue.h"

#include "ProductElement_sbx.h"
#include "sbxTypes.h"

namespace sbx {
const std::string kBidragstrappe_VARNAME = "Bidragstrappe";
const std::string kAftaleIkraftdato_VARNAME = "AftaleIkraftdato";
const std::string kInsuranceCondition_VARNAME = "InsuranceConditionVersion";

const std::string kUnionAgreementRelationship_VARNAME = "UnionAgreementRelationship";
const std::string kUnionAgreementOid_VARNAME = "UnionAgreementOid";

const std::string kUnionAgreementEmployerPct1stStep = "UnionAgreementEmployerPct1stStep";
const std::string kUnionAgreementTotalPct1stStep = "UnionAgreementTotalPct1stStep";

const std::string kOUTSIDE = "U";
const std::string kFOLLOWS = "F";
const std::string kINCLUDED = "O";

const unsigned short int undefined_oid {0};

// typedefs
typedef unsigned short int oid;
typedef unsigned short int parameter_oid;
typedef unsigned short int koncept_oid;
typedef unsigned short int subkoncept_oid;
typedef unsigned short int unionagreement_oid;
typedef unsigned short int elementtype_oid;
typedef unsigned short int product_oid;
typedef unsigned short int productelement_oid;
typedef unsigned short int number_of_employees;
typedef number_of_employees max_number_of_employees;


const koncept_oid EPENSION 				= 1;
const koncept_oid ITPENSION 			= 2;
const koncept_oid AMP					= 3;
const koncept_oid OSV					= 4;
const koncept_oid SSG					= 5;
const koncept_oid Tommerhandelpension	= 6;
const koncept_oid PROSA					= 7;
const koncept_oid REVISOR				= 8;
const koncept_oid ADVOKATPAKKEN			= 9;

namespace utils {


class dk_numpunct : public std::numpunct<char>
{
  protected:
    virtual char do_thousands_sep() const;
    virtual char do_decimal_point() const;
    virtual std::string do_truename() const;
    virtual std::string do_falsename() const;
    virtual std::string do_grouping() const;

};

std::string toUpper(const std::string& s);
bool toBool(const std::string& s);
sbx::ValidationCode toValCode(unsigned short valCode, sbx::ValidationCode defaultValCode = sbx::ValidationCode::kFail);

std::string comparisonTypeName(const sbx::ComparisonTypes& comparisonType);
std::string constructRCName(const sbx::ProductElement& pe, const sbx::ComparisonTypes& ct);
std::string constructMinMaxExpr(const sbx::ProductElement& pe, const sbx::ComparisonTypes& ct);
std::string formatValue(const mup::Value& val);
std::string formatValue(long);
std::string formatValue(int);
std::string formatValue(double);
std::string formatValue(bool);
std::string formatMessage(std::string str, const std::vector<std::string>& parameters);

const std::string& convertUAR(sbx::UnionAgreementRelationship uar);

}

} /* namespace sbx */

#endif /* RULEENGINE_UTILS_H_ */
