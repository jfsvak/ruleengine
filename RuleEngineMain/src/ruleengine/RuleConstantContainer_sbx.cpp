/*
 * RuleConstantContainer.cpp
 *
 *  Created on: 20/03/2015
 *      Author: jfsvak
 */

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <limits>
#include <iterator>
#include <memory>
#include <vector>
#include <string>
#include <sstream>

#include "../json/json.h"

#include "Date_sbx.h"
#include "Constant_sbx.h"
#include "RuleConstantContainer_sbx.h"
#include "RuleEngine_sbx.h"
#include "sbxTypes.h"
#include "Product_sbx.h"
#include "UnionAgreementContributionStep_sbx.h"
#include "Utils.h"

using namespace std;

namespace sbx {
bool RuleConstantContainer::_printDebug {false};
bool RuleConstantContainer::_printErr {false};

/**
 * Copies the vector of Constants into the _globalConstants vector
 */
void RuleConstantContainer::initGlobalConstants(const vector<Constant>& global_constants)
{
	// load the global constants into the container using copy-constructor
	if (RuleConstantContainer::_printDebug) { cout << "  _globalConstants = global_constants" << endl;}

	// set size in internal vector of global constants the the same size of the incoming constants
	_globalConstants.resize(global_constants.size());

	// run through the incoming list of constants and wrap each in a shared_ptr
	for (size_t gcIndex = 0; gcIndex < global_constants.size(); gcIndex++)
	{
		_globalConstants[gcIndex] = make_shared<Constant>(global_constants[gcIndex]);
	}

	if (RuleConstantContainer::_printDebug) { cout << "  _globalConstants = global_constants done!" << endl; }

	_initInternalMaps();
}

/**
 * Initialises the container from json string containing
 */
void RuleConstantContainer::initConstants(const std::string& jsonContents)
{
	// Load/parse the json string to get all the rule constants and create the vector of Constant objects for the container
	Json::Reader reader{};
	Json::Value root{};

	bool parsingSuccessful = reader.parse(jsonContents, root);

	if (parsingSuccessful)
	{
		// if we can parse the json string, then get the list of rule constants
		_initRuleConstants(root["data"].get("FIRMA_RULE_ENGINE", 0).get("ruleConstants", 0));
		_initProductElements(root["data"].get("FIRMA_RULE_ENGINE", 0).get("products", 0));
		_initParameters(root["data"].get("FIRMA_RULE_ENGINE", 0).get("parameters", 0));
		_initParametersToProducts(root["data"].get("FIRMA_RULE_ENGINE", 0).get("parameterProducts", 0));
	}
	else
	{
		cerr << "Could not parse json string: " << reader.getFormattedErrorMessages() << endl;
		throw invalid_argument(reader.getFormattedErrorMessages());
	}

	//	  loadRuleCatalogue();
}

void RuleConstantContainer::_initRuleConstants(const Json::Value& ruleConstantList)
{
	// if we can parse the json string, then get the list of rule constants
	if (ruleConstantList.size() > 0)
	{
		size_t index = 0;
		// new vector of constants to passed to RuleConstantContainer. Set size to the number of incoming json elements
		_globalConstants.resize(ruleConstantList.size());

		if (RuleConstantContainer::_printDebug) {
			cout << "  Looping over [" << ruleConstantList.size() << "] json rule constants to create" << endl;
		}

		// iterate over the list of rule constants and create Constant objects to put into the RuleConstantContainer
		for (Json::ValueIterator ruleConstantElement = ruleConstantList.begin(); ruleConstantElement != ruleConstantList.end(); ++ruleConstantElement)
		{
			int productElementOid = ruleConstantElement->get("productElementOid", 0).asInt();
			int underKonceptOid = ruleConstantElement->get("underKonceptOid", 0).asInt();
			int unionAgreementOid = ruleConstantElement->get("unionAgreementOid", 0).asInt();
			//				  int generalAgreementOid = ruleConstantElement->get("generalAgreementOid", 0).asInt();
			int comparisonTypeOid = ruleConstantElement->get("comparisonTypeOid", 0).asInt();
			string value = ruleConstantElement->get("value", "").asString();
			bool isDefault = ruleConstantElement->get("isDefault", false).asBool();

			shared_ptr<Constant> constantPtr = make_shared<Constant>(static_cast<short int>(underKonceptOid),
					static_cast<short int>(unionAgreementOid),
					static_cast<ProductElementOid>(productElementOid),
					static_cast<ComparisonTypes>(comparisonTypeOid),
					value,
					isDefault);

			_globalConstants[index++] = constantPtr;
		}

		// initialise the container with the created vector
		if (RuleConstantContainer::_printDebug) cout << "  Initialising _container" << endl;

		_initInternalMaps();
	}
	else
	{
		if (RuleConstantContainer::_printErr) cerr << "No rule constants found to load" << endl;
	}
}
/**
 * Initialise the internal map of products and product elements
 */
void RuleConstantContainer::_initProductElements(const Json::Value& products)
{

	if (products.size() > 0)
	{
		if (RuleConstantContainer::_printDebug) { cout << "  Looping over [" << products.size() << "] json products to create" << endl; }

		// iterate over the list of products and create shared_ptr Product objects to put into the RuleConstantContainer
		for (Json::ValueIterator jsonProduct = products.begin(); jsonProduct != products.end(); ++jsonProduct)
		{
			int productOid = jsonProduct->get("oid", 0).asInt();
			string name = jsonProduct->get("name", "").asString();

			shared_ptr<Product> productPtr = make_shared<sbx::Product>(static_cast<sbx::product_oid>(productOid), name);

			Json::Value productElements = jsonProduct->get("productElements", 0);

			if (productElements.size() > 0)
			{
				for (Json::ValueIterator jsonPEIterator = productElements.begin(); jsonPEIterator != productElements.end(); ++jsonPEIterator)
				{
					int peOid = jsonPEIterator->get("oid", 0).asInt();
					string variableName = jsonPEIterator->get("variableName", "").asString();
					string guiName = jsonPEIterator->get("guiName", "").asString();
					int elementType = jsonPEIterator->get("elementTypeOid", 0).asInt();

					productPtr->addProductElementOid(peOid);
					shared_ptr<ProductElement> pePtr = make_shared<sbx::ProductElement>(static_cast<sbx::productelement_oid>(peOid), variableName, guiName, static_cast<sbx::ProductElementTypes>(elementType), static_cast<sbx::product_oid>(productOid));

					_productElementMap[peOid] = pePtr;
					_varNameToPEOidMap[variableName] = peOid;
				}
			}

			_addFakeProductElements(productPtr);

			_productsMap[productOid] = productPtr;
		}
	}
	else
	{
		if (RuleConstantContainer::_printErr) cerr << "No Products found to load" << endl;
	}
}

const std::map<sbx::koncept_oid, sbx::Koncept>& RuleConstantContainer::getKoncepts() const
{
	return _koncepts;
}

void RuleConstantContainer::initKoncepts(const std::string& jsonContents)
{
	Json::Reader reader{Json::Features{}};
	Json::Value root {};

	bool parsingSuccessful = reader.parse(jsonContents, root);

	if (parsingSuccessful)
	{
		Json::Value konceptsJSON = root["data"].get("KONCEPTS", 0);

		// if we can parse the json string, then get initialise the koncepts
		if (konceptsJSON.size() > 0)
		{
			if (RuleConstantContainer::_printDebug) { cout << "\n  Looping over [" << konceptsJSON.size() << "] json Koncepts to initialise" << endl; }

			// iterate over the list of rules and create Ruleobjects to put into the RuleConstantContainer
			for (Json::ValueIterator valueIterator = konceptsJSON.begin(); valueIterator != konceptsJSON.end(); ++valueIterator)
			{
				int oid = valueIterator->get("oid", 0).asInt();
				string name = valueIterator->get("name", "").asString();

				if (RuleConstantContainer::_printDebug) { cout << "  Creating Koncept [" << oid << "], name [" << name<< "]" << endl; }
				Koncept k{static_cast<sbx::koncept_oid>(oid), name};

				_initSubkoncepts(valueIterator->get("subKoncepts", ""), k);

				_koncepts.insert(make_pair(oid, k));
			}
		}
		else
		{
			if (RuleConstantContainer::_printDebug) { cout << "Empty Koncepts...nothing to load" << endl; }
		}
	}
	else
	{
		cerr << "Could not parse json string with Koncept basedata" << endl;
		throw invalid_argument(reader.getFormattedErrorMessages());
	}

}

void RuleConstantContainer::_initSubkoncepts(const Json::Value& subkonceptsJSON, sbx::Koncept& koncept)
{
	if (subkonceptsJSON.size() > 0)
	{
		if (RuleConstantContainer::_printDebug) { cout << "\n  Looping over [" << subkonceptsJSON.size() << "] json Koncepts to initialise" << endl; }

		for (Json::ValueIterator valueIterator = subkonceptsJSON.begin(); valueIterator != subkonceptsJSON.end(); ++valueIterator)
		{
			int subkonceptOid = valueIterator->get("oid", 0).asInt();
			string name = valueIterator->get("name", "").asString();
			int minEmployeeNumber = valueIterator->get("minEmployeeNumber", -1).asInt();
			int maxEmployeeNumber = valueIterator->get("maxEmployeeNumber", -1).asInt();
			int relatedSubkonceptOid = valueIterator->get("relatedUnderkonceptOid", 0).asInt();

			if (RuleConstantContainer::_printDebug) { cout << "  Creating Subkoncept [" << subkonceptOid << "], name [" << name<< "], min [" << minEmployeeNumber << "], max [" << maxEmployeeNumber << "], relatedsubkoncept oid [" << relatedSubkonceptOid << "]" << endl; }

			Subkoncept subkoncept{static_cast<sbx::subkoncept_oid> (subkonceptOid), name, static_cast<sbx::number_of_employees>(minEmployeeNumber), static_cast<sbx::number_of_employees>(maxEmployeeNumber), static_cast<sbx::subkoncept_oid>(relatedSubkonceptOid)};

			Json::Value allowedParametersJSON = valueIterator->get("parameters", "");

			if (allowedParametersJSON.size() > 0)
				for (Json::ValueIterator paramValueIterator = allowedParametersJSON.begin(); paramValueIterator != allowedParametersJSON.end(); ++paramValueIterator)
					subkoncept.addAllowedParameter(paramValueIterator->get("oid", 0).asInt());

			koncept.addSubKoncept(subkoncept);
			_subkoncepts.insert(make_pair(subkonceptOid, subkoncept));
		}
	}
	else
		if (RuleConstantContainer::_printDebug) { cout << "Empty Subkoncepts...nothing to load" << endl; }
}

void RuleConstantContainer::initUnionAgreements(const std::string& jsonContents)
{
	Json::Reader reader{Json::Features{}};
	Json::Value root {};

	bool parsingSuccessful = reader.parse(jsonContents, root);

	if (parsingSuccessful)
	{
		Json::Value unionAgreementsJSON = root["data"].get("UNION_AGREEMENTS", 0);

		// if we can parse the json string, then get initialise the koncepts
		if (unionAgreementsJSON.size() > 0)
		{
			if (RuleConstantContainer::_printDebug) { cout << "\n  Looping over [" << unionAgreementsJSON.size() << "] json UnionAgreements to initialise" << endl; }

			// iterate over the list of rules and create Ruleobjects to put into the RuleConstantContainer
			for (Json::ValueIterator valueIterator = unionAgreementsJSON.begin(); valueIterator != unionAgreementsJSON.end(); ++valueIterator)
			{
				int oid = valueIterator->get("oid", 0).asInt();
				int unionAgreementNumber = valueIterator->get("unionAgreementNumber", 0).asInt();
				string name = valueIterator->get("unionAgreementName", "").asString();
				string name2 = valueIterator->get("unionAgreementName2", "").asString();

				if (RuleConstantContainer::_printDebug) { cout << "  Creating UnionAgreement oid [" << oid << "], unionAgreementNumber[" << unionAgreementNumber << "], name [" << name << "], name2 [" << name2 << "" << endl; }
				UnionAgreement ua{static_cast<sbx::unionagreement_oid>(oid), unionAgreementNumber, name, name2};

				Json::Value unionAgreementContributionScaleSteps = valueIterator->get("unionAgreementContributionScaleSteps", "");

				if (unionAgreementContributionScaleSteps.size() > 0)
				{
					for (Json::ValueIterator paramValueIterator = unionAgreementContributionScaleSteps.begin(); paramValueIterator != unionAgreementContributionScaleSteps.end(); ++paramValueIterator)
					{
						int oid = paramValueIterator->get("oid", 0).asInt();
						string type = paramValueIterator->get("type", 0).asString();
						int year = paramValueIterator->get("year", 0).asInt();
						double employeePercent = paramValueIterator->get("employeePercent", 0).asDouble();
						double totalPercent = paramValueIterator->get("totalPercent", 0).asDouble();
						string startDate = paramValueIterator->get("startDate", 0).asString();

						sbx::Date d = (startDate.length() > 0) ? Date{startDate} : Date{"01.01.2009"};

						UnionAgreementContributionStep step{oid, employeePercent, totalPercent, year, type, d};

						ua.addContributionStep(step);
					}
				}

				_unionAgreements.insert(make_pair(oid, ua));
			}
		}
		else
		{
			if (RuleConstantContainer::_printDebug) { cout << "Empty UnionAgreements...nothing to load" << endl; }
		}
	}
	else
	{
		cerr << "Could not parse json string with UnionAgreement basedata" << endl;
		throw invalid_argument(reader.getFormattedErrorMessages());
	}

}


void RuleConstantContainer::_addFakeProductElements(std::shared_ptr<sbx::Product> productPtr)
{
	// hard code productelement for bidragstrappe (999) and set relation to product "Samlet_bidragsprocent"
	if (productPtr->getOid() == (sbx::product_oid) sbx::ProductOid::kSamlet_Bidragsprocent_ProductOid)
	{
		_productElementMap[(sbx::productelement_oid) sbx::ProductElementOid::kBidragstrappe] = make_shared<sbx::ProductElement>(static_cast<sbx::productelement_oid>(sbx::ProductElementOid::kBidragstrappe), kBidragstrappe_VARNAME, kBidragstrappe_VARNAME, sbx::ProductElementTypes::kUnknownPEType, sbx::ProductOid::kSamlet_Bidragsprocent_ProductOid);
		_varNameToPEOidMap[kBidragstrappe_VARNAME] = (sbx::productelement_oid) sbx::ProductElementOid::kBidragstrappe;
		productPtr->addProductElementOid(sbx::ProductElementOid::kBidragstrappe);
	}

	// hardcode product element for AftaleIkraftdato (998) and set relation to product "Ikraftdato_Nuvaerende"
	if (productPtr->getOid() == (sbx::product_oid) sbx::ProductOid::kIkraftdato_Nuvaerende_ProductOid)
	{
		_productElementMap[(sbx::productelement_oid) sbx::ProductElementOid::kAftaleIkraftdato] = make_shared<sbx::ProductElement>(static_cast<sbx::productelement_oid>(sbx::ProductElementOid::kAftaleIkraftdato), kAftaleIkraftdato_VARNAME, kAftaleIkraftdato_VARNAME, sbx::ProductElementTypes::kLong, sbx::ProductOid::kIkraftdato_Nuvaerende_ProductOid);
		_varNameToPEOidMap[kAftaleIkraftdato_VARNAME] = (sbx::productelement_oid) sbx::ProductElementOid::kAftaleIkraftdato;
		productPtr->addProductElementOid(sbx::ProductElementOid::kAftaleIkraftdato);
	}

	if (productPtr->getOid() == (sbx::product_oid) sbx::ProductOid::kOverenskomst_ProductOid)
	{
		// UnionAgreementOid
		_productElementMap[(sbx::productelement_oid) (sbx::ProductElementOid::kUnionAgreementOid)]
						   = make_shared<sbx::ProductElement>( static_cast<sbx::productelement_oid>(sbx::ProductElementOid::kUnionAgreementOid),
								   	   	   	   	   	   	   	   kUnionAgreementOid_VARNAME,
															   kUnionAgreementOid_VARNAME,
															   sbx::ProductElementTypes::kLong,
															   sbx::ProductOid::kOverenskomst_ProductOid);
		_varNameToPEOidMap[kUnionAgreementOid_VARNAME] = (sbx::productelement_oid) (sbx::ProductElementOid::kUnionAgreementOid);
		productPtr->addProductElementOid(sbx::ProductElementOid::kUnionAgreementOid);

		// UnionAgreementRelationship
		_productElementMap[(sbx::productelement_oid) (sbx::ProductElementOid::kUnionAgreementRelationship)]
						   = make_shared<sbx::ProductElement>( static_cast<sbx::productelement_oid>(sbx::ProductElementOid::kUnionAgreementRelationship),
								   	   	   	   	   	   	   	   kUnionAgreementRelationship_VARNAME,
															   kUnionAgreementRelationship_VARNAME,
															   sbx::ProductElementTypes::kText,
															   sbx::ProductOid::kOverenskomst_ProductOid);
		_varNameToPEOidMap[kUnionAgreementRelationship_VARNAME] = (sbx::productelement_oid) (sbx::ProductElementOid::kUnionAgreementRelationship);
		productPtr->addProductElementOid(sbx::ProductElementOid::kUnionAgreementRelationship);
	}
}

void RuleConstantContainer::_initParameters(const Json::Value& parameters)
{
	if (parameters.size() > 0)
	{
		if (RuleConstantContainer::_printDebug)
			cout << "  Looping over [" << parameters.size() << "] json parameters to create" << endl;

		// iterate over the list of parameters and extra Parameter object plus populate parameter to product map
		for (Json::ValueIterator jsonParameter = parameters.begin(); jsonParameter != parameters.end(); ++jsonParameter)
		{
			int oid = jsonParameter->get("oid", 0).asInt();
			string name = jsonParameter->get("name", 0).asString();
			string type = jsonParameter->get("type", 0).asString();
			int elementTypeOid = jsonParameter->get("elementTypeOid", 0).asInt();

			// if the parameterOid is not already found in the map, then create a Parameter and add it to the map
			Parameter parameter { static_cast<sbx::parameter_oid>(oid), name, type, static_cast<sbx::ProductElementTypes>(elementTypeOid) };
			_parameterMap[oid] = parameter;
		}
	}
	else
	{
		if (RuleConstantContainer::_printErr) cerr << "No Parameters found to load" << endl;
	}
}

bool RuleConstantContainer::existsAs(sbx::productelement_oid peOid, const sbx::ComparisonTypes& ct) const
{
	this->_checkContextInitialisation();

	subkoncept_oid skOid = _subkoncept.getOid();
	subkoncept_oid rskOid = _subkoncept.getRelatedSubkonceptOid();

	switch(ct)
	{
	case kMin: 		return ((_ukMinValuesMap.find(skOid) != _ukMinValuesMap.cend() && _ukMinValuesMap.at(skOid).find(peOid) != _ukMinValuesMap.at(skOid).cend())
						||  (_ukMinValuesMap.find(rskOid) != _ukMinValuesMap.cend() && _ukMinValuesMap.at(rskOid).find(peOid) != _ukMinValuesMap.at(rskOid).cend())
						||  (_unionAgreementRelationship == sbx::INCLUDED
								&& (_uaMinValuesMap.find(_unionAgreementOid) != _uaMinValuesMap.cend() && _uaMinValuesMap.at(_unionAgreementOid).find(peOid) != _uaMinValuesMap.at(_unionAgreementOid).cend())));

	case kMax: 		return ((_ukMaxValuesMap.find(skOid) != _ukMaxValuesMap.cend() && _ukMaxValuesMap.at(skOid).find(peOid) != _ukMaxValuesMap.at(skOid).cend())
						||  (_ukMaxValuesMap.find(rskOid) != _ukMaxValuesMap.cend() && _ukMaxValuesMap.at(rskOid).find(peOid) != _ukMaxValuesMap.at(rskOid).cend())
						||  (_unionAgreementRelationship == sbx::INCLUDED
								&& (_uaMaxValuesMap.find(_unionAgreementOid) != _uaMaxValuesMap.cend() && _uaMaxValuesMap.at(_unionAgreementOid).find(peOid) != _uaMaxValuesMap.at(_unionAgreementOid).cend())));

	case kEnum:		// fall through
	case kEquals:	return ((_ukOptionsMap.find(skOid) != _ukOptionsMap.cend() && _ukOptionsMap.at(skOid).find(peOid) != _ukOptionsMap.at(skOid).cend())
						||  (_ukOptionsMap.find(rskOid) != _ukOptionsMap.cend() && _ukOptionsMap.at(rskOid).find(peOid) != _ukOptionsMap.at(rskOid).cend())
						||  (_unionAgreementRelationship == sbx::INCLUDED
								&& (_uaOptionsMap.find(_unionAgreementOid) != _uaOptionsMap.cend() && _uaOptionsMap.at(_unionAgreementOid).find(peOid) != _uaOptionsMap.at(_unionAgreementOid).cend())));

	default: 		return false;
	}
}

void RuleConstantContainer::_initParametersToProducts(const Json::Value& parametersToProducts)
{
	if (parametersToProducts.size() > 0)
	{
		if (RuleConstantContainer::_printDebug)
			cout << "  Looping over [" << parametersToProducts.size() << "] json parameterProducts to create" << endl;

		// iterate over the list of parameters and extra Parameter object plus populate parameter to product map
		for (Json::ValueIterator jsonParameterToProduct = parametersToProducts.begin(); jsonParameterToProduct != parametersToProducts.end(); ++jsonParameterToProduct)
		{
			int parameterOid = jsonParameterToProduct->get("parameterOid", 0).asInt();
			int productOid = jsonParameterToProduct->get("productOid", 0).asInt();
			int underkonceptOid = jsonParameterToProduct->get("underKonceptOid", 0).asInt();
//			bool displayOnKIDisp = jsonParameterToProduct->get("displayOnKonceptInfoDisp", false).asBool();
			bool displayOnTA = jsonParameterToProduct->get("displayOnTA", false).asBool();

			// add the parameterOid -> productOid relation for that underkoncept to the vector of productOids
			// that contains many productOids to one parameterOid for one underkonceptOid
			_parameterOidToProductOids[underkonceptOid][parameterOid].insert(productOid);

			// if product found in product map, add all the product elements to the list of allowed product elements for this parameterOid
			if (_productsMap.find(productOid) != _productsMap.cend() && displayOnTA)
			{
				shared_ptr<sbx::Product> product = _productsMap.at(productOid);
				_parameterOidToProductElementOids[underkonceptOid][parameterOid].insert(product->getProductElementOids().cbegin(), product->getProductElementOids().cend());
			}
		}
	}
	else
	{
		if (RuleConstantContainer::_printErr) cerr << "No Parameters found to load" << endl;
	}
}

/**
 Initialises the internal maps containing shared_ptr to the constants in the _globalConstants
 */
void RuleConstantContainer::_initInternalMaps()
{
	// initialise the 3 maps of constant types
	for (auto& constant : _globalConstants)
	{
		// equals/enum goes into a vector in the _ukOptionsMap/uaOptionsMap as a shared_ptr to the Constant in a _globalConstants
		// min/max values goes into min/max-maps as a shared_ptr to the Constant in the _globalConstants
		switch (constant->getComparisonType())
		{
		case kEnum:
		case kEquals:
			if (constant->getUnderKonceptOid() != undefined_oid)
				_ukOptionsMap[constant->getUnderKonceptOid()][constant->getProductElement()].push_back(constant);

			if (constant->getUnionAgreementOid() != undefined_oid)
				_uaOptionsMap[constant->getUnionAgreementOid()][constant->getProductElement()].push_back(constant);

			break;
		case kMin:
			if (constant->getUnderKonceptOid() != undefined_oid)
				_ukMinValuesMap[constant->getUnderKonceptOid()][constant->getProductElement()] = constant;

			if (constant->getUnionAgreementOid() != undefined_oid)
				_uaMinValuesMap[constant->getUnionAgreementOid()][constant->getProductElement()] = constant;

			break;
		case kMax:
			if (constant->getUnderKonceptOid() != undefined_oid)
				_ukMaxValuesMap[constant->getUnderKonceptOid()][constant->getProductElement()] = constant;

			if (constant->getUnionAgreementOid() != undefined_oid)
				_uaMaxValuesMap[constant->getUnionAgreementOid()][constant->getProductElement()] = constant;

			break;
		default:
			break;
		}
	}
}

/**
 * Initialises the local, current context for this RuleConstantContainer
 */
void RuleConstantContainer::initContext(const sbx::Subkoncept& subkoncept, const sbx::KonceptInfo& ki, sbx::UnionAgreementRelationship uar, sbx::unionagreement_oid uaOid)
{
	// since three maps containing pointers to Constants are created in the initialisation of the globalConstants, switching context is simply to just set the underKonceptOid and unionAgreementOid and relationship
	_subkoncept = subkoncept;
	_ki = ki;
	_unionAgreementRelationship = uar;
	_unionAgreementOid = uaOid;
	_contextInitialised = true;
}

void RuleConstantContainer::clearContext()
{
	_subkoncept = {};
	_ki = {};
	_unionAgreementOid = sbx::undefined_oid;
	_contextInitialised = false;
}

bool RuleConstantContainer::isContainerInitialised() const
{
	return _contextInitialised;
}

sbx::UnionAgreementRelationship RuleConstantContainer::getUnionAgreementRelationship() const
{
	this->_checkContextInitialisation();
	return _unionAgreementRelationship;
}

sbx::unionagreement_oid RuleConstantContainer::getUnionAgreementOid() const
{
	this->_checkContextInitialisation();
	return _unionAgreementOid;
}

void RuleConstantContainer::_checkContextInitialisation() const
{
	if (!_contextInitialised)
		throw domain_error("Context not initialised!");
}
/**
 * Gets a vector of strings with option values for productElement
 */
std::vector<std::string> RuleConstantContainer::getOptions(sbx::productelement_oid productElementOid)
{
	this->_checkContextInitialisation();

	std::vector<std::shared_ptr<Constant>> constantList = this->getOptionsList(productElementOid);
	// create new vector of strings only
	std::vector<string> stringOptions(0);

	for (auto& constant : constantList)
		stringOptions.push_back(constant->stringValue());

	return stringOptions;
}

/**
 * Gets a vector of shared_ptr->Constant
 * Priority for getting constants in context.
 * 1. Union agreement constants first
 * 2. Underkoncept
 * 3. Related underkoncept
 * 4. If nothing still not found, return empty list
 */
std::vector<std::shared_ptr<Constant>> RuleConstantContainer::getOptionsList(sbx::productelement_oid productElementOid) const
{
	this->_checkContextInitialisation();

	if (_unionAgreementRelationship == sbx::UnionAgreementRelationship::INCLUDED)
	{
		if (_unionAgreementOid != undefined_oid)
		{
			try {
				return _uaOptionsMap.at(_unionAgreementOid).at(productElementOid);
			} catch (const std::out_of_range& oor) {
				// Its ok, just proceed to uk / related uk options lists
				if (RuleConstantContainer::_printDebug) cout << "OptionsList not found in uaMap [" << _unionAgreementOid << "][" << productElementOid <<"] : " << oor.what();
			}
		}
		else
		{
			stringstream ss {};
			ss << "Union Agreement Context not initialised correctly! Invalid Union agreement oid[" << _unionAgreementOid << "]!";
			throw domain_error(ss.str());
		}
	}

	try {
		return _ukOptionsMap.at(_subkoncept.getOid()).at(productElementOid);
	} catch (const std::out_of_range& oor) {
		// Its ok, just proceed to related uk options lists
		if (RuleConstantContainer::_printDebug) cout << "OptionsList not found in uk Map [" << _subkoncept.getOid() << "][" << productElementOid <<"] : " << oor.what();
	}

	if (_subkoncept.getRelatedSubkonceptOid() != undefined_oid)
	{
		try {
			return _ukOptionsMap.at(_subkoncept.getRelatedSubkonceptOid()).at(productElementOid);
		} catch (const std::out_of_range& oor) {
			// Its ok, just proceed to return empty list
			if (RuleConstantContainer::_printDebug) cout << "OptionsList not found in related uk Map [" << _subkoncept.getRelatedSubkonceptOid() << "][" << productElementOid <<"] : " << oor.what();
		}
	}

	return {};
}

/**
 * Gets a shared_ptr to a single Constant for the productElement and comparisonType
 */
std::shared_ptr<sbx::Constant> RuleConstantContainer::getConstant(sbx::productelement_oid productElementOid, const sbx::ComparisonTypes& comparisonType)
{
	this->_checkContextInitialisation();

	subkoncept_oid ukOid = _subkoncept.getOid();

	switch (comparisonType)
	{
	case kMin:
		if (_unionAgreementRelationship == sbx::INCLUDED)
			if (_uaMinValuesMap.find(_unionAgreementOid) != _uaMinValuesMap.cend()
					&& _uaMinValuesMap.at(_unionAgreementOid).find(productElementOid) != _uaMinValuesMap.at(_unionAgreementOid).cend())
				return _uaMinValuesMap.at(_unionAgreementOid).at(productElementOid);

		if (_ukMinValuesMap.find(ukOid) != _ukMinValuesMap.cend()
				&& _ukMinValuesMap.at(ukOid).find(productElementOid) != _ukMinValuesMap.at(ukOid).cend())
			return _ukMinValuesMap.at(ukOid).at(productElementOid);

		if (_subkoncept.getRelatedSubkonceptOid() != undefined_oid)
		{
			subkoncept_oid rukOid = _subkoncept.getRelatedSubkonceptOid();

			if (_ukMinValuesMap.find(rukOid) != _ukMinValuesMap.cend()
					&& _ukMinValuesMap.at(rukOid).find(productElementOid) != _ukMinValuesMap.at(rukOid).cend())
				return _ukMinValuesMap.at(rukOid).at(productElementOid);
		}
		break;
	case kMax:
		if (_unionAgreementRelationship == sbx::INCLUDED)
			if (_uaMaxValuesMap.find(_unionAgreementOid) != _uaMaxValuesMap.cend()
					&& _uaMaxValuesMap.at(_unionAgreementOid).find(productElementOid) != _uaMaxValuesMap.at(_unionAgreementOid).cend())
				return _uaMaxValuesMap.at(_unionAgreementOid).at(productElementOid);

		if (_ukMaxValuesMap.find(ukOid) != _ukMaxValuesMap.cend()
				&& _ukMaxValuesMap.at(ukOid).find(productElementOid) != _ukMaxValuesMap.at(ukOid).cend())
			return _ukMaxValuesMap.at(ukOid).at(productElementOid);

		if (_subkoncept.getRelatedSubkonceptOid() != undefined_oid)
		{
			subkoncept_oid rukOid = _subkoncept.getRelatedSubkonceptOid();

			if (_ukMaxValuesMap.find(rukOid) != _ukMaxValuesMap.cend()
					&& _ukMaxValuesMap.at(rukOid).find(productElementOid) != _ukMaxValuesMap.at(rukOid).cend())
				return _ukMaxValuesMap.at(rukOid).at(productElementOid);
		}
		break;
	case kUnknown:
		default:
		throw domain_error("Only ComparisonType (Min, Max) supported");
	}

	// constant not found - create dummy constant and return that
	return createConstant(0, 0, productElementOid, comparisonType);
}

std::shared_ptr<sbx::Constant> RuleConstantContainer::createConstant(sbx::subkoncept_oid underkonceptOid, sbx::unionagreement_oid unionAgreementOid, sbx::productelement_oid peOid, sbx::ComparisonTypes comparisonType)
{
	ostringstream s {};
	switch (comparisonType)
	{
	case kMin:
		s << fixed << 0; //std::numeric_limits<long>::min();
		break;
	case kMax:
		s << fixed << std::numeric_limits<long>::max();
		break;
	default:
		s << "";
		break;
	}

	if (RuleConstantContainer::_printDebug)
		cout << "Creating default comparison type [" << sbx::utils::comparisonTypeName(comparisonType) << "] value [" << s.str() << "]" << endl;

	return make_shared<sbx::Constant>(underkonceptOid, unionAgreementOid, static_cast<sbx::ProductElementOid>(peOid), comparisonType, s.str(), false, false);
}

std::set<sbx::product_oid> RuleConstantContainer::getProductOids(sbx::parameter_oid parameterOid) const
{
	this->_checkContextInitialisation();

	subkoncept_oid ukOid = _subkoncept.getOid();

	std::set<sbx::product_oid> productOids{};

	if (_parameterOidToProductOids.find(ukOid) != _parameterOidToProductOids.cend()
			&& _parameterOidToProductOids.at(ukOid).find(parameterOid) != _parameterOidToProductOids.at(ukOid).cend())
		copy(	_parameterOidToProductOids.at(ukOid).at(parameterOid).cbegin(),
				_parameterOidToProductOids.at(ukOid).at(parameterOid).cend(),
				std::inserter(productOids, productOids.begin()));

	if (_subkoncept.getRelatedSubkonceptOid() != undefined_oid)
	{
		subkoncept_oid relatedUKOid = _subkoncept.getRelatedSubkonceptOid();

		if (_parameterOidToProductOids.find(relatedUKOid) != _parameterOidToProductOids.cend()
				&& _parameterOidToProductOids.at(relatedUKOid).find(parameterOid) != _parameterOidToProductOids.at(relatedUKOid).cend())
			copy(	_parameterOidToProductOids.at(relatedUKOid).at(parameterOid).cbegin(),
					_parameterOidToProductOids.at(relatedUKOid).at(parameterOid).cend(),
					std::inserter(productOids, productOids.begin()));
	}

	return productOids;
}

std::set<sbx::productelement_oid, std::less<sbx::productelement_oid>> RuleConstantContainer::getAllowedPEOids() const
{
	std::set<sbx::productelement_oid, std::less<sbx::productelement_oid>> allowedProductElementOids {};

	for (auto& parameterIt : _ki.getParameterValues()) {
		const std::set<sbx::productelement_oid>& productElementOids = this->getProductElementOids(parameterIt.first);
		allowedProductElementOids.insert(productElementOids.cbegin(), productElementOids.cend());
	}

	return allowedProductElementOids;
}

/**
 * \brief Gets allowed productelement oids for the given parameter Oid
 *
 * \param parameterOid parameter oid to get product elements for.
 * \return set of product element oids allowed for the parameterOid
 */
std::set<sbx::productelement_oid> RuleConstantContainer::getProductElementOids(sbx::parameter_oid parameterOid) const
{
	this->_checkContextInitialisation();

	subkoncept_oid ukOid = _subkoncept.getOid();

	std::set<sbx::productelement_oid> productElementOids{};

	if (_parameterOidToProductElementOids.find(ukOid) != _parameterOidToProductElementOids.cend()
			&& _parameterOidToProductElementOids.at(ukOid).find(parameterOid) != _parameterOidToProductElementOids.at(ukOid).cend())

		copy(	_parameterOidToProductElementOids.at(ukOid).at(parameterOid).cbegin(),
				_parameterOidToProductElementOids.at(ukOid).at(parameterOid).cend(),
				std::inserter(productElementOids, productElementOids.begin()));
	else
		if (RuleConstantContainer::_printDebug)  { cout << "No ProductElementOids found for subkoncept [" << ukOid << "], parameterOid[" << parameterOid << "]!"; }

	// add productelements for related subkoncept
	if (_subkoncept.getRelatedSubkonceptOid() != undefined_oid)
	{
		subkoncept_oid relatedUKOid = _subkoncept.getRelatedSubkonceptOid();

		if (_parameterOidToProductElementOids.find(relatedUKOid) != _parameterOidToProductElementOids.cend()
				&& _parameterOidToProductElementOids.at(relatedUKOid).find(parameterOid) != _parameterOidToProductElementOids.at(relatedUKOid).cend())
			copy(	_parameterOidToProductElementOids.at(relatedUKOid).at(parameterOid).cbegin(),
					_parameterOidToProductElementOids.at(relatedUKOid).at(parameterOid).cend(),
					std::inserter(productElementOids, productElementOids.begin()));
		else
			if (RuleConstantContainer::_printDebug)  { cout << "No ProductElementOids found for related subkoncept [" << relatedUKOid << "], parameterOid[" << parameterOid << "]!"; }
	}

	return productElementOids;
}

/**
 * Gets the number of global constants in this container
 */
std::size_t RuleConstantContainer::size() const
{
	return _globalConstants.size();
}

sbx::ProductElement RuleConstantContainer::getProductElement(sbx::productelement_oid productElementOid)
{
	if (_productElementMap.find(productElementOid) != _productElementMap.cend())
		return _productElementMap.at(productElementOid);

	std::stringstream s {};
	s << productElementOid;
	throw domain_error("ProductElement [" + s.str() + "] not found!");
}

/**
 * Returns the peOid corrosponding to the varName
 * If no peOid is found for the varName kUnknownProductElement == 0 is returned
 */
sbx::productelement_oid RuleConstantContainer::getProductElementOid(const std::string& varName) const
{
	if (_varNameToPEOidMap.find(varName) != _varNameToPEOidMap.cend())
	{
		return _varNameToPEOidMap.at(varName);
	}

	return (sbx::productelement_oid) kUnknownProductElement;
}

sbx::UnionAgreementContributionStep RuleConstantContainer::getUAContributionStep(sbx::unionagreement_oid uaOid, const sbx::Date& inceptionDate) const
{
	if (_unionAgreements.find(uaOid) != _unionAgreements.cend())
	{
		UnionAgreement ua = _unionAgreements.at(uaOid);

		if (ua.getContributionSteps().size() == 1)
		{
			auto step = ua.getContributionSteps().find(UnionAgreementContributionStep{ua.getContributionSteps().cbegin()->oid()});
			return UnionAgreementContributionStep{*step};//
		}

		if (ua.getContributionSteps().size() > 0)
		{
			for ( UnionAgreementContributionStep step : ua.getContributionSteps())
			{
				if (step.startDate() >= inceptionDate)
					return step;
			}

			auto step = ua.getContributionSteps().find(UnionAgreementContributionStep{ua.getContributionSteps().crbegin()->oid()});
			return UnionAgreementContributionStep{step->oid(), step->employeePct(), step->totalPct(), step->year(), step->type(), step->startDate()};
		}
	}

	stringstream ss{};
	ss << uaOid;
	throw domain_error("Contribution ladder for Union Agreement [" + ss.str() + "] not initialised!");
}

const sbx::UnionAgreement& RuleConstantContainer::getUnionAgreement(sbx::unionagreement_oid uaOid) const
{
	this->_checkContextInitialisation();

	const auto& it = _unionAgreements.find(uaOid);

	if (it != _unionAgreements.cend())
		return it->second;

	stringstream ss{};
	ss << uaOid;
	throw domain_error("Union Agreement with oid [" + ss.str() + "] not loaded!");
}

bool RuleConstantContainer::isUnionAgreementLoaded(sbx::unionagreement_oid uaOid) const
{
	return (_unionAgreements.find(uaOid) != _unionAgreements.cend());
}

void RuleConstantContainer::printKoncepts() const
{
	cout << "\n\n       ---======== Koncepts =======--- \n";
	cout << "Number of Koncepts loaded: " << _koncepts.size() << endl;

	for (const auto& i : _koncepts)
	{
		cout << "\n" << i.second.getName() << " (" << i.second.getOid() << ")";

		cout << "\nOid         Name                             Employees      Allowed Parameters";
		cout << "\n----------  -------------------------------  -------------  ------------------------" << endl;
		for (auto& ii : i.second.getSubkoncepts())
		{
			const Subkoncept& sk = ii.second;
			stringstream ss{};
			ss << "(" << sk.getOid() << " -> " << sk.getRelatedSubkonceptOid() << ")";
			cout << setw(10) << ss.str();
			cout << "  " << left << setw(31)  << sk.getName();
			ss.str("");
			ss << "[" << sk.getMinNumberOfEmployees() << "-" << sk.getMaxNumberOfEmployees()<< "]";
			cout << "  " << setw(13) << ss.str();

			ss.str("");
			for_each(sk.getAllowedParameters().cbegin(), sk.getAllowedParameters().cend(), [&ss](sbx::parameter_oid pOid) { ss << pOid << ", ";});
			cout << "  " << ss.str().substr(0, ss.str().length()-2);

			cout << endl;
		}
	}
	cout << "\n------------- Koncepts END ------------" << endl;
}

void RuleConstantContainer::printUnionAgreements() const
{
	cout << "\n\n       ---======== UnionAgreements =======--- \n";
	cout << "Number of Union Agreements loaded: " << _unionAgreements.size() << endl;

	for (const auto& i : _unionAgreements)
	{
		cout << "\n" << i.second.getName() << " (" << i.second.getOid() << ")";

		cout << "\nOid  Type        Year  EmpPct  Total  StartDate";
		cout << "\n---  ----------  ----  ------  -----  ----------" << endl;
		for (const auto& step : i.second.getContributionSteps())
		{
			cout << left << setw(3) << step.oid();
			cout << "  " << setw(10) << step.type();
			cout << "  " << setw(4) << step.year();
			cout << "  " << setw(6) << step.employeePct();
			cout << "  " << setw(5) << step.totalPct();
			cout << "  " << setw(10) << step.startDate();

			cout << endl;
		}
	}
	cout << "\n------------- UnionAgreements END ------------" << endl;
}

/**
 * Outputs to cout the entire content of Constant Container
 */
void RuleConstantContainer::printSubkonceptConstants(sbx::subkoncept_oid subkonceptOid, sbx::productelement_oid productElement) const
{
//	if (subkonceptOid == undefined_oid)
//	{
//		cout << "\n       ---======== Global Constants =======--- \n\n";
//		printConstantHeader();
//
//		for (const auto& c : _globalConstants)
//			printConstant(make_shared<Constant>(c));
//	}
//	else
	cout << "\n\nInitialised context: Underkoncept[" << _subkoncept.getOid() << "] / UnionAgreement[" << _unionAgreementOid << "]" << endl;
	_printStringOptions("Underkoncept", _ukOptionsMap, subkonceptOid, productElement);

	if (subkonceptOid != undefined_oid)
	{
		Subkoncept subkoncept = _subkoncepts.at(subkonceptOid);
		if (subkoncept.getRelatedSubkonceptOid() != undefined_oid)
			_printStringOptions("Related Underkoncept", _ukOptionsMap, subkoncept.getRelatedSubkonceptOid(), productElement);
	}

	_printConstants("Min Subkoncept", _ukMinValuesMap, subkonceptOid, productElement);

	if (subkonceptOid != undefined_oid)
	{
		Subkoncept subkoncept = _subkoncepts.at(subkonceptOid);
		if (subkoncept.getRelatedSubkonceptOid() != undefined_oid)
			_printConstants("Min Related subkoncept ", _ukMinValuesMap, subkoncept.getRelatedSubkonceptOid(), productElement);
	}

	_printConstants("Max Subkoncept", _ukMaxValuesMap, subkonceptOid, productElement);

	if (subkonceptOid != undefined_oid)
	{
		Subkoncept subkoncept = _subkoncepts.at(subkonceptOid);
		if (subkoncept.getRelatedSubkonceptOid() != undefined_oid)
			_printConstants("Max Related subkoncept ", _ukMaxValuesMap, subkoncept.getRelatedSubkonceptOid(), productElement);
	}
}

/**
 * Print constant values for
 * 1. subkoncept
 * 2. related subkoncept if exists
 * 3. union agreement if exists
 */
void RuleConstantContainer::printConstantsFromContext(sbx::productelement_oid peOid) const
{
	this->printSubkonceptConstants(_subkoncept.getOid(), peOid);

	if (_unionAgreementRelationship == INCLUDED)
		this->printUnionAgreementConstants(_unionAgreementOid, peOid);
}

void RuleConstantContainer::printUnionAgreementConstants(sbx::unionagreement_oid uaOid, sbx::productelement_oid peOid) const
{
	_printStringOptions("Union Agreement", _uaOptionsMap, uaOid, peOid);
	_printConstants("Min Union Agreement", _uaMinValuesMap, uaOid, peOid);
	_printConstants("Max Union Agreement", _uaMaxValuesMap, uaOid, peOid);
}

void RuleConstantContainer::_printStringOptions(const std::string& mapHeader, const std::map<sbx::oid, std::map<sbx::productelement_oid, std::vector<std::shared_ptr<sbx::Constant>>>>& map, sbx::oid mapOid, sbx::productelement_oid peOid) const
{
	cout << "\n\n       ---======== String Options - " << mapHeader << "(" << mapOid << "," << peOid << ") =======--- \n";
	printConstantHeader();

	if (map.find(mapOid) == map.cend())
		// if mapOid not found, run though all mapOids and print for all.
		for(const auto& mapIt : map)
		{
			if (peOid == undefined_oid)
				for (const auto& peIt : mapIt.second)
					for (const auto& c : peIt.second)
						this->printConstant(c);
			else
				if (mapIt.second.find(peOid) != mapIt.second.cend())
				{
					for (const auto& c : mapIt.second.at(peOid))
						this->printConstant(c);
				}
				else
					cout << "No string options for peOid (" << peOid << ")" << endl;
		}
	else
	{
		if (peOid == undefined_oid)
		{
			for (const auto& peIt : map.at(mapOid))
				for (const auto& c : peIt.second)
					this->printConstant(c);
		}
		else
			if (map.at(mapOid).find(peOid) != map.at(mapOid).cend())
				for (const auto& c : map.at(mapOid).at(peOid))
					this->printConstant(c);
			else
				cout << "No string options for peOid (" << peOid << ")" << endl;
	}
}

void RuleConstantContainer::_printConstants(const std::string& mapHeader, const std::map<sbx::oid, std::map<sbx::productelement_oid, std::shared_ptr<sbx::Constant>>>& map, sbx::oid mapOid, sbx::productelement_oid peOid) const
{
	cout << "\n\n       ---======== " << mapHeader << " values =======--- \n\n";
	printConstantHeader();

	if (mapOid == undefined_oid)
		for (const auto& ukit : map)
			for (const auto& peit : ukit.second)
				printConstant(peit.second);
	else
		for (const auto& peit : map.at(mapOid))
			printConstant(peit.second);
}

void RuleConstantContainer::printConstantHeader() const
{
	cout << "Address    UK  UA  PE   CT  strValue                                       double       long  bool   isDefault" << endl;
	cout << "---------  --  --  ---  --  ------------------------------------------  ---------  ---------  -----  ---------" << endl;
}

void RuleConstantContainer::printConstant(const std::shared_ptr<sbx::Constant>& c) const
{
	cout << left << setw(9) << c;
	cout << "  " << right << setw(2) << c->getUnderKonceptOid();
	cout << "  " << right << setw(2) << c->getUnionAgreementOid();
	cout << "  " << right << setw(3) << (int) c->getProductElement();
	cout << "  " << right << setw(2) << (int) c->getComparisonType();
	cout << "  " << left << setw(40) << c->stringValue().substr(0, 40);
	cout << "  " << right << setw(11) << c->doubleValue();
	cout << "  " << right << setw(9) << c->longValue();
	cout << "  " << right << setw(5) << boolalpha << c->boolValue();
	cout << "  " << left << setw(7) << (c->isDefault() ? "*" : "") << endl;
}

void RuleConstantContainer::printContainerOverview(sbx::subkoncept_oid underKonceptOid) const
{
	subkoncept_oid ukOid = _subkoncept.getOid();
	cout << "Current context : UK[" << ukOid << "], UA[" << _unionAgreementOid << "]" << endl;
	cout << "Global constants: [" << size() << "]" << endl;
	cout << "Products: [" << _productsMap.size() << "]" << endl;
	cout << "ProductsElements: [" << _productElementMap.size() << "]" << endl;
	cout << "Parameters: [" << _parameterMap.size() << "]" << endl;

	// print stats for all underkoncepts
	cout << "UK\tEquals\tMin\tMax\tParameters\n";
	if (underKonceptOid == undefined_oid)
	{
		for (const auto& optionsMapItem : _ukOptionsMap)
		{
			cout << right << setw(2) << optionsMapItem.first;
			cout << "\t" << right << setw(6) << optionsMapItem.second.size();
			cout << "\t" << right << setw(3) << (_ukMinValuesMap.find(optionsMapItem.first) != _ukMinValuesMap.end() ? _ukMinValuesMap.at(optionsMapItem.first).size() : 0);
			cout << "\t" << right << setw(3) << (_ukMaxValuesMap.find(optionsMapItem.first) != _ukMaxValuesMap.end() ? _ukMaxValuesMap.at(optionsMapItem.first).size() : 0);
			cout << "\t" << right << setw(3)
					<< (_parameterOidToProductOids.find(optionsMapItem.first) != _parameterOidToProductOids.end() ? _parameterOidToProductOids.at(optionsMapItem.first).size() : 0);
			cout << endl;
		}
	}
	else
	{
		// print stats for this underkoncept
		cout << right << setw(2) << underKonceptOid;
		cout << "\t" << right << setw(6) << (_ukOptionsMap.find(underKonceptOid) != _ukOptionsMap.end() ? _ukOptionsMap.at(underKonceptOid).size() : 0);
		cout << "\t" << right << setw(3) << (_ukMinValuesMap.find(underKonceptOid) != _ukMinValuesMap.end() ? _ukMinValuesMap.at(underKonceptOid).size() : 0);
		cout << "\t" << right << setw(3) << (_ukMaxValuesMap.find(underKonceptOid) != _ukMaxValuesMap.end() ? _ukMaxValuesMap.at(underKonceptOid).size() : 0);
		cout << "\t" << right << setw(3)
				<< (_parameterOidToProductOids.find(underKonceptOid) != _parameterOidToProductOids.end() ? _parameterOidToProductOids.at(underKonceptOid).size() : 0);
		cout << endl;
	}
}

void RuleConstantContainer::printContainerOverview(sbx::subkoncept_oid underKonceptOid, sbx::ComparisonTypes type) const
{
	subkoncept_oid ukOid = _subkoncept.getOid();
	cout << "Current context : Underkoncept[" << ukOid << "], UnionAgreement[" << _unionAgreementOid << "]" << endl;
	cout << "Showing context : Underkoncept[" << underKonceptOid << "], type[" << (int) type << "]" << endl;

	switch (type)
	{
	case kEquals:
		cout << "Constants for type: [" << static_cast<short int>(type) << "], [" << _ukOptionsMap.at(underKonceptOid).size() << "]" << endl;
		cout << " PE\tValues" << endl;

		for (auto& iterator : _ukOptionsMap.at(underKonceptOid))
		{
			cout << right << setw(3) << (int) iterator.first << "\t";

			for (auto& value : iterator.second)
			{
				cout << value->stringValue() << ", ";
			}
			cout << endl;
		}

		break;
	case kMin:
		//                count = _ukMinValuesMap.at(underKonceptOid).size();
		cout << "nothing yet" << endl;
		break;
	case kMax:
		//                count = _ukMaxValuesMap.at(underKonceptOid).size();
		cout << "nothing yet" << endl;
		break;
	default:
		cout << "nothing yet" << endl;
		break;
	}
}

void RuleConstantContainer::printProducts() const
{
	cout << "\n\n       ---======== Products loaded =======--- \n";
	cout << "Index  Oid  Name                                         Product Elements" << endl;
	cout << "-----  ---  -------------------------------------------- ----------------" << endl;

	for (auto& product : _productsMap)
	{
		cout << right << setw(5) << product.first;
		cout << "  " << right << setw(3) << product.second->getOid();
		cout << "  " << left << setw(43) << product.second->getName();

		if (product.second->getProductElementOids().size() > 0)
		{
			ostringstream s{};
			ostream_iterator<short> oit(s, ", ");
			copy(product.second->getProductElementOids().cbegin(), product.second->getProductElementOids().cend(), oit);
			cout << "  " << s.str();
		}

		cout << endl;
	}
}

void RuleConstantContainer::printParameters() const
{
	cout << "\n\n       ---======== Parameters loaded =======--- \n";
	cout << "Index  Oid  Name                                                  Type         ElementType" << endl;
	cout << "-----  ---  ----------------------------------------------------  -----------  -----------" << endl;

	if (_parameterMap.size() > 0)
	{
		for (auto& parameter : _parameterMap)
		{
			cout << right << setw(5) << parameter.first;
			cout << "  " << right << setw(3) << parameter.second.getOid();
			cout << "  " << left << setw(52) << parameter.second.getName().substr(0, 50);
			cout << "  " << left << setw(11) << parameter.second.getType();
			cout << "  " << right << setw(10) << (int) parameter.second.getElementType();
			cout << endl;
		}
	}
	else
	{
		cout << "No Parameters loaded" << endl;
	}
}

void RuleConstantContainer::printParametersToProducts(sbx::koncept_oid kOid) const
{

	auto start = (kOid != undefined_oid) ? _koncepts.find(kOid) : _koncepts.cbegin();
	auto end = (kOid != undefined_oid) ? _koncepts.find(kOid) : _koncepts.cend();

	for (auto& it = start; it != end; it++ )
	{
		const Koncept& koncept = it->second;

		cout << "\n\n    ---- " << koncept.getName() << " (" << koncept.getOid() << ") ----" << endl;

		for (const auto& item : koncept.getSubkoncepts())
		{
			const Subkoncept& subkoncept = item.second;

			cout << "\n" << subkoncept.getName() << " (" << subkoncept.getOid();

			if (subkoncept.getRelatedSubkonceptOid() != undefined_oid)
				cout << "-> " << subkoncept.getRelatedSubkonceptOid();

			cout << ")" << endl;

			if (_parameterOidToProductOids.find(subkoncept.getOid()) != _parameterOidToProductOids.end())
			{
				cout << "Parameters Directly on subkoncept [" << subkoncept.getOid() << "]" << endl;
				cout << "ParamOid  Product oids" << endl;
				cout << "--------  -------------------------------------------------------" << endl;
				_printParameterList(_parameterOidToProductOids.at(subkoncept.getOid()));
			}
			else
				cout << "No Parameters To Products loaded for subkoncept [" << subkoncept.getOid() << "]" << endl;

			if (subkoncept.getRelatedSubkonceptOid() != undefined_oid)
			{
				cout << "Parameters on related subkoncept [" << subkoncept.getRelatedSubkonceptOid() << "]" << endl;
				cout << "ParamOid  Product oids" << endl;
				cout << "--------  -------------------------------------------------------" << endl;

				if (_parameterOidToProductOids.find(subkoncept.getRelatedSubkonceptOid()) != _parameterOidToProductOids.end())
					_printParameterList(_parameterOidToProductOids.at(subkoncept.getRelatedSubkonceptOid()));
				else
					cout << "No Parameters To Products loaded" << endl;
			}
		}
	}
}

void RuleConstantContainer::_printParameterList(const std::map<sbx::parameter_oid, std::set<sbx::product_oid>>& values) const
{
	for (auto& itemIt : values)
	{
		cout << setw(8) << itemIt.first;
		if (itemIt.second.size() > 0)
		{
			ostringstream s{};
			ostream_iterator<sbx::product_oid> oit { s, ", " };
			copy(itemIt.second.cbegin(), itemIt.second.cend(), oit);
			cout << "  " << s.str();
		}
		cout << endl;
	}
}


} // sbx namespace end
