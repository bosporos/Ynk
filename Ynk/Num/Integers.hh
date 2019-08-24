//
// file ynk/_um/Integers.hhpp
// author Maximilien M. Cura
//

#ifndef __YNK_NUM_INTEGERS
#define __YNK_NUM_INTEGERS

#include <Ynk/Num/NativeIntegers.hh>
#include <Ynk/Option/Option.hh>
#include <Ynk/Lang/RemoveIntegerUnsignedness.hh>

#include <tuple>

#include <cstdio>

#define ___ynk_int_gen_operator(symbol)       \
    self_type operator symbol (self_type rhs) \
    {                                         \
        return inner_ symbol rhs.inner_;      \
    }                                         \
    self_type operator symbol (_u8 rhs)       \
    {                                         \
        return inner_ symbol rhs;             \
    }                                         \
    self_type operator symbol (_u16 rhs)      \
    {                                         \
        return inner_ symbol rhs;             \
    }                                         \
    self_type operator symbol (_u32 rhs)      \
    {                                         \
        return inner_ symbol rhs;             \
    }                                         \
    self_type operator symbol (_u64 rhs)      \
    {                                         \
        return inner_ symbol rhs;             \
    }                                         \
    self_type operator symbol (_i8 rhs)       \
    {                                         \
        return inner_ symbol rhs;             \
    }                                         \
    self_type operator symbol (_i16 rhs)      \
    {                                         \
        return inner_ symbol rhs;             \
    }                                         \
    self_type operator symbol (_i32 rhs)      \
    {                                         \
        return inner_ symbol rhs;             \
    }                                         \
    self_type operator symbol (_i64 rhs)      \
    {                                         \
        return inner_ symbol rhs;             \
    }                                         \
    self_type operator symbol (_usize rhs)    \
    {                                         \
        return inner_ symbol rhs;             \
    }                                         \
    self_type operator symbol (_isize rhs)    \
    {                                         \
        return inner_ symbol rhs;             \
    }
#define ___ynk_int_gen_modifying_operator(symbol)     \
    self_type &                                       \
        operator symbol (self_type rhs)               \
    {                                                 \
        inner_ symbol rhs.inner_;                     \
        return *this;                                 \
    }                                                 \
    self_type & operator symbol (underlying_type rhs) \
    {                                                 \
        inner_ symbol rhs;                            \
        return *this;                                 \
    }
#define ___ynk_int_gen_comparator(symbol) \
    bool operator symbol (self_type rhs)  \
    {                                     \
        return inner_ symbol rhs.inner_;  \
    }                                     \
    bool operator symbol (_u8 rhs)        \
    {                                     \
        return inner_ symbol rhs;         \
    }                                     \
    bool operator symbol (_u16 rhs)       \
    {                                     \
        return inner_ symbol rhs;         \
    }                                     \
    bool operator symbol (_u32 rhs)       \
    {                                     \
        return inner_ symbol rhs;         \
    }                                     \
    bool operator symbol (_u64 rhs)       \
    {                                     \
        return inner_ symbol rhs;         \
    }                                     \
    bool operator symbol (_i8 rhs)        \
    {                                     \
        return inner_ symbol rhs;         \
    }                                     \
    bool operator symbol (_i16 rhs)       \
    {                                     \
        return inner_ symbol rhs;         \
    }                                     \
    bool operator symbol (_i32 rhs)       \
    {                                     \
        return inner_ symbol rhs;         \
    }                                     \
    bool operator symbol (_i64 rhs)       \
    {                                     \
        return inner_ symbol rhs;         \
    }                                     \
    bool operator symbol (_usize rhs)     \
    {                                     \
        return inner_ symbol rhs;         \
    }                                     \
    bool operator symbol (_isize rhs)     \
    {                                     \
        return inner_ symbol rhs;         \
    }

namespace Ynk {
    template <unsigned S, class U, class V>
    struct int_impl;

    template <unsigned _Size, typename _Underlying>
    struct uint_impl
    {
        typedef uint_impl<_Size, _Underlying> self_type;
        typedef _Underlying underlying_type;
        typedef uint_impl<32, _u32> size_type;

        underlying_type inner_;

        static const self_type min_value () { return { static_cast<_Underlying> (0) }; };
        static const self_type max_value () { return { static_cast<_Underlying> (~0) }; };

        uint_impl ()
            : inner_ { 0 }
        {}
        uint_impl (underlying_type inner)
            : inner_ { inner }
        {}
        uint_impl (self_type const & orig)
            : inner_ { orig.inner_ }
        {}

        Option<self_type> checked_add (self_type rhs)
        {
            std::tuple<self_type, bool> tmp = this->overflowing_add (rhs);
            return std::get<1> (tmp) ? None () : Option (std::move (std::get<self_type> (tmp)));
        }
        Option<self_type> checked_sub (self_type rhs)
        {
            std::tuple<self_type, bool> tmp = this->overflowing_sub (rhs);
            return std::get<1> (tmp) ? None () : Option (std::move (std::get<self_type> (tmp)));
        }
        Option<self_type> checked_mul (self_type rhs)
        {
            std::tuple<self_type, bool> tmp = this->overflowing_mul (rhs);
            return std::get<1> (tmp) ? None () : Option (std::move (std::get<self_type> (tmp)));
        }
        Option<self_type> checked_div (self_type rhs)
        {
            return rhs.inner_ ? Some (inner_ / rhs.inner_) : None ();
        }
        Option<self_type> checked_rem (self_type rhs)
        {
            return rhs.inner_ ? Some (inner_ % rhs.inner_) : None ();
        }
        Option<self_type> checked_shl (size_type rhs)
        {
            std::tuple<self_type, bool> tmp = this->overflowing_shl (rhs);
            return std::get<1> (tmp) ? None () : Option (std::move (std::get<self_type> (tmp)));
        }
        Option<self_type> checked_shr (size_type rhs)
        {
            std::tuple<self_type, bool> tmp = this->overflowing_shr (rhs);
            return std::get<1> (tmp) ? None () : Option (std::move (std::get<self_type> (tmp)));
        }

        self_type saturating_add (self_type rhs)
        {
            Option<self_type> tmp = this->checked_add (rhs);
            return tmp.has_value () ? tmp.unwrap () : self_type::max_value ();
        }
        self_type saturating_sub (self_type rhs)
        {
            Option<self_type> tmp = this->checked_sub (rhs);
            return tmp.has_value () ? tmp.unwrap () : self_type::min_value ();
        }
        self_type saturating_mul (self_type rhs)
        {
            Option<self_type> tmp = this->checked_mul (rhs);
            return tmp.has_value () ? tmp.unwrap () : self_type::max_value ();
        }

        self_type wrapping_add (self_type rhs)
        {
            underlying_type tmp;
            __builtin_add_overflow (inner_, rhs.inner_, &tmp);
            return tmp;
        }
        self_type wrapping_sub (self_type rhs)
        {
            underlying_type tmp;
            __builtin_sub_overflow (inner_, rhs.inner_, &tmp);
            return tmp;
        }
        self_type wrapping_mul (self_type rhs)
        {
            underlying_type tmp;
            __builtin_mul_overflow (inner_, rhs.inner_, &tmp);
            return tmp;
        }
        self_type wrapping_div (self_type rhs)
        {
            return inner_ / rhs.inner_;
        }
        self_type wrapping_rem (self_type rhs)
        {
            return inner_ % rhs.inner_;
        }
        self_type wrapping_shl (size_type rhs)
        {
            return inner_ << static_cast<self_type> (rhs & (_Size - 1));
        }
        self_type wrapping_shr (size_type rhs)
        {
            return inner_ >> static_cast<self_type> (rhs & (_Size - 1));
        }

        std::tuple<self_type, bool> overflowing_add (self_type rhs)
        {
            underlying_type tmp;
            bool r = __builtin_add_overflow (inner_, rhs.inner_, &tmp);
            return std::make_tuple (self_type (tmp), r);
        }
        std::tuple<self_type, bool> overflowing_sub (self_type rhs)
        {
            underlying_type tmp;
            bool r = __builtin_sub_overflow (inner_, rhs.inner_, &tmp);
            return std::make_tuple (self_type (tmp), r);
        }
        std::tuple<self_type, bool> overflowing_mul (self_type rhs)
        {
            underlying_type tmp;
            bool r = __builtin_mul_overflow (inner_, rhs.inner_, &tmp);
            return std::make_tuple (self_type (tmp), r);
        }
        std::tuple<self_type, bool> overflowing_div (self_type rhs)
        {
            return std::make_tuple (self_type (inner_ / rhs.inner_), false);
        }
        std::tuple<self_type, bool> overflowing_rem (self_type rhs)
        {
            return std::make_tuple (self_type (inner_ % rhs.inner_), false);
        }
        std::tuple<self_type, bool> overflowing_shl (self_type rhs)
        {
            return std::make_tuple (this->wrapping_shl (rhs), (rhs > (_Size - 1)));
        }
        std::tuple<self_type, bool> overflowing_shr (self_type rhs)
        {
            return std::make_tuple (this->wrapping_shr (rhs), (rhs > (_Size - 1)));
        }

        ___ynk_int_gen_operator (+);
        ___ynk_int_gen_operator (-);
        ___ynk_int_gen_operator (*);
        ___ynk_int_gen_operator (/);
        ___ynk_int_gen_operator (%);

        ___ynk_int_gen_operator (<<);
        ___ynk_int_gen_operator (>>);
        ___ynk_int_gen_operator (&);
        ___ynk_int_gen_operator (|);
        ___ynk_int_gen_operator (^);

        ___ynk_int_gen_modifying_operator (+=);
        ___ynk_int_gen_modifying_operator (-=);
        ___ynk_int_gen_modifying_operator (*=);
        ___ynk_int_gen_modifying_operator (/=);
        ___ynk_int_gen_modifying_operator (%=);

        ___ynk_int_gen_modifying_operator (<<=);
        ___ynk_int_gen_modifying_operator (>>=);
        ___ynk_int_gen_modifying_operator (&=);
        ___ynk_int_gen_modifying_operator (|=);
        ___ynk_int_gen_modifying_operator (^=);

        ___ynk_int_gen_modifying_operator (=);

        ___ynk_int_gen_comparator (==);
        ___ynk_int_gen_comparator (!=);
        ___ynk_int_gen_comparator (>);
        ___ynk_int_gen_comparator (<);
        ___ynk_int_gen_comparator (>=);
        ___ynk_int_gen_comparator (<=);
        // todo: operator<=>

        // prefix
        self_type & operator++ ()
        {
            ++inner_;
            return *this;
        }
        // postfix
        self_type operator++ (int)
        {
            inner_++;
            return self_type (inner_ - 1);
        }
        // prefix
        self_type & operator-- ()
        {
            --inner_;
            return *this;
        }
        // postfix
        self_type operator-- (int)
        {
            inner_--;
            return self_type (inner_ + 1);
        }
        self_type operator~ () { return ~inner_; }
        bool operator! () { return !inner_; };

        int_impl<_Size, typename Ynk::RemoveIntegerUnsignedness<_Underlying>::Type, _Underlying> operator- ()
        {
            if (this->inner_ <= static_cast<_Underlying> (int_impl<_Size, typename Ynk::RemoveIntegerUnsignedness<_Underlying>::Type, _Underlying>::max_value ()))
                return { -static_cast<typename Ynk::RemoveIntegerUnsignedness<_Underlying>::Type> (this->inner_) };
            else {
                std::fprintf (stderr, "uint_impl<%u>: cannot perform unary negation: too large", _Size);
                // Can't backtrace: Backtrace depends Integers
                // Backtrace::print_backtrace ();
                std::abort ();
            }
        }

        operator underlying_type () const { return inner_; }
        operator underlying_type () { return inner_; }
    };

    template <unsigned _Size, typename _Underlying, typename _Unsigned>
    struct int_impl
    {
        typedef int_impl<_Size, _Underlying, _Unsigned> self_type;
        typedef _Underlying underlying_type;
        typedef uint_impl<32, _u32> size_type;

        underlying_type inner_;

        int_impl ()
            : inner_ { 0 }
        {}
        int_impl (underlying_type inner)
            : inner_ { inner }
        {}
        int_impl (self_type const & orig)
            : inner_ { orig.inner_ }
        {}

        static const self_type min_value ()
        {
            return { ~static_cast<_Underlying> (0) ^ (static_cast<_Unsigned> (~0) >> 1) };
        };
        static const self_type max_value ()
        {
            return { ~(~static_cast<_Underlying> (0) ^ (static_cast<_Unsigned> (~0) >> 1)) };
        };

        Option<self_type> checked_add (self_type rhs)
        {
            std::tuple<self_type, bool> tmp = this->overflowing_add (rhs);
            return std::get<1> (tmp) ? None () : Option (std::move (std::get<self_type> (tmp)));
        }
        Option<self_type> checked_sub (self_type rhs)
        {
            std::tuple<self_type, bool> tmp = this->overflowing_sub (rhs);
            return std::get<1> (tmp) ? None () : Option (std::move (std::get<self_type> (tmp)));
        }
        Option<self_type> checked_mul (self_type rhs)
        {
            std::tuple<self_type, bool> tmp = this->overflowing_mul (rhs);
            return std::get<1> (tmp) ? None () : Option (std::move (std::get<self_type> (tmp)));
        }
        Option<self_type> checked_div (self_type rhs)
        {
            return (rhs.inner_ == 0 || (inner_ == self_type::min_value () && rhs == -1))
                ? None ()
                : inner_ / rhs.inner_;
        }
        Option<self_type> checked_rem (self_type rhs)
        {
            return (rhs.inner_ == 0 || (inner_ == self_type::min_value () && rhs == -1))
                ? None ()
                : inner_ % rhs.inner_;
        }
        Option<self_type> checked_shl (size_type rhs)
        {
            std::tuple<self_type, bool> tmp = this->overflowing_shl (rhs);
            return std::get<1> (tmp) ? None () : Option (std::move (std::get<self_type> (tmp)));
        }
        Option<self_type> checked_shr (size_type rhs)
        {
            std::tuple<self_type, bool> tmp = this->overflowing_shr (rhs);
            return std::get<1> (tmp) ? None () : Option (std::move (std::get<self_type> (tmp)));
        }

        self_type saturating_add (self_type rhs)
        {
            Option<self_type> tmp = this->checked_add (rhs);
            if (tmp.has_value ()) {
                return tmp.unwrap ();
            } else {
                return rhs >= 0 ? self_type::max_value () : self_type::min_value ();
            }
        }
        self_type saturating_sub (self_type rhs)
        {
            Option<self_type> tmp = this->checked_sub (rhs);
            if (tmp.has_value ()) {
                return tmp.unwrap ();
            } else {
                return rhs >= 0 ? self_type::min_value () : self_type::max_value ();
            }
        }
        self_type saturating_mul (self_type rhs)
        {
            Option<self_type> tmp = this->checked_mul (rhs);
            if (tmp.has_value ())
                return tmp.unwrap ();
            else
                return ((inner_ < 0 && rhs < 0) || (inner_ > 0 && rhs > 0))
                    ? self_type::max_value ()
                    : self_type::min_value ();
        }

        self_type wrapping_add (self_type rhs)
        {
            underlying_type tmp;
            __builtin_sadd_overflow (inner_, rhs.inner_, &tmp);
            return tmp;
        }
        self_type wrapping_sub (self_type rhs)
        {
            underlying_type tmp;
            __builtin_ssub_overflow (inner_, rhs.inner_, &tmp);
            return tmp;
        }
        self_type wrapping_mul (self_type rhs)
        {
            underlying_type tmp;
            __builtin_smul_overflow (inner_, rhs.inner_, &tmp);
            return tmp;
        }
        self_type wrapping_div (self_type rhs)
        {
            return inner_ / rhs.inner_;
        }
        self_type wrapping_rem (self_type rhs)
        {
            return inner_ % rhs.inner_;
        }
        self_type wrapping_shl (size_type rhs)
        {
            return inner_ << static_cast<self_type> (rhs & (_Size - 1));
        }
        self_type wrapping_shr (size_type rhs)
        {
            return inner_ >> static_cast<self_type> (rhs & (_Size - 1));
        }

        std::tuple<self_type, bool> overflowing_add (self_type rhs)
        {
            underlying_type tmp;
            bool r = __builtin_sadd_overflow (inner_, rhs.inner_, &tmp);
            return std::make_tuple (self_type (tmp), r);
        }
        std::tuple<self_type, bool> overflowing_sub (self_type rhs)
        {
            underlying_type tmp;
            bool r = __builtin_ssub_overflow (inner_, rhs.inner_, &tmp);
            return std::make_tuple (self_type (tmp), r);
        }
        std::tuple<self_type, bool> overflowing_mul (self_type rhs)
        {
            underlying_type tmp;
            bool r = __builtin_smul_overflow (inner_, rhs.inner_, &tmp);
            return std::make_tuple (self_type (tmp), r);
        }
        std::tuple<self_type, bool> overflowing_div (self_type rhs)
        {
            if (inner_ == self_type::min_value && rhs == -1)
                return std::make_tuple (self_type (inner_), true);
            else
                return std::make_tuple (self_type (inner_ / rhs.inner_), false);
        }
        std::tuple<self_type, bool> overflowing_rem (self_type rhs)
        {
            if (inner_ == self_type::min_value && rhs == -1)
                return std::make_tuple (self_type (inner_), true);
            else
                return std::make_tuple (self_type (inner_ % rhs.inner_), false);
        }
        std::tuple<self_type, bool> overflowing_shl (self_type rhs)
        {
            return std::make_tuple (this->wrapping_shl (rhs), (rhs > (_Size - 1)));
        }
        std::tuple<self_type, bool> overflowing_shr (self_type rhs)
        {
            return std::make_tuple (this->wrapping_shr (rhs), (rhs > (_Size - 1)));
        }

        ___ynk_int_gen_operator (+);
        ___ynk_int_gen_operator (-);
        ___ynk_int_gen_operator (*);
        ___ynk_int_gen_operator (/);
        ___ynk_int_gen_operator (%);

        ___ynk_int_gen_operator (<<);
        ___ynk_int_gen_operator (>>);
        ___ynk_int_gen_operator (&);
        ___ynk_int_gen_operator (|);
        ___ynk_int_gen_operator (^);

        ___ynk_int_gen_modifying_operator (+=);
        ___ynk_int_gen_modifying_operator (-=);
        ___ynk_int_gen_modifying_operator (*=);
        ___ynk_int_gen_modifying_operator (/=);
        ___ynk_int_gen_modifying_operator (%=);

        ___ynk_int_gen_modifying_operator (<<=);
        ___ynk_int_gen_modifying_operator (>>=);
        ___ynk_int_gen_modifying_operator (&=);
        ___ynk_int_gen_modifying_operator (|=);
        ___ynk_int_gen_modifying_operator (^=);

        ___ynk_int_gen_modifying_operator (=);

        ___ynk_int_gen_comparator (==);
        ___ynk_int_gen_comparator (!=);
        ___ynk_int_gen_comparator (>);
        ___ynk_int_gen_comparator (<);
        ___ynk_int_gen_comparator (>=);
        ___ynk_int_gen_comparator (<=);
        // todo: operator<=>

        // prefix
        self_type & operator++ ()
        {
            ++inner_;
            return *this;
        }
        // postfix
        self_type operator++ (int)
        {
            inner_++;
            return self_type (inner_ - 1);
        }
        // prefix
        self_type & operator-- ()
        {
            --inner_;
            return *this;
        }
        // postfix
        self_type operator-- (int)
        {
            inner_--;
            return self_type (inner_ + 1);
        }
        self_type operator~ () { return ~inner_; }
        bool operator! () { return !inner_; };

        self_type operator- () { return -inner_; }

        operator underlying_type () const { return inner_; }
        operator underlying_type () { return inner_; }
    };

    using u8    = Ynk::uint_impl<8, Ynk::_u8>;
    using u16   = Ynk::uint_impl<16, Ynk::_u16>;
    using u32   = Ynk::uint_impl<32, Ynk::_u32>;
    using u64   = Ynk::uint_impl<64, Ynk::_u64>;
    using usize = Ynk::uint_impl<64, Ynk::_usize>;
    using uptr  = Ynk::uint_impl<64, Ynk::_uptr>;

    using i8    = Ynk::int_impl<8, Ynk::_i8, Ynk::_u8>;
    using i16   = Ynk::int_impl<16, Ynk::_i16, Ynk::_u16>;
    using i32   = Ynk::int_impl<32, Ynk::_i32, Ynk::_u32>;
    using i64   = Ynk::int_impl<64, Ynk::_i64, Ynk::_u64>;
    using isize = Ynk::int_impl<64, Ynk::_isize, Ynk::_usize>;
    using iptr  = Ynk::int_impl<64, Ynk::_iptr, Ynk::_uptr>;
}

#undef ___ynk_int_gen_operator
#undef ___ynk_int_gen_comparator
#undef ___ynk_int_gen_modifying_operator

#define ___ynk_int_gen_literal_operator_decl(__TYPE__, __SIZE__, __BANG__)                       \
    Ynk::__TYPE__ operator""_u##__BANG__ (unsigned long long integral);                          \
    Ynk::int_impl<__SIZE__,                                                                      \
                  typename Ynk::RemoveIntegerUnsignedness<Ynk::__TYPE__::underlying_type>::Type, \
                  Ynk::__TYPE__::underlying_type>                                                \
        operator""_i##__BANG__ (unsigned long long integral)

___ynk_int_gen_literal_operator_decl (u8, 8, 8);
___ynk_int_gen_literal_operator_decl (u16, 16, 16);
___ynk_int_gen_literal_operator_decl (u32, 32, 32);
___ynk_int_gen_literal_operator_decl (u64, 64, 64);
static_assert (8 == sizeof (void *));
___ynk_int_gen_literal_operator_decl (usize, 64, z);

#undef ___ynk_int_gen_literal_operator_decl

#endif /* !@__YNK_NUM_INTEGERS */
