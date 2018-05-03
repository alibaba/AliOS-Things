/*
 * Copyright (c) 2004, 2005 by
 * Ralf Corsepius, Ulm/Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#ifndef _SYS__INTSUP_H
#define _SYS__INTSUP_H

#include <sys/features.h>

#if __GNUC_PREREQ (3, 2)
/* gcc > 3.2 implicitly defines the values we are interested */
#define __STDINT_EXP(x) __##x##__
#else
#define __STDINT_EXP(x) x
#include <limits.h>
#endif

/* Check if "long long" is 64bit wide */
/* Modern GCCs provide __LONG_LONG_MAX__, SUSv3 wants LLONG_MAX */
#if ( defined(__LONG_LONG_MAX__) && (__LONG_LONG_MAX__ > 0x7fffffff) ) \
  || ( defined(LLONG_MAX) && (LLONG_MAX > 0x7fffffff) )
#define __have_longlong64 1
#endif

/* Check if "long" is 64bit or 32bit wide */
#if __STDINT_EXP(LONG_MAX) > 0x7fffffff
#define __have_long64 1
#elif __STDINT_EXP(LONG_MAX) == 0x7fffffff && !defined(__SPU__)
#define __have_long32 1
#endif

#endif /* _SYS__INTSUP_H */
