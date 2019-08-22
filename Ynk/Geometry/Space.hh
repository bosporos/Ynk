//
// file ynk/geometry/space.hh
// author Maximilien M. Cura
//

#ifndef __YNK_GEOMETRY_SPACE
#define __YNK_GEOMETRY_SPACE

#include <Ynk/Panic/Panic.hh>
#include <Ynk/Num/Integers.hh>
#include <Ynk/Lang/Move.hh>

#include <cmath>
#include <initializer_list>

namespace Ynk {
    enum class SpaceType : _usize {
        Cartesian, /* <x, y, z, ...> */
        Polar, /* <r, theta, phi, ...> */
    };

    template <unsigned _N, class _T>
    struct Space;

    template <unsigned _N, class _T>
    struct Vec
    {
        typedef _T value_type;

        Space<_N, _T> * space;

        _T values[_N];

        Vec (Space<_N, _T> * space, std::initializer_list<_T> values)
            : space { space }
        {
            for (usize i = 0; i < _N; i++) {
                this->values[i] = values.begin ()[i];
            }
        }

        Vec (Vec const & orig)
            : space { orig.space }
        {
            for (usize i = 0; i < _N; i++) {
                this->values[i] = orig.values[i];
            }
        }

        _T & operator[] (usize index) { return this->values[index]; }
        _T const & operator[] (usize index) const { return this->values[index]; }

        Vec operator+ (Vec const & rhs) const
        {
            Vec tmp { *this };
            space->op_vec_add (tmp, rhs);
            return tmp;
        }
        Vec operator- (Vec const & rhs) const
        {
            Vec tmp { *this };
            space->op_vec_sub (tmp, rhs);
            return tmp;
        }
        Vec operator* (Vec const & rhs) const
        {
            Vec tmp { *this };
            space->op_vec_mul (tmp, rhs);
            return tmp;
        }
        Vec operator/ (Vec const & rhs) const
        {
            Vec tmp { *this };
            space->op_vec_div (tmp, rhs);
            return tmp;
        }
        Vec & operator+= (Vec const & rhs)
        {
            space->op_vec_add (*this, rhs);
            return *this;
        }
        Vec & operator-= (Vec const & rhs)
        {
            space->op_vec_sub (*this, rhs);
            return *this;
        }
        Vec & operator*= (Vec const & rhs)
        {
            space->op_vec_mul (*this, rhs);
            return *this;
        }
        Vec & operator/= (Vec const & rhs)
        {
            space->op_vec_div (*this, rhs);
            return *this;
        }
    };

    template <unsigned _N, class _T>
    struct Space
    {
        static const int kPolarRadiusIndex { 0 };

        static Space<_N, _T> * instance (SpaceType st)
        {
            static Space<_N, _T> ** insts = nullptr;
            if (insts == nullptr) {
                insts = new Space<_N, _T> *[2];
                for (usize i = 0; i < 2_uz; i++) {
                    insts[i] = new Space<_N, _T> (
                        static_cast<SpaceType> (
                            static_cast<_usize> (i)));
                }
            }
            return insts[static_cast<_usize> (st)];
        }

        const usize N = _N;
        const SpaceType space_type;
        typedef _T value_type;

        // FACTORY OPERATIONS

        Vec<_N, _T> create_vec (std::initializer_list<_T> values)
        {
            return Vec<_N, _T> (this, values);
        }

        // SPACIAL TRANSFORMATION OPERATIONS

        Vec<_N, _T> mold_to_shape (Vec<_N, _T> const & vec)
        {
            SpaceType originating_spacial = vec.space->space_type;
            if (originating_spacial == this->space_type)
                return { vec };
            // Not implemented;
            panic ("Fatal error: not implemented: Space::mold_to_shape()");
        }

        // VECTOR OPERATIONS

        void op_vec_add (Vec<_N, _T> & lhs, Vec<_N, _T> const & rhs)
        {
            switch (this->space_type) {
                case SpaceType::Cartesian:
                    for (usize i = 0; i < _N; i++) {
                        lhs[i] += rhs[i];
                    }
                    break;
                case SpaceType::Polar:
                    panic ("Fatal error: Space::op_vec_add() for SpaceType::Polar is not implemented");
            }
        }

        void op_vec_sub (Vec<_N, _T> & lhs, Vec<_N, _T> const & rhs)
        {
            switch (this->space_type) {
                case SpaceType::Cartesian:
                    for (usize i = 0; i < _N; i++) {
                        lhs[i] -= rhs[i];
                    }
                    break;
                case SpaceType::Polar:
                    panic ("Fatal error: Space::op_vec_sub() for SpaceType::Polar is not implemented");
            }
        }

        void op_vec_mul (Vec<_N, _T> & lhs, _T scale_factor)
        {
            switch (this->space_type) {
                case SpaceType::Cartesian:
                    for (usize i = 0; i < _N; i++) {
                        lhs[i] *= scale_factor;
                    }
                    break;
                case SpaceType::Polar:
                    lhs[Space::kPolarRadiusIndex] *= scale_factor;
                    break;
            }
        }

        void op_vec_div (Vec<_N, _T> & lhs, _T scale_factor)
        {
            switch (this->space_type) {
                case SpaceType::Cartesian:
                    for (usize i = 0; i < _N; i++) {
                        lhs[i] /= scale_factor;
                    }
                    break;
                case SpaceType::Polar:
                    lhs[Space::kPolarRadiusIndex] /= scale_factor;
                    break;
            }
        }

        void op_vec_normalize (Vec<_N, _T> & vec)
        {
            switch (this->space_type) {
                case SpaceType::Cartesian:
                    this->op_vec_div (vec, this->op_vec_maximum (vec));
                    break;
                case SpaceType::Polar:
                    vec[Space::kPolarRadiusIndex] = 1;
                    break;
            }
        }

        _T op_vec_maximum (Vec<_N, _T> const & vec)
        {
            switch (this->space_type) {
                case SpaceType::Cartesian: {
                    _T highest = 0;
                    for (usize i = 0; i < _N; i++) {
                        if (vec[i] > highest)
                            highest = vec[i];
                    }
                    return highest;
                }
                case SpaceType::Polar:
                    return vec[Space::kPolarRadiusIndex];
            }
        }

        _T op_vec_minimum (Vec<_N, _T> const & vec)
        {
            switch (this->space_type) {
                case SpaceType::Cartesian: {
                    _T lowest = vec[0];
                    for (usize i = 1; i < _N; i++) {
                        if (vec[i] < lowest)
                            lowest = vec[i];
                    }
                    return lowest;
                }
                case SpaceType::Polar:
                    return vec[Space::kPolarRadiusIndex];
            }
        }

        _T op_vec_magnitude (Vec<_N, _T> const & vec)
        {
            switch (this->space_type) {
                case SpaceType::Cartesian: {
                    _T sum = 0;
                    for (usize i = 0; i < _N; i++) {
                        sum += vec[i] * vec[i];
                    }
                    return std::sqrtl (sum);
                }
                case SpaceType::Polar:
                    return vec[Space::kPolarRadiusIndex];
            }
        }

        void op_vec_abs (Vec<_N, _T> & vec)
        {
            switch (this->space_type) {
                case SpaceType::Cartesian:
                    for (usize i = 0; i < _N; i++)
                        if (vec[i] < 0)
                            vec[i] = -vec[i];
                    break;
                case SpaceType::Polar:
                    if (vec[Space::kPolarRadiusIndex] < 0)
                        vec[Space::kPolarRadiusIndex] = -vec[Space::kPolarRadiusIndex];
                    break;
            }
        }

        _T op_vec_product (Vec<_N, _T> const & vec)
        {
            // Multiplicative identity
            _T product = 1;
            for (usize i = 0; i < _N; i++) {
                product *= vec[i];
            }
            return product;
        }

        _T op_vec_sum (Vec<_N, _T> const & vec)
        {
            // Additive identity
            _T sum = 0;
            for (usize i = 0; i < _N; i++) {
                sum += vec[i];
            }
            return sum;
        }

        _T op_vec_dot (Vec<_N, _T> const & lhs, Vec<_N, _T> const & rhs)
        {
            _T sum = 0;
            for (usize i = 0; i < _N; i++) {
                sum += lhs[i] * rhs[i];
            }
            return sum;
        }

    private:
        Space (SpaceType st)
            : space_type { st }
        {}
    };
}

namespace Ynk::Fmt {
    template <unsigned _N, class _T>
    struct Formatter<::Ynk::Vec<_N, _T>>
    {
        static void format (::Ynk::Vec<_N, _T> vec, FormatContext ctx)
        {
            ctx.write_char ('<');
            for (usize i = 0; i < _N; i++) {
                if (i > 0_uz)
                    ctx.write_char (',');
                ctx.write_str (Fmt::format ("{}", vec.values[i]));
            }
            ctx.write_char ('>');
        }
    };
}

#endif /* !@__YNK_GEOMETRY_SPACE */
