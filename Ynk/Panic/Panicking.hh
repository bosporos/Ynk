//
// file ynk/panic/panicking.hh
// author Maximilien M. Cura
//

#ifndef __YNK_PANIC_PANICKING
#define __YNK_PANIC_PANICKING

#include <Ynk/Utility.hh>
#include <Ynk/Num/Integers.hh>
#include <Ynk/String/String.hh>

namespace Ynk::Panicking {
    struct PanicInfo
    {
        usize line;

        String file;

        String msg;
    };

    namespace _Uncatchable {
        struct YNK_VEILED _PanicUnwindingException
        {
            PanicInfo & inner;
        };
    }
}

#endif /* !@__YNK_PANIC_PANICKING */
