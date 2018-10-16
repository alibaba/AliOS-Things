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

#ifndef _CONSOLE_DEBUG_H_
#define _CONSOLE_DEBUG_H_

#include <stdio.h>
#include "compiler.h"
#include "sys/xr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONS_DBG_ON     0
#define CONS_WRN_ON     1
#define CONS_ERR_ON     1

#define CONS_CHECK_OVERFLOW     1

#define CONS_SYSLOG     printf
#define CONS_ABORT()    do { } while (1) //sys_abort()

#define CONS_LOG(flags, fmt, arg...)    \
    do {                                \
        if (flags)                      \
            CONS_SYSLOG(fmt, ##arg);    \
    } while (0)

#define CONS_DBG(fmt, arg...)   CONS_LOG(CONS_DBG_ON, "[cons] "fmt, ##arg)
#define CONS_WRN(fmt, arg...)   CONS_LOG(CONS_WRN_ON, "[cons WRN] "fmt, ##arg)
#define CONS_ERR(fmt, arg...)                               \
    do {                                                    \
        CONS_LOG(CONS_ERR_ON, "[cons ERR] %s():%d, "fmt,    \
               __func__, __LINE__, ##arg);                  \
        if (CONS_ERR_ON)                                    \
            CONS_ABORT();                                   \
    } while (0)

/* debug in interrupt handler */
#ifdef __CONFIG_XIP_SECTION_FUNC_LEVEL

#define CONS_IT_LOG(flags, fmt, arg...)                 \
    do {                                                \
        if (flags) {                                    \
            __nonxip_data static char __fmt[] = fmt;    \
            CONS_SYSLOG(__fmt, ##arg);                  \
        }                                               \
    } while (0)

#define CONS_IT_DBG(fmt, arg...)    CONS_IT_LOG(CONS_DBG_ON, "[cons] "fmt, ##arg)
#define CONS_IT_WRN(fmt, arg...)    CONS_IT_LOG(CONS_WRN_ON, "[cons WRN] "fmt, ##arg)
#define CONS_IT_ERR(fmt, arg...)                            \
    do {                                                    \
        CONS_IT_LOG(CONS_ERR_ON, "[cons ERR] %s():%d, "fmt, \
               __s_func, __LINE__, ##arg);                  \
        if (CONS_ERR_ON)                                    \
            CONS_ABORT();                                   \
    } while (0)

#else /* __CONFIG_XIP_SECTION_FUNC_LEVEL */

#define __s_func        __func__
#define CONS_IT_DBG     CONS_DBG
#define CONS_IT_WRN     CONS_WRN
#define CONS_IT_ERR     CONS_ERR

#endif /* __CONFIG_XIP_SECTION_FUNC_LEVEL */

#ifdef __cplusplus
}
#endif

#endif /* _CONSOLE_DEBUG_H_ */
