//
// file ynk/sys/any-posix/Sync/Mutex.cc
// author Maximilien M. Cura
//

#include <Ynk/sys/any-posix/Sync/Mutex.hh>
#include <Ynk/Utility.hh>

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
    YNK_UNUSED const int r = pthread_mutex_destroy (&this->inner);
}

void Mutex::lock ()
{
    YNK_UNUSED const int r = pthread_mutex_lock (&this->inner);
}

void Mutex::unlock ()
{
    YNK_UNUSED const int r = pthread_mutex_unlock (&this->inner);
}

bool Mutex::try_lock ()
{
    return 0 == pthread_mutex_trylock (&this->inner);
}
