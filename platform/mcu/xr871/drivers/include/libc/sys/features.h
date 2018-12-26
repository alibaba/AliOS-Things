/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * In order to redefine int32_t to int when compiles using GCC, this file is
 * added to override <sys/features.h> of GCC.
 */

#ifndef _LIBC_SYS_FEATURES_H_
#define _LIBC_SYS_FEATURES_H_

#ifdef __CONFIG_LIBC_REDEFINE_GCC_INT32_TYPE

/*
 * Redefine some macros defined by GCC
 */
#ifdef __INT32_MAX__
#undef __INT32_MAX__
#endif
#define __INT32_MAX__ 2147483647

#ifdef __UINT32_MAX__
#undef __UINT32_MAX__
#endif
#define __UINT32_MAX__ 4294967295U

#ifdef __INT_LEAST32_MAX__
#undef __INT_LEAST32_MAX__
#endif
#define __INT_LEAST32_MAX__ 2147483647

#ifdef __UINT_LEAST32_MAX__
#undef __UINT_LEAST32_MAX__
#endif
#define __UINT_LEAST32_MAX__ 4294967295U

#ifdef __INT_LEAST32_TYPE__
#undef __INT_LEAST32_TYPE__
#endif
#define __INT_LEAST32_TYPE__ int

#ifdef __UINT_LEAST32_TYPE__
#undef __UINT_LEAST32_TYPE__
#endif
#define __UINT_LEAST32_TYPE__ unsigned int

#ifdef __INT32_C
#undef __INT32_C
#endif
#define __INT32_C(c) c

#ifdef __UINT32_C
#undef __UINT32_C
#endif
#define __UINT32_C(c) c ## U

#ifdef __INT32_TYPE__
#undef __INT32_TYPE__
#endif
#define __INT32_TYPE__ int

#ifdef __UINT32_TYPE__
#undef __UINT32_TYPE__
#endif
#define __UINT32_TYPE__ unsigned int

#endif /* __CONFIG_LIBC_REDEFINE_GCC_INT32_TYPE */

/*
 * Include <sys/features.h> of compiler
 */
#include_next <sys/features.h>

#endif /* _LIBC_SYS_FEATURES_H_ */
