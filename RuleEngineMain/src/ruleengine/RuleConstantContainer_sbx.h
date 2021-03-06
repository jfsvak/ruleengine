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

#include "../json/json.h"

#include "Constant_sbx.h"
#include "ContributionStep_sbx.h"
#include "Date_sbx.h"
#include "Koncept_sbx.h"
#include "KonceptInfo_sbx.h"
#include "Parameter_sbx.h"
#include "Product_sbx.h"
#include "ProductElement_sbx.h"
#include "Utils.h"
#include "sbxTypes.h"
#include "Subkoncept_sbx.h"
#include "UnionAgreement_sbx.h"

namespace sbx {

class RuleConstantContainer {

public:
	static bool _printDebug;
	static bool _printErr;

	void initGlobalConstants(const std::vector<sbx::Constant>& globalConstants);
	void initConstants(const std::string& jsonContents);
	void initContext(const sbx::Subkoncept& subkoncept, const sbx::KonceptInfo&, sbx::UnionAgreementRelationship, sbx::unionagreement_oid = undefined_oid);
	void clearContext();
	void initKoncepts(const std::string& json);
	void initUnionAgreements(const std::string& jsonContents);

	bool existsAs(sbx::productelement_oid peOid, const sbx::ComparisonTypes&) const;

	std::vector<std::string> getOptions(sbx::productelement_oid productElementOid);
	std::vector<std::shared_ptr<sbx::Constant>> getOptionsList(sbx::productelement_oid productElementOid) const;
	std::shared_ptr<sbx::Constant> getConstant(sbx::productelement_oid productElement, const sbx::ComparisonTypes& comparisonType);
	const std::map<sbx::koncept_oid, sbx::Koncept>& getKoncepts() const;

	std::set<sbx::productelement_oid, std::less<sbx::productelement_oid>> getAllowedPEOids() const;
	std::set<sbx::product_oid> getProductOids(sbx::parameter_oid parameterOid) const;
	std::set<sbx::productelement_oid> getProductElementOids(sbx::parameter_oid parameterOid) const;
	sbx::ProductElement getProductElement(sbx::productelement_oid productElementOid);
	sbx::productelement_oid getProductElementOid(const std::string& varName) const;
	sbx::UnionAgreementContributionStep getUAContributionStep(sbx::unionagreement_oid uaOid, const sbx::Date& inceptionDate) const;
	const sbx::UnionAgreement& getUnionAgreement(sbx::unionagreement_oid) const;

	sbx::UnionAgreementRelationship getUnionAgreementRelationship() const;
	sbx::unionagreement_oid getUnionAgreementOid() const;

	bool isUnionAgreementLoaded(sbx::unionagreement_oid) const;
	bool isContainerInitialised() const;
	std::shared_ptr<sbx::Constant> createConstant(sbx::subkoncept_oid underkonceptOid, sbx::unionagreement_oid unionAgreementOid, sbx::productelement_oid peOid, sbx::ComparisonTypes comparisonType);

	// ----- util functions------
	void printKoncepts() const;
	void printUnionAgreements() const;
	void printConstantHeader() const;
	void printSubkonceptConstants(sbx::subkoncept_oid = sbx::undefined_oid, sbx::productelement_oid = sbx::undefined_oid) const;
	void printUnionAgreementConstants(sbx::unionagreement_oid = sbx::undefined_oid, sbx::productelement_oid = sbx::undefined_oid) const;
	void printConstantsFromContext(sbx::productelement_oid = sbx::undefined_oid) const;
	void printContainerOverview(sbx::subkoncept_oid subKonceptOid) const;
	void printContainerOverview(sbx::subkoncept_oid subKonceptOid, sbx::ComparisonTypes type) const;
	void printProducts() const;
	void printParameters() const;
	void printParametersToProducts(sbx::subkoncept_oid underkonceptOid = sbx::undefined_oid) const;
	std::size_t size() const;

private:
    
	void printConstant(const std::shared_ptr<sbx::Constant>& c) const;
	void _printStringOptions(const std::string& mapHeader, const std::map<sbx::oid, std::map<sbx::productelement_oid, std::vector<std::shared_ptr<sbx::Constant>>>>& map, sbx::oid mapOid = sbx::undefined_oid, sbx::productelement_oid peOid = sbx::undefined_oid) const;
	void _printConstants(const std::string& mapHeader, const std::map<sbx::oid, std::map<sbx::productelement_oid, std::shared_ptr<sbx::Constant>>>& map, sbx::oid mapOid = sbx::undefined_oid, sbx::productelement_oid peOid = sbx::undefined_oid) const;
	void _printParameterList(const std::map<sbx::parameter_oid, std::set<sbx::product_oid>>& values) const;
    void _initInternalMaps();
	void _initRuleConstants(const Json::Value& ruleConstantsList);
	void _initProductElements(const Json::Value& products);
	void _initParameters(const Json::Value& parameters);
	void _initParametersToProducts(const Json::Value& parameters);
	void _initSubkoncepts(const Json::Value& subkonceptsJSON, sbx::Koncept& koncept);
	void _addFakeProductElements(std::shared_ptr<sbx::Product> productPtr);
	void _checkContextInitialisation() const;

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
	std::map<sbx::subkoncept_oid, std::map<sbx::productelement_oid, std::vector<std::shared_ptr<sbx::Constant>>>> _ukOptionsMap;

	/**
	 * _ukMinValuesMap:
	 * First index is underKonceptOid
	 * Second index is productElementOid, value is a pointer to the constant-instance in _globalConstants
	 * Subscript format is: _ukMinValuesMap[underkonceptOid][productElementOid]
	 * Value: shared_ptr to Constant
	 */
	std::map<sbx::subkoncept_oid, std::map<sbx::productelement_oid, std::shared_ptr<sbx::Constant>>> _ukMinValuesMap;

	/**
	 * _ukMaxValuesMap:
	 * First index is underKonceptOid
	 * Second index is product element oid,
	 * value is a pointer to the constant-instance in _globalConstants
	 * Subscript format is: _ukMaxValuesMap[underkonceptOid][productElementOid]
	 * Value: shared_ptr to Constant
	 */
	std::map<sbx::subkoncept_oid, std::map<sbx::productelement_oid, std::shared_ptr<sbx::Constant>>> _ukMaxValuesMap;

	/**
	 *  _uaOptionsMap
	 *  Map of map to a vector of pointers to Constant in _globalConstants
	 *  First index is unionAgreementOid
	 *  Second index is ProductElementOid
	 *  Subscript format is: _uaOptionsMap[unionAgreementOid][productElementOid]
	 *  Value is a vector of shared_ptr->Constant
	 **/
	std::map<sbx::unionagreement_oid, std::map<sbx::productelement_oid, std::vector<std::shared_ptr<sbx::Constant>>>> _uaOptionsMap;

	/**
	 * _uaMinValuesMap:
	 * First index is unionAgreementOid
	 * Second index is productElementOid, value is a pointer to the constant-instance in _globalConstants
	 * Subscript format is: _uaMinValuesMap[unionAgreementOid][productElementOid]
	 * Value: shared_ptr to Constant
	 */
	std::map<sbx::unionagreement_oid, std::map<sbx::productelement_oid, std::shared_ptr<sbx::Constant>>> _uaMinValuesMap;

	/**
	 * _uaMaxValuesMap:
	 * First index is unionAgreementOid
	 * Second index is product element oid,
	 * value is a pointer to the constant-instance in _globalConstants
	 * Subscript format is: _uaMaxValuesMap[unionAgreementOid][productElementOid]
	 * Value: shared_ptr to Constant
	 */
	std::map<sbx::unionagreement_oid, std::map<sbx::productelement_oid, std::shared_ptr<sbx::Constant>>> _uaMaxValuesMap;

	/**
	 * Map of koncept oids to koncepts
	 * Each koncept has a list of subkoncepts
	 */
	std::map<sbx::koncept_oid, sbx::Koncept> _koncepts;

	std::map<sbx::subkoncept_oid, sbx::Subkoncept> _subkoncepts;

	/**
	 * _productsMap:
	 * Index is productOid
	 * Values is a shared_ptr to a Product
	 */
	std::map<sbx::product_oid, std::shared_ptr<sbx::Product>> _productsMap;

	/**
	 * _productElementMap:
	 * Index is productOid
	 * Values is a shared_ptr to a ProductElement
	 */
	std::map<sbx::product_oid, std::shared_ptr<sbx::ProductElement>> _productElementMap;
	/**
	 * _parameterMap:
	 * Index is parameterOid
	 * Values is a Parameter
	 */
	std::map<sbx::parameter_oid, sbx::Parameter> _parameterMap;

	/**
	 * First index is subKonceptOid
	 * Second index is parameter
	 * Value is a set of allowed products
	 */
	std::map<sbx::subkoncept_oid, std::map<sbx::parameter_oid, std::set<sbx::product_oid>>> _parameterOidToProductOids;

	/**
	 * First index is subKonceptOid
	 * Second index is parameter
	 * Value is a set of allowed product element oids
	 */
	std::map<sbx::subkoncept_oid, std::map<sbx::parameter_oid, std::set<sbx::productelement_oid>>> _parameterOidToProductElementOids;

	/**
	 * _varNameToPEOidMap:
	 * Index is varName as string
	 * Value is productelement oid
	 */
	std::map<std::string, sbx::productelement_oid> _varNameToPEOidMap;

	/**
	 * Index: union agreement oid
	 * Value: UnionAgreement
	 */
	std::map<sbx::unionagreement_oid, sbx::UnionAgreement> _unionAgreements;

	/** CONTEXT */
	sbx::KonceptInfo _ki;
	sbx::Subkoncept _subkoncept;
	sbx::UnionAgreementRelationship _unionAgreementRelationship;
	sbx::unionagreement_oid _unionAgreementOid;
	bool _contextInitialised;
};

} // sbx namespace end

#endif /* RULEENGINE_RULECONSTANTCONTAINER_H_ */
