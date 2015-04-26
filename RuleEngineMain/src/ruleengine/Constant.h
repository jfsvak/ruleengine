#ifndef RULEENGINE_CONSTANT_H_
#define RULEENGINE_CONSTANT_H_

#include <string>

#include "sbxTypes.h"

namespace sbx {

class Constant {
  private:
	short _underKonceptOid;
	short _unionAgreementOid;
	ProductElementOid _productElement;
	ComparisonTypes _comparisonType; //comparison type
	std::string _stringValue;
	bool _default;
  public:
	static bool _printDebug;
	Constant(void);
	Constant(const Constant&); // copy constructor, overloaded to provide cout printing for debug
	Constant(const std::shared_ptr<Constant>&); // copy constructor for shared_ptr, overloaded to provide cout printing for debug
	Constant(const short underkonceptOid, const short unionAgreementOid, const ProductElementOid productElement, const ComparisonTypes comparisonType, const std::string value, const bool isDefault = false);
	virtual ~Constant(void);
	std::string stringValue() const;
	long longValue() const;
	double doubleValue() const;
	short getUnderKonceptOid() const;
	short getUnionAgreementOid() const;
	ProductElementOid getProductElement() const;
	ComparisonTypes getComparisonType() const;
	bool isDefault() const;
	void printValues() const;
};

} // namespace sbx
#endif /* RULEENGINE_CONSTANT_H_ */
