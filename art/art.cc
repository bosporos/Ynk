#include <Ynk/App.h>
#include <Ynk/backtrace/Backtrace.h>
#include <Ynk/fmt/Fmt.h>

#include <cstdio>

//! Main function
//!
//! Normally, C/C++ programs start with a `int main(int argc, char ** argv)` function,
//! but when using Ynk, this is the appropriate way to define the entry point into
//! the program.
//!
//! `argc` and `argv` are actually being passed into this function invisibly by Ynk;
//! they may be used herein, it's just part of the C++ magic I had to use to make
//! this work.
YNK_APP (Art)
{
    // std::printf ("Hello, world!\n");

    // Ynk::usize depth;
    // Ynk::Backtrace::BacktraceElement * trace = Ynk::Backtrace::backtrace (depth);
    // Ynk::Backtrace::print_backtrace (trace, depth);

    // std::printf ("Hello, %s!\n", u8"Móçtu");

    // Ynk::Fmt::print ("%", u64 (2));

    std::printf ("%s\n", (u8"Hello"_y + ", "_y + u8"world!"_y).into_inner_volatile ());

    return 0;
}
