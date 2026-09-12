# DrumBeats

DrumBeats is a Windows desktop application for playing multiple synchronized audio tracks for a song. It is designed for setups that use a click track, a drumless song, and separated drums, but any of those tracks can be omitted when adding a song.

## Features

- Play, pause, resume, and restart synchronized tracks.
- Enable or mute the click, drum, and drumless-song tracks independently.
- Seek through the song by clicking or dragging the timeline slider.
- Add songs to a local music library.
- Browse for supported audio files from the user's computer.
- Store song names and local track paths between launches.
- Use embedded application backgrounds and other Qt resources.

Supported audio formats:

- `.mp3`
- `.wav`
- `.m4a`

## Downloading And Running

### Prebuilt Windows App

The `release/` folder contains a packaged Windows build. Download the complete folder, or download the repository as a ZIP and extract it first.

Run:

```text
release/DrumBeats.exe
```

Keep the folder structure intact. The executable needs the Qt DLLs, FFmpeg DLLs, and plugin folders beside it. Do not copy only `DrumBeats.exe` to another location.

The packaged build includes:

- `DrumBeats.exe`
- Qt runtime DLLs
- FFmpeg runtime DLLs used by Qt Multimedia
- Qt platform, image, multimedia, network, style, and other plugins
- `LICENSE`
- `THIRD_PARTY_NOTICES.md`

On first launch, the application creates an `AppData/` folder beside the executable. Your song library and application log are stored there:

```text
AppData/songs_data.dat
AppData/log.txt
```

The application does not include music files. When adding a song, choose audio files that already exist on your computer. The saved library stores their local file paths, so moving or renaming those files can make a saved song unavailable.

## Building From Source

### Requirements

- Windows
- CMake 3.16 or newer
- A C++20 compiler
- Qt 6 with the Widgets and Multimedia modules
- MinGW or another compiler supported by the installed Qt build

This project was developed with the MSYS2 UCRT64 environment. The current `CMakeLists.txt` expects Qt at:

```text
C:/Users/veres/MSYS2/ucrt64/lib/cmake
```

If Qt is installed elsewhere, update `CMAKE_PREFIX_PATH` in `CMakeLists.txt` or provide the correct path when configuring CMake.

The source build also requires the `assets/` directory because `assets/resources.qrc` is part of the CMake target. If that directory is not present in a fresh checkout, the resource files must be obtained separately before building.

### Configure And Build

From the project root:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
```

The development executable is created at:

```text
build/DrumBeats.exe
```

To create a redistributable folder after building, copy the executable to a separate folder and run Qt's deployment tool against it:

```powershell
windeployqt --release --compiler-runtime release/DrumBeats.exe
```

The deployment folder must contain the resulting DLLs and plugin directories. Review [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) before distributing a build.

## Project Layout

```text
DrumBeats/
├── assets/                  Qt resource collection and embedded images
├── release/                 Prebuilt Windows executable and runtime files
├── src/                     C++ source, headers, and Qt Designer UI file
│   ├── main.cpp             Application entry point
│   ├── MainWindow.cpp       Main window behavior and signal connections
│   ├── MainWindow.h         Main window declaration
│   ├── MainWindow.ui        Qt Designer interface layout
│   ├── Utils.cpp            Logging and utility implementations
│   ├── Utils.h              Shared types, logging, and filesystem helpers
│   ├── SETTINGS.h           Paths, supported formats, and application settings
│   └── saveTools.h          Song-library save/load helpers
├── CMakeLists.txt            CMake project configuration
├── LICENSE                  MIT license for DrumBeats source code
└── THIRD_PARTY_NOTICES.md   Notices for bundled Qt and FFmpeg components
```

The `build/`, `.cache/`, and `compile_commands.json` files are generated development output. They are not required to run the packaged application.

## How It Works

The application is a Qt Widgets program with a `QMainWindow` containing a `QStackedWidget` for the main menu, settings, music library, song adder, and playback screen.

Three `QMediaPlayer` instances play the selected click, drum, and drumless-song files. The drumless-song player supplies the main timeline duration and position. Its Qt signals update the timeline slider and elapsed/remaining time labels. Seeking changes the position of all available players together.

Application images are listed in `assets/resources.qrc`. CMake's automatic resource processing embeds those resources into the executable during the build, so the packaged application does not need the original image files beside it.

Song records are stored as four text lines per song: name, click-track path, drumless-song path, and drum-track path. The file is saved relative to the executable in `AppData/songs_data.dat`.

## Development Notes

- Keep source files under `src/`.
- Keep generated build output under `build/`.
- Keep redistributable binaries and their runtime dependencies under `release/`.
- Update `assets/resources.qrc` when adding embedded images.
- Re-run CMake after changing the project file or Qt resource collection.
- Test the packaged executable separately from the development build because the packaged build depends on deployed DLLs and plugin folders.

## Licensing

DrumBeats source code is available under the MIT License. The packaged application also contains Qt and FFmpeg components with separate licenses and obligations. See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for details.
