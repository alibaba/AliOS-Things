/*-----------------------------------------------------------------------------
 *      Name:         CV_Typedefs.h
 *      Purpose:      Test framework filetypes and structures description
 *----------------------------------------------------------------------------
 *      Copyright (c) 2017 - 2018 Arm Limited. All rights reserved.
 *----------------------------------------------------------------------------*/
#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

typedef unsigned int    BOOL;

#ifndef __TRUE
 #define __TRUE         1
#endif
#ifndef __FALSE
 #define __FALSE        0
#endif

#ifndef ENABLED
 #define ENABLED        1
#endif
#ifndef DISABLED
 #define DISABLED       0
#endif

#ifndef NULL
 #ifdef __cplusplus              // EC++
  #define NULL          0
 #else
  #define NULL          ((void *) 0)
 #endif
#endif

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

#if defined( __GNUC__ ) || defined ( __clang__ )
static const int PATH_DELIMITER = '/';
#else
static const int PATH_DELIMITER = '\\';
#endif

//lint -emacro(9016,__FILENAME__) allow pointer arithmetic for truncating filename
//lint -emacro(613,__FILENAME__) null pointer is checked
#define __FILENAME__ ((strrchr(__FILE__, PATH_DELIMITER) != NULL) ? (strrchr(__FILE__, PATH_DELIMITER) + 1) : __FILE__)

/* Assertions and test results */
#define SET_RESULT(res, desc) (void)__set_result(__FILENAME__, __LINE__, (res), (desc));

//lint -emacro(9031,ASSERT_TRUE) allow boolean condition as parameter
//lint -emacro(613,ASSERT_TRUE) null pointer is checked
#define ASSERT_TRUE(cond) (void)__assert_true (__FILENAME__, __LINE__, (cond) ? 1U : 0U)

#endif /* __TYPEDEFS_H__ */
