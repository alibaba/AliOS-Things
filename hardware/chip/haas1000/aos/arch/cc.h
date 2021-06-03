/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __CC_H__
#define __CC_H__

#include <stdio.h>
#include <stdint.h>
#include "ulog/ulog.h"
//#include "cpu.h"
//#include "typedef.h"

#define LWIP_MAILBOX_QUEUE  1
#define LWIP_TIMEVAL_PRIVATE 0
#define LWIP_NO_INTTYPES_H  0

#if defined(__GNUC__)
 #define PACK_STRUCT_BEGIN
 #define PACK_STRUCT_STRUCT __attribute__((packed))
 #define PACK_STRUCT_FIELD(x) x
#elif defined(__ICCARM__)
 #define PACK_STRUCT_BEGIN __packed
 #define PACK_STRUCT_STRUCT
 #define PACK_STRUCT_FIELD(x) x
#else
 #define PACK_STRUCT_BEGIN
 #define PACK_STRUCT_STRUCT
 #define PACK_STRUCT_FIELD(x) x
#endif


#if LWIP_NO_INTTYPES_H
#define U8_F  "2d"
#define X8_F  "2x"
#define U16_F "4d"
#define S16_F "4d"
#define X16_F "x"
#define U32_F "8ld"
#define S32_F "8ld"
#define X32_F "lx"
#define SZT_F U32_F
#endif

#define LWIP_PLATFORM_TAG "DEBUG"
/*
 *  Platform specific diagnostic output -
 *   LWIP_PLATFORM_DIAG(x)    - non-fatal, print a message.
 *   LWIP_PLATFORM_ASSERT(x)  - fatal, print message and abandon execution.
 *   Portability defines for printf formatters:
 *   U16_F, S16_F, X16_F, U32_F, S32_F, X32_F, SZT_F
 */
#ifndef LWIP_PLATFORM_ASSERT
#define LWIP_PLATFORM_ASSERT(x) \
    do \
    {   LOGE(LWIP_PLATFORM_TAG, "Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); \
    } while(0)
#endif

#ifndef LWIP_ULOG_DIAG
#define LWIP_ULOG_DIAG(x, ...)  LOGI(LWIP_PLATFORM_TAG, x, ##__VA_ARGS__)
#endif

#ifndef LWIP_PLATFORM_DIAG
#define LWIP_PLATFORM_DIAG(x) do {LWIP_ULOG_DIAG x;} while(0)
#endif




/*
 * unknow defination
 */
// cup byte order
#ifndef BYTE_ORDER
#define BYTE_ORDER          LITTLE_ENDIAN
#endif

#ifndef LWIP_RAND
#define LWIP_RAND() ((uint32_t)aos_rand())
#endif

#if 0

#define LWIP_MAILBOX_QUEUE  1
#define LWIP_TIMEVAL_PRIVATE 0
#define LWIP_NO_INTTYPES_H  1

#define LWIP_RAND() ((uint32_t)rand())

#define LWIP_PLATFORM_DIAG(x)	do {printf x;} while(0)



#define U16_F "d"
#define S16_F "d"
#define X16_F "x"
#define U32_F "d"
#define S32_F "d"
#define X32_F "x"
#define SZT_F "uz"

/* define compiler specific symbols */
#if defined (__ICCARM__)
#if !defined (__IARSTDLIB__)
#define _STRING
#ifndef memcmp
#define memcmp(dst, src, sz)			_memcmp(dst, src, sz)
#endif
#ifndef memset
#define memset(dst, val, sz)			_memset(dst, val, sz)
#endif
#ifndef memcpy
#define memcpy(dst, src, sz)			_memcpy(dst, src, sz)
#endif
#endif // __IARSTDLIB__

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_USE_INCLUDES

#elif defined (__CC_ARM)

#define PACK_STRUCT_BEGIN __packed
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

#elif defined (__GNUC__)

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __attribute__ ((__packed__))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_USE_INCLUDES

#elif defined (__TASKING__)

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

#endif

//#define LWIP_PLATFORM_DIAG printf //Realtek add

#define LWIP_PLATFORM_ASSERT(x) //do { if(!(x)) while(1); } while(0)
#endif

#endif /* __CC_H__ */
