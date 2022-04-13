/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* __CC_ARM  */
#if defined(__CC_ARM)

#ifndef WEAK
#ifndef __MINGW32__
#define WEAK __attribute__ ((weak))
#else
#define WEAK
#endif
#endif

#ifndef NORETURN
#define NORETURN __declspec(noreturn)
#endif

#ifndef USED
#define USED __attribute__((used))
#endif

#ifndef UNUSED
#define UNUSED __attribute__((unused))
#endif

#ifndef NO_INLINE
#define NO_INLINE __attribute__((noinline))
#endif

#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE __forceinline
#endif

#ifndef ALIGNED
#define ALIGNED(size) __attribute__((__aligned__(size)))
#endif

#ifndef SECTION
#define SECTION(name) __attribute__((section(name)))
#endif

/* __GNUC__ */
#elif (defined __GNUC__)

#ifndef WEAK
#ifndef __MINGW32__
#define WEAK __attribute__ ((weak))
#else
#define WEAK
#endif
#endif

#ifndef NORETURN
#define NORETURN __attribute__((__noreturn__))
#endif

#ifndef USED
#define USED __attribute__((used))
#endif

#ifndef UNUSED
#define UNUSED __attribute__((unused))
#endif

#ifndef NO_INLINE
#define NO_INLINE __attribute__((noinline))
#endif

#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE __attribute__((__always_inline__))
#endif

#ifndef ALIGNED
#define ALIGNED(size) __attribute__((__aligned__(size)))
#endif

#ifndef SECTION
#define SECTION(name) __attribute__((section(name)))
#endif

/* __ICCARM__ */
#elif (defined __ICCARM__)

#ifndef WEAK
#ifndef __MINGW32__
#define WEAK __weak
#else
#define WEAK
#endif
#endif

#ifndef NORETURN
#if __ICCARM_V8
#define NORETURN __attribute__((__noreturn__))
#else
#define NORETURN _Pragma("object_attribute=__noreturn")
#endif
#endif

#if __ICCARM_V8
#define USED __attribute__((used))
#else
#define USED _Pragma("__root")
#endif

#ifndef UNUSED
#define UNUSED __attribute__((unused))
#endif

#ifndef NO_INLINE
#define NO_INLINE _Pragma("inline=never")
#endif

#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE _Pragma("inline=forced")
#endif

#ifndef ALIGNED
#define ALIGNED(size) _Pragma(data_alignment = size)
#endif

#ifndef SECTION
#define SECTION(name) __attribute__((section(name)))
#endif

#endif

#ifdef __cplusplus
}
#endif

