//
// file ynk/fmt/Fmt.h
// author Maximilien M. Cura
//

#ifndef __YNK_FMT_FMT
#define __YNK_FMT_FMT

#include <Ynk/num/Integers.h>
#include <Ynk/fmt/Dispatcher.h>
#include <Ynk/lang/Move.h>

#include <Ynk/string/String.h>

namespace Ynk::Fmt {

    // Based on Sasha Goldshtein's safe_printf (https://blogs.microsoft.co.il/sasha/2015/01/29/compile-time-runtime-safe-replacement-printf/)

    struct ConstexprString
    {
        const unsigned long length;
        const char * inner;

        template <unsigned long N>
        constexpr ConstexprString (char const (&str)[N])
            : length { N }
            , inner { str }
        {}

        constexpr unsigned long count_fmt_symbols ()
        {
            unsigned long count = 0;
            for (unsigned long i = 0; i < this->length; i++) {
                if (this->inner[i] == '%') {
                    if (i != this->length - 1 && this->inner[i + 1] == '%') {
                        // Skip
                        i++;
                    } else {
                        count++;
                    }
                }
            }

            return count;
        }
    };
}

#endif /* !@__YNK_FMT_FMT */
