//
// file ynk/string/String.h
// author Maximilien M. Cura
//

#ifndef __YNK_STRING_STRING
#define __YNK_STRING_STRING

#include <Ynk/num/NativeIntegers.h>

#include <cstdlib>
#include <cstdio>

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
        char * inner;
        usize len_bytes;
        usize len_chars;

        String ()
            : inner { new char[1] { '\0' } }
            , len_bytes { 1 }
            , len_chars { 0 }
        {}

        String (const char * str)
            : inner { new char[strlen (str) + 1] }
            , len_bytes { strlen (str) + 1 }
            , len_chars { __ynk_utf8_strlen (str) }
        {
            strlcpy (this->inner, str, this->len_bytes);
        }

        String (String const & orig)
            : inner { new char[orig.len_bytes] }
            , len_bytes { orig.len_bytes }
            , len_chars { orig.len_chars }
        {
            strlcpy (this->inner, orig.inner, this->len_bytes);
        }

        String (String && orig)
            : inner { orig.inner }
            , len_bytes { orig.len_bytes }
            , len_chars { orig.len_chars }
        {
            orig.inner     = nullptr;
            orig.len_bytes = 0;
            orig.len_chars = 0;
        }

        ~String ()
        {
            delete[] this->inner;
        }

        usize length ()
        {
            return this->len_bytes - 1;
        }

        usize char_length ()
        {
            return this->len_chars;
        }

        usize byte_length ()
        {
            return this->len_bytes;
        }
    };
}

#endif /* !@__YNK_STRING_STRING */
