//
// file ynk/lang/NonMoveable.h
// author Maximilien M. Cura
//

#ifndef __YNK_LANG_NONMOVEABLE
#define __YNK_LANG_NONMOVEABLE

#define YNK_MAKE_NON_MOVEABLE(_Name) _Name (_Name &&) = delete

#endif
