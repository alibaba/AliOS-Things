/**
  ******************************************************************************
  * @file    rtl8721dhp_clk.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware rom patch functions to manage clk 
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#include "ameba_soc.h"

/**
  * @brief    Configure NCO 2M Clock
  * @param  clk_out_Hz: the NCO output clock(unit: Hz).
  * @note [14:0]: expected frequency of nco output clk, unit is 1KHz
  * @note [16]: function enable
  * @retval   none
  */
void NCO2M_Init(u32 clk_out_Hz)
{
	u32 Temp;
	u32 clk_out = clk_out_Hz / 1000;

	/* Check the parameters */
	assert_param((clk_out > 0 && clk_out <= 0x7FFF));

	/*disable the NCO clock and configure NCO output  clock*/
	Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_UART_XTAL_DIV_CTRL);

	Temp &= (~(BIT_LSYS_UART_XTAL_DIV_EN |BIT_LSYS_MASK_UART_XTAL_DIV_FREQ));
	
	Temp |= clk_out;

	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_UART_XTAL_DIV_CTRL, Temp);
}

/**
  * @brief    enable or disable the NCO2M Clock
  * @param  NewState: the new state of the NCO 2M clock.
  *              This parameter can be: ENABLE or DISABLE.
  * @retval   none
  */
void NCO2M_Cmd(u32 NewState)
{
	u32 Temp;

	Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_UART_XTAL_DIV_CTRL);	

	if(NewState != DISABLE) {
		/*enable XTAL division circuit for UART*/
		Temp |= BIT_LSYS_UART_XTAL_DIV_EN;
	} else {
		/*disable XTAL division circuit for UART*/
		Temp &= (~BIT_LSYS_UART_XTAL_DIV_EN);		
	}

	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_UART_XTAL_DIV_CTRL, Temp);	
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
