# Third-Party Notices

DrumBeats is released under the MIT License in [LICENSE](LICENSE). The files in `release/`, `windows-release/`, and `linux-release/` also contain third-party software, which is covered by its own licenses.

## Qt 6

The Windows and Linux packages include Qt 6 runtime libraries and plugins, including Qt Core, Qt GUI, Qt Widgets, Qt Multimedia, and related plugins. The exact Qt version depends on the build environment; the current Linux package was built with Qt 6.4.2.

The MSYS2 packages used to build and deploy DrumBeats identify the applicable Qt license as LGPL-3.0-only with the Qt GPL exception, together with additional licenses for components bundled by Qt. Qt's license texts and source code are available from:

- Qt licensing: <https://www.qt.io/licensing/open-source-lgpl-obligations>
- Qt source code: <https://code.qt.io/cgit/>
- LGPL 3.0: <https://www.gnu.org/licenses/lgpl-3.0.html>
- Qt GPL exception: <https://code.qt.io/cgit/qt/qtbase.git/tree/LICENSES/Qt-GPL-exception-1.0.txt>

Qt remains separate third-party software and is not relicensed under DrumBeats' MIT License. When distributing the release package, retain the Qt license information and comply with the applicable LGPL requirements, including the requirements concerning user relinking and corresponding source code.

## FFmpeg

The Windows package includes FFmpeg 8.1.2 libraries used by Qt Multimedia. The Linux package includes Qt's FFmpeg multimedia plugin and uses compatible codec libraries provided by its build/runtime environment:

- Windows FFmpeg DLLs beside the executable
- Windows `multimedia/ffmpegmediaplugin.dll`
- Linux `plugins/multimedia/libffmpegmediaplugin.so`

The MSYS2 package used for the Windows build identifies FFmpeg as **GPL-3.0-or-later**. The Linux package's exact FFmpeg licensing and codec obligations follow the libraries and plugin used by the Linux build; corresponding package license files are included under `linux-release/licenses/`.

- FFmpeg website: <https://ffmpeg.org/>
- FFmpeg source code: <https://git.ffmpeg.org/ffmpeg.git>
- GPL 3.0: <https://www.gnu.org/licenses/gpl-3.0.html>

FFmpeg remains separate third-party software and is not relicensed under DrumBeats' MIT License. The GPL obligations must be reviewed before distributing this release package. In particular, corresponding source code and the applicable license information must be made available as required by the GPL.

## Other bundled components

The Qt and FFmpeg packages may include additional codec, image, font, compression, and system-library components with their own notices. Their license information is available in the package metadata and source distributions used to create this release. Do not remove or replace upstream notices when redistributing the package.

This notice is provided for attribution and distribution information. It is not legal advice.
