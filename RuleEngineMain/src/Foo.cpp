/*
 * Foo.cpp
 *
 *  Created on: 25/04/2015
 *      Author: jespe_000
 */
#include <iostream>
#include <memory>

#include "Foo.h"

using namespace std;

Foo::Foo() : _val {0}, _val2 {0} {
	cout << "+ ";
	print();
}

Foo::Foo(int val, int val2) : _val {val}, _val2{val2} {
	cout << "+ ";
	print();
}

Foo::Foo(const Foo& origin)	: _val{origin._val}, _val2{origin._val2} {
	cout << "= Foo{" << std::addressof(origin) << "}->Foo{" << this << "}" << endl;
}

void Foo::print() const {
	cout << "Foo{" << this << "}, val[" << _val << "], val2[" << _val2 << "]" << endl;
}

int Foo::getValInspect() const {
	return _val;
}

int Foo::getVal() const {
	return _val;
}

Foo::~Foo() {
	cout << "- ";
	print();
}
