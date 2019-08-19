//
// file ynk/string/String.h
// author Maximilien M. Cura
//

#ifndef __YNK_STRING_STRING
#define __YNK_STRING_STRING

#include <Ynk/Num/NativeIntegers.h>
#include <Ynk/Num/Integers.h>
#include <Ynk/Lang/Move.h>
#include <Ynk/Option/Option.h>
#include <Ynk/Box/Box.h>

#include <cstdlib>
#include <cstdio>

#include <strings.h> /* strlcpy, bcopy */

#include <vector>

extern "C"
{
    //! Unfortunate !!magic!!
    Ynk::_u64 __ynk_utf8_strlen (const char *);
}

static_assert (sizeof (char) == 1, "char is not 8 bits; contact the developer for support.");

namespace Ynk {
    struct String
    {
        std::vector<char8_t> inner;

        String ();
        String (const char * x);
        String (const char8_t * x);
        String (usize len);
        String (String const & x);
        String (String && x);

        usize char_length () const;
        usize byte_length () const;
        usize length () const;

        // Todo: Char
        // Todo: KMP substring search
        // Todo: index_of()
        // Todo: iterators

        Option<usize> index_of (char8_t chr) const;
        Option<usize> index_of (char8_t chr, usize start) const;
        String substr (usize idx) const;
        String substr (usize idx, usize length) const;
        String append (String rhs) const;
        String prepend (String rhs) const;
        String & unslide (String const & rhs);
        String & push (String const & rhs);
        std::vector<char8_t> & bytes ();
        String operator+ (String rhs) const;
        String & operator+= (String rhs);
        const char * into_inner_volatile ();
    };
}

Ynk::String operator""_y (const char * x, Ynk::_usize);
Ynk::String operator""_y (const char8_t * x, Ynk::_usize);

#endif /* !@__YNK_STRING_STRING */
