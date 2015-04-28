/*
 * RuleConstantContainer.cpp
 *
 *  Created on: 20/03/2015
 *      Author: jfsvak
 */

#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>

#include "RuleConstantContainer.h"
#include "sbxTypes.h"
#include "Constant.h"
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
            Json::Value ruleConstantList = root["data"].get("ruleConstant", 0);
            
            if (ruleConstantList.size() > 0) {
                size_t index = 0;
                // new vector of constants to passed to RuleConstantContainer. Set size to the number of incoming json elements
                _globalConstants.resize(ruleConstantList.size());
                
                if (RuleConstantContainer::_printDebug) {
                    cout << "  Looping over [" << ruleConstantList.size() << "] json rule constants to create" << endl;
                }
                
                // iterate over the list of rule constants and create Constant objects to put into the RuleConstantContainer
                for ( Json::ValueIterator ruleConstantElement = ruleConstantList.begin(); ruleConstantElement != ruleConstantList.end(); ++ruleConstantElement ) {
                    int productElementOid = ruleConstantElement->get("productElementOid", 0).asInt();
                    int underKonceptOid = ruleConstantElement->get("underKonceptOid", 0).asInt();
                    int unionAgreementOid = ruleConstantElement->get("unionAgreementOid", 0).asInt();
                    //				  int generalAgreementOid = ruleConstantElement->get("generalAgreementOid", 0).asInt();
                    int comparisonTypeOid = ruleConstantElement->get("comparisonTypeOid", 0).asInt();
                    string value = ruleConstantElement->get("value", "").asString();
                    bool isDefault = ruleConstantElement->get("isDefault", false).asBool();
                    
                    shared_ptr<Constant> constantPtr = make_shared<Constant> (static_cast<short int>(underKonceptOid), static_cast<short int>(unionAgreementOid), static_cast<ProductElementOid>(productElementOid), static_cast<ComparisonTypes>(comparisonTypeOid), value, isDefault);
                    
                    _globalConstants[index++] = constantPtr;
                }
                
                // initialise the container with the created vector
                
                if (RuleConstantContainer::_printDebug) {
                    cout << "  Initialising _container" << endl;
                }
                
                _initInternalMaps();
            }
            else {
                cout << "No rule constants found to load" << endl;
            }
        } else {
            cout << "Could not parse json" << endl;
        }
        
        //	  loadRuleCatalogue();
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
            std::vector<string> stringOptions (constantList.size());
            
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
    std::vector<std::shared_ptr<Constant>> RuleConstantContainer::getOptionsList(const sbx::ProductElementOid productElement)
    {
        if (_contextInitialised) {
            return _ukOptionsMap[_underKonceptOid][productElement];
        }
        
        throw domain_error("Context not initialised!");
    }
    
    /**
     * Gets a shared_ptr to a single Constant for the productElement and comparisonType
     */
    std::shared_ptr<sbx::Constant> RuleConstantContainer::getConstant(const sbx::ProductElementOid productElement, const sbx::ComparisonTypes comparisonType)
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
                const auto c = peit.second;
                printConstant(c);
            }
        }
        cout << "\n\n       ---======== Max values Options =======--- \n\n";
        printConstantHeader();
        
        for (const auto& ukit : _ukMaxValuesMap)
        {
            for (const auto& peit : ukit.second)
            {
                const auto c = peit.second;
                printConstant(c);
            }
        }
    }
    
    void RuleConstantContainer::printConstantHeader() const
    {
        cout << "Address   UK  UA  PE  CT    strValue     double       long  isDefault" << endl;
        cout << "--------  --  --  --  --  ----------  ---------  ---------  ---------" << endl;
    }
    
    void RuleConstantContainer::printConstant(const std::shared_ptr<sbx::Constant> c) const
    {
        cout << c << setw(4) << c->getUnderKonceptOid() << setw(4) << c->getUnionAgreementOid() << setw(4) << c->getProductElement() << setw(4) << c->getComparisonType();
        cout << right << setw(12) << c->stringValue() << right << setw(11) << c->doubleValue() << right << setw(11) << c->longValue() << setw(9) << (c->isDefault() ? "*" : "") << endl;
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
    
} // sbx namespace end
