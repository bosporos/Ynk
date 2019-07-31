//
// file ynk/backtrace/Backtrace.cc
// author Maximilien M. Cura
//

#include <Ynk/backtrace/Backtrace.h>

#include <cxxabi.h>
#include <libunwind.h>
#include <cstdio> /* std::asprintf, std::fprintf, stderr, std::printf */
#include <cstdlib> /* std::malloc, std::free */

char * Ynk::Backtrace::demangle (const char * mangled, usize & length)
{
    int err;
    char * fixed = abi::__cxa_demangle (mangled, nullptr, (size_t *)&length, &err);
    if (!err) {
        return fixed;
    } else {
        return const_cast<char *> (mangled);
    }
}

Ynk::Backtrace::BacktraceElement * Ynk::Backtrace::backtrace (usize & number)
{
    unw_cursor_t cursor;
    unw_context_t context;

    unw_getcontext (&context);
    unw_init_local (&cursor, &context);

    BacktraceElement * first   = nullptr;
    BacktraceElement * current = nullptr;

    number = 0;

    while (unw_step (&cursor) > 0) {
        BacktraceElement * tmp = new BacktraceElement ();
        if (LIKELY (first != nullptr)) {
            current->next = tmp;
            current       = tmp;
        } else {
            current = tmp;
            first   = current;
        }
        number++;

        unw_word_t offset, ip;
        unw_get_reg (&cursor, UNW_REG_IP, &ip);

        if (ip == 0) {
            break;
        }

        char sym[256];
        if (unw_get_proc_name (&cursor, sym, sizeof (sym), &offset) == 0) {
            int err;
            char * nameptr;
            char * demangled = abi::__cxa_demangle (sym, nullptr, nullptr, &err);
            if (!err) {
                nameptr = demangled;
            } else {
                nameptr = (char *)std::malloc (sizeof (char) * (strlen (sym) + 1));
                if (nameptr == NULL) {
                    // panic ("Ynk::Backtrace::Backtrace cannot backtrace: allocation failed");
                    abort ();
                }
                strlcpy (nameptr, sym, 257);
            }
            asprintf (&current->text, "0x%llx: %s+0x%llx", ip, nameptr, offset);
            std::free (demangled);
        } else {
            asprintf (&current->text, "0x%llx: -- error: could not obtain symbol", ip);
        }
    }

    current->next = nullptr;

    return first;
}

void Ynk::Backtrace::print_backtrace ()
{
    unw_cursor_t cursor;
    unw_context_t context;
    unw_word_t offset, instruction_pointer;
    usize index = 0;

    unw_getcontext (&context);
    unw_init_local (&cursor, &context);

    while (unw_step (&cursor) > 0) {
        unw_get_reg (&cursor, UNW_REG_IP, &instruction_pointer);
        if (!instruction_pointer)
            break;

        std::fprintf (stderr, "\tFrame %zi: ", (size_t)index++);
        std::fprintf (stderr, "0x%llx:", instruction_pointer);

        char sym[256];
        if (unw_get_proc_name (&cursor, sym, sizeof (sym), &offset) == 0) {
            int err;
            char * nameptr   = sym;
            char * demangled = abi::__cxa_demangle (sym, nullptr, nullptr, &err);
            if (!err)
                nameptr = demangled;
            std::fprintf (stderr, " %s+0x%llx\n", nameptr, offset);
            if (!err)
                std::free (demangled);
        } else {
            std::fprintf (stderr, " -- error: could not obtain symbol\n");
        }
    }
}

void Ynk::Backtrace::print_backtrace (BacktraceElement * elm, usize depth)
{
    usize index = 0;

    std::printf ("Trace depth: %zi\n", (size_t)depth);
    do {
        std::printf ("\tFrame %zi: %s\n", (size_t)index++, elm->text);
        elm = elm->next;
    } while (elm != nullptr);
}
