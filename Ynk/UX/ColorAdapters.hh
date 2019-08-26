//
// file ynk/ux/ColorAdapters.hh
// author Maximilien M. Cura
//

#ifndef __YNK_UX_COLOR_ADAPTERS
#define __YNK_UX_COLOR_ADAPTERS

// #if !__has_include(<linmath.h>)
// #    include "linmath.h"
// #endif
#include <glad/glad.h>
#include <OpenGL/glu.h>
#include <glfw/glfw3.h>

#include <Ynk/String/String.hh>
#include <Ynk/Fmt/Fmt.hh>
#include <Ynk/UX/Color.hh>

namespace Ynk::UX {
    template <unsigned W>
    struct GLSetColorAdapter;

    template <>
    struct GLSetColorAdapter<3>
    {
        void SetColor (RGBA c) { glColor3ub (c.iargb.red, c.iargb.green, c.iargb.blue); }
    };

    template <>
    struct GLSetColorAdapter<4>
    {
        void SetColor (RGBA c) { glColor4ub (c.iargb.red, c.iargb.green, c.iargb.blue, c.iargb.alpha); }
    };

    template <unsigned W = 4>
    void GLSetColor (RGBA c)
    {
        GLSetColorAdapter<W>::SetColor (c);
    }

    String ANSI (RGBA c);
    String ANSIRst ();
}

#endif /* !@__YNK_UX_COLOR_ADAPTERS */
