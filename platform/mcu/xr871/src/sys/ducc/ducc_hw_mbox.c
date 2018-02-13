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

#include "driver/chip/hal_mbox.h"
#include "ducc_mbox.h"
#include "ducc_debug.h"

#ifdef __CONFIG_ARCH_DUAL_CORE

#define DUCC_OPT_HW_MBOX_PM_PATCH	HAL_MBOX_PM_PATCH /* use MBOX_A only */

#if DUCC_OPT_HW_MBOX_PM_PATCH

#define DUCC_HW_MBOX            MBOX_A
#define DUCC_HW_MBOX_TX         DUCC_HW_MBOX
#define DUCC_HW_MBOX_RX         DUCC_HW_MBOX

#ifdef __CONFIG_ARCH_APP_CORE
	#define DUCC_HW_MBOX_SELF   MBOX_USER0
	#define DUCC_HW_MBOX_OTHER  MBOX_USER1
#elif (defined(__CONFIG_ARCH_NET_CORE))
	#define DUCC_HW_MBOX_SELF   MBOX_USER1
	#define DUCC_HW_MBOX_OTHER  MBOX_USER0
#endif /* __CONFIG_ARCH_APP_CORE */

uint8_t g_ducc_hw_mbox_init_cnt = 0;

#ifdef __CONFIG_ARCH_APP_CORE

int ducc_hw_mbox_init(uint32_t id, int is_tx)
{
	MBOX_Queue queue = (MBOX_Queue)id;

	if (g_ducc_hw_mbox_init_cnt++ == 0) {
		HAL_MBOX_Init(DUCC_HW_MBOX);
		HAL_MBOX_EnableIRQ(DUCC_HW_MBOX);
	}

	if (is_tx) {
		/* init sender and receiver of queue */
		HAL_MBOX_QueueInit(DUCC_HW_MBOX, DUCC_HW_MBOX_SELF, queue, MBOX_DIR_TX);

		/* enable receiver's irq */
		HAL_MBOX_QueueEnableIRQ(DUCC_HW_MBOX, DUCC_HW_MBOX_OTHER, queue, MBOX_DIR_RX);
	} else {
		/* init sender and receiver of queue */
		HAL_MBOX_QueueInit(DUCC_HW_MBOX, DUCC_HW_MBOX_SELF, queue, MBOX_DIR_RX);

		/* enable receiver's irq */
		HAL_MBOX_QueueEnableIRQ(DUCC_HW_MBOX, DUCC_HW_MBOX_SELF, queue, MBOX_DIR_RX);
	}

	return 0;
}

int ducc_hw_mbox_deinit(uint32_t id, int is_tx)
{
	MBOX_Queue queue = (MBOX_Queue)id;

	if (is_tx) {
		HAL_MBOX_QueueDisableIRQ(DUCC_HW_MBOX, DUCC_HW_MBOX_OTHER, queue, MBOX_DIR_RX);
		HAL_MBOX_QueueDeInit(DUCC_HW_MBOX, DUCC_HW_MBOX_SELF, queue, MBOX_DIR_TX);

	} else {
		HAL_MBOX_QueueDisableIRQ(DUCC_HW_MBOX, DUCC_HW_MBOX_SELF, queue, MBOX_DIR_RX);
		HAL_MBOX_QueueDeInit(DUCC_HW_MBOX, DUCC_HW_MBOX_SELF, queue, MBOX_DIR_RX);
	}

	if ((g_ducc_hw_mbox_init_cnt > 0) && (--g_ducc_hw_mbox_init_cnt == 0)) {
		HAL_MBOX_DisableIRQ(DUCC_HW_MBOX);
		HAL_MBOX_DeInit(DUCC_HW_MBOX);
	}

	return 0;
}

#elif (defined(__CONFIG_ARCH_NET_CORE))

int ducc_hw_mbox_init(uint32_t id, int is_tx)
{
	if (g_ducc_hw_mbox_init_cnt++ == 0) {
		HAL_MBOX_EnableIRQ(DUCC_HW_MBOX);
	}
	return 0;
}

int ducc_hw_mbox_deinit(uint32_t id, int is_tx)
{
	if ((g_ducc_hw_mbox_init_cnt > 0) && (--g_ducc_hw_mbox_init_cnt == 0)) {
		HAL_MBOX_DisableIRQ(DUCC_HW_MBOX);
	}
	return 0;
}

#endif /* __CONFIG_ARCH_APP_CORE */

#else /* DUCC_OPT_HW_MBOX_PM_PATCH */

#ifdef __CONFIG_ARCH_APP_CORE
	#define DUCC_HW_MBOX_TX         MBOX_A
	#define DUCC_HW_MBOX_TX_USER    MBOX_USER0
	#define DUCC_HW_MBOX_RX         MBOX_N
	#define DUCC_HW_MBOX_RX_USER    MBOX_USER1
#elif (defined(__CONFIG_ARCH_NET_CORE))
	#define DUCC_HW_MBOX_TX         MBOX_N
	#define DUCC_HW_MBOX_TX_USER    MBOX_USER0
	#define DUCC_HW_MBOX_RX         MBOX_A
	#define DUCC_HW_MBOX_RX_USER    MBOX_USER1
#endif /* __CONFIG_ARCH_APP_CORE */

uint8_t g_ducc_hw_mbox_enable = 0;

int ducc_hw_mbox_init(uint32_t id, int is_tx)
{
	MBOX_T *mbox;
	MBOX_User user;
	MBOX_Queue queue;
	MBOX_Direction dir;

	if (g_ducc_hw_mbox_enable == 0) {
		HAL_MBOX_Init(DUCC_HW_MBOX_TX);
		HAL_MBOX_Init(DUCC_HW_MBOX_RX);
		g_ducc_hw_mbox_enable = 1;
	}

	queue = (MBOX_Queue)id;
	if (is_tx) {
		mbox = DUCC_HW_MBOX_TX;
		user = DUCC_HW_MBOX_TX_USER;
		dir = MBOX_DIR_TX;
	} else {
		mbox = DUCC_HW_MBOX_RX;
		user = DUCC_HW_MBOX_RX_USER;
		dir = MBOX_DIR_RX;
	}
	HAL_MBOX_QueueInit(mbox, user, queue, dir);
	if (!is_tx) {
		HAL_MBOX_QueueEnableIRQ(mbox, user, queue, dir);
	}

	return 0;
}

int ducc_hw_mbox_deinit(uint32_t id, int is_tx)
{
	MBOX_T *mbox;
	MBOX_User user;
	MBOX_Queue queue;
	MBOX_Direction dir;

	queue = (MBOX_Queue)id;
	if (is_tx) {
		mbox = DUCC_HW_MBOX_TX;
		user = DUCC_HW_MBOX_TX_USER;
		dir = MBOX_DIR_TX;
	} else {
		mbox = DUCC_HW_MBOX_RX;
		user = DUCC_HW_MBOX_RX_USER;
		dir = MBOX_DIR_RX;
	}
	if (!is_tx) {
		HAL_MBOX_QueueDisableIRQ(mbox, user, queue, dir);
	}
	HAL_MBOX_QueueDeInit(mbox, user, queue, dir);

	if (g_ducc_hw_mbox_enable == 1) {
		HAL_MBOX_DeInit(DUCC_HW_MBOX_TX);
		HAL_MBOX_DeInit(DUCC_HW_MBOX_RX);
		g_ducc_hw_mbox_enable = 0;
	}

	return 0;
}

#endif /* DUCC_OPT_HW_MBOX_PM_PATCH */

int ducc_hw_mbox_send(uint32_t id, void *msg)
{
	MBOX_T *mbox = DUCC_HW_MBOX_TX;
	MBOX_Queue queue = (MBOX_Queue)id;
#if DUCC_WARN_ON
	int i = 0;
#endif

	while (HAL_MBOX_QueueIsFull(mbox, queue)) {
#if DUCC_WARN_ON
		if (++i > 1000) {
			DUCC_WARN("h/w mbox %d (%p) is full\n", queue, mbox);
			i = 0;
		}
#endif
	}

	HAL_MBOX_QueuePutMsg(mbox, queue, (uint32_t)msg);
	return 0;
}

#if 0
int ducc_hw_mbox_recv(uint32_t id, void **msg)
{
	MBOX_T *mbox = DUCC_HW_MBOX_RX;
	MBOX_Queue queue = (MBOX_Queue)id;

	if (HAL_MBOX_QueueGetMsgNum(mbox, queue) > 0) {
		*msg = (void *)HAL_MBOX_QueueGetMsg(queue);
		return 0;
	}

	DUCC_WARN("h/w mbox %d (%p) is empty\n", queue, mbox);
	return -1;
}
#endif

void MBOX_IRQCallback(MBOX_T *mbox, MBOX_Queue queue, MBOX_Direction dir)
{
	uint32_t id = queue;
	void *msg;

	DUCC_HW_MBOX_DBG("%s(), mbox %p, queue %d, dir %d\n", __func__, mbox, queue, dir);

	if (dir == MBOX_DIR_RX) {
		if (mbox != DUCC_HW_MBOX_RX) {
			DUCC_WARN("mbox %p != %p (rx mbox)\n", mbox, DUCC_HW_MBOX_RX);
			return;
		}

		if (HAL_MBOX_QueueGetMsgNum(mbox, queue) > 0) {
			msg = (void *)HAL_MBOX_QueueGetMsg(mbox, queue);
		} else {
			DUCC_WARN("h/w mbox %d (%p) is empty\n", queue, mbox);
			return;
		}

#if 0 // only for test
		DUCC_HW_MBOX_DBG("%s(), queue %d, dir %d, msg %u\n", __func__, queue, dir, (uint32_t)msg);
		return;
#endif
		ducc_mbox_msg_callback(id, msg);
	} else {
		DUCC_WARN("mbox %p tx irq!\n", mbox);
	}
}

#endif /* __CONFIG_ARCH_DUAL_CORE */
