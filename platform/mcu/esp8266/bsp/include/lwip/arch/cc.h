/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__

#include "c_types.h"

#define EFAULT 14

#define ERRNO
#define LWIP_PROVIDE_ERRNO

#if (1)
#define BYTE_ORDER LITTLE_ENDIAN
#else
#define BYTE_ORDER BIG_ENDIAN
#endif

typedef unsigned long   mem_ptr_t;
typedef int sys_prot_t;

#define S16_F "d"
#define U16_F "d"
#define X16_F "x"

#define S32_F "d"
#define U32_F "d"
#define X32_F "x"

//#define PACK_STRUCT_FIELD(x) x __attribute__((packed))
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

//#define LWIP_DEBUG

#include <stdio.h>

#ifndef os_printf
#define os_printf(fmt, ...) do {    \
    static const char flash_str[] ICACHE_RODATA_ATTR STORE_ATTR = fmt;  \
    printf(flash_str, ##__VA_ARGS__);   \
    } while(0)
#endif

#ifdef LWIP_DEBUG
#define LWIP_PLATFORM_DIAG(x)   do {os_printf x;} while(0)
#define LWIP_PLATFORM_ASSERT(x) do {os_printf(x); sys_arch_assert(__FILE__, __LINE__);} while(0)
#else
#define LWIP_PLATFORM_DIAG(x)
#define LWIP_PLATFORM_ASSERT(x)
#endif

#ifndef LWIP_PLATFORM_BYTESWAP
#define LWIP_PLATFORM_BYTESWAP 1
#endif

#define LWIP_PLATFORM_HTONS(_n)  ((u16_t)((((_n) & 0xff) << 8) | (((_n) >> 8) & 0xff)))
#define LWIP_PLATFORM_HTONL(_n)  ((u32_t)( (((_n) & 0xff) << 24) | (((_n) & 0xff00) << 8) | (((_n) >> 8)  & 0xff00) | (((_n) >> 24) & 0xff) ))

#endif /* __ARCH_CC_H__ */
