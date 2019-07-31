//
// file ynk/lang/NonCopyable.h
// author Maximilien M. Cura
//

#ifndef __YNK_LANG_NONCOPYABLE
#define __YNK_LANG_NONCOPYABLE

#define YNK_MAKE_NONCOPYABLE(_Name) _Name (_Name const &) = delete

#endif
