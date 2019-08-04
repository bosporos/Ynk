//
// file Ynk/Num/NativeIntegers.h
// author Maximilien M. Cura
//
// changelog
//     30 july 2019    created
//

#ifndef __YNK_NUM_NATIVE_INTEGERS
#define __YNK_NUM_NATIVE_INTEGERS

#include <Ynk/Compiler.h>
#include <Ynk/Platform.h>

#include <unistd.h>

namespace Ynk {

#if COMPILER(GNU_COMPATIBLE)
    typedef __UINT8_TYPE__ _u8;
    typedef __UINT16_TYPE__ _u16;
    typedef __UINT32_TYPE__ _u32;
    typedef __UINT64_TYPE__ _u64;
    typedef unsigned __int128 _u128;
    typedef __INT8_TYPE__ _i8;
    typedef __INT16_TYPE__ _i16;
    typedef __INT32_TYPE__ _i32;
    typedef __INT64_TYPE__ _i64;
    typedef __int128 _i128;
#else
#    error "Ynk requires a GCC-compatible compiler (preferably GCC or CLang)"
#endif

#if CPU(ADDRESS64)

    static_assert (sizeof (size_t) == sizeof (unsigned long));

    typedef unsigned long _usize;
    typedef _usize _uptr;
    typedef long _isize;
    typedef _isize _iptr;
#else
#    error "Ynk requires a 64-bit system"
#endif

    static_assert (sizeof (_usize) == sizeof (void *));
    static_assert (sizeof (_isize) == sizeof (void *));
    static_assert (sizeof (_uptr) == sizeof (void *));
    static_assert (sizeof (_iptr) == sizeof (void *));

}

#endif /* !@__YNK_NUM_NATIVE_INTEGERS */
