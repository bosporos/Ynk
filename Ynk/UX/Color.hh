//
// file ynk/ux/Color.hh
// author Maximilien M. Cura
//

#ifndef __YNK_UX_COLOR
#define __YNK_UX_COLOR

#include <Ynk/Num/Integers.hh>
#include <Ynk/Num/NativeFloats.hh>

// Sometimes, we can take the R & B channels separately (i.e. when multiplying by u8's)
// because the channel-widths are 8, and thus the operation can't spill the channel
// because 2^8 * 2^8 = 2^16, limiting the upstream channel multiplication spill possibility
#define RB_MASK 0x00FF00FF
#define GN_MASK 0x0000FF00
#define ALPHA_MASK 0xFF000000
#define RED_MASK 0x00FF0000
#define GREEN_MASK 0x0000FF00
#define BLUE_MASK 0x000000FF

namespace Ynk::UX {
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

        RGBA (_u8, _u8, _u8);
        RGBA (_u8, _u8, _u8, _u8);
        RGBA (_u32);

        RGBA lerp (RGBA, float);
        RGBA blend (RGBA);
        RGBA blend_add_pin (RGBA);
        RGBA blend_sub_pin (RGBA);

        // Todo: add more blend modes
    };

    RGBA hsva (_f32, _f32, _f32, _f32);

}

#endif /* !@__ART_COLOR */
