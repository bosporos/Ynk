//
// file ynk/backtrace/Backtrace.hh
// author Maximilien M. Cura
//

#ifndef __YNK_BACKTRACE_BACKTRACE
#define __YNK_BACKTRACE_BACKTRACE

#include <Ynk/Num/Integers.hh>

namespace Ynk::Backtrace {
    struct BacktraceElement
    {
        char * text;
        BacktraceElement * next;
    };

    char * demangle (const char *, usize &);

    void print_backtrace (BacktraceElement *, usize);
    void print_backtrace ();

    BacktraceElement * backtrace (usize &);
}

#endif /* !@__YNK_BACKTRACE_BACKTRACE */
