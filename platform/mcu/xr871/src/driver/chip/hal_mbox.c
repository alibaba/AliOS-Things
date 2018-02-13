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
#include "hal_base.h"

/* useful macros */
#define MBOX_REG_BITS		32

#define MBOX_IRQ_ALL_BITS	((1 << (MBOX_QUEUE_NUM << 1)) - 1)

#define MBOX_GET_IRQ_BIT_POS(queue, dir) \
	((queue << 1) + (dir == MBOX_DIR_TX ? 1 : 0))


#define N_CCM_BUS_PERIPH_BIT_MSGBOX		HAL_BIT(2)
#define N_CCM_BUS_PERIPH_CLK_CTRL_REG	(*((__IO uint32_t *)(0xA0041024U)))
#define N_CCM_BUS_PERIPH_RST_CTRL_REG	(*((__IO uint32_t *)(0xA0041028U)))

void HAL_MBOX_Init(MBOX_T *mbox)
{
	if (mbox == MBOX_A) {
		HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_MSGBOX);
		HAL_CCM_BusReleasePeriphReset(CCM_BUS_PERIPH_BIT_MSGBOX);
	} else {
		HAL_SET_BIT(N_CCM_BUS_PERIPH_CLK_CTRL_REG, N_CCM_BUS_PERIPH_BIT_MSGBOX);
		HAL_SET_BIT(N_CCM_BUS_PERIPH_RST_CTRL_REG, N_CCM_BUS_PERIPH_BIT_MSGBOX);
	}
}

void HAL_MBOX_DeInit(MBOX_T *mbox)
{
	if (mbox == MBOX_A) {
		HAL_CCM_BusForcePeriphReset(CCM_BUS_PERIPH_BIT_MSGBOX);
		HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_MSGBOX);
	} else {
		HAL_CLR_BIT(N_CCM_BUS_PERIPH_RST_CTRL_REG, N_CCM_BUS_PERIPH_BIT_MSGBOX);
		HAL_CLR_BIT(N_CCM_BUS_PERIPH_CLK_CTRL_REG, N_CCM_BUS_PERIPH_BIT_MSGBOX);
	}
}

#if HAL_MBOX_PM_PATCH

#ifdef __CONFIG_ARCH_APP_CORE

#include "driver/chip/hal_dma.h"

void HAL_MBOX_QueueInit(MBOX_T *mbox, MBOX_User user, MBOX_Queue queue, MBOX_Direction dir)
{
	uint32_t regIdx;
	uint32_t rxBit, txBit;

	HAL_ASSERT_PARAM(user < MBOX_USER_NUM);
	HAL_ASSERT_PARAM(queue < MBOX_QUEUE_NUM);
	HAL_ASSERT_PARAM(dir < MBOX_DIR_NUM);

	/* NB: MUST set the users of queue's TX/RX at the same time */

	rxBit = (queue << 3);
	txBit = rxBit + 4;

	regIdx = rxBit / MBOX_REG_BITS;
	rxBit = rxBit % MBOX_REG_BITS;
	txBit = txBit % MBOX_REG_BITS;

	/* TODO: may need spinlock to lock this operation */
	if (user == MBOX_USER0) {
		HAL_MODIFY_REG(mbox->CTRL[regIdx],
		               HAL_BIT(rxBit) | HAL_BIT(txBit),
		               dir == MBOX_DIR_TX ? HAL_BIT(rxBit) : HAL_BIT(txBit));
	} else {
		HAL_MODIFY_REG(mbox->CTRL[regIdx],
		               HAL_BIT(rxBit) | HAL_BIT(txBit),
		               dir == MBOX_DIR_TX ? HAL_BIT(txBit) : HAL_BIT(rxBit));
	}
}

static void MBOX_WriteRegister_DMA(volatile uint32_t *reg, uint32_t val)
{
	DMA_ChannelInitParam dmaParam;
	DMA_Channel dmaChan;
	static volatile uint32_t data;

	dmaChan = HAL_DMA_Request();
	if (dmaChan == DMA_CHANNEL_INVALID) {
		HAL_ERR("no free dma channel\n");
		return;
	}

	data = val;
	HAL_Memset(&dmaParam, 0, sizeof(dmaParam));
	dmaParam.cfg = HAL_DMA_MakeChannelInitCfg(DMA_WORK_MODE_SINGLE,
		                                  DMA_WAIT_CYCLE_1,
		                                  DMA_BYTE_CNT_MODE_REMAIN,
		                                  DMA_DATA_WIDTH_32BIT,
		                                  DMA_BURST_LEN_1,
		                                  DMA_ADDR_MODE_FIXED,
		                                  DMA_PERIPH_SRAM,
		                                  DMA_DATA_WIDTH_32BIT,
		                                  DMA_BURST_LEN_1,
		                                  DMA_ADDR_MODE_FIXED,
		                                  DMA_PERIPH_SRAM);
	dmaParam.irqType = DMA_IRQ_TYPE_END;
	dmaParam.endCallback = NULL;
	dmaParam.endArg = NULL;
	HAL_DMA_Init(dmaChan, &dmaParam);
	HAL_DMA_Start(dmaChan, (uint32_t)&data, (uint32_t)reg, 1);
	while (HAL_DMA_GetByteCount(dmaChan) != 0) { }
	HAL_DMA_Stop(dmaChan);
	HAL_DMA_DeInit(dmaChan);
	HAL_DMA_Release(dmaChan);
}

void HAL_MBOX_QueueEnableIRQ(MBOX_T *mbox, MBOX_User user, MBOX_Queue queue, MBOX_Direction dir)
{
	uint32_t bit;

	HAL_ASSERT_PARAM(user < MBOX_USER_NUM);
	HAL_ASSERT_PARAM(queue < MBOX_QUEUE_NUM);
	HAL_ASSERT_PARAM(dir < MBOX_DIR_NUM);

	bit = MBOX_GET_IRQ_BIT_POS(queue, dir);

	if (user == MBOX_USER0) {
		HAL_SET_BIT(mbox->IRQ0_EN, HAL_BIT(bit));
	} else {
		uint32_t irq_en = mbox->IRQ1_EN;
		HAL_SET_BIT(irq_en, HAL_BIT(bit));
		MBOX_WriteRegister_DMA(&mbox->IRQ1_EN, irq_en);
	}
}

void HAL_MBOX_QueueDisableIRQ(MBOX_T *mbox, MBOX_User user, MBOX_Queue queue, MBOX_Direction dir)
{
	uint32_t bit;

	HAL_ASSERT_PARAM(user < MBOX_USER_NUM);
	HAL_ASSERT_PARAM(queue < MBOX_QUEUE_NUM);
	HAL_ASSERT_PARAM(dir < MBOX_DIR_NUM);

	bit = MBOX_GET_IRQ_BIT_POS(queue, dir);

	if (user == MBOX_USER0) {
		HAL_CLR_BIT(mbox->IRQ0_EN, HAL_BIT(bit));
	} else {
		uint32_t irq_en = mbox->IRQ1_EN;
		HAL_CLR_BIT(irq_en, HAL_BIT(bit));
		MBOX_WriteRegister_DMA(&mbox->IRQ1_EN, irq_en);
	}
}

#endif /* __CONFIG_ARCH_APP_CORE */

void HAL_MBOX_EnableIRQ(MBOX_T *mbox)
{
	IRQn_Type IRQn;

	IRQn = (mbox == MBOX_A) ? MBOX_A_IRQn : MBOX_N_IRQn;
	HAL_NVIC_SetPriority(IRQn, NVIC_PERIPHERAL_PRIORITY_DEFAULT);
	HAL_NVIC_EnableIRQ(IRQn);
}

void HAL_MBOX_DisableIRQ(MBOX_T *mbox)
{
	IRQn_Type IRQn;

	IRQn = (mbox == MBOX_A) ? MBOX_A_IRQn : MBOX_N_IRQn;
	HAL_NVIC_SetPriority(IRQn, NVIC_PERIPHERAL_PRIORITY_DEFAULT);
	HAL_NVIC_DisableIRQ(IRQn);
}

#else /* HAL_MBOX_PM_PATCH */

void HAL_MBOX_QueueInit(MBOX_T *mbox, MBOX_User user, MBOX_Queue queue, MBOX_Direction dir)
{
	uint32_t regIdx;
	uint32_t bit;

	HAL_ASSERT_PARAM(user < MBOX_USER_NUM);
	HAL_ASSERT_PARAM(queue < MBOX_QUEUE_NUM);
	HAL_ASSERT_PARAM(dir < MBOX_DIR_NUM);

	bit = (queue << 3) + (dir == MBOX_DIR_TX ? 4 : 0);
	regIdx = bit / MBOX_REG_BITS;
	bit = bit % MBOX_REG_BITS;

	/* TODO: may need spinlock to lock this operation */
	if (user == MBOX_USER0) {
		HAL_CLR_BIT(mbox->CTRL[regIdx], HAL_BIT(bit));
	} else {
		HAL_SET_BIT(mbox->CTRL[regIdx], HAL_BIT(bit));
	}
}

void HAL_MBOX_QueueEnableIRQ(MBOX_T *mbox, MBOX_User user, MBOX_Queue queue, MBOX_Direction dir)
{
	uint32_t bit;
	IRQn_Type IRQn;
	uint32_t irqEnBits;

	HAL_ASSERT_PARAM(user < MBOX_USER_NUM);
	HAL_ASSERT_PARAM(queue < MBOX_QUEUE_NUM);
	HAL_ASSERT_PARAM(dir < MBOX_DIR_NUM);

	bit = MBOX_GET_IRQ_BIT_POS(queue, dir);

	if (user == MBOX_USER0) {
		irqEnBits = mbox->IRQ0_EN;
		HAL_SET_BIT(mbox->IRQ0_EN, HAL_BIT(bit));
	} else {
		irqEnBits = mbox->IRQ1_EN;
		HAL_SET_BIT(mbox->IRQ1_EN, HAL_BIT(bit));
	}

	if (irqEnBits == 0) {
		IRQn = (mbox == MBOX_A) ? MBOX_A_IRQn : MBOX_N_IRQn;
		HAL_NVIC_SetPriority(IRQn, NVIC_PERIPHERAL_PRIORITY_DEFAULT);
		HAL_NVIC_EnableIRQ(IRQn);
	}
}

void HAL_MBOX_QueueDisableIRQ(MBOX_T *mbox, MBOX_User user, MBOX_Queue queue, MBOX_Direction dir)
{
	uint32_t bit;
	IRQn_Type IRQn;
	uint32_t irqEnBits;

	HAL_ASSERT_PARAM(user < MBOX_USER_NUM);
	HAL_ASSERT_PARAM(queue < MBOX_QUEUE_NUM);
	HAL_ASSERT_PARAM(dir < MBOX_DIR_NUM);

	bit = MBOX_GET_IRQ_BIT_POS(queue, dir);

	if (user == MBOX_USER0) {
		HAL_CLR_BIT(mbox->IRQ0_EN, HAL_BIT(bit));
		irqEnBits = mbox->IRQ0_EN;
	} else {
		HAL_CLR_BIT(mbox->IRQ1_EN, HAL_BIT(bit));
		irqEnBits = mbox->IRQ1_EN;
	}

	if (irqEnBits == 0) {
		IRQn = (mbox == MBOX_A) ? MBOX_A_IRQn : MBOX_N_IRQn;
		HAL_NVIC_SetPriority(IRQn, NVIC_PERIPHERAL_PRIORITY_DEFAULT);
		HAL_NVIC_DisableIRQ(IRQn);
	}
}

#endif /* HAL_MBOX_PM_PATCH */

void HAL_MBOX_QueueDeInit(MBOX_T *mbox, MBOX_User user, MBOX_Queue queue, MBOX_Direction dir)
{
	/* Nothing to do */
}

int HAL_MBOX_QueueIsPendingIRQ(MBOX_T *mbox, MBOX_User user, MBOX_Queue queue, MBOX_Direction dir)
{
	uint32_t bit;

	HAL_ASSERT_PARAM(user < MBOX_USER_NUM);
	HAL_ASSERT_PARAM(queue < MBOX_QUEUE_NUM);
	HAL_ASSERT_PARAM(dir < MBOX_DIR_NUM);

	bit = MBOX_GET_IRQ_BIT_POS(queue, dir);

	if (user == MBOX_USER0) {
		return HAL_GET_BIT(mbox->IRQ0_STATUS, HAL_BIT(bit));
	} else {
		return HAL_GET_BIT(mbox->IRQ1_STATUS, HAL_BIT(bit));
	}
}

void HAL_MBOX_QueueClrPendingIRQ(MBOX_T *mbox, MBOX_User user, MBOX_Queue queue, MBOX_Direction dir)
{
	uint32_t bit;

	HAL_ASSERT_PARAM(user < MBOX_USER_NUM);
	HAL_ASSERT_PARAM(queue < MBOX_QUEUE_NUM);
	HAL_ASSERT_PARAM(dir < MBOX_DIR_NUM);

	bit = MBOX_GET_IRQ_BIT_POS(queue, dir);

	if (user == MBOX_USER0) {
		HAL_SET_BIT(mbox->IRQ0_STATUS, HAL_BIT(bit));
	} else {
		HAL_SET_BIT(mbox->IRQ1_STATUS, HAL_BIT(bit));
	}
}

int HAL_MBOX_QueueIsFull(MBOX_T *mbox, MBOX_Queue queue)
{
	HAL_ASSERT_PARAM(queue < MBOX_QUEUE_NUM);

	return HAL_GET_BIT(mbox->FIFO_STATUS[queue], MBOX_QUEUE_FULL_BIT);
}

uint32_t HAL_MBOX_QueueGetMsgNum(MBOX_T *mbox, MBOX_Queue queue)
{
	HAL_ASSERT_PARAM(queue < MBOX_QUEUE_NUM);

	return HAL_GET_BIT_VAL(mbox->MSG_STATUS[queue],
						   MBOX_QUEUE_MSG_NUM_SHIFT,
						   MBOX_QUEUE_MSG_NUM_VMASK);
}

/* NB: befor put @msg to @queue, make sure @queue is not full */
void HAL_MBOX_QueuePutMsg(MBOX_T *mbox, MBOX_Queue queue, uint32_t msg)
{
	mbox->MSG[queue] = msg;
}

/* NB: befor get @msg from @queue, make sure @queue has message */
uint32_t HAL_MBOX_QueueGetMsg(MBOX_T *mbox, MBOX_Queue queue)
{
	return mbox->MSG[queue];
}

__weak void MBOX_IRQCallback(MBOX_T *mbox, MBOX_Queue queue, MBOX_Direction dir)
{
}

static void MBOX_IRQHandler(MBOX_T *mbox, MBOX_User user)
{
	uint32_t i;
	uint32_t irqPending, irqPendingBackup;
	uint32_t isRxPending, isTxPending;

	if (user == MBOX_USER0) {
		irqPending = mbox->IRQ0_STATUS & mbox->IRQ0_EN & MBOX_IRQ_ALL_BITS; /* get pending bits */
		HAL_MBOX_DBG("mbox %p, user %d, STATUS 0x%x, EN 0x%x, irqPending 0x%x\n",
		             mbox, user, mbox->IRQ0_STATUS, mbox->IRQ0_EN, irqPending);
	} else {
		irqPending = mbox->IRQ1_STATUS & mbox->IRQ1_EN & MBOX_IRQ_ALL_BITS; /* get pending bits */
		HAL_MBOX_DBG("mbox %p, user %d, STATUS 0x%x, EN 0x%x, irqPending 0x%x\n",
		             mbox, user, mbox->IRQ1_STATUS, mbox->IRQ1_EN, irqPending);
	}
	irqPendingBackup = irqPending;

	for (i = MBOX_QUEUE_0; i < MBOX_QUEUE_NUM && irqPending != 0; ++i) {
		isRxPending = (irqPending & HAL_BIT(0));
		if (isRxPending) {
			MBOX_IRQCallback(mbox, i, MBOX_DIR_RX);
		}

		isTxPending = (irqPending & HAL_BIT(1));
		if (isTxPending) {
			MBOX_IRQCallback(mbox, i, MBOX_DIR_TX);
		}

		irqPending >>= 2;
	}

	if (user == MBOX_USER0) {
		mbox->IRQ0_STATUS = irqPendingBackup; /* clear pending bits */
	} else {
		mbox->IRQ1_STATUS = irqPendingBackup; /* clear pending bits */
	}
}

void MBOX_A_IRQHandler(void)
{
	MBOX_IRQHandler(MBOX_A, MBOX_USER0);
}

void MBOX_N_IRQHandler(void)
{
	MBOX_IRQHandler(MBOX_N, MBOX_USER1);
}
