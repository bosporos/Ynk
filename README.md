# Ynk

Stock C++ library, targeted at (computationally intensive) 2d generative art.

Future plans:
 - More string support (UTF-8, UTF-16, etc.)
 - A strong pthread-based multithreading support system
 - Fast synchronization primitives based on WTF's ParkingLot
 - More comprehensive 2d graphics systems
 - More graph-theory based library support
 - Nostdlib (minimal libc/libcxx use, no STL use, maximized use of platform APIs and interfaces)
 - Integrated CRXAlloc
 - Infinite-length numbers and other maths support
 - Better support for common patterns + structures (i.e. Iterator, etc.)
 - Testing framework
 - CMake. Seriously. The build on this thing is just a big ninjafile right now. Like, I literally have includes sticking into my home directory, and the ninjafile assumes in-tree GLAD. Yep. CMake. Pls send help

Atm, to build, you need the following:

 - [clang](https://clang.llvm.org) 8
 - [ninja](https://ninja-build.org) 1.3+
 - [OpenGL](https://opengl.org) 4.1+
 - [GLFW](https://glfw.org) 3.3
 - [GLAD](https://github.com/Dav1dde/glad)
 - [linmath](https://github.com/datenwolf/linmath.h) or as provided in GLFW deps

Brief note: currently, there is NO support for Windows platforms; if you do have
MS compatibility patches, feel free to add them- Ynk has only been tested on macOS,
so all portability contributions are welcome.

If you're reading through the code, then check STRUCTURE.md for details. If you
see any other `.md` files, then those are documentation files, and might be worth reading.

## Building

Requirements:

 - There must be a "glad/glad.h" somewhere in the include path
 - `arc4random_uniform()` must be accessible from `stdlib.h`

Remove the last few lines of clang/dev-cxxflags.txt (i.e. section "Kludge").

Run `ninja` in the directory containing this README.

There should now be a `build` directory.

#### Packaging for macOS

Run `artisan/macos/osx_export.sh` from the directory containing this README (don't run it anywhere else, seriously).

There should be two things of interest in the `build` directory: `YnkExported.app` and `YnkExported.app.zip`.
