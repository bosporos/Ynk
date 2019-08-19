//
// file ynk/lang/Traits.hh
// author Maximilien M. Cura
//

#ifndef __YNK_LANG_TRAITS
#define __YNK_LANG_TRAITS

namespace Ynk {
    template <typename T, T V>
    struct IntegralConstant
    {
        static constexpr T value = V;
        constexpr operator T () const { return value; }
        constexpr T operator() () const { return value; }
    };

    template <typename T>
    struct IsLValueReference : IntegralConstant<bool, 0>
    {};

    template <typename T>
    struct IsLValueReference<T &> : IntegralConstant<bool, 1>
    {};

    template <typename T>
    struct IsReference : IntegralConstant<bool, 0>
    {};

    template <typename T>
    struct IsReference<T &> : IntegralConstant<bool, 1>
    {};

    template <typename T>
    struct IsReference<T &&> : IntegralConstant<bool, 1>
    {};

    template <typename T>
    struct IsRValueReference : IntegralConstant<bool, 0>
    {};

    template <typename T>
    struct IsRValueReference<T &&> : IntegralConstant<bool, 1>
    {};

    template <typename T>
    struct IsConst : IntegralConstant<bool, 0>
    {};

    template <typename T>
    struct IsConst<T const> : IntegralConstant<bool, 1>
    {};

}

#endif
