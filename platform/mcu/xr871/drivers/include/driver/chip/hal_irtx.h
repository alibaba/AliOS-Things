/**
  * @file  hal_irtx.h
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

#ifndef _DRIVER_CHIP_HAL_IRTX_H_
#define _DRIVER_CHIP_HAL_IRTX_H_

#include "driver/chip/hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief select 32k clock as clk source definition.
 *
 * @note IR_CLK_32K_USED can be used only external 32768 crystal enabled.
 */
//#define IR_CLK_32K_USED         1       /* keep the same define in hal_irrx.h */

/*
 * @brief IRTX use example:
 * 1 define IR_CLK_32K_USED as ir clk source if 32k can be used when suspend.
 *

#include "driver/chip/hal_irtx.h"
#include "driver/chip/ir_nec.h"

void main(const void *arg)
{
	IRTX_InitTypeDef irtx_param;
	IRTX_HandleTypeDef *irtx;

	irtx_param.PulsePolarity = IRTX_TPPI_NONE;
	irtx_param.ModulateDutyLevel = IRTX_DRMC_TIME_1;
	irtx_param.SendModeType = IRTX_TTS_NONE; //IRTX_TTS_CYCLICAL for cycle mode
	irtx_param.CyclicalCnt = 3; //send 3 times if cycle mode
#if defined (IR_CLK_32K_USED)
	irtx_param.IdleDurationCnt = IRTX_32K_NEC_IDC_VALUE;
#else
	uint32_t clk = HAL_GetHFClock();

	if (clk == HOSC_CLOCK_26M) {
		irtx_param.IdleDurationCnt = IRTX_26M_NEC_IDC_VALUE;
	} else if (clk == HOSC_CLOCK_24M) {
		irtx_param.IdleDurationCnt = IRTX_24M_NEC_IDC_VALUE;
	} else {
		printf("%s unknow clk type(%d)!\n", __func__, clk);
	}
#endif

	irtx_param.InternalModulation = IRTX_IMS_ENABLE;
	irtx = HAL_IRTX_Init(&irtx_param);

	while (1) {
		OS_Sleep(5);
		// add=0x59, key=0x16
		HAL_IRTX_Transmit(irtx, IRTX_NEC_PROTO, IR_NEC_CODE(0x59, 0x16));
	}
}
 *
 */

/**
 * @brief Add a new ir protocal.
 * @note Define a new ir protocal in IRTX_ProtocalsDef and IRTX_PROTOS_FUN_INIT.
 */
typedef enum
{
	IRTX_NEC_PROTO          = 0,
	//IRTX_ITT_PROTO          = 1,
	IRTX_PROTO_NUM          /* keep last */
} IRTX_ProtocalsDef;

typedef uint32_t (*IRTX_Proto_Code)(uint8_t *txBuff, uint32_t ir_tx_code);

#define IRTX_PROTOS_FUN_INIT(handler)                                           \
	do {                                                                    \
		(handler)->Protos[IRTX_NEC_PROTO] = IRTX_NECPacket_Code;        \
		/*(handler)->Protos[IRTX_ITT_PROTO] = IRTX_ITTPacket_Code;*/    \
	} while (0)

/*
 * @brief The high/low level modulated carrier duty ratio select.
 * @note Bit field definition of IRTX Transmit Global register of bit5~6.
 *       DRMC_TIME_1 can trasmit longer, DRMC_TIME_3 has less power.
 */
typedef enum
{
	IRTX_DRMC_TIME_1        = (0),          /* 1:1 */
	IRTX_DRMC_TIME_2        = (1<<5),       /* 1:2 */
	IRTX_DRMC_TIME_3        = (2<<5),       /* 1:3 */
	IRTX_DRMC_TIME_MASK     = (3<<5)
} IRTX_DRMC_TIME;

/*
 * @brief Bit field definition of IRTX Transmit Global register of bit2.
 * @note Transmit pulse polarity.
 */
typedef enum
{
	IRTX_TPPI_NONE          = (0),
	IRTX_TPPI_INVERT        = (1<<2),
	IRTX_TPPI_MASK          = (1<<2)
} IRTX_TPPI_Type;

/**
 * @brief IRTX internal modulation signal enable.
 * @note Select disable when connect tx/rx directly in test mode.
 */
typedef enum
{
	IRTX_IMS_DISABLE        = (0),
	IRTX_IMS_ENABLE         = (1<<7)
} IRTX_IMS_Type;

/*
 * @bried Transmit mode select.
 *
 * @note Bit field definition of IRTX Transmit Control register of bit0,
 *        transmit type: signal or cyclical mode.
 *       The NEC repeat code start with S0 is 4.5mS not 2.25mS, for hardware
 *        not support change S0.
 */
typedef enum
{
	IRTX_TTS_NONE           = (0),
	IRTX_TTS_CYCLICAL       = (1),
	IRTX_TTS_MASK           = (1)
} IRTX_TTS_Type;

/** @brief IRTX initialization parameters. */
typedef struct
{
	IRTX_DRMC_TIME          ModulateDutyLevel;      /* 1/3(less power cosumption) ~ 1/1(more tx lenght) */
	IRTX_TTS_Type           SendModeType;           /* signal or cyclical mode */
	uint32_t                CyclicalCnt;            /* count of cyclical mode */
	IRTX_TPPI_Type          PulsePolarity;          /* pulse polarity */
	IRTX_IMS_Type           InternalModulation;     /* internal modulation signal enable */
	uint32_t                IdleDurationCnt;        /* idle time for cyclical */
} IRTX_InitTypeDef;

struct IRTX_HandleDef;

typedef struct IRTX_HandleDef IRTX_HandleTypeDef;

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
extern void HAL_IRTX_Transmit(IRTX_HandleTypeDef *irtx, uint32_t protos_sel, uint32_t ir_tx_code);

/**
 * @brief Initializes the IRTX peripheral.
 * @param param:
 *        @arg param->[in] The configuration information.
 * @retval  IRTX handler.
 */
extern IRTX_HandleTypeDef *HAL_IRTX_Init(IRTX_InitTypeDef *param);

/**
 * @brief DeInitializes the IRTX peripheral.
 * @param irtx:
 *        @arg irtx->IRTX handler.
 * @retval  None.
 */
extern void HAL_IRTX_DeInit(IRTX_HandleTypeDef *irtx);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_IRTX_H_ */
