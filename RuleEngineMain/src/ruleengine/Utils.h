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
const std::string kUnionAgreementRelationship_VARNAME = "UnionAgreementRelationship";
const std::string kUnionAgreementOid_VARNAME = "UnionAgreementOid";

const std::string kUnionAgreementEmployerPct1stStep = "UnionAgreementEmployerPct1stStep";
const std::string kUnionAgreementTotalPct1stStep = "UnionAgreementTotalPct1stStep";

const std::string kOUTSIDE = "U";
const std::string kFOLLOWS = "F";
const std::string kINCLUDED = "O";


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

}

} /* namespace sbx */

#endif /* RULEENGINE_UTILS_H_ */
