/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _DIAG_H_
#define _DIAG_H_

#include "platform_autoconf.h"
#include "basic_types.h"
#include "rtl_trace.h"

u32 DiagPrintf(const char *fmt, ...);
u32 DiagPrintfD(const char *fmt, ...);
int DiagVSprintf(char *buf, const char *fmt, const int *dp);


/** @addtogroup Ameba_Platform
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
	MODULE_OS			= 0,  /**< FreeRTOS */
	MODULE_BOOT		= 1,  /**< bootloader */
	MODULE_GDMA		= 2,  /**< gdma */
	MODULE_GPIO		= 3,  /**< gpio */
	MODULE_TIMER		= 4,  /**< timer */
	MODULE_I2C			= 5,  /**< i2c */
	MODULE_I2S			= 6,  /**< i2s */
	MODULE_PWM		= 7,  /**< pwm */
	MODULE_SDIO		= 8,  /**< sdio */
	MODULE_SPI			= 9,  /**< spi */
	MODULE_FLASH		= 10, /**< flash */
	MODULE_UART		= 11, /**< uart */
	MODULE_USB_OTG	= 12, /**< usb otg */
	MODULE_IPSEC		= 13, /**< ipsec */
	MODULE_ADC		= 14, /**< adc */
	MODULE_EFUSE		= 15, /**< efuse */
	MODULE_MONIT		= 16, /**< monitor */
	MODULE_MISC		= 17, /**< misc */
	MODULE_IR			= 18,
	MODULE_QDECODE	= 19,
	MODULE_KEYSCAN	= 20,
	MODULE_SGPIO		= 21,
	MODULE_AUDIO		= 22,
	MODULE_LCD		= 23,
	MODULE_WIFIFW	= 24,
	MODULE_BT			= 25,
	MODULE_IPC			= 26,
	MODULE_KM4		= 27,

	MODULE_NUMs		= 32 /**< Module Number */
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
  * 
  * LOG_MASK(LEVEL_ERROR, debug[LEVEL_ERROR]);
  * LOG_MASK(LEVEL_WARN, debug[LEVEL_WARN]);
  * LOG_MASK(LEVEL_INFO, debug[LEVEL_INFO]);
  * LOG_MASK(LEVEL_TRACE, debug[LEVEL_TRACE]);
  * 
  * DBG_PRINTF(MODULE_BOOT, LEVEL_INFO, "MODULE_BOOT Info.\n");
  * DBG_PRINTF(MODULE_BOOT, LEVEL_ERROR, "MODULE_BOOT Error!\n");
  * @endcode
  */
#define LOG_MASK(level, config) do {\
		ConfigDebug[level] = config;\
} while (0)

#define LOG_MASK_MODULE(module, level, new_status) do {\
	if (new_status == ENABLE) { \
		ConfigDebug[level] |= BIT(module); \
	} else { \
		ConfigDebug[level] &= ~BIT(module); \
	} \
} while (0)

/**
  * @brief  DBG_PRINTF is used to print log
  */
//#define RELEASE_VERSION
#ifdef RELEASE_VERSION
#define DBG_PRINTF(MODULE, LEVEL, pFormat, ...)     do {\
		if ((LEVEL < LEVEL_NUMs) && (MODULE < MODULE_NUMs) && (ConfigDebug[LEVEL] & BIT(MODULE))) {\
		}\
	}while(0)
#else
#define DBG_PRINTF(MODULE, LEVEL, pFormat, ...)     do {\
   if ((LEVEL < LEVEL_NUMs) && (MODULE < MODULE_NUMs) && (ConfigDebug[LEVEL] & BIT(MODULE)))\
        DiagPrintf("["#MODULE"-"#LEVEL"]:"pFormat, ##__VA_ARGS__);\
}while(0)
#endif

#define DBG_ERR_MSG_ON(x)       (ConfigDebug[LEVEL_ERROR] |= BIT(x))
#define DBG_WARN_MSG_ON(x)      (ConfigDebug[LEVEL_WARN] |= BIT(x))
#define DBG_INFO_MSG_ON(x)      (ConfigDebug[LEVEL_INFO] |= BIT(x))

#define DBG_ERR_MSG_OFF(x)      (ConfigDebug[LEVEL_ERROR] &= ~BIT(x))
#define DBG_WARN_MSG_OFF(x)     (ConfigDebug[LEVEL_WARN] &= ~BIT(x))
#define DBG_INFO_MSG_OFF(x)     (ConfigDebug[LEVEL_INFO] &= ~BIT(x))
#define DRIVER_PREFIX	"RTL8721D[Driver]: "

#ifdef CONFIG_DEBUG_LOG
#define DBG_8195A(...)     do {\
    if (unlikely(ConfigDebug[LEVEL_ERROR] & BIT(MODULE_MISC))) \
        DiagPrintf("\r" __VA_ARGS__);\
}while(0)

#define MONITOR_LOG(...)     do {\
     if (unlikely(ConfigDebug[LEVEL_ERROR] & BIT(MODULE_MONIT))) \
        DiagPrintf( __VA_ARGS__);\
}while(0)

#else   // else of "#if CONFIG_DEBUG_LOG"
#define DBG_8195A(...)
#define MONITOR_LOG(...)
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

extern u32 ConfigDebugBuffer;
extern u32 ConfigDebugClose;
extern u32 ConfigDebug[];


#endif //_DIAG_H_
