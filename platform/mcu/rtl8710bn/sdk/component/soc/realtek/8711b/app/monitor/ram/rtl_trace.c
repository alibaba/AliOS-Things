/**
  ******************************************************************************
  * @file    rtl_trace.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions for log print and mask.
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
#include "platform_stdlib.h"

extern void rtl_libc_init(void);

const char *debug_prefix[MODULE_NUMs] = {
	"OS",	//		= 0,  /**< FreeRTOS */
	"BOOT",	//		= 1,  /**< bootloader */
	"GDMA",	//		= 2,  /**< gdma */
	"GPIO",	//		= 3,  /**< gpio */
	"TIMER",	//		= 4,  /**< timer */
	"I2C",	//		= 5,  /**< i2c */
	"I2S",	//		= 6,  /**< i2s */
	"PWM",	//		= 7,  /**< pwm */
	"SDIO",	//		= 8,  /**< sdio */
	"SPI",	//		= 9,  /**< spi */
	"FLASH",	//		= 10, /**< flash */
	"UART",	//		= 11, /**< uart */
	"USOC",	//		= 12, /**< usoc */
	"IPSEC",	//		= 13, /**< ipsec */
	"ADC",	//		= 14, /**< adc */
	"EFUSE",	//		= 15, /**< efuse */
	"MONIT",	//		= 16, /**< monitor */
	"MISC",	//		= 17, /**< misc */
};

const char *debug_level[LEVEL_NUMs] = {
	"E",	//LEVEL_ERROR = 0
	"W",	//LEVEL_WARN	= 1
	"I",	//LEVEL_INFO = 2
	"T"	//LEVEL_TRACE	= 3
};

u32 ConfigDebug[LEVEL_NUMs];


void LOG_MASK(u32 config[])
{
	int i = 0;

	for (i = 0; i < LEVEL_NUMs; i++) {
		ConfigDebug[i] = config[i];
	}
}

void LOG_MASK_MODULE(u32 module, u32 level, u32 new_status)
{
	if (new_status == ENABLE) {
		ConfigDebug[level] |= BIT(module);
	} else {
		ConfigDebug[level] &= ~BIT(module);
	}
}

void LOG_PRINTF(u32 module, u32 level, u32 line, const char*fmt, ...)
{
	static char print_buffer[256];
	u32 len = 0;
	va_list ap;

	rtl_libc_init();
	
	len = DiagSPrintf(print_buffer, "[%s-%s-%d]: ", debug_prefix[module], debug_level[level], line);

	va_start (ap, fmt);
	vsnprintf(print_buffer + len, 256 - len, fmt, ap);
	va_end (ap);
	
	DiagPrintf(print_buffer);
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
