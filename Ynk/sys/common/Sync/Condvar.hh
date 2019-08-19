//
// file ynk/sys/common/Sync/Condvar.hh
// author Maximilien M. Cura
//

#ifndef __YNK_SYS_SYNC_CONDVAR
#define __YNK_SYS_SYNC_CONDVAR

#include <Ynk/Platform.hh>
#include <Ynk/Features.hh>

#if OS(POSIX)
#    include <Ynk/sys/any-posix/Sync/Condvar.hh>
#else
#    error "At present, Ynk only supports pthread synchronization primitives"
#endif

#endif /* !@__YNK_SYS_SYNC_CONDVAR */
