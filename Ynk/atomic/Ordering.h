//
// file petra/atomic/Ordering.h
// author Maximilien A. Cura <ushabti.cms@gmail.com>
//

#ifndef __PETRA_ATOMIC_ORDERING
#define __PETRA_ATOMIC_ORDERING

#include <petra/numeric/NativeIntegers.h>

namespace Petra::Atomic {
    enum class Ordering : _u8 {
#if defined(__ATOMIC_RELAXED)
        Relaxed = __ATOMIC_RELAXED,
        Consume = __ATOMIC_CONSUME,
        Acquire = __ATOMIC_ACQUIRE,
        Release = __ATOMIC_RELEASE,
        AcqRel  = __ATOMIC_ACQ_REL,
        SeqCst  = __ATOMIC_SEQ_CST,
#else
#    error "Petra requires the __ATOMIC_* macros & __atomic_* builtins"
#endif
    }
}

#endif
