#ifndef RULEENGINE_PRODUCTELEMENT_H_
#define RULEENGINE_PRODUCTELEMENT_H_

#include <string>

#include "sbxTypes.h"

namespace sbx {

class ProductElement
{
public:
	ProductElement(void);
	ProductElement(sbx::ProductElementOid productElementName, std::string variableName, sbx::ProductElementTypes productElementType);
	short getProductOid();
	virtual ~ProductElement(void);
private:
	sbx::ProductElementOid _productElement;
	std::string _variableName;
	sbx::ProductElementTypes _elementType;
	short _productOid;
};

} // sbx namespace

#endif /* RULEENGINE_PRODUCTELEMENT_H_ */
