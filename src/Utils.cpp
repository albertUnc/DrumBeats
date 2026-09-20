//utils.cpp
#include "Utils.h"

Log logs;
vector<Song> songs;

long long getNowSeconds() {
    auto now = chrono::system_clock::now();
    auto seconds = chrono::floor<chrono::seconds>(now);
    return seconds.time_since_epoch().count();
}
string currentMinute() { //RETURNS [] FORMAT, USE IN LOGS ONLY
    long long s = getNowSeconds();
    chrono::seconds duration{s};
    chrono::sys_seconds tp{duration}; //a time point
    chrono::zoned_time local{chrono::current_zone(), tp};
    string human = format("{:%H:%M:%S}", local);
    return human;
}
string readDate(long long s) { //RETURNS EXAMPLE "2532 Jul 14"
    chrono::seconds duration{s};
    chrono::sys_seconds tp{duration}; //a time point
    return format("{:%Y %b %d}", tp);
}
string mTtos(const messageType &type) {
    switch(type) {
        case messageType::Info : return (string)LOG_INFO;
        case messageType::Warn : return (string)LOG_WARN;
        case messageType::Error : return (string)LOG_ERROR;
        case messageType::Fatal : return (string)LOG_FATAL;
    }
    return "[]";
}