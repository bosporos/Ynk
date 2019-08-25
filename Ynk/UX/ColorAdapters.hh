//
// file ynk/ux/ColorAdapters.hh
// author Maximilien M. Cura
//

#ifndef __YNK_UX_COLOR_ADAPTERS
#define __YNK_UX_COLOR_ADAPTERS

#include <Ynk/String/String.hh>
#include <Ynk/Fmt/Fmt.hh>
#include <Ynk/UX/Color.hh>

namespace Ynk::UX {
    template <unsigned W>
    struct GLSetColorAdapter;

    template <>
    struct GLSetColorAdapter<3>
    {
        void SetColor (RGBA c) { glColor3ub (c.irgba.red, c.irgba.green, c.irgba.blue); }
    };

    template <>
    struct GLSetColorAdapter<4>
    {
        void SetColor (RGBA c) { glColor4ub (c.irgba.red, c.irgba.green, c.irgba.blue, c.irgba.alpha); }
    };

    template <unsigned W = 4>
    void GLSetColor (RGBA c)
    {
        GLSetColorAdapter<W>::SetColor (c);
    }

    String ANSI (RGBA c)
    {
        return Fmt::format ("\x1b[38;2;{};{};{}m", c.irgba.red, c.irgba.green, c.irgba.blue);
    }

    String ANSIRst ()
    {
        return String ("\x1b[0m");
    }

}

#endif /* !@__YNK_UX_COLOR_ADAPTERS */
