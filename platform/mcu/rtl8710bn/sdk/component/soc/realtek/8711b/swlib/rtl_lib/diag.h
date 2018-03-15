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

#include <stddef.h> /* for size_t */
#include "rtl_trace.h"

extern u32 ConfigDebugClose;
extern u32 ConfigDebugErr;
extern u32 ConfigDebugInfo;
extern u32 ConfigDebugWarn;

extern u32 CfgSysDebugErr;
extern u32 CfgSysDebugInfo;
extern u32 CfgSysDebugWarn;

#define DBG_ERR_MSG_ON(x)       (ConfigDebugErr |= (x))
#define DBG_WARN_MSG_ON(x)      (ConfigDebugWarn |= (x))
#define DBG_INFO_MSG_ON(x)      (ConfigDebugInfo |= (x))

#define DBG_ERR_MSG_OFF(x)      (ConfigDebugErr &= ~(x))
#define DBG_WARN_MSG_OFF(x)     (ConfigDebugWarn &= ~(x))
#define DBG_INFO_MSG_OFF(x)     (ConfigDebugInfo &= ~(x))

// Define debug group
#define	_DBG_BOOT_		0x00000001
#define	_DBG_GDMA_		0x00000002
#define	_DBG_GPIO_		0x00000004
#define	_DBG_TIMER_		0x00000008
#define	_DBG_I2C_		0x00000010
#define	_DBG_I2S_		0x00000020
#define	_DBG_PWM_		0x00000200
#define	_DBG_SDIO_		0x00000400
#define	_DBG_SSI_		0x00000800
#define	_DBG_SPI_FLASH_	0x00001000
#define	_DBG_UART_		0x00004000
#define	_DBG_USB_OTG_	0x00008000
#define	_DBG_USB_CORE_	0x00010000
#define	_DBG_CRYPTO_	0x00020000
#define	_DBG_ADC_		0x00040000
#define	_DBG_USOC_		0x00080000
#define	_DBG_EFUSE_		0x00100000
#define	_DBG_MONIT_		0x00200000
#define	_DBG_MISC_		0x40000000
#define	_DBG_FAULT_		0x80000000

extern _LONG_CALL_ u32 DiagPrintf(const char *fmt, ...);
u32 DiagSPrintf(u8 *buf, const char *fmt, ...);
int prvDiagPrintf(const char *fmt, ...);
int prvDiagSPrintf(char *buf, const char *fmt, ...);

#define _DbgDump  DiagPrintf

#define DRIVER_PREFIX	"RTL8195A[Driver]: "
#define HAL_PREFIX      "RTL8195A[HAL]: "

#define SDIO_WARN_PREFIX        "[SDIO Wrn]"
#define SDIO_INFO_PREFIX        "[SDIO Inf]"
#define UART_WARN_PREFIX        "[UART Wrn]"
#define USB_INFO_PREFIX         "[USB  Inf]"
#define EFUSE_INFO_PREFIX  "[EFUSE Inf] "

//#ifdef
#define CONFIG_DEBUG_ERROR      1
#define CONFIG_DEBUG_WARN       1
#define CONFIG_DEBUG_INFO       1

#ifndef likely
#define likely(x)               (x)
#define unlikely(x)             (x)
#endif

// =============================================================

#if CONFIG_DEBUG_WARN     // if Build-In Debug Warring Message
#define DBG_SDIO_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_SDIO_)) \
        _DbgDump("\r"SDIO_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_UART_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_UART_)) \
        _DbgDump("\r"UART_WARN_PREFIX __VA_ARGS__);\
}while(0)
#else   // else of "#if CONFIG_DEBUG_WARN"
#define DBG_SDIO_WARN(...)
#define DBG_UART_WARN(...)
#endif  // end of else of "#if CONFIG_DEBUG_WARN"

// =============================================================

#if CONFIG_DEBUG_INFO     // if Build-In Debug Information Message
#define DBG_SDIO_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_SDIO_)) \
        _DbgDump("\r"SDIO_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_EFUSE_INFO(...)     do {\
    if (likely(ConfigDebugInfo & _DBG_EFUSE_)) \
        _DbgDump("\r"EFUSE_INFO_PREFIX __VA_ARGS__);\
}while(0)

#else   // else of "#if CONFIG_DEBUG_INFO"
#define DBG_SDIO_INFO(...)
#define DBG_EFUSE_INFO(...)
#endif  // end of else of "#if CONFIG_DEBUG_INFO"

#ifdef CONFIG_DEBUG_LOG
#define DBG_8195A_HAL(...)     do {\
    if (unlikely(ConfigDebugErr & (_DBG_MISC_))) \
        _DbgDump("\r"HAL_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_8195A_USOC(...)     do {\
    if (unlikely(ConfigDebugErr & _DBG_USOC_)) \
        _DbgDump("\r"USB_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_8195A(...)     do {\
    if (unlikely(ConfigDebugErr & _DBG_MISC_)) \
        _DbgDump("\r" __VA_ARGS__);\
}while(0)

#define MONITOR_LOG(...)     do {\
    if (unlikely(ConfigDebugErr & _DBG_MONIT_)) \
        _DbgDump( __VA_ARGS__);\
}while(0)

#else   // else of "#if CONFIG_DEBUG_LOG"
#define DBG_8195A_HAL(...)
#define DBG_8195A(...)
#define MONITOR_LOG(...)
#define DBG_8195A_USOC(...)

#define DBG_SDIO_WARN(...)
#define DBG_UART_WARN(...)

#define DBG_SDIO_INFO(...)
#define DBG_EFUSE_INFO(...)

#endif

#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef enum {
	DBG_CFG_ERR=0,
	DBG_CFG_WARN=1,
	DBG_CFG_INFO=2
} DBG_CFG_TYPE;

typedef struct {
	u8 cmd_name[16];
	u32	cmd_type;
} DBG_CFG_CMD;

typedef enum _CONSOLE_OP_STAGE_ {
	ROM_STAGE = 0,
	RAM_STAGE = 1
}CONSOLE_OP_STAGE;

#endif //_DIAG_H_
