//
// file art/Art.hh
// author Maximilien M. Cura
//

#ifndef __ART
#define __ART

// #if !__has_include(<linmath.h>)
// #    include "linmath.h"
// #endif
#include <glad/glad.h>
#include <OpenGL/glu.h>
#include <glfw/glfw3.h>

#include <Ynk/App.hh>   // Ynk::App::*, YNK_APP, YNK_LAUNCH_APP, etc.
#include <Ynk/Num/Integers.hh>
#include <Ynk/Fmt/Fmt.hh>
#include <Ynk/UX/Color.hh>
#include <Ynk/Geometry/Space.hh>

#include <Ynk/GL/Program.hh>

namespace Art {
    namespace GLFWHooks {
        void OnErrorFn (int, const char *);
        void OnKeyFn (GLFWwindow *, int, int, int, int);
        void OnWindowSizeFn (GLFWwindow *, int, int);
    }

    int Init (int, char **, Ynk::App::Stub *);
    void Render ();
    void Teardown ();

    extern Ynk::Space<3, double> * d3;
    extern Ynk::Space<2, Ynk::i64> * iq2;

    extern Ynk::Vec<2, Ynk::i64> window_size;

    extern GLFWwindow * window;
    extern Ynk::GL::Program * program;

    void Notify (Ynk::String);
    void Warn (Ynk::String);
    void Error (Ynk::String);
    void _notification_base (Ynk::String);

    void _gl_dump_error_codes ();
    void _gl_purge_error_stack (Ynk::String);
}

#endif /* !@__ART */
