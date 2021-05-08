/*
 *   Copyright (c) 2014 - 2019 Oleh Kulykov <info@resident.name>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
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

