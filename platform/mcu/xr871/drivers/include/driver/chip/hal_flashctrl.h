/**
  * @file  hal_flashctrl.h
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

#ifndef __HAL_FLASHCTRL_H_
#define __HAL_FLASHCTRL_H_

#include <stdbool.h>
#include <stdlib.h>
#include "driver/chip/hal_def.h"
#include "sys/xr_debug.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	__IO uint32_t COMMON_CFG;			/* ,     Address offset: 0x000   */
	__IO uint32_t I_CMD_CFG;            /* ,     Address offset: 0x004   */
	__IO uint32_t I_DUMMY_H;            /* ,     Address offset: 0x008   */
	__IO uint32_t I_DUMMY_L;            /* ,     Address offset: 0x00C   */
	__IO uint32_t I_CS_WAIT;   			/* ,     Address offset: 0x010   */
	__IO uint32_t I_IO_WAIT;//CMD_WAIT; /* ,     Address offset: 0x014   */
	__IO uint32_t RESERVE0[2];
	__IO uint32_t FLASH_COMMON_CFG;     /* ,     Address offset: 0x020   */
	__I  uint32_t XIP_EXEC;   			/* ,     Address offset: 0x024   */

	__IO uint32_t COMMON_ADD_CFG;   	/* ,     Address offset: 0x028   */
	__IO uint32_t S_CMD_CFG;   			/* ,     Address offset: 0x02C   */
	__IO uint32_t S_ADDR_CFG;   		/* ,     Address offset: 0x030   */
	__IO uint32_t S_WR_NUM;   			/* ,     Address offset: 0x034   */
	__IO uint32_t S_RD_NUM;   			/* ,     Address offset: 0x038   */
	__IO uint32_t S_DUMMY_H;   			/* ,     Address offset: 0x03C   */
	__IO uint32_t S_DUMMY_L;   			/* ,     Address offset: 0x040   */
	__IO uint32_t FIFO_TRIG_LEVEL;   	/* ,     Address offset: 0x044   */
	__I  uint32_t FIFO_STATUS;   		/* ,     Address offset: 0x048   */
	__IO uint32_t S_IO_WAIT;   			/* ,     Address offset: 0x04C   */
	__I  uint32_t WRAP_MODE;   			/* ,     Address offset: 0x050   */
	__IO uint32_t START_SEND;   		/* ,     Address offset: 0x054   */
	__IO uint32_t INT_EN;   			/* ,     Address offset: 0x058   */
	__IO uint32_t INT_STA;   			/* ,     Address offset: 0x05C   */
	__IO uint32_t RESERVE1[40];
	__IO uint32_t S_WDATA;   			/* ,     Address offset: 0x100   */
	__IO uint32_t RESERVE2[63];
	/* (0X200 - 0X100) * 4 / 16 -1 = 63*/
	__I  uint32_t S_RDATA;   			/* ,     Address offset: 0x200   */
} FLASH_CTRL_T;



#define FLASH_CTRL ((FLASH_CTRL_T *)FLASH_CTRL_BASE)

/*
 * @brief
 */
#define FC_CC_CONT_EN_SHIFT					(20)
#define FC_CC_CONT_EN_MASK					(0x1U << FC_CC_CONT_EN_SHIFT)

#define FC_CC_IO1_SHIFT						(16)
#define FC_CC_IO1_MASK						(0x3U << FC_CC_IO1_SHIFT)

#define FC_CC_IO2_SHIFT						(12)
#define FC_CC_IO2_MASK						(0x3U << FC_CC_IO2_SHIFT)

#define FC_CC_IO3_SHIFT						(8)
#define FC_CC_IO3_MASK						(0x3U << FC_CC_IO2_SHIFT)

typedef enum {
	FC_IO_OUTPUT_0,
	FC_IO_OUTPUT_1,
	FC_IO_OUTPUT_Z
} FC_Io_Output;

#define FC_CC_PREFETCH_EN_SHIFT				(4)
#define FC_CC_PREFETCH_EN_MASK				(0x1U << FC_CC_PREFETCH_EN_SHIFT)

#define FC_CC_IBUS_EN_SHIFT					(0)
#define FC_CC_IBUS_EN_MASK					(0x1U << FC_CC_IBUS_EN_SHIFT)

#define FC_ICC_CMD_SHIFT					(24)
#define FC_ICC_CMD_MASK						(0xFFU << FC_ICC_CMD_SHIFT)

#define FC_ICC_CMD_BIT_SHIFT				(20)
#define FC_ICC_CMD_BIT_MASK					(0x3U << FC_ICC_CMD_BIT_SHIFT)

#define FC_ICC_ADDR_BIT_SHIFT				(16)
#define FC_ICC_ADDR_BIT_MASK				(0x3U << FC_ICC_ADDR_BIT_SHIFT)

#define FC_ICC_DUMMY_BIT_SHIFT				(12)
#define FC_ICC_DUMMY_BIT_MASK					(0x3U << FC_ICC_DUMMY_BIT_SHIFT)

#define FC_ICC_DUMMY_WIDTH_SHIFT				(4)
#define FC_ICC_DUMMY_WIDTH_MASK				(0x7FU << FC_ICC_DUMMY_WIDTH_SHIFT)

#define FC_ICC_DATA_BIT_SHIFT				(0)
#define FC_ICC_DATA_BIT_MASK				(0x3U << FC_ICC_DATA_BIT_SHIFT)

typedef enum {
	FC_CYCLEBITS_0,
	FC_CYCLEBITS_1,
	FC_CYCLEBITS_2,
	FC_CYCLEBITS_4
} FC_CycleBits;

#define FC_IDH_SHIFT						(0)
#define FC_IDH_MASK							(0xFFFFFFFFU << FC_IDH_SHIFT)

#define FC_IDL_SHIFT						(0)
#define FC_IDL_MASK							(0xFFFFFFFFU << FC_IDL_SHIFT)

#define FC_ICW_BEGIN_SHIFT					(16)
#define FC_ICW_BEGIN_MASK					(0xFFU << FC_ICW_BEGIN_SHIFT)

#define FC_ICW_OVER_SHIFT					(8)
#define FC_ICW_OVER_MASK					(0xFFU << FC_ICW_OVER_SHIFT)

#define FC_ICW_DESEL_SHIFT					(0)
#define FC_ICW_DESEL_MASK					(0xFFU << FC_ICW_DESEL_SHIFT)

#define FC_IIW_CMD_SHIFT					(16)
#define FC_IIW_CMD_MASK						(0xFFU << FC_IIW_CMD_SHIFT)

#define FC_IIW_ADDR_SHIFT					(8)
#define FC_IIW_ADDR_MASK					(0xFFU << FC_IIW_ADDR_SHIFT)

#define FC_IIW_DUM_SHIFT					(0)
#define FC_IIW_DUM_MASK						(0xFFU << FC_IIW_DUM_SHIFT)

#define FC_FCC_WAIT_DATA_SHIFT				(12)
#define FC_FCC_WAIT_DATA_MASK				(0x3U << FC_FCC_WAIT_DATA_SHIFT)

#define FC_FCC_CS_SHIFT						(8)
#define FC_FCC_CS_MASK						(0x1U << FC_FCC_CS_SHIFT)

typedef enum {
	FC_TCTRL_CS_LOW_ENABLE = 0 << FC_FCC_CS_SHIFT,
	FC_TCTRL_CS_HIGH_ENABLE = 1 << FC_FCC_CS_SHIFT
} FC_Cs;

#define FC_FCC_FBS_SHIFT					(4)
#define FC_FCC_FBS_MASK						(0x1U << FC_FCC_FBS_SHIFT)

typedef enum {
	FC_TCTRL_FBS_MSB = 0 << FC_FCC_FBS_SHIFT,
	FC_TCTRL_FBS_LSB = 1 << FC_FCC_FBS_SHIFT
} FC_TCTRL_Fbs;

#define FC_FCC_CPOL_SHIFT					(1)
#define FC_FCC_CPOL_MASK					(0x1U << FC_FCC_CPOL_SHIFT)

#define FC_FCC_CPHA_SHIFT					(0)
#define FC_FCC_CPHA_MASK					(0x1U << FC_FCC_CPHA_SHIFT)

typedef enum {
	FC_SCLK_Mode0 = 0 << FC_FCC_CPHA_SHIFT,
	FC_SCLK_Mode1 = 1 << FC_FCC_CPHA_SHIFT,
	FC_SCLK_Mode2 = 2 << FC_FCC_CPHA_SHIFT,
	FC_SCLK_Mode3 = 3 << FC_FCC_CPHA_SHIFT
} FC_Sclk_Mode;

#define FC_XE_SHIFT							(0)
#define FC_XE_MASK							(0x1U << FC_XE_SHIFT)


#define FC_CAC_CS_DESEL_WAIT_SHIFT			(4)
#define FC_CAC_CS_DESEL_WAIT_MASK			(0xFFU << FC_CAC_CS_DESEL_WAIT_SHIFT)

#define FC_CAC_TX_FIFO_RESET_SHIFT			(3)
#define FC_CAC_TX_FIFO_RESET_MASK			(0x1U << FC_CAC_TX_FIFO_RESET_SHIFT)

#define FC_CAC_RX_FIFO_RESET_SHIFT			(2)
#define FC_CAC_RX_FIFO_RESET_MASK			(0x1U << FC_CAC_RX_FIFO_RESET_SHIFT)

#define FC_CAC_WRAP_EN_SHIFT				(1)
#define FC_CAC_WRAP_EN_MASK					(0x1U << FC_CAC_WRAP_EN_SHIFT)

#define FC_CAC_ADDR_SIZE_MODE_SHIFT			(0)
#define FC_CAC_ADDR_SIZE_MODE_MASK			(0x1U << FC_CAC_ADDR_SIZE_MODE_SHIFT)

typedef enum {
	FC_ADDR_MODE_24BIT = 0 << FC_CAC_ADDR_SIZE_MODE_SHIFT,
	FC_ADDR_MODE_32BIT = 1 << FC_CAC_ADDR_SIZE_MODE_SHIFT,
} FC_Addr_Mode;

#define FC_SCC_CMD_SHIFT					(24)
#define FC_SCC_CMD_MASK						(0xFFU << FC_SCC_CMD_SHIFT)

#define FC_SCC_CMD_BIT_SHIFT				(20)
#define FC_SCC_CMD_BIT_MASK					(0x3U << FC_SCC_CMD_BIT_SHIFT)

#define FC_SCC_ADDR_BIT_SHIFT				(16)
#define FC_SCC_ADDR_BIT_MASK				(0x3U << FC_SCC_ADDR_BIT_SHIFT)

#define FC_SCC_DUMMY_BIT_SHIFT				(12)
#define FC_SCC_DUMMY_BIT_MASK				(0x3U << FC_SCC_DUMMY_BIT_SHIFT)

#define FC_SCC_DUMMY_DATA_BIT_SHIFT			(4)
#define FC_SCC_DUMMY_DATA_BIT_MASK			(0x3FU << FC_SCC_DUMMY_DATA_BIT_SHIFT)

#define FC_SCC_DATA_BIT_SHIFT				(0)
#define FC_SCC_DATA_BIT_MASK				(0x3U << FC_SCC_DATA_BIT_SHIFT)

#define FC_SAC_SHIFT						(0)
#define FC_SAC_MASK							(0xFFFFFFFFU << FC_SAC_SHIFT)

#define FC_SWN_SHIFT						(0)
#define FC_SWN_MASK							(0xFFU << FC_SWN_SHIFT)

#define FC_SRN_SHIFT						(0)
#define FC_SRN_MASK							(0xFFFFFFFFU << FC_SRN_SHIFT)

#define FC_SDH_SHIFT						(0)
#define FC_SDH_MASK							(0xFFFFFFFFU << FC_SDH_SHIFT)

#define FC_SDL_SHIFT						(0)
#define FC_SDL_MASK							(0xFFFFFFFFU << FC_SDL_SHIFT)

#define FC_FTL_WR_FIFO_FULL_SHIFT			(24)
#define FC_FTL_WR_FIFO_FULL_MASK			(0xFFU << FC_FTL_WR_FIFO_FULL_SHIFT)

#define FC_FTL_WR_FIFO_EMPTY_SHIFT			(16)
#define FC_FTL_WR_FIFO_EMPTY_MASK			(0xFFU << FC_FTL_WR_FIFO_EMPTY_SHIFT)

#define FC_FTL_RD_FIFO_FULL_SHIFT			(8)
#define FC_FTL_RD_FIFO_FULL_MASK			(0xFFU << FC_FTL_RD_FIFO_FULL_SHIFT)

#define FC_FTL_RD_FIFO_EMPTY_SHIFT			(0)
#define FC_FTL_RD_FIFO_EMPTY_MASK			(0xFFU << FC_FTL_RD_FIFO_EMPTY_SHIFT)

#define FC_FS_STATUS_DGB_SHIFT				(24)
#define FC_FS_STATUS_DGB_MASK				(0xFU << FC_FS_STATUS_DGB_SHIFT)
#define FC_FS_STATUS_DGB_VMASK				(0xFU)

#define FC_FS_WR_BUF_VALID_SHIFT			(23)
#define FC_FS_WR_BUF_VALID_MASK				(0x1U << FC_FS_WR_BUF_VALID_SHIFT)

#define FC_FS_WR_BUF_CNT_SHIFT				(20)
#define FC_FS_WR_BUF_CNT_MASK				(0x7U << FC_FS_WR_BUF_CNT_SHIFT)
#define FC_FS_WR_BUF_CNT_VMASK				(0x7U)

#define FC_FS_RD_BUF_VALID_SHIFT			(19)
#define FC_FS_RD_BUF_VALID_MASK				(0x1U << FC_FS_RD_BUF_VALID_SHIFT)

#define FC_FS_RD_BUF_CNT_SHIFT				(16)
#define FC_FS_RD_BUF_CNT_MASK				(0x7U << FC_FS_RD_BUF_CNT_SHIFT)
#define FC_FS_RD_BUF_CNT_VMASK				(0x7U)

#define FC_FS_WR_FIFO_CNT_SHIFT				(8)
#define FC_FS_WR_FIFO_CNT_MASK				(0xFFU << FC_FS_WR_FIFO_CNT_SHIFT)
#define FC_FS_WR_FIFO_CNT_VMASK				(0xFFU)

#define FC_FS_RD_FIFO_CNT_SHIFT				(0)
#define FC_FS_RD_FIFO_CNT_MASK				(0xFFU << FC_FS_RD_FIFO_CNT_SHIFT)
#define FC_FS_RD_FIFO_CNT_VMASK				(0xFFU)

#define FC_SIW_CMD_SHIFT					(16)
#define FC_SIW_CMD_MASK						(0xFFU << FC_SIW_CMD_SHIFT)

#define FC_SIW_ADDR_SHIFT					(8)
#define FC_SIW_ADDR_MASK					(0xFFU << FC_SIW_ADDR_SHIFT)

#define FC_SIW_DUMMY_SHIFT					(0)
#define FC_SIW_DUMMY_MASK					(0xFFU << FC_SIW_DUMMY_SHIFT)

#define FC_WM_SHIFT							(0)
#define FC_WM_MASK							(0x1U << FC_WM_SHIFT)

#define FC_SS_SHIFT							(0)
#define FC_SS_MASK							(0x1U << FC_SS_SHIFT)

#define FC_IE_CMPL_SHIFT					(12)
#define FC_IE_CMPL_MASK						(0x1U << FC_IE_CMPL_SHIFT)

#define FC_IE_WR_FIFO_UNDERFLOW_SHIFT		(11)
#define FC_IE_WR_FIFO_UNDERFLOW_MASK		(0x1U << FC_IE_WR_FIFO_UNDERFLOW_SHIFT)

#define FC_IE_WR_FIFO_OVERFLOW_SHIFT		(10)
#define FC_IE_WR_FIFO_OVERFLOW_MASK			(0x1U << FC_IE_WR_FIFO_OVERFLOW_SHIFT)

#define FC_IE_RD_FIFO_UNDERFLOW_SHIFT		(9)
#define FC_IE_RD_FIFO_UNDERFLOW_MASK		(0x1U << FC_IE_RD_FIFO_UNDERFLOW_SHIFT)

#define FC_IE_RD_FIFO_OVERFLOW_SHIFT		(8)
#define FC_IE_RD_FIFO_OVERFLOW_MASK			(0x1U << FC_IE_RD_FIFO_OVERFLOW_SHIFT)

#define FC_IE_WR_FIFO_FULL_SHIFT			(6)
#define FC_IE_WR_FIFO_FULL_MASK				(0x1U << FC_IE_WR_FIFO_FULL_SHIFT)

#define FC_IE_WR_FIFO_EMPTY_SHIFT			(5)
#define FC_IE_WR_FIFO_EMPTY_MASK			(0x1U << FC_IE_WR_FIFO_EMPTY_SHIFT)

#define FC_IE_WR_FIFO_READY_SHIFT			(4)
#define FC_IE_WR_FIFO_READY_MASK			(0x1U << FC_IE_WR_FIFO_READY_SHIFT)

#define FC_IE_RD_FIFO_FULL_SHIFT			(2)
#define FC_IE_RD_FIFO_FULL_MASK				(0x1U << FC_IE_RD_FIFO_FULL_SHIFT)

#define FC_IE_RD_FIFO_EMPTY_SHIFT			(1)
#define FC_IE_RD_FIFO_EMPTY_MASK			(0x1U << FC_IE_RD_FIFO_EMPTY_SHIFT)

#define FC_IE_RD_FIFO_READY_SHIFT			(0)
#define FC_IE_RD_FIFO_READY_MASK			(0x1U << FC_IE_RD_FIFO_READY_SHIFT)

#define FC_IS_CMPL_SHIFT					(12)
#define FC_IS_CMPL_MASK						(0x1U << FC_IS_CMPL_SHIFT)

#define FC_IS_WR_FIFO_UNDERFLOW_SHIFT		(11)
#define FC_IS_WR_FIFO_UNDERFLOW_MASK		(0x1U << FC_IS_WR_FIFO_UNDERFLOW_SHIFT)

#define FC_IS_WR_FIFO_OVERFLOW_SHIFT		(10)
#define FC_IS_WR_FIFO_OVERFLOW_MASK			(0x1U << FC_IS_WR_FIFO_OVERFLOW_SHIFT)

#define FC_IS_RD_FIFO_UNDERFLOW_SHIFT		(9)
#define FC_IS_RD_FIFO_UNDERFLOW_MASK		(0x1U << FC_IS_RD_FIFO_UNDERFLOW_SHIFT)

#define FC_IS_RD_FIFO_OVERFLOW_SHIFT		(8)
#define FC_IS_RD_FIFO_OVERFLOW_MASK			(0x1U << FC_IS_RD_FIFO_OVERFLOW_SHIFT)

#define FC_IS_WR_FIFO_FULL_SHIFT			(6)
#define FC_IS_WR_FIFO_FULL_MASK				(0x1U << FC_IS_WR_FIFO_FULL_SHIFT)

#define FC_IS_WR_FIFO_EMPTY_SHIFT			(5)
#define FC_IS_WR_FIFO_EMPTY_MASK			(0x1U << FC_IS_WR_FIFO_EMPTY_SHIFT)

#define FC_IS_WR_FIFO_READY_SHIFT			(4)
#define FC_IS_WR_FIFO_READY_MASK			(0x1U << FC_IS_WR_FIFO_READY_SHIFT)

#define FC_IS_RD_FIFO_FULL_SHIFT			(2)
#define FC_IS_RD_FIFO_FULL_MASK				(0x1U << FC_IS_RD_FIFO_FULL_SHIFT)

#define FC_IS_RD_FIFO_EMPTY_SHIFT			(1)
#define FC_IS_RD_FIFO_EMPTY_MASK			(0x1U << FC_IS_RD_FIFO_EMPTY_SHIFT)

#define FC_IS_RD_FIFO_READY_SHIFT			(0)
#define FC_IS_RD_FIFO_READY_MASK			(0x1U << FC_IS_RD_FIFO_READY_SHIFT)

#define FC_SW_SHIFT							(0)
#define FC_SW_MASK							(0xFFFFFFFFU << FC_SW_SHIFT)

#define FC_SR_SHIFT							(0)
#define FC_SR_MASK							(0xFFFFFFFFU << FC_SR_SHIFT)


typedef struct {
	uint8_t cs_begin;       /*!< cs active to valid clk edge setup minimum time */
	uint8_t cs_over;        /*!< valid clk edge to cs active hold minimum time */
	uint8_t cs_deselect;	/*!< cs minimum deselect time after read */
	uint8_t cmd_over;
	uint8_t addr_over;
	uint8_t dummy_over;
	uint8_t data;			/*!< delay n half cycle */
} Flash_Ctrl_DelayCycle;

typedef enum {
	XIP_MODE_NORMAL,
	XIP_MODE_FAST,
	XIP_MODE_DUAL_O,
	XIP_MODE_DUAL_IO,
	XIP_MODE_QUAD_O,
	XIP_MODE_QUAD_IO
} XIP_Mode;

typedef struct XIP_Instruction{
	uint8_t cmd;                /*!< command */
	FC_CycleBits cmd_line;      /*!< line mode of command */
	FC_CycleBits addr_line;     /*!< line mode of address */
	FC_CycleBits dummy_line;	/*!< line mode of dummy */
	FC_CycleBits data_line;     /*!< line mode of data */
	uint32_t dum_btyes;         /*!< dummy length */
//	uint32_t dummyh;
//	uint32_t dummyl;
} XIP_Instruction;

typedef struct {
	uint32_t addr;                  /*!< XIP code started address in flash */
	uint32_t freq;                  /*!< flash working frequency */
	Flash_Ctrl_DelayCycle delay;	/*!< board delay config */
	XIP_Instruction ins;            /*!< XIP read instruction */
	bool cont_mode;                 /*!< continue mode or not */
} XIP_Config;

typedef struct Flashc_Config
{
	uint32_t freq;          /*!< flash working frequency */
//	uint32_t t_shsl_ns;		/*!< flash t_shsl parameter. for calculate the cs delay. */
} Flashc_Config;

typedef struct Flashc_Delay
{
	uint32_t t_shsl_ns;		/*!< flash t_shsl parameter. for calculate the cs delay. */
} Flashc_Delay;

typedef struct FC_InstructionField
{
	uint8_t *pdata;     /*!< instruction field: data */
	uint32_t len;       /*!< instruction field: data len */
	FC_CycleBits line;	/*!< instruction field: line mode */
} FC_InstructionField;

typedef enum Flashc_Commands
{
	/*TODO: tbc...*/
	Flashc_Commands_NOTHING
} Flashc_Commands;

HAL_Status HAL_Flashc_Xip_Init(XIP_Config *cfg);
HAL_Status HAL_Flashc_Xip_Deinit();
void HAL_Flashc_Xip_RawEnable();
void HAL_Flashc_Xip_RawDisable();
void HAL_Flashc_Xip_Enable();
void HAL_Flashc_Xip_Disable();

HAL_Status HAL_Flashc_Init(const Flashc_Config *cfg);
HAL_Status HAL_Flashc_Deinit();
HAL_Status HAL_Flashc_Open();
HAL_Status HAL_Flashc_Close();
HAL_Status HAL_Flashc_Control(Flashc_Commands op, void *arg);
HAL_Status HAL_Flashc_Write(FC_InstructionField *cmd, FC_InstructionField *addr, FC_InstructionField *dummy, FC_InstructionField *data, bool dma);
HAL_Status HAL_Flashc_Read(FC_InstructionField *cmd, FC_InstructionField *addr, FC_InstructionField *dummy, FC_InstructionField *data, bool dma);


#ifdef __cplusplus
}
#endif


#endif
