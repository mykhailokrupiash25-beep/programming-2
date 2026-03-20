#include <iomanip>
#include <iostream>
#include <vector>
#include <string>
#include <fstream> 

#include "profile.h"

using namespace std;

class ReadingManager {
public:
    ReadingManager()
        : user_pages_(MAX_USER_COUNT_ + 1, 0),
        pages_statistics_(MAX_PAGE_COUNT_ + 1, 0),
        total_users_(0) {}

    void Read(int user_id, int page_count) {
        int old_page = user_pages_[user_id];

        if (old_page == 0) {
            total_users_++;
        }
        else {
            pages_statistics_[old_page]--;
        }

        user_pages_[user_id] = page_count;
        pages_statistics_[page_count]++;
    }

    double Cheer(int user_id) const {
        int current_page = user_pages_[user_id];

        if (current_page == 0) return 0;
        if (total_users_ == 1) return 1;

        int users_less_than = 0;
        for (int p = 1; p < current_page; ++p) {
            users_less_than += pages_statistics_[p];
        }

        return users_less_than * 1.0 / (total_users_ - 1);
    }

private:
    static const int MAX_USER_COUNT_ = 100'000;
    static const int MAX_PAGE_COUNT_ = 1000;

    vector<int> user_pages_;
    vector<int> pages_statistics_;
    int total_users_;
};

int main() {
    LOG_DURATION("full_execution");

    ifstream input_file("input.txt");

    if (!input_file.is_open()) {
        cerr << "Error: Could not open input.txt" << endl;
        return 1;
    }

    cin.rdbuf(input_file.rdbuf());

    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ReadingManager manager;
    int query_count;

    if (!(cin >> query_count)) return 0;

    for (int i = 0; i < query_count; ++i) {
        string query_type;
        cin >> query_type;
        int user_id;
        cin >> user_id;

        if (query_type == "READ") {
            int page_count;
            cin >> page_count;
            manager.Read(user_id, page_count);
        }
        else if (query_type == "CHEER") {
            cout << setprecision(6) << manager.Cheer(user_id) << "\n";
        }
    }

    return 0;
}