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

#ifndef _NET_WLAN_WLAN_DEBUG_H_
#define _NET_WLAN_WLAN_DEBUG_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys/xr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define wlan_malloc(l)		malloc(l)
#define wlan_free(p)		free(p)
#define wlan_memcpy(d, s, n)	memcpy(d, s, n)
#define wlan_memset(s, c, n) 	memset(s, c, n)
#define wlan_strlen(s)		strlen(s)
#define wlan_strlcpy(d, s, n)	strlcpy(d, s, n)

#define WLAN_DBG_ON	0
#define WLAN_WARN_ON	1
#define WLAN_ERR_ON	1
#define WLAN_ABORT_ON	0

#define WLAN_SYSLOG	printf
#define WLAN_ABORT()	sys_abort()

#define WLAN_LOG(flags, fmt, arg...)			\
	do {						\
		if (flags) 				\
			WLAN_SYSLOG(fmt, ##arg);	\
	} while (0)

#define WLAN_DBG(fmt, arg...)	\
	WLAN_LOG(WLAN_DBG_ON, "[wlan] "fmt, ##arg)

#define WLAN_WARN(fmt, arg...)	\
	WLAN_LOG(WLAN_WARN_ON, "[wlan WARN] "fmt, ##arg)

#define WLAN_ERR(fmt, arg...)						\
	do {								\
		WLAN_LOG(WLAN_ERR_ON, "[wlan ERR] %s():%d, "fmt,	\
	           __func__, __LINE__, ##arg);				\
	    if (WLAN_ABORT_ON)						\
			WLAN_ABORT();					\
	} while (0)

#ifdef __cplusplus
}
#endif

#endif /* _NET_WLAN_WLAN_DEBUG_H_ */
