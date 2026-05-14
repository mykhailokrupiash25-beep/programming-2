#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <algorithm>
#include "profile.h"
using namespace std;

void sort(){
    LOG_DURATION("function: ");
    int n;
    if (!(cin >> n)) {
        throw runtime_error("an error occurred while reading the file");
    }
    list<int> players;
    vector<list<int>::iterator> positions(100002, players.end());
    for (int i = 0; i < n; ++i) {
        int current_player, target_player;
        cin >> current_player >> target_player;
        auto new_position = players.insert(positions.at(target_player), current_player);
        positions.at(current_player) = new_position;
    }
    for (int player : players) {
        cout << player << "\n";
    }
}

int main() {
    LOG_DURATION("all ");
    freopen("input.txt", "r", stdin);
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    sort();
    return 0;
}