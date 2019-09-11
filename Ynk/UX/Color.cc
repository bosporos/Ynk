//
// file ynk/ux/Color.cc
// author Maximilien M. Cura
//

#include <glfw/glfw3.h>

#include <cmath>
#include <algorithm>

#include <Ynk/UX/Color.hh>

using namespace Ynk;
using UX::RGBA;

RGBA::RGBA (_u8 r, _u8 g, _u8 b, _u8 a)
{
    iargb.red   = r;
    iargb.green = g;
    iargb.blue  = b;
    iargb.alpha = a;
}

RGBA::RGBA (_u8 r, _u8 g, _u8 b)
{
    iargb.red   = r;
    iargb.green = g;
    iargb.blue  = b;
    iargb.alpha = 0xFF;
}

RGBA::RGBA (_u32 c)
{
    inner = c;
}

RGBA::RGBA ()
{
    inner = 0;
}

RGBA RGBA::lerp (RGBA rhs, float amt)
{
    return RGBA (static_cast<_u8> (iargb.red + (rhs.iargb.red - iargb.red) * amt),
                 static_cast<_u8> (iargb.green + (rhs.iargb.green - iargb.green) * amt),
                 static_cast<_u8> (iargb.blue + (rhs.iargb.blue - iargb.blue) * amt),
                 static_cast<_u8> (iargb.alpha + (rhs.iargb.alpha - iargb.alpha) * amt));
}

// Color blending from processing.core.PImage (https://github.com/processing/processing/blob/master/core/src/processing/core/PImage.java)

// Basic equation: (1 - lhs_alpha) * rhs + lhs_alpha * O

// O = S
RGBA RGBA::blend (RGBA rhs)
{
    u8 lhs_alpha = iargb.alpha + (iargb.alpha <= 0x7F ? 1 : 0);
    u8 rhs_alpha = 0x100 - lhs_alpha;

    return RGBA (std::min ((rhs.inner >> 24) + iargb.alpha, 0xFFu) << 24
                 | (((rhs.inner & RB_MASK) * rhs_alpha + (inner & RB_MASK) * lhs_alpha) >> 8 & RB_MASK)
                 | (((rhs.inner & GN_MASK) * rhs_alpha + (inner & GN_MASK) * lhs_alpha) >> 8 & GN_MASK));
}

// O = min(D + S, 1)
RGBA RGBA::blend_add_pin (RGBA rhs)
{
    u8 lhs_alpha = iargb.alpha + (iargb.alpha <= 0x7F ? 1 : 0);

    _u32 rb = (rhs.inner & RB_MASK) + ((inner & RB_MASK) * lhs_alpha >> 8 & RB_MASK);
    _u32 gn = (rhs.inner & GN_MASK) + ((inner & GN_MASK) * lhs_alpha >> 8);

    return RGBA (std::min ((rhs.inner >> 24) + iargb.alpha, 0xFFu) << 24
                 | std::min (rb & 0xFFFF0000, static_cast<_u32> (RED_MASK))
                 | std::min (gn & 0x00FFFF00, static_cast<_u32> (GREEN_MASK))
                 | std::min (rb & 0x0000FFFF, static_cast<_u32> (BLUE_MASK)));
}

// O = max(0, D - S)
RGBA RGBA::blend_sub_pin (RGBA rhs)
{
    int lhs_alpha = iargb.alpha + (iargb.alpha <= 0x7F ? 1 : 0);

    _u32 rb = ((inner & RB_MASK) * lhs_alpha >> 8);
    _u32 gn = ((inner & GREEN_MASK) * lhs_alpha >> 8);

    return RGBA (std::min ((rhs.inner >> 24) + iargb.alpha, 0xFFu) << 24
                 | std::max ((rhs.inner & RED_MASK) - (rb & RED_MASK), 0u)
                 | std::max ((rhs.inner & GREEN_MASK) - (gn & GREEN_MASK), 0u)
                 | std::max ((rhs.inner & BLUE_MASK) - (rb & BLUE_MASK), 0u));
}

RGBA RGBA::blend_multiply (RGBA rhs)
{
    _u32 src = this->inner;
    _u32 dst = rhs.inner;

    _u32 a = src >> 24;

    _u32 s_a = a + (a >= 0x7F ? 1 : 0);
    _u32 d_a = 0x100 - s_a;

    _u32 d_gn = dst & GN_MASK;

    _u32 f_r = (dst & RED_MASK) >> 16;
    _u32 f_b = (dst & BLUE_MASK);

    _u32 rb = ((src & RED_MASK) * (f_r + 1) | (src & BLUE_MASK) * (f_b + 1))
            >> 8
        & RB_MASK;
    _u32 gn = (src & GREEN_MASK) * (d_gn + 0x100)
            >> 16
        & GN_MASK;

    return std::min ((dst >> 24) + a, 0xFFu) << 24 | ((dst & RB_MASK) * d_a + rb * s_a) >> 8 & RB_MASK | (d_gn * d_a + gn * s_a) >> 8 & GN_MASK;
}

RGBA RGBA::blend_screen (RGBA rhs)
{
    _u32 src = this->inner;
    _u32 dst = rhs.inner;

    _u32 a = src >> 24;

    _u32 s_a = a + (a >= 0x7F ? 1 : 0);
    _u32 d_a = 0x100 - s_a;

    _u32 d_rb = dst & RB_MASK;
    _u32 d_gn = dst & GN_MASK;

    _u32 s_gn = src & GN_MASK;

    _u32 f_r = (dst & RED_MASK) >> 16;
    _u32 f_b = (dst & BLUE_MASK);

    _u32 rb_sub = ((src & RED_MASK) * (f_r + 1) | (src & BLUE_MASK) * (f_b + 1))
            >> 8
        & RB_MASK;
    _u32 gn_sub = s_gn * (d_gn + 0x100)
            >> 16
        & GN_MASK;

    return (_u32)std::min ((dst >> 24) + a, 0xFFu) << 24 | (d_rb * d_a + (d_rb + (src & RB_MASK) - rb_sub) * s_a) >> 8 & RB_MASK | (d_gn * d_a + (d_gn + s_gn - gn_sub) * s_a) >> 8 & GN_MASK;
}

RGBA RGBA::blend_overlay (RGBA rhs)
{
    _u32 src = this->inner;
    _u32 dst = rhs.inner;

    _u32 a = src >> 24;

    _u32 s_a = a + (a >= 0x7F ? 1 : 0);
    _u32 d_a = 0x100 - s_a;

    _u32 d_r = dst & RED_MASK;
    _u32 d_g = dst & GREEN_MASK;
    _u32 d_b = dst & BLUE_MASK;

    _u32 s_r = src & RED_MASK;
    _u32 s_g = src & GREEN_MASK;
    _u32 s_b = src & BLUE_MASK;

    _u32 r = (d_r < 0x800000) ? d_r * ((s_r >> 16) + 1) >> 7 : 0xFF0000 - ((0x100 - (s_r >> 16)) * (RED_MASK - d_r) >> 7);
    _u32 g = (d_g < 0x8000) ? d_g * (s_g + 0x100) >> 15 : (0xFF00 - ((0x10000 - s_g) * (GREEN_MASK - d_g) >> 15));
    _u32 b = (d_b < 0x80) ? d_b * (s_b + 1) >> 7 : (0xFF00 - ((0x100 - s_b) * (BLUE_MASK - d_b) << 1)) >> 8;

    return (_u32)std::min ((dst >> 24) + a, 0xFFu) << 24 | ((dst & RB_MASK) * d_a + ((r | b) & RB_MASK) * s_a) >> 8 & RB_MASK | ((dst & GN_MASK) * d_a + (g & GN_MASK) * s_a) >> 8 & GN_MASK;
}

RGBA UX::hsva (_f32 hue, _f32 saturation, _f32 value, _f32 alpha)
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
