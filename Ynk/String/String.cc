//
// file ynk/string/String.cc
// author Maximilien M. Cura
//

#include <Ynk/String/String.hh>

using namespace Ynk;

Ynk::String operator""_y (const char * x, Ynk::_usize)
{
    return Ynk::String (x);
}

Ynk::String operator""_y (const char8_t * x, Ynk::_usize)
{
    return Ynk::String (x);
}

Ynk::String::String ()
    : inner (1)
{
    inner[0] = static_cast<char8_t> ('\0');
}

Ynk::String::String (const char * x)
    : inner (std::strlen (x) + 1)
{
    const usize length = std::strlen (x);
    for (usize i = 0; i < length; i++) {
        this->inner[i] = static_cast<const char8_t> (x[i]);
    }
    this->inner[length] = static_cast<char8_t> ('\0');
}

Ynk::String::String (const char8_t * x)
    : inner (std::strlen (reinterpret_cast<const char *> (x)) + 1)
{
    const usize length = std::strlen (reinterpret_cast<const char *> (x));
    for (usize i = 0; i < length; i++) {
        this->inner[i] = x[i];
    }
    this->inner[length] = static_cast<char8_t> ('\0');
}

Ynk::String::String (usize len)
    : inner (len)
{}

Ynk::String::String (String const & x)
    : inner (x.inner)
{}

Ynk::String::String (String && x)
    : inner (Ynk::Move (x.inner))
{}

usize Ynk::String::char_length () const
{
    // If vector::size() is 0, then vector::data() may actually be a nullptr.
    if (this->inner.size () > 0) {
        return __ynk_utf8_strlen (reinterpret_cast<const char *> (this->inner.data ()));
    } else {
        return 0;
    }
}

usize Ynk::String::byte_length () const
{
    return this->inner.size ();
}

usize Ynk::String::length () const
{
    return this->inner.size () - 1;
}

// Todo: Char
// Todo: KMP substring search
// Todo: index_of()
// Todo: iterators

Option<usize> Ynk::String::index_of (char8_t chr) const
{
    return this->index_of (chr, 0);
}

Option<usize> Ynk::String::index_of (char8_t chr, usize start) const
{
    for (usize i = start; i < this->inner.size (); i++) {
        if (this->inner[i] == chr) {
            return Some (Ynk::Move (i));
        }
    }
    return None ();
}

String Ynk::String::substr (usize idx) const
{
    String tmp;
    tmp.inner.insert (tmp.inner.begin (), this->inner.begin () + idx, this->inner.end () - 1);
    return Ynk::Move (tmp);
}

String Ynk::String::substr (usize idx, usize length) const
{
    String tmp;
    tmp.inner.insert (tmp.inner.begin (), this->inner.begin () + idx, this->inner.begin () + idx + length);
    return Ynk::Move (tmp);
}

String Ynk::String::append (String rhs) const
{
    return Ynk::Move (rhs.unslide (*this));
}

String Ynk::String::prepend (String rhs) const
{
    return Ynk::Move (rhs.push (*this));
}

String & Ynk::String::unslide (String const & rhs)
{
    if (rhs.inner[rhs.inner.size ()] == '\0')
        this->inner.insert (this->inner.begin (), rhs.inner.begin (), rhs.inner.end () - 1);
    else
        this->inner.insert (this->inner.begin (), rhs.inner.begin (), rhs.inner.end ());
    return *this;
}

String & Ynk::String::push (String const & rhs)
{
    if (rhs.inner[rhs.inner.size () - 1] == 0)
        this->inner.insert (this->inner.end () - 1, rhs.inner.begin (), rhs.inner.end () - 1);
    else
        this->inner.insert (this->inner.end () - 1, rhs.inner.begin (), rhs.inner.end ());
    if (this->inner.size () > 2 && this->inner[this->inner.size () - 2] == 0)
        this->inner.resize (this->inner.size () - 1);
    return *this;
}

std::vector<char8_t> & Ynk::String::bytes ()
{
    return this->inner;
}

String Ynk::String::operator+ (String rhs) const
{
    return this->append (rhs);
}

String & Ynk::String::operator+= (String rhs)
{
    return this->push (rhs);
}

const char * Ynk::String::into_inner_volatile ()
{
    return reinterpret_cast<const char *> (this->inner.data ());
}
