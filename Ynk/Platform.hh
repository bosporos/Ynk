//
// file ynk/platform.hh
// author Maximilien M. Cura
//

#ifndef __YNK_PLATFORM
#define __YNK_PLATFORM

#include <Ynk/Compiler.hh>

#define PLATFORM(YNK_FEATURE) (defined YNK_PLATFORM_##YNK_FEATURE && YNK_PLATFORM_##YNK_FEATURE)
#define CPU(YNK_FEATURE) (defined YNK_CPU_##YNK_FEATURE && YNK_CPU_##YNK_FEATURE)
#define OS(YNK_FEATURE) (defined YNK_OS_##YNK_FEATURE && YNK_OS_##YNK_FEATURE)
#define ENABLE(YNK_FEATURE) (defined ENABLE_##YNK_FEATURE && ENABLE_##YNK_FEATURE)

#if defined(__x86_64__) || defined(_M_X64)
#    define YNK_CPU_X86_64 1
#endif

#if COMPILER(GNU_COMPATIBLE)
#    if __SIZEOF_POINTER__ == 8
#        define YNK_CPU_ADDRESS64 1
#    else
#        define YNK_CPU_ADDRESS32 1
#    endif
#else /* +GNU_COMPATIBLE || */
#    include <stdint.h>
#    if UINTPTR_MAX > UINT32_MAX
#        define YNK_CPU_ADDRESS64 1
#    else
#        define YNK_CPU_ADDRESS32 1
#    endif
#endif /* +GNU_COMPATIBLE */

#endif /* !@__YNK_PLATFORM */
