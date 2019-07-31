#include <Ynk/App.h>
#include <Ynk/backtrace/Backtrace.h>

#include <cstdio>

void maze ();

YNK_APP ()
{
    std::printf ("Hello, world!\n");

    Ynk::usize depth;
    Ynk::Backtrace::BacktraceElement * trace = Ynk::Backtrace::backtrace (depth);
    Ynk::Backtrace::print_backtrace (trace, depth);

    return 0;
}
