#include <iostream>
#include <stdlib.h>

#include "ProductElementValue.h"

using namespace std;

namespace sbx {

ProductElementValue::ProductElementValue(void)
		: _productElementOid { 0 }, _stringValue { "" }
{
	// empty constructor needed for vector initialisation.
}
ProductElementValue::ProductElementValue(const sbx::ProductElementValue& otherProductElementValue)
		: _productElementOid { static_cast<unsigned short>(otherProductElementValue._productElementOid) }, _stringValue { otherProductElementValue._stringValue }
{

}

ProductElementValue::ProductElementValue(unsigned short peOid, const std::string& stringValue)
		: _productElementOid { peOid }, _stringValue { stringValue }
{
}

ProductElementValue::ProductElementValue(const sbx::ProductElementOid& peOid, const std::string& stringValue)
		: _productElementOid { static_cast<unsigned short>(peOid) }, _stringValue { stringValue }
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

unsigned short ProductElementValue::getProductElementOid() const
{
	return _productElementOid;
}

/**
 *
 */
ProductElementValue::~ProductElementValue(void)
{
	_stringValue = "";
}
} // namespace sbx
