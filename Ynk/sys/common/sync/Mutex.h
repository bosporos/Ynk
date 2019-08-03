//
// file ynk/sys/common/Sync/Mutex.h
// author Maximilien M. Cura
//

#ifndef __YNK_SYS_SYNC_MUTEX
#define __YNK_SYS_SYNC_MUTEX

#if OS(POSIX)
#    include <Ynk/sys/any-posix/Sync/Mutex.h>
#else
#    error "At present, Ynk only supports pthread synchronization primitives"
#endif

#endif /* !@__YNK_SYS_SYNC_MUTEX */
