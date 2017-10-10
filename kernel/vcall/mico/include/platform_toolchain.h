/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#ifndef WEAK
#ifndef __MINGW32__
#define WEAK             __attribute__((weak))
#else
/* MinGW doesn't support weak */
#define WEAK
#endif
#endif

#ifndef USED
#define USED             __attribute__((used))
#endif

#ifndef MAY_BE_UNUSED
#define MAY_BE_UNUSED    __attribute__((unused))
#endif

#ifndef NORETURN
#define NORETURN         __attribute__((noreturn))
#endif

#ifndef ALIGNED
#define ALIGNED(size)    __attribute__((aligned(size)))
#endif

#ifndef SECTION
#define SECTION(name)    __attribute__((section(name)))
#endif

#ifndef NEVER_INLINE
#define NEVER_INLINE     __attribute__((noinline))
#endif

#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE    __attribute__((always_inline))
#endif

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

void *memrchr( const void *s, int c, size_t n );


/* Windows doesn't come with support for strlcpy */
#ifdef WIN32
size_t strlcpy (char *dest, const char *src, size_t size);
#endif /* WIN32 */

#ifdef __cplusplus
} /* extern "C" */
#endif

