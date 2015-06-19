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

			shared_ptr<Product> productPtr = make_shared<sbx::Product>(static_cast<unsigned short>(productOid), name);

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
					shared_ptr<ProductElement> pePtr = make_shared<sbx::ProductElement>(static_cast<unsigned short>(peOid), variableName, guiName, static_cast<sbx::ProductElementTypes>(elementType), static_cast<unsigned short>(productOid));

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
			if (RuleEngine::_printDebug) { cout << "\n  Looping over [" << konceptsJSON.size() << "] json Koncepts to initialise" << endl; }

			// iterate over the list of rules and create Ruleobjects to put into the RuleConstantContainer
			for (Json::ValueIterator valueIterator = konceptsJSON.begin(); valueIterator != konceptsJSON.end(); ++valueIterator)
			{
				int oid = valueIterator->get("oid", 0).asInt();
				string name = valueIterator->get("name", "").asString();

				if (RuleEngine::_printDebug) { cout << "  Creating Koncept [" << oid << "], name [" << name<< "]" << endl; }
				Koncept k{static_cast<sbx::koncept_oid>(oid), name};

				_initSubkoncepts(valueIterator->get("subKoncepts", ""), k);

				_koncepts.insert(make_pair(oid, k));
			}
		}
		else
		{
			if (RuleEngine::_printDebug) { cout << "Empty Koncepts...nothing to load" << endl; }
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
		if (RuleEngine::_printDebug) { cout << "\n  Looping over [" << subkonceptsJSON.size() << "] json Koncepts to initialise" << endl; }

		for (Json::ValueIterator valueIterator = subkonceptsJSON.begin(); valueIterator != subkonceptsJSON.end(); ++valueIterator)
		{
			int subkonceptOid = valueIterator->get("oid", 0).asInt();
			string name = valueIterator->get("name", "").asString();
			int minEmployeeNumber = valueIterator->get("minEmployeeNumber", -1).asInt();
			int maxEmployeeNumber = valueIterator->get("maxEmployeeNumber", -1).asInt();
			int relatedSubkonceptOid = valueIterator->get("relatedUnderkonceptOid", 0).asInt();

			if (RuleEngine::_printDebug) { cout << "  Creating Subkoncept [" << subkonceptOid << "], name [" << name<< "], min [" << minEmployeeNumber << "], max [" << maxEmployeeNumber << "], relatedsubkoncept oid [" << relatedSubkonceptOid << "]" << endl; }

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
		if (RuleEngine::_printDebug) { cout << "Empty Subkoncepts...nothing to load" << endl; }
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
			if (RuleEngine::_printDebug) { cout << "\n  Looping over [" << unionAgreementsJSON.size() << "] json UnionAgreements to initialise" << endl; }

			// iterate over the list of rules and create Ruleobjects to put into the RuleConstantContainer
			for (Json::ValueIterator valueIterator = unionAgreementsJSON.begin(); valueIterator != unionAgreementsJSON.end(); ++valueIterator)
			{
				int oid = valueIterator->get("oid", 0).asInt();
				int unionAgreementNumber = valueIterator->get("unionAgreementNumber", 0).asInt();
				string name = valueIterator->get("unionAgreementName", "").asString();
				string name2 = valueIterator->get("unionAgreementName2", "").asString();

				if (RuleEngine::_printDebug) { cout << "  Creating UnionAgreement oid [" << oid << "], unionAgreementNumber[" << unionAgreementNumber << "], name [" << name << "], name2 [" << name2 << "" << endl; }
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

						UnionAgreementContributionStep step{oid, employeePercent, totalPercent, year, type, startDate};
						ua.addContributionStep(step);
					}
				}

				_unionAgreements.insert(make_pair(oid, ua));
			}
		}
		else
		{
			if (RuleEngine::_printDebug) { cout << "Empty UnionAgreements...nothing to load" << endl; }
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
	if (productPtr->getOid() == (unsigned short) sbx::ProductOid::kSamlet_Bidragsprocent_ProductOid)
	{
		_productElementMap[(unsigned short) sbx::ProductElementOid::kBidragstrappe] = make_shared<sbx::ProductElement>(static_cast<unsigned short>(sbx::ProductElementOid::kBidragstrappe), kBidragstrappe_VARNAME, kBidragstrappe_VARNAME, sbx::ProductElementTypes::kUnknownPEType, sbx::ProductOid::kSamlet_Bidragsprocent_ProductOid);
		_varNameToPEOidMap[kBidragstrappe_VARNAME] = (unsigned short) sbx::ProductElementOid::kBidragstrappe;
		productPtr->addProductElementOid(sbx::ProductElementOid::kBidragstrappe);
	}

	// hardcode product element for AftaleIkraftdato (998) and set relation to product "Ikraftdato_Nuvaerende"
	if (productPtr->getOid() == (unsigned short) sbx::ProductOid::kIkraftdato_Nuvaerende_ProductOid)
	{
		_productElementMap[(unsigned short) sbx::ProductElementOid::kAftaleIkraftdato] = make_shared<sbx::ProductElement>(static_cast<unsigned short>(sbx::ProductElementOid::kAftaleIkraftdato), kAftaleIkraftdato_VARNAME, kAftaleIkraftdato_VARNAME, sbx::ProductElementTypes::kLong, sbx::ProductOid::kIkraftdato_Nuvaerende_ProductOid);
		_varNameToPEOidMap[kAftaleIkraftdato_VARNAME] = (unsigned short) sbx::ProductElementOid::kAftaleIkraftdato;
		productPtr->addProductElementOid(sbx::ProductElementOid::kAftaleIkraftdato);
	}

	if (productPtr->getOid() == (unsigned short) sbx::ProductOid::kOverenskomst_ProductOid)
	{
		// UnionAgreementOid
		_productElementMap[(unsigned short) (sbx::ProductElementOid::kUnionAgreementOid)]
						   = make_shared<sbx::ProductElement>( static_cast<unsigned short>(sbx::ProductElementOid::kUnionAgreementOid),
								   	   	   	   	   	   	   	   kUnionAgreementOid_VARNAME,
															   kUnionAgreementOid_VARNAME,
															   sbx::ProductElementTypes::kLong,
															   sbx::ProductOid::kOverenskomst_ProductOid);
		_varNameToPEOidMap[kUnionAgreementOid_VARNAME] = (unsigned short) (sbx::ProductElementOid::kUnionAgreementOid);
		productPtr->addProductElementOid(sbx::ProductElementOid::kUnionAgreementOid);

		// UnionAgreementRelationship
		_productElementMap[(unsigned short) (sbx::ProductElementOid::kUnionAgreementRelationship)]
						   = make_shared<sbx::ProductElement>( static_cast<unsigned short>(sbx::ProductElementOid::kUnionAgreementRelationship),
								   	   	   	   	   	   	   	   kUnionAgreementRelationship_VARNAME,
															   kUnionAgreementRelationship_VARNAME,
															   sbx::ProductElementTypes::kText,
															   sbx::ProductOid::kOverenskomst_ProductOid);
		_varNameToPEOidMap[kUnionAgreementRelationship_VARNAME] = (unsigned short) (sbx::ProductElementOid::kUnionAgreementRelationship);
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
			Parameter parameter { static_cast<unsigned short>(oid), name, type, static_cast<sbx::ProductElementTypes>(elementTypeOid) };
			_parameterMap[oid] = parameter;
		}
	}
	else
	{
		if (RuleConstantContainer::_printErr) cerr << "No Parameters found to load" << endl;
	}
}

bool RuleConstantContainer::existsAs(unsigned short peOid, const sbx::ComparisonTypes& ct) const
{
	if (!_contextInitialised)
	{
		throw domain_error("Context not initialised!");
	}

	subkoncept_oid skOid = _subkoncept.getOid();

	switch(ct)
	{
	case kMin: 		return (_ukMinValuesMap.find(skOid) != _ukMinValuesMap.cend() && _ukMinValuesMap.at(skOid).find(peOid) != _ukMinValuesMap.at(skOid).cend());
	case kMax: 		return (_ukMaxValuesMap.find(skOid) != _ukMaxValuesMap.cend() && _ukMaxValuesMap.at(skOid).find(peOid) != _ukMaxValuesMap.at(skOid).cend());
	case kEnum:		// fall through
	case kEquals:	return (_ukOptionsMap.find(skOid) != _ukOptionsMap.cend() && _ukOptionsMap.at(skOid).find(peOid) != _ukOptionsMap.at(skOid).cend());
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
	for (const auto& constant : _globalConstants)
	{

		// equals/enum goes into _ukOptionsMap as a shared_ptr to the Constant in a _globalConstants
		// min/max values goes into min/max-map as a shared_ptr to the Constant in the _globalConstants
		switch (constant->getComparisonType())
		{
		case kEnum:
		case kEquals:
			if (constant->getUnderKonceptOid() != 0)
				_ukOptionsMap[constant->getUnderKonceptOid()][constant->getProductElement()].push_back(constant);

			if (constant->getUnionAgreementOid() != 0)
				_uaOptionsMap[constant->getUnionAgreementOid()][constant->getProductElement()].push_back(constant);

			break;
		case kMin:
			if (constant->getUnderKonceptOid() != 0)
				_ukMinValuesMap[constant->getUnderKonceptOid()][constant->getProductElement()] = constant;

			if (constant->getUnionAgreementOid() != 0)
				_uaMinValuesMap[constant->getUnionAgreementOid()][constant->getProductElement()] = constant;

			break;
		case kMax:
			if (constant->getUnderKonceptOid() != 0)
				_ukMaxValuesMap[constant->getUnderKonceptOid()][constant->getProductElement()] = constant;

			if (constant->getUnionAgreementOid() != 0)
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
void RuleConstantContainer::initContext(const sbx::Subkoncept& subkoncept, sbx::UnionAgreementRelationship uar, sbx::unionagreement_oid uaOid)
{
	// since three maps containing pointers to Constants are created in the initialisation of the globalConstants, switching context is simply to just set the underKonceptOid and unionAgreementOid
	_subkoncept = subkoncept;
	_unionAgreementRelationship = uar;
	_unionAgreementOid = uaOid;
	_contextInitialised = true;
}

/**
 * Gets a vector of strings with option values for productElement
 */
std::vector<std::string> RuleConstantContainer::getOptions(unsigned short productElementOid)
{
	if (!_contextInitialised)
		throw domain_error("Context not initialised!");

	std::vector<std::shared_ptr<Constant>> constantList = this->getOptionsList(productElementOid);
	// create new vector of strings only
	std::vector<string> stringOptions(0);

	for (auto& constant : constantList)
		stringOptions.push_back(constant->stringValue());

	return stringOptions;
}

/**
 * Gets a vector of shared_ptr->Constant
 */
std::vector<std::shared_ptr<Constant>> RuleConstantContainer::getOptionsList(unsigned short productElementOid)
{
	if (!_contextInitialised)
		throw domain_error("Context not initialised!");

	subkoncept_oid ukOid = _subkoncept.getOid();

	if (_ukOptionsMap.find(ukOid) != _ukOptionsMap.cend()
			&& _ukOptionsMap.at(ukOid).find(productElementOid) != _ukOptionsMap.at(ukOid).cend())
		return _ukOptionsMap.at(ukOid).at(productElementOid);

	if (_subkoncept.getRelatedSubkonceptOid() != undefined_oid)
	{
		subkoncept_oid rukOid = _subkoncept.getRelatedSubkonceptOid();

		if (_ukOptionsMap.find(rukOid) != _ukOptionsMap.cend()
				&& _ukOptionsMap.at(rukOid).find(productElementOid) != _ukOptionsMap.at(rukOid).cend())
			return _ukOptionsMap.at(rukOid).at(productElementOid);
	}

	return {};
}


/**
 * Gets a shared_ptr to a single Constant for the productElement and comparisonType
 */
std::shared_ptr<sbx::Constant> RuleConstantContainer::getConstant(unsigned short productElementOid, const sbx::ComparisonTypes& comparisonType)
{
	if (!_contextInitialised)
		throw domain_error("Context not initialised!");

	subkoncept_oid ukOid = _subkoncept.getOid();

	switch (comparisonType)
	{
	case kMin:
		if (_ukMinValuesMap.find(ukOid) != _ukMinValuesMap.cend()
				&& _ukMinValuesMap.at(ukOid).find(productElementOid) != _ukMinValuesMap.at(ukOid).cend())
			return _ukMinValuesMap[ukOid][productElementOid];

		if (_subkoncept.getRelatedSubkonceptOid() != undefined_oid)
		{
			subkoncept_oid rukOid = _subkoncept.getRelatedSubkonceptOid();

			if (_ukMinValuesMap.find(rukOid) != _ukMinValuesMap.cend()
					&& _ukMinValuesMap.at(rukOid).find(productElementOid) != _ukMinValuesMap.at(rukOid).cend())
				return _ukMinValuesMap.at(rukOid).at(productElementOid);
		}
		break;
	case kMax:
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

std::shared_ptr<sbx::Constant> RuleConstantContainer::createConstant(unsigned short underkonceptOid, unsigned short unionAgreementOid, unsigned short peOid, sbx::ComparisonTypes comparisonType)
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

std::set<unsigned short> RuleConstantContainer::getProductOids(sbx::parameter_oid parameterOid) const
{
	if (_contextInitialised)
		throw domain_error("Context not initialised!");

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

std::set<unsigned short> RuleConstantContainer::getProductElementOids(sbx::parameter_oid parameterOid) const
{
	if (!_contextInitialised)
		throw domain_error("Context not initialised!");

	subkoncept_oid ukOid = _subkoncept.getOid();

	std::set<unsigned short> productElementOids{};

	if (_parameterOidToProductElementOids.find(ukOid) != _parameterOidToProductElementOids.cend()
			&& _parameterOidToProductElementOids.at(ukOid).find(parameterOid) != _parameterOidToProductElementOids.at(ukOid).cend())

		copy(	_parameterOidToProductElementOids.at(ukOid).at(parameterOid).cbegin(),
				_parameterOidToProductElementOids.at(ukOid).at(parameterOid).cend(),
				std::inserter(productElementOids, productElementOids.begin()));
	else
		if (RuleConstantContainer::_printErr)  { cerr << "No ProductElementOids found for subkoncept [" << ukOid << "], parameterOid[" << parameterOid << "]!"; }

	// add productelements for related subkoncept
	if (_subkoncept.getRelatedSubkonceptOid() != undefined_oid)
	{
		subkoncept_oid relatedUKOid = _subkoncept.getRelatedSubkonceptOid();

		if (_parameterOidToProductElementOids.find(relatedUKOid) != _parameterOidToProductElementOids.cend()
				&& _parameterOidToProductElementOids.at(relatedUKOid).find(parameterOid) != _parameterOidToProductElementOids.at(relatedUKOid).cend())
			copy(_parameterOidToProductElementOids.at(relatedUKOid).at(parameterOid).cbegin(),
					_parameterOidToProductElementOids.at(relatedUKOid).at(parameterOid).cend(),
					std::inserter(productElementOids, productElementOids.begin()));
		else
			if (RuleConstantContainer::_printErr)  { cerr << "No ProductElementOids found for related subkoncept [" << relatedUKOid << "], parameterOid[" << parameterOid << "]!"; }
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

sbx::ProductElement RuleConstantContainer::getProductElement(unsigned short productElementOid)
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
unsigned short RuleConstantContainer::getProductElementOid(const std::string& varName) const
{
	if (_varNameToPEOidMap.find(varName) != _varNameToPEOidMap.cend())
	{
		return _varNameToPEOidMap.at(varName);
	}

	return (unsigned short) kUnknownProductElement;
}

void RuleConstantContainer::initUAContributionSteps(const std::map<unsigned short, std::vector<sbx::ContributionStep>>& uaLadders)
{
	_uaContributionLadders = uaLadders;
}

std::shared_ptr<sbx::ContributionStep> RuleConstantContainer::getUAContributionStep(unsigned short uaOid)
{
	if (_uaContributionLadders.find(uaOid) != _uaContributionLadders.cend() && !_uaContributionLadders.at(uaOid).empty())
		return make_shared<sbx::ContributionStep>(_uaContributionLadders.at(uaOid).front());

	stringstream ss{};
	ss << uaOid;
	throw domain_error("Contribution ladder for Union Agreement [" + ss.str() + "] not initialised!");
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
void RuleConstantContainer::printConstants(sbx::subkoncept_oid subkonceptOid) const
{
	if (subkonceptOid == undefined_oid)
	{
		cout << "\n       ---======== Global Constants =======--- \n\n";
		printConstantHeader();

		for (const auto& c : _globalConstants)
			printConstant(make_shared<Constant>(c));
	}

	Subkoncept subkoncept = _subkoncepts.at(subkonceptOid);

	cout << "\n\nInitialised context: Underkoncept[" << _subkoncept.getOid() << "] / UnionAgreement[" << _unionAgreementOid << "]" << endl;
	cout << "\n\n       ---======== String Options =======--- \n";
	printConstantHeader();

	if (subkonceptOid == undefined_oid)
		for(const auto& ukit : _ukOptionsMap)
			for (const auto& peit : ukit.second)
				for (const auto& c : peit.second)
					printConstant(c);
	else
		for (const auto& peit : _ukOptionsMap.at(subkonceptOid))
			for (const auto& c : peit.second)
				printConstant(c);

	if (subkoncept.getRelatedSubkonceptOid() != undefined_oid)
	{
		cout << "      ----------Related subkoncept-------------" << endl;
		for (const auto& peit : _ukOptionsMap.at(subkoncept.getRelatedSubkonceptOid()))
			for (const auto& c : peit.second)
				printConstant(c);
	}

	cout << "\n\n       ---======== Min values Options =======--- \n\n";
	printConstantHeader();

	if (subkonceptOid == undefined_oid)
		for (const auto& ukit : _ukMinValuesMap)
			for (const auto& peit : ukit.second)
				printConstant(peit.second);
	else
		for (const auto& peit : _ukMinValuesMap.at(subkonceptOid))
			printConstant(peit.second);

	if (subkoncept.getRelatedSubkonceptOid() != undefined_oid)
	{
		cout << "      ----------Related subkoncept-------------" << endl;
		for (const auto& peit : _ukMinValuesMap.at(subkoncept.getRelatedSubkonceptOid()))
			printConstant(peit.second);
	}

	cout << "\n\n       ---======== Max values Options =======--- \n\n";
	printConstantHeader();

	if (subkonceptOid == undefined_oid)
		for (const auto& ukit : _ukMaxValuesMap)
			for (const auto& peit : ukit.second)
				printConstant(peit.second);
	else
		for (const auto& peit : _ukMaxValuesMap.at(subkonceptOid))
			printConstant(peit.second);

	if (subkoncept.getRelatedSubkonceptOid() != undefined_oid)
	{
		cout << "      ----------Related subkoncept-------------" << endl;
		for (const auto& peit : _ukMaxValuesMap.at(subkoncept.getRelatedSubkonceptOid()))
			printConstant(peit.second);
	}
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

void RuleConstantContainer::printConstants(sbx::subkoncept_oid underKonceptOid, sbx::productelement_oid productElement) const
{
	cout << "\n\n       ---======== String Options =======--- \n";

	Subkoncept subkoncept = _subkoncepts.at(underKonceptOid);

	if (_ukOptionsMap.find(underKonceptOid) != _ukOptionsMap.cend() && _ukOptionsMap.at(underKonceptOid).find(productElement) != _ukOptionsMap.at(underKonceptOid).end())
	{
		printConstantHeader();
		for (auto& c : _ukOptionsMap.at(underKonceptOid).at(productElement))
			printConstant(c);
	}
	else
		cout << "No Options values for UK[" << underKonceptOid << "], PE[" << (int) productElement << "]" << endl;

	if (subkoncept.getRelatedSubkonceptOid() != undefined_oid)
	{
		if (_ukOptionsMap.find(subkoncept.getRelatedSubkonceptOid()) != _ukOptionsMap.cend() && _ukOptionsMap.at(subkoncept.getRelatedSubkonceptOid()).find(productElement) != _ukOptionsMap.at(subkoncept.getRelatedSubkonceptOid()).end())
		{
			cout << "   ------------ Related koncept --------------" << endl;
			for (auto& c : _ukOptionsMap.at(subkoncept.getRelatedSubkonceptOid()).at(productElement))
				printConstant(c);
		}
	}

	cout << "\n\n       ---======== Min values Options =======--- \n";

	if (_ukMinValuesMap.find(underKonceptOid) != _ukMinValuesMap.cend() && _ukMinValuesMap.at(underKonceptOid).find(productElement) != _ukMinValuesMap.at(underKonceptOid).end())
	{
		printConstantHeader();
		auto& minC = _ukMinValuesMap.at(underKonceptOid).at(productElement);
		printConstant(minC);
	}
	else
		cout << "No Min constant for UK[" << underKonceptOid << "], PE[" << (int) productElement << "]" << endl;

	if (subkoncept.getRelatedSubkonceptOid() != undefined_oid)
	{
		if (_ukMinValuesMap.find(subkoncept.getRelatedSubkonceptOid()) != _ukMinValuesMap.cend() && _ukMinValuesMap.at(subkoncept.getRelatedSubkonceptOid()).find(productElement) != _ukMinValuesMap.at(subkoncept.getRelatedSubkonceptOid()).end())
		{
			cout << "   ------------ Related koncept --------------" << endl;
			auto& minC = _ukMinValuesMap.at(subkoncept.getRelatedSubkonceptOid()).at(productElement);
			printConstant(minC);
		}
	}

	cout << "\n\n       ---======== Max values Options =======--- \n";

	if (_ukMaxValuesMap.find(underKonceptOid) != _ukMaxValuesMap.cend() && _ukMaxValuesMap.at(underKonceptOid).find(productElement) != _ukMaxValuesMap.at(underKonceptOid).end())
	{
		printConstantHeader();
		auto& maxC = _ukMaxValuesMap.at(underKonceptOid).at(productElement);
		printConstant(maxC);
	}
	else
		cout << "No Max constant for UK[" << underKonceptOid << "], PE[" << (int) productElement << "]" << endl;

	if (subkoncept.getRelatedSubkonceptOid() != undefined_oid)
	{
		if (_ukMaxValuesMap.find(subkoncept.getRelatedSubkonceptOid()) != _ukMaxValuesMap.cend() && _ukMaxValuesMap.at(subkoncept.getRelatedSubkonceptOid()).find(productElement) != _ukMaxValuesMap.at(subkoncept.getRelatedSubkonceptOid()).end())
		{
			cout << "   ------------ Related koncept --------------" << endl;
			auto& minC = _ukMaxValuesMap.at(subkoncept.getRelatedSubkonceptOid()).at(productElement);
			printConstant(minC);
		}
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

void RuleConstantContainer::_printParameterList(const std::map<unsigned short, std::set<unsigned short>>& values) const
{
	for (auto& itemIt : values)
	{
		cout << setw(8) << itemIt.first;
		if (itemIt.second.size() > 0)
		{
			ostringstream s{};
			ostream_iterator<unsigned short> oit { s, ", " };
			copy(itemIt.second.cbegin(), itemIt.second.cend(), oit);
			cout << "  " << s.str();
		}
		cout << endl;
	}
}


} // sbx namespace end
