//
// file ynk/sys/any-posix/Sync/Mutex.hh
// author Maximilien M. Cura
//

#ifndef __YNK_SYS_ANY_POSIX_SYNC_MUTEX
#define __YNK_SYS_ANY_POSIX_SYNC_MUTEX

#include <pthread.h>

#include <Ynk/Lang/NonCopyable.hh>
#include <Ynk/Lang/NonMoveable.hh>

namespace Ynk::Sys::Sync {
    struct Mutex
    {
        pthread_mutex_t inner;

        YNK_MAKE_NON_COPYABLE (Mutex);
        YNK_MAKE_NON_MOVEABLE (Mutex);

        Mutex ();
        ~Mutex ();

        void lock ();
        bool try_lock ();
        void unlock ();
    };
}

#endif /* !@__YNK_SYS_ANY_POSIX_SYNC_MUTEX */
