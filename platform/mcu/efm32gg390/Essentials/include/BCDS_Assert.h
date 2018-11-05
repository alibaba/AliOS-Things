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

/**
 *
 *  @ingroup BCDS_ESSENTIALS
 *
 *  @defgroup  ASSERT Assertion
 *  @{
 *
 *  @brief Enhanced Assertion functionality
 *
 *  @details This module offers several helpful functions for assertion, like:
 *  - static assertion, i.e. at compile time
 *  - dynamic assertion, i.e. during run time, customizable with user defined callback.
 *
 *  @file BCDS_Assert.h
 **/

/* header definition ******************************************************** */
#ifndef BCDS_ASSERT_H_
#define BCDS_ASSERT_H_

#include "BCDS_Retcode.h"

/**
 * Check for usage of the C99 standard or PC-Lint in order to provide
 * support of the static assertion functionality
 *  */
#if ((__STDC_VERSION__ <= 199901L) || defined(_lint))

/* Adding linting exceptions for common static_assert() lint issues. */
//lint -emacro(514, static_assert)  Unusual boolean expression happens when asserting boolean expressions
//lint -emacro(19, static_assert)   Useless declaration of code or declaration which are not used

#ifndef __FILE__
#define __FILE_ 	"Filename not defined"
#endif

#ifndef __LINE__
#define __LINE__ 	0
#endif

/* Helper macro - direct use is forbidden! */
#define ASSERT_CONCAT_(a, b) a##b

/* Helper macro - direct use is forbidden! */
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)

/**
 * @brief 	Static (compilation time) assertion macro.
 *
 * @warning This macro can't be used twice on the same __LINE__ number.
 *          The user must ensure that even in header files the __LINE__
 *          number is always unique. Protect header files against multiple
 *          time inclusions and if possible put static assertions into C
 *          files instead of headers.
 *
 *
 * @details The C99 standard does not support static assertions, nor the sizeof() keyword
 *          in preprocessor macros. To be able to perform compilation time variable size
 *          related checks, this macro is provided as a workaround. Note that the macro
 *          has some limitations.
 *
 * 			Check if the int integer type is 4 bytes long, the following
 *          static assertion can be used:
 *
 * @code
 *          static_assert(sizeof(int) == 4, "The int type shall be 32 bits long.");
 * @endcode
 *
 * @param [in]	expression : A logical statement which can be true or false.
 * @param [in]	message : A string about the test case - note that it is not compiled into the code.
 */
#define static_assert(expression, message) \
	enum { ASSERT_CONCAT(ASSERT_LINE_, __LINE__) = 1/(!!(expression)) };
#else

/* Expand the C11 Keyword _Static_assert if not included already by the compiler */
#ifndef static_assert
#define static_assert _Static_assert
#endif

#endif /* __STDC_VERSION__ || _lint*/


#ifndef NDEBUG /* valid only for debug builds */

#undef assert /* removing the default compiler assert function */

/**
 * @brief 	Program assertion macro.
 * @details	The program assertion macro implements an assertion, which can be used to
 * 			verify assumptions made by the program and get notified of this information
 * 			through a callback if this assumption is false and stop program execution.
 * 			Program assertion is behaving differently in debug and release builds.
 *
 * @note 	Release builds must be built with the NDEBUG symbol defined.
 *
 * @param [in]	expression : A logical statement which can be true or false.
 */
#define assert(expression) \
	do{ if (!(expression)) { Assert_Dynamic((unsigned long)__LINE__,(unsigned char*)__FILE__); } } while (0)

/**
 * @brief   This data type represents a function pointer which would be executed
 * 			when an assertion is triggered.
 *
 * @note    Ideally the first operation that is expected from the API implementing
 * 			this functionality is to disable all the Interrupts of the system
 * 			(based on the Hardware) and if required, it can trace / LOG the line
 * 			number and filename from where the assertion came.
 *
 * @param [in]	line : The line number of the source code from where the assert() is called.
 *
 * @param [in]	file : The file name of the source code from where the assert() is called.
 */
typedef void (*Assert_Callback_T)(const unsigned long line, const unsigned char * const file);

/**
 * @brief 	This function initializes the Assert module. It registers an
 * 			assertion callback function. When an assertion is hit, registered
 * 			callback function is called.
 *
 * 			Ideally, the first thing to be done in the callback function it to
 * 			disable the interrupts of the system (hardware dependent). For trouble shooting,
 * 			it can also trace / log the line number and filename from where the assertion
 * 			came.
 *
 * @note 	Release builds must be built with the NDEBUG symbol defined.
 * 			This API should be called before making use of any one of the
 * 			assertion API's except for the static_assert().
 *
 * @param [in]	callback : A callback function which is executed when an assertion is triggered.
 *
 * @retval	#RETCODE_OK on success
 * @retval	#RETCODE_INVALID_PARAM if callback is not valid
 */
Retcode_T Assert_Initialize(Assert_Callback_T callback);

/**
 * @brief 	Dynamic assert function.
 * 			The dynamic assert function calls a user mapped callback function
 * 			(where atleast all the interrupts disabling is expected) and would
 * 			freeze the system by entering an endless loop.
 *
 * @note 	Release builds must be built with the NDEBUG symbol defined.
 * 			User shall not use this API and rather call the assert() macro with
 * 			required assertion expression. Defining the ASSERT_USE_STD_EXIT macro
 * 			in CFLAGS_DEBUG make variable would use the standard C exit() API.
 *
 * @param [in]	line : The line number of the source code from where the assert() is called.
 *
 * @param [in]	file : The file name of the source code from where the assert() is called.
 */
void Assert_Dynamic(const unsigned long line, const unsigned char * const file);

/**
 * @deprecated since version vxx.xx.xx, replaced by #Assert_Initialize
 * @todo replace vxx.xx.xx with next version
 *
 * @brief   This function initializes the Assert module. It registers an
 *          assertion callback function. When an assertion is hit, registered
 *          callback function is called.
 *
 *          Ideally, the first thing to be done in the callback function it to
 *          disable the interrupts of the system (hardware dependent). For trouble shooting,
 *          it can also trace / log the line number and filename from where the assertion
 *          came.
 *
 * @note    Release builds must be built with the NDEBUG symbol defined.
 *          This API should be called before making use of any one of the
 *          assertion API's except for the static_assert().
 *
 * @param [in]  callback : A callback function which is executed when an assertion is triggered.
 *
 * @retval  #RETCODE_OK on success
 * @retval  #RETCODE_INVALID_PARAM if callback is not valid
 */
BCDS_DEPRECATED(Retcode_T Assert_initialize(Assert_Callback_T callback));

/**
 * @deprecated since version vxx.xx.xx, replaced by #Assert_Dynamic
 * @todo replace vxx.xx.xx with next version
 *
 * @brief   Dynamic assert function.
 *          The dynamic assert function calls a user mapped callback function
 *          (where atleast all the interrupts disabling is expected) and would
 *          freeze the system by entering an endless loop.
 *
 * @note    Release builds must be built with the NDEBUG symbol defined.
 *          User shall not use this API and rather call the assert() macro with
 *          required assertion expression. Defining the ASSERT_USE_STD_EXIT macro
 *          in CFLAGS_DEBUG make variable would use the standard C exit() API.
 *
 * @param [in]  line : The line number of the source code from where the assert() is called.
 *
 * @param [in]  file : The file name of the source code from where the assert() is called.
 */
BCDS_DEPRECATED(void Assert_dynamic(const unsigned long line, const unsigned char * const file));

#else /* valid only for release builds */

/**
 * @note 	For release builds where NDEBUG flag is defined
 * 			dynamic assertion and Assert initialize function
 * 			call are disabled. The Callback function is made void
 * 			to suppress unused function warning.
 */
#undef assert /* removing the default compiler assert function */
#define assert(expression)							((void)0)
/* @deprecated Assert_initialize function will be removed as part of PFM-659 */
#define Assert_initialize(callback)                 ((Retcode_T)0)
#define Assert_Initialize(callback)					((Retcode_T)0)

#endif /* NDEBUG */

#endif /* BCDS_ASSERT_H_ */
/**@} */
/** ************************************************************************* */
