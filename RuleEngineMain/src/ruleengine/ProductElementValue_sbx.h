#ifndef RULEENGINE_PRODUCTELEMENTVALUE_H_
#define RULEENGINE_PRODUCTELEMENTVALUE_H_

#include <string>

#include "ProductElement_sbx.h"
#include "sbxTypes.h"

namespace sbx {

class ProductElementValue {
  public:
	ProductElementValue(void);
	ProductElementValue(const sbx::ProductElementValue& otherProductElementValue); // copy constructor
	ProductElementValue(unsigned short peOid, const std::string& stringValue);
	ProductElementValue(const sbx::ProductElementOid& peOid, const std::string& stringValue);

	std::string stringValue() const;
	long longValue() const;
	double doubleValue() const;
	unsigned short getProductElementOid() const;
	void setValue(const std::string& value);
	virtual ~ProductElementValue(void);
  private:
	unsigned short _productElementOid;
	std::string _stringValue;
};

} // namespace sbx
#endif /* RULEENGINE_PRODUCTELEMENTVALUE_H_ */
