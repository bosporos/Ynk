//
// file ynk/sys/any-posix/Sync/Condvar.cc
// author Maximilien M. Cura
//

#include <Ynk/Sys/any-posix/Sync/Condvar.h>

using Ynk::Sys::Sync::Condvar;

Condvar::Condvar ()
{
    inner = PTHREAD_COND_INITIALIZER;
}

Condvar::~Condvar ()
{
    const int r = pthread_cond_destroy (&this->inner);
}

void Condvar::notify_one ()
{
    const int r = pthread_cond_signal (&this->inner);
}

void Condvar::notify_all ()
{
    const int r = pthread_cond_broadcast (&this->inner);
}

void Condvar::wait (Ynk::Sys::Sync::Mutex & m)
{
    m.lock ();
    const int r = pthread_cond_wait (&inner, &m.inner);
}
