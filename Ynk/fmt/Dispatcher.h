//
// file ynk/fmt/Dispatcher.h
// author Maximilien M. Cura
//

#ifndef __YNK_FMT_DISPATCHER
#define __YNK_FMT_DISPATCHER

#include <Ynk/num/Integers.h>

#include <cstdlib>
#include <errno.h>
#include <inttypes.h>

namespace Ynk::Fmt {
    enum class Dispatch {
        Display,
        Debug,
        /* Not supported yet */
        // Octal,
        // LowerHex,
        // UpperHex,
        // Pointer,
    };

    template <class T, Dispatch type>
    struct DispatchImpl
    {};

#define _Ynk_intfmt(_T, _C, _F, _P)                                                 \
    template <>                                                                     \
    struct DispatchImpl<_T, Dispatch::_F>                                           \
    {                                                                               \
        static char * dispatch (_T & object)                                        \
        {                                                                           \
            char * ret;                                                             \
            const int r = asprintf (&ret, _P, (_C)object);                          \
            if (!r) {                                                               \
                return ret;                                                         \
            } else {                                                                \
                errno = r;                                                          \
                perror ("Ynk: DispatchImpl::dispatch() failed (asprintf failure)"); \
                std::abort ();                                                      \
            }                                                                       \
        }                                                                           \
    }

    //
    // Display
    //

    struct Display
    {
        virtual char * display () = 0;
    };

    template <>
    struct DispatchImpl<Display, Dispatch::Display>
    {
        static char * dispatch (Display & object)
        {
            return object.display ();
        }
    };

    _Ynk_intfmt (u8, _u8, Display, PRIu8);
    _Ynk_intfmt (u16, _u16, Display, PRIu16);
    _Ynk_intfmt (u32, _u32, Display, PRIu32);
    _Ynk_intfmt (u64, _u64, Display, PRIu64);

    _Ynk_intfmt (i8, _i8, Display, PRIi8);
    _Ynk_intfmt (i16, _i16, Display, PRIi16);
    _Ynk_intfmt (i32, _i32, Display, PRIi32);
    _Ynk_intfmt (i64, _i64, Display, PRIi64);

    //
    // Debug
    //

    struct Debug
    {
        virtual char * debug () = 0;
    };

    template <>
    struct DispatchImpl<Debug, Dispatch::Debug>
    {
        static char * dispatch (Debug & object)
        {
            return object.debug ();
        }
    };

    _Ynk_intfmt (u8, _u8, Debug, PRIu8);
    _Ynk_intfmt (u16, _u16, Debug, PRIu16);
    _Ynk_intfmt (u32, _u32, Debug, PRIu32);
    _Ynk_intfmt (u64, _u64, Debug, PRIu64);

    _Ynk_intfmt (i8, _i8, Debug, PRIi8);
    _Ynk_intfmt (i16, _i16, Debug, PRIi16);
    _Ynk_intfmt (i32, _i32, Debug, PRIi32);
    _Ynk_intfmt (i64, _i64, Debug, PRIi64);

#undef _Ynk_intfmt

    template <class T, Dispatch type>
    char * dispatch (T & object)
    {
        return DispatchImpl<T, type>::dispatch (object);
    }

}

#endif /* !@__YNK_FMT_DISPATCHER */
