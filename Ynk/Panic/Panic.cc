//
// file ynk/panic/Panic.cc
// author Maximilien M. Cura
//

#include <Ynk/Utility.h>
#include <Ynk/Panic/Panic.h>
#include <Ynk/Fmt/Fmt.h>

#include <Ynk/sys/any-posix/Sync/RwLock.h>

#include <cstdio>

static YNK_HIDDEN ::Ynk::usize update_panic_count (::Ynk::isize);
static YNK_NORETURN void panic_impl (::Ynk::Panicking::PanicInfo &);

YNK_HIDDEN
static ::Ynk::Panic::HookFn __HOOK__ = ::Ynk::Panic::default_hook;
YNK_HIDDEN
static ::Ynk::Sys::Sync::RwLock __HOOK_LOCK__;

bool Ynk::Panic::panicking ()
{
    return update_panic_count (0_iz) != 0_uz;
}

void Ynk::Panic::set_hook (Ynk::Panic::HookFn hook)
{
    if (Ynk::Panic::panicking ()) {
        panic ("Ynk::Panic: Cannot modify panic hook from panicking thread!");
    }

    __HOOK_LOCK__.write ();
    __HOOK__ = hook;
    __HOOK_LOCK__.write_unlock ();
}

::Ynk::Panic::HookFn Ynk::Panic::get_hook ()
{
    __HOOK_LOCK__.read ();
    HookFn tmp = __HOOK__;
    __HOOK_LOCK__.read_unlock ();

    return tmp;
}

YNK_NORETURN
void Ynk::Panic::begin_panic (const char * file, usize line, String message)
{
    Panicking::PanicInfo info { line, String (file), message };

    __HOOK_LOCK__.read ();
    // A temporary object is used because we cannot risk calling __HOOK__ directly
    // as it might throw an exception, and without a sentinel (which is too high
    // level & takes up more memory & complexity, and might panic on its own,
    // Nine forfend!), so a noexcept operation is performed instead (pointer assignment).
    // I really don't think this following line can throw an exception.
    // If __HOOK__ were to throw an exception or panic, then __HOOK_LOCK__ would
    // never get read_unlock'ed, and it would never be open for writing, and
    // any thread that called Ynk::Panic::set_hook(...) would block forever
    Ynk::Panic::HookFn hook_tmp = __HOOK__;
    __HOOK_LOCK__.read_unlock ();

    // Now that the read has been unlocked, we're free to do whatever without
    // worrying about the other threads.
    hook_tmp (info);

    panic_impl (info);
}

::Ynk::usize update_panic_count (::Ynk::isize amt)
{
    thread_local static ::Ynk::usize panic_count = 0_uz;
    panic_count += amt;
    return panic_count;
}

YNK_NORETURN
void panic_impl (Ynk::Panicking::PanicInfo & info)
{
    throw ::Ynk::Panicking::_Uncatchable::_PanicUnwindingException { info };
}
