//
// file ynk/sys/any-posix/Sync/Condvar.cc
// author Maximilien M. Cura
//

#include <Ynk/sys/any-posix/Sync/Condvar.hh>
#include <Ynk/Utility.hh>

using Ynk::Sys::Sync::Condvar;

Condvar::Condvar ()
{
    inner = PTHREAD_COND_INITIALIZER;
}

Condvar::~Condvar ()
{
    YNK_UNUSED const int r = pthread_cond_destroy (&this->inner);
}

void Condvar::notify_one ()
{
    YNK_UNUSED const int r = pthread_cond_signal (&this->inner);
}

void Condvar::notify_all ()
{
    YNK_UNUSED const int r = pthread_cond_broadcast (&this->inner);
}

void Condvar::wait (Ynk::Sys::Sync::Mutex & m)
{
    m.lock ();
    YNK_UNUSED const int r = pthread_cond_wait (&inner, &m.inner);
}
