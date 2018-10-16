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

#ifndef _SYS_DUCC_DUCC_DEBUG_H_
#define _SYS_DUCC_DUCC_DEBUG_H_

#include <stdio.h>
#include "compiler.h"
#include "sys/xr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DUCC_DBG_ON         0
#define DUCC_WRN_ON         1
#define DUCC_ERR_ON         1
#define DUCC_ABORT_ON       0

#define DUCC_DBG_WLAN       1
#define DUCC_DBG_NET        1
#define DUCC_DBG_APP        1
#define DUCC_DBG_MBOX       1
#define DUCC_DBG_HW_MBOX    1

#define DUCC_SYSLOG         printf
#define DUCC_ABORT()        sys_abort()

#define DUCC_LOG(flags, fmt, arg...)    \
    do {                                \
        if (flags)                      \
            DUCC_SYSLOG(fmt, ##arg);    \
    } while (0)

#define DUCC_DBG(fmt, arg...) \
    DUCC_LOG(DUCC_DBG_ON, "[ducc] "fmt, ##arg)

#define DUCC_WLAN_DBG(fmt, arg...) \
    DUCC_LOG(DUCC_DBG_ON && DUCC_DBG_WLAN, "[ducc wlan] "fmt, ##arg)

#define DUCC_NET_DBG(fmt, arg...) \
    DUCC_LOG(DUCC_DBG_ON && DUCC_DBG_NET, "[ducc net] "fmt, ##arg)

#define DUCC_APP_DBG(fmt, arg...) \
    DUCC_LOG(DUCC_DBG_ON && DUCC_DBG_APP, "[ducc app] "fmt, ##arg)

#define DUCC_MBOX_DBG(fmt, arg...) \
    DUCC_LOG(DUCC_DBG_ON && DUCC_DBG_MBOX, "[ducc mbox] "fmt, ##arg)

#define DUCC_HW_MBOX_DBG(fmt, arg...) \
    DUCC_LOG(DUCC_DBG_ON && DUCC_DBG_HW_MBOX, "[ducc hw-mbox] "fmt, ##arg)

#define DUCC_WRN(fmt, arg...) \
    DUCC_LOG(DUCC_WRN_ON, "[ducc WRN] %s():%d, "fmt, __func__, __LINE__, ##arg)

#define DUCC_ERR(fmt, arg...)                               \
    do {                                                    \
        DUCC_LOG(DUCC_ERR_ON, "[ducc ERR] %s():%d, "fmt,    \
                              __func__, __LINE__, ##arg);   \
        if (DUCC_ABORT_ON)                                  \
            DUCC_ABORT();                                   \
    } while (0)

/* debug in interrupt handler */
#ifdef __CONFIG_XIP_SECTION_FUNC_LEVEL

#define DUCC_IT_LOG(flags, fmt, arg...)                 \
    do {                                                \
        if (flags) {                                    \
            __nonxip_data static char __fmt[] = fmt;    \
            DUCC_SYSLOG(__fmt, ##arg);                  \
        }                                               \
    } while (0)

#define DUCC_IT_DBG(fmt, arg...) \
    DUCC_IT_LOG(DUCC_DBG_ON, "[ducc] "fmt, ##arg)

#define DUCC_IT_HW_MBOX_DBG(fmt, arg...) \
    DUCC_IT_LOG(DUCC_DBG_ON && DUCC_DBG_HW_MBOX, "[ducc hw-mbox] "fmt, ##arg)

#define DUCC_IT_WRN(fmt, arg...) \
    DUCC_IT_LOG(DUCC_WRN_ON, "[ducc WRN] %s():%d, "fmt, __s_func, __LINE__, ##arg)

#define DUCC_IT_ERR(fmt, arg...)                            \
    do {                                                    \
        DUCC_IT_LOG(DUCC_ERR_ON, "[ducc ERR] %s():%d, "fmt, \
                              __s_func, __LINE__, ##arg);   \
        if (DUCC_ABORT_ON)                                  \
            DUCC_ABORT();                                   \
    } while (0)

#else /* __CONFIG_XIP_SECTION_FUNC_LEVEL */

#define __s_func                __func__
#define DUCC_IT_DBG             DUCC_DBG
#define DUCC_IT_HW_MBOX_DBG     DUCC_HW_MBOX_DBG
#define DUCC_IT_WRN             DUCC_WRN
#define DUCC_IT_ERR             DUCC_ERR

#endif /* __CONFIG_XIP_SECTION_FUNC_LEVEL */

#ifdef __cplusplus
}
#endif

#endif	/* _SYS_DUCC_DUCC_DEBUG_H_ */
