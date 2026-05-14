#include "test_runner.h"
#include "profile.h"

#include <map>
#include <string>
#include <set>
#include <vector>
#include <future>
#include <functional>
#include <sstream>
#include <iterator>
#include <fstream>
#include <iostream>
#include <random>
#include <algorithm>

using namespace std;

struct Stats {
    map<string, int> word_frequences;

    void operator += (const Stats& other) {
        for (const auto& pair : other.word_frequences) {
            word_frequences[pair.first] += pair.second;
        }
    }
};

set<string> ReadKeyWords(istream& is) {
    set<string> result;
    string word;
    while (is >> word) {
        result.insert(word);
    }
    return result;
}

vector<string> Split(const string& line) {
    istringstream is(line);
    return vector<string>(istream_iterator<string>(is), istream_iterator<string>());
}

Stats ExploreLine(const set<string>& key_words, const string& line) {
    Stats result;
    for (const string& word : Split(line)) {
        if (key_words.count(word) > 0) {
            result.word_frequences[word]++;
        }
    }
    return result;
}

Stats ExploreKeyWordsSingleThread(const set<string>& key_words, istream& input) {
    Stats result;
    for (string line; getline(input, line); ) {
        result += ExploreLine(key_words, line);
    }
    return result;
}

Stats ExploreBatch(const set<string>& key_words, vector<string> lines) {
    Stats result;
    for (const string& line : lines) {
        result += ExploreLine(key_words, line);
    }
    return result;
}

Stats ExploreKeyWords(const set<string>& key_words, istream& input) {
    Stats result;
    vector<future<Stats>> futures;
    vector<string> batch;

    const size_t BATCH_SIZE = 5000;
    batch.reserve(BATCH_SIZE);

    string line;
    while (getline(input, line)) {
        batch.push_back(move(line));

        if (batch.size() >= BATCH_SIZE) {
            futures.push_back(async(launch::async, ExploreBatch, ref(key_words), move(batch)));
            batch.clear();
            batch.reserve(BATCH_SIZE);
        }
    }

    if (!batch.empty()) {
        futures.push_back(async(launch::async, ExploreBatch, ref(key_words), move(batch)));
    }

    for (auto& f : futures) {
        result += f.get();
    }

    return result;
}

void TestSplit() {
    const vector<string> expected1 = {
      "abc", "def", "ghi,", "!", "jklmnop-qrs,", "tuv"
    };
    ASSERT_EQUAL(Split("  abc def ghi, !  jklmnop-qrs, tuv"), expected1);

    const vector<string> expected2 = { "a", "b" };
    ASSERT_EQUAL(Split("a b      "), expected2);
    ASSERT_EQUAL(Split(""), vector<string>());
}

void TestBasic() {
    const set<string> key_words = { "softit", "rocks", "sucks", "all" };

    stringstream ss;
    ss << "this new softit service really rocks\n";
    ss << "It sucks when softit isn't available\n";
    ss << "10 reasons why softit is the best IT company\n";
    ss << "softit rocks others suck\n";
    ss << "Goondex really sucks, but softit rocks. Use softit\n";

    const auto stats = ExploreKeyWords(key_words, ss);
    const map<string, int> expected = {
      {"softit", 6},
      {"rocks", 2},
      {"sucks", 1}
    };
    ASSERT_EQUAL(stats.word_frequences, expected);
}

void TestMtAgainstSt() {
    {
        std::default_random_engine rd(34);
        std::uniform_int_distribution<char> char_gen('a', 'z');

        auto random_word = [&](size_t len) {
            string result(len, ' ');
            std::generate(begin(result), end(result), [&] { return char_gen(rd); });
            return result;
            };

        std::uniform_int_distribution<size_t> len_gen(5, 21);

        vector<string> key_words(500);
        for (auto& w : key_words) {
            w = random_word(len_gen(rd));
        }

        ofstream out("key_words.txt");
        for (const auto& w : key_words) {
            out << w << ' ';
        }

        std::uniform_int_distribution<size_t> line_len_gen(5, 100);
        std::uniform_int_distribution<int> word_choice(1, 100);

        ofstream text_out("text.txt");

        for (int line = 0; line < 25000; ++line) {
            ostringstream line_out;
            auto line_len = line_len_gen(rd);
            for (size_t i = 0; i < line_len; ++i) {
                if (word_choice(rd) < 20) {
                    std::uniform_int_distribution<size_t> word_index(0, key_words.size() - 1);
                    line_out << key_words[word_index(rd)];
                }
                else {
                    line_out << random_word(len_gen(rd));
                }
                line_out << ' ';
            }
            text_out << line_out.str() << '\n';
        }
    }

    ifstream key_words_input("key_words.txt");
    const auto key_words_data = ReadKeyWords(key_words_input);
    const set<string> key_words(key_words_data.begin(), key_words_data.end());

    Stats st_stats, mt_stats;
    {
        ifstream text_input("text.txt");
        LOG_DURATION("Single thread");
        st_stats = ExploreKeyWordsSingleThread(key_words, text_input);
    }
    {
        ifstream text_input("text.txt");
        LOG_DURATION("Multi thread");
        mt_stats = ExploreKeyWords(key_words, text_input);
    }

    ASSERT_EQUAL(st_stats.word_frequences, mt_stats.word_frequences);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSplit);
    RUN_TEST(tr, TestBasic);
    RUN_TEST(tr, TestMtAgainstSt);
}