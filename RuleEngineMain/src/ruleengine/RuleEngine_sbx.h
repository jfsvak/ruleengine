//============================================================================
// Name        : helloworld2.cpp
// Author      : xjes
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#ifndef RULEENGINE_RULEENGINE_H_
#define RULEENGINE_RULEENGINE_H_

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "muParser/mpParser.h"
#include "sbxTypes.h"
#include "RuleConstantContainer_sbx.h"
#include "Constant_sbx.h"
#include "ProductElementValue_sbx.h"
#include "Product_sbx.h"
#include "KonceptInfo_sbx.h"
#include "TA_sbx.h"
#include "ValidationResult.h"

namespace sbx {

class RuleEngine {
  public:
	static bool _printDebug;
	static const short int VALID = 1;
	static const short int INVALID = -1;

	void initConstants(const std::vector<Constant>& globalConstants);
	void initConstants(const std::string& jsonContents);
	void initContext(short underkoncept_oid, short unionagreement_oid);
	void initContext(const sbx::KonceptInfo& ki);

	std::vector<std::string> getOptions(sbx::ProductElementOid productElement);
	const std::vector<std::shared_ptr<Constant>>& getOptionsList(sbx::ProductElementOid productElement);
	const std::shared_ptr<sbx::Constant>& getDefaultValue(sbx::ProductElementOid productElementOid);
	std::shared_ptr<sbx::Constant> getConstant(sbx::ProductElementOid productElement, sbx::ComparisonTypes comparisonType);

	int validate(sbx::ProductElementOid productElement, const std::vector<std::pair<std::string,long>>& p_operands);
	sbx::ValidationResult validate(const sbx::ProductElementValue& peValue);
	int validate(sbx::ComparisonTypes comparisonType) const;
	int validate(const sbx::KonceptInfo&, const sbx::TA& ta);

	const sbx::RuleConstantContainer& getContainer() const;
  private:
	sbx::KonceptInfo _ki;
	sbx::RuleConstantContainer _container;
	std::map<sbx::ProductElementOid, std::vector<std::string>> _ruleCatalogue;
    // mup::ParserX p;

	// private util methods
	void printVariables(mup::ParserX p);
	void printExpressions(mup::ParserX p);
	void loadRuleConstants(mup::ParserX&);
	void loadRuleCatalogue();
};

} // sbx namespace end

#endif // RULEENGINE_RULEENGINE_H_
