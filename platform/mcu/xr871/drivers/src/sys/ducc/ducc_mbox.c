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

#include "ducc.h"
#include "ducc_os.h"
#include "ducc_debug.h"
#include "ducc_hw_mbox.h"


/*
 * ducc mbox is used to buffer/receive message from another cpu core.
 */

#define DUCC_MBOX_SIZE	4
#define DUCC_MBOX_NUM	DUCC_TYPE_NUM

static ducc_msgqueue_t ducc_mbox[DUCC_MBOX_NUM];

#ifdef __CONFIG_ARCH_APP_CORE
__nonxip_rodata
static ducc_msgqueue_t * const g_ducc_mbox[DUCC_ID_NUM] = {
	[DUCC_ID_APP2NET_NORMAL] = NULL,
	[DUCC_ID_APP2NET_DATA]   = NULL,
	[DUCC_ID_NET2APP_NORMAL] = &ducc_mbox[0],
	[DUCC_ID_NET2APP_DATA]   = &ducc_mbox[1],
};
#elif (defined(__CONFIG_ARCH_NET_CORE))
__nonxip_rodata
static ducc_msgqueue_t * const g_ducc_mbox[DUCC_ID_NUM] = {
	[DUCC_ID_APP2NET_NORMAL] = &ducc_mbox[0],
	[DUCC_ID_APP2NET_DATA]   = &ducc_mbox[1],
	[DUCC_ID_NET2APP_NORMAL] = NULL,
	[DUCC_ID_NET2APP_DATA]   = NULL,
};
#endif /* __CONFIG_ARCH_APP_CORE */

int ducc_mbox_init(uint32_t id, int is_tx)
{
	if (id >= DUCC_ID_NUM) {
		DUCC_ERR("invalid mbox id 0x%x\n", id);
		return -1;
	}

	if (ducc_hw_mbox_init(id, is_tx) != 0) {
		return -1;
	}

	if (is_tx)
		return 0;

	ducc_msgqueue_t *mbox = g_ducc_mbox[id];

	if (ducc_msgqueue_is_valid(mbox)) {
		DUCC_MBOX_DBG("mbox 0x%x already inited\n", id);
		return 0;
	}

	if (ducc_msgqueue_create(mbox, DUCC_MBOX_SIZE) != 0) {
		DUCC_ERR("ducc_msgqueue_create() failed\n");
		return -1;
	}

	return 0;
}

int ducc_mbox_deinit(uint32_t id, int is_tx)
{
	if (id >= DUCC_ID_NUM) {
		DUCC_ERR("invalid mbox id 0x%x\n", id);
		return -1;
	}

	if (ducc_hw_mbox_deinit(id, is_tx) != 0) {
		return -1;
	}

	if (is_tx)
		return 0;

	ducc_msgqueue_t *mbox = g_ducc_mbox[id];

	if (!ducc_msgqueue_is_valid(mbox)) {
		DUCC_MBOX_DBG("mbox 0x%x already deinited\n", id);
		return 0;
	}

	ducc_msgqueue_delete(mbox);

	return 0;
}

int ducc_mbox_send(uint32_t id, void *msg)
{
	if (ducc_hw_mbox_send(id, msg) != 0) {
		DUCC_ERR("ducc_hw_mbox_send() failed, id 0x%x\n", id);
		return -1;
	}
	return 0;
}

void *ducc_mbox_recv(uint32_t id, uint32_t timeout)
{
	void *msg;
	int ret = ducc_msgqueue_recv(g_ducc_mbox[id], &msg, timeout);
	return (ret == 0 ? msg : NULL);
}

__nonxip_text
void ducc_mbox_msg_callback(uint32_t id, void *msg)
{
#if (defined(__CONFIG_XIP_SECTION_FUNC_LEVEL) && DUCC_ERR_ON)
	__nonxip_data static char __s_func[] = "ducc_mbox_msg_callback";
#endif

	if (id >= DUCC_ID_NUM) {
		return; /* for wakeup only */
	}

	if (msg == DUCC_RELEASE_REQ_VAL(id)) {
		ducc_req_release(id);
		return;
	}

	if (ducc_msgqueue_send(g_ducc_mbox[id], msg, 0) != 0) {
		DUCC_IT_ERR("ducc_msgqueue_send() failed, id %u\n", id);
	}
}
