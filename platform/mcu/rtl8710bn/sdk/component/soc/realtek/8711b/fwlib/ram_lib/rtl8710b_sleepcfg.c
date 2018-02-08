/**
  ******************************************************************************
  * @file    rtl8710b_sleepcfg.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of pin control:
  *           - sleep wakeup event
  *           - sleep power management
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

const PWRCFG_TypeDef sleep_pwrmgt_config[]=
{
//  	Module								Status
	{BIT_SYSON_PMOPT_SLP_XTAL_EN,		OFF},	/* XTAL: 2.2mA */
	{BIT_SYSON_PMOPT_SNZ_XTAL_EN,		OFF},	/* ADC power save use it */
	{BIT_SYSON_PMOPT_SNZ_SYSPLL_EN,	OFF},	/* ADC power save use it */
	{0xFFFFFFFF,							OFF},	/* Table end */
};

/* if X can wakeup dsleep, it can wakeup dstandby & sleep */
/* if X can wakeup dstandby, it can wakeup sleep */
const PWRCFG_TypeDef sleep_wevent_config[]=
{
//  	Module									Status
	{BIT_SYSON_WEVT_GPIO_DSTBY_MSK,		ON},	/* dstandby:	wakepin 0~3 wakeup */
	{BIT_SYSON_WEVT_A33_AND_A33GPIO_MSK,	ON},	/* dsleep:	REGU A33 Timer(1K low precision timer) & A33 wakepin wakeup*/
	{BIT_SYSON_WEVT_ADC_MSK,				ON},	/* sleep:		ADC Wakeup */
	{BIT_SYSON_WEVT_SDIO_MSK,				OFF},	/* sleep:		SDIO Wakeup */
	{BIT_SYSON_WEVT_RTC_MSK,				ON},	/* dstandby:	RTC Wakeup */
	{BIT_SYSON_WEVT_UART1_MSK,				ON},	/* sleep:		UART1 Wakeup */
	{BIT_SYSON_WEVT_UART0_MSK,				ON},	/* sleep:		UART0 Wakeup */
	{BIT_SYSON_WEVT_I2C1_MSK,				OFF},	/* sleep:		I2C1 Wakeup */
	{BIT_SYSON_WEVT_I2C0_MSK,				OFF},	/* sleep:		I2C0 Wakeup */
	{BIT_SYSON_WEVT_WLAN_MSK,				ON},	/* sleep:		WLAN Wakeup */
	{BIT_SYSON_WEVT_I2C1_ADDRMATCH_MSK,	OFF},	/* sleep:		I2C1 Slave RX address Wakeup */
	{BIT_SYSON_WEVT_I2C0_ADDRMATCH_MSK,	OFF},	/* sleep:		I2C0 Slave RX address Wakeup */
	{BIT_SYSON_WEVT_USB_MSK,				OFF},	/* sleep:		USB Wakeup */
	{BIT_SYSON_WEVT_GPIO_MSK,				ON},	/* sleep:		GPIO Wakeup */
	{BIT_SYSON_WEVT_OVER_CURRENT_MSK,		OFF},	/* sleep:		REGU OVER_CURRENT Wakeup */
	{BIT_SYSON_WEVT_SYSTIM_MSK,			ON},	/* dstandby:	250K SYS Timer(ANA Timer) Wakeup */
	
	{0xFFFFFFFF,								OFF},	/* Table end */
};

/* you should set BIT_SYSON_WEVT_GPIO_DSTBY_MSK ON if you use wakepin */
const WAKEPIN_TypeDef sleep_wakepin_config[]=
{
//  	Module			Status		Polarity
	{WAKUP_0,		OFF,			0},	/* wakeup_0: GPIOA_18 */
	{WAKUP_1,		ON,			0},	/* wakeup_1: GPIOA_5 */
	{WAKUP_2,		OFF,			0},	/* wakeup_2: GPIOA_22 */
	{WAKUP_3,		OFF,			0},	/* wakeup_3: GPIOA_23 */
	
	{0xFFFFFFFF,		OFF,			0},	/* Table end */
};

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
