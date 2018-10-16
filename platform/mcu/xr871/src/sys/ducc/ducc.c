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

#include "ducc_os.h"
#include "ducc_debug.h"
#include "ducc.h"


/*
 * ducc semaphore is used to wait respond from another cpu core.
 */

#define DUCC_SEM_NUM	DUCC_TYPE_NUM

static ducc_semaphore_t ducc_req_sem[DUCC_SEM_NUM];
#ifdef __CONFIG_ARCH_APP_CORE
__nonxip_rodata
static ducc_semaphore_t * const g_ducc_req_sem[DUCC_ID_NUM] = {
	[DUCC_ID_APP2NET_NORMAL] = NULL,
	[DUCC_ID_APP2NET_DATA]   = NULL,
	[DUCC_ID_NET2APP_NORMAL] = &ducc_req_sem[0],
	[DUCC_ID_NET2APP_DATA]   = &ducc_req_sem[1],
};
#elif (defined(__CONFIG_ARCH_NET_CORE))
__nonxip_rodata
static ducc_semaphore_t * const g_ducc_req_sem[DUCC_ID_NUM] = {
	[DUCC_ID_APP2NET_NORMAL] = &ducc_req_sem[0],
	[DUCC_ID_APP2NET_DATA]   = &ducc_req_sem[1],
	[DUCC_ID_NET2APP_NORMAL] = NULL,
	[DUCC_ID_NET2APP_DATA]   = NULL,
};
#endif /* __CONFIG_ARCH_APP_CORE */


int ducc_req_init(uint32_t id)
{
	return ducc_semaphore_create(g_ducc_req_sem[id], 0);
}

void ducc_req_deinit(uint32_t id)
{
	ducc_semaphore_delete(g_ducc_req_sem[id]);
}

int ducc_req_wait(uint32_t id)
{
	return ducc_semaphore_wait(g_ducc_req_sem[id]);
}

__nonxip_text
void ducc_req_release(uint32_t id)
{
	ducc_semaphore_release(g_ducc_req_sem[id]);
}
