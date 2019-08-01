//
// file ynk/compiler.h
// author Maximilien M. Cura
//

#ifndef __YNK_COMPILER
#define __YNK_COMPILER

#define COMPILER(YNK_FEATURE) (defined YNK_COMPILER_##YNK_FEATURE && YNK_COMPILER_##YNK_FEATURE)
#define COMPILER_SUPPORTS(YNK_FEATURE) (defined YNK_COMPILER_SUPPORTS_##YNK_FEATURE && YNK_COMPILER_SUPPORTS_##YNK_FEATURE)
#define COMPILER_QUIRK(YNK_FEATURE) (defined YNK_COMPILER_QUIRK_##YNK_FEATURE && YNK_COMPILER_QUIRK_##YNK_FEATURE)

#ifdef __has_builtin
#    define COMPILER_HAS_CLANG_BUILTIN(x) __has_builtin (x)
#else /* @__has_builtin || */
#    define COMPILER_HAS_CLANG_BUILTIN(x) 0
#endif /* @__has_feature */

#ifdef __has_feature
#    define COMPILER_HAS_CLANG_FEATURE(x) __has_feature (x)
#else /* @__has_feature || */
#    define COMPILER_HAS_CLANG_FEATURE(x) 0
#endif /* @__has_feature */

#ifdef __has_extension
#    define COMPILER_HAS_CLANG_EXTENSION(x) __has_extension (x)
#else
#    define COMPILER_HAS_CLANG_EXTENSION(x) 0
#endif

#if defined(__clang__)
#    define YNK_COMPILER_CLANG 1
#    define YNK_COMPILER_SUPPORTS_BLOCKS COMPILER_HAS_CLANG_FEATURE (blocks)
#    define YNK_COMPILER_SUPPORTS_STATIC_ASSERT COMPILER_HAS_CLANG_FEATURE (c_static_assert)
#    define YNK_COMPILER_SUPPORTS_BUILTIN_IS_TRIVIALLY_COPYABLE COMPILER_HAS_CLANG_FEATURE (is_trivially_copyable)
#    define YNK_COMPILER_GCC_OR_CLANG 1
#endif /* @__clang__ */

#if defined(__GNUC__)
#    define YNK_COMPILER_GNU_COMPATIBLE 1
#endif /* @__GNUC__ */

#if !defined(ALWAYS_INLINE) && COMPILER(GCC_OR_CLANG)
#    define ALWAYS_INLINE inline __attribute__ ((__always_inline__))
#endif
#if !defined(ALWAYS_INLINE)
#    define ALWAYS_INLINE inline
#endif

#if !defined(NEVER_INLINE) && COMPILER(GCC_OR_CLANG)
#    define NEVER_INLINE __attribute__ ((__noinline__))
#endif
#if !defined(NEVER_INLINE)
#    define NEVER_INLINE
#endif

#if !defined(REFERENCED_EXTERNALLY) && COMPILER(GCC_OR_CLANG)
#    define REFERENCED_EXTERNALLY __attribute__ ((__used__))
#endif
#if !defined(REFERENCED_EXTERNALLY)
#    define REFERENCED_EXTERNALLY
#endif

#if !defined(UNLIKELY) && COMPILER(GCC_OR_CLANG)
#    define UNLIKELY(x) __builtin_expect (!!(x), 0)
#endif
#if !defined(UNLIKELY)
#    define UNLIKELY(x) (x)
#endif

#if !defined(LIKELY) && COMPILER(GCC_OR_CLANG)
#    define LIKELY(x) __builtin_expect (!!(x), 1)
#endif
#if !defined(LIKELY)
#    define LIKELY(x) (x)
#endif

#endif /* !@__YNK_COMPILER */
