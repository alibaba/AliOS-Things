/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
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
#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__


#include <sys/types.h>
#include <errno.h>
#include <sys/time.h>

#include "types.h"
#include "sys/defs.h"

/* Types based on stdint.h */
typedef uint8_t         u8_t;
typedef int8_t          s8_t;
typedef uint16_t        u16_t;
typedef int16_t         s16_t;
typedef uint32_t        u32_t;
typedef int32_t         s32_t;
typedef uintptr_t       mem_ptr_t;

//#define LWIP_COMPAT_MUTEX 1
#define LWIP_MAILBOX_QUEUE  1

#define LWIP_TIMEVAL_PRIVATE 0

#define LWIP_NO_INTTYPES_H  1

/* Define (sn)printf formatters for these lwIP types */
#if LWIP_NO_INTTYPES_H
#define X8_F  "02x"
#define U16_F "hu"
#define S16_F "hd"
#define X16_F "hx"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"
#define SZT_F "uz"
#endif

/* Define platform endianness */
#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif /* BYTE_ORDER */


#if (defined(__GNUC__))
    /* GNU Compiler */
    #define PACK_STRUCT_BEGIN
    #define PACK_STRUCT_STRUCT __attribute__((__packed__))
    #define PACK_STRUCT_END
    #define PACK_STRUCT_FIELD(fld) fld
    #define ALIGNED(n) __attribute__((aligned (n)))
#elif defined(__CC_ARM)
    /* ARM Compiler */
    #define PACK_STRUCT_BEGIN __packed
    #define PACK_STRUCT_STRUCT
    #define PACK_STRUCT_END
    #define PACK_STRUCT_FIELD(fld) fld
    #define ALIGNED(n) __align(n)
#else
    #error "Compiler not supported."
#endif

/* Provide Thumb-2 routines for GCC to improve performance */
#if defined(__GNUC__) && defined(__thumb2__)
    #define MEMCPY(dst,src,len)     thumb2_memcpy(dst,src,len)
    #define SMEMCPY(dst,src,len)    thumb2_memcpy(dst,src,len)
    #define LWIP_CHKSUM             thumb2_checksum
    /* Set algorithm to 0 so that unused lwip_standard_chksum function
       doesn't generate compiler warning */
    #define LWIP_CHKSUM_ALGORITHM   0

    void thumb2_memcpy(void* pDest, const void* pSource, size_t length);
    u16_t thumb2_checksum(void* pData, int length);
#else
    #include <string.h>
    #define MEMCPY(dst,src,len)     memcpy(dst,src,len)
    #define SMEMCPY(dst,src,len)    memcpy(dst,src,len)
    /* Used with IP headers only */
    #define LWIP_CHKSUM_ALGORITHM   1
#endif


#include <stdio.h>

#ifdef NDEBUG
#define LWIP_NOASSERT
#else // Assertions enabled

// If assertions are on, the default LWIP_ERROR handler behaviour is to
// abort w/ an assertion failure. Don't do this, instead just print the error (if LWIP_DEBUG is set)
// and run the handler (same as the LWIP_ERROR behaviour if LWIP_NOASSERT is set).
#ifdef LWIP_DEBUG
#define LWIP_ERROR(message, expression, handler) do { if (!(expression)) { \
  puts(message); handler;}} while(0)
#else
// If LWIP_DEBUG is not set, return the error silently (default LWIP behaviour, also.)
#define LWIP_ERROR(message, expression, handler) do { if (!(expression)) { \
  handler;}} while(0)
#endif // LWIP_DEBUG

#endif /* NDEBUG */

/* Plaform specific diagnostic output */
#define LWIP_PLATFORM_DIAG(x)   do {printf x;} while(0)
#define LWIP_PLATFORM_ASSERT(x)                                 \
    do {                                                        \
        printf("%s at line %d in %s\n", x, __LINE__, __FILE__); \
    } while(0)

#include "kernel/os/os_time.h"
#define LWIP_RAND()                 OS_Rand32()

#include "sys/endian.h"
#define LWIP_PLATFORM_BYTESWAP      1
#define LWIP_PLATFORM_HTONS(x)      htobe16(x)  //__REV16(x)
#define LWIP_PLATFORM_HTONL(x)      htobe32(x)  //__REV(x)

#include "compiler.h"
#define LWIP_INLINE	__inline

#endif /* __ARCH_CC_H__ */
