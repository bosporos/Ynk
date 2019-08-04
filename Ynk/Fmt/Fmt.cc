//
// file ynk/fmt/Fmt.cc
// author Maximilien M. Cura
//

#include <Ynk/Fmt/Fmt.h>

void Ynk::Fmt::format_inner (String fmt_str, String & carry)
{
    carry.push (fmt_str);
}

Ynk::String Ynk::Fmt::format (String fmt_str)
{
    return String (fmt_str);
}

void Ynk::print_err (String fmt_str)
{
    std::fprintf (stderr, "%s", fmt_str.into_inner_volatile ());
}

void Ynk::println_err (String fmt_str)
{
    std::fprintf (stderr, "%s\n", fmt_str.into_inner_volatile ());
}

void Ynk::print (String fmt_str)
{
    std::printf ("%s", fmt_str.into_inner_volatile ());
}

void Ynk::println (String fmt_str)
{
    std::printf ("%s\n", fmt_str.into_inner_volatile ());
}
