#
# file CMakeLists.txt
# author Maximilien M. Cura
#

cmake_minimum_required(VERSION 3.13)

cmake_minimum_required(VERSION 3.13)

project(Ynk)

enable_language(ASM)
enable_language(C)
enable_language(CXX)

option(USE_X86_64_SVR4_UTF8_STRLEN "Use a specialized UTF-8 string length algorithm instead of the generic C++ on" OFF)

set(ycm_root "${PROJECT_SOURCE_DIR}")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# ==============================================================================
# PLATFORM CONFIG
# ==============================================================================

if(APPLE)
    set(YNK_PLATFORM_APPLE ON)
    set(YNK_POSIX ON)
endif()

# ==============================================================================
# PLATFORM DEPENDENCIES
# ==============================================================================

if(YNK_PLATFORM_APPLE)
    find_library(_MACOS_OPENGL OpenGL)
    if(NOT _MACOS_OPENGL)
        message(FATAL_ERROR "OpenGL not found!")
    else()
        set(YNK_LIB_OPENGL ${_MACOS_OPENGL})
    endif()
else()
    set(YNK_LIB_OPENGL "gl")
endif()

if(YNK_POSIX)
    find_library(_POSIX_PTHREAD pthread)
    if(NOT _POSIX_PTHREAD)
        message(FATAL_ERROR "libpthread not found!")
    else()
        set(YNK_LIB_PTHREAD ${pthread})
    endif()
endif()

# ==============================================================================
# TOOLING SETUP
# ==============================================================================

list(APPEND CMAKE_ASM_FLAGS "--config ${ycm_root}/clang/dev-asflags.txt -fcolor-diagnostics")
list(APPEND CMAKE_C_FLAGS "--config ${ycm_root}/clang/dev-cflags.txt -fcolor-diagnostics")
list(APPEND CMAKE_CXX_FLAGS "--config ${ycm_root}/clang/dev-cxxflags.txt -fcolor-diagnostics")
# list(APPEND CMAKE_SHARED_LINKER_FLAGS "--config ${ycm_root}/clang/dev-ldflags.txt -fcolor-diagnostics")

include_directories(${ycm_root})

# ==============================================================================
# SOURCE FILES
# ==============================================================================

# LIBRARY (libynked)

set(ycm_libynk_sources "${ycm_root}/ynk/App.cc" "${ycm_root}/ynk/Bootstrap.c" "${ycm_root}/ynk/Bootstrap.cc" "${ycm_root}/ynk/Backtrace/Backtrace.cc" "${ycm_root}/ynk/Fmt/Fmt.cc" "${ycm_root}/ynk/GL/Program.cc" "${ycm_root}/ynk/GL/Shader.cc" "${ycm_root}/ynk/Num/Integers.cc" "${ycm_root}/ynk/Panic/Panic.cc" "${ycm_root}/ynk/Panic/DefaultPanicHook.cc" "${ycm_root}/ynk/String/String.cc" "${ycm_root}/ynk/UX/Color.cc" "${ycm_root}/ynk/UX/ColorAdapters.cc")

if(USE_X86_64_SVR4_UTF8_STRLEN)
    list(APPEND ycm_libynk_sources "${ycm_root}/ynk/String/utf8_strlen.x86_64-svr4.s")
else()
    list(APPEND ycm_libynk_sources "${ycm_root}/ynk/String/utf8_strlen.cc")
endif()
if(YNK_LIB_PTHREAD)
    list(APPEND ycm_libynk_sources "${ycm_root}/ynk/sys/any-posix/sync/condvar.cc" "${ycm_root}/ynk/sys/any-posix/sync/mutex.cc" "${ycm_root}/ynk/sys/any-posix/sync/rwlock.cc")
endif()
if(YNK_PLATFORM_APPLE)
    list(APPEND ycm_libynk_sources "${ycm_root}/ynk/sys/macos/sync/threadid.cc")
endif()

# EXECUTABLE (ynked)

set(ycm_ynkart_sources "${ycm_root}/art/Art.cc" "${ycm_root}/art/Notes.cc" "${ycm_root}/art/GL/Errors.cc" "${ycm_root}/art/GL/Hooks.cc" "${ycm_root}/art/GL/Setup.cc" "${ycm_root}/art/GLSimulationApp/GLSimulationApp.cc" "${ycm_root}/art/Model/Brush.cc" "${ycm_root}/art/Model/PaperLayer.cc" "${ycm_root}/art/Model/TintLayer.cc" "${ycm_root}/art/Model/WaterLayer.cc" "${ycm_root}/art/PushRelabel/PushRelabel.cc" "${ycm_root}/art/TUIApp/TUIApp.cc")

# ==============================================================================
# DEALING WITH TOOLING
# ==============================================================================

find_program(_CCACHE_FOUND ccache)
if(_CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

# ==============================================================================
# FINAL BUILD
# ==============================================================================

add_library(ynked SHARED ${ycm_libynk_sources})
target_link_libraries(ynked glfw glad c c++ ${YNK_LIB_PTHREAD} ${YNK_LIB_OPENGL})

add_executable(artisan ${ycm_ynkart_sources})
target_link_libraries(artisan ynked glfw glad c c++ ${YNK_LIB_PTHREAD} ${YNK_LIB_OPENGL})
