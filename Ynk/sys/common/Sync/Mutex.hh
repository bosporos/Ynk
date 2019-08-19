//
// file ynk/sys/common/Sync/Mutex.hh
// author Maximilien M. Cura
//

#ifndef __YNK_SYS_SYNC_MUTEX
#define __YNK_SYS_SYNC_MUTEX

#include <Ynk/Platform.hh>
#include <Ynk/Features.hh>

#if OS(POSIX)
#    include <Ynk/sys/any-posix/Sync/Mutex.hh>
#else
#    error "At present, Ynk only supports pthread synchronization primitives"
#endif

#endif /* !@__YNK_SYS_SYNC_MUTEX */
