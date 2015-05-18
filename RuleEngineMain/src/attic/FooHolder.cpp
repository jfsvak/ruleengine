/*
 * FooHolder.cpp
 *
 *  Created on: 25/04/2015
 *      Author: jespe_000
 */

#include "attic/FooHolder.h"

#include <iostream>

FooHolder::FooHolder() {
	// TODO Auto-generated constructor stub
}

void FooHolder::fillVectorOfFoo(std::vector<Foo>& vFoo) {
	std::cout << "  Filling vectorOfFoo with copies from another vector" << std::endl;
	vectorOfFoo = vFoo;
}

void FooHolder::createVector() {
	vectorOfFoo.resize(5);
	for ( int i =0;i<5;i++) {
		Foo f {i,i};
		vectorOfFoo[i] = f;
	}
}

void FooHolder::addFoo(Foo& f) {
	vectorOfFoo.push_back(f);
}

const Foo& FooHolder::getFoo(int index) const {
	return vectorOfFoo[index];
}

const std::vector<Foo>& FooHolder::getFoosByRef() const {
	return vectorOfFoo;
}

std::vector<Foo> FooHolder::getFoosByValue() {
	return vectorOfFoo;
}

FooHolder::~FooHolder() {
	// TODO Auto-generated destructor stub
}


