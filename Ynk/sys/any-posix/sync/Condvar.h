//
// file ynk/sys/any-posix/sync/Condvar.h
// author Maximilien M. Cura
//

#ifndef __YNK_SYS_ANY_POSIX_SYNC_CONDVAR
#define __YNK_SYS_ANY_POSIX_SYNC_CONDVAR

#include <pthread.h>

#include <Ynk/lang/NonCopyable.h>
#include <Ynk/lang/NonMoveable.h>

#include <Ynk/sys/any-posix/sync/Mutex.h>

namespace Ynk::Sys::Sync {
    struct Condvar
    {
        pthread_cond_t inner;

        YNK_MAKE_NON_COPYABLE (Condvar);
        YNK_MAKE_NON_MOVEABLE (Condvar);

        Condvar ();
        ~Condvar ();

        void notify_one ();
        void notify_all ();
        void wait (Ynk::Sys::Sync::Mutex & m);
    }
}

#endif /* !@__YNK_SYS_ANY_POSIX_SYNC_CONDVAR */
