//
// file ynk/panic/DefaultPanicHook.cc
// author Maximilien M. Cura
//

#include <Ynk/Panic/Panic.h>
#include <Ynk/Backtrace/Backtrace.h>
#include <Ynk/Fmt/Fmt.h>

#include <Ynk/sys/common/Sync/ThreadId.h>

#include <cstdio>

#include <sys/syscall.h>
#include <unistd.h>

void Ynk::Panic::default_hook (Ynk::Panicking::PanicInfo & info)
{
    Ynk::Sys::Sync::ThreadId tid;
    Ynk::println_err ("Thread {} panicked at {}:{}: {}",
                      u64 { tid.inner },
                      info.file,
                      info.line,
                      info.msg);

    // heapalloc-free
    Ynk::Backtrace::print_backtrace ();

    fflush (stderr);
}
