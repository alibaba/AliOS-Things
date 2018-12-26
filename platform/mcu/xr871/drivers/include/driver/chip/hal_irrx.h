/**
  * @file  hal_irrx.h
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

#ifndef _DRIVER_CHIP_HAL_IRRX_H_
#define _DRIVER_CHIP_HAL_IRRX_H_

#include "driver/chip/hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Protocal supported.
 *
 * @note Only support NEC protocal now, rewrite IRRX_XXXPacket_Handler
 *        if you want support other protocals.
 */

/**
 * @brief select 32k clock as clk source definition.
 *
 * @note IR_CLK_32K_USED can be used only external 32768 crystal enabled.
 */
//#define IR_CLK_32K_USED         1       /* keep the same define in hal_irtx.h */

/**
 * @brief IR Key Match definition.
 *
 * @note If defined IRRX_CHECK_ADDR_CODE, only addr_code(IRRX_ADDR_CODE)
 *        will be reported.
 */
//#define IRRX_CHECK_ADDR_CODE    1
#define IRRX_ADDR_CODE          (0x7f80)        /* (addr|~addr) */

/**
 * @brief IRRX use example:
 * 1. define IR_CLK_32K_USED as ir clk source if 32k can be used when suspend.
 * 2. define IRRX_CHECK_ADDR_CODE if you want to support only one ir remoter
 *     which addr is defined by IRRX_ADDR_CODE.
 *
#include "driver/chip/hal_irrx.h"

static void irrx_rxcplt_callback(uint32_t addr, uint32_t key)
{
	printf("received ir code addr:0x%02x key:0x%02x\n", addr, key);
}

void main(const void *arg)
{
	IRRX_InitTypeDef irrx_param;

	irrx_param.PulsePolariyInvert = IRRX_RPPI_INVERT;
	irrx_param.rxCpltCallback = &irrx_rxcplt_callback;
	HAL_IRRX_Init(&irrx_param);
}
 *
 */

/*
 * Bit field definition of IRRX configure register, receiver pulse polarity.
 */
typedef enum
{
	IRRX_RPPI_NONE          = 0,
	IRRX_RPPI_INVERT        = HAL_BIT(2)
} IRRX_RppiTypeDef;

struct IRRX_HandleDef;
typedef struct IRRX_HandleDef IRRX_HandleTypeDef;

/** @brief Type define of IRRX IRQ callback function. */
typedef void (*IRRX_RxCpltCallback)(uint32_t addr, uint32_t key);

/** @brief IRRX initialization parameters. */
typedef struct
{
	IRRX_RppiTypeDef        PulsePolariyInvert;
	IRRX_RxCpltCallback     rxCpltCallback;
} IRRX_InitTypeDef;

/**
 * @brief Initializes the IRRX peripheral.
 * @param param:
 *        @arg param->[in] The configuration information.
 * @retval  None.
 */
extern IRRX_HandleTypeDef *HAL_IRRX_Init(IRRX_InitTypeDef *param);

/**
 * @brief DeInitializes the IRRX peripheral.
 * @param irrx:
 *        @arg irrx->[in] IRRX handler.
 * @retval  None.
 */
extern void HAL_IRRX_DeInit(IRRX_HandleTypeDef *irrx);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_IRRX_H_ */
