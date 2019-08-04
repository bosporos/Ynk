//
// file ynk/string/String.cc
// author Maximilien M. Cura
//

#include <Ynk/String/String.h>

Ynk::String operator""_y (const char * x, Ynk::_usize)
{
    return Ynk::String (x);
}

Ynk::String operator""_y (const char8_t * x, Ynk::_usize)
{
    return Ynk::String (x);
}
