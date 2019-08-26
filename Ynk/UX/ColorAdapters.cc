//
// file ynk/UX/ColorAdapters.cc
// author Maximilien M. Cura
//

#include <Ynk/UX/ColorAdapters.hh>

Ynk::String Ynk::UX::ANSI (Ynk::UX::RGBA c)
{
    return Ynk::Fmt::format ("\x1b[38;2;{};{};{}m", c.iargb.red, c.iargb.green, c.iargb.blue);
}

Ynk::String Ynk::UX::ANSIRst ()
{
    return Ynk::String ("\x1b[0m");
}
