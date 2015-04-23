#include <string>

#include "sbxTypes.h"
#include "ProductElement.h"

using namespace std;

namespace sbx {

ProductElement::ProductElement(void)
{
};

ProductElement::ProductElement(ProductElementOid productElement, string variableName, ProductElementTypes productElementType) :
		_productElement {productElement},
		_variableName {variableName},
		_elementType {productElementType}
{
};

ProductElement::~ProductElement(void)
{

}

} // sbx namespace
