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
	Product();
	Product(const sbx::Product&); // copy constructor
	Product(short oid, std::string& name);
	virtual ~Product();
	const std::string& getName() const;
	short getOid();
	const std::vector<short>& getProductElementOids() const;
	void addProductElementOid(short oid);
private:
	short _oid;
	std::string _name;
	std::vector<short> _productElementOids;
};

}
#endif /* RULEENGINE_PRODUCT_H_ */
