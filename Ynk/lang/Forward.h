//
// file ynk/lang/Forward.h
// author Maximilien M. Cura
//

#ifndef __YNK_LANG_FORWARD
#define __YNK_LANG_FORWARD

#include <ynk/Compiler.h>
#include <ynk/lang/RemoveReference.h>

namespace Ynk {
    template <typename T>
    ALWAYS_INLINE constexpr T && Forward (
        typename Ynk::RemoveReference<T>::type & x)
    {
        return static_cast<T &&> (x);
    }

    template <typename T>
    ALWAYS_INLINE constexpr T && Forward (
        typename Ynk::RemoveReference<T>::type && x)
    {
        static_assert (!Ynk::IsLValueReference (x), "Cannot forward an lvalue as an rvalue") return static_cast<T &&> (x);
        return static_cast<T &&> (x);
    }
}

#endif
