//
// file ynk/atomic/Ordering.h
// author Maximilien M. Cura
//

#ifndef __YNK_ATOMIC_ORDERING
#define __YNK_ATOMIC_ORDERING

#include <Ynk/num/NativeIntegers>

namespace Ynk::Atomic {
    enum class Ordering : _u8 {
#if defined(__ATOMIC_RELAXED)
        Relaxed = __ATOMIC_RELAXED,
        Consume = __ATOMIC_CONSUME,
        Acquire = __ATOMIC_ACQUIRE,
        Release = __ATOMIC_RELEASE,
        AcqRel  = __ATOMIC_ACQ_REL,
        SeqCst  = __ATOMIC_SEQ_CST,
#else
#    error "Ynk requires the __ATOMIC_* macros & __atomic_* builtins"
#endif
    }
}

#endif
