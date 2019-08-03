//
// file ynk/option/Option.h
// author Maximilien M. Cura
//

#ifndef __YNK_OPTION_OPTION
#define __YNK_OPTION_OPTION

// #include "panic/panic.hpp"

#include <Ynk/Lang/Move.h>

#include <cstdlib>

namespace Ynk {
    template <class T, class... Args>
    T __option_map_is_copyable__ (T t, Args...)
    {
        return t;
    }

    template <class T>
    struct Option;
    template <>
    struct Option<void>
    {};

    template <class T>
    Option<T> Some (T const & o)
    {
        return Option<T> (o);
    }
    template <class T>
    Option<T> Some (T && o)
    {
        return Option<T> (Ynk::Move (o));
    }

    // template <class T>
    // Option<T> None ()
    // {
    //     return Option<T> ();
    // }

    using None = Option<void>;

    template <class T>
    struct Option
    {
        union {
            T inner;
        };
        const bool has_value;

        Option ()
            : has_value { 0 }
        {}

        Option (T const & value)
            : inner { value }
            , has_value { 1 }
        {}

        Option (T && value)
            : inner { Ynk::Move (value) }
            , has_value { 1 }
        {}

        Option (Option<void>)
            : has_value { 0 }
        {}

        Option (Option<T> const & o)
            : has_value { o.has_value }
        {
            if (has_value) {
                new (&inner) T (o.inner);
            }
        }

        Option (Option<T> && o)
            : has_value { o.has_value }
            , inner { Ynk::Move (o.inner) }
        {}
        ~Option ()
        {
            // Should not actually destroy inner
            // A: If inner is allocated solely on stack, and T is neither
            //      a reference nor an rvalue reference, then error/result
            //      will deallocate with the class
            // B: If inner is allocated solely on stack, and T is a
            //      reference or an rvalue reference, then the original
            //      value shouldn't be destroyed here.
            // C: If T is a pointer, it's impossible to tell if inner
            //      was allocated on heap (let alone malloc v. new), or is
            //      simply pointing to a durable object somewhere
        }
        constexpr T unwrap () const
        {
            if (!has_value) {
                // panic ("option::unwrap: has no value");
                std::abort ();
            }
            return inner;
        }
        constexpr T unwrap_or (T default_value) const
        {
            return has_value ? inner : default_value;
        }
        constexpr T unwrap_or (T (*func) ())
        {
            return has_value ? inner : func ();
        }

        template <class M>
        constexpr auto map (const M & m) const & -> Ynk::Option<
            decltype (m (__option_map_is_copyable__ (this->inner)))>
        {
            using returning_t = decltype (m (inner));
            if (!has_value) {
                return Option<returning_t> ();
            } else {
                return Option<returning_t> (m (inner));
            }
        }

        template <class M>
        constexpr auto map (const M & m) && -> Ynk::Option<
            decltype (m (Ynk::Move (this->inner)))>
        {
            using returning_t = decltype (m (Ynk::Move (inner)));
            if (!has_value) {
                return Option<returning_t> ();
            } else {
                return Option<returning_t> (m (Ynk::Move (inner)));
            }
        }
        constexpr operator bool () const { return has_value; }
    };
}

#endif /* !@__YNK_OPTION_OPTION */
