#include <string>
#include <vector>
#include <iostream>

#include "gtest/gtest.h"

#include "../ruleengine/Date_sbx.h"

using namespace std;
using namespace sbx;

class Date_SBX : public ::testing::Test {
protected:
    virtual void SetUp() {
    }

};

TEST_F(Date_SBX, Date_String_Parsing) {
	Date d{"07.10.2015"};

	EXPECT_EQ(2015, d.year());
	EXPECT_EQ(10, d.month());
	EXPECT_EQ(7, d.day());
}

TEST_F(Date_SBX, Date_String_Parsing_Invalid) {
	EXPECT_NO_THROW(Date d1{"28.02.2015"});
	EXPECT_NO_THROW(Date d1{"29.02.2000"});
	EXPECT_NO_THROW(Date d2{"31.12.2015"});

	EXPECT_THROW(Date d3{"01012015"}, invalid_argument);
	EXPECT_THROW(Date d3{"010115"}, invalid_argument);
	EXPECT_THROW(Date d3{"as.df.2015"}, invalid_argument);
	EXPECT_THROW(Date d4{"01.00.20"}, invalid_argument);
	EXPECT_THROW(Date d5{"01.01.99"}, invalid_argument);
}

TEST_F(Date_SBX, Date_int_Parsing) {
	Date d{20151007};

	EXPECT_EQ(2015, d.year());
	EXPECT_EQ(10, d.month());
	EXPECT_EQ(7, d.day());
}

TEST_F(Date_SBX, Date_add_year) {
	Date d{20151007};

	d.add(sbx::Date::YEAR, 3);
	EXPECT_EQ(2018, d.year());
	EXPECT_EQ(10, d.month());
	EXPECT_EQ(7, d.day());
}

TEST_F(Date_SBX, Date_add_year_leap) {
	Date d{20000229};

	d.add(sbx::Date::YEAR, 3);
	EXPECT_EQ(2003, d.year());
	EXPECT_EQ(3, d.month());
	EXPECT_EQ(1, d.day());
}

TEST_F(Date_SBX, Date_add_year_leap_plus4) {
	Date d{20000229};

	d.add(sbx::Date::YEAR, 4);
	EXPECT_EQ(2004, d.year());
	EXPECT_EQ(2, d.month());
	EXPECT_EQ(29, d.day());
}

TEST_F(Date_SBX, Date_add_month) {
	Date d{20151007};

	EXPECT_THROW(d.add(sbx::Date::MONTH, 2), invalid_argument);
}

TEST_F(Date_SBX, Date_add_day) {
	Date d{20151007};

	EXPECT_THROW(d.add(sbx::Date::DAY, 2), invalid_argument);
}
