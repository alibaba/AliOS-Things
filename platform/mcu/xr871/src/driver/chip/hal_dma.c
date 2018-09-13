/**
  * @file  hal_dma.c
  * @author  XRADIO IOT WLAN Team
  */

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

#include "driver/chip/hal_dma.h"
#include "hal_base.h"

#define DMA_IRQ_BITS(chan, type)	((uint32_t)type << ((uint32_t)chan << 1))
#define DMA_IRQ_ALL_BITS			((1 << (DMA_CHANNEL_NUM << 1)) - 1)

typedef struct {
	DMA_IRQCallback 	endCallback;
	void			   *endArg;
#if HAL_DMA_TRANSFER_HALF_IRQ_SUPPORT
	DMA_IRQCallback 	halfCallback;
	void			   *halfArg;
#endif
} DMA_Private;

static DMA_Private gDMAPrivate[DMA_CHANNEL_NUM];

static uint8_t gDMAChannelUsed = 0;

void DMA_IRQHandler(void)
{
	uint32_t i;
	uint32_t irqStatus;
#if HAL_DMA_TRANSFER_HALF_IRQ_SUPPORT
	uint32_t isHalfPending;
#endif
	uint32_t isEndPending;
	DMA_Private *priv;

	irqStatus = DMA->IRQ_STATUS & DMA->IRQ_EN & DMA_IRQ_ALL_BITS; /* get pending bits */
	DMA->IRQ_STATUS = irqStatus; /* clear pending bits */
	priv = gDMAPrivate;

	for (i = DMA_CHANNEL_0; i < DMA_CHANNEL_NUM && irqStatus != 0; ++i) {
#if HAL_DMA_TRANSFER_HALF_IRQ_SUPPORT
		isHalfPending = irqStatus & HAL_BIT(0);
		if (isHalfPending && priv[i].halfCallback) {
			priv[i].halfCallback(priv[i].halfArg);
		}
#endif
		isEndPending = irqStatus & HAL_BIT(1);
		if (isEndPending && priv[i].endCallback) {
			priv[i].endCallback(priv[i].endArg);
		}
		irqStatus >>= 2;
	}
}

void DMA_EnableIRQ(DMA_Channel chan, DMA_IRQType type)
{
	HAL_SET_BIT(DMA->IRQ_EN, DMA_IRQ_BITS(chan, type));
}

void DMA_DisableIRQ(DMA_Channel chan, DMA_IRQType type)
{
	HAL_CLR_BIT(DMA->IRQ_EN, DMA_IRQ_BITS(chan, type));
}

int DMA_IsPendingIRQ(DMA_Channel chan, DMA_IRQType type)
{
	return HAL_GET_BIT(DMA->IRQ_STATUS, DMA_IRQ_BITS(chan, type));
}

void DMA_ClearPendingIRQ(DMA_Channel chan, DMA_IRQType type)
{
	HAL_SET_BIT(DMA->IRQ_STATUS, DMA_IRQ_BITS(chan, type));
}

__STATIC_INLINE void DMA_DisableAllIRQ(DMA_Channel chan)
{
	HAL_CLR_BIT(DMA->IRQ_EN, DMA_IRQ_BITS(chan, DMA_IRQ_TYPE_VMASK));
}

__STATIC_INLINE void DMA_ClearAllPendingIRQ(DMA_Channel chan)
{
	uint32_t irqStatus = HAL_GET_BIT(DMA->IRQ_STATUS,
									 DMA_IRQ_BITS(chan, DMA_IRQ_TYPE_VMASK));
	if (irqStatus) {
		DMA->IRQ_STATUS = irqStatus;
	}
}

static void DMA_Attach(void)
{
	HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_DMA);
	HAL_CCM_BusReleasePeriphReset(CCM_BUS_PERIPH_BIT_DMA);
}

static void DMA_Detach(void)
{
	HAL_CCM_BusForcePeriphReset(CCM_BUS_PERIPH_BIT_DMA);
	HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_DMA);
}

/**
 * @brief Request an available DMA channel
 * @note Before using DMA, an available DMA channel MUST be requested first
 * @retval DMA_Channel, DMA_CHANNEL_INVALID on failed
 */
DMA_Channel HAL_DMA_Request(void)
{
	DMA_Channel i;
	DMA_Channel chan = DMA_CHANNEL_INVALID;
	unsigned long flags;

	flags = HAL_EnterCriticalSection();

	if (gDMAChannelUsed == 0) {
		DMA_Attach();
	}

	for (i = DMA_CHANNEL_0; i < DMA_CHANNEL_NUM; ++i) {
		if ((gDMAChannelUsed & HAL_BIT(i)) == 0) {
			HAL_SET_BIT(gDMAChannelUsed, HAL_BIT(i));
			chan = i;
			break;
		}
	}

	HAL_ExitCriticalSection(flags);

	return chan;
}

/**
 * @brief Release the DMA channel
 * @param[in] chan DMA channel to be released
 * @return None
 */
void HAL_DMA_Release(DMA_Channel chan)
{
	unsigned long flags;

	flags = HAL_EnterCriticalSection();

	HAL_CLR_BIT(gDMAChannelUsed, HAL_BIT(chan));

	if (gDMAChannelUsed == 0) {
		DMA_Detach();
	}

	HAL_ExitCriticalSection(flags);
}

/**
 * @brief Initialize the DMA channel according to the specified parameters
 * @param[in] chan DMA channel
 * @param[in] param Pointer to DMA_ChannelInitParam structure
 * @return None
 */
void HAL_DMA_Init(DMA_Channel chan, const DMA_ChannelInitParam *param)
{
	DMA_Private *priv;
	unsigned long flags;

	HAL_ASSERT_PARAM(chan < DMA_CHANNEL_NUM);

	flags = HAL_EnterCriticalSection();

	priv = &gDMAPrivate[chan];

	/* set configuration */
	DMA->CHANNEL[chan].CTRL = param->cfg; /* NB: it will stop/reset the DMA channel */

	/* set callback */
	if (param->irqType & DMA_IRQ_TYPE_END) {
		priv->endCallback = param->endCallback;
		priv->endArg = param->endArg;
	} else {
		priv->endCallback = NULL;
		priv->endArg = NULL;
	}
#if HAL_DMA_TRANSFER_HALF_IRQ_SUPPORT
	if (param->irqType & DMA_IRQ_TYPE_HALF) {
		priv->halfCallback = param->halfCallback;
		priv->halfArg = param->halfArg;
	} else {
		priv->halfCallback = NULL;
		priv->halfArg = NULL;
	}
#endif

	/* clear pending bits */
#if 0
	if (DMA_IsPendingIRQ(chan, param->irqType)) {
		DMA_ClearPendingIRQ(chan, param->irqType);
	}
#else
	DMA_ClearAllPendingIRQ(chan);
#endif

	/* enable IRQ */
	if ((DMA->IRQ_EN & DMA_IRQ_ALL_BITS) == 0) {
		HAL_NVIC_SetPriority(DMA_IRQn, NVIC_PERIPHERAL_PRIORITY_DEFAULT);
		HAL_NVIC_EnableIRQ(DMA_IRQn);
	}
	DMA_EnableIRQ(chan, param->irqType);

	HAL_ExitCriticalSection(flags);
}

/**
 * @brief DeInitialize the specified DMA channel
 * @param[in] chan DMA channel
 * @return None
 */
void HAL_DMA_DeInit(DMA_Channel chan)
{
	DMA_Private *priv;
	unsigned long flags;

	HAL_ASSERT_PARAM(chan < DMA_CHANNEL_NUM);

	flags = HAL_EnterCriticalSection();

	priv = &gDMAPrivate[chan];

	DMA_DisableAllIRQ(chan);
	if ((DMA->IRQ_EN & DMA_IRQ_ALL_BITS) == 0) {
		HAL_NVIC_DisableIRQ(DMA_IRQn);
	}
	DMA_ClearAllPendingIRQ(chan);

	priv->endCallback = NULL;
	priv->endArg = NULL;
#if HAL_DMA_TRANSFER_HALF_IRQ_SUPPORT
	priv->halfCallback = NULL;
	priv->halfArg = NULL;
#endif

	HAL_ExitCriticalSection(flags);
}

/**
 * @brief Start the DMA transfer of the specified DMA channel
 * @param[in] chan DMA channel
 * @param[in] srcAddr The source address of DMA transfer
 * @param[in] dstAddr The destination address of DMA transfer
 * @param[in] datalen The length of data to be transferred from source to destination
 * @return None
 *
 * @note The source/destination address MUST be aligned to the
 *       source/destination DMA transaction data width defined by DMA_DataWidth.
 * @note The date length MUST not be more than DMA_DATA_MAX_LEN.
 */
void HAL_DMA_Start(DMA_Channel chan, uint32_t srcAddr, uint32_t dstAddr, uint32_t datalen)
{
	HAL_ASSERT_PARAM(chan < DMA_CHANNEL_NUM);
	HAL_ASSERT_PARAM(datalen <= DMA_DATA_MAX_LEN);

	/* TODO: check alignment of @srcAddr and @dstAddr */

	DMA->CHANNEL[chan].SRC_ADDR = srcAddr;
	DMA->CHANNEL[chan].DST_ADDR = dstAddr;
	DMA->CHANNEL[chan].BYTE_CNT = datalen & DMA_BYTE_CNT_VMASK;
	HAL_SET_BIT(DMA->CHANNEL[chan].CTRL, DMA_START_BIT);
}

/**
 * @brief Stop the DMA transfer of the specified DMA channel
 * @param[in] chan DMA channel
 * @return None
 */
void HAL_DMA_Stop(DMA_Channel chan)
{
	HAL_ASSERT_PARAM(chan < DMA_CHANNEL_NUM);
	HAL_CLR_BIT(DMA->CHANNEL[chan].CTRL, DMA_START_BIT); /* NB: it will reset the channel */
}

/**
 * @brief Check the busy status of the specified DMA channel
 * @param[in] chan DMA channel
 * @return 1 on busy, 0 on idle
 */
int HAL_DMA_IsBusy(DMA_Channel chan)
{
	HAL_ASSERT_PARAM(chan < DMA_CHANNEL_NUM);
	return HAL_GET_BIT(DMA->CHANNEL[chan].CTRL, DMA_BUSY_BIT);
}

/**
 * @brief Get the byte counter of the specified DMA channel
 * @param[in] chan DMA channel
 * @return The byte counter of the specified DMA channel
 *     - If DMA byte count mode is DMA_BYTE_CNT_MODE_NORMAL, the return value
 *       is the data length set by HAL_DMA_Start().
 *     - If DMA byte count mode is DMA_BYTE_CNT_MODE_REMAIN, the return value
 *       is the length of the remaining data not transferred.
 */
uint32_t HAL_DMA_GetByteCount(DMA_Channel chan)
{
	HAL_ASSERT_PARAM(chan < DMA_CHANNEL_NUM);
	return (DMA->CHANNEL[chan].BYTE_CNT & DMA_BYTE_CNT_VMASK);
}
