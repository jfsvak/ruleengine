#include "ProductElementValue.h"

#include <iostream>
#include <stdlib.h>
using namespace std;

namespace sbx {

  ProductElementValue::ProductElementValue(void) :
		  _productElement {},
		  _productElementType {},
		  _stringValue { "" }
  {
	  // empty constructor needed for vector initialisation.
  }

  ProductElementValue::ProductElementValue(const ProductElementOid productElement, const ProductElementTypes productElementType, const std::string& stringValue) :
		  _productElement { productElement},
		  _productElementType { productElementType },
		  _stringValue { stringValue }
  {
  }

  /**
   * Get the constant value as a string
   */
  std::string ProductElementValue::stringValue() const
  {
	  return _stringValue;
  }

  /**
   * Get the constant value as a long value
   */
  long ProductElementValue::longValue() const
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
  double ProductElementValue::doubleValue() const
  {
	  // convert string into a double value
	  const char *carr = &_stringValue[0];
	  double doubleValue = atof(carr);
	  delete carr;
	  return doubleValue;
  }

  ProductElementOid ProductElementValue::getProductElement() const
  {
	  return _productElement;
  }

  ProductElementTypes ProductElementValue::getProductElementType() const
  {
	  return _productElementType;
  }

  /**
   *
   */
  ProductElementValue::~ProductElementValue(void)
  {
	  _stringValue = "";
  }
} // namespace sbx
