//
// file ynk/sys/macos/sync/ThreadId.cc
// author Maximilien M. Cura
//

#include <Ynk/Utility.hh>
#include <Ynk/sys/common/Sync/ThreadId.hh>

#if COMPILER(CLANG)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wexpansion-to-defined"
#endif

#include <pthread.h>

#if COMPILER(CLANG)
#    pragma clang diagnostic pop
#endif

YNK_VEILED
static Ynk::_u64 __YNK_THREAD_ID__ ()
{
    Ynk::_u64 tid_tmp      = 0;
    YNK_UNUSED const int r = pthread_threadid_np (NULL, &tid_tmp);
    // debug_assert_eq(r, 0);
    return tid_tmp;
}

YNK_VEILED
static const thread_local Ynk::_u64 __THREAD_ID__ = __YNK_THREAD_ID__ ();

using ::Ynk::Sys::Sync::ThreadId;

ThreadId::ThreadId ()
    : inner { __THREAD_ID__ }
{}

void ThreadId::bind ()
{
    this->inner = __THREAD_ID__;
}

bool ThreadId::current ()
{
    return __THREAD_ID__ == this->inner;
}

bool ThreadId::operator== (ThreadId const & rhs)
{
    return this->inner == rhs.inner;
}
