//
// file ynk/atomic/Atomic.h
// author Maximilien A. Cura <ushabti.cms@gmail.com>
//

#ifndef __YNK_ATOMIC_ATOMIC
#define __YNK_ATOMIC_ATOMIC

#include <Ynk/lang/NonCopyable.h>
#include <Ynk/lang/NonMoveable.h>

#include <Ynk/atomic/Ordering.h>
#include <stdlib.h> /* abort */

namespace Ynk::Atomic {
#if COMPILER_HAS_CLANG_FEATURE(c_atomic)
    template <typename T>
    struct Atomic {

        YNK_MAKE_NONCOPYABLE (Atomic);
        YNK_MAKE_NONMOVEABLE (Atomic);

        _Atomic volatile T inner;

        Atomic ()
        {
            __c11_atomic_init (&inner, static_cast<T> (0));
        }

        Atomic (T initial)
        {
            __c11_atomic_init (&inner, initial);
        }

        ~Atomic ()
        {}

        void store (T const value, const Ordering order = Ordering::Relaxed)
        {
            switch (order) {
                case Ordering::Relaxed: __c11_atomic_store (&inner, value, __ATOMIC_RELAXED); break;
                case Ordering::Release: __c11_atomic_store (&inner, value, __ATOMIC_RELEASE); break;
                case Ordering::SeqCst: __c11_atomic_store (&inner, value, __ATOMIC_SEQ_CST); break;
                default:
                    // tg-panic! invalid ordering on atomic store
                    abort ();
            }
        }

        T load (const Ordering order = Ordering::Relaxed)
        {
            switch (order) {
                case Ordering::Relaxed: return __c11_atomic_load (&inner, __ATOMIC_RELAXED);
                case Ordering::Acquire: return __c11_atomic_load (&inner, __ATOMIC_ACQUIRE);
                case Ordering::Consume: return __c11_atomic_load (&inner, __ATOMIC_CONSUME);
                case Ordering::SeqCst: return __c11_atomic_load (&inner, __ATOMIC_SEQ_CST);
                default:
                    // tg-panic! invalid ordering on atomic load
                    abort ();
            }
        }

        T swap (T const value, const Ordering order = Ordering::Relaxed)
        {
            switch (order) {
                case Ordering::Relaxed: return __c11_atomic_exchange (&inner, value, __ATOMIC_RELAXED);
                case Ordering::Release: return __c11_atomic_exchange (&inner, value, __ATOMIC_RELEASE);
                case Ordering::Acquire: return __c11_atomic_exchange (&inner, value, __ATOMIC_ACQUIRE);
                case Ordering::Consume: return __c11_atomic_exchange (&inner, value, __ATOMIC_CONSUME);
                case Ordering::AcqRel: return __c11_atomic_exchange (&inner, value, __ATOMIC_ACQ_REL);
                case Ordering::SeqCst: return __c11_atomic_exchange (&inner, value, __ATOMIC_SEQ_CST);
                default:
                    // tg-panic! invalid ordering on atomic swap
                    abort ();
            }
        }

        bool compare_and_swap (T expect, T desire, const Ordering order = Ordering::Relaxed)
        {
            switch (order) {
                case Ordering::Release: return this->compare_exchange (expect, desire, order, Ordering::Relaxed);
                case Ordering::Consume: return this->compare_exchange (expect, desire, order, Ordering::Relaxed);
                case Ordering::Relaxed: return this->compare_exchange (expect, desire, order, Ordering::Relaxed);
                case Ordering::SeqCst: return this->compare_exchange (expect, desire, order, Ordering::SeqCst);
                case Ordering::Acquire: return this->compare_exchange (expect, desire, order, Ordering::Acquire);
                case Ordering::AcqRel: return this->compare_exchange (expect, desire, order, Ordering::Acquire);
            }
        }

        bool compare_exchange (
            T expect,
            T desire,
            Ordering success,
            Ordering failure)
        {
#    define _Ynk_Atomic_Helper(a, b) return __c11_atomic_compare_exchange (&inner, expect, desire, a, b);
            // failure <= success !
            // relaxed, con, acq, rel, acqrel, seqcst
            if (failure == Ordering::Relaxed) {
                switch (success) {
                    case Ordering::Relaxed: _Ynk_Atomic_Helper (__ATOMIC_RELAXED, __ATOMIC_RELAXED) case Ordering::Consume : _Ynk_Atomic_Helper (__ATOMIC_CONSUME, __ATOMIC_RELAXED) case Ordering::Acquire : _Ynk_Atomic_Helper (__ATOMIC_ACQUIRE, __ATOMIC_RELAXED) case Ordering::Release : _Ynk_Atomic_Helper (__ATOMIC_RELEASE, __ATOMIC_RELAXED) case Ordering::AcqRel : _Ynk_Atomic_Helper (__ATOMIC_ACQ_REL, __ATOMIC_RELAXED) case Ordering::SeqCst : _Ynk_Atomic_Helper (__ATOMIC_SEQ_CST, __ATOMIC_RELAXED) default :
                        // tg-panic! invalid memory ordering
                        abort ();
                }
            } else if (failure == Ordering::Acquire) {
                switch (success) {
                    case Ordering::Acquire: _Ynk_Atomic_Helper (__ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE) case Ordering::Release : _Ynk_Atomic_Helper (__ATOMIC_RELEASE, __ATOMIC_ACQUIRE) case Ordering::AcqRel : _Ynk_Atomic_Helper (__ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE) case Ordering::SeqCst : _Ynk_Atomic_Helper (__ATOMIC_SEQ_CST, __ATOMIC_ACQUIRE) default :
                        // tg-panic! invalid memory ordering
                        abort ();
                }
            } else {
                // tg-panic! invalid memory ordering
                abort ();
            }
#    undef _Ynk_Atomic_Helper
        }

        bool compare_exchange_weak (
            T expect,
            T desire,
            Ordering success,
            Ordering failure)
        {
#    define _Ynk_Atomic_Helper(a, b) \
        return __c11_atomic_compare_exchange_weak (&inner, expect, desire, a, b);
            // failure <= success !
            // relaxed, con, acq, rel, acqrel, seqcst
            if (failure == Ordering::Relaxed) {
                switch (success) {
                    case Ordering::Relaxed: _Ynk_Atomic_Helper (__ATOMIC_RELAXED, __ATOMIC_RELAXED) case Ordering::Consume : _Ynk_Atomic_Helper (__ATOMIC_CONSUME, __ATOMIC_RELAXED) case Ordering::Acquire : _Ynk_Atomic_Helper (__ATOMIC_ACQUIRE, __ATOMIC_RELAXED) case Ordering::Release : _Ynk_Atomic_Helper (__ATOMIC_RELEASE, __ATOMIC_RELAXED) case Ordering::AcqRel : _Ynk_Atomic_Helper (__ATOMIC_ACQ_REL, __ATOMIC_RELAXED) case Ordering::SeqCst : _Ynk_Atomic_Helper (__ATOMIC_SEQ_CST, __ATOMIC_RELAXED) default :
                        // tg-panic! invalid memory ordering
                        abort ();
                }
            } else if (failure == Ordering::Acquire) {
                switch (success) {
                    case Ordering::Acquire: _Ynk_Atomic_Helper (__ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE) case Ordering::Release : _Ynk_Atomic_Helper (__ATOMIC_RELEASE, __ATOMIC_ACQUIRE) case Ordering::AcqRel : _Ynk_Atomic_Helper (__ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE) case Ordering::SeqCst : _Ynk_Atomic_Helper (__ATOMIC_SEQ_CST, __ATOMIC_ACQUIRE) default :
                        // tg-panic! invalid memory ordering
                        abort ();
                }
            } else {
                // tg-panic! invalid memory ordering
                abort ();
            }
#    undef _Ynk_Atomic_Helper
        }

        T fetch_add (T const value, const Ordering order = Ordering::Relaxed)
        {
            switch (order) {
                case Ordering::Relaxed: return __c11_atomic_fetch_add (&inner, value, __ATOMIC_RELAXED);
                case Ordering::Release: return __c11_atomic_fetch_add (&inner, value, __ATOMIC_RELEASE);
                case Ordering::Acquire: return __c11_atomic_fetch_add (&inner, value, __ATOMIC_ACQUIRE);
                case Ordering::Consume: return __c11_atomic_fetch_add (&inner, value, __ATOMIC_CONSUME);
                case Ordering::AcqRel: return __c11_atomic_fetch_add (&inner, value, __ATOMIC_ACQ_REL);
                case Ordering::SeqCst: return __c11_atomic_fetch_add (&inner, value, __ATOMIC_SEQ_CST);
                default:
                    // tg-panic! invalid ordering on atomic add
                    abort ();
            }
        }

        T fetch_sub (T const value, const Ordering order = Ordering::Relaxed)
        {
            switch (order) {
                case Ordering::Relaxed: return __c11_atomic_fetch_sub (&inner, value, __ATOMIC_RELAXED);
                case Ordering::Release: return __c11_atomic_fetch_sub (&inner, value, __ATOMIC_RELEASE);
                case Ordering::Acquire: return __c11_atomic_fetch_sub (&inner, value, __ATOMIC_ACQUIRE);
                case Ordering::Consume: return __c11_atomic_fetch_sub (&inner, value, __ATOMIC_CONSUME);
                case Ordering::AcqRel: return __c11_atomic_fetch_sub (&inner, value, __ATOMIC_ACQ_REL);
                case Ordering::SeqCst: return __c11_atomic_fetch_sub (&inner, value, __ATOMIC_SEQ_CST);
                default:
                    // tg-panic! invalid ordering on atomic sub
                    abort ();
            }
        }

        T fetch_and (T const value, const Ordering order = Ordering::Relaxed)
        {
            switch (order) {
                case Ordering::Relaxed: return __c11_atomic_fetch_and (&inner, value, __ATOMIC_RELAXED);
                case Ordering::Release: return __c11_atomic_fetch_and (&inner, value, __ATOMIC_RELEASE);
                case Ordering::Acquire: return __c11_atomic_fetch_and (&inner, value, __ATOMIC_ACQUIRE);
                case Ordering::Consume: return __c11_atomic_fetch_and (&inner, value, __ATOMIC_CONSUME);
                case Ordering::AcqRel: return __c11_atomic_fetch_and (&inner, value, __ATOMIC_ACQ_REL);
                case Ordering::SeqCst: return __c11_atomic_fetch_and (&inner, value, __ATOMIC_SEQ_CST);
                default:
                    // tg-panic! invalid ordering on atomic and
                    abort ();
            }
        }

        T fetch_or (T const value, const Ordering order = Ordering::Relaxed)
        {
            switch (order) {
                case Ordering::Relaxed: return __c11_atomic_fetch_or (&inner, value, __ATOMIC_RELAXED);
                case Ordering::Release: return __c11_atomic_fetch_or (&inner, value, __ATOMIC_RELEASE);
                case Ordering::Acquire: return __c11_atomic_fetch_or (&inner, value, __ATOMIC_ACQUIRE);
                case Ordering::Consume: return __c11_atomic_fetch_or (&inner, value, __ATOMIC_CONSUME);
                case Ordering::AcqRel: return __c11_atomic_fetch_or (&inner, value, __ATOMIC_ACQ_REL);
                case Ordering::SeqCst: return __c11_atomic_fetch_or (&inner, value, __ATOMIC_SEQ_CST);
                default:
                    // tg-panic! invalid ordering on atomic or
                    abort ();
            }
        }

        T fetch_xor (T const value, const Ordering order = Ordering::Relaxed)
        {
            switch (order) {
                case Ordering::Relaxed: return __c11_atomic_fetch_xor (&inner, value, __ATOMIC_RELAXED);
                case Ordering::Release: return __c11_atomic_fetch_xor (&inner, value, __ATOMIC_RELEASE);
                case Ordering::Acquire: return __c11_atomic_fetch_xor (&inner, value, __ATOMIC_ACQUIRE);
                case Ordering::Consume: return __c11_atomic_fetch_xor (&inner, value, __ATOMIC_CONSUME);
                case Ordering::AcqRel: return __c11_atomic_fetch_xor (&inner, value, __ATOMIC_ACQ_REL);
                case Ordering::SeqCst: return __c11_atomic_fetch_xor (&inner, value, __ATOMIC_SEQ_CST);
                default:
                    // tg-panic! invalid ordering on atomic xor
                    abort ();
            }
        }
    };
#else /* __has_feature(c_atomic) */
#    error "Ynk currently only has support for clang's __c11_atomic_* builtins...; support for GCC-style __atomic_* is pending"
#endif
}

#endif
