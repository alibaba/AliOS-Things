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

#include "board_common.h"
#include "board.h"

HAL_Status board_pinmux_cfg(HAL_BoardIoctlReq req,
                            const GPIO_PinMuxParam *pinmux, uint32_t count)
{
	switch (req) {
	case HAL_BIR_PINMUX_INIT:
		HAL_GPIO_PinMuxConfig(pinmux, count);
		break;
	case HAL_BIR_PINMUX_DEINIT:
		HAL_GPIO_PinMuxDeConfig(pinmux, count);
		break;
	default:
		return HAL_INVALID;
	}

	return HAL_OK;
}

void board_chip_clock_init(void)
{
#if (BOARD_HOSC_CLOCK == HOSC_CLOCK_24M)
	HAL_PRCM_SetHOSCType(PRCM_HOSC_TYPE_24M);
	HAL_PRCM_SetSysPLL(PRCM_SYS_PLL_PARAM_HOSC24M);
#elif (BOARD_HOSC_CLOCK == HOSC_CLOCK_26M)
	HAL_PRCM_SetHOSCType(PRCM_HOSC_TYPE_26M);
	HAL_PRCM_SetSysPLL(PRCM_SYS_PLL_PARAM_HOSC26M);
#elif (BOARD_HOSC_CLOCK == HOSC_CLOCK_40M)
	HAL_PRCM_SetHOSCType(PRCM_HOSC_TYPE_40M);
	HAL_PRCM_SetSysPLL(PRCM_SYS_PLL_PARAM_HOSC40M);
#elif (BOARD_HOSC_CLOCK == HOSC_CLOCK_52M)
	HAL_PRCM_SetHOSCType(PRCM_HOSC_TYPE_52M);
	HAL_PRCM_SetSysPLL(PRCM_SYS_PLL_PARAM_HOSC52M);
#else
	#error "Invalid HOSC value!"
#endif

#if (defined(BOARD_LOSC_EXTERNAL) && BOARD_LOSC_EXTERNAL)
	HAL_PRCM_SetLFCLKSource(PRCM_LFCLK_SRC_EXT32K);
	HAL_PRCM_DisableInter32KCalib();
#else
	HAL_PRCM_SetLFCLKSource(PRCM_LFCLK_SRC_INTER32K);
	HAL_PRCM_EnableInter32KCalib();
#endif

	HAL_PRCM_SetCPUAClk(BOARD_CPU_CLK_SRC, BOARD_CPU_CLK_FACTOR);
	HAL_PRCM_SetDevClock(BOARD_DEV_CLK_FACTOR);
	HAL_CCM_BusSetClock(BOARD_AHB2_CLK_DIV, BOARD_APB_CLK_SRC, BOARD_APB_CLK_DIV);
}
