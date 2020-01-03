/**
 * @file re_types.h  Defines basic types
 *
 * Copyright (C) 2010 Creytiv.com
 */

#ifndef RE_TYPES_H
#define RE_TYPES_H

#include <stdbool.h>
#include <network/network.h>

/*
 * Any C compiler conforming to C99 or later MUST support __func__
 */
#if __STDC_VERSION__ >= 199901L
#define __REFUNC__ (const char *)__func__
#else
#define __REFUNC__ __FUNCTION__
#endif

/** Get number of elements in an array */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) ((sizeof(a))/(sizeof((a)[0])))
#endif

/** Align a value to the boundary of mask */
#define ALIGN_MASK(x, mask)    (((x)+(mask))&~(mask))

/** Get the minimal value */
#ifndef MIN
#define MIN(a,b) (((a)<(b)) ? (a) : (b))
#endif

/** Get the maximal value */
#ifndef MAX
#define MAX(a,b) (((a)>(b)) ? (a) : (b))
#endif

/** Get the minimal value */
#ifndef min
#define min(x,y) MIN(x, y)
#endif

/** Get the maximal value */
#ifndef max
#define max(x,y) MAX(x, y)
#endif
#endif /* RE_TYPES_H */
