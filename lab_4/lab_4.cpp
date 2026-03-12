#include "student.h"
#include "test_runner.h"
#include "profile.h"
#include <algorithm>
#include <vector>

using namespace std;

bool Compare(const Student& first, const Student& second) {
    return first.Less(second);
}

void TestComparison() {
    LOG_DURATION("TestComparison total");

    Student newbie{
      "Ivan", "Ivaniv", {
        {"C++", 1.0},
        {"Algorithms", 3.0}
      },
      2.0
    };

    Student cpp_expert{
      "Petro", "Petriv", {
        {"C++", 9.5},
        {"Algorithms", 6.0}
      },
      7.75
    };

    Student guru{
      "Stepan", "Stepaniv", {
        {"C++", 10.0},
        {"Algorithms", 10.0}
      },
      10.0
    };

    ASSERT(Compare(guru, newbie));
    ASSERT(Compare(guru, cpp_expert));
    ASSERT(!Compare(newbie, cpp_expert));
}

void TestSorting() {
    LOG_DURATION("TestSorting total");

    vector<Student> students{
      {"Stepan", "Stepaniv", {{"Maths", 2.}}, 2.},
      {"Semen", "Semeniv", {{"Maths", 4.}}, 4.},
      {"Ivan", "Ivaniv", {{"Maths", 5.}}, 5.},
      {"Petro", "Petriv", {{"Maths", 3.}}, 3.},
      {"Oleksandr", "Oleksandrenko", {{"Maths", 1.}}, 1.}
    };

    {
        LOG_DURATION("Sort duration");
        sort(students.begin(), students.end(), Compare);
    }

    ASSERT(is_sorted(students.begin(), students.end(),
        [](Student first, Student second) {
            return first.Less(second);
        })
    );
}

int main() {
    TestRunner tr;

    LOG_DURATION("Total tests time");

    RUN_TEST(tr, TestComparison);
    RUN_TEST(tr, TestSorting);

    return 0;
}