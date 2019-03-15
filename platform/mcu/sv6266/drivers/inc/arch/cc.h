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
#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__

#if 1
/* Include some files for defining library routines */
//#include <ssv_ex_lib.h>
//#include <log.h>
#include "soc_types.h"
#include "lwip_os_porting.h"
#include <stdio.h>
//#include <ssv_lib.h>
#else
/* Declare fuction prototypes for assert/diag/error - leads to some warnings,
 * but good to test if no includes are missing. */
int LOG_PRINTF(const char *format, ...);
void abort(void);
struct _iobuf;
typedef struct _iobuf FILE;
int fflush(FILE *stream);
#endif

#ifdef CONFIG_OS_RHINO
//#define FFLUSH(x) 
//#define hal_print printf
#endif

/** @todo fix some warnings: don't use #pragma if compiling with cygwin gcc */
#ifndef __GNUC__
#include <limits.h>
#pragma warning (disable: 4244) /* disable conversion warning (implicit integer promotion!) */
#pragma warning (disable: 4127) /* conditional expression is constant */
#pragma warning (disable: 4996) /* 'strncpy' was declared deprecated */
#pragma warning (disable: 4103) /* structure packing changed by including file */
#endif

//#define LWIP_PROVIDE_ERRNO
#define ERRNO
#define LWIP_MAILBOX_QUEUE

/* Define platform endianness (might already be defined) */
#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif /* BYTE_ORDER */

/* Define generic types used in lwIP */
#if 0
typedef unsigned   char    u8_t;
typedef signed     char    s8_t;
typedef unsigned   short   u16_t;
typedef signed     short   s16_t;
typedef unsigned   long    u32_t;
typedef signed     long    s32_t;
#endif

typedef size_t mem_ptr_t;
typedef u32_t sys_prot_t;

/* Define (sn)printf formatters for these lwIP types */
#define X8_F  "02x"
#define U16_F "u"
#define S16_F "d"
#define X16_F "x"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"
#define SZT_F U32_F

/* Compiler hints for packing structures */
#ifndef PACK_STRUCT_STRUCT
#define PACK_STRUCT_STRUCT
#endif

#ifndef PACK_STRUCT_USE_INCLUDES
#define PACK_STRUCT_USE_INCLUDES
#endif

/* Plaform specific diagnostic output */
#define LWIP_PLATFORM_DIAG(x)   do { hal_print x; } while(0)

#if ASSERT_ERROR_WITH_MSG
#define LWIP_PLATFORM_ASSERT(x) do { hal_print("Assertion \"%s\" failed at line %d in %s\r\n", \
                                     x, __LINE__, __FILE__); FFLUSH(NULL); halt(); } while(0)

#define LWIP_ERROR(message, expression, handler) do { if (!(expression)) { \
  hal_print("Assertion \"%s\" failed at line %d in %s\r\n", message, __LINE__, __FILE__); \
  FFLUSH(NULL);handler;} } while(0)
#else
#define LWIP_PLATFORM_ASSERT(x) do { hal_print("Assertion failed at line %d in %s\r\n", \
                                     __LINE__, __FILE__); FFLUSH(NULL); halt(); } while(0)

#define LWIP_ERROR(message, expression, handler) do { if (!(expression)) { \
  hal_print("Assertion failed at line %d in %s\r\n", __LINE__, __FILE__); \
  FFLUSH(NULL);handler;} } while(0)
#endif

/* C runtime functions redefined */
//#define snprintf _snprintf

u32_t dns_lookup_external_hosts_file(const char *name);

#define LWIP_RAND() ((u32_t)OS_Random())

#endif /* __ARCH_CC_H__ */
