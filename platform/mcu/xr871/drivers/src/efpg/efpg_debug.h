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

#ifndef _EFPG_DEBUG_H_
#define _EFPG_DEBUG_H_

#include <stdio.h>
#include "sys/xr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EFPG_DBG_ON		0
#define EFPG_WARN_ON	0
#define EFPG_ERR_ON		1
#define EFPG_ABORT_ON	0

#define EFPG_SYSLOG		printf
#define EFPG_ABORT()	sys_abort()

#define EFPG_LOG(flags, fmt, arg...)	\
	do {								\
		if (flags) 						\
			EFPG_SYSLOG(fmt, ##arg);	\
	} while (0)

#define EFPG_DBG(fmt, arg...)	EFPG_LOG(EFPG_DBG_ON, "[efpg] "fmt, ##arg)
#define EFPG_WARN(fmt, arg...)	EFPG_LOG(EFPG_WARN_ON, "[efpg WARN] "fmt, ##arg)
#define EFPG_ERR(fmt, arg...)								\
	do {													\
		EFPG_LOG(EFPG_ERR_ON, "[efpg ERR] %s():%d, "fmt,	\
	           __func__, __LINE__, ##arg);					\
	    if (EFPG_ABORT_ON)									\
			EFPG_ABORT();									\
	} while (0)

#ifdef __cplusplus
}
#endif

#endif /* _EFPG_DEBUG_H_ */