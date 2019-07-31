//
// file ynk/utility.h
// author Maximilien M. Cura
//

#ifndef YNK_UTILITY
#define YNK_UTILITY

#define YNK_PUBLIC __attribute__ ((visibility ("default")))
#define YNK_FACADE __attribute__ ((visibility ("protected")))
#define YNK_VEILED __attribute__ ((visibility ("hidden")))
#define YNK_HIDDEN __attribute__ ((visibility ("internal")))

#define YNK_NONNULL __attribute__ ((nonnull))
#define YNK_FN_NONNULL __attribute__ ((returns_nonnull))

#define YNK_FN_PTR(ret, name, ...) ret (*name) (__VA_ARGS__)
#define YNK_FN_PTR_T(ret, ...) ret (*) (__VA_ARGS__)

#endif /* !@__YNK_UTILITY */
