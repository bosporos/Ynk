//
// file Ynk/fmt/Fmtd.hh
// author Maximilien M. Cura
//

#ifndef __YNK_FMT_FMTD
#define __YNK_FMT_FMTD

#include <Ynk/Fmt/Fmt.hh>
#include <Ynk/Num/NativeFloats.hh>
#include <stdio.h>

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

#define _Ynk_DefaultFmt_Helper(T, F)                  \
    template <>                                       \
    struct Formatter<T>                               \
    {                                                 \
        static void format (T obj, FormatContext ctx) \
        {                                             \
            char * asprintf_str;                      \
            asprintf (&asprintf_str, F, obj);         \
            String ynk_str (asprintf_str);            \
            std::free (asprintf_str);                 \
            ctx.write_str (ynk_str);                  \
        }                                             \
    }

    _Ynk_DefaultFmt_Helper (_u8, "%hhu");
    _Ynk_DefaultFmt_Helper (_u16, "%hu");
    _Ynk_DefaultFmt_Helper (_u32, "%u");
    _Ynk_DefaultFmt_Helper (_u64, "%llu");
    _Ynk_DefaultFmt_Helper (_usize, "%zu");

    _Ynk_DefaultFmt_Helper (_i8, "%hhi");
    _Ynk_DefaultFmt_Helper (_i16, "%hi");
    _Ynk_DefaultFmt_Helper (_i32, "%i");
    _Ynk_DefaultFmt_Helper (_i64, "%lli");
    _Ynk_DefaultFmt_Helper (_isize, "%zi");

    _Ynk_DefaultFmt_Helper (_f32, "%f");
    _Ynk_DefaultFmt_Helper (_f64, "%lf");

    template <>
    struct Formatter<const char *>
    {
        static void format (const char * obj, FormatContext ctx) { ctx.write_str (String (obj)); }
    };
}

#endif /* !@__YNK_FMT_FMTD */
