//
// file ynk/math/Math.hh
// author Maximilien M. Cura
//

#ifndef __YNK_MATH_MATH
#define __YNK_MATH_MATH

namespace Ynk::Math {
    template <unsigned S, class U>
    uint_impl<S, U> clamp (uint_impl<S, U> x, uint_impl<S, U> lower, uint_impl<S, U> upper)
    {
        if (x < lower)
            return lower;
        if (x > upper)
            return upper;
        return x;
    }

    template <unsigned S, class U, class T>
    int_impl<S, U, T> min (int_impl<S, U, T> lhs, int_impl<S, U, T> rhs)
    {
        if (lhs < rhs)
            return lhs;
        return rhs;
    }

    template <unsigned S, class U>
    uint_impl<S, U> min (uint_impl<S, U> lhs, uint_impl<S, U> rhs)
    {
        if (lhs < rhs)
            return lhs;
        return rhs;
    }
}

#endif /* !@__YNK_MATH_MATH */
