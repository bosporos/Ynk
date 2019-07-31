//
// file Ynk/rc/Rc.h
// author Maximilien M. Cura
//

#ifndef __YNK_RC_RC
#define __YNK_RC_RC

// #include "common.hpp"
// #include "panic/panic.hpp"
#include <Ynk/lang/Conditional.h>
#include <Ynk/lang/Traits.h>
#include <Ynk/lang/Move.h>

#include <Ynk/option/Option.h>
#include <Ynk/box/Box.h>

#include <new>
#include <type_traits>
#include <cstdlib>
#include <cstring>

namespace Ynk {

    template <class T>
    struct Box;

    template <class T, typename U = usize>
    struct Rc
    {
        T * inner;
        static Rc<T, U> take (const T * ptr)
        {
            Rc<T, U> tmp;
            tmp.inner = const_cast<T *> (ptr);
            try {
                // It doesn't carry data, so it's okay.
                //  31 July 2019: What's OK?
                tmp.count = new U ();
            } catch (std::bad_alloc) {
                std::fprintf (stderr, "could not create rc: out of memory");
                std::abort ();
                // panic ("could not create rc: out of memory");
            }
            tmp.retain ();
            return Ynk::Move (tmp);
        }
        static Rc<T, U> take (typename Ynk::Conditional<std::is_integral<T>::value, T, T &&>::type obj)
        {
            Rc<T, U> tmp;
            try {
                u8 * ptr  = new u8[sizeof (T)];
                tmp.inner = reinterpret_cast<T *> (ptr);
            } catch (std::bad_alloc) {
                // panic ("could not create rc: out of memory");
                std::fprintf (stderr, "could not create rc: out of memory");
                std::abort ();
            }
            std::memmove (tmp.inner, &obj, sizeof (T));
            try {
                tmp.count = new U ();
            } catch (std::bad_alloc) {
                delete tmp.inner;
                // panic ("could not create rc: out of memory");
                std::fprintf (stderr, "could not create rc: out of memory");
                std::abort ();
            }
            tmp.retain ();
            return Ynk::Move (tmp);
        }

        Rc (Rc<T, U> && o)
            : inner { o.inner }
            , count { o.count }
        {
            o.inner = nullptr;
            o.count = nullptr;
        }

        Rc (Rc<T, U> const & o)
            : inner { o.inner }
            , count { o.count }
        {
            this->retain ();
        }
        ~Rc ()
        {
            this->release ();
        }
        U get_num_refs ()
        {
            if (count != nullptr) {
                return *count;
            } else {
                return static_cast<U> (0);
            }
        }
        T * as_ptr ()
        {
            return inner;
        }
        T & operator* ()
        {
            return *inner;
        }
        bool is_unique ()
        {
            return this->get_num_refs () == 0;
        }
        bool is_empty ()
        {
            return count == nullptr;
        }
        T * operator-> ()
        {
            return inner;
        }
        Option<Box<T>> as_box ()
        {
            if (is_unique ()) {
                Box<T> tmp (Box<T>::take (inner));
                inner = nullptr;
                this->release ();
                count = nullptr;
                return Option (Ynk::Move (tmp));
            } else {
                return Option<Box<T>> ();
            }
        }

    private:
        U * count;

        Rc ()
        {
        }

        void release ()
        {
            if (count != nullptr) {
                *count--;
                if (*count == 0) {
                    if (inner != nullptr) {
                        delete inner;
                    }
                    delete count;
                }
            }
        }

        void retain ()
        {
            if (count != nullptr) {
                *count++;
            }
        }
    };

}

#endif /* !@__YNK_RC_RC */
