//
// file ynk/sys/common/sync/ThreadId.hh
// author Maximilien M. Cura
//

#ifndef __YNK_SYS_SYNC_THREADID
#define __YNK_SYS_SYNC_THREADID

#include <Ynk/Platform.hh>
#include <Ynk/Features.hh>

#include <Ynk/Num/NativeIntegers.hh>

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
