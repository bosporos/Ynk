//
// file ynk/string/String.h
// author Maximilien M. Cura
//

#ifndef __YNK_STRING_STRING
#define __YNK_STRING_STRING

#include <Ynk/num/NativeIntegers.h>

extern "C"
{
    //! Unfortunate !!magic!!
    Ynk::_u64 __ynk_utf8_strlen (const char *);
}

static_assert (sizeof (char) == 8, "char is not 8 bits; contact the developer for support.");

#endif /* !@__YNK_STRING_STRING */
