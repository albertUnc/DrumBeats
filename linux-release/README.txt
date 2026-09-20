DrumBeats Linux release
=======================

Run the application from this directory with:

    ./DrumBeats.sh

The launcher sets the bundled Qt library and plugin paths. Keep the lib/ and
plugins/ directories beside DrumBeats and qt.conf.

This package was built on x86_64 Linux with Qt 6.4.2, CMake, and the system
FFmpeg 6.1 libraries. It is a relocatable Qt application bundle for Linux
systems with compatible host graphics, audio, and standard C/C++ libraries.
It does not bundle glibc, X11/Wayland, OpenGL, PulseAudio, or kernel drivers;
those are host-platform components and should be supplied by the target Linux
distribution. The FFmpeg multimedia plugin is included, but its codec and
system-library dependencies must be compatible with the target system.

The application stores its library and log in AppData/ beside the executable.
Music files are not included.

License files are in licenses/. DrumBeats is MIT-licensed. Qt and FFmpeg are
separate third-party components; retain their notices when redistributing this
folder and review the applicable LGPL/GPL obligations. The copyright files in
licenses/ identify the installed package versions used for this build.