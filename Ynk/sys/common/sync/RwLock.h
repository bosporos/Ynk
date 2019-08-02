//
// file ynk/sys/common/sync/RwLock.h
// author Maximilien M. Cura
//

#ifndef __YNK_SYS_SYNC_RWLOCK
#define __YNK_SYS_SYNC_RWLOCK

#if OS(POSIX)
#    include <Ynk/sys/any-posix/sync/RwLock.h>
#else
#    error "At present, Ynk only supports pthread synchronization primitives"
#endif

#endif /* !@__YNK_SYS_SYNC_RWLOCK */