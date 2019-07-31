//
// file ynk/sys/any-posix/sync/RwLock.h
// author Maximilien M. Cura
//

#ifndef __YNK_SYS_ANY_POSIX_SYNC_RWLOCK
#define __YNK_SYS_ANY_POSIX_SYNC_RWLOCK

#include <pthread.h>

#include <Ynk/lang/NonCopyable.h>
#include <Ynk/lang/NonMoveable.h>

#include <Ynk/atomic/Atomic.h>

namespace Ynk::Sys::Sync {
    struct RwLock
    {
        pthread_rwlock_t inner;
        Atomic::Atomic<Ynk::_usize> num_readers;
        volatile bool write_locked;

        YNK_MAKE_NON_COPYABLE (RwLock);
        YNK_MAKE_NON_MOVEABLE (RwLock);

        RwLock ();
        ~RwLock ();

        void read ();
        bool try_read ();
        void write ();
        bool try_write ();

        void raw_unlock ();
        void read_unlock ();
        void write_unlock ();
    };
}

#endif /* !@__YNK_SYS_ANY_POSIX_SYNC_RWLOCK */
