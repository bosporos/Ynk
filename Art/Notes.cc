//
// file art/Notes.cc
// author Maximilien M. Cura
//

#include <Art/Art.hh>

#include <Ynk/Utility.hh> /* YNK_VEILED */
#include <Ynk/UX/ColorAdapters.hh>

#define YNK_NOTIFICATION_COLOR 0
#define YNK_WARNING_COLOR YNK_NOTIFICATION_COLOR + 1
#define YNK_ERROR_COLOR YNK_WARNING_COLOR + 1

#define YNK_PROG_COLOR YNK_ERROR_COLOR + 1

using namespace Ynk;

static YNK_VEILED UX::RGBA notification_colors[] = {
    UX::RGBA (0x8E, 0x94, 0xF2),
    UX::RGBA (0xBB, 0xDA, 0xFF),
    UX::RGBA (0xDB, 0x54, 0x61),
    UX::RGBA (0xDA, 0xB6, 0xFC)
};

void Art::Notify (String note)
{
    _notification_base (Fmt::format ("{}{}{}\n",
                                     UX::ANSI (notification_colors[YNK_NOTIFICATION_COLOR]),
                                     note,
                                     UX::ANSIRst ()));
}

void Art::Warn (String note)
{
    _notification_base (Fmt::format ("{}{}{}\n",
                                     UX::ANSI (notification_colors[YNK_WARNING_COLOR]),
                                     note,
                                     UX::ANSIRst ()));
}

void Art::Error (String note)
{
    _notification_base (Fmt::format ("{}{}{}\n",
                                     UX::ANSI (notification_colors[YNK_ERROR_COLOR]),
                                     note,
                                     UX::ANSIRst ()));
}

void Art::_notification_base (String text)
{
    print ("[{}Art{}]: {}",
           UX::ANSI (notification_colors[YNK_PROG_COLOR]),
           UX::ANSIRst (),
           text);
}
