//
// file ynk/string/utf8_strlen.c
// author Maximilien M. Cura
//

#include <Ynk/String/String.hh>

//! Unfortunately slow.
Ynk::_u64 __ynk_utf8_strlen (const char * string)
{
    Ynk::_u64 char_count = 0, utf8_count = 0;
    while (string[char_count]) {
        if ((string[char_count++] & 0xc0) != 0x80)
            utf8_count++;
    }
    return utf8_count;
}
