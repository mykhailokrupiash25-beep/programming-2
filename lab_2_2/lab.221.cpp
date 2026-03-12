#include "airline_ticket.h"
#include "test_runner.h"

#include <algorithm>
#include <tuple>
#include <string>
#include <map>
#include <sstream>

using namespace std;

istream& operator>>(istream& is, Date& d) {
    is >> d.year;
    is.ignore(1); 
    is >> d.month;
    is.ignore(1); 
    is >> d.day;
    return is;
}

bool operator==(const Date& lhs, const Date& rhs) {
    return tie(lhs.year, lhs.month, lhs.day) == tie(rhs.year, rhs.month, rhs.day);
}

ostream& operator<<(ostream& os, const Date& d) {
    return os << d.year << "-" << d.month << "-" << d.day;
}

istream& operator>>(istream& is, Time& t) {
    is >> t.hours;
    is.ignore(1);
    is >> t.minutes;
    return is;
}

bool operator==(const Time& lhs, const Time& rhs) {
    return tie(lhs.hours, lhs.minutes) == tie(rhs.hours, rhs.minutes);
}

ostream& operator<<(ostream& os, const Time& t) {
    return os << t.hours << ":" << t.minutes;
}

#define UPDATE_FIELD(ticket, field, values) {        \
    auto it = values.find(#field);                   \
    if (it != values.end()) {                        \
        istringstream is(it->second);                \
        is >> ticket.field;                          \
    }                                                \
}

void TestUpdate() {
    AirlineTicket t;
    t.from = "LVIV";
    t.to = "BORYSPIL";
    t.airline = "I7";
    t.departure_date = { 2018, 2, 28 };
    t.departure_time = { 17, 40 };
    t.price = 3500;

    const map<string, string> updates = {
      {"from", "Ivano-frank"},
      {"to", "BORYSPIL"},
      {"departure_date", "2026-29-01"},
      {"departure_time", "14:00"},
      {"price", "5000"}
    };

    UPDATE_FIELD(t, from, updates);
    UPDATE_FIELD(t, to, updates);
    UPDATE_FIELD(t, departure_date, updates);
    UPDATE_FIELD(t, departure_time, updates);
    UPDATE_FIELD(t, price, updates);

    ASSERT_EQUAL(t.from, "Ivano-frank");
    ASSERT_EQUAL(t.to, "BORYSPIL");
    ASSERT_EQUAL(t.departure_date, (Date{ 2026, 29, 1 }));
    ASSERT_EQUAL(t.departure_time, (Time{ 14, 00 }));
    ASSERT_EQUAL(t.price, 5000);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestUpdate);
}