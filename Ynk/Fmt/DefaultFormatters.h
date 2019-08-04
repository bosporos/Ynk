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
            int base     = 10;
            bool prepend = false;
            bool upper   = false;
            bool pad     = false;
            switch (ctx.number_fmt) {
                case NumberFormat::Binary:
                    base = 2;
                    pad  = true;
                    break;
                case NumberFormat::Octal:
                    base = 8;
                    pad  = true;
                    break;
                case NumberFormat::UpperHex:
                    upper = true;
                case NumberFormat::LowerHex:
                    base    = 16;
                    pad     = true;
                    prepend = true;
                    break;
                case NumberFormat::Decimal:
                case NumberFormat::Default:
                    base = 10;
                    break;
            }
            if (prepend)
                ctx.write_str (u8"0x"_y);
            ctx.write_int (obj, base, pad, upper);
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
            bool pad     = false;
            switch (ctx.number_fmt) {
                case NumberFormat::Binary:
                    base = 2;
                    pad  = true;
                    break;
                case NumberFormat::Octal:
                    base = 8;
                    pad  = true;
                    break;
                case NumberFormat::UpperHex:
                    upper = true;
                case NumberFormat::LowerHex:
                    base    = 16;
                    pad     = true;
                    prepend = true;
                    break;
                case NumberFormat::Decimal:
                case NumberFormat::Default:
                    base = 10;
                    break;
            }
            if (prepend)
                ctx.write_int (obj, base, pad, upper, "0x"_y);
            else
                ctx.write_int (obj, base, pad, upper);
        }
    };
}

#endif /* !@__YNK_FMT_FMTD */
