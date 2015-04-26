/*
 * Foo.h
 *
 *  Created on: 25/04/2015
 *      Author: jespe_000
 */

#ifndef FOO_H_
#define FOO_H_

class Foo {
public:
	Foo();
	explicit Foo(int val, int val2);
	Foo(const Foo& origin); // copy constructor
	int getValInspect() const;
	int getVal() const;
	virtual ~Foo();
private:
	int _val;
	int _val2;
	void print() const;
};

#endif /* FOO_H_ */
