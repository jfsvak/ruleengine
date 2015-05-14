#include <string>
#include <iostream>
#include <memory>

#include "sbxTypes.h"
#include "ProductElement_sbx.h"

using namespace std;

namespace sbx {
bool ProductElement::_printDebug = false;

ProductElement::ProductElement(void) :
		_oid { 0 }, _variableName { "" }, _guiName { "" }, _elementType { sbx::ProductElementTypes::kText }, _productOid { 0 }
{

}

ProductElement::ProductElement(const sbx::ProductElement& otherProductElement) :
		_oid { otherProductElement._oid }, _variableName { otherProductElement._variableName }, _guiName { otherProductElement._guiName }, _elementType {
				otherProductElement._elementType }, _productOid { otherProductElement._productOid }
{
	if (ProductElement::_printDebug)
	{
		cout << "= Ori.ProductElement{" << addressof(otherProductElement) << "}=>";
		printValues();
	}

}

ProductElement::ProductElement(const std::shared_ptr<sbx::ProductElement>& otherProductElement) :
		_oid { otherProductElement->_oid }, _variableName { otherProductElement->_variableName }, _guiName { otherProductElement->_guiName }, _elementType {
				otherProductElement->_elementType }, _productOid { otherProductElement->_productOid }
{
	if (ProductElement::_printDebug)
	{
		cout << "= Ori.Shared_ptrProductElement{" << addressof(*otherProductElement) << "}=>";
		printValues();
	}
}

ProductElement::ProductElement(unsigned short oid, const std::string& variableName, const std::string& guiName, sbx::ProductElementTypes productElementType,
		unsigned short productOid) :
		_oid { oid }, _variableName { variableName }, _guiName { guiName }, _elementType { productElementType }, _productOid { productOid }
{
	if (ProductElement::_printDebug)
	{
		cout << "+ ";
		printValues(); //Constant{" << this << ", " << underKonceptOid << ", " << unionAgreementOid << ", " << productElement << ", " << comparisonType << ", " << value << ", " << isDefault << "}" << endl;
	}
}

unsigned short ProductElement::getProductOid()
{
	return _productOid;
}

const sbx::ProductElementTypes& ProductElement::getElementType() const
{
	return _elementType;
}

const std::string& ProductElement::getGuiName() const
{
	return _guiName;
}

unsigned short ProductElement::getOid() const
{
	return _oid;
}

const std::string& ProductElement::getVariableName() const
{
	return _variableName;
}

void ProductElement::printValues() const
{
	cout << "ProductElement{" << this << ", oid[" << _oid << "], variableName[" << _variableName << "], guiName[" << _guiName << "], elementType["
			<< (int) _elementType << "], productOid[" << _productOid << "]" << endl;
}

ProductElement::~ProductElement(void)
{
	if (ProductElement::_printDebug)
	{
		cout << "- ";
		printValues();
	}

}

} // sbx namespace
