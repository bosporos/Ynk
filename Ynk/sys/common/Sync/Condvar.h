//
// file ynk/sys/common/Sync/Condvar.h
// author Maximilien M. Cura
//

#ifndef __YNK_SYS_SYNC_CONDVAR
#define __YNK_SYS_SYNC_CONDVAR

#include <Ynk/Platform.h>
#include <Ynk/Features.h>

#if OS(POSIX)
#    include <Ynk/sys/any-posix/Sync/Condvar.h>
#else
#    error "At present, Ynk only supports pthread synchronization primitives"
#endif

#endif /* !@__YNK_SYS_SYNC_CONDVAR */
