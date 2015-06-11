/*
 * RuleConstantContainer.h
 *
 *  Created on: 20/03/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_RULECONSTANTCONTAINER_H_
#define RULEENGINE_RULECONSTANTCONTAINER_H_

#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>

#include "sbxTypes.h"
#include "Constant_sbx.h"
#include "ContributionStep.h"
#include "Product_sbx.h"
#include "Parameter_sbx.h"
#include "ProductElement_sbx.h"
#include "../json/json.h"

namespace sbx {

class RuleConstantContainer {

public:
	static bool _printDebug;
	static bool _printErr;

	void initGlobalConstants(const std::vector<sbx::Constant>& globalConstants);
	void initConstants(const std::string& jsonContents);
	void initContext(const unsigned short underkoncept_oid, const short unionAgreementOid);
	void initUAContributionSteps(const std::map<unsigned short, std::vector<sbx::ContributionStep>>&);

	bool existsAs(unsigned short peOid, const sbx::ComparisonTypes&) const;

	std::vector<std::string> getOptions(unsigned short productElementOid);
	std::vector<std::shared_ptr<sbx::Constant>> getOptionsList(unsigned short productElementOid);
	std::shared_ptr<sbx::Constant> getConstant(unsigned short productElement, const sbx::ComparisonTypes& comparisonType);

	const std::set<unsigned short>& getProductOids(unsigned short parameterOid) const;
	std::set<unsigned short> getProductElementOids(unsigned short parameterOid) const;
	sbx::ProductElement getProductElement(unsigned short productElementOid);
	unsigned short getProductElementOid(const std::string& varName) const;
	std::shared_ptr<sbx::ContributionStep> getUAContributionStep(unsigned short uaOid);

	std::shared_ptr<sbx::Constant> createConstant(unsigned short underkonceptOid, unsigned short unionAgreementOid, unsigned short peOid, sbx::ComparisonTypes comparisonType);

	// ----- util functions------
	void printConstantHeader() const;
	void printConstants() const;
	void printConstants(unsigned short int underKonceptOid, unsigned short productElement) const;
	void printContainerOverview(unsigned short int underKonceptOid) const;
	void printContainerOverview(unsigned short int underKonceptOid, sbx::ComparisonTypes type) const;
	void printProducts() const;
	void printParameters() const;
	void printParametersToProducts(unsigned short underkonceptOid) const;
	std::size_t size() const;
private:
    
	void printConstant(const std::shared_ptr<sbx::Constant>& c) const;
    void _initInternalMaps();
	void _initRuleConstants(const Json::Value& ruleConstantsList);
	void _initProductElements(const Json::Value& products);
	void _initParameters(const Json::Value& parameters);
	void _initParametersToProducts(const Json::Value& parameters);
	void _addFakeProductElements(std::shared_ptr<sbx::Product> productPtr);

	/**
	 * vector of shared pointers holding all global constants for all contexts
	 */
	std::vector<std::shared_ptr<Constant>> _globalConstants;

	/**
	 *  _ukOptionsMap
	 *  Map of map to a vector of pointers to Constant in _globalConstants
	 *  First index is underKonceptOid
	 *  Second index is ProductElementOid
	 *  Subscript format is: _ukOptionsMap[underkonceptOid][productElementOid]
	 *  Value is a vector of shared_ptr->Constant
	 **/
	std::map<unsigned short, std::map<unsigned short, std::vector<std::shared_ptr<sbx::Constant>>>> _ukOptionsMap;

	/**
	 * _ukMinValuesMap:
	 * First index is underKonceptOid
	 * Second index is productElementOid, value is a pointer to the constant-instance in _globalConstants
	 * Subscript format is: _ukMinValuesMap[underkonceptOid][productElementOid]
	 * Value: shared_ptr to Constant
	 */
	std::map<unsigned short, std::map<unsigned short, std::shared_ptr<sbx::Constant>>> _ukMinValuesMap;

	/**
	 * _ukMaxValuesMap:
	 * First index is underKonceptOid
	 * Second index is product element oid,
	 * value is a pointer to the constant-instance in _globalConstants
	 * Subscript format is: _ukMinValuesMap[underkonceptOid][productElementOid]
	 * Value: shared_ptr to Constant
	 */
	std::map<unsigned short, std::map<unsigned short, std::shared_ptr<sbx::Constant>>> _ukMaxValuesMap;

	/**
	 * _productsMap:
	 * Index is productOid (unsigned short)
	 * Values is a shared_ptr to a Product
	 */
	std::map<unsigned short, std::shared_ptr<sbx::Product>> _productsMap;

	/**
	 * _productElementMap:
	 * Index is productOid (unsigned short)
	 * Values is a shared_ptr to a ProductElement
	 */
	std::map<unsigned short, std::shared_ptr<sbx::ProductElement>> _productElementMap;
	/**
	 * _parameterMap:
	 * Index is parameterOid (unsigned short)
	 * Values is a Parameter
	 */
	std::map<unsigned short, sbx::Parameter> _parameterMap;
	/**
	 * _varNameToPEOidMap:
	 * Index is parameterOid (unsigned short)
	 * Values is a Parameter
	 */
	std::map<std::string, unsigned short> _varNameToPEOidMap;

	/**
	 * First index is underKonceptOid
	 * Second index is parameter
	 * Value is a set of allowed products
	 */
	std::map<unsigned short, std::map<unsigned short, std::set<unsigned short>>> _parameterOidToProductOids;

	/**
	 * First index is underKonceptOid
	 * Second index is parameter
	 * Value is a set of allowed product element oids
	 */
	std::map<unsigned short, std::map<unsigned short, std::set<unsigned short>>> _parameterOidToProductElementOids;

	/**
	 * Index: union agreement oid
	 * Value: one contribution step
	 */
	std::map<unsigned short, std::vector<sbx::ContributionStep>> _uaContributionLadders;

	short _underKonceptOid;
	short _unionAgreementOid;
	bool _contextInitialised;
};

} // sbx namespace end

#endif /* RULEENGINE_RULECONSTANTCONTAINER_H_ */
