#ifndef RULEENGINE_PRODUCTELEMENTVALUE_H_
#define RULEENGINE_PRODUCTELEMENTVALUE_H_

#include <string>

#include "sbxTypes.h"

namespace sbx {

class ProductElementValue {
  private:
	sbx::ProductElementOid _productElementOid;
	sbx::ProductElementTypes _productElementType;
	std::string _stringValue;
  public:
	ProductElementValue(void);
	ProductElementValue(const sbx::ProductElementOid productElementOid, const sbx::ProductElementTypes productElementType, const std::string& stringValue);
	virtual ~ProductElementValue(void);
	std::string stringValue() const;
	long longValue() const;
	double doubleValue() const;
	sbx::ProductElementOid getProductElementOid() const;
	sbx::ProductElementTypes getProductElementType() const;
};

} // namespace sbx
#endif /* RULEENGINE_PRODUCTELEMENTVALUE_H_ */
