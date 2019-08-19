//
// file ynk/result/Result.hh
// author Maximilien M. Cura
//

#ifndef __YNK_RESULT_RESULT
#define __YNK_RESULT_RESULT

#include <Ynk/Panic/Panic.hh>
#include <Ynk/Option/Option.hh>
#include <Ynk/Lang/Move.hh>

namespace Ynk {

    //! Helper struct returned by Ynk::result::Ok<T>
    //!
    //! May be passed by copy or move to Ynk::result::Result's ctor.
    template <class T>
    struct OkResultInitializer
    {
        //! Object of type `T` to be used as Ynk::result::Result's
        //! `result`.
        T inner;
    };

    //! Helper struct returned by Ynk::result::Err<T>
    //!
    //! May be passed by copy or move to Ynk::result::Result's ctor.
    template <class T>
    struct ErrResultInitializer
    {
        //! Object of type `T` to be used as Ynk::result::Result's
        //! `error`.
        T inner;
    };

    template <class T>
    constexpr OkResultInitializer<T> Ok (T const & x)
    {
        return { x };
    }

    template <class T>
    constexpr ErrResultInitializer<T> Err (T const & x)
    {
        return { x };
    }

    template <class T>
    constexpr OkResultInitializer<T> Ok (T && x)
    {
        return { Ynk::Move (x) };
    }

    template <class T>
    constexpr ErrResultInitializer<T> Err (T && x)
    {
        return { Ynk::Move (x) };
    }

    template <class R, class E>
    struct Result
    {
        union {
            R result;
            E error;
        };
        const bool is_error = 0;

        constexpr Result (OkResultInitializer<R> const & o)
            : result { o.inner }
            , is_error { 0 }
        {}

        constexpr Result (ErrResultInitializer<E> const & o)
            : error { o.inner }
            , is_error { 1 }
        {}

        Result (OkResultInitializer<R> && o)
            : result { Ynk::Move (o.inner) }
            , is_error { 0 }
        {}

        Result (ErrResultInitializer<E> && o)
            : error { Ynk::Move (o.inner) }
            , is_error { 1 }
        {}

        constexpr Result (Result<R, E> const & r)
            : is_error { r.is_error }
        {
            if (is_error) {
                new (&error) E (r.error);
            } else {
                new (&result) R (r.result);
            }
        }

        Result (Result<R, E> && r)
            : is_error { r.is_error }
        {
            if (is_error) {
                new (&error) E (Ynk::Move (r.error));
            } else {
                new (&result) R (Ynk::Move (r.result));
            }
        }
        ~Result ()
        {
            // Should not actually destroy error/result
            // A: If error/result is allocated solely on stack, and T is neither
            //      a reference nor an rvalue reference, then error/result will
            //      deallocate with the class
            // B: If error/result is allocated solely on stack, and T is a
            //      reference or an rvalue reference, then the original value
            //      shouldn't be destroyed here.
            // C: If T is a pointer, it's impossible to tell if error/result
            //      was allocated on heap (let alone malloc v. new), or is
            //      simply pointing to a durable object somewhere
        }
        constexpr R unwrap () const
        {
            if (is_error) {
                panic ("result::unwrap: is an error");
            }
            return Ynk::Move (result);
        }
        constexpr R unwrap_or (R default_value) const
        {
            return Ynk::Move (is_error ? default_value : result);
        }
        constexpr E unwrap_err () const
        {
            if (!is_error) {
                panic ("result::unwrap_err: isn't an error");
            }
            return Ynk::Move (result);
        }
        constexpr E unwrap_err_or (R default_value) const
        {
            return Ynk::Move (is_error ? default_value : error);
        }

        constexpr auto ok () const -> Option<R>
        {
            return !is_error ? Option<R> (result) : Option<R> ();
        }

        constexpr auto err () const -> Option<E>
        {
            return is_error ? Option<E> (error) : Option<E> ();
        }
        constexpr bool is_err () const { return is_error; }
    };
}

#endif /* !@__YNK_RESULT_RESULT */
