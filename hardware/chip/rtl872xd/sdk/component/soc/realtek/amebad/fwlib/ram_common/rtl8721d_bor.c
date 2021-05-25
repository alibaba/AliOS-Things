/**
  ******************************************************************************
  * @file    rtl8721d_bor.c
  * @author
  * @version V1.0.0
  * @date    2017-10-16
  * @brief   This file contains all the functions prototypes for the bor firmware
  *             library, including the following functionalities of bor controller:
  *           - mode set
  *           - threshold set
  *           - Debounce time set
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */


#include "ameba_soc.h"

/**
  * @brief  Set BOR high and low threshold.
  * @param  Thres_Low: BOR low threshold.
  *          This parameter can be one of the following values:
  *            @arg BOR_TH_LOW0
  *            @arg BOR_TH_LOW1
  *            @arg BOR_TH_LOW2
  *            @arg BOR_TH_LOW3
  *            @arg BOR_TH_LOW4
  *            @arg BOR_TH_LOW5
  *            @arg BOR_TH_LOW6
  *            @arg BOR_TH_LOW7
  * @param  Thres_High: BOR high threshold.
  *          This parameter can be one of the following values:
  *            @arg BOR_TH_HIGH0
  *            @arg BOR_TH_HIGH1
  *            @arg BOR_TH_HIGH2 
  *            @arg BOR_TH_HIGH3
  *            @arg BOR_TH_HIGH4 
  *            @arg BOR_TH_HIGH5
  *            @arg BOR_TH_HIGH6 
  *            @arg BOR_TH_HIGH7
  * @note   For chip to work normally, BOR_TH_LOW0 and BOR_TH_HIGH0 are not supported.
  * @retval   None
  */
VOID BOR_ThresholdSet(u32 Thres_Low, u32 Thres_High)
{
	u32 temp=0;
	assert_param(IS_BOR_TH_LOW(Thres_Low));
	assert_param(IS_BOR_TH_HIGH(Thres_High));

	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL0);
	temp &= (~((BIT_MASK_BOD_THRESHOLD_L << BIT_SHIFT_BOD_THRESHOLD_L) | \
				(BIT_MASK_BOD_THRESHOLD_H << BIT_SHIFT_BOD_THRESHOLD_H)));
	temp |= ((Thres_Low << BIT_SHIFT_BOD_THRESHOLD_L) | \
				(Thres_High << BIT_SHIFT_BOD_THRESHOLD_H));
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL0,temp);
}

/**
  * @brief  Set BOR interrupt mode debounce cycle.
  * @param  Option: BOR mode. 
  *          This parameter can be one of the following values:
  *            @arg BOR_INTR: BOR interrupt mode
  * @param  Dbnc_Value: debounce cycle, in unit of ANA4M clock cycles.
  * @retval   None
  */
VOID BOR_DbncSet(u32 Option, u32 Dbnc_Value)
{
	u32 temp=0;
	if (BOR_INTR == Option) {
		assert_param(IS_BOR_INTR_DBNC_THRES(Dbnc_Value));
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_BOD_DBNC_CTRL);
		temp &= (~BIT_LSYS_MASK_BOD_DBNC_CYC);
		temp |= (BIT_LSYS_BOD_DBNC_FEN | Dbnc_Value);
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_BOD_DBNC_CTRL, temp);
	}
}

/**
  * @brief  Clear BOR interrupt.
  * @retval   None
  */
VOID BOR_ClearINT(void)
{
	u32 temp = 0;

	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_SLP_WAKE_EVENT_STATUS1);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_SLP_WAKE_EVENT_STATUS1, temp);
}

/**
  * @brief  Enable BOR.
  * @param  Option: BOR mode.
  *          This parameter can be one of the following values:
  *            @arg BOR_RESET: BOR reset mode
  *            @arg BOR_INTR: BOR interrupt mode
  * @param  NewState: ENABLE or DISABLE.
  * @retval   None
  */
VOID BOR_ModeSet(u32 Option, u32 NewStatus)
{
	u32 temp=0;
	if (BOR_INTR == Option) {
		if (NewStatus == DISABLE) {
			temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL0);
			temp &= ~BIT_POW_BOD_INTERRUPT;
			HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL0, temp);

			temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_SLP_WAKE_EVENT_MSK1);
			temp &= ~BIT_LP_BOD_MSK;
			HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_SLP_WAKE_EVENT_MSK1, temp);
		} else {
			temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL0);
			temp |= BIT_POW_BOD_INTERRUPT;
			HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL0, temp);

			temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_SLP_WAKE_EVENT_MSK1);
			temp |= BIT_LP_BOD_MSK;
			HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_SLP_WAKE_EVENT_MSK1, temp);
		}
		
	} else {
		if (NewStatus == DISABLE) {
			temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_BOOT_REASON1);
			temp &= ~BIT_AON_BOD_RESET_EN;
			HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_BOOT_REASON1, temp);
		} else {
			temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL0);
			temp |= BIT_POW_BOD_RESET;
			HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL0,temp);

			DelayUs(200);

			temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_BOOT_REASON1);
			temp |= BIT_AON_BOD_RESET_EN;
			HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_BOOT_REASON1, temp);
		}
	}
}
