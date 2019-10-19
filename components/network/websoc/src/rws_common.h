/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */


#ifndef __RWS_COMMON_H__
#define __RWS_COMMON_H__ 1

#include <stdio.h>

/* check os */
/* gcc -dM -E - < /dev/null */

/* check windows */
#if !defined(RWS_OS_WINDOWS)
#if defined(WIN32) || defined(_WIN32) || defined(WIN32_LEAN_AND_MEAN) || defined(_WIN64) || defined(WIN64)
#define RWS_OS_WINDOWS 1
#endif
#endif
/* end check windows */

/* check Apple */
#if !defined(RWS_OS_APPLE)
#if defined(__APPLE__) || defined(__MACH__)
#define RWS_OS_APPLE 1
#endif
#endif

/* check Unix */
#if !defined(RWS_OS_UNIX)
#if defined(__unix) || defined(unix) || defined(__unix__)
#define RWS_OS_UNIX 1
#endif
#endif

/* check Linux */
#if !defined(RWS_OS_LINUX)
#if defined(__linux__) || defined(__linux)
#define RWS_OS_LINUX 1
#endif
#endif

/* check Android */
#if !defined(RWS_OS_ANDROID)
#if defined(__ANDROID__) || defined(ANDROID)
#define RWS_OS_ANDROID 1
#endif
#endif

#endif

