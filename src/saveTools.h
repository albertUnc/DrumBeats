#pragma once
#include "Utils.h"
static constexpr string currentSig = "[saveTools.h] ";

inline void saveSongs() {
    string oldSig = logs.getCurrentSignature();
    logs.setSignature(currentSig);
    logs.write("Saving songs...\n");
    //0.Check if there are even any songs
    if (songs.empty()) {
        logs.write("No songs to save. returning saveSongs()\n", messageType::Warn);
        logs.setSignature(oldSig);
        return;
    }
    //1.Get absolute path to file to save to
    fs::path absPath = getExeDir() / SONGS_EXE_RELATIVE_PATH;
    //2.Check for file
    if (!fs::exists(absPath)) {
        if (!fs::exists(absPath.parent_path())) {
            logs.write("AppData (or whatever the parent of the song saves is) folder does not exist.\n", messageType::Warn);
        } else {
            logs.write("Songs saves file does not exist. Recreating it...\n", messageType::Warn);
        }
        fs::create_directories(absPath.parent_path());
    }
    //3.Create and write file
    ofstream file(absPath, std::ios::trunc); //clear on load
    int count = 0;
    for (Song temp : songs) {
        logs.write(format("Saving song {}\n", temp.name));
        file << format(
            "{}\n{}\n{}\n{}\n",
            temp.name, temp.clickPath, temp.songPath, temp.drumsPath
        );
        count++;
    }
    logs.write(format("Saved {} songs.\n", count));
    logs.setSignature(oldSig);
}
inline void loadSongs() {
    string oldSig = logs.getCurrentSignature();
    logs.setSignature(currentSig);
    logs.write("Loading songs...\n");
    fs::path absPath = getExeDir() / SONGS_EXE_RELATIVE_PATH;
    if (!fs::exists(absPath)) {
        logs.write("Path to save file doesn't exist!", messageType::Error);
        logs.setSignature(oldSig);
        return;
    }
    std::ifstream file(absPath);
    string line, name, click, song, drums;
    int index = 1;
    int count = 0;
    while (getline(file, line)) {
        if (index == 1) {
            name = line;
            index++;
            continue;
        }
        if (index == 2) {
            click = line;
            index++;
            continue;
        }
        if (index == 3) {
            song = line;
            index++;
            continue;
        }
        if (index == 4) {
            drums = line;

            Song temp(click, song, drums, name);
            songs.push_back(temp);
            logs.write(format("Loaded song {}\n", name));

            index = 1;
            count++;
        }
    }
    logs.write(format("Loaded {} songs.\n", count));
    logs.setSignature(oldSig);
}