//Utils.h
#pragma once
#include "SETTINGS.h"
#include <format>
#include <fstream>
#include <string>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <vector>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#endif
using std::format, std::ofstream, std::string, std::vector;
namespace chrono = std::chrono;
namespace fs = std::filesystem;

string currentMinute();
long long getNowSeconds();
string readDate(long long s);

inline fs::path getExeDir() {
#ifdef _WIN32
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    fs::path unc(path);
    return unc.parent_path();
#else
    std::error_code error;
    fs::path executable = fs::read_symlink("/proc/self/exe", error);
    if (!error) return executable.parent_path();
    return fs::current_path();
#endif
}

inline std::ofstream makeLog() {
    fs::path relative(LOG_EXE_RELATIVE_PATH);
    fs::path path = getExeDir() / relative;
    fs::create_directories(path.parent_path());
    if (CLEAR_LOG_ON_LOAD) {
        return std::ofstream(path, std::ios::trunc);
    } else {
        return std::ofstream(path, std::ios::app);
    }
}

enum class messageType {
    Info,
    Warn,
    Error,
    Fatal
};

string mTtos(const messageType &type);

class Log {
    private:
    ofstream file;
    string signature = "[UNKNOWN] ";
    bool clearIfNeeded() {
        //get abs path of current log
        fs::path fileAbs = getExeDir() / LOG_EXE_RELATIVE_PATH;
        if (!fs::exists(fileAbs)) return true;
        //if too big
        if (fs::file_size(fileAbs) >= MAX_LOG_SIZE) {
            //close log so renaming.deleting/anything doesnt get locked
            file.close();
            //make abs path to the old file
            fs::path oldsPath = fileAbs;
            oldsPath.replace_extension(".old");
            //if it exists, delete it
            if (fs::exists(oldsPath)) {
                fs::remove(oldsPath);
            }
            //rename the current file to the .old
            fs::rename(fileAbs, oldsPath);
            return true;
        }
        return false;
    }

    public:
    void setSignature(string s) {
        signature = s;
    }
    void write(string message, messageType type = messageType::Info) {
        file << format(
            "{} {}{}{}", currentMinute(), signature, mTtos(type), message
        );
        file.flush();
        if (clearIfNeeded()) {
            file = std::move(makeLog());
        }
    }
    void welcome() {
        file = move(makeLog());
        file << format(
            "\n\nLogs loaded. Current date: {}.\nCurrent source is: {}\n\n",
            readDate(getNowSeconds()), signature
        );
    }
    void clearLog() {
        bool temp = CLEAR_LOG_ON_LOAD;
        CLEAR_LOG_ON_LOAD = true;
        file.close();
        file = move(makeLog());
        CLEAR_LOG_ON_LOAD = temp;
    }
    string getCurrentSignature() {return signature;}
};
extern Log logs;

struct Song {
    string clickPath;
    string songPath;
    string drumsPath;
    string name;
    Song(const string click, const string song, const string drums, const string nm) {
        logs.write(format("Creating new song. Details:\nClick track: {}\nSong track: {}\nDrums track: {}\nName: {}\n", click, song, drums, nm));
        clickPath = click;
        songPath = song;
        drumsPath = drums;
        name = nm;
    }
};
extern vector<Song> songs;