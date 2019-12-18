/**
 * @file compiler.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup aos_compiler compiler
 * Compiler defined macro to add attributes for funtion, variable or type defined.
 *
 * @{
 */

/* __CC_ARM  */
#if defined(__CC_ARM)

/* Define a weak symbol */
#ifndef WEAK
#ifndef __MINGW32__
#define WEAK __attribute__ ((weak))
#else
#define WEAK
#endif
#endif

/* Tells the compiler that the function won't ever return */
#ifndef NORETURN
#define NORETURN __declspec(noreturn)
#endif

/* Code must be emitted even if the function is not referenced */
#ifndef USED
#define USED __attribute__((used))
#endif

/* Prevents the compiler from generating warnings if the function is not referenced */
#ifndef UNUSED
#define UNUSED __attribute__((unused))
#endif

/* Prevents the compiler from making any use of a constant data value for
optimization purposes, without affecting its placement in the object */
#ifndef NO_INLINE
#define NO_INLINE __attribute__((noinline))
#endif

/* Forces the compiler to compile a C or C++ function inline */
#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE __forceinline
#endif

/* Specifies a minimum alignment for the variable or structure field, measured in bytes */
#ifndef ALIGNED
#define ALIGNED(size) __attribute__((__aligned__(size)))
#endif

/* Specifies that a obj must be placed in a particular section */
#ifndef SECTION
#define SECTION(name) __attribute__((section(name)))
#endif

/* __GNUC__ */
#elif (defined __GNUC__)

/* Define a weak symbol */
#ifndef WEAK
#ifndef __MINGW32__
#define WEAK __attribute__ ((weak))
#else
#define WEAK
#endif
#endif

/* Tells the compiler that the function won't ever return */
#ifndef NORETURN
#define NORETURN __attribute__((__noreturn__))
#endif

/* Code must be emitted even if the function is not referenced */
#ifndef USED
#define USED __attribute__((used))
#endif

/* Prevents the compiler from generating warnings if the function is not referenced */
#ifndef UNUSED
#define UNUSED __attribute__((unused))
#endif

/* Prevents the compiler from making any use of a constant data value for
optimization purposes, without affecting its placement in the object */
#ifndef NO_INLINE
#define NO_INLINE __attribute__((noinline))
#endif

/* Forces the compiler to compile a C or C++ function inline */
#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE __attribute__((__always_inline__))
#endif

/* Specifies a minimum alignment for the variable or structure field, measured in bytes */
#ifndef ALIGNED
#define ALIGNED(size) __attribute__((__aligned__(size)))
#endif

/* Specifies that a obj must be placed in a particular section */
#ifndef SECTION
#define SECTION(name) __attribute__((section(name)))
#endif

/* __ICCARM__ */
#elif (defined __ICCARM__)

/* Define a weak symbol */
#ifndef WEAK
#ifndef __MINGW32__
#define WEAK __weak
#else
#define WEAK
#endif
#endif

/* Tells the compiler that the function won't ever return */
#ifndef NORETURN
#if __ICCARM_V8
#define NORETURN __attribute__((__noreturn__))
#else
#define NORETURN _Pragma("object_attribute=__noreturn")
#endif
#endif

/* Code must be emitted even if the function is not referenced */
#if __ICCARM_V8
#define USED __attribute__((used))
#else
#define USED _Pragma("__root")
#endif

/* Prevents the compiler from generating warnings if the function is not referenced */
#ifndef UNUSED
#define UNUSED __attribute__((unused))
#endif

/* Prevents the compiler from making any use of a constant data value for
optimization purposes, without affecting its placement in the object */
#ifndef NO_INLINE
#define NO_INLINE _Pragma("inline=never")
#endif

/* Forces the compiler to compile a C or C++ function inline */
#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE _Pragma("inline=forced")
#endif

/* Specifies a minimum alignment for the variable or structure field, measured in bytes */
#ifndef ALIGNED
#define ALIGNED(size) _Pragma(data_alignment = size)
#endif

/* Specifies that a obj must be placed in a particular section */
#ifndef SECTION
#define SECTION(name) __attribute__((section(name)))
#endif

#endif

/** @} */

#ifdef __cplusplus
}
#endif

