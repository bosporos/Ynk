//
// file ynk/geometry/space.h
// author Maximilien M. Cura
//

#ifndef __YNK_GEOMETRY_SPACE
#define __YNK_GEOMETRY_SPACE

#include <Ynk/Panic/Panic.h>
#include <Ynk/Num/Integers.h>
#include <Ynk/Lang/Move.h>

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
    struct Vector
    {
        typedef _T value_type;

        Space<_N, _T> * space;

        _T values[_N];

        Vector (Space<_N, _T> * space, std::initializer_list<_T> values)
            : space { space }
        {
            for (usize i = 0; i < _N; i++) {
                this->values[i] = values.begin ()[i];
            }
        }

        Vector (Vector const & orig)
            : space { orig.space }
        {
            for (usize i = 0; i < _N; i++) {
                this->values[i] = orig.values[i];
            }
        }

        _T & operator[] (usize index) { return this->values[index]; }
        _T const & operator[] (usize index) const { return this->values[index]; }

        Vector operator+ (Vector const & rhs) const
        {
            Vector tmp { *this };
            space->op_vec_add (tmp, rhs);
            return tmp;
        }
        Vector operator- (Vector const & rhs) const
        {
            Vector tmp { *this };
            space->op_vec_sub (tmp, rhs);
            return tmp;
        }
        Vector operator* (Vector const & rhs) const
        {
            Vector tmp { *this };
            space->op_vec_mul (tmp, rhs);
            return tmp;
        }
        Vector operator/ (Vector const & rhs) const
        {
            Vector tmp { *this };
            space->op_vec_div (tmp, rhs);
            return tmp;
        }
        Vector & operator+= (Vector const & rhs)
        {
            space->op_vec_add (*this, rhs);
            return *this;
        }
        Vector & operator-= (Vector const & rhs)
        {
            space->op_vec_sub (*this, rhs);
            return *this;
        }
        Vector & operator*= (Vector const & rhs)
        {
            space->op_vec_mul (*this, rhs);
            return *this;
        }
        Vector & operator/= (Vector const & rhs)
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

        Vector<_N, _T> create_vector (std::initializer_list<_T> values)
        {
            return Vector<_N, _T> (this, values);
        }

        // SPACIAL TRANSFORMATION OPERATIONS

        Vector<_N, _T> mold_to_shape (Vector<_N, _T> const & vec)
        {
            SpaceType originating_spacial = vec.space->space_type;
            if (originating_spacial == this->space_type)
                return { vec };
            // Not implemented;
            panic ("Fatal error: not implemented: Space::mold_to_shape()");
        }

        // VECTOR OPERATIONS

        void op_vec_add (Vector<_N, _T> & lhs, Vector<_N, _T> const & rhs)
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

        void op_vec_sub (Vector<_N, _T> & lhs, Vector<_N, _T> const & rhs)
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

        void op_vec_mul (Vector<_N, _T> & lhs, _T scale_factor)
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

        void op_vec_div (Vector<_N, _T> & lhs, _T scale_factor)
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

        void op_vec_normalize (Vector<_N, _T> & vec)
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

        _T op_vec_maximum (Vector<_N, _T> const & vec)
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

        _T op_vec_minimum (Vector<_N, _T> const & vec)
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

        _T op_vec_magnitude (Vector<_N, _T> const & vec)
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

        void op_vec_abs (Vector<_N, _T> & vec)
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

        _T op_vec_product (Vector<_N, _T> const & vec)
        {
            // Multiplicative identity
            _T product = 1;
            for (usize i = 0; i < _N; i++) {
                product *= vec[i];
            }
            return product;
        }

        _T op_vec_sum (Vector<_N, _T> const & vec)
        {
            // Additive identity
            _T sum = 0;
            for (usize i = 0; i < _N; i++) {
                sum += vec[i];
            }
            return sum;
        }

        _T op_vec_dot (Vector<_N, _T> const & lhs, Vector<_N, _T> const & rhs)
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
    struct Formatter<::Ynk::Vector<_N, _T>>
    {
        static void format (::Ynk::Vector<_N, _T> vec, FormatContext ctx)
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
