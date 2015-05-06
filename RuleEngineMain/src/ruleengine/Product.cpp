/*
 * Product.cpp
 *
 *  Created on: 06/05/2015
 *      Author: jfsvak
 */

#include <string>

#include "Product.h"

namespace sbx {

Product::~Product() {
}

Product::Product(short oid, std::string& name) : _oid {oid}, _name {name} {
}

Product::Product(const sbx::Product& otherProduct) : _oid {otherProduct._oid}, _name {otherProduct._name} {
}

const std::string& Product::getName() const {
	return _name;
}

short int Product::getOid() {
	return _oid;
}

const std::vector<short>& Product::getProductElementOids() const {
	return _productElementOids;
}

void Product::addProductElementOid(short oid) {
	_productElementOids.push_back(oid);
}

}
