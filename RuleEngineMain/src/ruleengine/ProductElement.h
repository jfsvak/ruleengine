#ifndef RULEENGINE_PRODUCTELEMENT_H_
#define RULEENGINE_PRODUCTELEMENT_H_

#include <string>
#include <memory>

#include "sbxTypes.h"

namespace sbx {

class ProductElement
{
public:
	static bool _printDebug;
	ProductElement(void);
	ProductElement(const sbx::ProductElement& otherProductElement); // copy constructor
	ProductElement(const std::shared_ptr<sbx::ProductElement>& otherProductElement); // copy constructure for shared_ptr
	ProductElement(unsigned short oid, const std::string& variableName, const std::string& guiName, sbx::ProductElementTypes elementType, unsigned short productOid);
	unsigned short getOid() const;
	unsigned short getProductOid();
	const sbx::ProductElementTypes& getElementType() const;
	const std::string& getGuiName() const;
	const std::string& getVariableName() const;
	virtual ~ProductElement(void);
private:
	void printValues() const;
	unsigned short _oid;
	std::string _variableName;
	std::string _guiName;
	sbx::ProductElementTypes _elementType;
	unsigned short _productOid;
};

} // sbx namespace

#endif /* RULEENGINE_PRODUCTELEMENT_H_ */
