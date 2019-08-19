//
// file ynk/lang/NonCopyable.hh
// author Maximilien M. Cura
//

#ifndef __YNK_LANG_NONCOPYABLE
#define __YNK_LANG_NONCOPYABLE

#define YNK_MAKE_NON_COPYABLE(_Name) _Name (_Name const &) = delete

#endif
