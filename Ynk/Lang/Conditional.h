//
// file Ynk/lang/Condition.h
// author Maximilien M. Cura
//

#ifndef __YNK_LANG_CONDITION
#define __YNK_LANG_CONDITION

namespace Ynk {
    template <bool C, class T, class U>
    struct Conditional
    {};

    template <class T, class U>
    struct Conditional<true, T, U>
    {
        typedef T type;
    };

    template <class T, class U>
    struct Conditional<false, T, U>
    {
        typedef U type;
    };

    template <bool C, class T>
    struct If
    {};

    template <class T>
    struct If<true, T>
    {
        typedef T type;
    };

    template <class T>
    struct If<false, T>
    {};
}

#endif /* !@__YNK_LANG_CONDITION */
