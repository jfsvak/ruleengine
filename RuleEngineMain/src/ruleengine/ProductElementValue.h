#ifndef RULEENGINE_PRODUCTELEMENTVALUE_H_
#define RULEENGINE_PRODUCTELEMENTVALUE_H_

#include <string>

#include "sbxTypes.h"

namespace sbx {

class ProductElementValue {
  private:
	sbx::ProductElementNames _productElement;
	sbx::ProductElementTypes _productElementType;
	std::string _stringValue;
  public:
	ProductElementValue(void);
	ProductElementValue(const sbx::ProductElementNames productElementNames, const sbx::ProductElementTypes productElementType, const std::string& stringValue);
	virtual ~ProductElementValue(void);
	std::string stringValue() const;
	long longValue() const;
	double doubleValue() const;
	sbx::ProductElementNames getProductElement() const;
	sbx::ProductElementTypes getProductElementType() const;
};

} // namespace sbx
#endif /* RULEENGINE_PRODUCTELEMENTVALUE_H_ */
