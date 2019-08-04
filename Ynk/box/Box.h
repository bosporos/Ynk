//
// file Ynk/box/Box.h
// author Maximilien M. Cura
//

#ifndef __YNK_BOX_BOX
#define __YNK_BOX_BOX

// #include "panic/panic.hpp"
#include <Ynk/Lang/Conditional.h>
#include <Ynk/Lang/Traits.h>
#include <Ynk/Lang/Move.h>

#include <new>
#include <type_traits>
#include <cstdlib>
#include <cstring>
#include <cstdio>

namespace Ynk {

    template <class T, class U>
    struct Rc;

    template <class T>
    struct Box
    {
        T * inner;

        static Box<T> take (const T * ptr)
        {
            Box<T> tmp;
            tmp.inner = const_cast<T *> (ptr);
            return Ynk::Move (tmp);
        }

        static Box<T> take (typename Ynk::Conditional<std::is_integral<T>::value, T, T &&>::type obj)
        {
            Box<T> tmp;
            try {
                u8 * ptr  = new u8[sizeof (T)];
                tmp.inner = reinterpret_cast<T *> (ptr);
            } catch (std::bad_alloc) {
                // panic ("could not create box: out of memory");
                std::fprintf (stderr, "could not create box: out of memory");
                std::abort ();
            }
            std::memmove (tmp.inner, &obj, sizeof (T));
            return Ynk::Move (tmp);
        }

        Box (Box<T> && o)
            : inner { o.inner }
        {
            o.inner = nullptr;
        }

        ~Box ()
        {
            if (inner != nullptr) {
                delete inner;
            }
        }

        T & operator* ()
        {
            return *inner;
        }

        T * as_ptr ()
        {
            return inner;
        }

        bool is_empty ()
        {
            return inner == nullptr;
        }

        T * operator-> ()
        {
            return inner;
        }

        template <class U = usize>
        Rc<T, U> as_rc ()
        {
            return Rc<T, U>::take (inner);
        }

    private:
        Box (Box<T> const & o) = delete;

        Box ()
        {
        }
    };
}

#endif /* !@__YNK_BOX_BOX */
