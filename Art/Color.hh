//
// file Art/Color.hh
// author Maximilien M. Cura
//

#ifndef __ART_COLOR
#define __ART_COLOR

#include <Ynk/Num/Integers.hh>
#include <Ynk/Num/NativeFloats.hh>

#include <glfw/glfw3.h>

#include <cmath>

// Sometimes, we can take the R & B channels separately (i.e. when multiplying by u8's)
// because the channel-widths are 8, and thus the operation can't spill the channel
// because 2^8 * 2^8 = 2^16, limiting the upstream channel multiplication spill possibility
#define RB_MASK 0x00FF00FF
#define GN_MASK 0x0000FF00
#define ALPHA_MASK 0xFF000000
#define RED_MASK 0x00FF0000
#define GREEN_MASK 0x0000FF00
#define BLUE_MASK 0x000000FF

using namespace Ynk;

namespace Art {
    struct RGBA
    {
        struct InternalRGBA
        {
            _u32 alpha : 8;
            _u32 red : 8;
            _u32 green : 8;
            _u32 blue : 8;
        };
        union {
            InternalRGBA irgba;
            _u32 inner;
        };

        RGBA (_u8 r, _u8 g, _u8 b, _u8 a)
        {
            irgba.red   = r;
            irgba.green = g;
            irgba.blue  = b;
            irgba.alpha = a;
        }

        RGBA (_u32 c)
        {
            inner = c;
        }

        void gl_color3 ()
        {
            glColor3ub (irgba.red, irgba.green, irgba.blue);
        }

        void gl_color4 ()
        {
            glColor4ub (irgba.red, irgba.green, irgba.blue, irgba.alpha);
        }

        RGBA lerp (RGBA rhs, float amt)
        {
            return RGBA (static_cast<_u8> (irgba.red + (rhs.irgba.red - irgba.red) * amt),
                         static_cast<_u8> (irgba.green + (rhs.irgba.green - irgba.green) * amt),
                         static_cast<_u8> (irgba.blue + (rhs.irgba.blue - irgba.blue) * amt),
                         static_cast<_u8> (irgba.alpha + (rhs.irgba.alpha - irgba.alpha) * amt));
        }

        // Color blending from processing.core.PImage (https://github.com/processing/processing/blob/master/core/src/processing/core/PImage.java)

        // Basic equation: (1 - lhs_alpha) * rhs + lhs_alpha * O

        // O = S
        RGBA blend (RGBA rhs)
        {
            u8 lhs_alpha = irgba.alpha + (irgba.alpha >= 0x7F ? 1 : 0);
            u8 rhs_alpha = 0x100 - lhs_alpha;

            return RGBA (std::min ((rhs.inner >> 24) + irgba.alpha, 0xFFu) << 24
                         | (((rhs.inner & RB_MASK) * rhs_alpha + (inner & RB_MASK) * lhs_alpha) >> 8 & RB_MASK)
                         | (((rhs.inner & GN_MASK) * rhs_alpha + (inner & GN_MASK) * lhs_alpha) >> 8 & GN_MASK));
        }

        // O = min(D + S, 1)
        RGBA blend_add_pin (RGBA rhs)
        {
            u8 lhs_alpha = irgba.alpha + (irgba.alpha >= 0x7F ? 1 : 0);

            _u32 rb = (rhs.inner & RB_MASK) + ((inner & RB_MASK) * lhs_alpha >> 8 & RB_MASK);
            _u32 gn = (rhs.inner & GN_MASK) + ((inner & GN_MASK) * lhs_alpha >> 8);

            return RGBA (std::min ((rhs.inner >> 24) + irgba.alpha, 0xFFu) << 24
                         | std::min (rb & 0xFFFF0000, static_cast<_u32> (RED_MASK))
                         | std::min (gn & 0x00FFFF00, static_cast<_u32> (GREEN_MASK))
                         | std::min (rb & 0x0000FFFF, static_cast<_u32> (BLUE_MASK)));
        }

        // O = max(0, D - S)
        RGBA blend_sub_pin (RGBA rhs)
        {
            int lhs_alpha = irgba.alpha + (irgba.alpha >= 0x7F ? 1 : 0);

            _u32 rb = ((inner & RB_MASK) * lhs_alpha >> 8);
            _u32 gn = ((inner & GREEN_MASK) * lhs_alpha >> 8);

            return RGBA (std::min ((rhs.inner >> 24) + irgba.alpha, 0xFFu) << 24
                         | std::max ((rhs.inner & RED_MASK) - (rb & RED_MASK), 0u)
                         | std::max ((rhs.inner & GREEN_MASK) - (gn & GREEN_MASK), 0u)
                         | std::max ((rhs.inner & BLUE_MASK) - (rb & BLUE_MASK), 0u));
        }

        // Todo: add more blend modes
    };

    RGBA hsva (_f32 hue, _f32 saturation, _f32 value, _f32 alpha)
    {
        _f32 kr = std::fmod (5. + (hue / 6.), 6.);
        _f32 kg = std::fmod (3. + (hue / 6.), 6.);
        _f32 kb = std::fmod (1. + (hue / 6.), 6.);
        _f32 fr = value - value * saturation * std::max (std::min (std::min (kr, 4 - kr), 1.0f), 0.0f);
        _f32 fg = value - value * saturation * std::max (std::min (std::min (kg, 4 - kg), 1.0f), 0.0f);
        _f32 fb = value - value * saturation * std::max (std::min (std::min (kb, 4 - kb), 1.0f), 0.0f);
        return RGBA (
            static_cast<_u8> (static_cast<_f32> (0xFF) * fr),
            static_cast<_u8> (static_cast<_f32> (0xFF) * fg),
            static_cast<_u8> (static_cast<_f32> (0xFF) * fb),
            static_cast<_u8> (static_cast<_f32> (0xFF) * alpha));
    }
}

#endif /* !@__ART_COLOR */
