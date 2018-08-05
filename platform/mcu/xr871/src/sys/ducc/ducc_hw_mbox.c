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

struct ducc_hw_mbox_priv {
	MBOX_T   *mbox_tx;
	MBOX_T   *mbox_rx;
	MBOX_User user_self;
	MBOX_User user_other;
	uint8_t   pm_patch_enabled;
	uint8_t   init_cnt;
};

static struct ducc_hw_mbox_priv g_ducc_hw_mbox_priv;

#define DUCC_HW_MBOX_TX             (g_ducc_hw_mbox_priv.mbox_tx)
#define DUCC_HW_MBOX_RX             (g_ducc_hw_mbox_priv.mbox_rx)
#define DUCC_HW_MBOX_SELF           (g_ducc_hw_mbox_priv.user_self)
#define DUCC_HW_MBOX_OTHER          (g_ducc_hw_mbox_priv.user_other)
#define DUCC_HW_MBOX_PM_PATCH_EN    (g_ducc_hw_mbox_priv.pm_patch_enabled)
#define DUCC_HW_MBOX_INIT_CNT       (g_ducc_hw_mbox_priv.init_cnt)

static void ducc_hw_mbox_init_priv(void)
{
	struct ducc_hw_mbox_priv *priv = &g_ducc_hw_mbox_priv;

	priv->pm_patch_enabled = HAL_MBOX_IsPmPatchEnabled();

	if (priv->pm_patch_enabled) {
		/* Rules
		 *	 - use MBOX_A only
		 *	 - app core init MBOX_A
		 *	 - app core init sender and receiver of all queues
		 *	 - app core enable receiver's queue IRQ
		 *	 - app/net core enable its own rx NVIC IRQ
		 */
		priv->mbox_tx = MBOX_A;
		priv->mbox_rx = MBOX_A;
#ifdef __CONFIG_ARCH_APP_CORE
		priv->user_self = MBOX_USER0;
		priv->user_other = MBOX_USER1;
#elif defined(__CONFIG_ARCH_NET_CORE)
		priv->user_self = MBOX_USER1;
		priv->user_other = MBOX_USER0;
#endif
	} else {
		/* Rules
		 *	 - use both MBOX_A and MBOX_N
		 *	 - tx core init its tx mbox (app core init MBOX_A, net core init MBOX_N)
		 *	 - tx core init sender and receiver of all queues
		 *	 - tx core enable receiver's queue IRQ
		 *	 - rx core enable its own rx NVIC IRQ
		 */
#ifdef __CONFIG_ARCH_APP_CORE
		priv->mbox_tx = MBOX_A;
		priv->mbox_rx = MBOX_N;
#elif defined(__CONFIG_ARCH_NET_CORE)
		priv->mbox_tx = MBOX_N;
		priv->mbox_rx = MBOX_A;
#endif
		priv->user_self = MBOX_USER0;
		priv->user_other = MBOX_USER1;
	}
}

int ducc_hw_mbox_init(uint32_t id, int is_tx)
{
	MBOX_Queue queue;

	if (DUCC_HW_MBOX_TX == NULL) {
		ducc_hw_mbox_init_priv();
	}

	if (DUCC_HW_MBOX_INIT_CNT++ == 0) {
#ifdef __CONFIG_ARCH_NET_CORE
		if (DUCC_HW_MBOX_PM_PATCH_EN) {
			HAL_MBOX_EnableIRQ(DUCC_HW_MBOX_RX);
		} else
#endif
		{
			HAL_MBOX_Init(DUCC_HW_MBOX_TX);
			HAL_MBOX_EnableIRQ(DUCC_HW_MBOX_RX);
		}
	}
#ifdef __CONFIG_ARCH_NET_CORE
	if (DUCC_HW_MBOX_PM_PATCH_EN) {
		return 0;
	}
#endif

	queue = (MBOX_Queue)id;

	if (is_tx) {
		/* init sender and receiver of queue */
		HAL_MBOX_QueueInit(DUCC_HW_MBOX_TX, DUCC_HW_MBOX_SELF, queue, MBOX_DIR_TX);

		/* enable receiver's irq */
		HAL_MBOX_QueueEnableIRQ(DUCC_HW_MBOX_TX, DUCC_HW_MBOX_OTHER, queue, MBOX_DIR_RX);
	}
#ifdef __CONFIG_ARCH_APP_CORE
	else if (DUCC_HW_MBOX_PM_PATCH_EN) {
		/* init sender and receiver of queue */
		HAL_MBOX_QueueInit(DUCC_HW_MBOX_RX, DUCC_HW_MBOX_SELF, queue, MBOX_DIR_RX);

		/* enable receiver's irq */
		HAL_MBOX_QueueEnableIRQ(DUCC_HW_MBOX_RX, DUCC_HW_MBOX_SELF, queue, MBOX_DIR_RX);
	}
#endif

	return 0;
}

int ducc_hw_mbox_deinit(uint32_t id, int is_tx)
{
	MBOX_Queue queue;

#ifdef __CONFIG_ARCH_NET_CORE
	if (DUCC_HW_MBOX_PM_PATCH_EN) {
		goto out;
	}
#endif

	queue = (MBOX_Queue)id;

	if (is_tx) {
		HAL_MBOX_QueueDisableIRQ(DUCC_HW_MBOX_TX, DUCC_HW_MBOX_OTHER, queue, MBOX_DIR_RX);
		HAL_MBOX_QueueDeInit(DUCC_HW_MBOX_TX, DUCC_HW_MBOX_SELF, queue, MBOX_DIR_TX);
	}
#ifdef __CONFIG_ARCH_APP_CORE
	else if (DUCC_HW_MBOX_PM_PATCH_EN) {
		HAL_MBOX_QueueDisableIRQ(DUCC_HW_MBOX_RX, DUCC_HW_MBOX_SELF, queue, MBOX_DIR_RX);
		HAL_MBOX_QueueDeInit(DUCC_HW_MBOX_RX, DUCC_HW_MBOX_SELF, queue, MBOX_DIR_RX);
	}
#endif

#ifdef __CONFIG_ARCH_NET_CORE
out:
#endif
	if ((DUCC_HW_MBOX_INIT_CNT > 0) && (--DUCC_HW_MBOX_INIT_CNT == 0)) {
#ifdef __CONFIG_ARCH_NET_CORE
		if (DUCC_HW_MBOX_PM_PATCH_EN) {
			HAL_MBOX_DisableIRQ(DUCC_HW_MBOX_RX);
		} else
#endif
		{
			HAL_MBOX_DisableIRQ(DUCC_HW_MBOX_RX);
			HAL_MBOX_DeInit(DUCC_HW_MBOX_TX);
		}
	}

	return 0;
}

int ducc_hw_mbox_send(uint32_t id, void *msg)
{
	MBOX_T *mbox = DUCC_HW_MBOX_TX;
	MBOX_Queue queue = (MBOX_Queue)id;
#if DUCC_WRN_ON
	int i = 0;
#endif

	while (HAL_MBOX_QueueIsFull(mbox, queue)) {
#if DUCC_WRN_ON
		if (++i > 1000) {
			DUCC_WRN("h/w mbox %d (%p) is full\n", queue, mbox);
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

	DUCC_WRN("h/w mbox %d (%p) is empty\n", queue, mbox);
	return -1;
}
#endif

__nonxip_text
void MBOX_IRQCallback(MBOX_T *mbox, MBOX_Queue queue, MBOX_Direction dir)
{
	uint32_t id = queue;
	void *msg;

#ifdef __CONFIG_XIP_SECTION_FUNC_LEVEL
#if ((DUCC_DBG_ON && DUCC_DBG_HW_MBOX) || DUCC_WRN_ON)
	__nonxip_data static char __s_func[] = "MBOX_IRQCallback";
#endif
#endif

	DUCC_IT_HW_MBOX_DBG("%s(), mbox %p, queue %d, dir %d\n", __s_func, mbox, queue, dir);

	if (dir == MBOX_DIR_RX) {
		if (mbox != DUCC_HW_MBOX_RX) {
			DUCC_IT_WRN("mbox %p != %p (rx mbox)\n", mbox, DUCC_HW_MBOX_RX);
			return;
		}

		if (HAL_MBOX_QueueGetMsgNum(mbox, queue) > 0) {
			msg = (void *)HAL_MBOX_QueueGetMsg(mbox, queue);
		} else {
			DUCC_IT_WRN("h/w mbox %d (%p) is empty\n", queue, mbox);
			return;
		}

#if 0 // only for test
		DUCC_IT_HW_MBOX_DBG("%s(), queue %d, dir %d, msg %u\n", __s_func, queue, dir, (uint32_t)msg);
		return;
#endif
		ducc_mbox_msg_callback(id, msg);
	} else {
		DUCC_IT_WRN("mbox %p tx irq!\n", mbox);
	}
}

#endif /* __CONFIG_ARCH_DUAL_CORE */
