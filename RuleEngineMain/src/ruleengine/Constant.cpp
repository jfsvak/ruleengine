#include "Constant.h"

#include <iostream>
#include <stdlib.h>
using namespace std;

namespace sbx {

  bool Constant::_printDebug = false;

  Constant::Constant(void) :
		  _underKonceptOid { 0 },
		  _unionAgreementOid { 0 },
		  _stringValue { "" },
		  _default { false }
  {
	  // empty constructor needed for vector initialisation.
  }

  /**
   * Only constructor, needs all values
   */
  Constant::Constant( const short underKonceptOid,
		  	  	  	  const short unionAgreementOid,
					  const ProductElementNames productElement,
					  const ComparisonTypes comparisonType,
					  const std::string value,
					  const bool isDefault) :
							  _underKonceptOid { underKonceptOid },
							  _unionAgreementOid { unionAgreementOid },
							  _productElement { productElement },
							  _comparisonType { comparisonType },
							  _stringValue { value },
							  _default { isDefault }
  {
	  if (Constant::_printDebug) {
		  cout << "Constant{" << this << ", " << underKonceptOid << ", " << unionAgreementOid << ", " << productElement << ", " << comparisonType << ", " << value << ", " << isDefault << "}" << endl;
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
	  const char *carr = &_stringValue[0];
	  long longValue = atol(carr);
	  delete carr;
	  return longValue;
  }

  /**
   * Gets the constant value as a double
   */
  double Constant::doubleValue() const
  {
	  // convert string into a double value
	  const char *carr = &_stringValue[0];
	  double doubleValue = atof(carr);
	  delete carr;
	  return doubleValue;
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

  ProductElementNames Constant::getProductElement() const
  {
	  return _productElement;
  }

  ComparisonTypes Constant::getComparisonType() const
  {
	  return _comparisonType;
  }

  /**
   *
   */
  Constant::~Constant(void)
  {
	  if (Constant::_printDebug) {
		  cout << "Destructing ";
		  printValues();
	  }
	  _stringValue = "";
  }

  void Constant::printValues() const
  {
	  cout << "Constant{" << this << ", uk[" << _underKonceptOid << "], ua[" << _unionAgreementOid << "], pe[" << _productElement << "], ct[" << _comparisonType << "], ";
	  cout << "\"" << stringValue() << "\", double[" << doubleValue() << "], long[" << longValue() << "], " << boolalpha << _default << "} " << endl;
  }
} // namespace sbx
