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

using namespace std;

namespace sbx {
	bool RuleConstantContainer::_printDebug = false;

  /**
   * Copies the vector of Constants into the _globalConstants vector
   */
  void RuleConstantContainer::initGlobalConstants(const vector<Constant> &global_constants)
  {
	  // load the global constants into the container using copy-constructor
	  if (RuleConstantContainer::_printDebug)
		  cout << "  _globalConstants = global_constants" << endl;

	  _globalConstants = global_constants;

	  if (RuleConstantContainer::_printDebug)
		  cout << "  _globalConstants = global_constants done!" << endl;

	  // initialise the 3 maps of constant types
	  for ( auto& constant : _globalConstants) {

  		  // equals goes into _ukOptionsMap as shared_ptr to the Constant in a vector
		  // min/max values goes into min/max-map as a shared_ptr to the Constant
		  if (RuleConstantContainer::_printDebug)
			  cout << "  make_shared<Constant>(constant)!" << endl;

		  shared_ptr<Constant> ptr = make_shared<Constant>(constant);

		  if (RuleConstantContainer::_printDebug)
			  cout << "  make_shared<Constant>(constant) done!" << endl;

		  switch(constant.getComparisonType()) {
		  	  case kEquals:
//		  		  _optionsMap[constant.getProductElement()].push_back(constant.stringValue());
		  		  _ukOptionsMap[constant.getUnderKonceptOid()][constant.getProductElement()].push_back(ptr);
		  		  break;
		  	  case kMin:
			  	  _ukMinValuesMap[constant.getUnderKonceptOid()][constant.getProductElement()] = ptr;
		  		  break;
			  case kMax:
				  _ukMaxValuesMap[constant.getUnderKonceptOid()][constant.getProductElement()] = ptr;
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
  vector<string> RuleConstantContainer::getOptions(const sbx::ProductElementOid productElement)
  {
	  if (_contextInitialised) {
		  std::vector<std::shared_ptr<Constant>> constantList = _ukOptionsMap[_underKonceptOid][productElement];
		  // create new vector of strings only
		  std::vector<string> stringOptions { };

		  for (auto constant : constantList) {
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
			  cout << "\t" << right << setw(3) << _ukMinValuesMap.at(optionsMapItem.first).size();
			  cout << "\t" << right << setw(3) << _ukMaxValuesMap.at(optionsMapItem.first).size();
			  cout << endl;
		  }
	  }
	  else
	  {
		  // print stats for this underkoncept
		  cout << right << setw(2) << underKonceptOid;
		  cout << "\t" << right << setw(6) << _ukOptionsMap.at(underKonceptOid).size();
		  cout << "\t" << right << setw(3) << _ukMinValuesMap.at(underKonceptOid).size();
		  cout << "\t" << right << setw(3) << _ukMaxValuesMap.at(underKonceptOid).size();
		  cout << endl;
	  }
  }

} // sbx namespace end
