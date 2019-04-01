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
#ifndef __LWIP_DEBUG_H__
#define __LWIP_DEBUG_H__

#include "lwip/arch.h"

/** lower two bits indicate debug level
 * - 0 all
 * - 1 warning
 * - 2 serious
 * - 3 severe
 */
#define LWIP_DBG_LEVEL_ALL     0x00
#define LWIP_DBG_LEVEL_OFF     LWIP_DBG_LEVEL_ALL /* compatibility define only */
#define LWIP_DBG_LEVEL_WARNING 0x01 /* bad checksums, dropped packets, ... */
#define LWIP_DBG_LEVEL_SERIOUS 0x02 /* memory allocation failures, ... */
#define LWIP_DBG_LEVEL_SEVERE  0x03
#define LWIP_DBG_MASK_LEVEL    0x03

/** flag for LWIP_DEBUGF to enable that debug message */
#define LWIP_DBG_ON            0x80U
/** flag for LWIP_DEBUGF to disable that debug message */
#define LWIP_DBG_OFF           0x00U

/** flag for LWIP_DEBUGF indicating a tracing message (to follow program flow) */
#define LWIP_DBG_TRACE         0x40U
/** flag for LWIP_DEBUGF indicating a state debug message (to follow module states) */
#define LWIP_DBG_STATE         0x20U
/** flag for LWIP_DEBUGF indicating newly added code, not thoroughly tested yet */
#define LWIP_DBG_FRESH         0x10U
/** flag for LWIP_DEBUGF to halt after printing this debug message */
#define LWIP_DBG_HALT          0x08U

#define LWIP_DBG_MOD_MEM                                 (1<<16)
#define LWIP_DBG_MOD_L3_SOCKET                           (1<<17)
#define LWIP_DBG_MOD_L3_API                              (1<<18)
#define LWIP_DBG_MOD_L3_TCP                              (1<<19)
#define LWIP_DBG_MOD_L3_UDP                              (1<<20)
#define LWIP_DBG_MOD_L3_IP                               (1<<21)
#define LWIP_DBG_MOD_L3_OTHER_PROTO                      (1<<22)

#define LWIP_DBG_ALL_MODULES                            (0x007f0000)

#ifndef LWIP_NOASSERT
#define LWIP_ASSERT(message, assertion) do { if(!(assertion)) \
  LWIP_PLATFORM_ASSERT(message); } while(0)
#else  /* LWIP_NOASSERT */
#define LWIP_ASSERT(message, assertion) 
#endif /* LWIP_NOASSERT */

/** if "expression" isn't true, then print "message" and execute "handler" expression */
#ifndef LWIP_ERROR
#define LWIP_ERROR(message, expression, handler) do { if (!(expression)) { \
  LWIP_PLATFORM_ASSERT(message); handler;}} while(0)
#endif /* LWIP_ERROR */

#if LWIP_DEBUG
/** print debug message only if debug message type is enabled...
 *  AND is of correct type AND is at least LWIP_DBG_LEVEL
 */
extern u32 g_log_module;
extern u32 g_log_min_level;


//debug b31~(log module)~b16~ ~b7~(LWIP_DBG_ON/OFF)~b7~b6~(LWIP_DBG_XX)~b3~~b1~(debug level)~b0
#define LWIP_DEBUGF(debug, message) do { \
                               if ( \
                                   ((debug) & LWIP_DBG_ON) && \
                                   ((debug) & LWIP_DBG_TYPES_ON) && \
                                   ((debug) & g_log_module) && \
                                   ((s16_t)((debug) & LWIP_DBG_MASK_LEVEL) >= g_log_min_level)) { \
                                 LWIP_PLATFORM_DIAG(message); \
                                 if ((debug) & LWIP_DBG_HALT) { \
                                   while(1); \
                                 } \
                               } \
                             } while(0)

#else  /* LWIP_DEBUG */
#define LWIP_DEBUGF(debug, message) 
#endif /* LWIP_DEBUG */

#endif /* __LWIP_DEBUG_H__ */

