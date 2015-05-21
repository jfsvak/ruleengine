/*
 * RuleConstantContainer.cpp
 *
 *  Created on: 20/03/2015
 *      Author: jfsvak
 */

#include <iostream>
#include <iomanip>
#include <limits>
#include <iterator>
#include <memory>
#include <vector>
#include <string>
#include <sstream>

#include "RuleConstantContainer_sbx.h"
#include "RuleEngine_sbx.h"
#include "sbxTypes.h"
#include "Constant_sbx.h"
#include "Product_sbx.h"
#include "../json/json.h"
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
	Json::Reader reader;
	Json::Value root;

	bool parsingSuccessful = reader.parse(jsonContents, root);

	if (parsingSuccessful)
	{
		// if we can parse the json string, then get the list of rule constants
		_initRuleConstants(root["data"].get("ruleConstant", 0));
		_initProducts(root["data"].get("product", 0));
		_initParameters(root["data"].get("parameter", 0));
		_initParametersToProducts(root["data"].get("parameterProduct", 0));
	}
	else
	{
		if (RuleConstantContainer::_printErr)
			cout << "Could not parse json string" << endl;
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
		if (RuleConstantContainer::_printDebug) {
			cout << "  Initialising _container" << endl;
		}

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
void RuleConstantContainer::_initProducts(const Json::Value& products)
{

	if (products.size() > 0)
	{
		if (RuleConstantContainer::_printDebug) {
			cout << "  Looping over [" << products.size() << "] json products to create" << endl;
		}

		// iterate over the list of products and create shared_ptr Product objects to put into the RuleConstantContainer
		for (Json::ValueIterator jsonProduct = products.begin(); jsonProduct != products.end(); ++jsonProduct)
		{
			int productOid = jsonProduct->get("oid", 0).asInt();
			string name = jsonProduct->get("name", "").asString();

			shared_ptr<Product> productPtr = make_shared<sbx::Product>(static_cast<unsigned short>(productOid), name);

			Json::Value productElements = jsonProduct->get("productElement", 0);

			if (productElements.size() > 0)
			{
				for (Json::ValueIterator jsonPEIterator = productElements.begin(); jsonPEIterator != productElements.end(); ++jsonPEIterator)
				{
					int peOid = jsonPEIterator->get("oid", 0).asInt();
					string name = jsonPEIterator->get("variableName", "").asString();
					string guiName = jsonPEIterator->get("guiName", "").asString();
					int elementType = jsonPEIterator->get("elementTypeOid", 0).asInt();

					productPtr->addProductElementOid(peOid);
					shared_ptr<ProductElement> pePtr = make_shared<sbx::ProductElement>(static_cast<unsigned short>(peOid), name, guiName, static_cast<sbx::ProductElementTypes>(elementType), static_cast<unsigned short>(productOid));

					_productElementMap[peOid] = pePtr;
				}
			}

			_productsMap[productOid] = productPtr;
		}
	}
	else
	{
		if (RuleConstantContainer::_printErr) cerr << "No Products found to load" << endl;
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
	if (!_contextInitialised) {
		throw domain_error("Context not initialised!");
	}

	switch(ct)
	{
	case kMin: 		return (_ukMinValuesMap.find(_underKonceptOid) != _ukMinValuesMap.cend() && _ukMinValuesMap.at(_underKonceptOid).find(peOid) != _ukMinValuesMap.at(_underKonceptOid).cend());
	case kMax: 		return (_ukMaxValuesMap.find(_underKonceptOid) != _ukMaxValuesMap.cend() && _ukMaxValuesMap.at(_underKonceptOid).find(peOid) != _ukMaxValuesMap.at(_underKonceptOid).cend());
	case kEnum:		// fall through
	case kEquals:	return (_ukOptionsMap.find(_underKonceptOid) != _ukOptionsMap.cend() && _ukOptionsMap.at(_underKonceptOid).find(peOid) != _ukOptionsMap.at(_underKonceptOid).cend());
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
//			bool displayOnTA = jsonParameterToProduct->get("displayOnTA", false).asBool();

			// add the parameterOid -> productOid relation for that underkoncept to the vector of productOids
			// that contains many productOids to one parameterOid for one underkonceptOid
			_parameterOidToProductOids[underkonceptOid][parameterOid].insert(productOid);

			// if product found in product map, add all the product elements to the list of allowed product elements for this parameterOid
			if (_productsMap.find(productOid) != _productsMap.cend())
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
			_ukOptionsMap[constant->getUnderKonceptOid()][constant->getProductElement()].push_back(constant);
			break;
		case kMin:
			_ukMinValuesMap[constant->getUnderKonceptOid()][constant->getProductElement()] = constant;
			break;
		case kMax:
			_ukMaxValuesMap[constant->getUnderKonceptOid()][constant->getProductElement()] = constant;
			break;
		default:
			break;
		}
	}
}

/**
 * Initialises the local, current context for this RuleConstantContainer
 */
void RuleConstantContainer::initContext(const unsigned short underKonceptOid, const short unionAgreementOid)
{
	// since three maps containing pointers to Constants are created in the initialisation of the globalConstants, switching context is simply to just set the underKonceptOid and unionAgreementOid
	_underKonceptOid = underKonceptOid;
	_unionAgreementOid = unionAgreementOid;
	_contextInitialised = true;
}

/**
 * Gets a vector of strings with option values for productElement
 */
std::vector<std::string> RuleConstantContainer::getOptions(unsigned short productElementOid)
{
	if (!_contextInitialised)
	{
		throw domain_error("Context not initialised!");
	}

	std::vector<std::shared_ptr<Constant>> constantList = _ukOptionsMap[_underKonceptOid][(productElementOid)];
	// create new vector of strings only
	std::vector<string> stringOptions(0);

	for (auto& constant : constantList)
	{
		stringOptions.push_back(constant->stringValue());
	}

	return stringOptions;
}

/**
 * Gets a vector of shared_ptr->Constant
 */
const std::vector<std::shared_ptr<Constant>>& RuleConstantContainer::getOptionsList(unsigned short  productElementOid)
{
	if (!_contextInitialised)
	{
		throw domain_error("Context not initialised!");
	}

	return _ukOptionsMap[_underKonceptOid][productElementOid];
}


/**
 * Gets a shared_ptr to a single Constant for the productElement and comparisonType
 */
std::shared_ptr<sbx::Constant> RuleConstantContainer::getConstant(unsigned short productElementOid, const sbx::ComparisonTypes& comparisonType)
{
	if (!_contextInitialised)
	{
		throw domain_error("Context not initialised!");
	}

	switch (comparisonType)
	{
	case kMin:
		if (_ukMinValuesMap.find(_underKonceptOid) != _ukMinValuesMap.cend()
				&& _ukMinValuesMap.at(_underKonceptOid).find(productElementOid) != _ukMinValuesMap.at(_underKonceptOid).cend())
		{
			return _ukMinValuesMap[_underKonceptOid][productElementOid];
		}
		break;
	case kMax:
		if (_ukMaxValuesMap.find(_underKonceptOid) != _ukMaxValuesMap.cend()
				&& _ukMaxValuesMap.at(_underKonceptOid).find(productElementOid) != _ukMaxValuesMap.at(_underKonceptOid).cend())
		{
			return _ukMaxValuesMap[_underKonceptOid][productElementOid];
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

const std::set<unsigned short>& RuleConstantContainer::getProductOids(unsigned short parameterOid) const
{
	if (_contextInitialised)
	{
		throw domain_error("Context not initialised!");
	}

	if (_parameterOidToProductOids.find(_underKonceptOid) != _parameterOidToProductOids.cend()
			&& _parameterOidToProductOids.at(_underKonceptOid).find(parameterOid) != _parameterOidToProductOids.at(_underKonceptOid).cend())
	{
		return _parameterOidToProductOids.at(_underKonceptOid).at(parameterOid);
	}

	throw domain_error("Product oids not found!");
}

std::set<unsigned short> RuleConstantContainer::getProductElementOids(unsigned short parameterOid) const
{

	if (!_contextInitialised)
	{
		throw domain_error("Context not initialised!");
	}

	if (_parameterOidToProductElementOids.find(_underKonceptOid) != _parameterOidToProductElementOids.cend()
			&& _parameterOidToProductElementOids.at(_underKonceptOid).find(parameterOid) != _parameterOidToProductElementOids.at(_underKonceptOid).cend())
	{
		return _parameterOidToProductElementOids.at(_underKonceptOid).at(parameterOid);
	}

	if (RuleConstantContainer::_printErr)  { cerr << "Could not find ProductElementOids for parameterOid[" << parameterOid << "], returning empty set!"; }

	// TODO: FIX THIS
	std::set<unsigned short> empty;
	return empty;
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
	{
		return _productElementMap.at(productElementOid);
	}

	std::stringstream s {};
	s << productElementOid;
	throw domain_error("ProductElement [" + s.str() + "] not found!");
}

/**
 * Outputs to cout the entire content of Constant Container
 */
void RuleConstantContainer::printConstants() const
{
	cout << "\n       ---======== Global Constants =======--- \n\n";
	printConstantHeader();

	for (const auto& c : _globalConstants)
	{
		printConstant(make_shared<Constant>(c));
	}

	cout << "\n\nInitialised context: Underkoncept[" << _underKonceptOid << "] / UnionAgreement[" << _unionAgreementOid << "]" << endl;
	cout << "\n\n       ---======== String Options =======--- \n";
	printConstantHeader();

	for (const auto& ukit : _ukOptionsMap)
	{

		for (const auto& peit : ukit.second)
		{
			for (const auto& c : peit.second)
			{
				printConstant(c);
			}
		}
	}
	cout << "\n\n       ---======== Min values Options =======--- \n\n";
	printConstantHeader();

	for (const auto& ukit : _ukMinValuesMap)
	{
		for (const auto& peit : ukit.second)
		{
			const auto& c = peit.second;
			printConstant(c);
		}
	}
	cout << "\n\n       ---======== Max values Options =======--- \n\n";
	printConstantHeader();

	for (const auto& ukit : _ukMaxValuesMap)
	{
		for (const auto& peit : ukit.second)
		{
			const auto& c = peit.second;
			printConstant(c);
		}
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

void RuleConstantContainer::printContainerOverview(unsigned short int underKonceptOid) const
{
	cout << "Current context : UK[" << _underKonceptOid << "], UA[" << _unionAgreementOid << "]" << endl;
	cout << "Global constants: [" << size() << "]" << endl;
	cout << "Products: [" << _productsMap.size() << "]" << endl;
	cout << "ProductsElements: [" << _productElementMap.size() << "]" << endl;
	cout << "Parameters: [" << _parameterMap.size() << "]" << endl;

	// print stats for all underkoncepts
	cout << "UK\tEquals\tMin\tMax\tParameters\n";
	if (underKonceptOid == 0)
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

void RuleConstantContainer::printContainerOverview(unsigned short int underKonceptOid, sbx::ComparisonTypes type) const
{
	cout << "Current context : Underkoncept[" << _underKonceptOid << "], UnionAgreement[" << _unionAgreementOid << "]" << endl;
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

void RuleConstantContainer::printConstants(unsigned short int underKonceptOid, unsigned short productElement) const
{
	cout << "\n\n       ---======== String Options =======--- \n";

	if (_ukOptionsMap.find(underKonceptOid) != _ukOptionsMap.cend() && _ukOptionsMap.at(underKonceptOid).find(productElement) != _ukOptionsMap.at(underKonceptOid).end())
	{
		printConstantHeader();
		for (auto& c : _ukOptionsMap.at(underKonceptOid).at(productElement))
		{
			printConstant(c);
		}
	}
	else
	{
		cout << "No Options values for UK[" << underKonceptOid << "], PE[" << (int) productElement << "]" << endl;
	}

	cout << "\n\n       ---======== Min values Options =======--- \n";

	if (_ukMinValuesMap.find(underKonceptOid) != _ukMinValuesMap.cend() && _ukMinValuesMap.at(underKonceptOid).find(productElement) != _ukMinValuesMap.at(underKonceptOid).end())
	{
		printConstantHeader();
		auto& minC = _ukMinValuesMap.at(underKonceptOid).at(productElement);
		printConstant(minC);
	}
	else
	{
		cout << "No Min constant for UK[" << underKonceptOid << "], PE[" << (int) productElement << "]" << endl;
	}

	cout << "\n\n       ---======== Max values Options =======--- \n";

	if (_ukMaxValuesMap.find(underKonceptOid) != _ukMaxValuesMap.cend() && _ukMaxValuesMap.at(underKonceptOid).find(productElement) != _ukMaxValuesMap.at(underKonceptOid).end())
	{
		printConstantHeader();
		auto& maxC = _ukMaxValuesMap.at(underKonceptOid).at(productElement);
		printConstant(maxC);
	}
	else
	{
		cout << "No Max constant for UK[" << underKonceptOid << "], PE[" << (int) productElement << "]" << endl;
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
			ostringstream s;
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
			cout << setw(5) << parameter.first;
			cout << "  " << setw(3) << parameter.second.getOid();
			cout << "  " << setw(52) << parameter.second.getName().substr(0, 50);
			cout << "  " << setw(11) << parameter.second.getType();
			cout << "  " << setw(10) << (int) parameter.second.getElementType();
			cout << endl;
		}
	}
	else
	{
		cout << "No Parameters loaded" << endl;
	}
}

void RuleConstantContainer::printParametersToProducts(unsigned short underkonceptOid) const
		{
	cout << "\n\n       ---======== Parameter To Products loaded =======--- \n";
	cout << "Underkoncept oid: " << underkonceptOid << endl;
	cout << "Parameter Oid  Product oids" << endl;
	cout << "-------------  -------------------------------------------------------" << endl;

	if (_parameterOidToProductOids.find(underkonceptOid) != _parameterOidToProductOids.end())
	{

		for (auto& parameterToProduct : _parameterOidToProductOids.at(underkonceptOid))
		{
			cout << setw(13) << parameterToProduct.first;
			if (parameterToProduct.second.size() > 0)
			{
				ostringstream s;
				ostream_iterator<unsigned short> oit { s, ", " };
				copy(parameterToProduct.second.cbegin(), parameterToProduct.second.cend(), oit);
				cout << "  " << s.str();
			}
			cout << endl;
		}
	}
	else
	{
		cout << "No Parameters To Products loaded" << endl;
	}
}

} // sbx namespace end
