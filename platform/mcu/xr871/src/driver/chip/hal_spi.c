/**
  * @file  hal_spi.c
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

#include <stdbool.h>
#include "hal_base.h"
#include "driver/chip/hal_spi.h"
#include "pm/pm.h"

/*************************************** Debug *****************************************/
#define SPI_MODULE (DBG_ON | XR_LEVEL_ALERT)
#define SPI_TRANSFER_DEBUG
//#define SPI_REG_DEBUG

#define SPI_ASSERT(condition) XR_ASSERT(condition, SPI_MODULE, #condition " failed\n")

#define SPI_DEBUG(msg, arg...) XR_DEBUG(SPI_MODULE, NOEXPAND, "[SPI Debug] " msg, ##arg)

#define SPI_ALERT(msg, arg...) XR_ALERT(SPI_MODULE, NOEXPAND, "[SPI Alert] " msg, ##arg)

#define SPI_ENTRY() XR_ENTRY(SPI_MODULE, "[SPI Entry]")
#define SPI_EXIT(val) XR_RET(SPI_MODULE, "[SPI Exit]", val)

#ifdef SPI_REG_DEBUG
#define SPI_REG_ALL(spi, msg) { \
		SPI_DEBUG(msg "\n"); \
		SPI_REG(spi->VER); \
		SPI_REG(spi->CTRL); \
		SPI_REG(spi->TCTRL); \
		SPI_REG(spi->IER); \
		SPI_REG(spi->STA); \
		SPI_REG(spi->FCTL); \
		SPI_REG(spi->FST); \
		SPI_REG(spi->WAIT); \
		SPI_REG(spi->CCTR); \
		SPI_REG(spi->BC); \
		SPI_REG(spi->TC); \
		SPI_REG(spi->BCC); \
		SPI_REG(spi->NDMA_MODE_CTRL); \
	}

#define SPI_REG(reg) SPI_DEBUG("register " #reg ": 0x%x.\n", reg);

#else
#define SPI_REG_ALL(spi, msg)

#define SPI_REG(reg)

#endif

#ifdef SPI_TRANSFER_DEBUG
#define SPI_DEBUG_TRANSFER_DATA(pdata, size, dir) { \
		uint32_t temp_size = size; \
		uint8_t *temp_pdata = pdata; \
		XR_DEBUG(SPI_MODULE, NOEXPAND, "[SPI Debug Transfer] " dir " size = %d: ", size); \
		while (temp_size--) \
			XR_DEBUG(SPI_MODULE, NOEXPAND, "0x%x ", *(temp_pdata++)); \
		XR_DEBUG(SPI_MODULE, NOEXPAND, "\n"); \
	}
#else
#define SPI_DEBUG_TRANSFER_DATA(pdata, size)
#endif


#define __SPI_STATIC_INLINE__ __STATIC_INLINE

/*
 * @brief Hardware Layer Interface
 */
__SPI_STATIC_INLINE__
uint32_t SPI_GetVersion(SPI_T *spi)
{
	return spi->VER;
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_Reset(SPI_T *spi)
{
	HAL_SET_BIT(spi->CTRL, SPI_CTRL_RST_MASK);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_SetMode(SPI_T *spi, SPI_CTRL_Mode mode)
{
	HAL_MODIFY_REG(spi->CTRL, SPI_CTRL_MODE_MASK, mode);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_Enable(SPI_T *spi)
{
	HAL_SET_BIT(spi->CTRL, SPI_CTRL_EN_MASK);
}

__SPI_STATIC_INLINE__
void SPI_Disable(SPI_T *spi)
{
	HAL_CLR_BIT(spi->CTRL, SPI_CTRL_EN_MASK);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_StartTransmit(SPI_T *spi)
{
	HAL_SET_BIT(spi->TCTRL, SPI_TCTRL_XCH_MASK);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_SetFirstTransmitBit(SPI_T *spi, SPI_TCTRL_Fbs bit)
{
	HAL_MODIFY_REG(spi->TCTRL, SPI_TCTRL_FBS_MASK, bit);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_EnableRapidsMode(SPI_T *spi, bool delay_sample)
{
	HAL_SET_BIT(spi->TCTRL, SPI_TCTRL_RPSM_MASK);
	HAL_MODIFY_REG(spi->TCTRL, SPI_TCTRL_SDC_MASK, delay_sample << SPI_TCTRL_SDC_SHIFT);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_DisableRapidsMode(SPI_T *spi)
{
	HAL_CLR_BIT(spi->TCTRL, SPI_TCTRL_RPSM_MASK);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_SetDuplex(SPI_T *spi, SPI_TCTRL_DHB_Duplex duplex)
{
	HAL_MODIFY_REG(spi->TCTRL, SPI_TCTRL_DHB_MASK, duplex);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_SetCsLevel(SPI_T *spi,  bool level)
{
	HAL_MODIFY_REG(spi->TCTRL, SPI_TCTRL_SS_LEVEL_MASK, level << SPI_TCTRL_SS_LEVEL_SHIFT);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_ManualChipSelect(SPI_T *spi, SPI_TCTRL_SS_Sel cs)
{
	HAL_SET_BIT(spi->TCTRL, SPI_TCTRL_SS_OWNER_MASK);
	HAL_MODIFY_REG(spi->TCTRL, SPI_TCTRL_SS_SEL_MASK, cs);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_AutoChipSelect(SPI_T *spi, SPI_TCTRL_SS_Sel cs, bool cs_remain)
{
	HAL_MODIFY_REG(spi->TCTRL, SPI_TCTRL_SS_SEL_MASK, cs);
	HAL_CLR_BIT(spi->TCTRL, SPI_TCTRL_SS_OWNER_MASK);
	HAL_MODIFY_REG(spi->TCTRL, SPI_TCTRL_SS_CTL_MASK, (!cs_remain) << SPI_TCTRL_SS_CTL_SHIFT);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_SetCsIdle(SPI_T *spi, bool idle)
{
	HAL_MODIFY_REG(spi->TCTRL, SPI_TCTRL_SPOL_MASK, (!!idle) << SPI_TCTRL_SPOL_SHIFT);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_SetSclkMode(SPI_T *spi, SPI_SCLK_Mode mode)
{
	HAL_MODIFY_REG(spi->TCTRL, SPI_TCTRL_CPOL_MASK | SPI_TCTRL_CPHA_MASK, mode);
}

typedef enum {
	SPI_INT_CS_DESELECT = SPI_IER_SS_INT_EN_MASK,
	SPI_INT_TRANSFER_COMPLETE = SPI_IER_TC_INT_EN_MASK,
	SPI_INT_TXFIFO_UNDER_RUN = SPI_IER_TF_UDR_INT_EN_MASK,
	SPI_INT_TXFIFO_OVERFLOW = SPI_IER_TF_OVF_INT_EN_MASK,
	SPI_INT_RXFIFO_UNDER_RUN = SPI_IER_RF_UDR_INT_EN_MASK,
	SPI_INT_RXFIFO_OVERFLOW = SPI_IER_RF_OVF_INT_EN_MASK,
	SPI_INT_TXFIFO_FULL = SPI_IER_TF_FUL_INT_EN_MASK,
	SPI_INT_TXFIFO_EMPTY = SPI_IER_TX_EMP_INT_EN_MASK,
	SPI_INT_TXFIFO_READY = SPI_IER_TX_ERQ_INT_EN_MASK,
	SPI_INT_RXFIFO_FULL = SPI_IER_RF_FUL_INT_EN_MASK,
	SPI_INT_RXFIFO_EMPTY = SPI_IER_RX_EMP_INT_EN_MASK,
	SPI_INT_RXFIFO_READY = SPI_IER_RF_RDY_INT_EN_MASK
} SPI_Int_Type;

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_EnableInt(SPI_T *spi, SPI_Int_Type type)
{
	HAL_SET_BIT(spi->IER, type);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_DisableInt(SPI_T *spi, SPI_Int_Type type)
{
	HAL_CLR_BIT(spi->IER, type);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
bool SPI_IntState(SPI_T *spi, SPI_Int_Type type)
{
	return !!HAL_GET_BIT(spi->STA, type);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
bool SPI_ClearInt(SPI_T *spi, SPI_Int_Type type)
{
	HAL_SET_BIT(spi->STA, type);
	return HAL_GET_BIT(spi->STA, type);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_DebugReadTx(SPI_T *spi, uint32_t *data)
{
	// tbc...
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_DebugWriteRx(SPI_T *spi, uint32_t *data)
{
	// tbc...
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_ResetTxFifo(SPI_T *spi)
{
	HAL_SET_BIT(spi->FCTL, SPI_FCTL_TF_RST_MASK);
	while(HAL_GET_BIT(spi->FCTL, SPI_FCTL_TF_RST_MASK) != 0);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_ResetRxFifo(SPI_T *spi)
{
	HAL_SET_BIT(spi->FCTL, SPI_FCTL_RF_RST_MASK);
	while(HAL_GET_BIT(spi->FCTL, SPI_FCTL_RF_RST_MASK) != 0);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_DMA(SPI_T *spi, bool txEn, bool rxEn)
{
	HAL_MODIFY_REG(spi->FCTL,
				   SPI_FCTL_TF_DRQ_EN_MASK | SPI_FCTL_RF_DRQ_EN_MASK,
				   ((!!txEn) << SPI_FCTL_TF_DRQ_EN_SHIFT) | ((!!rxEn) << SPI_FCTL_RF_DRQ_EN_SHIFT));
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_SetTxFifoThreshold(SPI_T *spi, uint8_t threshold)
{
	HAL_MODIFY_REG(spi->FCTL, SPI_FCTL_TX_TRIG_LEVEL_MASK, threshold << SPI_FCTL_TX_TRIG_LEVEL_SHIFT);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_SetRxFifoThreshold(SPI_T *spi, uint8_t threshold)
{
	HAL_MODIFY_REG(spi->FCTL, SPI_FCTL_RX_TRIG_LEVEL_MASK, threshold << SPI_FCTL_RX_TRIG_LEVEL_SHIFT);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
uint8_t SPI_GetTxFifoCounter(SPI_T *spi)
{
	return (uint8_t)((spi->FST & SPI_FST_TF_CNT_MASK) >> SPI_FST_TF_CNT_SHIFT);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
uint8_t SPI_GetRxFifoCounter(SPI_T *spi)
{
	return (uint8_t)((spi->FST & SPI_FST_RF_CNT_MASK) >> SPI_FST_RF_CNT_SHIFT);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_EnableDualMode(SPI_T *spi)
{
	HAL_SET_BIT(spi->BCC, SPI_BCC_DRM_MASK);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_DisableDualMode(SPI_T *spi)
{
	HAL_CLR_BIT(spi->BCC, SPI_BCC_DRM_MASK);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_SetInterval(SPI_T *spi, uint16_t nSCLK)
{
	HAL_MODIFY_REG(spi->WAIT, SPI_WAIT_WCC_MASK, nSCLK << SPI_WAIT_WCC_SHIFT);
}

/*
 * @brief
 */
static void SPI_SetClkDiv(SPI_T *spi, uint16_t div)
{
	uint8_t n = 0;
	if (div < 1) {
		SPI_DEBUG("spi div < 1\n");
		return;
	}

	if (div > 2 * (0xFF + 1)) {
		HAL_CLR_BIT(spi->CCTR, SPI_CCTR_DRS_MASK);
		do {
			div = (div == 1) ? 0 : ((div + 1) / 2);
			n++;
		} while (div);

		SPI_DEBUG("SPI CLK Div into 2^%d\n", n);
		HAL_MODIFY_REG(spi->CCTR, SPI_CCTR_CDR1_MASK, (n & 0x0F) << SPI_CCTR_CDR1_SHIFT);
	} else {
		HAL_SET_BIT(spi->CCTR, SPI_CCTR_DRS_MASK);
		n = ((div + 1)/ 2) - 1;
		HAL_MODIFY_REG(spi->CCTR, SPI_CCTR_CDR2_MASK, (n & 0xFF) << SPI_CCTR_CDR2_SHIFT);
	}
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_SetDataSize(SPI_T *spi, uint32_t data_size, uint32_t dummy_size)
{
	HAL_MODIFY_REG(spi->BC, SPI_BC_MBC_MASK, data_size + dummy_size);
	HAL_MODIFY_REG(spi->TC, SPI_TC_MWTC_MASK, data_size);
	HAL_MODIFY_REG(spi->BCC, SPI_BCC_STC_MASK, data_size);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_Write(SPI_T *spi, uint8_t *data)
{
	HAL_REG_8BIT(&spi->TXD) = *data;
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
void SPI_Read(SPI_T *spi, uint8_t *data)
{
	*data = HAL_REG_8BIT(&spi->RXD);
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
uint8_t *SPI_TxAddress(SPI_T *spi)
{
	return (uint8_t *)&spi->TXD;
}

/*
 * @brief
 */
__SPI_STATIC_INLINE__
uint8_t *SPI_RxAddress(SPI_T *spi)
{
	return (uint8_t *)&spi->RXD;
}


/************************ private **************************************/

typedef struct {
	uint8_t		*ptr; /* Pointer to SPI transfer Buffer */
	uint32_t	size; /* SPI transfer size */
	uint32_t	count; /* SPI Transfer Counter */
} SPI_TransferBuffer;

#define SPI_TRANSFERBUFFER_DEFAULT \
	{ \
	 .ptr = NULL, \
	 .size = 0, \
	 .count = 0\
	}


/**
  * @brief  HAL SPI State structure definition
  */
typedef enum
{
  SPI_STATUS_RESET      = 0x00,		/*!< SPI not yet initialized or disabled                */
  SPI_STATUS_READY      = 0x01,		/*!< SPI initialized and ready for use                  */
  SPI_STATUS_BUSY       = 0x02,		/*!< SPI is occupied                             */
  SPI_STATUS_BUSY_TX    = 0x12,		/*!< Data Transmission process is ongoing               */
  SPI_STATUS_BUSY_RX    = 0x22,		/*!< Data Reception process is ongoing                  */
  SPI_STATUS_BUSY_TX_RX = 0x32,		/*!< Data Transmission and Reception process is ongoing */
  SPI_STATUS_ERROR      = 0x03		/*!< SPI error state                                    */
} SPI_Status;

typedef struct {
	SPI_Status status;
	HAL_Mutex lock;
} SPI_StateMachine;

typedef struct {
	SPI_T				*spi;
	SPI_Config 			config;
	SPI_StateMachine 	sm;
	bool				dmaSupport;
	DMA_Channel			tx_dmaChannel;
	DMA_Channel			rx_dmaChannel;
	SPI_TransferBuffer 	tx; // can be optimized
	SPI_TransferBuffer 	rx;	// can be optimized
	uint32_t			mclk;
	HAL_Semaphore		block;
	SPI_IO_Mode			ioMode;
	bool				cs_idle;
	SPI_CS				cs_using;
} SPI_Handler;



static SPI_Handler spi_handler[SPI_NUM] = {
		{
			.spi			= 		(SPI_T *)SPI0_BASE,
			.tx_dmaChannel	=		DMA_CHANNEL_INVALID,
			.rx_dmaChannel	=		DMA_CHANNEL_INVALID,
			.dmaSupport		= 		0,
			.mclk			= 		0,
			.rx				= 		{0}, //SPI_TRANSFERBUFFER_DEFAULT,
			.tx				=		{0}, //SPI_TRANSFERBUFFER_DEFAULT,
			.sm				=		{0},
			.config			=		SPI_CONFIG_DEFAULT,
			.block			=		{0},
			.ioMode			= 		SPI_IO_MODE_NORMAL,
			.cs_idle		=		1,
			.cs_using		=		SPI_TCTRL_SS_SEL_SS0
		},
		{
			.spi 			= 		(SPI_T *)SPI1_BASE,
			.tx_dmaChannel	=		DMA_CHANNEL_INVALID,
			.rx_dmaChannel	=		DMA_CHANNEL_INVALID,
			.dmaSupport		= 		0,
			.mclk			= 		0,
			.rx				= 		{0}, //SPI_TRANSFERBUFFER_DEFAULT,
			.tx				=		{0}, //SPI_TRANSFERBUFFER_DEFAULT,
			.sm				=		{0},
			.config			=		SPI_CONFIG_DEFAULT,
			.block			= 		{0},
			.ioMode			= 		SPI_IO_MODE_NORMAL,
			.cs_idle		=		1,
			.cs_using		=		SPI_TCTRL_SS_SEL_SS0
		}
};

/*
 * @brief
 */
__SPI_STATIC_INLINE__
SPI_Handler *HAL_SPI_GetInstance(SPI_Port port)
{
	return &spi_handler[port];
}


/*
 * @brief
 */
static void HAL_SPI_EnableCCMU(SPI_Port port)
{
	if (port == SPI0) {
		HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_SPI0);
		HAL_CCM_SPI0_EnableMClock();
	} else if (port == SPI1) {
		HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_SPI1);
		HAL_CCM_SPI1_EnableMClock();
	}
}


/*
 * @brief
 */
static void HAL_SPI_DisableCCMU(SPI_Port port)
{
	if (port == SPI0) {
		HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_SPI0);
		HAL_CCM_SPI0_DisableMClock();
	} else if (port == SPI1) {
		HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_SPI1);
		HAL_CCM_SPI1_DisableMClock();
	}
}

static inline void HAL_SPI_ResetCCMU(SPI_Port port)
{
	CCM_BusPeriphBit ccm_spi = CCM_BUS_PERIPH_BIT_SPI0 + port;	/* spi_port translate to ccm_bit */
	HAL_CCM_BusForcePeriphReset(ccm_spi);
	HAL_CCM_BusReleasePeriphReset(ccm_spi);
}


/*
 * @brief
 */
static bool HAL_SPI_ConfigCCMU(SPI_Port port, uint32_t clk)
{
	CCM_AHBPeriphClkSrc src;
	uint32_t mclk;
	uint32_t div;

	if (clk > HAL_GetHFClock())
	{
		mclk = HAL_PRCM_GetDevClock();
		src = CCM_AHB_PERIPH_CLK_SRC_DEVCLK;
		SPI_DEBUG("CCMU src = CCM_AHB_PERIPH_CLK_SRC_DEVCLK.\n");
	}
	else
	{
		mclk = HAL_GetHFClock();
		src = CCM_AHB_PERIPH_CLK_SRC_HFCLK;
		SPI_DEBUG("CCMU src = CCM_AHB_PERIPH_CLK_SRC_HFCLK.\n");
	}

	div = (mclk + clk - 1) / clk;
	div = div==0 ? 1 : div;

	SPI_DEBUG("CCMU div = %d\n", div);
	if (div > (16 * 8))
		return 0;

	if (port == SPI0)
	{
		if (div > 64)
			HAL_CCM_SPI0_SetMClock(src, CCM_PERIPH_CLK_DIV_N_8, (CCM_PeriphClkDivM)((div >> 3) - 1));
		else if (div > 32)
			HAL_CCM_SPI0_SetMClock(src, CCM_PERIPH_CLK_DIV_N_4, (CCM_PeriphClkDivM)((div >> 2) - 1));
		else if (div > 16)
			HAL_CCM_SPI0_SetMClock(src, CCM_PERIPH_CLK_DIV_N_2, (CCM_PeriphClkDivM)((div >> 1) - 1));
		else
			HAL_CCM_SPI0_SetMClock(src, CCM_PERIPH_CLK_DIV_N_1, (CCM_PeriphClkDivM)((div >> 0) - 1));
	}
	else if (port == SPI1)
	{
		if (div > 64)
			HAL_CCM_SPI1_SetMClock(src, CCM_PERIPH_CLK_DIV_N_8, (CCM_PeriphClkDivM)((div >> 3) - 1));
		else if (div > 32)
			HAL_CCM_SPI1_SetMClock(src, CCM_PERIPH_CLK_DIV_N_4, (CCM_PeriphClkDivM)((div >> 2) - 1));
		else if (div > 16)
			HAL_CCM_SPI1_SetMClock(src, CCM_PERIPH_CLK_DIV_N_2, (CCM_PeriphClkDivM)((div >> 1) - 1));
		else
			HAL_CCM_SPI1_SetMClock(src, CCM_PERIPH_CLK_DIV_N_1, (CCM_PeriphClkDivM)((div >> 0) - 1));
	}

	return 1;
}


/*
 * @brief
 */
static void HAL_SPI_TxDMAIntFunc(void *arg)
{
	SPI_Handler *hdl = (SPI_Handler *)arg;

//	SPI_DEBUG("tx DMA Interrupt for SPI.\n");

	if (hdl->sm.status != SPI_STATUS_BUSY_TX_RX) {
		SPI_DMA(hdl->spi, 0, 0);
		HAL_SemaphoreRelease(&hdl->block);
	}
}

static void HAL_SPI_RxDMAIntFunc(void *arg)
{
	SPI_Handler *hdl = (SPI_Handler *)arg;

//	SPI_DEBUG("rx DMA Interrupt for SPI.\n");
	SPI_DMA(hdl->spi, 0, 0);
	HAL_SemaphoreRelease(&hdl->block);
}

int pm_resume[SPI_NUM] = {0};
#ifdef CONFIG_PM
static int spi_suspend(struct soc_device *dev, enum suspend_state_t state)
{
	SPI_Handler *hdl = HAL_SPI_GetInstance((SPI_Port)dev->platform_data);
	if (hdl->sm.status != SPI_STATUS_READY)
		return -1;
	return 0;
}

static int spi_resume(struct soc_device *dev, enum suspend_state_t state)
{
	SPI_Port port = (SPI_Port)dev->platform_data;
	SPI_Handler *hdl = HAL_SPI_GetInstance((SPI_Port)dev->platform_data);
	SPI_T *spi = hdl->spi;

	switch (state) {
	case PM_MODE_SLEEP:
		break;
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
		pm_resume[port] = 1;
		// CCMU config
		HAL_SPI_DisableCCMU(port);
		HAL_SPI_ResetCCMU(port);
		HAL_SPI_ConfigCCMU(port, hdl->mclk);
		HAL_SPI_EnableCCMU(port);

		SPI_Disable(spi);
		SPI_Reset(spi);
		SPI_ResetRxFifo(spi);
		SPI_ResetTxFifo(spi);

		HAL_SPI_DisableCCMU(port);
		break;
	default:
		break;
	}

	return 0;
}

static struct soc_device_driver spi_drv = {
	.name = "spi",
	.suspend_noirq = spi_suspend,
	.resume_noirq = spi_resume,
};

static struct soc_device spi_dev[SPI_NUM] = {
	{.name = "spi0", .driver = &spi_drv, .platform_data = (void *)SPI0,},
	{.name = "spi1", .driver = &spi_drv, .platform_data = (void *)SPI1,},
};

#define SPI_DEV(id) (&spi_dev[id])

#endif


/************************ public **************************************/

/**
  * @brief Initialize SPI driver.
  * @note Each SPI port can master sevral devices, HAL_SPI_Init initialize the
  *       SPI port for its own devices. Mclk configed here must be the max freq
  *       of all devices under this SPI port.
  * @param port: spi port
  * @param gconfig:
  *        @arg gconfig->mclk: SPI controller frequency. It must be the the max
  *             freq of all devices under this SPI port.
  *        @arg gconfig->cs_level: chip selected level of SPI devices enabled,
  *             cs level should be same amound all devices.
  * @retval HAL_Status: The status of driver
  */
HAL_Status HAL_SPI_Init(SPI_Port port, const SPI_Global_Config *gconfig)
{
	SPI_Handler *hdl = HAL_SPI_GetInstance(port);
	SPI_T *spi = hdl->spi;
	HAL_Status ret = HAL_OK;

	SPI_ENTRY();
	SPI_ASSERT(port > SPI_NUM);

	if (hdl->sm.status != SPI_STATUS_RESET) {
		ret = HAL_ERROR;
		SPI_ALERT("Changing State incorrectly in %s\n", __func__);
		goto out;
	}

	if ((ret = HAL_MutexInit(&hdl->sm.lock)) != HAL_OK) {
		SPI_ALERT("Mutex init failed\n");
		goto out;
	}

	if ((ret = HAL_SemaphoreInit(&hdl->block, 0, 1)) != HAL_OK) {
		HAL_MutexDeinit(&hdl->sm.lock);
		SPI_ALERT("Semaphore init failed\n");
		goto out;
	}

	HAL_Memset(&hdl->tx, 1, sizeof(SPI_TransferBuffer));
	HAL_Memset(&hdl->rx, 1, sizeof(SPI_TransferBuffer));
//	hdl->mclk = SPI_SOURCE_CLK;
	hdl->mclk = gconfig->mclk * 1; /* a larger mclk could be divided easier by
							 multi-device-frequency, 2 is not necessary */

	hdl->cs_idle = !gconfig->cs_level;
	SPI_SetCsIdle(spi, hdl->cs_idle);

	// CCMU config
	HAL_SPI_DisableCCMU(port);
	HAL_SPI_ResetCCMU(port);
	if (HAL_SPI_ConfigCCMU(port, hdl->mclk) != 1) {
		SPI_ALERT("mclk by CCMU divided too low, config failed.\n");
		ret = HAL_ERROR;
		goto out;
	}
	HAL_SPI_EnableCCMU(port);

	SPI_Disable(spi);
	SPI_Reset(spi);
	SPI_ResetRxFifo(spi);
	SPI_ResetTxFifo(spi);

	// default SPI Register Config
	SPI_SetDuplex(spi, SPI_TCTRL_DHB_HALF_DUPLEX);
	SPI_SetMode(spi, hdl->config.mode);
	SPI_SetFirstTransmitBit(spi, hdl->config.firstBit);
	SPI_SetSclkMode(spi, hdl->config.sclkMode);
	SPI_SetClkDiv(spi, (hdl->mclk + hdl->config.sclk - 1) / hdl->config.sclk);

	hdl->sm.status = SPI_STATUS_READY;

	SPI_REG_ALL(hdl->spi, "inited");
	HAL_SPI_DisableCCMU(port);

#ifdef CONFIG_PM
	pm_register_ops(SPI_DEV(port));
#endif

out:
	SPI_EXIT(ret);
	return ret;
}

/**
  * @brief Deinitialize SPI driver.
  * @param port: spi port
  * @retval HAL_Status: The status of driver
  */
HAL_Status HAL_SPI_Deinit(SPI_Port port)
{
	SPI_Handler *hdl = HAL_SPI_GetInstance(port);
	HAL_Status ret = HAL_OK;

	SPI_ENTRY();

	if (hdl->sm.status != SPI_STATUS_READY) {
		ret = HAL_ERROR;
		SPI_ALERT("Changing State incorrectly in %s\n", __func__);
		goto out;
	}

#ifdef CONFIG_PM
	pm_unregister_ops(SPI_DEV(port));
#endif

	HAL_MutexDeinit(&hdl->sm.lock);
	HAL_SemaphoreDeinit(&hdl->block);
	SPI_Disable(hdl->spi);
	HAL_SPI_DisableCCMU(port);
	hdl->sm.status = SPI_STATUS_RESET;

out:
	SPI_EXIT(ret);
	return ret;
}

/**
  * @brief Open a SPI device before using this spi device.
  * @note Other SPI devices will block when open a spi device, please make sure
  *       this device will be close eventually.
  * @param port: spi port
  * @param cs: spi cs pin. SPI_TCTRL_SS_SEL_SS0~3 are pin cs0~cs3.
  * @param config:
  *        @arg config->mode: spi in master mode or slave mode. slave is not
  *             supported for now.
  *        @arg config->opMode: use dma to move data or CPU to move data.
  *        @arg config->firstBit: data on line is MSB or LSB.
  *        @arg config->sclk: spi device working frequency.
  *        @arg config->sclkMode: SPI sclk mode.
  * @param msec: timeout in millisecond
  * @retval HAL_Status: The status of driver
  */
HAL_Status HAL_SPI_Open(SPI_Port port, SPI_CS cs, SPI_Config *config, uint32_t msec)
{
	SPI_Handler *hdl = HAL_SPI_GetInstance(port);
	SPI_T *spi = hdl->spi;
	HAL_Status ret = HAL_OK;
	unsigned long flags;

	SPI_ENTRY();
	flags = HAL_EnterCriticalSection();

	if ((ret = HAL_MutexLock(&hdl->sm.lock, msec)) != HAL_OK)
		goto out;
	if (hdl->sm.status != SPI_STATUS_READY) {
		ret = HAL_ERROR;
		SPI_ALERT("Changing State incorrectly in %s, state: %d -> busy\n", __func__, hdl->sm.status);
		goto failed;
	}
	hdl->sm.status = SPI_STATUS_BUSY;

	hdl->cs_using = cs;
	HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_SPI, port), cs);

	HAL_SPI_EnableCCMU(port);

	// DMA config
	if (config->opMode == SPI_OPERATION_MODE_DMA) {
		DMA_ChannelInitParam tx_param;
		DMA_ChannelInitParam rx_param;
		HAL_Memset(&tx_param, 0, sizeof(tx_param));
		HAL_Memset(&rx_param, 0, sizeof(rx_param));

		if ((hdl->tx_dmaChannel = HAL_DMA_Request()) == DMA_CHANNEL_INVALID) {
			SPI_ALERT("DMA request failed \n");
			goto init_failed;
		}
		if ((hdl->rx_dmaChannel = HAL_DMA_Request()) == DMA_CHANNEL_INVALID) {
			SPI_ALERT("DMA request failed \n");
			HAL_DMA_Release(hdl->tx_dmaChannel);
			goto init_failed;
		}

		tx_param.cfg = HAL_DMA_MakeChannelInitCfg(DMA_WORK_MODE_SINGLE,
											   DMA_WAIT_CYCLE_2,
											   DMA_BYTE_CNT_MODE_REMAIN,
											   DMA_DATA_WIDTH_8BIT,
											   DMA_BURST_LEN_1,
											   DMA_ADDR_MODE_FIXED,
											   (DMA_Periph)(DMA_PERIPH_SPI0 + port),
											   DMA_DATA_WIDTH_8BIT,
											   DMA_BURST_LEN_1,
											   DMA_ADDR_MODE_INC,
											   DMA_PERIPH_SRAM);
		tx_param.irqType = DMA_IRQ_TYPE_END;
		tx_param.endCallback = HAL_SPI_TxDMAIntFunc;
		tx_param.endArg = hdl;

		rx_param.cfg = HAL_DMA_MakeChannelInitCfg(DMA_WORK_MODE_SINGLE,
											   DMA_WAIT_CYCLE_2,
											   DMA_BYTE_CNT_MODE_REMAIN,
											   DMA_DATA_WIDTH_8BIT,
											   DMA_BURST_LEN_1,
											   DMA_ADDR_MODE_INC,
											   DMA_PERIPH_SRAM,
											   DMA_DATA_WIDTH_8BIT,
											   DMA_BURST_LEN_1,
											   DMA_ADDR_MODE_FIXED,
											   (DMA_Periph)(DMA_PERIPH_SPI0 + port));
		rx_param.irqType = DMA_IRQ_TYPE_END;
		rx_param.endCallback = HAL_SPI_RxDMAIntFunc;
		rx_param.endArg = hdl;

		HAL_DMA_Init(hdl->rx_dmaChannel, &rx_param);
		HAL_DMA_Init(hdl->tx_dmaChannel, &tx_param);
	}

	SPI_Disable(spi);

	if (HAL_Memcmp(config, &hdl->config, sizeof(*config)) || pm_resume[port]) {	// ________________maybe unuseful_______________ //
		HAL_Memcpy(&hdl->config, config, sizeof(*config));
		SPI_SetMode(spi, config->mode);
		SPI_SetFirstTransmitBit(spi, config->firstBit);
		SPI_SetSclkMode(spi, config->sclkMode);
		SPI_SetClkDiv(spi, (hdl->mclk + config->sclk - 1) / config->sclk);
		SPI_SetDuplex(spi, SPI_TCTRL_DHB_HALF_DUPLEX);
		pm_resume[port] = 0;
	}

/*	if (config->csMode == SPI_CS_MODE_AUTO)
		SPI_AutoChipSelect(spi, cs, 1, hdl->cs_idle);
	else {
		SPI_ManualChipSelect(spi, cs, hdl->cs_idle);
		SPI_SetCsLevel(spi, hdl->cs_idle);
	}*/
	SPI_ManualChipSelect(spi, cs);
	SPI_SetDataSize(spi, 0, 0);
	SPI_Enable(spi);
	SPI_REG_ALL(spi, "opened");

	HAL_ExitCriticalSection(flags);
	SPI_EXIT(ret);
	return HAL_OK;

init_failed:
	HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_SPI, port), cs);
failed:
	HAL_MutexUnlock(&hdl->sm.lock);
out:
	HAL_ExitCriticalSection(flags);
	SPI_EXIT(ret);
	return ret;
}

/**
  * @brief Close a SPI device to release SPI port.
  * @param port: spi port
  * @retval HAL_Status: The status of driver
  */
HAL_Status HAL_SPI_Close(SPI_Port port)
{
	SPI_Handler *hdl = HAL_SPI_GetInstance(port);
	HAL_Status ret = HAL_OK;
	unsigned long flags;

	SPI_ENTRY();
	flags = HAL_EnterCriticalSection();


	if (hdl->sm.status != SPI_STATUS_BUSY) {
		ret = HAL_ERROR;
		SPI_ALERT("Changing State incorrectly in %s, state: %d -> ready\n", __func__, hdl->sm.status);
		goto out;
	}
	hdl->sm.status = SPI_STATUS_READY;

	if (hdl->config.opMode == SPI_OPERATION_MODE_DMA) {
		HAL_DMA_Stop(hdl->tx_dmaChannel);
		HAL_DMA_Stop(hdl->rx_dmaChannel);
		HAL_DMA_DeInit(hdl->tx_dmaChannel);
		HAL_DMA_DeInit(hdl->rx_dmaChannel);
		HAL_DMA_Release(hdl->tx_dmaChannel);
		HAL_DMA_Release(hdl->rx_dmaChannel);
	}

	HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_SPI, port), hdl->cs_using);
	SPI_Disable(hdl->spi);
	HAL_SPI_DisableCCMU(port);
	HAL_SPI_Config(port, SPI_ATTRIBUTION_IO_MODE, SPI_IO_MODE_NORMAL);
	HAL_MutexUnlock(&hdl->sm.lock);
out:
	HAL_ExitCriticalSection(flags);
	SPI_EXIT(ret);
	return ret;

}

/**
  * @brief Enable SPI device or not by chip select pin
  * @note Example: device is select by low voltage level, HAL_SPI_CS(spi_port, 1)
  *       output cs pin to low voltage level.
  * @param port: spi port.
  * @param select: select/enable device or not.
  * @retval HAL_Status: The status of driver
  */
HAL_Status HAL_SPI_CS(SPI_Port port, bool select)
{
	SPI_Handler *hdl = HAL_SPI_GetInstance(port);
	SPI_ENTRY();

	SPI_SetCsLevel(hdl->spi, select ^ hdl->cs_idle);

	SPI_EXIT(HAL_OK);
	return HAL_OK;
}

/**
  * @brief Config SPI attribution, such as dual rx mode.
  * @note This is a ioctl.
  *       attr : arg
  *       SPI_ATTRIBUTION_IO_MODE : SPI_IO_MODE_NORMAL
  *                               : SPI_IO_MODE_DUAL_RX
  *       other is not support for now.
  * @param port: spi port
  * @param attr:
  *        @arg SPI_ATTRIBUTION_IO_MODE: config SPI to a IO mode, for example:
  *             flash has a dual output fast read mode may use this. And the
  *             arg can be SPI_IO_MODE_NORMAL or SPI_IO_MODE_DUAL_RX, others
  *             are not support for now.
  * @param arg: an arguement according attribution.
  * @retval HAL_Status: The status of driver
  */
HAL_Status HAL_SPI_Config(SPI_Port port, SPI_Attribution attr, uint32_t arg)
{
	SPI_Handler *hdl = HAL_SPI_GetInstance(port);
	HAL_Status ret = HAL_OK;
	SPI_ENTRY();

	if (attr == SPI_ATTRIBUTION_IO_MODE) {
		if (arg == SPI_IO_MODE_NORMAL)
			hdl->ioMode = SPI_IO_MODE_NORMAL;
		else if (arg == SPI_IO_MODE_DUAL_RX)
			hdl->ioMode = SPI_IO_MODE_DUAL_RX;
		else {
			ret = HAL_ERROR;
			goto out;
		}
	}

out:
	SPI_EXIT(ret);
	return ret;
}

/**
  * @brief Receive data from SPI device.
  * @param port: spi port
  * @param data: the buf to store received data, created by user.
  * @param size: the data size needed to receive.
  * @retval HAL_Status: The status of driver
  */
HAL_Status HAL_SPI_Receive(SPI_Port port, uint8_t *data, uint32_t size)
{
	SPI_Handler *hdl = HAL_SPI_GetInstance(port);
	SPI_TransferBuffer *rx = &hdl->rx;
	SPI_T *spi = hdl->spi;
	HAL_Status ret = HAL_OK;
	unsigned long flags;

	SPI_ENTRY();
	flags = HAL_EnterCriticalSection();

	if (hdl->sm.status != SPI_STATUS_BUSY) {
		ret = HAL_INVALID;
		SPI_ALERT("Changing State incorrectly in %s, state: %d -> rx\n", __func__, hdl->sm.status);
		HAL_ExitCriticalSection(flags);
		goto out;
	}
	hdl->sm.status = SPI_STATUS_BUSY_RX;
	HAL_ExitCriticalSection(flags);

	if (hdl->ioMode == SPI_IO_MODE_DUAL_RX)
		SPI_EnableDualMode(spi);

	// check spi enable?

	rx->ptr = data;
	rx->count = size;
	rx->size = size;
	SPI_SetDataSize(spi, 0, rx->size);

	SPI_SetDuplex(hdl->spi, SPI_TCTRL_DHB_HALF_DUPLEX);
	SPI_ResetRxFifo(spi);

	SPI_REG_ALL(spi, "prepare to receive");

	if (hdl->config.opMode == SPI_OPERATION_MODE_DMA)
	{
		HAL_DMA_Start(hdl->rx_dmaChannel, (uint32_t)SPI_RxAddress(hdl->spi), (uint32_t)rx->ptr, rx->count);

		SPI_DMA(spi, 0, 1);
		SPI_StartTransmit(spi);

		SPI_REG_ALL(spi, "spi receive dma start");
		HAL_SemaphoreWait(&hdl->block, SPI_MAX_WAIT_MS);	//--------------------return error
		HAL_DMA_Stop(hdl->rx_dmaChannel);
		while (SPI_IntState(hdl->spi, SPI_INT_TRANSFER_COMPLETE) == 0);
		SPI_ClearInt(spi, SPI_INT_TRANSFER_COMPLETE);
	}
	else if (hdl->config.opMode == SPI_OPERATION_MODE_POLL)
	{
		SPI_StartTransmit(spi);
		SPI_REG_ALL(spi, "receive start");

		while (rx->count > 0)
		{
			uint32_t len;
			SPI_REG_ALL(spi, "receiving");

/*			while (SPI_IntState(spi, SPI_INT_RXFIFO_EMPTY) != 0);
			rx->count--;
			SPI_Read(spi, rx->ptr);
			rx->ptr++;*/
			len = SPI_GetRxFifoCounter(spi);
			while (len--) {
				rx->count--;
				SPI_Read(spi, rx->ptr);
				rx->ptr++;
			}
		}

		while (SPI_IntState(hdl->spi, SPI_INT_TRANSFER_COMPLETE) == 0);
		SPI_ClearInt(spi, SPI_INT_TRANSFER_COMPLETE);
	}

	if (hdl->ioMode == SPI_IO_MODE_DUAL_RX)
		SPI_DisableDualMode(spi);

	SPI_REG_ALL(spi, "receive completed");
	SPI_DEBUG_TRANSFER_DATA(data, size, "Rx");

out:
	hdl->sm.status = SPI_STATUS_BUSY;
	SPI_EXIT(ret);
	return ret;
}


/**
  * @brief Transmit data to SPI device.
  * @param port: spi port
  * @param data: the data transmitted to SPI device.
  * @param size: the data size needed to transmit.
  * @retval HAL_Status: The status of driver
  */
HAL_Status HAL_SPI_Transmit(SPI_Port port, uint8_t *data, uint32_t size) /* timeout is not that timeout */
{
	SPI_Handler *hdl = HAL_SPI_GetInstance(port);
	SPI_TransferBuffer *tx = &hdl->tx;
	SPI_T *spi = hdl->spi;
	HAL_Status ret = HAL_OK;
	unsigned long flags;

	flags = HAL_EnterCriticalSection();
	if (hdl->sm.status != SPI_STATUS_BUSY) {
		ret = HAL_INVALID;
		SPI_ALERT("Changing State incorrectly in %s, state: %d -> tx\n", __func__, hdl->sm.status);
		HAL_ExitCriticalSection(flags);
		goto out;
	}
	hdl->sm.status = SPI_STATUS_BUSY_TX;
	HAL_ExitCriticalSection(flags);

	// check spi enable?

	SPI_ResetTxFifo(spi);
	SPI_SetDuplex(hdl->spi, SPI_TCTRL_DHB_HALF_DUPLEX);

	tx->ptr = data;
	tx->count = size;
	tx->size = size;
	SPI_SetDataSize(spi, tx->size, 0);

	SPI_REG_ALL(spi, "prepare transmit");

	if (hdl->config.opMode == SPI_OPERATION_MODE_DMA)
	{
		HAL_DMA_Start(hdl->tx_dmaChannel, (uint32_t)tx->ptr, (uint32_t)SPI_TxAddress(hdl->spi), tx->count);

		SPI_DMA(spi, 1, 0);
		SPI_StartTransmit(spi);

		HAL_SemaphoreWait(&hdl->block, SPI_MAX_WAIT_MS);
		SPI_REG_ALL(spi, "writing by DMA finish");
		while (SPI_IntState(hdl->spi, SPI_INT_TRANSFER_COMPLETE) == 0);
		SPI_ClearInt(spi, SPI_INT_TRANSFER_COMPLETE);
		HAL_DMA_Stop(hdl->tx_dmaChannel);
		//ret = HAL_DMA_GetByteCount(hdl->tx_dmaChannel);
	}
	else if (hdl->config.opMode == SPI_OPERATION_MODE_POLL)
	{
		SPI_StartTransmit(spi);
		SPI_REG_ALL(spi, "transmit start, TCTRL 31 bit should be 1");
		while (tx->count > 0)
		{
//			while (SPI_IntState(spi, SPI_INT_TXFIFO_FULL) != 0);
			while (SPI_GetTxFifoCounter(spi) == SPI_FIFO_SIZE);
			tx->count--;
			SPI_Write(spi, tx->ptr);
			tx->ptr++;

			SPI_REG_ALL(spi, "writing");
		}

		while (SPI_IntState(hdl->spi, SPI_INT_TRANSFER_COMPLETE) == 0);
		SPI_ClearInt(spi, SPI_INT_TRANSFER_COMPLETE);
	}

	SPI_REG_ALL(spi, "transmit completed");
	SPI_DEBUG_TRANSFER_DATA(data, size, "Tx");

out:
	hdl->sm.status = SPI_STATUS_BUSY;
	SPI_EXIT(ret);
	return ret;
}

/**
  * @brief Transmit and Receive data from SPI device in the same time.
  * @note Transmit data by MOSI pin, and the exactly same time, receive the
  *       data by MISO pin.
  * @param port: spi port
  * @param tx_data: the data transmitted to SPI device.
  * @param rx_data: the buf to store received data, created by user.
  * @param size: the data size needed to transmit and receive.
  * @retval HAL_Status: The status of driver
  */
HAL_Status HAL_SPI_TransmitReceive(SPI_Port port, uint8_t *tx_data, uint8_t *rx_data, uint32_t size)
{
	SPI_Handler *hdl = HAL_SPI_GetInstance(port);
	SPI_TransferBuffer *tx = &hdl->tx;
	SPI_TransferBuffer *rx = &hdl->rx;
	SPI_T *spi = hdl->spi;
	HAL_Status ret = HAL_OK;
	unsigned long flags;

	flags = HAL_EnterCriticalSection();
	if (hdl->sm.status != SPI_STATUS_BUSY) {
		ret = HAL_INVALID;
		SPI_ALERT("Changing State incorrectly in %s, state: %d -> rx\n", __func__, hdl->sm.status);
		HAL_ExitCriticalSection(flags);
		goto out;
	}
	hdl->sm.status = SPI_STATUS_BUSY_TX_RX;
	HAL_ExitCriticalSection(flags);

	// check spi enable?

	tx->ptr = tx_data;
	tx->count = size;
	tx->size = size;
	rx->ptr = rx_data;
	rx->count = size;
	rx->size = size;
	SPI_SetDataSize(spi, size, 0);

	SPI_SetDuplex(hdl->spi, SPI_TCTRL_DHB_FULL_DUPLEX);
	SPI_ResetTxFifo(spi);
	SPI_ResetRxFifo(spi);

	if (hdl->config.opMode == SPI_OPERATION_MODE_DMA)
	{
		HAL_DMA_Start(hdl->tx_dmaChannel, (uint32_t)tx->ptr, (uint32_t)SPI_TxAddress(hdl->spi), tx->count);
		HAL_DMA_Start(hdl->rx_dmaChannel, (uint32_t)SPI_RxAddress(hdl->spi), (uint32_t)rx->ptr, rx->count);

		SPI_DMA(spi, 1, 1);
		SPI_StartTransmit(spi);

		HAL_SemaphoreWait(&hdl->block, SPI_MAX_WAIT_MS);
		HAL_DMA_Stop(hdl->tx_dmaChannel);
		HAL_DMA_Stop(hdl->rx_dmaChannel);
		while (SPI_IntState(hdl->spi, SPI_INT_TRANSFER_COMPLETE) == 0);
		SPI_ClearInt(spi, SPI_INT_TRANSFER_COMPLETE);
	}
	else if (hdl->config.opMode == SPI_OPERATION_MODE_POLL)
	{
		SPI_StartTransmit(spi);
		while ((tx->count > 0) || (rx->count > 0))
		{
			while ((SPI_IntState(spi, SPI_INT_RXFIFO_FULL) == 0) && (tx->count > 0)) {
				tx->count--;
				SPI_Write(spi, tx->ptr);
				tx->ptr++;
			}
			while (SPI_GetRxFifoCounter(spi) > 0) {
				rx->count--;
				SPI_Read(spi, rx->ptr);
				rx->ptr++;
			}
		}

		if ((tx->count != 0) || (rx->count != 0)) {
			SPI_ALERT("spi_tx_rx error with tx count = %d, rx count = %d.\n", tx->count, rx->count);
			ret = HAL_ERROR;
			goto out;
		}

		while (SPI_IntState(hdl->spi, SPI_INT_TRANSFER_COMPLETE) == 0);
		SPI_ClearInt(spi, SPI_INT_TRANSFER_COMPLETE);

	}

	SPI_DEBUG_TRANSFER_DATA(rx_data, size, "Rx");
	SPI_DEBUG_TRANSFER_DATA(tx_data, size, "Tx");

out:
	hdl->sm.status = SPI_STATUS_BUSY;
	return ret;
}


