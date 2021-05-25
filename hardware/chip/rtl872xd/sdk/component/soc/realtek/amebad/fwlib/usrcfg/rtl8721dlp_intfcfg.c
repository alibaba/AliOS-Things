/**
  ******************************************************************************
  * @file    rtl8721dlp_intfcfg.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities:
  *           - uart mbed function config
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

UARTCFG_TypeDef uart_config[2]=
{
	/* UART0 */
	{
		.LOW_POWER_RX_ENABLE = DISABLE, /*Enable low power RX*/
	},	
	/* UART1 */
	{
		.LOW_POWER_RX_ENABLE = DISABLE,
	},
};

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
