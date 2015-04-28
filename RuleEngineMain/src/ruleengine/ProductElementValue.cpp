#include "ProductElementValue.h"

#include <iostream>
#include <stdlib.h>
using namespace std;

namespace sbx {

  ProductElementValue::ProductElementValue(void) :
		  _productElementOid {},
		  _productElementType {},
		  _stringValue { "" }
  {
	  // empty constructor needed for vector initialisation.
  }

  ProductElementValue::ProductElementValue(const ProductElementOid productElement, const ProductElementTypes productElementType, const std::string& stringValue) :
		  _productElementOid { productElement},
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
	  return atol(_stringValue.c_str());
  }

  /**
   * Gets the constant value as a double
   */
  double ProductElementValue::doubleValue() const
  {
	  // convert string into a double value
	  return atof(_stringValue.c_str());
  }

  ProductElementOid ProductElementValue::getProductElementOid() const
  {
	  return _productElementOid;
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
