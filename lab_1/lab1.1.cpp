#include "test_runner.h"
#include <sstream>
#include <string>
using namespace std;

class Logger {
public:
    explicit Logger(ostream& output_stream) : os(output_stream) {
    }

    void SetLogLine(bool value) { log_line = value; }
    void SetLogFile(bool value) { log_file = value; }

    void SetFile(const string& f) { file = f; }
    void SetLine(int l) { line = l; }

    void Log(const string& message);

private:
    ostream& os;
    bool log_line = false;
    bool log_file = false;
    string file;
    int line = 0;
};

#define LOG(logger, message)       \
  logger.SetFile(FILE);        \
  logger.SetLine(LINE);        \
  logger.Log(message);

void Logger::Log(const string& message) {
    if (log_file && log_line) {
        os << file << ":" << line << " " << message << endl;
    }
    else if (log_file) {
        os << file << " " << message << endl;
    }
    else if (log_line) {
        os << "Line " << line << " " << message << endl;
    }
    else {
        os << message << endl;
    }
}

void TestLog() {

#line 1 "logger.cpp"

    ostringstream logs;
    Logger l(logs);
    LOG(l, "hello");

    l.SetLogFile(true);
    LOG(l, "hello");

    l.SetLogLine(true);
    LOG(l, "hello");

    l.SetLogFile(false);
    LOG(l, "hello");

    string expected = "hello\n";
    expected += "logger.cpp hello\n";
    expected += "logger.cpp:10 hello\n";
    expected += "Line 13 hello\n";

    ASSERT_EQUAL(logs.str(), expected);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestLog);
}
