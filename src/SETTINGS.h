//SETTINGS.h
#pragma once
#include <string>
#include <cstdint>
#include <vector>

#include "Utils.h"

using std::string;

#ifdef ERROR
#undef ERROR
#endif

//Normally, a settings file would contain #define's and const vars, but this one will contain non-const types as well, since some of the settings will be changeable from the app.

constexpr inline std::string NAME = "DrumBeats";

constexpr inline std::string_view LOG_INFO = "[INFO] ";
constexpr inline std::string_view LOG_WARN = "[WARN] ";
constexpr inline std::string_view LOG_ERROR = "[ERROR] ";
constexpr inline std::string_view LOG_FATAL = "[FATAL] ";
constexpr inline std::string_view LOG_APPDATA_RELATIVE_PATH = "log.txt";
inline bool CLEAR_LOG_ON_LOAD = false;
inline uintmax_t MAX_LOG_SIZE = 1ULL * 1024 * 1024 * 1024; //1GB, change the 1 at the start to increase GB, change the first 1024 and remove the first 1 to change MB. For examble if you want just 500 MB as the limit: 500 * 1024 * 1024
//Important note^: whatever you change, the first number should have ULL after it. This converts it to unsigned long long, which has a much bigger limit, this prevents interger overflow

constexpr inline std::string_view SONGS_APPDATA_RELATIVE_PATH = "songs_data.dat";

const std::vector<string> supportedFormats = {
    ".mp3",
    ".wav",
    ".m4a"
};