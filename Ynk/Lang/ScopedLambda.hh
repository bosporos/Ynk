//
// file ynk/lang/ScopedLambda.hh
// author Maximilien M. Cura
//
// Based directly on WTF's ScopedLambda*
//

#ifndef __YNK_LANG_SCOPEDLAMBDA
#define __YNK_LANG_SCOPEDLAMBDA

#include <Ynk/Lang/Move.hh>
#include <Ynk/Lang/Forward.hh>

// From Mozilla WTF

namespace Ynk {
    template <typename FunctionType> struct ScopedLambda;
    template <typename ResultType, typename... ArgumentTypes>
    struct ScopedLambda<ResultType (ArgumentTypes...)> {

        ResultType (*impl) (void * arg, ArgumentTypes...);
        void * arg;

        ScopedLambda (ResultType (*impl) (void * arg, ArgumentTypes...) = nullptr, void * arg = nullptr)
            : impl (impl)
            , arg { arg }
        {}

        template <typename... InvocationArgumentTypes>
        ResultType operator() (InvocationArgumentTypes &&... arguments) const
        {
            return impl (arg, Ynk::Forward<InvocationArgumentTypes> (arguments)...);
        }
    };

    template <typename FunctionType, typename Functor> struct ScopedLambdaFunctor;
    template <typename ResultType, typename... ArgumentTypes, typename Functor>
    struct ScopedLambdaFunctor<ResultType (ArgumentTypes...), Functor>
        : public ScopedLambda<ResultType (ArgumentTypes...)> {

        Functor functor;

        template <typename InvocationFunctor>
        ScopedLambdaFunctor (InvocationFunctor && functor)
            : ScopedLambda<ResultType (ArgumentTypes...)> (impl_fn, this)
            , functor (Ynk::Forward<InvocationFunctor> (functor))
        {}

        ScopedLambdaFunctor (const ScopedLambdaFunctor & other)
            : ScopedLambda<ResultType (ArgumentTypes...)> (impl_fn, this)
            , functor (other.impl)
        {}

        ScopedLambdaFunctor (ScopedLambdaFunctor && other)
            : ScopedLambda<ResultType (ArgumentTypes...)> (impl_fn, this)
            , functor (Ynk::Move (other))
        {}

        ScopedLambdaFunctor & operator= (const ScopedLambdaFunctor & other)
        {
            functor = other.functor;
            return *this;
        }

        ScopedLambdaFunctor & operator= (ScopedLambdaFunctor && other)
        {
            functor = Ynk::Move (other.functor);
            return *this;
        }

        static ResultType impl_fn (void * self, ArgumentTypes... arguments)
        {
            return static_cast<ScopedLambdaFunctor *> (self)->functor (arguments...);
        }
    };

    template <typename FnType, typename Functor>
    ScopedLambdaFunctor<FnType, Functor> scoped_lambda (Functor const & functor)
    {
        return ScopedLambdaFunctor<FnType, Functor> (functor);
    }

    template <typename FnType, typename Functor>
    ScopedLambdaFunctor<FnType, Functor> scoped_lambda (Functor && functor)
    {
        return ScopedLambdaFunctor<FnType, Functor> (Ynk::Move (functor));
    }

    template <typename FnType, typename Functor> struct ScopedLambdaRefFunctor;
    template <typename Result Type, typename... ArgumentTypes, typename Functor>
    struct ScopedLambdaRefFunctor<ResultType (ArgumentTypes...), Functor> : public ScopedLambda<ResultType (ArgumentTypes...)> {
        const Functor * functor;

        ScopedLambdaRefFunctor (Functor const & functor)
            : ScopedLambda<ResultType (ArgumentTypes...)> (impl_fn, this)
            , functor (&functor)
        {}

        ScopedLambdaRefFunctor (ScopedLambdaRefFunctor const & other)
            : ScopedLambda<ResultType (ArgumentTypes...)> (impl_fn, this)
            , functor (other.functor)
        {}

        ScopedLambdaRefFunctor (ScopedLambdaRefFunctor && other)
            : ScopedLambda<ResultType (ArgumentTypes...)> (impl_fn, this)
            , functor (other.functor)
        {}

        ScopedLambdaRefFunctor & operator= (ScopedLambdaRefFunctor const & other)
        {
            functor = other.functor;
            return *this;
        }

        ScopedLambdaRefFunctor & operator= (ScopedLambdaRefFunctor && other)
        {
            functor = other.functor;
            return *this;
        }

        static ResultType impl_fn (void * self, ArgumentTypes... arguments)
        {
            return (*static_cast<ScopedLambdaRefFunctor *> (self)->functor) (arguments...);
        }
    };

    template <typename FnType, typename Functor>
    ScopedLambdaRefFunctor<FnType, Functor> scoped_lambda_ref (Functor const & functor)
    {
        return ScopedLambdaRefFunctor<FnType, Functor> (functor);
    }
}

#endif
