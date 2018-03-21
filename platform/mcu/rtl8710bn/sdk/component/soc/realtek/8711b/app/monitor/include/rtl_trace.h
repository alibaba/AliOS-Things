/**
  ******************************************************************************
  * @file    rtl_trace.h
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


#ifndef _RTK_TRACE_H_
#define _RTK_TRACE_H_

/** @addtogroup AmebaZ_Platform
  * @{
  */

/** @defgroup Platform_Debug Debug
  * @brief Platform_Debug modules
  * @{
  */

/** @defgroup Platform_Debug_Log_Trace_Exported_Types Log Trace Types
  * @{
  */

/** @brief Log Module Definition */
typedef enum {
	MODULE_OS		= 0,  /**< FreeRTOS */
	MODULE_BOOT		= 1,  /**< bootloader */
	MODULE_GDMA	= 2,  /**< gdma */
	MODULE_GPIO		= 3,  /**< gpio */
	MODULE_TIMER	= 4,  /**< timer */
	MODULE_I2C		= 5,  /**< i2c */
	MODULE_I2S		= 6,  /**< i2s */
	MODULE_PWM		= 7,  /**< pwm */
	MODULE_SDIO		= 8,  /**< sdio */
	MODULE_SPI		= 9,  /**< spi */
	MODULE_FLASH	= 10, /**< flash */
	MODULE_UART		= 11, /**< uart */
	MODULE_USOC		= 12, /**< usoc */
	MODULE_IPSEC	= 13, /**< ipsec */
	MODULE_ADC		= 14, /**< adc */
	MODULE_EFUSE	= 15, /**< efuse */
	MODULE_MONIT	= 16, /**< monitor */
	MODULE_MISC		= 17, /**< misc */

	MODULE_NUMs           /**< Module Number */
} MODULE_DEFINE;

/** @brief Log Level Definition */
typedef enum {
	LEVEL_ERROR	= 0, /**< Error */
	LEVEL_WARN	= 1, /**< Warning */
	LEVEL_INFO		= 2, /**< Information */
	LEVEL_TRACE	= 3, /**< Trace Data */
	LEVEL_NUMs		= 4  /**< Level Number */
} LEVEL_DEFINE;
/** End of Platform_Debug_Log_Trace_Exported_Types
  * @}
  */

/** @defgroup Platform_Debug_Log_Trace_Functions_And_Macro Function & Macro
  * @{
  */

/** @cond private */
/** @brief  Debug Log mask */
extern u32 ConfigDebug[];
/** @endcond */

/**
  * @brief  Debug Log Mask Set.
  * @param  config[4] --  Print log if bit MODULE_DEFINE of config[LEVEL_DEFINE] is 1;
  * @return void.
  * @note   Here is a MODULE_BOOT module sample to demostrate the using of LOG_MASK().
  *         We want to print log under ERROR level and INFO level.
  * @code{.c}
  * u32 debug[4];
  * debug[LEVEL_ERROR] = BIT(MODULE_BOOT);
  * debug[LEVEL_WARN]  = 0x0;
  * debug[LEVEL_INFO]  = BIT(MODULE_BOOT);
  * debug[LEVEL_TRACE] = 0x0;
  * LOG_MASK(debug);
  * 
  * DBG_PRINTF(MODULE_BOOT, LEVEL_INFO, "MODULE_BOOT Info.\n");
  * DBG_PRINTF(MODULE_BOOT, LEVEL_ERROR, "MODULE_BOOT Error!\n");
  * @endcode
  */
void LOG_MASK_MODULE(u32 module, u32 level, u32 new_status);
void LOG_MASK(u32 config[]);
void LOG_PRINTF(u32 module, u32 level, u32 line, const char*fmt, ...);

/**
  * @brief  DBG_PRINTF is used to print log
  */
#define RELEASE_VERSION
#ifdef RELEASE_VERSION
#define DBG_PRINTF(MODULE, LEVEL, pFormat, ...)     do {\
		if ((LEVEL < LEVEL_NUMs) && (MODULE < MODULE_NUMs) && (ConfigDebug[LEVEL] & BIT(MODULE))) {\
			DEBUG_TRACE_DATA_SECTION static const char traceFormat[] = pFormat;\
		}\
	}while(0)
#else
#define DBG_PRINTF(MODULE, LEVEL, pFormat, ...)     do {\
		if ((LEVEL < LEVEL_NUMs) && (MODULE < MODULE_NUMs) && (ConfigDebug[LEVEL] & BIT(MODULE))) {\
			static const char traceFormat[] DEBUG_TRACE_DATA_SECTION = pFormat;\
			LOG_PRINTF(MODULE, LEVEL, __LINE__, traceFormat, ##__VA_ARGS__); \
		}\
	}while(0)
#endif

/** End of Platform_Debug_Log_Trace_Functions_And_Macro
  * @}
  */

/** End of Platform_Debug
  * @}
  */

/** End of AmebaZ_Platform
  * @}
  */

#endif //_RTK_TRACE_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
