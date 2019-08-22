//
// file ynk/fmt/Fmt.hh
// author Maximilien M. Cura
//

#ifndef __YNK_FMT_FMT
#define __YNK_FMT_FMT

#include <Ynk/Num/Integers.hh>
#include <Ynk/Backtrace/Backtrace.hh>
#include <Ynk/String/String.hh>

#include <Ynk/Option/Option.hh>

#include <Ynk/Lang/Move.hh>
#include <Ynk/Lang/Forward.hh>

#include <Ynk/Utility.hh>

// #include <tgmath.h>
#include <cmath>
#include <inttypes.h>

namespace Ynk::Fmt {

    // (Structurally) Based on Sasha Goldshtein's safe_printf (https://blogs.microsoft.co.il/sasha/2015/01/29/compile-time-runtime-safe-replacement-printf/)
    // In terms of concept, based on Rust's std::fmt module

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
                if (this->inner[i] == '{') {
                    if (i != this->length - 1) {
                        if (this->inner[i + 1] == '{') {
                            // Skip
                            i++;
                        } else if (i != this->length - 2) {
                            count++;
                        }
                    }
                }
            }
            return count;
        }
    };

    template <class... T>
    constexpr unsigned long number_ (T &&...)
    {
        return sizeof...(T);
    }

    // format(String fmt_str, T... arguments)
    //     loop:
    //         format_slide_to_carry(fmt_str, carry);
    //         format_carry_head<H>(head, ctx, carry)
    //             FormatDispatcher<H>::dispatch(head, ctx)
    //                 Formatter<H>::format(head, ctx)

    enum class NumberFormat {
        Binary,
        Octal,
        Decimal,
        UpperHex,
        LowerHex,
        Default,
    };

    struct FormatContext
    {
        // Width
        // Padding (L/R)
        _u32 sign_plus : 1;
        _u32 sign_minus : 1;
        _u32 alternate : 1;

        usize width;
        usize pad_left;
        // width + pad_left -> pad_right
        usize precision;

        NumberFormat number_fmt;

        String & carry;

        FormatContext (String spec, String & carry)
            : sign_plus { false }
            , sign_minus { false }
            , alternate { false }
            , width { 0 }
            , pad_left { 0 }
            , precision { 0 }
            , number_fmt { NumberFormat::Default }
            , carry { carry }
        {
            if (spec.length ()) {
                if (spec.inner[0] != ':') {
                    switch (spec.inner[0]) {
                        case 'b': this->number_fmt = NumberFormat::Binary; break;
                        case 'o': this->number_fmt = NumberFormat::Octal; break;
                        case 'd': this->number_fmt = NumberFormat::Decimal; break;
                        case 'X': this->number_fmt = NumberFormat::UpperHex; break;
                        case 'x': this->number_fmt = NumberFormat::LowerHex; break;
                        default:
                            std::fprintf (stderr, "FormatContext: Unrecognized number format: %c\n", (char)spec.inner[0]);
                            Backtrace::print_backtrace ();
                            std::abort ();
                    }
                } else {
                }
            }
        }

        void write_char (char8_t chr)
        {
            this->carry.inner.insert (this->carry.inner.end () - 1, chr);
        }

        void write_str (String str)
        {
            this->carry.push (str);
        }

        template <unsigned S, class U, class V>
        void write_int (int_impl<S, U, V> ui, u8 base, bool pad, bool hexupper, String prefix)
        {
            if (ui < 0) {
                this->write_char (u8'-');
                ui.inner_ = -ui.inner_;
            }
            this->write_str (prefix);
            this->write_int (static_cast<uint_impl<S, V>> (ui), base, pad, hexupper);
        }

        template <unsigned S, class U, class V>
        void write_int (int_impl<S, U, V> ui, u8 base, bool pad, bool hexupper)
        {
            if (ui < 0) {
                this->write_char (u8'-');
                ui.inner_ = -ui.inner_;
            }
            this->write_int (static_cast<uint_impl<S, V>> (ui), base, pad, hexupper);
        }

        template <unsigned S, class U>
        void write_int (uint_impl<S, U> ui, u8 base, bool pad, bool hexupper)
        {
            if (base == 2) {
                String str (static_cast<usize> (S));
                const U highest_bit = ~static_cast<U> (0) << (S - 1);
                U iterating         = ui.inner_;
                bool in_padding     = true;
                for (u8 i = 0; i < S; i++) {
                    U hit = highest_bit & iterating;
                    if (hit && in_padding)
                        in_padding = false;
                    if (hit || (pad || !in_padding)) {
                        str.inner[i] = hit ? u8'1' : u8'0';
                    }
                    iterating <<= 1;
                }
                this->write_str (str);
            } else if (base == 8) {
                const usize pinches = S / 3 + !!(S % 3);
                const u8 cinches    = S % 3;
                String str (pinches);
                const U highest_pinch = ~static_cast<U> (0) << (S - 1);
                U iterating           = ui.inner_;
                bool in_padding       = true;
                const U cinch         = iterating & (~static_cast<U> (0) << (S - cinches));
                if (cinch || in_padding) {
                    in_padding   = false;
                    str.inner[0] = u8'0' + (cinch >> (S - cinches));
                }
                iterating <<= cinches;
                for (u8 i = 1; i < pinches; i++) {
                    u8 pinch = ((highest_pinch & iterating) >> (S - 3));
                    if (pinch && in_padding)
                        in_padding = false;
                    if (pinch || (pad || !in_padding)) {
                        str.inner[i] = u8'0' + pinch;
                    }
                    iterating <<= 3;
                }
                this->write_str (str);
            } else if (base == 10) {
                usize p10 = static_cast<U> (
                    pad ? std::pow (
                        static_cast<U> (10),
                        static_cast<_u8> (
                            std::log10 (uint_impl<S, U>::max_value ().inner_)))
                        : std::pow (
                            static_cast<U> (10),
                            static_cast<_u8> (
                                std::ceil (std::log10 (ui.inner_)))));
                String str (p10.inner_ == 1 ? static_cast<usize> (1) : static_cast<usize> (std::log10 (p10.inner_)));
                U iterating     = ui.inner_;
                bool in_padding = true;
                usize i         = 0;
                if (iterating != 0) {
                    while (iterating) {
                        u8 n = static_cast<u8> (iterating / p10);
                        if (n && in_padding)
                            in_padding = false;
                        if (n || (pad || !in_padding)) {
                            str.inner[i++] = u8'0' + n;
                            iterating -= p10 * n;
                        }
                        p10 /= 10;
                    }
                } else {
                    this->write_char (u8'0');
                }
                this->write_str (str);
            } else if (base == 16) {
                const usize nybbles = S >> 2;
                String str (nybbles);
                const U highest_nybble = ~static_cast<U> (0) << (S - 4);
                U iterating            = ui.inner_;
                bool in_padding        = true;
                for (u8 i = 0; i < nybbles; i++) {
                    u8 nybble = ((highest_nybble & iterating) >> (S - 4));
                    if (nybble & in_padding)
                        in_padding = false;
                    if (nybble || (pad || !in_padding)) {
                        str.inner[i] = nybble < 0xA
                            ? u8'0' + nybble
                            /* Weird hack with UTF-8 or ASCII */
                            : (hexupper ? (u8'7' + nybble) : (u8'W' + nybble));
                    }
                    iterating <<= 4;
                }
                this->write_str (str);
            } else {
                std::fprintf (stderr, "FormatContext::write_int: base is not among [2,8,10,16].");
                Backtrace::print_backtrace ();
                std::abort ();
            }
        }
    };

    template <class T>
    struct Formatter
    {};

    template <class T>
    struct FormatDispatcher
    {
        static void dispatch (T & obj, FormatContext ctx)
        {
            Formatter<T>::format (obj, ctx);
        }
    };

    void format_inner (String fmt_str, String & carry);

    template <class H, class... T>
    void format_inner (String fmt_str, String & carry, H head, T... tail)
    {
        isize index = 0;
        isize lah;
        do {
            index = fmt_str.index_of ('{', static_cast<usize> (index)).unwrap ();
            if (UNLIKELY (static_cast<usize> (index) == (fmt_str.length () - 1)
                          || fmt_str.bytes ().at (static_cast<usize> (index) + 1) == '{')) {
                index += 2;
                continue;
            } else {
                lah = index;
                do {
                    lah = fmt_str.index_of ('}', static_cast<usize> (lah)).unwrap ();
                    if (static_cast<usize> (lah) == fmt_str.length () - 1 || fmt_str.bytes ().at (static_cast<usize> (lah) + 1) != '}') {
                        break;
                    } else if (fmt_str.bytes ().at (static_cast<usize> (lah) + 1) == '}') {
                        lah += 2;
                    }
                } while (lah != -1);
                if (lah != -1)
                    break;
            }
        } while (index >= 0);

        if (index == -1 || lah == -1) {
            std::fprintf (stderr, "Argument number mismatch.");
            Backtrace::print_backtrace ();
            std::abort ();
        } else {
            carry.push (fmt_str.substr (0, static_cast<usize> (index)));
            String remainder = fmt_str.substr (static_cast<usize> (index));
            String gather    = remainder.substr (0, static_cast<usize> (lah) - static_cast<usize> (index) - 1);
            FormatContext ctx (gather, carry);
            FormatDispatcher<H>::dispatch (head, ctx);

            format_inner (remainder.substr (static_cast<usize> (lah) - static_cast<usize> (index) + 1), carry, tail...);
        }
    }

    String format (String fmt_str);

    template <class... Args>
    String format (String fmt_str, Args... args)
    {
        String carry;
        format_inner (fmt_str, carry, args...);
        return Ynk::Move (carry);
    }
}

namespace Ynk {
    void print (String fmt_str);

    void println (String fmt_str);

    template <class... Args>
    void print (String fmt_str, Args... args)
    {
        std::printf ("%s", Fmt::format (fmt_str, args...).into_inner_volatile ());
    }

    template <class... Args>
    void println (String fmt_str, Args... args)
    {
        std::printf ("%s\n", Fmt::format (fmt_str, args...).into_inner_volatile ());
    }

    void print_err (String fmt_str);

    void println_err (String fmt_str);

    template <class... Args>
    void print_err (String fmt_str, Args... args)
    {
        std::fprintf (stderr, "%s", Fmt::format (fmt_str, args...).into_inner_volatile ());
    }

    template <class... Args>
    void println_err (String fmt_str, Args... args)
    {
        String fmtd = Fmt::format (fmt_str, args...);
        // std::printf ("Size: %z\n", fmtd.inner.size ());
        // for (_usize i = 0; i < fmtd.inner.size (); i++)
        //     std::printf ("%c", fmtd.inner[i]);
        // std::printf ("\n");
        // for (_usize i = 0; i < fmtd.inner.size (); i++)
        //     std::printf ("%hhx(%c) ", fmtd.inner[i], fmtd.inner[i]);
        // std::printf ("\n");
        // std::fflush (stdout);
        std::fprintf (stderr, "%s\n", fmtd.into_inner_volatile ());
    }
}

#include <Ynk/Fmt/DefaultFormatters.hh>

#endif /* !@__YNK_FMT_FMT */
