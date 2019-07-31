//
// file ynk/sys/common/sync/Condvar.h
// author Maximilien M. Cura
//

#ifndef __YNK_SYS_SYNC_CONDVAR
#define __YNK_SYS_SYNC_CONDVAR

#if OS(POSIX)
#    include <Ynk/sys/any-posix/sync/Condvar.h>
#else
#    error "At present, Ynk only supports pthread synchronization primitives"
#endif

#endif /* !@__YNK_SYS_SYNC_CONDVAR */
