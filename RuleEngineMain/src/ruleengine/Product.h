/*
 * Product.h
 *
 *  Created on: 06/05/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_PRODUCT_H_
#define RULEENGINE_PRODUCT_H_

#include <vector>

namespace sbx {

class Product {
public:
	static bool _printDebug;
	Product();
	Product(const sbx::Product&); // copy constructor
	Product(unsigned short oid, std::string& name);
	virtual ~Product();
	const std::string& getName() const;
	unsigned short getOid();
	const std::vector<unsigned short>& getProductElementOids() const;
	void addProductElementOid(unsigned short oid);
private:
	void printValues() const;
	int _oid;
	std::string _name;
	std::vector<unsigned short> _productElementOids;
};

}
#endif /* RULEENGINE_PRODUCT_H_ */
