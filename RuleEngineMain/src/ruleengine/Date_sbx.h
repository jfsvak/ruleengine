/*
 * Date_sbx.h
 *
 *  Created on: 22/06/2015
 *      Author: jfsvak
 */

#ifndef RULEENGINE_DATE_SBX_H_
#define RULEENGINE_DATE_SBX_H_

#include <array>
#include <iostream>
#include <string>

namespace sbx {

class Date {
	friend std::ostream& operator << (std::ostream&, const sbx::Date& d);
//	friend std::istream& operator >> (std::istream&, sbx::Date& d);
public:
	Date();
	explicit Date(const std::string& dateToParse);
	explicit Date(int dateAsInteger);
	Date(unsigned short day, unsigned short month, unsigned short year);

	bool operator < (const sbx::Date& otherDate) const;
	bool operator <= (const sbx::Date& otherDate) const;
	bool operator > (const sbx::Date& otherDate) const;
	bool operator >= (const sbx::Date& otherDate) const;
	bool operator == (const sbx::Date& otherDate) const;


	int day() const;
	int month() const;
	int year() const;

	static bool leapYear(int);
	virtual ~Date();
private:
	bool extractDate(const std::string& s, int& d, int& m, int& y);
	void setDate(int d, int m, int y);
	unsigned short _day;
	unsigned short _month;
	unsigned short _year;
	static const std::array<unsigned short, 13> DAYS;
};

} /* namespace sbx */

#endif /* RULEENGINE_DATE_SBX_H_ */
