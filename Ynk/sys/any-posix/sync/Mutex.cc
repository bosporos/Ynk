//
// file ynk/sys/any-posix/sync/Mutex.cc
// author Maximilien M. Cura
//

#include <Ynk/sys/any-posix/sync/Mutex.h>

using Ynk::Sys::Sync::Mutex;

Mutex::Mutex ()
{
    inner = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutexattr_t mattr;
    int r = pthread_mutexattr_init (&mattr);
    r     = pthread_mutexattr_settype (&mattr, PTHREAD_MUTEX_NORMAL);
    r     = pthread_mutex_init (&this->inner, &mattr);
    r     = pthread_mutexattr_destroy (&mattr);
}

Mutex::~Mutex ()
{
    const int r = pthread_mutex_destroy (&this->inner);
}

void Mutex::lock ()
{
    const int r = pthread_mutex_lock (&this->inner);
}

void Mutex::unlock ()
{
    const int r = pthread_mutex_unlock (&this->inner);
}

bool Mutex::try_lock ()
{
    return 0 == pthread_mutex_trylock (&this->inner);
}
