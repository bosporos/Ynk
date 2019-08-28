//
// file art/Art.hh
// author Maximilien M. Cura
//

#ifndef __ART
#define __ART

/*
 * Grab everything we need for working with OpenGL + GLFW
 */
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <Ynk/App.hh> /* Ynk::App::*, YNK_APP, YNK_LAUNCH_APP, etc. */
// Sized integer wrapping types.
#include <Ynk/Num/Integers.hh> /* i8, i16, i32, i64, u8, u16, u32, u64, isize, usize */
// For working with strings
#include <Ynk/Fmt/Fmt.hh> /* print, println, print_err, println_err, Fmt::Formatter<T>, Fmt::FormatContext, Fmt::format() */
// For working with colors
#include <Ynk/UX/Color.hh> /* RGBA, hsva */
// For working with coordinates
#include <Ynk/Geometry/Space.hh> /* Vector<>, Space<> */

// For working with OpenGL shader programs
#include <Ynk/GL/Program.hh>

namespace Art {

    //! Event callbacks from the UI system
    namespace GLFWHooks {

        //! Called in the event that errors occur with GLFW.
        void OnErrorFn (int, const char *);

        //! When a key is pressed. Currently unused.
        void OnKeyFn (GLFWwindow *, int, int, int, int);

        //! When the window is resized. Currently not applicable.
        void OnWindowSizeFn (GLFWwindow *, int, int);
    }

    // ========================================================================
    // Functions dealing with the general OpenGL windowing + context creation
    // ========================================================================

    //! Creates the OpenGL context window
    int GLApp_Init (Ynk::App::Stub *);
    //! Tears it back down again
    void GLApp_Teardown ();

    // ========================================================================
    // Functions dealing with the TUI debugging interface (`ynked tui`)
    // ========================================================================

    //! Runs the TUI simulation in-terminal
    void TUIApp ();

    // ========================================================================
    // Functions dealing with the OpenGL watercolor simulation app (`ynked gl-sim`)
    // ========================================================================

    //! Takes care of entry/exit
    void GLSimulationApp (Ynk::App::Stub *);

    //! Runs the actual processing
    void GLSimulationApp_Render ();

    // ========================================================================
    // Coordinate spaces
    // ========================================================================

    //! 3-dimensional double-precision Cartesian coordinate system. Def'd in Art.cc
    extern Ynk::Space<3, double> * d3;
    //! 2-dimensional 64-bit signed integer Cartesian coordinate system. Def'd in Art.cc
    extern Ynk::Space<2, Ynk::i64> * iq2;

    // ========================================================================
    // Link-level global variables
    // ========================================================================

    //! Size of the GLFW window.
    extern Ynk::Vec<2, Ynk::i64> window_size;

    //! The GLFW window
    extern GLFWwindow * window;

    //! The OpenGL program (vertex + fragment shaders from shaders/core-*-shader.glsl)
    extern Ynk::GL::Program * program;

    // ========================================================================
    // Diagnostic/debugging functions
    // ========================================================================

    //! Notification. Prints to the console; impl. in Notes.cc
    void Notify (Ynk::String);
    //! Warning. Prints to the console; impl. in Notes.cc
    void Warn (Ynk::String);
    //! Error. Prints to the console; impl. in Notes.cc
    void Error (Ynk::String);
    //! Notification base; all Art-level notifications should go through this; impl. in Notes.cc
    void _notification_base (Ynk::String);

    //! Dumps the OpenGL error code values, because OpenGL likes to dump error code values
    //! to the console raw, without translating them back into something human-readable.
    //!
    //! So, we print out all the error codes, and then cross-reference, enabling faster diagnostics.
    //! Impl. in GL/Errors.cc
    void _gl_dump_error_codes ();

    //! Purges the OpenGL error stack to the console.
    //! Impl. in GL/Errors.cc
    void _gl_purge_error_stack (Ynk::String);
}

#endif /* !@__ART */
