//
// file ynk/sys/common/Sync/RwLock.hh
// author Maximilien M. Cura
//

#ifndef __YNK_SYS_SYNC_RWLOCK
#define __YNK_SYS_SYNC_RWLOCK

#include <Ynk/Platform.hh>
#include <Ynk/Features.hh>

#if OS(POSIX)
#    include <Ynk/sys/any-posix/Sync/RwLock.hh>
#else
#    error "At present, Ynk only supports pthread synchronization primitives"
#endif

#endif /* !@__YNK_SYS_SYNC_RWLOCK */
