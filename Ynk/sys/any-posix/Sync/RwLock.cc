//
// file ynk/sys/any-posix/Sync/RwLock.cc
// author Maximilien M. Cura
//

#include <Ynk/sys/any-posix/Sync/RwLock.hh>
#include <Ynk/Atomic/Ordering.hh>
#include <Ynk/Utility.hh>

#include <errno.h>

using Ynk::Atomic::Ordering;
using Ynk::Sys::Sync::RwLock;

RwLock::RwLock ()
    : num_readers { 0 }
    , write_locked { false }
{
    inner = PTHREAD_RWLOCK_INITIALIZER;
}

RwLock::~RwLock ()
{
    YNK_UNUSED int r = pthread_rwlock_destroy (&this->inner);
    // debug_assert_eq(r, 0);
}

void RwLock::read ()
{
    int r = pthread_rwlock_rdlock (&this->inner);
    if (r == EAGAIN) {
        // @TODO: make this a panic: exceeded max number of readers
        abort ();
    } else if (r == EDEADLK || this->write_locked) {
        if (r == 0) {
            this->raw_unlock ();
        }
        // @TODO: make this a panic: would result in deadlock
        abort ();
    } else {
        // debug_assert_eq (r, 0);
        this->num_readers.fetch_add (1, Ordering::Relaxed);
    }
}

bool RwLock::try_read ()
{
    int r = pthread_rwlock_tryrdlock (&this->inner);
    if (r == 0) {
        if (this->write_locked) {
            this->raw_unlock ();
            return false;
        } else {
            this->num_readers.fetch_add (1, Ordering::Relaxed);
            return true;
        }
    } else {
        return false;
    }
}

void RwLock::write ()
{
    int r = pthread_rwlock_wrlock (&this->inner);
    if (r == EDEADLK || this->write_locked || this->num_readers.load (Ordering::Relaxed) != 0) {
        if (r == 0) {
            this->raw_unlock ();
        }
        // @TODO: make this a panic: would result in deadlock
        abort ();
    } else {
        // debug_assert_eq (r, 0);
    }
    this->write_locked = true;
}

bool RwLock::try_write ()
{
    int r = pthread_rwlock_trywrlock (&this->inner);
    if (r == 0) {
        if (this->write_locked || this->num_readers.load (Ordering::Relaxed) != 0) {
            this->raw_unlock ();
            return false;
        } else {
            this->write_locked = true;
            return true;
        }
    } else {
        return false;
    }
}

void RwLock::raw_unlock ()
{
    YNK_UNUSED const int r = pthread_rwlock_unlock (&this->inner);
    // debug_assert_eq (r, 0);
}

void RwLock::read_unlock ()
{
    // debug_assert (!this->write_locked);
    this->num_readers.fetch_sub (1, Ordering::Relaxed);
    this->raw_unlock ();
}

void RwLock::write_unlock ()
{
    // debug_assert_eq (this->num_readers.load (Ordering::Relaxed), 0);
    // debug_assert (this->write_locked);
    this->write_locked = false;
    this->raw_unlock ();
}
