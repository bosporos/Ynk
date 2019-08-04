//
// file ynk/lang/RemoveIntegerUnsignedness.h
// author Maximilien M. Cura
//

#ifndef __YNK_LANG_REMOVE_INTEGER_UNSIGNEDNESS
#define __YNK_LANG_REMOVE_INTEGER_UNSIGNEDNESS

#include <Ynk/Num/NativeIntegers.h>

namespace Ynk {

    template <class T>
    struct RemoveIntegerUnsignedness
    {};

#define _Ynk_RIU(__UNSIGNED__, __SIGNED__)         \
    template <>                                    \
    struct RemoveIntegerUnsignedness<__UNSIGNED__> \
    {                                              \
        typedef __SIGNED__ Type;                   \
    }

    _Ynk_RIU (_u8, _i8);
    _Ynk_RIU (_u16, _i16);
    _Ynk_RIU (_u32, _i32);
    _Ynk_RIU (_u64, _i64);
    _Ynk_RIU (_usize, _isize);
    // _uptr == _usize, _iptr = _isize, so not necessary

#undef _Ynk_RIU
}

#endif /* !@__YNK_LANG_REMOVE_INTEGER_UNSIGNEDNESS */
