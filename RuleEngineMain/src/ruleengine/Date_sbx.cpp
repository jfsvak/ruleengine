/*
 * Date_sbx.cpp
 *
 *  Created on: 22/06/2015
 *      Author: jfsvak
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdlib.h>

#include "Date_sbx.h"

namespace sbx {

const std::array<unsigned short, 13> Date::DAYS = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

std::ostream& operator << (std::ostream& output, const Date &d)
{
	std::stringstream ss{};
	ss << ((d.day() < 10) ? "0" : "") << d.day();
	ss << ".";
	ss << ((d.month() < 10) ? "0" : "") << d.month();
	ss << ".";
	ss << d.year();
	output << ss.str();
	return output;
}

//std::istream& operator >> (std::istream& input, Date &d)
//{
//	input >> setw(2) >> d.;
//}

bool Date::leapYear(int testYear)
{
	if (testYear % 400 == 0 || (testYear % 100 != 0 && testYear % 4 == 0))
		return true;
	else
		return false;
}

Date::Date()
{
}

Date::Date(const std::string& dateToParse)
{
	int d,m,y;

	if (extractDate(dateToParse, d, m, y))
	{
		setDate(d, m, y);
	}
	else
		throw std::invalid_argument("date [" + dateToParse + "]is invalid");
}

Date::Date(int dateAsInteger)
{
	int d,m,y;
	std::stringstream ss{};
	ss << dateAsInteger;
	std::string sDate = ss.str();
	y = atol(ss.str().substr(0, 4).c_str());
	m = atol(ss.str().substr(4, 2).c_str());
	d = atol(ss.str().substr(6, 2).c_str());
	setDate(d,m,y);
}

bool Date::extractDate(const std::string& s, int& d, int& m, int& y)
{
	std::istringstream is(s);
	char delimiter;
	is >> d >> delimiter >> m >> delimiter >> y;
//	if (is >> d >> delimiter >> m >> delimiter >> y)
//	{
//		struct tm t = {0};
//		t.tm_mday = d;
//		t.tm_mon = m - 1;
//		t.tm_year = y - 1900;
//		t.tm_isdst = -1;
//
//		// normalize:
//		time_t when = mktime(&t);
//		const struct tm *norm = localtime(&when);
//		// the actual date would be:
//		// m = norm->tm_mon + 1;
//		// d = norm->tm_mday;
//		// y = norm->tm_year;
//		// e.g. 29/02/2013 would become 01/03/2013
//
//		// validate (is the normalized date still the same?):
//		return (norm->tm_mday == d    &&
//				norm->tm_mon  == m - 1 &&
//				norm->tm_year == y - 1900);
//	}
	return true;
}

Date::Date(unsigned short day, unsigned short month, unsigned short year)
{
	this->setDate(day, month, year);
}

int Date::day() const
{
	return _day;
}

int Date::month() const
{
	return _month;
}

int Date::year() const
{
	return _year;
}

void Date::setDate(int d, int m, int y)
{
	if ( m >=1 && m <= 12)
		_month = m;
	else
		throw std::invalid_argument("Month must be between 1-12");

	if ( y >= 1900 && y <= 2100)
		_year = y;

	if (( m == 2 && leapYear(y) && d >=1 && d <= 29)
			|| (d >= 1 && d <= DAYS[m]))
		_day = d;
	else
		throw std::invalid_argument("Day [] is out of range for current month [] and year []");
}

bool Date::operator < (const sbx::Date& otherDate) const
{
	if (this != &otherDate) {

		if (this->_year < otherDate._year)
			return true;
		else if (this->_year > otherDate._year)
			return false;
		else if (this->_month < otherDate._month)
			return true;
		else if (this->_month > otherDate._month)
			return false;
		else if (this->_day < otherDate._day)
			return true;
		else if (this->_day > otherDate._day)
			return false;
	}

	return false;
}

bool Date::operator > (const sbx::Date& otherDate) const
{
	if (this != &otherDate) {

		if (this->_year > otherDate._year)
			return true;
		else if (this->_year < otherDate._year)
			return false;
		else if (this->_month > otherDate._month)
			return true;
		else if (this->_month < otherDate._month)
			return false;
		else if (this->_day > otherDate._day)
			return true;
		else if (this->_day < otherDate._day)
			return false;
	}

	return false;
}

bool Date::operator <=(const sbx::Date& otherDate) const
{
	return (*this == otherDate || *this < otherDate);
}

bool Date::operator >=(const sbx::Date& otherDate) const
{
	return (*this == otherDate || *this > otherDate);
}

bool Date::operator ==(const sbx::Date& otherDate) const
{
	if (this == &otherDate)
		return true;
	return (this->_year == otherDate._year && this->_month == otherDate._month && this->_day == otherDate._day);
}

Date::~Date()
{
}


} /* namespace sbx */

