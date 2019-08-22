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

Atm, to build, you need the following:

 - [clang](https://clang.llvm.org) 8
 - [ninja](https://ninja-build.org) 1.3+
 - [OpenGL] (https://opengl.org) 4.1+
 - [GLFW](https://glfw.org) 3.3
 - [GLAD](https://github.com/Dav1dde/glad)

Brief note: currently, there is NO support for Windows platforms; if you do have
MS compatibility patches, feel free to add them- Ynk has only been tested on macOS,
so all portability contributions are welcome.

If you're reading through the code, then check STRUCTURE.md for details. If you
see any other `.md` files, then those are documentation files, and might be worth reading.
