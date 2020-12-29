/*
 * Copyright (c) 2011-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Debug aid
 *
 *
 * The __ASSERT() macro can be used inside kernel code.
 *
 * Assertions are enabled by setting the __ASSERT_ON symbol to a non-zero value.
 * There are two ways to do this:
 *   a) Use the ASSERT and ASSERT_LEVEL kconfig options
 *   b) Add "CFLAGS += -D__ASSERT_ON=<level>" at the end of a project's Makefile
 * The Makefile method takes precedence over the kconfig option if both are
 * used.
 *
 * Specifying an assertion level of 1 causes the compiler to issue warnings that
 * the kernel contains debug-type __ASSERT() statements; this reminder is issued
 * since assertion code is not normally present in a final product. Specifying
 * assertion level 2 suppresses these warnings.
 *
 * The __ASSERT_EVAL() macro can also be used inside kernel code.
 *
 * It makes use of the __ASSERT() macro, but has some extra flexibility.  It
 * allows the developer to specify different actions depending whether the
 * __ASSERT() macro is enabled or not.  This can be particularly useful to
 * prevent the compiler from generating comments (errors, warnings or remarks)
 * about variables that are only used with __ASSERT() being assigned a value,
 * but otherwise unused when the __ASSERT() macro is disabled.
 *
 * Consider the following example:
 *
 * int  x;
 *
 * x = foo ();
 * __ASSERT (x != 0, "foo() returned zero!");
 *
 * If __ASSERT() is disabled, then 'x' is assigned a value, but never used.
 * This type of situation can be resolved using the __ASSERT_EVAL() macro.
 *
 * __ASSERT_EVAL ((void) foo(),
 *		  int x = foo(),
 *                x != 0,
 *                "foo() returned zero!");
 *
 * The first parameter tells __ASSERT_EVAL() what to do if __ASSERT() is
 * disabled.  The second parameter tells __ASSERT_EVAL() what to do if
 * __ASSERT() is enabled.  The third and fourth parameters are the parameters
 * it passes to __ASSERT().
 *
 * The __ASSERT_NO_MSG() macro can be used to perform an assertion that reports
 * the failed test and its location, but lacks additional debugging information
 * provided to assist the user in diagnosing the problem; its use is
 * discouraged.
 */

#ifndef ___ASSERT__H_
#define ___ASSERT__H_
#include <stdio.h>

#ifdef CONFIG_ASSERT
#ifndef __ASSERT_ON
#define __ASSERT_ON CONFIG_ASSERT_LEVEL
#endif
#endif

#ifdef CONFIG_FORCE_NO_ASSERT
#undef __ASSERT_ON
#define __ASSERT_ON 0
#endif

#if defined(CONFIG_ASSERT_VERBOSE)
#define __ASSERT_PRINT(fmt, ...) printk(fmt, ##__VA_ARGS__)
#else /* CONFIG_ASSERT_VERBOSE */
#define __ASSERT_PRINT(fmt, ...)
#endif /* CONFIG_ASSERT_VERBOSE */

#ifdef CONFIG_ASSERT_NO_MSG_INFO
#define __ASSERT_MSG_INFO(fmt, ...)
#else  /* CONFIG_ASSERT_NO_MSG_INFO */
#define __ASSERT_MSG_INFO(fmt, ...) __ASSERT_PRINT("\t" fmt "\n", ##__VA_ARGS__)
#endif /* CONFIG_ASSERT_NO_MSG_INFO */

#if !defined(CONFIG_ASSERT_NO_COND_INFO) && !defined(CONFIG_ASSERT_NO_FILE_INFO)
#define __ASSERT_LOC(test)                              \
	__ASSERT_PRINT("ASSERTION FAIL [%s] @ %s:%d\n", \
		Z_STRINGIFY(test),                      \
		__FILE__, __LINE__)
#endif

#if defined(CONFIG_ASSERT_NO_COND_INFO) && !defined(CONFIG_ASSERT_NO_FILE_INFO)
#define __ASSERT_LOC(test)                         \
	__ASSERT_PRINT("ASSERTION FAIL @ %s:%d\n", \
		__FILE__, __LINE__)
#endif

#if !defined(CONFIG_ASSERT_NO_COND_INFO) && defined(CONFIG_ASSERT_NO_FILE_INFO)
#define __ASSERT_LOC(test)                      \
	__ASSERT_PRINT("ASSERTION FAIL [%s]\n", \
		Z_STRINGIFY(test))
#endif

#if defined(CONFIG_ASSERT_NO_COND_INFO) && defined(CONFIG_ASSERT_NO_FILE_INFO)
#define __ASSERT_LOC(test)                 \
	__ASSERT_PRINT("ASSERTION FAIL\n")
#endif

#ifdef __ASSERT_ON
#if (__ASSERT_ON < 0) || (__ASSERT_ON > 2)
#error "Invalid __ASSERT() level: must be between 0 and 2"
#endif

#if __ASSERT_ON
#include <misc/printk.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_ASSERT_NO_FILE_INFO
void assert_post_action(void);
#define __ASSERT_POST_ACTION() assert_post_action()
#else  /* CONFIG_ASSERT_NO_FILE_INFO */
void assert_post_action(const char *file, unsigned int line);
#define __ASSERT_POST_ACTION() assert_post_action(__FILE__, __LINE__)
#endif /* CONFIG_ASSERT_NO_FILE_INFO */

#ifdef __cplusplus
}
#endif

#define __ASSERT_NO_MSG(test)                                             \
	do {                                                              \
		if (!(test)) {                                            \
			__ASSERT_LOC(test);                               \
			__ASSERT_POST_ACTION();                           \
		}                                                         \
	} while (false)

#define __ASSERT(test, fmt, ...)                                          \
	do {                                                              \
		if (!(test)) {                                            \
			__ASSERT_LOC(test);                               \
			__ASSERT_MSG_INFO(fmt, ##__VA_ARGS__);            \
			__ASSERT_POST_ACTION();                           \
		}                                                         \
	} while (false)

#define __ASSERT_EVAL(expr1, expr2, test, fmt, ...)                \
	do {                                                       \
		expr2;                                             \
		__ASSERT(test, fmt, ##__VA_ARGS__);                \
	} while (0)

#if (__ASSERT_ON == 1)
#warning "__ASSERT() statements are ENABLED"
#endif
#else
#define __ASSERT(test, fmt, ...) { }
#define __ASSERT_EVAL(expr1, expr2, test, fmt, ...) expr1
#define __ASSERT_NO_MSG(test) { }
#endif
#else
#define __ASSERT(test, fmt, ...) { }
#define __ASSERT_EVAL(expr1, expr2, test, fmt, ...) expr1
#define __ASSERT_NO_MSG(test) { }
#endif

#endif /* ZEPHYR_INCLUDE_SYS___ASSERT_H_ */
