//
// file ynk/sys/common/Sync/RwLock.h
// author Maximilien M. Cura
//

#ifndef __YNK_SYS_SYNC_RWLOCK
#define __YNK_SYS_SYNC_RWLOCK

#include <Ynk/Platform.h>
#include <Ynk/Features.h>

#if OS(POSIX)
#    include <Ynk/sys/any-posix/Sync/RwLock.h>
#else
#    error "At present, Ynk only supports pthread synchronization primitives"
#endif

#endif /* !@__YNK_SYS_SYNC_RWLOCK */
