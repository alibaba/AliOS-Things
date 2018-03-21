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

#ifndef _IMAGE_DEBUG_H_
#define _IMAGE_DEBUG_H_

#include <stdio.h>
#include "sys/xr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IMAGE_DBG_ON	0
#define IMAGE_WARN_ON	0
#define IMAGE_ERR_ON	1
#define IMAGE_ABORT_ON	0

#define FDCM_DBG_ON		0
#define FDCM_WARN_ON	0
#define FDCM_ERR_ON		1
#define FDCM_ABORT_ON	0

#define FLASH_DBG_ON	0
#define FLASH_WARN_ON	0
#define FLASH_ERR_ON	1
#define FLASH_ABORT_ON	0

#define IMAGE_SYSLOG	printf
#define IMAGE_ABORT()	sys_abort()

#define IMAGE_LOG(flags, fmt, arg...)	\
	do {								\
		if (flags) 						\
			IMAGE_SYSLOG(fmt, ##arg);	\
	} while (0)

#define IMAGE_DBG(fmt, arg...)	IMAGE_LOG(IMAGE_DBG_ON, "[image] "fmt, ##arg)
#define IMAGE_WARN(fmt, arg...)	IMAGE_LOG(IMAGE_WARN_ON, "[image WARN] "fmt, ##arg)
#define IMAGE_ERR(fmt, arg...)								\
	do {													\
		IMAGE_LOG(IMAGE_ERR_ON, "[image ERR] %s():%d, "fmt,	\
				  __func__, __LINE__, ##arg);				\
	    if (IMAGE_ABORT_ON)									\
			IMAGE_ABORT();									\
	} while (0)

#define FDCM_SYSLOG		printf
#define FDCM_ABORT()	sys_abort()

#define FDCM_LOG(flags, fmt, arg...)	\
	do {								\
		if (flags)						\
			FDCM_SYSLOG(fmt, ##arg);	\
	} while (0)

#define FDCM_DBG(fmt, arg...)	FDCM_LOG(FDCM_DBG_ON, "[FDCM] "fmt, ##arg)
#define FDCM_WARN(fmt, arg...)	FDCM_LOG(FDCM_WARN_ON, "[FDCM WARN] "fmt, ##arg)
#define FDCM_ERR(fmt, arg...)								\
	do {													\
		FDCM_LOG(FDCM_ERR_ON, "[FDCM ERR] %s():%d, "fmt, 	\
				 __func__, __LINE__, ##arg);				\
		if (FDCM_ABORT_ON) 									\
			FDCM_ABORT();									\
	} while (0)

#define FLASH_SYSLOG	printf
#define FLASH_ABORT()	sys_abort()

#define FLASH_LOG(flags, fmt, arg...)	\
	do {								\
		if (flags)						\
			FLASH_SYSLOG(fmt, ##arg);	\
	} while (0)

#define FLASH_DBG(fmt, arg...)	FLASH_LOG(FLASH_DBG_ON, "[flash] "fmt, ##arg)
#define FLASH_WARN(fmt, arg...)	FLASH_LOG(FLASH_WARN_ON, "[flash WARN] "fmt, ##arg)
#define FLASH_ERR(fmt, arg...)								\
	do {													\
		FLASH_LOG(FLASH_ERR_ON, "[flash ERR] %s():%d, "fmt, \
				  __func__, __LINE__, ##arg);				\
		if (FLASH_ABORT_ON) 								\
			FLASH_ABORT();									\
	} while (0)

#ifdef __cplusplus
}
#endif

#endif /* _IMAGE_DEBUG_H_ */
