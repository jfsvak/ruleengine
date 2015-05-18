/*
 * FooHolder.h
 *
 *  Created on: 25/04/2015
 *      Author: jespe_000
 */

#ifndef ATTIC_FOOHOLDER_H_
#define ATTIC_FOOHOLDER_H_

#include <vector>

#include "attic/Foo.h"

class FooHolder {
public:
	FooHolder();
	virtual ~FooHolder();
	void createVector();
	void fillVectorOfFoo(std::vector<Foo>&);
	void addFoo(Foo& f);
	const Foo& getFoo(int index) const;
	const std::vector<Foo>& getFoosByRef() const;
	std::vector<Foo> getFoosByValue();
private:
	std::vector<Foo> vectorOfFoo;
};

#eATTIC_FOOHOLDER_H_OLDER_H_ */
