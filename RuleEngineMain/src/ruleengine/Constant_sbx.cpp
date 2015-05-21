#include <iostream>
#include <memory>
#include <stdlib.h>

#include "Constant_sbx.h"
#include "sbxTypes.h"

using namespace std;

namespace sbx {

  bool Constant::_printDebug = false;

  Constant::Constant(void) :
		  _underKonceptOid { 0 },
		  _unionAgreementOid { 0 },
		  _productElement { sbx::ProductElementOid::kUnknownProductElement},
		  _comparisonType { sbx::ComparisonTypes::kUnknown },
		  _stringValue { "" },
		  _default { false },
		  _autoCreated { false }
  {
	  // empty constructor needed for vector initialisation.
  }

  Constant::Constant( const Constant& origin ) :
									  _underKonceptOid { origin._underKonceptOid },
									  _unionAgreementOid { origin._unionAgreementOid },
									  _productElement { origin._productElement },
									  _comparisonType { origin._comparisonType },
									  _stringValue { origin._stringValue },
									  _default { origin._default },
									  _autoCreated { origin._autoCreated }
  {
	  if (Constant::_printDebug) {
		  cout << "= Ori.Constant{"<< addressof(origin) << "}=>";
		  printValues();
	  }
  }

  Constant::Constant(const std::shared_ptr<Constant>& origin) :
										_underKonceptOid { origin->_underKonceptOid },
										_unionAgreementOid { origin->_unionAgreementOid },
										_productElement { origin->_productElement },
										_comparisonType { origin->_comparisonType },
										_stringValue { origin->_stringValue },
										_default { origin->_default },
										_autoCreated { origin->_autoCreated }
  {
	  if (Constant::_printDebug) {
		  cout << "= Ori.Shared_ptrConstant{"<< addressof(*origin) << "}=>";
		  printValues();
	  }
  }


  /**
   * Only constructor, needs all values
   */
  Constant::Constant( short underKonceptOid,
		  	  	  	  short unionAgreementOid,
					  ProductElementOid productElement,
					  ComparisonTypes comparisonType,
					  const std::string& value,
					  bool isDefault,
					  bool isAutoCreated) :
							  _underKonceptOid { underKonceptOid },
							  _unionAgreementOid { unionAgreementOid },
							  _productElement { productElement },
							  _comparisonType { comparisonType },
							  _stringValue { value },
							  _default { isDefault },
							  _autoCreated { isAutoCreated }
  {
	  if (Constant::_printDebug) {
		  cout << "+ ";
		  printValues(); //Constant{" << this << ", " << underKonceptOid << ", " << unionAgreementOid << ", " << productElement << ", " << comparisonType << ", " << value << ", " << isDefault << "}" << endl;
	  }
  }

  /**
   * Get the constant value as a string
   */
  std::string Constant::stringValue() const
  {
	  return _stringValue;
  }

  /**
   * Get the constant value as a long value
   */
  long Constant::longValue() const
  {
	  // convert string into a long value
	  return atol(_stringValue.c_str());
  }

  /**
   * Gets the constant value as a double
   */
  double Constant::doubleValue() const
  {
	  // convert string into a double value
      return atof(_stringValue.c_str());
  }

  /**
   * Gets the Constant value as a bool
   * "true" and "1" are considered true, everything else is false
   */
  bool Constant::boolValue() const
  {
	  return toBool(_stringValue);
  }

  short Constant::getUnderKonceptOid() const
  {
	  return _underKonceptOid;
  }

  bool Constant::isDefault() const
  {
	return _default;
  }

  short Constant::getUnionAgreementOid() const
  {
	  return _unionAgreementOid;
  }

  ProductElementOid Constant::getProductElement() const
  {
	  return _productElement;
  }

  ComparisonTypes Constant::getComparisonType() const
  {
	  return _comparisonType;
  }

  bool sbx::Constant::isAutoCreated() const
  {
  	return _autoCreated;
  }

  void sbx::Constant::setAutoCreated(bool autoCreated)
  {
  	_autoCreated = autoCreated;
  }

  void Constant::printValues() const
  {
	  cout << "Constant{" << this << ", uk[" << _underKonceptOid << "], ua[" << _unionAgreementOid << "], pe[" << (int)_productElement << "], ct[" << (int)_comparisonType << "], ";
	  cout << "\"" << stringValue() << "\", double[" << doubleValue() << "], long[" << longValue() << "], default[" << boolalpha << _default << "], autoCreated[" << boolalpha << _autoCreated << "] }" << endl;
  }

  /**
   *
   */
  Constant::~Constant(void)
  {
	  if (Constant::_printDebug) {
		  cout << "- ";
		  printValues();
	  }
	  _stringValue = "";
  }

//  std::ostream &operator<<( std::ostream &output, const sbx::Constant &c)
//  {
//	  output << "Constant{" << addressof(c) << ", uk[" << c._underKonceptOid << "], ua[" << c._unionAgreementOid << "], pe[" << (int)c._productElement << "], ct[" << (int)c._comparisonType << "], ";
//	  output << "\"" << c.stringValue() << "\", double[" << c.doubleValue() << "], long[" << c.longValue() << "], " << boolalpha << c._default << "} " << endl;
//      return output;
//  }

}// namespace sbx
