/*----------------------------------------------------------------------------*/
/*
 *  Copyright (C) Bosch Connected Devices and Solutions.
 *  All Rights Reserved. Confidential.
 *
 *  Distribution only to people who need to know this information in
 *  order to do their job.(Need-to-know principle).
 *  Distribution to persons outside the company, only if these persons
 *  signed a non-disclosure agreement.
 *  Electronic transmission, e.g. via electronic mail, must be made in
 *  encrypted form.
 */
/*----------------------------------------------------------------------------*/

//lint -e49 error in standard libraries suppressed

/**

 *	@ingroup BCDS_ESSENTIALS
 *
 *  @defgroup BASICS Basics
 *  @{
 *  @brief Basic macro definitions and standard imports
 *
 *  @details This file provides the standard int and bool header support apart from
 *  compiler abstraction library.
 *  - The library provides macros to abstract from non standard C compiler
 *    switches like GCC's __attribute__ keyword.
 *  - abstraction macros for all attributes including always_inline,
 *    section, unused arguments and variable macros.
 *  @file BCDS_Basics.h
 **/

#ifndef BCDS_BASICS_H_
#define BCDS_BASICS_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#ifndef false
#define false ((bool) 0)
#endif
#ifndef true
#define true ((bool) 1)
#endif

#ifndef TRUE
#define TRUE        ((bool) 1)
#endif
#ifndef FALSE
#define FALSE       ((bool) 0)
#endif

#ifndef NULL
#define NULL        ((void *) 0)
#endif

#ifndef null
#define null        ((void *) 0)
#endif

/**
 * @brief	Macro to inform the compiler that a variable is intentionally
 * not in use.
 *
 * @param [in] variableName : The unused variable.
 */
#define BCDS_UNUSED(variableName) 	((void) variableName)

/**
 * @brief	Macro to inform the compiler that a function or type is deprecated.
 *
 * @param [in] param : function or type to deprecate.
 */
#define BCDS_DEPRECATED(param) 	param __attribute__ ((deprecated))


/**
 * @brief	Macro to inform the compiler that a function is intentionally not in use.
 *
 * GCC: in GCC the compiler option -Wunused (-Wall) generates warnings if a
 * function is not in use. Using the BCDS_UNUSED_FUNC() macro results in
 * the following changes in compiler behavior:
 * - attached to a function, the macro means that the function is meant to be
 *   possibly unused. GCC will not produce a warning for this function.
 *
 * @code
 * void BCDS_UNUSED_FUNC(FOO_foo(uint8_t param1, uint8_t param2)){ ; }
 * @endcode
 *
 * @param [in] function : The unused function with its arguments list.
 */
#ifdef __GNUC__
#  define BCDS_UNUSED_FUNC(function) __attribute__((__unused__)) function
#else
#  define BCDS_UNUSED_FUNC(function) function
#endif

/**
 * @brief	Macro to inform the compiler that this section is a part of the provided
 * linker section.
 *	
 * @code
 * void BCDS_SECTION(section_name)
 * @endcode
 *
 * @param [in] sectionName : The section name that has been provided in the Linker file.
 */
#ifdef __GNUC__
#  define BCDS_SECTION(sectionName) __attribute__((section(#sectionName)))
#else
#  define BCDS_SECTION(sectionName)
#endif

/**
 * @brief	Macro to force the compiler to always inline an inline function.
 *
 * GCC: in GCC, the inline keyword is not always regarded by the compiler.
 * Based on the situation the compiler can decide to emit a real function
 * instead of the function body only. Defining a function as always_inline
 * makes it madatory for the compiler to inline it always.
 *
 * @code
 * BCDS_ALWAYS_INLINE void FOO_foo(void){ ; }
 * @endcode
 *
 * @note Inlining makes C level debugging hard as the inlined code is not aligned
 * with the source file. To eliminate this issue, in debug builds the macro
 * is disabled.
 */
#if defined(__GNUC__) && defined(NDEBUG)
#  define BCDS_ALWAYS_INLINE __attribute__((always_inline))
#else
#  define BCDS_ALWAYS_INLINE
#endif

/**
 * @brief	Macro to inline a function.
 *
 * GCC: in GCC, the inline keyword is not always regarded by the compiler.
 * Based on the situation the compiler can decide to emit a real function
 * instead of the function body only. Inline is not supported by C89 or
 * before C standards.
 * @code
 * BCDS_INLINE void FOO_foo(void){ ; }
 * @endcode
 * @note Inlining makes C level debugging hard as the inlined code is not aligned
 * with the source file.
 */
#if (__STDC_VERSION__ >= 199901L) /* check to C99 mode */
#  define BCDS_INLINE inline
#else
#  define BCDS_INLINE __attribute__((__unused__))
#endif

#endif /* BCDS_BASICS_H_ */
/**@} */
