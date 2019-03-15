/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef WEAK
#ifndef __MINGW32__
#define WEAK             __attribute__((weak))
#else
#define WEAK
#endif
#endif

#ifndef NORETURN
#define NORETURN         __attribute__((noreturn))
#endif

#ifndef USED
#define USED             __attribute__((used))
#endif

#ifndef UNUSED
#define UNUSED    __attribute__((unused))
#endif

#ifndef NO_INLINE
#define NO_INLINE     __attribute__((noinline))
#endif

#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE    __attribute__((always_inline))
#endif

#ifndef ALIGNED
#define ALIGNED(size)    __attribute__((aligned(size)))
#endif

#ifndef SECTION
#define SECTION(name)    __attribute__((section(name)))
#endif

void *memrchr( const void *s, int c, size_t n );

#ifdef WIN32
size_t strlcpy (char *dest, const char *src, size_t size);
#endif /* WIN32 */

#ifdef __cplusplus
} /* extern "C" */
#endif

