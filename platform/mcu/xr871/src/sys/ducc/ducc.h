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

#ifndef _SYS_DUCC_DUCC_H_
#define _SYS_DUCC_DUCC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* simulate h/w msgbox using timers and registers or not */
#define DUCC_SIMULATE_HW_MBOX	0

enum DUCC_ID {
	DUCC_ID_APP2NET_NORMAL = 0,
	DUCC_ID_APP2NET_DATA,
	DUCC_ID_NET2APP_NORMAL,
	DUCC_ID_NET2APP_DATA,

	DUCC_ID_NUM
};

#define DUCC_DIR_NUM	2	/* APP2NET, NET2APP */
#define DUCC_TYPE_NUM	2	/* NORMAL, DATA */

struct ducc_req {
#if DUCC_SIMULATE_HW_MBOX
	uint32_t id;
#endif
	uint32_t cmd;
	uint32_t param;
	int result;	/* @out */
};

#define DUCC_RELEASE_REQ_MAGIC	0xfa050000
#define DUCC_RELEASE_REQ_MASK	0xffff0000
#define DUCC_RELEASE_REQ_VAL(id) \
	((void *)(DUCC_RELEASE_REQ_MAGIC | (id)))
#define DUCC_IS_RELEASE_REQ(r) \
	((((uint32_t)(r)) & DUCC_RELEASE_REQ_MASK) == DUCC_RELEASE_REQ_MAGIC)
#define DUCC_RELEASE_REQ_ID(r) \
	(((uint32_t)(r)) & ~DUCC_RELEASE_REQ_MASK)

int ducc_req_init(uint32_t id);
void ducc_req_deinit(uint32_t id);
int ducc_req_wait(uint32_t id);
void ducc_req_release(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif	/* _SYS_DUCC_DUCC_H_ */
