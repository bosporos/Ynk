//
// file ynk/lang/Move.hh
// author Maximilien M. Cura
//

#ifndef __YNK_LANG_MOVE
#define __YNK_LANG_MOVE

#include <Ynk/Lang/Dequalify.hh>
#include <Ynk/Lang/Traits.hh>

namespace Ynk {
    template <typename T>
    ALWAYS_INLINE constexpr typename Ynk::RemoveReference<T>::type && Move (T && value)
    {
        static_assert (Ynk::IsLValueReference<T>::value, "T is not an lvalue reference; move is not necessary");

        using Dequalified = typename Ynk::RemoveReference<T>::type;
        static_assert (!Ynk::IsConst<Dequalified>::value, "T is const qualified");

        return static_cast<typename RemoveReference<T>::type &&> (value);
    }
}

// #define move(x) Ynk::Move (x)

#endif
