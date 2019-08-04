//
// file ynk/lang/Dequalify.h
// author Maximilien M. Cura
//

#ifndef __YNK_LANG_DEQUALIFY
#define __YNK_LANG_DEQUALIFY

namespace Ynk {
    template <typename T>
    struct RemoveReference {
        typedef T type;
    };

    template <typename T>
    struct RemoveReference<T &> {
        typedef T type;
    };

    template <typename T>
    struct RemoveReference<T &&> {
        typedef T type;
    };

    template <typename T>
    struct RemoveConstQualification {
        typedef T type;
    };

    template <typename T>
    struct RemoveConstQualification<T const> {
        typedef T type;
    };
}

#endif
