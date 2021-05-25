/**
  ******************************************************************************
  * @file    rtl8721dlp_pinmapcfg.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of pin control:
  *           - pinmux
  *           - active pad pull up & pull down
  *           - sleep pad pull up & pull down
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

/* KM0 without KM4 ON: 4.30mA */
/* KM0 CG without KM4 ON: 135uA */
/* DSLP(RTC, KEYSCAN OFF, TOUCH OFF): 2uA under power supply, but 7uA under current meter */
/* DSLP(RTC+KEYSCAN, TOUCH OFF): 3uA under power supply */
/* DSLP(RTC+KEYSCAN+TOUCH): 3.9uA under power supply (peak is 3.7mA) */
const PMAP_TypeDef pmap_func[]=
{
//  	Pin Name		Func PU/PD			Slp PU/PD			DSlp PU/PD				LowPowerPin
	{_PA_0,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PA_1,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PA_2,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PA_3,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PA_4,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PA_5,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PA_6,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PA_7,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //UART_LOG_TXD
	{_PA_8,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //UART_LOG_RXD
	{_PA_9,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PA_10,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PA_11,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PA_12,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		TRUE}, //keyscan, row0
	{_PA_13,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		TRUE}, //keyscan, row1
	{_PA_14,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		TRUE}, //keyscan, row2
	{_PA_15,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		TRUE}, //keyscan, row3
	{_PA_16,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		TRUE}, //keyscan, row4
	{_PA_17,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		TRUE}, //keyscan, col3
	{_PA_18,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		TRUE}, //keyscan,col4
	{_PA_19,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		TRUE}, //keyscan, col2
	{_PA_20,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		TRUE}, //keyscan, col7
	{_PA_21,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		TRUE}, //keyscan, row7
	{_PA_22,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PA_23,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //HS USI UART
	{_PA_24,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //HS USI UART
	{_PA_25,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		TRUE}, //keyscan, col1
	{_PA_26,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		TRUE}, //keyscan, col0
	{_PA_27,		GPIO_PuPd_UP,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //SWD_DATA or normal_mode_sel
	{_PA_28,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_DOWN,		FALSE}, //
	{_PA_29,		GPIO_PuPd_UP,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PA_30,		GPIO_PuPd_UP,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //sps_sel
	{_PA_31,		GPIO_PuPd_UP,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PB_0,		GPIO_PuPd_UP,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PB_1,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //ADC
	{_PB_2,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //ADC
	{_PB_3,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //SWD_CLK
	{_PB_4,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //Touch0
	{_PB_5,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //Touch1
	{_PB_6,		GPIO_PuPd_DOWN,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //Touch2
	{_PB_6,		GPIO_PuPd_NOPULL,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE},  //dmic power
	{_PB_7,		GPIO_PuPd_NOPULL,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //Touch3
	{_PB_8,		GPIO_PuPd_UP,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PB_9,		GPIO_PuPd_UP,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PB_10,		GPIO_PuPd_UP,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PB_11,		GPIO_PuPd_UP,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PB_12,		GPIO_PuPd_UP,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //SPI_DATA3
	{_PB_13,		GPIO_PuPd_UP,		GPIO_PuPd_DOWN,	GPIO_PuPd_KEEP,		FALSE}, //SPI_CLK
	{_PB_14,		GPIO_PuPd_UP,		GPIO_PuPd_DOWN,	GPIO_PuPd_KEEP,		FALSE}, //SPI_DATA0
	{_PB_15,		GPIO_PuPd_UP,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //SPI_DATA2
	{_PB_16,		GPIO_PuPd_UP,		GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //SPI_CS
	{_PB_17,		GPIO_PuPd_UP,		GPIO_PuPd_DOWN,	GPIO_PuPd_KEEP,		FALSE}, //SPI_DATA1
	{_PB_18,		GPIO_PuPd_DOWN,	GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PB_19,		GPIO_PuPd_DOWN,	GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PB_20,		GPIO_PuPd_DOWN,	GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PB_21,		GPIO_PuPd_DOWN,	GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	//{_PB_22,		GPIO_PuPd_DOWN,	GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //IR pin should no-pull when you use IR
	{_PB_22,		GPIO_PuPd_UP,	GPIO_PuPd_KEEP, 	GPIO_PuPd_KEEP, 	FALSE},  //flash power mos control, ext pull low already
	{_PB_23,		GPIO_PuPd_DOWN,	GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PB_24,		GPIO_PuPd_UP,	GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PB_25,		GPIO_PuPd_UP,	GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PB_26,		GPIO_PuPd_NOPULL,	GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, // vbat_en, ext pul high already
	{_PB_27,		GPIO_PuPd_UP,	GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PB_28,		GPIO_PuPd_DOWN,	GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PB_29,		GPIO_PuPd_DOWN,	GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PB_30,		GPIO_PuPd_DOWN,	GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PB_31,		GPIO_PuPd_DOWN,	GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //
	{_PNC,		GPIO_PuPd_KEEP,	GPIO_PuPd_KEEP,		GPIO_PuPd_KEEP,		FALSE}, //table end
};

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
