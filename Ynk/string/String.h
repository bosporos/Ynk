//
// file ynk/string/String.h
// author Maximilien M. Cura
//

#ifndef __YNK_STRING_STRING
#define __YNK_STRING_STRING

#include <Ynk/num/NativeIntegers.h>
#include <Ynk/num/Integers.h>
#include <Ynk/lang/Move.h>
#include <Ynk/option/Option.h>
#include <Ynk/box/Box.h>

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

        String ()
            : inner (1)
        {
            inner[0] = static_cast<char8_t> ('\0');
        }

        String (const char * x)
            : inner (std::strlen (x) + 1)
        {
            const usize length = std::strlen (x);
            for (usize i = 0; i < length; i++) {
                this->inner[i] = static_cast<const char8_t> (x[i]);
            }
            this->inner[length] = static_cast<char8_t> ('\0');
        }

        String (const char8_t * x)
            : inner (std::strlen (reinterpret_cast<const char *> (x)) + 1)
        {
            const usize length = std::strlen (reinterpret_cast<const char *> (x));
            for (usize i = 0; i < length; i++) {
                this->inner[i] = x[i];
            }
            this->inner[length] = static_cast<char8_t> ('\0');
        }

        String (String const & x)
            : inner (x.inner)
        {}

        String (String && x)
            : inner (Ynk::Move (x.inner))
        {}

        usize char_length () const
        {
            // If vector::size() is 0, then vector::data() may actually be a nullptr.
            if (this->inner.size () > 0) {
                return __ynk_utf8_strlen (reinterpret_cast<const char *> (this->inner.data ()));
            } else {
                return 0;
            }
        }

        usize byte_length () const
        {
            return this->inner.size ();
        }

        usize length () const
        {
            return this->inner.size () - 1;
        }

        // Todo: KMP substring search
        // Todo: index_of()
        // Todo: iterators

        Option<usize> index_of (char8_t chr) const
        {
            for (usize i = 0; i < this->inner.size (); i++) {
                if (this->inner[i] == chr) {
                    return Some (Ynk::Move (i));
                }
            }
            return None ();
        }

        String substr (usize idx) const
        {
            String tmp;
            tmp.inner.insert (tmp.inner.begin (), this->inner.begin () + idx, this->inner.end ());
            return Ynk::Move (tmp);
        }

        String substr (usize idx, usize length) const
        {
            String tmp;
            tmp.inner.insert (tmp.inner.begin (), this->inner.begin () + idx, this->inner.begin () + idx + length);
            return Ynk::Move (tmp);
        }

        String append (String rhs) const
        {
            return Ynk::Move (rhs.unslide (*this));
        }

        String prepend (String rhs) const
        {
            return Ynk::Move (rhs.push (*this));
        }

        String & unslide (String const & rhs)
        {
            this->inner.insert (this->inner.begin (), rhs.inner.begin (), rhs.inner.end () - 1);
            return *this;
        }

        String & push (String const & rhs)
        {
            this->inner.insert (this->inner.end () - 1, rhs.inner.begin (), rhs.inner.end ());
            return *this;
        }

        String operator+ (String rhs) const
        {
            return this->append (rhs);
        }

        String & operator+= (String rhs)
        {
            return this->push (rhs);
        }

        const char * into_inner_volatile ()
        {
            return reinterpret_cast<const char *> (this->inner.data ());
        }
    };
}

Ynk::String operator""_y (const char * x, Ynk::_usize)
{
    return Ynk::String (x);
}

Ynk::String operator""_y (const char8_t * x, Ynk::_usize)
{
    return Ynk::String (x);
}

#endif /* !@__YNK_STRING_STRING */
