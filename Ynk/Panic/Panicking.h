//
// file ynk/panic/panicking.h
// author Maximilien M. Cura
//

#ifndef __YNK_PANIC_PANICKING
#define __YNK_PANIC_PANICKING

#include <Ynk/Utility.h>
#include <Ynk/Num/Integers.h>
#include <Ynk/String/String.h>

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
