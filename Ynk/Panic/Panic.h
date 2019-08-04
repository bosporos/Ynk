//
// file ynk/panic/Panic.h
// author Maximilien M. Cura
//

#ifndef __YNK_PANIC_PANIC
#define __YNK_PANIC_PANIC

#include <Ynk/Utility.h>
#include <Ynk/Num/Integers.h>
#include <Ynk/String/String.h>
#include <Ynk/Panic/Panicking.h>
#include <Ynk/Fmt/Fmt.h>

#define panic(...) (::Ynk::Panic::begin_panic (__FILE__, __LINE__, __VA_ARGS__))

namespace Ynk::Panic {

    typedef YNK_FN_PTR (void, HookFn, Panicking::PanicInfo &);

    bool panicking ();

    void set_hook (HookFn);

    HookFn get_hook ();

    void default_hook (Panicking::PanicInfo &);

    template <class... Args>
    YNK_NORETURN void begin_panic (const char * file, usize line, String fmt, Args... args)
    {
        String formatted_msg = Fmt::format (fmt, args...);

        begin_panic (file, line, formatted_msg);
    }

    YNK_NORETURN void begin_panic (const char *, usize, String message);
}

#endif /* !@__YNK_PANIC_PANIC */
