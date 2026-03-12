#include "airline_ticket.h"
#include "test_runner.h"

#include <algorithm>
#include <tuple>
#include <iomanip>

using namespace std;

bool operator<(const Date& lhs, const Date& rhs) {
    return tie(lhs.year, lhs.month, lhs.day) < tie(rhs.year, rhs.month, rhs.day);
}

bool operator==(const Date& lhs, const Date& rhs) {
    return tie(lhs.year, lhs.month, lhs.day) == tie(rhs.year, rhs.month, rhs.day);
}

// Вивід Date у потік
ostream& operator<<(ostream& os, const Date& d) {
    return os << setfill('0') << setw(4) << d.year << "-"
        << setw(2) << d.month << "-"
        << setw(2) << d.day;
}

bool operator<(const Time& lhs, const Time& rhs) {
    return tie(lhs.hours, lhs.minutes) < tie(rhs.hours, rhs.minutes);
}

bool operator==(const Time& lhs, const Time& rhs) {
    return tie(lhs.hours, lhs.minutes) == tie(rhs.hours, rhs.minutes);
}

ostream& operator<<(ostream& os, const Time& t) {
    return os << setfill('0') << setw(2) << t.hours << ":"
        << setw(2) << t.minutes;
}

#define SORT_BY(field)                                       \
  [](const AirlineTicket& lhs, const AirlineTicket& rhs) {   \
    return lhs.field < rhs.field;                            \
  }

void TestSortBy() {
    vector<AirlineTicket> tixs = {
      {"VKO", "AER", "Utair",     {2018, 2, 28}, {17, 40}, {2018, 2, 28}, {20,  0}, 1200},
      {"AER", "VKO", "Utair",     {2018, 3,  5}, {14, 15}, {2018, 3,  5}, {16, 30}, 1700},
      {"AER", "SVO", "Aeroflot",  {2018, 3,  5}, {18, 30}, {2018, 3,  5}, {20, 30}, 2300},
      {"PMI", "DME", "Iberia",    {2018, 2,  8}, {23, 00}, {2018, 2,  9}, {03, 30}, 9000},
      {"CDG", "SVO", "AirFrance", {2018, 3,  1}, {13, 00}, {2018, 3,  1}, {17, 30}, 8000},
    };

    sort(begin(tixs), end(tixs), SORT_BY(price));
    ASSERT_EQUAL(tixs.front().airline, "Utair");
    ASSERT_EQUAL(tixs.back().airline, "Iberia");

    sort(begin(tixs), end(tixs), SORT_BY(from));
    ASSERT_EQUAL(tixs.front().airline, "Utair");
    ASSERT_EQUAL(tixs.back().airline, "Utair");

    sort(begin(tixs), end(tixs), SORT_BY(arrival_date));
    ASSERT_EQUAL(tixs.front().airline, "Iberia");
    ASSERT_EQUAL(tixs.back().airline, "Aeroflot");
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSortBy);
}