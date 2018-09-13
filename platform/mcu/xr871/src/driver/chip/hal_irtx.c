/**
  * @file  hal_irtx.c
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

#include <string.h>
#include "driver/chip/hal_irtx.h"
#include "driver/chip/ir_nec.h"
#include "hal_base.h"
#include "sys/interrupt.h"
#include "pm/pm.h"

#define HAL_DBG_IRTX 0

#if (HAL_DBG_IRTX == 1)
#define HAL_IRTX_DBG(fmt, arg...) HAL_LOG(HAL_DBG_ON && HAL_DBG_IRTX, "[IRTX] "fmt, ##arg)
#define irtx_hex_dump_bytes(...) print_hex_dump_bytes(__VA_ARGS__)
#define IRTX_INF HAL_IRTX_DBG
#define IRTX_WRN HAL_WRN
#define IRTX_ERR HAL_ERR
#else
#define HAL_IRTX_DBG(fmt, arg...)
#define irtx_hex_dump_bytes(...)
#define IRTX_INF HAL_IRTX_DBG
#define IRTX_WRN(...)
#define IRTX_ERR(...)
#endif

/* Bit definition for cir Transmit Global register */
#define IRTX_RESET                      (1<<1)

#define IRTX_TXEN                       (1)

/* Bit definition for cir Transmit Control register */
#define IRTX_RCS_DIV_2                  (1<<1)
#define IRTX_RCS_DIV_4                  (2<<1)
#define IRTX_RCS_DIV_8                  (3<<1)
#define IRTX_RCS_DIV_64                 (4<<1)
#define IRTX_RCS_DIV_128                (5<<1)
#define IRTX_RCS_DIV_256                (6<<1)
#define IRTX_RCS_DIV_512                (7<<1)
#define IRTX_RCS_DIV_MASK               (7<<1)

#define IRTX_CSS_EN                     (1<<7)

/* Bit definition for cir Transmit Interrupt Control register */
#define IRTX_IT_TAI                     (1<<1)  /* TX FIFO Available Interrupt Enable */
#define IRTX_IT_TPE                     (1)     /* Transmit Packet End Interrupt Enable for Cyclical Pulse */
#define IRTX_IT_TUI                     (1)     /* Transmit FIFO under run Interrupt Enable for Non-cyclical Pulse */

/* Bit definition for cir Transmit Status register */
#define IRTX_FLAG_TPE                   (1)     /* Transmitter Packet End Flag for Cyclical Pulse */
#define IRTX_FLAG_TUR                   (1)     /* Transmitter FIFO Under Run Flag for Non-cyclical Pulse */
#define IRTX_FLAG_TAI                   (1<<1)  /* TX FIFO Available Interrupt Flag */

#define IRTX_STCT                       (1<<3)  /* Status of CIR Transmitter */

#define IRTX_CAVE_FREQ                  (38000) /* actually is 6.5M/IRTX_MC_VALUE=38011.7Hz, T=26.3uS */
#if (defined IR_CLK_32K_USED)
#define IRTX_32K_APB_PERIPH_CLK_SRC     CCM_APB_PERIPH_CLK_SRC_LFCLK
#define IRTX_32K_CCM_PERIPH_CLK_DIV_N   CCM_PERIPH_CLK_DIV_N_1
#define IRTX_32K_CCM_PERIPH_CLK_DIV_M   CCM_PERIPH_CLK_DIV_M_1
#define IRTX_32K_CCM_PERIPH_CLK         (32768)
#define IRTX_32K_MC_VALUE               (1)     /* 1 */
#define IRTX_32K_SAMPLE                 IRTX_RCS_DIV_2  /* IRTX_CCM_PERIPH_CLK/2=16.4KHz, Ts=61uS */
#else
#define IRTX_26M_APB_PERIPH_CLK_SRC     CCM_APB_PERIPH_CLK_SRC_HFCLK
#define IRTX_26M_CCM_PERIPH_CLK_DIV_N   CCM_PERIPH_CLK_DIV_N_1
#define IRTX_26M_CCM_PERIPH_CLK_DIV_M   CCM_PERIPH_CLK_DIV_M_8  /* 26M/8=3.25M */
#define IRTX_26M_CCM_PERIPH_CLK         (26000000/8)
#define IRTX_26M_MC_VALUE               (IRTX_26M_CCM_PERIPH_CLK/IRTX_CAVE_FREQ)    /* 3.25MHz/IRTX_CAVE_FREQ=85 */
#define IRTX_26M_SAMPLE                 IRTX_RCS_DIV_128  /* IRTX_CCM_PERIPH_CLK/128=25.4KHz, Ts=39.4uS */

#define IRTX_24M_APB_PERIPH_CLK_SRC     CCM_APB_PERIPH_CLK_SRC_HFCLK
#define IRTX_24M_CCM_PERIPH_CLK_DIV_N   CCM_PERIPH_CLK_DIV_N_1
#define IRTX_24M_CCM_PERIPH_CLK_DIV_M   CCM_PERIPH_CLK_DIV_M_4  /* 24M/4=6M */
#define IRTX_24M_CCM_PERIPH_CLK         (24000000/4)
#define IRTX_24M_MC_VALUE               (IRTX_24M_CCM_PERIPH_CLK/IRTX_CAVE_FREQ)    /* 6MHz/IRTX_CAVE_FREQ=157 */
#define IRTX_24M_SAMPLE                 IRTX_RCS_DIV_256  /* IRTX_CCM_PERIPH_CLK/256=23.4KHz, Ts=43.7uS */
#endif

#define IR_TX_FIFO_SIZE                 (128)

typedef struct
{
	__IO uint32_t TGR;              /* 0x00 IRTX Global Register */
	__IO uint32_t TMCR;             /* 0x04 IRTX Modulationg Control register */
	__IO uint32_t TCR;              /* 0x08 IRTX Control Register */
	__IO uint32_t IDCNT_H;          /* 0x0C IRTX Idle Duration threshold Resgister High */
	__IO uint32_t IDCNT_L;          /* 0x10 IRTX Idle Duration threshold Resgister Low */
	__IO uint32_t TICNT_H;          /* 0x14 IRTX Idle Counter Resgister High */
	__IO uint32_t TICNT_L;          /* 0x18 IRTX Idle Counter Resgister Low */
	     uint32_t RESERVED0[1];
	__IO uint32_t TER;              /* 0x20 IRTX FIFO empty Level Register */
	__IO uint32_t TICR;             /* 0x24 IRTX Interrupt Control Register */
	__IO uint32_t TACR;             /* 0x28 IRTX FIFO Available Counter Register */
	__IO uint32_t SR;               /* 0x2C IRTX Status Register */
	__IO uint32_t TXTR;             /* 0x30 IRTX Threshold Register */
	__IO uint32_t DMACR;            /* 0x34 IRTX DMA Control Register */
	     uint32_t RESERVED1[2];
	     uint32_t RESERVED2[4];     /* 0x40 */
	     uint32_t RESERVED3[4];     /* 0x50 */
	     uint32_t RESERVED4[4];     /* 0x60 */
	     uint32_t RESERVED5[4];     /* 0x70 */
	__IO uint32_t DR;               /* 0x80 IRTX Trainsmit FIFO Data Register */
} IRTX_TypeDef;

/* IRTX State structures definition */
typedef enum
{
	IRTX_STATE_RESET        = 0x00, /* Peripheral is not initialized */
	IRTX_STATE_READY        = 0x01, /* Peripheral Initialized and ready for use */
	IRTX_STATE_BUSY         = 0x02, /* An internal process is ongoing */
	IRTX_STATE_ERROR        = 0x04  /* Error */
} IRTX_StateTypeDef;

#define	IRTX_BUFFER_SIZE        256

/* IRTX handle Structure definition */
struct IRTX_HandleDef
{
	IRTX_TypeDef            *Instance;              /* IRTX registers base address */
	IRTX_TTS_Type           SendModeType;           /* signal or cyclical mode */
	uint32_t                CyclicalCnt;            /* count of cyclical mode */
	uint32_t                CyclicalTxCnt;          /* sended count of cyclical mode */
	uint16_t                TxNum;                  /* Used when send data more than 128 bytes */
	uint8_t                 txBuff[IRTX_BUFFER_SIZE]; /* Pointer to IRRX Rx transfer Buffer */
	uint16_t                txCnt;                  /* IRRX Rx Transfer layer Counter     */
	IRTX_StateTypeDef       State;                  /* IRTX communication state           */
	IRTX_Proto_Code         Protos[IRTX_PROTO_NUM];
};

static IRTX_HandleTypeDef hal_irtx;

static void HAL_IRTX_CycMode_Stop(IRTX_HandleTypeDef *irtx)
{
	if (irtx->SendModeType == IRTX_TTS_CYCLICAL) {
		irtx->Instance->TCR &= ~IRTX_CSS_EN;
		irtx->Instance->TGR |= IRTX_RESET;
		while (irtx->Instance->TGR & IRTX_RESET)
			;

		irtx->State = IRTX_STATE_READY;
	}
}

static void IRTX_IRQHandler(void)
{
	uint32_t intsta;
	IRTX_HandleTypeDef *irtx = &hal_irtx;

	intsta = irtx->Instance->SR;
	irtx->Instance->SR = intsta;
	//IRTX_INF("%s,%d %x\n", __func__, __LINE__, intsta);

	if (irtx->SendModeType == IRTX_TTS_CYCLICAL) {
		irtx->Instance->TICNT_L = 0;
		irtx->Instance->TICNT_H = 0;
		if (++irtx->CyclicalTxCnt >= irtx->CyclicalCnt) {
			HAL_IRTX_CycMode_Stop(irtx);
		}
		return ;
	}

	if (intsta & IRTX_FLAG_TPE) {
		irtx->Instance->TCR &= ~IRTX_CSS_EN;
		irtx->Instance->TICR &= ~IRTX_FLAG_TPE;
		irtx->State = IRTX_STATE_READY;
	}
	if (intsta & IRTX_FLAG_TAI) {
		while (irtx->TxNum < irtx->txCnt) {
			if (irtx->Instance->TACR == 0) {
				IRTX_WRN("%s,%d\n", __func__, __LINE__);
				return ;
			}
			irtx->Instance->DR = irtx->txBuff[irtx->TxNum++];
		}
		if (irtx->TxNum >= irtx->txCnt)
			irtx->Instance->TICR &= ~IRTX_FLAG_TAI;
	}
}

static void IRTX_SetConfig(IRTX_TypeDef *Instance, IRTX_InitTypeDef *Init)
{
#if !defined (IR_CLK_32K_USED)
	uint32_t clk = HAL_GetHFClock();
#endif

	HAL_ASSERT_PARAM(((Init->ModulateDutyLevel) == IRTX_DRMC_TIME_1) || \
	                 ((Init->ModulateDutyLevel) == IRTX_DRMC_TIME_2) || \
	                 ((Init->ModulateDutyLevel) == IRTX_DRMC_TIME_3));
	HAL_ASSERT_PARAM(((Init->SendModeType) == IRTX_TTS_NONE) || \
	                 ((Init->SendModeType) == IRTX_TTS_CYCLICAL));
	HAL_ASSERT_PARAM(((Init->PulsePolarity) == IRTX_TPPI_NONE) || \
	                 ((Init->PulsePolarity) == IRTX_TPPI_INVERT));

#if defined (IR_CLK_32K_USED)
	Instance->TMCR = IRTX_32K_MC_VALUE;

	Instance->TCR &= ~IRTX_RCS_DIV_MASK;
	Instance->TCR |= IRTX_32K_SAMPLE;
#else
	if (clk == HOSC_CLOCK_26M) {
		Instance->TMCR = IRTX_26M_MC_VALUE;

		Instance->TCR &= ~IRTX_RCS_DIV_MASK;
		Instance->TCR |= IRTX_26M_SAMPLE;
	} else if (clk == HOSC_CLOCK_24M) {
		Instance->TMCR = IRTX_24M_MC_VALUE;

		Instance->TCR &= ~IRTX_RCS_DIV_MASK;
		Instance->TCR |= IRTX_24M_SAMPLE;
	}
#endif

	Instance->TGR &= ~IRTX_DRMC_TIME_MASK;
	Instance->TGR |= Init->ModulateDutyLevel;

	Instance->TCR &= ~IRTX_TTS_MASK;
	Instance->TCR |= Init->SendModeType;

	Instance->TGR &= ~IRTX_TPPI_MASK;
	Instance->TGR |= Init->PulsePolarity;

	Instance->IDCNT_H = (Init->IdleDurationCnt >> 8) & 0x0F;
	Instance->IDCNT_L = Init->IdleDurationCnt & 0xff;

	Instance->TGR |= Init->InternalModulation;

	Instance->TER &= ~0x000000FF;
	Instance->TER |= IR_TX_FIFO_SIZE;

	Instance->SR = IRTX_FLAG_TAI | IRTX_FLAG_TPE;

	Instance->TXTR = IR_TX_FIFO_SIZE;

	Instance->TGR |= IRTX_TXEN;
}

#define MIN(x , y)  (((x) < (y)) ? (x) : (y))

/**
 * @brief Send a code by IRTX peripheral.
 * @param irtx:
 *        @arg irtx-> IRTX handler.
 * @param protos_sel:
 *        @arg protos_sel->[in] The protocal used.
 * @param ir_tx_code:
 *        @arg ir_tx_code->[in] The add and key(add|~addr|key|~key) will send.
 * @retval  None.
 */
void HAL_IRTX_Transmit(IRTX_HandleTypeDef *irtx, uint32_t protos_sel, uint32_t ir_tx_code)
{
	uint32_t i = 0;
	unsigned long flags;

	IRTX_INF("%s %x\n", __func__, ir_tx_code);
	if (irtx->State != IRTX_STATE_READY) {
		IRTX_WRN("wait ir tx ready!\n");
		return ;
	}
	irtx->State = IRTX_STATE_BUSY;
	irtx->CyclicalTxCnt = 0;

	HAL_ASSERT_PARAM(irtx->Protos[protos_sel]);
	irtx->txCnt = irtx->Protos[protos_sel](irtx->txBuff, ir_tx_code);

	irtx->Instance->TICR &= ~(IRTX_IT_TAI | IRTX_IT_TPE);
	irtx->Instance->SR = IRTX_FLAG_TAI | IRTX_FLAG_TPE;

	if (irtx->SendModeType == IRTX_TTS_CYCLICAL) {
		if (irtx->txCnt > IR_TX_FIFO_SIZE) {
			IRTX_ERR("send too much bytes in cyclical mode!\n");
			return ;
		}
		flags = arch_irq_save();
		for (i = 0; i < irtx->txCnt; i++) {
			while (irtx->Instance->TACR == 0) {
				IRTX_INF("%s: fifo full\n", __func__);
			}
			irtx->Instance->DR = irtx->txBuff[i];
		}
		arch_irq_restore(flags);

		irtx->Instance->TICR |= IRTX_IT_TPE;
		irtx->Instance->TCR |= IRTX_CSS_EN;
	} else {
		uint32_t tmp_tthreshold, tmp_ethreshold;

		if (irtx->txCnt > IR_TX_FIFO_SIZE) {
			tmp_tthreshold = IR_TX_FIFO_SIZE - 4;
			tmp_ethreshold = IR_TX_FIFO_SIZE >> 1;
		} else {
			tmp_tthreshold = irtx->txCnt - 1;
			tmp_ethreshold = irtx->txCnt >> 1;
		}
		irtx->Instance->TXTR = tmp_tthreshold;
		irtx->Instance->TER &= ~0x000000FF;
		irtx->Instance->TER |= tmp_ethreshold;
		irtx->TxNum = 0;
		flags = arch_irq_save();
		while (irtx->TxNum < MIN(IR_TX_FIFO_SIZE, irtx->txCnt)) {
			while (irtx->Instance->TACR == 0) {
				IRTX_INF("%s fifo full\n", __func__);
			}
			i = 0;
			irtx->Instance->DR = irtx->txBuff[irtx->TxNum++];
		}
		arch_irq_restore(flags);
		if (irtx->TxNum < irtx->txCnt)
			irtx->Instance->TICR |= IRTX_IT_TAI;
		irtx->Instance->TICR |= IRTX_IT_TPE; /* non-cycle transmit threshold */
	}
}

#ifdef CONFIG_PM
static IRTX_InitTypeDef hal_irtx_param;
static IRTX_StateTypeDef hal_irtx_suspending = 0;

static int irtx_suspend(struct soc_device *dev, enum suspend_state_t state)
{
	hal_irtx_suspending = 1;

	switch (state) {
	case PM_MODE_SLEEP:
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
		HAL_IRTX_DeInit(dev->platform_data);
		IRTX_INF("%s okay\n", __func__);
		break;
	default:
		break;
	}

	return 0;
}

static int irtx_resume(struct soc_device *dev, enum suspend_state_t state)
{
	switch (state) {
	case PM_MODE_SLEEP:
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
		HAL_IRTX_Init(&hal_irtx_param);
		IRTX_INF("%s okay\n", __func__);
		break;
	default:
		break;
	}

	hal_irtx_suspending = 0;

	return 0;
}

static struct soc_device_driver irtx_drv = {
	.name = "irtx",
	.suspend_noirq = irtx_suspend,
	.resume_noirq = irtx_resume,
};

static struct soc_device irtx_dev = {
	.name = "irtx",
	.driver = &irtx_drv,
};

#define IRTX_DEV (&irtx_dev)
#else
#define IRTX_DEV NULL
#endif

/**
 * @brief Initializes the IRTX peripheral.
 * @param param:
 *        @arg param->[in] The configuration information.
 * @retval  IRTX handler.
 */
IRTX_HandleTypeDef *HAL_IRTX_Init(IRTX_InitTypeDef *param)
{
#if !defined (IR_CLK_32K_USED)
	uint32_t clk = HAL_GetHFClock();
#endif
	IRTX_HandleTypeDef *irtx = &hal_irtx;

	irtx->SendModeType = param->SendModeType;
	if (irtx->SendModeType == IRTX_TTS_CYCLICAL) {
		HAL_ASSERT_PARAM(param->CyclicalCnt > 0);
		irtx->CyclicalCnt = param->CyclicalCnt;
	}

	irtx->Instance = (IRTX_TypeDef *)IRTX_BASE;
	if (irtx->State == IRTX_STATE_RESET) {
		HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_IRTX, 0), 0);
	}
#if defined (IR_CLK_32K_USED)
	HAL_CCM_IRTX_SetMClock(IRTX_32K_APB_PERIPH_CLK_SRC,
	                       IRTX_32K_CCM_PERIPH_CLK_DIV_N, \
	                       IRTX_32K_CCM_PERIPH_CLK_DIV_M);
#else
	if (clk == HOSC_CLOCK_26M) {
		HAL_CCM_IRTX_SetMClock(IRTX_26M_APB_PERIPH_CLK_SRC,
		                       IRTX_26M_CCM_PERIPH_CLK_DIV_N, \
		                       IRTX_26M_CCM_PERIPH_CLK_DIV_M);
	} else if (clk == HOSC_CLOCK_24M) {
		HAL_CCM_IRTX_SetMClock(IRTX_24M_APB_PERIPH_CLK_SRC,
		                       IRTX_24M_CCM_PERIPH_CLK_DIV_N, \
		                       IRTX_24M_CCM_PERIPH_CLK_DIV_M);
	} else {
		IRTX_ERR("%s unknow clk type(%d)!\n", __func__, clk);
		return NULL;
	}
#endif

	HAL_CCM_IRTX_EnableMClock();
	HAL_CCM_BusForcePeriphReset(CCM_BUS_PERIPH_BIT_IRTX);
	HAL_CCM_BusReleasePeriphReset(CCM_BUS_PERIPH_BIT_IRTX);
	HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_IRTX);

	IRTX_SetConfig(irtx->Instance, param);

	IRTX_PROTOS_FUN_INIT(irtx);
#ifdef CONFIG_PM
	if (!hal_irtx_suspending) {
		memcpy(&hal_irtx_param, param, sizeof(IRTX_InitTypeDef));
		IRTX_DEV->platform_data = irtx;
		pm_register_ops(IRTX_DEV);
	}
#endif

	HAL_NVIC_SetIRQHandler(IRTX_IRQn, IRTX_IRQHandler);
	NVIC_EnableIRQ(IRTX_IRQn);

	irtx->State = IRTX_STATE_READY;

	return irtx;
}

/**
 * @brief DeInitializes the IRTX peripheral.
 * @param irtx:
 *        @arg irtx->IRTX handler.
 * @retval  None.
 */
void HAL_IRTX_DeInit(IRTX_HandleTypeDef *irtx)
{
	if (irtx->State == IRTX_STATE_BUSY) {
		IRTX_ERR("try deinit when not busy\n");
		return ;
	}

	if (irtx->State != IRTX_STATE_READY)
		return ;

	irtx->Instance->TGR &= ~IRTX_TXEN;

	NVIC_DisableIRQ(IRTX_IRQn);

#ifdef CONFIG_PM
	if (!hal_irtx_suspending) {
		pm_unregister_ops(IRTX_DEV);
		IRTX_DEV->platform_data = NULL;
	}
#endif

	HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_IRTX);
	HAL_CCM_BusForcePeriphReset(CCM_BUS_PERIPH_BIT_IRTX);
	HAL_CCM_IRTX_DisableMClock();

	/* DeInit the low level hardware */
	HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_IRTX, 0), 0);

	irtx->State = IRTX_STATE_RESET;
}
