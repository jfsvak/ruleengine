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
#include <regex>
#include <string>

namespace sbx {

class Date {
	friend std::ostream& operator << (std::ostream&, const sbx::Date& d);
//	friend std::istream& operator >> (std::istream&, sbx::Date& d);


public:

	enum Field {
		DAY,
		MONTH,
		YEAR
	};
	Date();
	explicit Date(const std::string& dateToParse);
	explicit Date(int dateAsInteger);
	Date(unsigned short day, unsigned short month, unsigned short year);

	bool operator < (const sbx::Date& otherDate) const;
	bool operator <= (const sbx::Date& otherDate) const;
	bool operator > (const sbx::Date& otherDate) const;
	bool operator >= (const sbx::Date& otherDate) const;
	bool operator == (const sbx::Date& otherDate) const;
	std::string parseCode(std::regex_constants::error_type etype);

	void add(Date::Field, int amount);

	int day() const;
	int month() const;
	int year() const;

	static bool leapYear(int);
	virtual ~Date();
private:
	bool parseDateString(const std::string& s, int& d, int& m, int& y);
	bool validDateFormat(const std::string& date);
	void setDate(int d, int m, int y);
	unsigned short _day;
	unsigned short _month;
	unsigned short _year;
	static const std::array<unsigned short, 13> DAYS;
};

} /* namespace sbx */

#endif /* RULEENGINE_DATE_SBX_H_ */
