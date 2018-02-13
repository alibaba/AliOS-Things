/**
  * @file  hal_flashctrl.c
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
#include "driver/chip/hal_flashctrl.h"
#include "driver/chip/hal_flashcache.h"
#include "driver/chip/hal_dma.h"

#include "pm/pm.h"

#include "sys/xr_debug.h"

#define FC_DEBUG_ON (DBG_OFF)

#define FC_DEBUG(msg, arg...) XR_DEBUG((FC_DEBUG_ON | XR_LEVEL_ALL), NOEXPAND, "[FC Debug] <%s : %d> " msg "\n", __func__, __LINE__, ##arg)

#define FC_ERROR(msg, arg...) XR_ERROR((DBG_ON | XR_LEVEL_ALL), NOEXPAND, "[FC error] <%s : %d> " msg "\n", __func__, __LINE__, ##arg)

#define FC_REG(reg) FC_DEBUG("register " #reg " (addr: 0x%x): 0x%x.", (uint32_t)&(reg), reg);

#define FC_REG_ALL() \
	{ \
		FC_DEBUG("flash controller register:"); \
		FC_REG(FLASH_CTRL->COMMON_CFG); \
		FC_REG(FLASH_CTRL->I_CMD_CFG); \
		FC_REG(FLASH_CTRL->I_DUMMY_H); \
		FC_REG(FLASH_CTRL->I_DUMMY_L); \
		FC_REG(FLASH_CTRL->I_CS_WAIT); \
		FC_REG(FLASH_CTRL->I_IO_WAIT); \
		FC_REG(FLASH_CTRL->RESERVE0[0]); \
		FC_REG(FLASH_CTRL->FLASH_COMMON_CFG); \
		FC_REG(FLASH_CTRL->XIP_EXEC); \
		FC_REG(FLASH_CTRL->COMMON_ADD_CFG); \
		FC_REG(FLASH_CTRL->S_CMD_CFG); \
		FC_REG(FLASH_CTRL->S_ADDR_CFG); \
		FC_REG(FLASH_CTRL->S_WR_NUM); \
		FC_REG(FLASH_CTRL->S_RD_NUM); \
		FC_REG(FLASH_CTRL->S_DUMMY_H); \
		FC_REG(FLASH_CTRL->S_DUMMY_L); \
		FC_REG(FLASH_CTRL->FIFO_TRIG_LEVEL); \
		FC_REG(FLASH_CTRL->FIFO_STATUS); \
		FC_REG(FLASH_CTRL->S_IO_WAIT); \
		FC_REG(FLASH_CTRL->WRAP_MODE); \
		FC_REG(FLASH_CTRL->START_SEND); \
		FC_REG(FLASH_CTRL->INT_EN); \
		FC_REG(FLASH_CTRL->INT_STA); \
	}

#if (FC_DEBUG_ON == DBG_ON)
#define FC_WHILE_TIMEOUT(cond, i)	\
	i = 0x3FFFFFF;	\
	do	\
	{	\
		if (--i == 0) {	\
			FC_REG_ALL();	\
			return HAL_ERROR;	\
		}	\
	} while(cond)
#else
#define FC_WHILE_TIMEOUT(cond, i) \
	(void)i; 	\
	while(cond)
#endif


void udelay(unsigned int us);

static HAL_Status HAL_Flashc_DMAWrite(uint8_t *data, uint32_t size);
static HAL_Status HAL_Flashc_PollWrite(uint8_t *data, uint32_t size);
static HAL_Status HAL_Flashc_DMARead(uint8_t *data, uint32_t size);
static HAL_Status HAL_Flashc_PollRead(uint8_t *data, uint32_t size);

typedef enum {
	FC_EN_CONTINUE = 1 << FC_CC_CONT_EN_SHIFT,
	FC_EN_PREFETCH = 1 << FC_CC_PREFETCH_EN_SHIFT,
	FC_EN_IBUS = 1 << FC_CC_IBUS_EN_SHIFT
} FC_En;

static inline void FC_Ibus_Enable(uint32_t flash_ctrl_en)
{
	HAL_SET_BIT(FLASH_CTRL->COMMON_CFG, flash_ctrl_en);
}

static inline void FC_Ibus_Disable(uint32_t flash_ctrl_en)
{
	HAL_CLR_BIT(FLASH_CTRL->COMMON_CFG, flash_ctrl_en);
}

static inline bool FC_Ibus_IsXIP()
{
	return HAL_GET_BIT(FLASH_CTRL->XIP_EXEC, FC_XE_MASK);
}

/*typedef enum {
	FLASH_CTRL_READMODE_NORMAL_IO = () | () | (),
	FLASH_CTRL_READMODE_DUAL_OUTPUT,
	FLASH_CTRL_READMODE_DUAL_IO,
	FLASH_CTRL_READMODE_QUAD_OUTPUT,
	FLASH_CTRL_READMODE_QUAD_IO
} Flash_Ctrl_ReadMode;*/

static inline uint32_t FC_DefOutput(uint8_t io_num, FC_Io_Output io)
{
	uint32_t mask, shift;

	if (io_num == 1) {
		mask = FC_CC_IO1_MASK;
		shift = FC_CC_IO1_SHIFT;
	} else if (io_num == 2) {
		mask = FC_CC_IO2_MASK;
		shift = FC_CC_IO2_SHIFT;
	} else if (io_num == 3) {
		mask = FC_CC_IO3_MASK;
		shift = FC_CC_IO3_SHIFT;
	} else
		return -1;

	HAL_MODIFY_REG(FLASH_CTRL->COMMON_CFG, mask, io << shift);

	return 0;
}

static inline void FC_Ibus_ReadConfig(uint8_t read_cmd,
						  	  	  	  FC_CycleBits cmd,
						  	  	  	  FC_CycleBits addr,
						  	  	  	  FC_CycleBits dummy,
						  	  	  	  FC_CycleBits data,
						  	  	  	  uint8_t dummy_byte)
{
	uint8_t dummy_width;
/*	if (dummy == FC_CYCLEBITS_4)
		dummy_width = dummy_cycle * 4;
	else if (dummy == FC_CYCLEBITS_2)
		dummy_width = dummy_cycle * 2;???????????????????
	else
		dummy_width = dummy_cycle * dummy;*/

	if (dummy_byte > 8)
		dummy_byte = 8;
	dummy_width = dummy_byte * 8;

	HAL_MODIFY_REG(FLASH_CTRL->I_CMD_CFG,
				   FC_ICC_CMD_MASK
				   | FC_ICC_CMD_BIT_MASK
				   | FC_ICC_ADDR_BIT_MASK
				   | FC_ICC_DUMMY_BIT_MASK
				   | FC_ICC_DATA_BIT_MASK
				   | FC_ICC_DUMMY_WIDTH_MASK,
				   (read_cmd << FC_ICC_CMD_SHIFT)
				   | (cmd << FC_ICC_CMD_BIT_SHIFT)
				   | (addr << FC_ICC_ADDR_BIT_SHIFT)
				   | (dummy << FC_ICC_DUMMY_BIT_SHIFT)
				   | (data << FC_ICC_DATA_BIT_SHIFT)
				   | (dummy_width << FC_ICC_DUMMY_WIDTH_SHIFT));
}

static inline void FC_Ibus_DummyData(uint32_t dummyh, uint32_t dummyl)
{
	FLASH_CTRL->I_DUMMY_H = dummyh;
	FLASH_CTRL->I_DUMMY_L = dummyl;
}

static inline void FC_Ibus_TransmitDelay(Flash_Ctrl_DelayCycle *delay)
{
	HAL_MODIFY_REG(FLASH_CTRL->I_CS_WAIT,
				   FC_ICW_BEGIN_MASK
				   | FC_ICW_OVER_MASK
				   | FC_ICW_DESEL_MASK,
				   (delay->cs_begin << FC_ICW_BEGIN_SHIFT)
				   | (delay->cs_over << FC_ICW_OVER_SHIFT)
				   | (delay->cs_deselect << FC_ICW_DESEL_SHIFT));
	HAL_MODIFY_REG(FLASH_CTRL->I_IO_WAIT,
				   FC_IIW_CMD_MASK
				   | FC_IIW_ADDR_MASK
				   | FC_IIW_DUM_MASK,
				   (delay->cmd_over << FC_IIW_CMD_SHIFT)
				   | (delay->addr_over << FC_IIW_ADDR_SHIFT)
				   | (delay->dummy_over << FC_IIW_DUM_SHIFT));
	HAL_MODIFY_REG(FLASH_CTRL->FLASH_COMMON_CFG,
				   FC_FCC_WAIT_DATA_MASK,
				   delay->data << FC_FCC_WAIT_DATA_SHIFT);
}

static inline void FC_SetFlash(FC_Cs cs, FC_TCTRL_Fbs fbs, FC_Sclk_Mode mode)
{
	HAL_MODIFY_REG(FLASH_CTRL->FLASH_COMMON_CFG,
				   FC_FCC_CS_MASK
				   | FC_FCC_FBS_MASK
				   | FC_FCC_CPOL_MASK
				   | FC_FCC_CPHA_MASK,
				   cs | fbs | mode);
}

/*static inline void FC_Ibus_SetCsDelay(uint8_t enable_cyc, uint8_t disable_cyc, uint8_t deselect_cyc)
{
	HAL_MODIFY_REG(FLASH_CTRL->I_CS_WAIT,
				   FC_ICW_BEGIN_MASK
				   | FC_ICW_DESEL_MASK
				   | FC_ICW_OVER_MASK,
				   (enable_cyc << FC_ICW_BEGIN_SHIFT)
				   | (disable_cyc << FC_ICW_OVER_SHIFT)
				   | (deselect_cyc << FC_ICW_DESEL_SHIFT));
}

static inline void FC_Sbus_SetCsDelay(uint8_t enable_cyc, uint8_t disable_cyc, uint8_t deselect_cyc)
{
	HAL_MODIFY_REG(FLASH_CTRL->I_CS_WAIT,
				   FC_ICW_BEGIN_MASK
				   | FC_ICW_OVER_MASK,
				   (enable_cyc << FC_ICW_BEGIN_SHIFT)
				   | (disable_cyc << FC_ICW_OVER_SHIFT));

	HAL_MODIFY_REG(FLASH_CTRL->COMMON_ADD_CFG,
				   FC_CAC_CS_DESEL_WAIT_MASK,
				   deselect_cyc << FC_CAC_CS_DESEL_WAIT_SHIFT);
}*/

static inline void FC_Sbus_ResetFIFO(bool tx, bool rx)
{
	HAL_MODIFY_REG(FLASH_CTRL->COMMON_ADD_CFG,
				   FC_CAC_TX_FIFO_RESET_MASK | FC_CAC_RX_FIFO_RESET_MASK,
				   (tx << FC_CAC_TX_FIFO_RESET_SHIFT) | (rx << FC_CAC_RX_FIFO_RESET_SHIFT));
}

static inline void FC_WrapMode(bool enable)
{
	HAL_MODIFY_REG(FLASH_CTRL->COMMON_ADD_CFG, FC_CAC_WRAP_EN_MASK, enable << FC_CAC_WRAP_EN_SHIFT);
}

static inline void FC_AddressMode(FC_Addr_Mode mode)
{
	HAL_MODIFY_REG(FLASH_CTRL->COMMON_ADD_CFG, FC_CAC_ADDR_SIZE_MODE_MASK, mode << FC_CAC_ADDR_SIZE_MODE_SHIFT);
}

static inline void FC_Sbus_CommandConfig(FC_CycleBits cmd,
						  	  	  	  	 FC_CycleBits addr,
						  	  	  	  	 FC_CycleBits dummy,
						  	  	  	  	 FC_CycleBits data,
						  	  	  	  	 uint8_t dummy_byte)
{
	uint8_t dummy_width;

	if (dummy_byte > 8)
		dummy_byte = 8;
	dummy_width = dummy_byte * 8;

	HAL_MODIFY_REG(FLASH_CTRL->S_CMD_CFG,
				   FC_SCC_CMD_BIT_MASK
				   | FC_SCC_ADDR_BIT_MASK
				   | FC_SCC_DUMMY_BIT_MASK
				   | FC_SCC_DATA_BIT_MASK
				   | FC_SCC_DUMMY_DATA_BIT_MASK,
				   (cmd << FC_SCC_CMD_BIT_SHIFT)
				   | (addr << FC_SCC_ADDR_BIT_SHIFT)
				   | (dummy << FC_SCC_DUMMY_BIT_SHIFT)
				   | (data << FC_SCC_DATA_BIT_SHIFT)
				   | (dummy_width << FC_SCC_DUMMY_DATA_BIT_SHIFT));
}

static inline void FC_Sbus_Command(uint8_t cmd, uint32_t addr, uint32_t dummyh, uint32_t dummyl)
{
	HAL_MODIFY_REG(FLASH_CTRL->S_CMD_CFG, FC_SCC_CMD_MASK, cmd << FC_SCC_CMD_SHIFT);
	HAL_MODIFY_REG(FLASH_CTRL->S_ADDR_CFG, FC_SAC_MASK, addr << FC_SAC_SHIFT);
	HAL_MODIFY_REG(FLASH_CTRL->S_DUMMY_H, FC_SDH_MASK, dummyh << FC_SDH_SHIFT);
	HAL_MODIFY_REG(FLASH_CTRL->S_DUMMY_L, FC_SDL_MASK, dummyl << FC_SDL_SHIFT);
}

static inline void FC_Sbus_WriteSize(uint16_t size)
{
	if (size & (~0x1FF))
		FC_DEBUG("write number error");
	size &= 0x1FF;
	HAL_MODIFY_REG(FLASH_CTRL->S_WR_NUM, FC_SWN_MASK, size << FC_SWN_SHIFT);
}

static inline void FC_Sbus_ReadSize(uint32_t size)
{
	HAL_MODIFY_REG(FLASH_CTRL->S_RD_NUM, FC_SRN_MASK, size << FC_SRN_SHIFT);
}

/*static inline void FC_Sbus_DummyData(uint32_t dummyh, uint32_t dummyl)
{
	FLASH_CTRL->S_DUMMY_H = dummyh;
	FLASH_CTRL->S_DUMMY_L = dummyl;
}*/

static inline void FC_Sbus_TransmitDelay(Flash_Ctrl_DelayCycle *delay)
{
	HAL_MODIFY_REG(FLASH_CTRL->I_CS_WAIT,
				   FC_ICW_BEGIN_MASK
				   | FC_ICW_OVER_MASK,
				   (delay->cs_begin << FC_ICW_BEGIN_SHIFT)
				   | (delay->cs_over << FC_ICW_OVER_SHIFT));

	HAL_MODIFY_REG(FLASH_CTRL->COMMON_ADD_CFG,
				   FC_CAC_CS_DESEL_WAIT_MASK,
				   delay->cs_deselect << FC_CAC_CS_DESEL_WAIT_SHIFT);

	HAL_MODIFY_REG(FLASH_CTRL->S_IO_WAIT,
				   FC_SIW_CMD_MASK
				   | FC_SIW_ADDR_MASK
				   | FC_SIW_DUMMY_MASK,
				   (delay->cmd_over << FC_SIW_CMD_SHIFT)
				   | (delay->addr_over << FC_SIW_ADDR_SHIFT)
				   | (delay->dummy_over << FC_SIW_DUMMY_SHIFT));

	HAL_MODIFY_REG(FLASH_CTRL->FLASH_COMMON_CFG,
				   FC_FCC_WAIT_DATA_MASK,
				   delay->data << FC_FCC_WAIT_DATA_SHIFT);
}

static inline void FC_Sbus_FIFOTriggerLevel(uint8_t txfull, uint8_t txempty, uint8_t rxfull, uint8_t rxempty)
{
	HAL_MODIFY_REG(FLASH_CTRL->FIFO_TRIG_LEVEL,
				   FC_FTL_RD_FIFO_EMPTY_MASK
				   | FC_FTL_RD_FIFO_FULL_MASK
				   | FC_FTL_WR_FIFO_EMPTY_MASK
				   | FC_FTL_WR_FIFO_FULL_MASK,
				   (txfull << FC_FTL_WR_FIFO_FULL_SHIFT)
				   | (txempty << FC_FTL_WR_FIFO_EMPTY_SHIFT)
				   | (rxfull << FC_FTL_RD_FIFO_FULL_SHIFT)
				   | (rxempty << FC_FTL_RD_FIFO_EMPTY_SHIFT));
}

typedef enum FC_Sbus_RW
{
	FC_SBUS_READ,
	FC_SBUS_WRITE,
} FC_Sbus_RW;

bool FC_Sbus_IsAvailable(FC_Sbus_RW rw)
{
	if (rw == FC_SBUS_WRITE)
		return !!HAL_GET_BIT(FLASH_CTRL->FIFO_STATUS, FC_FS_WR_BUF_VALID_MASK);
	else
		return !!HAL_GET_BIT(FLASH_CTRL->FIFO_STATUS, FC_FS_RD_BUF_VALID_MASK);
}

int FC_Sbus_GetBufCnt(FC_Sbus_RW rw)
{
	if (rw == FC_SBUS_WRITE)
		return HAL_GET_BIT_VAL(FLASH_CTRL->FIFO_STATUS, FC_FS_WR_BUF_CNT_SHIFT, FC_FS_WR_BUF_CNT_VMASK);
	else
		return HAL_GET_BIT_VAL(FLASH_CTRL->FIFO_STATUS, FC_FS_RD_BUF_CNT_SHIFT, FC_FS_RD_BUF_CNT_VMASK);
}

int FC_Sbus_GetFIFOCnt(FC_Sbus_RW rw)
{
	if (rw == FC_SBUS_WRITE)
		return HAL_GET_BIT_VAL(FLASH_CTRL->FIFO_STATUS, FC_FS_WR_FIFO_CNT_SHIFT, FC_FS_WR_FIFO_CNT_VMASK);
	else
		return HAL_GET_BIT_VAL(FLASH_CTRL->FIFO_STATUS, FC_FS_RD_FIFO_CNT_SHIFT, FC_FS_RD_FIFO_CNT_VMASK);
}

/*
	Debug State:
	0x2 Send CMD;
	0x4 Send Address;
	0x6 Send Dummy;
	0x8 Send Data;
	0x9 Get Data;
*/
static inline int FC_Sbus_GetDebugState()
{
	return HAL_GET_BIT_VAL(FLASH_CTRL->FIFO_STATUS, FC_FS_STATUS_DGB_SHIFT, FC_FS_STATUS_DGB_VMASK);
}

#if (FC_DEBUG_ON == DBG_ON)
#define FC_DebugCheck(state) __FC_DebugCheck(state, __LINE__)
static int __FC_DebugCheck(int state, uint32_t line)
{
	int debug = FC_Sbus_GetDebugState();
	if (debug != state) {
		HAL_UDelay(5000);
		debug = FC_Sbus_GetDebugState();
		if (debug != state) {
			FC_DEBUG("line: %d, error state: 0x%x", line, state);
			FC_REG_ALL();
			return -1;
		}
	}
	return 0;
}
#else
#define FC_DebugCheck(state) __FC_DebugCheck(state)
static inline int __FC_DebugCheck(int state)
{
	while(FC_Sbus_GetDebugState() != state);
	return 0;
}

#endif

static inline bool FC_IsWrapMode()
{
	return !!HAL_GET_BIT(FLASH_CTRL->WRAP_MODE, FC_WM_MASK);
}

static inline void FC_Sbus_StartSend()
{
	HAL_SET_BIT(FLASH_CTRL->START_SEND, FC_SS_MASK);
}

static inline bool FC_Sbus_isSending()
{
	return !!HAL_GET_BIT(FLASH_CTRL->START_SEND, FC_SS_MASK);
}

typedef enum FC_Sbus_IntType
{
	FC_INT_HREADY_TIMEOUT		= 1 << 13,
	FC_INT_TC					= 1 << FC_IE_CMPL_SHIFT,
	FC_INT_WR_FIFO_UNDERFLOW	= 1 << FC_IE_WR_FIFO_UNDERFLOW_SHIFT,
	FC_INT_WR_FIFO_OVERFLOW		= 1 << FC_IE_WR_FIFO_OVERFLOW_SHIFT,
	FC_INT_RD_FIFO_UNDERFLOW	= 1 << FC_IE_RD_FIFO_UNDERFLOW_SHIFT,
	FC_INT_RD_FIFO_OVERFLOW		= 1 << FC_IE_RD_FIFO_OVERFLOW_SHIFT,
	FC_INT_WR_FIFO_FULL			= 1 << FC_IE_WR_FIFO_FULL_SHIFT,
	FC_INT_WR_FIFO_EMPTY		= 1 << FC_IE_WR_FIFO_EMPTY_SHIFT,
	FC_INT_WR_FIFO_READY		= 1 << FC_IE_WR_FIFO_READY_SHIFT,
	FC_INT_RD_FIFO_FULL			= 1 << FC_IE_RD_FIFO_FULL_SHIFT,
	FC_INT_RD_FIFO_EMPTY		= 1 << FC_IE_RD_FIFO_EMPTY_SHIFT,
	FC_INT_RD_FIFO_READY		= 1 << FC_IE_RD_FIFO_READY_SHIFT,
} FC_Sbus_IntType;

static inline void FC_Sbus_EnableInt(FC_Sbus_IntType type)
{
	HAL_SET_BIT(FLASH_CTRL->INT_EN, type);
}

static inline void FC_Sbus_DisableInt(FC_Sbus_IntType type)
{
	HAL_CLR_BIT(FLASH_CTRL->INT_EN, type);
}

static inline int FC_Sbus_GetStatus(FC_Sbus_IntType type)
{
	return !!HAL_GET_BIT(FLASH_CTRL->INT_STA, type);
}

static inline void FC_Sbus_ClrStatus(FC_Sbus_IntType type)
{
	HAL_CLR_BIT(FLASH_CTRL->INT_STA, type);
}

static inline void FC_Sbus_Write(uint8_t data)
{
	*((uint8_t *)&FLASH_CTRL->S_WDATA) = data;
}

static inline uint8_t FC_Sbus_Read(void)
{
	return *((uint8_t *)&FLASH_CTRL->S_RDATA);
}

static inline uint8_t FC_GetDataDelay(uint32_t freq)
{
	if (freq < 48000000)
		return 0;
	else if (freq <= 64000000)
		return 1;
	else
		return 2;
}

/*
 * @brief
 */
static uint8_t ccmu_on = 0;
static void HAL_Flashc_EnableCCMU()
{
	if (ccmu_on++ != 0)
		return;
	HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_FLASHC);
	HAL_CCM_FLASHC_EnableMClock();
}

/*
 * @brief
 */
static void HAL_Flashc_DisableCCMU()
{
	if (--ccmu_on != 0)
		return;

	FC_DEBUG("DISABLE CCMU");
	HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_FLASHC);
	HAL_CCM_FLASHC_DisableMClock();
}

static inline void HAL_Flashc_ResetCCMU()
{
	CCM_BusPeriphBit ccm_flashc = CCM_BUS_PERIPH_BIT_FLASHC;	/* spi_port translate to ccm_bit */
	HAL_CCM_BusForcePeriphReset(ccm_flashc);
	HAL_CCM_BusReleasePeriphReset(ccm_flashc);
}


/*
 * @brief
 */
static bool HAL_Flashc_ConfigCCMU(uint32_t clk)
{
	CCM_AHBPeriphClkSrc src;
	uint32_t mclk;
	uint32_t div;

	if (clk > HAL_GetHFClock())
	{
		mclk = HAL_PRCM_GetDevClock();
		src = CCM_AHB_PERIPH_CLK_SRC_DEVCLK;
	}
	else
	{
		mclk = HAL_GetHFClock();
		src = CCM_AHB_PERIPH_CLK_SRC_HFCLK;
	}

	div = (mclk + clk - 1) / clk;
	div = div==0 ? 1 : div;

	if (div > (16 * 8))
		return 0;

	if (div > 64)
		HAL_CCM_FLASHC_SetMClock(src, CCM_PERIPH_CLK_DIV_N_8, (CCM_PeriphClkDivM)((div >> 3) - 1));
	else if (div > 32)
		HAL_CCM_FLASHC_SetMClock(src, CCM_PERIPH_CLK_DIV_N_4, (CCM_PeriphClkDivM)((div >> 2) - 1));
	else if (div > 16)
		HAL_CCM_FLASHC_SetMClock(src, CCM_PERIPH_CLK_DIV_N_2, (CCM_PeriphClkDivM)((div >> 1) - 1));
	else
		HAL_CCM_FLASHC_SetMClock(src, CCM_PERIPH_CLK_DIV_N_1, (CCM_PeriphClkDivM)((div >> 0) - 1));

	return 1;
}

#ifdef CONFIG_PM
static int hal_flashc_suspending = 0;
static XIP_Config pm_ibus_cfg;
static Flashc_Config pm_sbus_cfg;
static uint8_t pm_xip = 0;
static struct soc_device flashc_dev;
#endif

static uint8_t xip_on = 0;
static uint8_t pin_inited = 0;
static FC_En xip_continue = 0;
static int sbusing = 0;


void HAL_XIP_Delay(unsigned int us);

static void HAL_Flashc_PinInit()
{
//	unsigned long flags = HAL_EnterCriticalSection();
	if (pin_inited++ != 0) {
//		HAL_ExitCriticalSection(flags);
		return;
	}
	/* open io */
	HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_FLASHC, 0), 0);
//	HAL_ExitCriticalSection(flags);
}

static void HAL_Flashc_PinDeinit()
{
//	unsigned long flags = HAL_EnterCriticalSection();
	if (--pin_inited != 0) {
//		HAL_ExitCriticalSection(flags);
		return;
	}
	//close io
	HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_FLASHC, 0), 0);
//	HAL_ExitCriticalSection(flags);
}

/**
  * @brief Initialize Flash controller IBUS driver (XIP).
  * @param cfg:
  *        @arg cfg->addr: Started address of XIP code in Flash.
  *        @arg cfg->freq: Flash working frequency.
  *        @arg cfg->delay: Delay of hardware.
  *        @arg cfg->ins: Instruction of XIP reading
  *        @arg cfg->cont_mode: Enable continue mode in reading or not.
  * @retval HAL_Status: The status of driver.
  */
HAL_Status HAL_Flashc_Xip_Init(XIP_Config *cfg)
{
#ifdef CONFIG_PM
	if (!hal_flashc_suspending)
		HAL_Memcpy(&pm_ibus_cfg, cfg, sizeof(pm_ibus_cfg));
	xip_on = 1;
#endif

	/* enable ccmu */
	HAL_Flashc_EnableCCMU();

	/* open io */
	HAL_Flashc_PinInit();

	FC_Ibus_Disable(FC_EN_CONTINUE | FC_EN_IBUS | FC_EN_PREFETCH);

	/* config flash controller */
	Flash_Ctrl_DelayCycle delay = {1, 0, 3, 0, 0, 0, 1};
	delay.data = FC_GetDataDelay(cfg->freq);
	FC_Ibus_TransmitDelay(&delay);

	FC_SetFlash(FC_TCTRL_CS_LOW_ENABLE, FC_TCTRL_FBS_MSB, FC_SCLK_Mode0);

	FC_Ibus_ReadConfig(cfg->ins.cmd,
			cfg->ins.cmd_line,
			cfg->ins.addr_line,
			cfg->ins.dummy_line,
			cfg->ins.data_line,
			cfg->ins.dum_btyes);
	if (cfg->cont_mode) {
		FC_Ibus_DummyData(0x20000000, 0);
		FC_Ibus_Enable(FC_EN_CONTINUE | FC_EN_IBUS);
		xip_continue = FC_EN_CONTINUE;
	} else {
		FC_Ibus_DummyData(0, 0);
		FC_Ibus_Enable(FC_EN_IBUS);
	}

	//config flash cache
	FlashCache_Config cache_cfg = {cfg->addr};
	Hal_FlashCache_Init(&cache_cfg);

	FC_DEBUG("cfg->freq: %d; cfg->ins.cmd: %d; cfg->ins.cmd_line: %d", cfg->freq, cfg->ins.cmd, cfg->ins.cmd_line);
	FC_DEBUG("cfg->ins.addr_line: %d; cfg->ins.dummy_line: %d; cfg->ins.data_line: %d", cfg->ins.addr_line, cfg->ins.dummy_line, cfg->ins.data_line);
	FC_DEBUG("cfg->ins.dum_btyes: %d; cfg->cont_mode: %d; cfg->addr: %d", cfg->ins.dum_btyes, cfg->cont_mode, cfg->addr);

	FC_DEBUG("ccmu : %d", ccmu_on);

	FC_REG_ALL();

	return HAL_OK;
}

/**
  * @brief Deinitialize Flash controller IBUS (XIP).
  * @param None
  * @retval HAL_Status: The status of driver.
  */
HAL_Status HAL_Flashc_Xip_Deinit()
{
	unsigned long flags = HAL_EnterCriticalSection();
	//config flash controller
	FC_Ibus_Disable(FC_EN_CONTINUE | FC_EN_IBUS | FC_EN_PREFETCH);
	xip_on = 0;
	HAL_ExitCriticalSection(flags);

	//close io
	HAL_Flashc_PinDeinit();

	//deinit flash cache
	Hal_FlashCache_Deinit();

	//disable ccmu
	HAL_Flashc_DisableCCMU();

	return HAL_OK;
}

/**
  * @internal
  * @brief Flash controller IBUS (XIP) Enable without Pin initialization.
  * @note Most for Flash controller SBUS. It will resume system schedule.
  * @param None
  * @retval None
  */
void HAL_Flashc_Xip_RawEnable()
{
	if (!xip_on)
		return;

//	HAL_UDelay(100);
//	FC_Ibus_Enable(FC_EN_IBUS | xip_continue);

	FC_Ibus_Enable(xip_continue);
	OS_ThreadResumeScheduler();
}

/**
  * @internal
  * @brief Flash controller IBUS (XIP) Enable with Pin initialization.
  * @note Most for SPI. It will resume system schedule.
  * @param None
  * @retval None
  */
void HAL_Flashc_Xip_Enable()
{
	/* open io */
	HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_FLASHC, 0), 0);
	HAL_Flashc_Xip_RawEnable();
}

/**
  * @internal
  * @brief Flash controller IBUS (XIP) Enable without Pin deinitialization.
  * @note Most for Flash controller SBUS. It will suspend system schedule.
  * @param None
  * @retval None
  */
void HAL_Flashc_Xip_RawDisable()
{
	if (!xip_on)
		return;

	OS_ThreadSuspendScheduler();
//	HAL_UDelay(100);
//	FC_Ibus_Disable(FC_EN_IBUS | xip_continue);
	while((FC_Sbus_GetDebugState() != 0x0c) && (FC_Sbus_GetDebugState() != 0x00));
	FC_Ibus_Disable(xip_continue);
}

/**
  * @internal
  * @brief Flash controller IBUS (XIP) Enable with Pin deinitialization.
  * @note Most for SPI. It will suspend system schedule.
  * @param None
  * @retval None
  */
void HAL_Flashc_Xip_Disable()
{
	HAL_Flashc_Xip_RawDisable();

	//close io
	HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_FLASHC, 0), 0);
}


/**
  * @brief Delay realization in Flash controller IBUS (XIP).
  * @note Delay can be system sleep while it's not in XIP, but must be a while
  *       delay without system interface while it's in XIP.
  * @param us: delay time in microsecond.
  * @retval None
  */
void HAL_XIP_Delay(unsigned int us)
{
	if (us == 0)
		return;

	if (xip_on)
	{
		HAL_UDelay(us);
	}
	else
	{
		us += 1023;
		unsigned int ms = us >> 10;
		HAL_MSleep(ms);
	}
}

/**
  * @brief Initialize Flash controller SBUS.
  * @param cfg:
  *        @arg cfg->freq: Flash working frequency.
  * @retval HAL_Status: The status of driver.
  */
HAL_Status HAL_Flashc_Init(const Flashc_Config *cfg)
{
	/* enable ccmu */
	HAL_Flashc_ResetCCMU();
	HAL_Flashc_ConfigCCMU(cfg->freq);
	HAL_Flashc_EnableCCMU();

	/* config flash controller */
	Flash_Ctrl_DelayCycle delay = {1, 0, 3, 0, 0, 0, 1};
	/*delay.cs_deselect = cfg->t_shsl_ns * (cfg->freq / 1000000) / 1000;*/
	delay.data = FC_GetDataDelay(cfg->freq);
	FC_Sbus_TransmitDelay(&delay);

	FC_SetFlash(FC_TCTRL_CS_LOW_ENABLE, FC_TCTRL_FBS_MSB, FC_SCLK_Mode0);
	FC_Sbus_ResetFIFO(1, 1);

	FC_DEBUG("ccmu : %d", ccmu_on);
	FC_REG_ALL();

	HAL_Flashc_DisableCCMU();

#ifdef CONFIG_PM
	if (!hal_flashc_suspending)
	{
		HAL_Memcpy(&pm_sbus_cfg, cfg, sizeof(pm_sbus_cfg));
		pm_register_ops(&flashc_dev);
	}
#endif

	return HAL_OK;
}

/**
 * @brief Deinitialize Flash controller SBUS.
 * @param None
 * @retval HAL_Status: The status of driver.
 */
HAL_Status HAL_Flashc_Deinit()
{
#ifdef CONFIG_PM
	if (!hal_flashc_suspending)
		pm_unregister_ops(&flashc_dev);
#endif

	return HAL_OK;
}

/**
 * @brief Open flash controller SBUS.
 * @note At the same time, it will disable XIP and suspend schedule.
 * @param None
 * @retval HAL_Status: The status of driver.
 */
HAL_Status HAL_Flashc_Open()
{
	sbusing = 1;
	HAL_Flashc_Xip_RawDisable();

	HAL_Flashc_EnableCCMU();
	HAL_Flashc_PinInit();
	FC_Sbus_ResetFIFO(1, 1);

	return HAL_OK;
}

/**
 * @brief Close flash controller SBUS.
 * @param None
 * @retval HAL_Status: The status of driver.
 */
HAL_Status HAL_Flashc_Close()
{
	HAL_Flashc_PinDeinit();
	HAL_Flashc_DisableCCMU();

	HAL_Flashc_Xip_RawEnable();
	sbusing = 0;
	return HAL_OK;
}

/**
 * @brief Flash controller ioctl.
 * @note op : arg
 *       nothing support for now.
 * @param op: ioctl command.
 * @param arg: ioctl arguement
 * @retval HAL_Status: The status of driver.
 */
HAL_Status HAL_Flashc_Control(Flashc_Commands op, void *arg)
{
	/*TODO: tbc...*/

	return HAL_INVALID;
}

/**
 * @brief Write flash by flash controller SBUS.
 * @note Send a instruction in command + address + dummy + write data.
 * @param cmd: Command of instruction.
 *        @arg cmd->pdata: The data is filled with in this field.
 *        @arg cmd->len: The data len of this field.
 *        @arg cmd->line: The number of line transfering this field data.
 * @param addr: Address of instruction
 * @param dummy: Dummy of instruction
 * @param data: Data of instruction
 * @param dma: Transfer data by DMA or not.
 * @retval HAL_Status: The status of driver.
 */
HAL_Status HAL_Flashc_Write(FC_InstructionField *cmd, FC_InstructionField *addr, FC_InstructionField *dummy, FC_InstructionField *data, bool dma)
{
	HAL_Status ret;
	FC_InstructionField zero;
	HAL_Memset(&zero, 0, sizeof(zero));

	/* instruction check */
	if (cmd == NULL)
		cmd = &zero;
	if (addr == NULL)
		addr = &zero;
	if (dummy == NULL)
		dummy = &zero;
	if (data == NULL)
		data = &zero;

	FC_Sbus_ResetFIFO(1, 1);
	FC_Sbus_CommandConfig(cmd->line, addr->line, dummy->line, data->line, dummy->len);
	FC_Sbus_Command(*(cmd->pdata), *((uint32_t *)addr->pdata), 0, 0);
	FC_Sbus_WriteSize(data->len);

	if (dma == 1 && data->len != 0 && !xip_on)
		ret = HAL_Flashc_DMAWrite(data->pdata, data->len);
	else
		ret = HAL_Flashc_PollWrite(data->pdata, data->len);

	if (ret != HAL_OK)
		FC_ERROR("error occured on cmd: 0x%x, data len: %d", *cmd->pdata, data->len);

	return ret;
}

/**
 * @brief Read flash by flash controller SBUS.
 * @note Send a instruction in command + address + dummy + read data.
 * @param cmd: Command of instruction.
 *        @arg cmd->pdata: The data is filled with in this field.
 *        @arg cmd->len: The data len of this field.
 *        @arg cmd->line: The number of line transfering this field data.
 * @param addr: Address of instruction
 * @param dummy: Dummy of instruction
 * @param data: Data of instruction
 * @param dma: Transfer data by DMA or not.
 * @retval HAL_Status: The status of driver.
 */
HAL_Status HAL_Flashc_Read(FC_InstructionField *cmd, FC_InstructionField *addr, FC_InstructionField *dummy, FC_InstructionField *data, bool dma)
{
	HAL_Status ret;
	FC_InstructionField zero;
	HAL_Memset(&zero, 0, sizeof(zero));

	/* instruction check */
	if (cmd == NULL)
		cmd = &zero;
	if (addr == NULL)
		addr = &zero;
	if (dummy == NULL)
		dummy = &zero;
	if (data == NULL)
		data = &zero;

	FC_Sbus_ResetFIFO(1, 1);
	FC_Sbus_CommandConfig(cmd->line, addr->line, dummy->line, data->line, dummy->len);
	FC_Sbus_Command(*(cmd->pdata), *((uint32_t *)addr->pdata), 0, 0);
	FC_Sbus_ReadSize(data->len);

	if (dma == 1 && data->len != 0 && !xip_on)
		ret = HAL_Flashc_DMARead(data->pdata, data->len);
	else
		ret = HAL_Flashc_PollRead(data->pdata, data->len);

	if (ret != HAL_OK)
		FC_ERROR("error occured on cmd: 0x%x, data len: %d", *cmd->pdata, data->len);

	return ret;
}

HAL_Semaphore dmaSem;

static void HAL_Flashc_DMARelease(void *arg)
{
	HAL_SemaphoreRelease(&dmaSem);
}

static HAL_Status HAL_Flashc_DMAWrite(uint8_t *data, uint32_t size)
{
	HAL_Status ret = HAL_OK;
	DMA_ChannelInitParam dma_arg;
	DMA_Channel dma_ch;
	HAL_Memset(&dma_arg, 0, sizeof(dma_arg));
	HAL_Memset(&dma_ch, 0, sizeof(dma_ch));
	HAL_Memset(&dmaSem, 0, sizeof(dmaSem));

	if (size == 0 || data == NULL)
		return HAL_ERROR;

	if ((dma_ch = HAL_DMA_Request()) == DMA_CHANNEL_INVALID) {
		FC_ERROR("DMA request failed");
		ret = HAL_BUSY;
		goto failed;
	}

	HAL_SemaphoreInit(&dmaSem, 0, 1);

	dma_arg.irqType = DMA_IRQ_TYPE_END;
	dma_arg.endCallback = HAL_Flashc_DMARelease;
	dma_arg.endArg = NULL;
	dma_arg.cfg = HAL_DMA_MakeChannelInitCfg(DMA_WORK_MODE_SINGLE,
										   DMA_WAIT_CYCLE_2,
										   DMA_BYTE_CNT_MODE_REMAIN,
										   DMA_DATA_WIDTH_8BIT,
										   DMA_BURST_LEN_1,
										   DMA_ADDR_MODE_FIXED,
										   (DMA_Periph)(DMA_PERIPH_FLASHC),
										   DMA_DATA_WIDTH_8BIT,
										   DMA_BURST_LEN_1,
										   DMA_ADDR_MODE_INC,
										   DMA_PERIPH_SRAM);
	HAL_DMA_Init(dma_ch, &dma_arg);
	HAL_DMA_Start(dma_ch, (uint32_t)data, (uint32_t)&FLASH_CTRL->S_WDATA, size);

	FC_Sbus_StartSend();
	if ((ret = HAL_SemaphoreWait(&dmaSem, 5000)) != HAL_OK)
		FC_ERROR("sem wait failed: %d", ret);

	uint32_t i;
//	FC_WHILE_TIMEOUT(FC_Sbus_GetStatus(FC_INT_TC) == 0, i);
	FC_WHILE_TIMEOUT(FC_Sbus_isSending(), i);
	FC_Sbus_ClrStatus(FC_INT_TC);

	HAL_DMA_Stop(dma_ch);
	HAL_DMA_DeInit(dma_ch);
	HAL_DMA_Release(dma_ch);

	HAL_SemaphoreDeinit(&dmaSem);

	if (FC_DebugCheck(0))
		return HAL_ERROR;

failed:
	return ret;
}

static HAL_Status HAL_Flashc_PollWrite(uint8_t *data, uint32_t size)
{
	uint32_t wsize = size;
	uint32_t i;

	FC_Sbus_StartSend();

	while (wsize--) {
		FC_WHILE_TIMEOUT(FC_Sbus_GetFIFOCnt(FC_SBUS_WRITE) > 100, i);
		FC_Sbus_Write(*(data++));
	}

//	FC_WHILE_TIMEOUT(FC_Sbus_GetDebugState() != 0, i);
//	FC_WHILE_TIMEOUT(FC_Sbus_GetStatus(FC_INT_TC) == 0, i);
	FC_WHILE_TIMEOUT(FC_Sbus_isSending(), i);
	FC_Sbus_ClrStatus(FC_INT_TC);

	if (FC_DebugCheck(0))
		return HAL_ERROR;

	return HAL_OK;
}

static HAL_Status HAL_Flashc_DMARead(uint8_t *data, uint32_t size)
{
	HAL_Status ret = HAL_OK;
	DMA_ChannelInitParam dma_arg;
	DMA_Channel dma_ch;
	HAL_Memset(&dma_arg, 0, sizeof(dma_arg));
	HAL_Memset(&dma_ch, 0, sizeof(dma_ch));
	HAL_Memset(&dmaSem, 0, sizeof(dmaSem));

	if (size == 0 || data == NULL)
		return HAL_ERROR;

	if ((dma_ch = HAL_DMA_Request()) == DMA_CHANNEL_INVALID) {
		FC_ERROR("DMA request failed");
		ret = HAL_BUSY;
		goto failed;
	}

	HAL_SemaphoreInit(&dmaSem, 0, 1);

	dma_arg.irqType = DMA_IRQ_TYPE_END;
	dma_arg.endCallback = HAL_Flashc_DMARelease;
	dma_arg.endArg = NULL;
	dma_arg.cfg = HAL_DMA_MakeChannelInitCfg(DMA_WORK_MODE_SINGLE,
										    DMA_WAIT_CYCLE_2,
										    DMA_BYTE_CNT_MODE_REMAIN,
										    DMA_DATA_WIDTH_8BIT,
										    DMA_BURST_LEN_1,
										    DMA_ADDR_MODE_INC,
										    DMA_PERIPH_SRAM,
										    DMA_DATA_WIDTH_8BIT,
										    DMA_BURST_LEN_1,
										    DMA_ADDR_MODE_FIXED,
										    (DMA_Periph)(DMA_PERIPH_FLASHC));
	HAL_DMA_Init(dma_ch, &dma_arg);
	HAL_DMA_Start(dma_ch, (uint32_t)&FLASH_CTRL->S_RDATA, (uint32_t)data, size);

	FC_Sbus_StartSend();
	if ((ret = HAL_SemaphoreWait(&dmaSem, 5000)) != HAL_OK)
		FC_ERROR("sem wait failed: %d", ret);

	uint32_t i;
//	FC_WHILE_TIMEOUT(FC_Sbus_GetStatus(FC_INT_TC) == 0, i);
	FC_WHILE_TIMEOUT(FC_Sbus_isSending(), i);
	FC_Sbus_ClrStatus(FC_INT_TC);

	HAL_DMA_Stop(dma_ch);
	HAL_DMA_DeInit(dma_ch);
	HAL_DMA_Release(dma_ch);

	HAL_SemaphoreDeinit(&dmaSem);

	if (FC_DebugCheck(0))
		return HAL_ERROR;

failed:
	return ret;
}

static HAL_Status HAL_Flashc_PollRead(uint8_t *data, uint32_t size)
{
	uint32_t rsize = size;
	uint32_t i;

	FC_Sbus_StartSend();

	while (rsize--)
	{
		FC_WHILE_TIMEOUT(FC_Sbus_GetFIFOCnt(FC_SBUS_READ) == 0, i);
		*(data++) = FC_Sbus_Read();
	}

//	FC_WHILE_TIMEOUT(FC_Sbus_GetDebugState() != 0, i);
//	FC_WHILE_TIMEOUT(FC_Sbus_GetStatus(FC_INT_TC) == 0, i);
	FC_WHILE_TIMEOUT(FC_Sbus_isSending(), i);
	FC_Sbus_ClrStatus(FC_INT_TC);

	if (FC_DebugCheck(0))
		return HAL_ERROR;

	return HAL_OK;
}


#ifdef CONFIG_PM
static int flashc_suspend(struct soc_device *dev, enum suspend_state_t state)
{
	/*
		suspend condition:
			(1) not in sbus opened state
			(2)
	*/
	hal_flashc_suspending = 1;

	if (sbusing)
		return -1;

	while((FC_Sbus_GetDebugState() != 0x0c) && (FC_Sbus_GetDebugState() != 0x00));

	switch (state) {
	case PM_MODE_SLEEP:
		FC_Ibus_Disable(xip_continue);
		HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_FLASHC);
		HAL_CCM_FLASHC_DisableMClock();
		break;
	case PM_MODE_STANDBY:
		if (xip_on)
		{
			HAL_Flashc_Xip_Deinit();
			FC_DEBUG("ccmu : %d", ccmu_on);
			pm_xip = 1;
		}
		HAL_Flashc_Deinit();
		FC_DEBUG("ccmu : %d", ccmu_on);
		break;
	case PM_MODE_HIBERNATION:
	case PM_MODE_POWEROFF:

		break;
	default:
		break;
	}

	FC_REG_ALL();
	return 0;
}

static int flashc_resume(struct soc_device *dev, enum suspend_state_t state)
{

	FC_REG_ALL();

	switch (state) {
	case PM_MODE_SLEEP:
		FC_Ibus_Enable(xip_continue);
		HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_FLASHC);
		HAL_CCM_FLASHC_EnableMClock();
		break;
	case PM_MODE_STANDBY:
		HAL_Flashc_Init(&pm_sbus_cfg);
		if (pm_xip)
		{
			pm_xip = 0;
			HAL_Flashc_Xip_Init(&pm_ibus_cfg);
			HAL_UDelay(300);
		}
		FC_DEBUG("ccmu: %d, pin: %d", ccmu_on, pin_inited);
		break;

	case PM_MODE_HIBERNATION:
	case PM_MODE_POWEROFF:

		break;
	default:
		break;
	}

	hal_flashc_suspending = 0;

	return 0;
}

static struct soc_device_driver flashc_drv = {
	.name = "flashc",
	.suspend_noirq = flashc_suspend,
	.resume_noirq = flashc_resume,
};

static struct soc_device flashc_dev = {
	.name = "flashc",
	.driver = &flashc_drv,
};
#endif

