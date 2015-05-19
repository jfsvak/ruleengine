#ifndef RULEENGINE_CONSTANT_H_
#define RULEENGINE_CONSTANT_H_

#include <string>
#include <iostream>
#include <memory>

#include "sbxTypes.h"

namespace sbx {

class Constant {
//	friend std::ostream &operator<<( std::ostream &, const sbx::Constant &);

  public:
	static bool _printDebug;
	Constant(void);
	Constant(const Constant&); // copy constructor, overloaded to provide cout printing for debug
	Constant(const std::shared_ptr<Constant>&); // copy constructor for shared_ptr, overloaded to provide cout printing for debug
	Constant(short underkonceptOid, short unionAgreementOid, ProductElementOid productElement, ComparisonTypes comparisonType, const std::string& value, bool isDefault = false, bool isAutoCreated = false);
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
	bool isAutoCreated() const;
	void setAutoCreated(bool autoCreated);

  private:
	short _underKonceptOid;
	short _unionAgreementOid;
	ProductElementOid _productElement;
	ComparisonTypes _comparisonType; //comparison type
	std::string _stringValue;
	bool _default;
	bool _autoCreated;
};

} // namespace sbx
#endif /* RULEENGINE_CONSTANT_H_ */
