/*
 * RuleConstantContainer.cpp
 *
 *  Created on: 20/03/2015
 *      Author: jfsvak
 */

#include <iostream>
#include <iomanip>
#include <iterator>
#include <memory>
#include <vector>

#include "RuleConstantContainer.h"
#include "sbxTypes.h"
#include "Constant.h"
#include "Product.h"
#include "../json/json.h"

using namespace std;

namespace sbx {
    bool RuleConstantContainer::_printDebug = false;
    
    /**
     * Copies the vector of Constants into the _globalConstants vector
     */
    void RuleConstantContainer::initGlobalConstants(const vector<Constant>& global_constants)
    {
        // load the global constants into the container using copy-constructor
        if (RuleConstantContainer::_printDebug)
            cout << "  _globalConstants = global_constants" << endl;
        
        // set size in internal vector of global constants the the same size of the incoming constants
        _globalConstants.resize(global_constants.size());
        
        // run through the incoming list of constants and wrap each in a shared_ptr
        for ( size_t gcIndex = 0; gcIndex < global_constants.size(); gcIndex++ ) {
            _globalConstants[gcIndex] = make_shared<Constant>(global_constants[gcIndex]);
        }
        
        if (RuleConstantContainer::_printDebug)
            cout << "  _globalConstants = global_constants done!" << endl;
        
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

        if(parsingSuccessful) {
            // if we can parse the json string, then get the list of rule constants
        	_initRuleConstants(root["data"].get("ruleConstant", 0));
			_initProducts(root["data"].get("product", 0));
        } else {
            cout << "Could not parse json string" << endl;
            throw invalid_argument(reader.getFormattedErrorMessages());
        }

        //	  loadRuleCatalogue();
    }

	void RuleConstantContainer::_initRuleConstants(const Json::Value& ruleConstantList) {
		// if we can parse the json string, then get the list of rule constants
		if (ruleConstantList.size() > 0) {
			size_t index = 0;
			// new vector of constants to passed to RuleConstantContainer. Set size to the number of incoming json elements
			_globalConstants.resize(ruleConstantList.size());

			if (RuleConstantContainer::_printDebug) {
				cout << "  Looping over [" << ruleConstantList.size() << "] json rule constants to create" << endl;
			}
			// iterate over the list of rule constants and create Constant objects to put into the RuleConstantContainer
			for (Json::ValueIterator ruleConstantElement = ruleConstantList.begin(); ruleConstantElement != ruleConstantList.end(); ++ruleConstantElement) {
				int productElementOid = ruleConstantElement->get("productElementOid", 0).asInt();
				int underKonceptOid = ruleConstantElement->get("underKonceptOid", 0).asInt();
				int unionAgreementOid = ruleConstantElement->get("unionAgreementOid", 0).asInt();
				//				  int generalAgreementOid = ruleConstantElement->get("generalAgreementOid", 0).asInt();
				int comparisonTypeOid = ruleConstantElement->get( "comparisonTypeOid", 0).asInt();
				string value = ruleConstantElement->get("value", "").asString();
				bool isDefault = ruleConstantElement->get("isDefault", false).asBool();
				shared_ptr<Constant> constantPtr = make_shared<Constant>(
						static_cast<short int>(underKonceptOid),
						static_cast<short int>(unionAgreementOid),
						static_cast<ProductElementOid>(productElementOid),
						static_cast<ComparisonTypes>(comparisonTypeOid), value,
						isDefault);
				_globalConstants[index++] = constantPtr;
			}
			// initialise the container with the created vector
			if (RuleConstantContainer::_printDebug) {
				cout << "  Initialising _container" << endl;
			}

			_initInternalMaps();
		} else {
			cout << "No rule constants found to load" << endl;
		}
	}

	/**
	 * Initialise the internal map of products and product elements
	 */
	void RuleConstantContainer::_initProducts(const Json::Value& products) {

		if (products.size() > 0) {
			// new vector of constants to passed to RuleConstantContainer. Set size to the number of incoming json elements
			if (RuleConstantContainer::_printDebug) {
				cout << "  Looping over [" << products.size() << "] json rule products to create" << endl;
			}

			// iterate over the list of rule constants and create Constant objects to put into the RuleConstantContainer
			for (Json::ValueIterator jsonProduct = products.begin(); jsonProduct != products.end(); ++jsonProduct) {
				int oid = jsonProduct->get("oid", 0).asInt();
				string name = jsonProduct->get("name", "").asString();

				shared_ptr<Product> productPtr = make_shared<sbx::Product>(
						static_cast<short int>(oid),
						name);

				Json::Value productElements = jsonProduct->get("productElement", 0);

				if (productElements.size() > 0)
				{
					for (Json::ValueIterator jsonPEIterator = productElements.begin(); jsonPEIterator != productElements.end(); ++jsonPEIterator) {
						int peOid = jsonPEIterator->get("oid", 0).asInt();
						productPtr->addProductElementOid(peOid);
					}
				}

				_productsMap[oid] = productPtr;
			}
		} else {
			cout << "No Products found to load" << endl;
		}
	}

    /**
     Initialises the internal maps containing shared_ptr to the constants in the _globalConstants
     */
    void RuleConstantContainer::_initInternalMaps() {
        // initialise the 3 maps of constant types
        for ( const auto& constant : _globalConstants ) {
            
            // equals/enum goes into _ukOptionsMap as a shared_ptr to the Constant in a _globalConstants
            // min/max values goes into min/max-map as a shared_ptr to the Constant in the _globalConstants
            switch(constant->getComparisonType()) {
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
    void RuleConstantContainer::initContext(const short underKonceptOid, const short unionAgreementOid)
    {
        // since three maps containing pointers to Constants are created in the initialisation of the globalConstants, switching context is simply to just set the underKonceptOid and unionAgreementOid
        _underKonceptOid = underKonceptOid;
        _unionAgreementOid = unionAgreementOid;
        _contextInitialised = true;
    }
    
    /**
     * Gets a vector of strings with option values for productElement
     */
    std::vector<std::string> RuleConstantContainer::getOptions(const sbx::ProductElementOid productElementOid) {
        if (_contextInitialised) {
            std::vector<std::shared_ptr<Constant>> constantList = _ukOptionsMap[_underKonceptOid][productElementOid];
            // create new vector of strings only
            std::vector<string> stringOptions (0);
            
            for (auto& constant : constantList) {
                stringOptions.push_back(constant->stringValue());
            }
            
            return stringOptions;
        }
        
        throw domain_error("Context not initialised!");
    }
    
    /**
     * Gets a vector of shared_ptr->Constant
     */
    const std::vector<std::shared_ptr<Constant>>& RuleConstantContainer::getOptionsList(const sbx::ProductElementOid productElement)
    {
        if (_contextInitialised) {
            return _ukOptionsMap[_underKonceptOid][productElement];
        }
        
        throw domain_error("Context not initialised!");
    }
    
    /**
     * Gets the default value for a ProductElement
     */
//    const std::shared_ptr<Constant>& RuleConstantContainer::getDefaultValue(sbx::ProductElementOid productElement) {
//    	const std::vector<std::shared_ptr<Constant>>& constants = getOptionsList(productElement);
//
//    	if (constants.size() <= 0) {
//    		throw domain_error("No default value for product element oid [" + productElement + "]!");
//    	}
//
//    	for (const auto& constant : constants) {
//    		if (constant->isDefault()) {
//    			return constant;
//    		}
//    	}
//
//    	// if no default value is set, return the first in the list
//    	return constants.front();
//    }

    /**
     * Gets a shared_ptr to a single Constant for the productElement and comparisonType
     */
    const std::shared_ptr<sbx::Constant>& RuleConstantContainer::getConstant(const sbx::ProductElementOid productElement, const sbx::ComparisonTypes comparisonType)
    {
        if (_contextInitialised) {
            
            switch(comparisonType)
            {
                case kMin:
                    return _ukMinValuesMap[_underKonceptOid][productElement];
                    break;
                case kMax:
                    return _ukMaxValuesMap[_underKonceptOid][productElement];
                    break;
                default:
                    throw domain_error("Only ComparisonType (Min, Max) supported");
            }
        }
        
        throw domain_error("Context not initialised!");
    }
    
    /**
     * Gets the number of global constants in this container
     */
    std::size_t RuleConstantContainer::size() const
    {
        return _globalConstants.size();
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
        cout << "Address    UK  UA  PE   CT  strValue                                       double       long  isDefault" << endl;
        cout << "---------  --  --  ---  --  ------------------------------------------  ---------  ---------  ---------" << endl;
    }
    
    void RuleConstantContainer::printConstant(const std::shared_ptr<sbx::Constant>& c) const
    {
        cout << left << setw(9) << c;
        cout << "  " << right << setw(2) << c->getUnderKonceptOid();
        cout << "  " << right << setw(2) << c->getUnionAgreementOid();
        cout << "  " << right << setw(3) << c->getProductElement();
        cout << "  " << right << setw(2) << c->getComparisonType();
        cout << "  " << left << setw(40) << c->stringValue().substr(0, 40);
        cout << "  " << right << setw(11) << c->doubleValue();
        cout << "  " << right << setw(9) << c->longValue();
        cout << "  " << left << setw(7) << (c->isDefault() ? "*" : "") << endl;
    }
    
    void RuleConstantContainer::printContainerOverview(short int underKonceptOid) const {
        cout << "Current context : UK["<< _underKonceptOid << "], UA["<< _unionAgreementOid << "]" << endl;
        cout << "Global constants: [" << size() << "]" << endl;
        
        // print stats for all underkoncepts
        cout << "UK\tEquals\tMin\tMax\n";
        if (underKonceptOid == 0)
        {
            for ( const auto& optionsMapItem : _ukOptionsMap) {
                cout << right << setw(2) << optionsMapItem.first;
                cout << "\t" << right << setw(6) << optionsMapItem.second.size();
                cout << "\t" << right << setw(3) << (_ukMinValuesMap.find(optionsMapItem.first) != _ukMinValuesMap.end() ? _ukMinValuesMap.at(optionsMapItem.first).size() : 0);
                cout << "\t" << right << setw(3) << (_ukMaxValuesMap.find(optionsMapItem.first) != _ukMaxValuesMap.end() ? _ukMaxValuesMap.at(optionsMapItem.first).size() : 0);
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
            cout << endl;
        }
    }
    
    void RuleConstantContainer::printContainerOverview(short int underKonceptOid, sbx::ComparisonTypes type) const {
        cout << "Current context : Underkoncept["<< _underKonceptOid << "], UnionAgreement["<< _unionAgreementOid << "]" << endl;
        cout << "Showing context : Underkoncept["<< underKonceptOid << "], type["<< type << "]" << endl;
        size_t count;
        switch(type) {
            case kEquals:
                count = _ukOptionsMap.at(underKonceptOid).size();
                cout << "Constants for type: [" << static_cast<short int>(type) << "], [" << count << "]" << endl;
                cout << " PE\tValues" << endl;
                
                for ( auto& iterator : _ukOptionsMap.at(underKonceptOid)) {
                    cout << right << setw(3) << iterator.first << "\t";
                    
                    for ( auto& value : iterator.second ) {
                        cout << value->stringValue() << ", ";
                    }
                    cout << endl;
                }
                
                break;
            case kMin:
                count = _ukMinValuesMap.at(underKonceptOid).size();
                cout << "nothing yet" << endl;
                break;
            case kMax:
                count = _ukMaxValuesMap.at(underKonceptOid).size();
                cout << "nothing yet" << endl;
                break;
            default:
                cout << "nothing yet" << endl;
                break;
        }
    }
    
    void RuleConstantContainer::printConstants(short int underKonceptOid, sbx::ProductElementOid productElement) const {
    	cout << "\n\n       ---======== String Options =======--- \n";

		if (_ukOptionsMap.find(underKonceptOid) != _ukOptionsMap.cend()
				&& _ukOptionsMap.at(underKonceptOid).find(productElement) != _ukOptionsMap.at(underKonceptOid).end())
		{
			printConstantHeader();
			for (auto& c : _ukOptionsMap.at(underKonceptOid).at(productElement))
			{
				printConstant(c);
			}
    	} else {
			cout << "No Options values for UK[" << underKonceptOid << "], PE[" << productElement << "]" << endl;
		}

		cout << "\n\n       ---======== Min values Options =======--- \n";

		if (_ukMinValuesMap.find(underKonceptOid) != _ukMinValuesMap.cend()
				&& _ukMinValuesMap.at(underKonceptOid).find(productElement) != _ukMinValuesMap.at(underKonceptOid).end())
		{
			printConstantHeader();
			auto& minC = _ukMinValuesMap.at(underKonceptOid).at(productElement);
			printConstant(minC);
		} else {
			cout << "No Min constant for UK[" << underKonceptOid << "], PE[" << productElement << "]" << endl;
		}

		cout << "\n\n       ---======== Max values Options =======--- \n";

		if (_ukMaxValuesMap.find(underKonceptOid) != _ukMaxValuesMap.cend() &&
				 _ukMaxValuesMap.at(underKonceptOid).find(productElement) != _ukMaxValuesMap.at(underKonceptOid).end())
		{
			printConstantHeader();
			auto& maxC = _ukMaxValuesMap.at(underKonceptOid).at(productElement);
			printConstant(maxC);
		}
		else {
			cout << "No Max constant for UK[" << underKonceptOid << "], PE[" << productElement << "]" << endl;
		}
    }

    void RuleConstantContainer::printProducts() const {
    	cout << "\n\n       ---======== Products loaded =======--- \n";
    	cout << "Index  Oid  Name                                         Product Elements" << endl;
    	cout << "-----  ---  -------------------------------------------- ----------------" << endl;

    	for (auto& product : _productsMap)
    	{
    		cout << right << setw(5) << product.first;
    		cout << "  " << right << setw(3) << product.second->getOid();
    		cout << "  " << left << setw(43) << product.second->getName();

    		if (product.second->getProductElementOids().size() > 0) {
				ostringstream s;
				ostream_iterator<short> oit(s, ", ");
				copy(product.second->getProductElementOids().cbegin(), product.second->getProductElementOids().cend(), oit);
				cout << "  " << s.str();
    		}

    		cout << endl;
    	}
    }

} // sbx namespace end
