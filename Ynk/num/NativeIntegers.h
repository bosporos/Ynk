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

namespace Ynk {

#if COMPILER(GCC_COMPATIBLE)
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
    typedef _u64 _usize;
    typedef _u64 _uptr;
    typedef _i64 _isize;
    typedef _i64 _iptr;
#else
#    error "Ynk requires a 64-bit system"
#endif

}

#endif /* !@__YNK_NUM_NATIVE_INTEGERS */
