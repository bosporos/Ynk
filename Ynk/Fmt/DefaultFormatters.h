//
// file Ynk/fmt/Fmtd.h
// author Maximilien M. Cura
//

#ifndef __YNK_FMT_FMTD
#define __YNK_FMT_FMTD

#include <Ynk/Fmt/Fmt.h>

namespace Ynk::Fmt {
    template <>
    struct Formatter<String>
    {
        static void format (String obj, FormatContext ctx)
        {
            ctx.write_str (obj);
        }
    };

    template <unsigned S, class U>
    struct Formatter<uint_impl<S, U>>
    {
        typedef uint_impl<S, U> resultant_type;

        static void format (resultant_type obj, FormatContext ctx)
        {
            ctx.write_int (obj, 16, true);
        }
    };

    template <unsigned S, class U, class V>
    struct Formatter<int_impl<S, U, V>>
    {
        typedef int_impl<S, U, V> resultant_type;

        static void format (resultant_type obj, FormatContext ctx)
        {
            int base     = 10;
            bool prepend = false;
            bool upper   = false;
            switch (ctx.number_fmt) {
                case NumberFormat::Binary: base = 2; break;
                case NumberFormat::Octal: base = 8; break;
                case NumberFormat::UpperHex0x:
                    prepend = true;
                case NumberFormat::UpperHex:
                    base  = 16;
                    upper = true;
                    break;
                case NumberFormat::LowerHex0x:
                    prepend = true;
                case NumberFormat::LowerHex:
                    base  = 16;
                    upper = false;
                    break;
            }
            if (prepend)
                ctx.write_str (u8"0x"_y);
            ctx.write_int (obj, base, true, upper);
        }
    };
}

#endif /* !@__YNK_FMT_FMTD */
