/*
 * Date_sbx.cpp
 *
 *  Created on: 22/06/2015
 *      Author: jfsvak
 */

#include <ctype.h>
#include <iostream>
#include <iomanip>
#include <regex>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include "Date_sbx.h"
#include "Utils.h"

using namespace std;

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

	if (parseDateString(dateToParse, d, m, y))
	{
		setDate(d, m, y);
	}
	else
		throw std::invalid_argument("Date [" + dateToParse + "] is invalid. Valid string format is dd.mm.yyyy");
}

bool Date::validDateFormat(const std::string& date) {
	const char* s = date.c_str();
	return (std::isdigit(s[0])
			&& std::isdigit(s[1])
			&& s[2] == '.'
			&& std::isdigit(s[3])
			&& std::isdigit(s[4])
			&& s[5] == '.'
			&& std::isdigit(s[6])
			&& std::isdigit(s[7])
			&& std::isdigit(s[8])
			&& std::isdigit(s[9]));
}

std::string Date::parseCode(std::regex_constants::error_type etype) {
    switch (etype) {
    case std::regex_constants::error_collate:
        return "error_collate: invalid collating element request";
    case std::regex_constants::error_ctype:
        return "error_ctype: invalid character class";
    case std::regex_constants::error_escape:
        return "error_escape: invalid escape character or trailing escape";
    case std::regex_constants::error_backref:
        return "error_backref: invalid back reference";
    case std::regex_constants::error_brack:
        return "error_brack: mismatched bracket([ or ])";
    case std::regex_constants::error_paren:
        return "error_paren: mismatched parentheses(( or ))";
    case std::regex_constants::error_brace:
        return "error_brace: mismatched brace({ or })";
    case std::regex_constants::error_badbrace:
        return "error_badbrace: invalid range inside a { }";
    case std::regex_constants::error_range:
        return "erro_range: invalid character range(e.g., [z-a])";
    case std::regex_constants::error_space:
        return "error_space: insufficient memory to handle this regular expression";
    case std::regex_constants::error_badrepeat:
        return "error_badrepeat: a repetition character (*, ?, +, or {) was not preceded by a valid regular expression";
    case std::regex_constants::error_complexity:
        return "error_complexity: the requested match is too complex";
    case std::regex_constants::error_stack:
        return "error_stack: insufficient memory to evaluate a match";
    default:
        return "";
    }
}

/**
 * Format: 20151231
 */
Date::Date(int dateAsInteger)
{
    if (dateAsInteger < 10000000) {
        throw std::invalid_argument("Date is invalid.");
    }
    
	int d,m,y;
	std::stringstream ss{};
	ss << dateAsInteger;
	y = atol(ss.str().substr(0, 4).c_str());
	m = atol(ss.str().substr(4, 2).c_str());
	d = atol(ss.str().substr(6, 2).c_str());
	setDate(d,m,y);
}

/**
 * Format: dd.mm.yyyy
 */
bool Date::parseDateString(const std::string& s, int& d, int& m, int& y)
{
	if (!validDateFormat(s)) {
		return false; //throw std::invalid_argument("Date [" + dateToParse + "] is invalid. Valid string format is dd.mm.yyyy");
	}

	std::istringstream is(s);
	char delimiter;
	is >> d >> delimiter >> m >> delimiter >> y;
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
		throw std::invalid_argument("Month [" + sbx::utils::int2string(m) + "] must be between 1-12");

	if ( y >= 1900 && y <= 2100)
		_year = y;

	if (( m == 2 && leapYear(y) && d >=1 && d <= 29)
			|| (d >= 1 && d <= DAYS[m]))
		_day = d;
	else
		throw std::invalid_argument("Day [" + sbx::utils::int2string(d) + "] is out of range for current month [" + sbx::utils::int2string(m) + "] and year [" + sbx::utils::int2string(y) + "]");
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

void Date::add(Date::Field f, int amount) {
	int newYear = year();
	int newMonth = month();
	int newDay = day();

	switch(f) {
		case YEAR:
			newYear += amount;

			// if not leapYear, but we have 29. feb, then set to 01. march
			if (!leapYear(newYear) && month() == 2 && day() == 29) {
				newMonth = 3;
				newDay = 1;
			}

			break;
		case MONTH:
		    throw std::invalid_argument("Adding months is not supported yet!");
		    break;
		case DAY:
			throw std::invalid_argument("Adding days is not supported yet!");
			break;
		default:
			throw std::invalid_argument("Unknown field type!");
			break;
	}

	this->setDate(newDay, newMonth, newYear);
}


Date::~Date()
{
}


} /* namespace sbx */

