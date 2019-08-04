//
// file ynk/sys/common/sync/ThreadId.h
// author Maximilien M. Cura
//

#ifndef __YNK_SYS_SYNC_THREADID
#define __YNK_SYS_SYNC_THREADID

#include <Ynk/Platform.h>
#include <Ynk/Features.h>

#include <Ynk/Num/NativeIntegers.h>

namespace Ynk::Sys::Sync {
    struct ThreadId
    {
        _u64 inner;

        ThreadId ();

        void bind ();
        bool current ();

        bool operator== (ThreadId const & rhs);
    };
}

#endif
