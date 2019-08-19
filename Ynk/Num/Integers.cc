//
// file ynk/num/integers.cc
// author Maximilien M. Cura
//

#include <Ynk/Num/Integers.hh>

#define ___ynk_int_gen_literal_operator(__TYPE__, __SIZE__, __BANG__)                                                     \
    Ynk::__TYPE__ operator""_u##__BANG__ (unsigned long long integral)                                                    \
    {                                                                                                                     \
        return { static_cast<Ynk::__TYPE__::underlying_type> (integral) };                                                \
    }                                                                                                                     \
    Ynk::int_impl<__SIZE__,                                                                                               \
                  typename Ynk::RemoveIntegerUnsignedness<Ynk::__TYPE__::underlying_type>::Type,                          \
                  Ynk::__TYPE__::underlying_type>                                                                         \
        operator""_i##__BANG__ (unsigned long long integral)                                                              \
    {                                                                                                                     \
        return { static_cast<typename Ynk::RemoveIntegerUnsignedness<Ynk::__TYPE__::underlying_type>::Type> (integral) }; \
    }

___ynk_int_gen_literal_operator (u8, 8, 8);
___ynk_int_gen_literal_operator (u16, 16, 16);
___ynk_int_gen_literal_operator (u32, 32, 32);
___ynk_int_gen_literal_operator (u64, 64, 64);
static_assert (8 == sizeof (void *));
___ynk_int_gen_literal_operator (usize, 64, z);
