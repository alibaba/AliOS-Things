/** ****************************************************************************
 * @copyright Copyright (c) XXX
 * All rights reserved.
 *
 * @file    debug.h
 * @brief   debug
 * @version V1.0.0
 * @author  liucp
 * @date    2018-2-26
 * @warning No Warnings
 * @note <b>history:</b>
 * 1.
 *
 */
#ifndef __DEBUG_H__
#define __DEBUG_H__

/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "hw_conf.h"
#include "uart-board.h"
#ifdef __cplusplus
extern "C"{
#endif

/**
 * \defgroup MID_DEBUG MID:DEBUG
 * @{
 */


/*******************************************************************************
 * DEBUG SWITCH MACROS
 */


/*******************************************************************************
 * MACROS
 */
#define DBG_LOG_MASK            (0xFFFFFFFF)
#define DBG_LOG_DEFUALT_MASK    (DBG_LOG_EXAMPLE_MASK | DBG_LOG_KERNEL_MASK)

#define DBG_LOG_BOARD_MASK          (0x0F << 0)        // 4 bit
#define DBG_LOG_BOARD_NORMAL        (0x01 << 0)
#define DBG_LOG_BOARD_LORA          (0x01 << 1)

#define DBG_LOG_DEVICE_MASK         (0x0F << 4)        // 4 bit
#define DBG_LOG_DEVICE_NORMAL       (0x01 << 4)
#define DBG_LOG_DEVICE_LORA         (0x01 << 5)



#define DBG_LOG_EXAMPLE_MASK        (0x0F << 8)        // 4 bit
#define DBG_LOG_EXAMPLE_NORMAL      (0x01 << 8)

#define DBG_LOG_KERNEL_MASK         (0xFF << 12)        // 8 bit
#define DBG_LOG_KERNEL_NORMAL       (0x01 << 12)
#define DBG_LOG_KERNEL_LORAMAC      (0x01 << 13)

#define DBG_LOG_PLATFORM_MASK       (0x0F << 20)        // 4 bit
#define DBG_LOG_PLATFORM_NORMAL     (0x01 << 20)

#define DBG_LOG_TOOLS_MASK          (0x0F << 24)        // 4 bit
#define DBG_LOG_TOOLS_NORMAL        (0x01 << 24)

#define DBG_LOG_UTILITY_MASK        ((uint32_t)0x0F << 28)        // 4 bit
#define DBG_LOG_UTILITY_NORMAL      ((uint32_t)0x01 << 28)




#ifdef LORA_DEBUG

#define DBG_GPIO_WRITE(gpio, n, x)  HW_GPIO_Write(gpio, n, (GPIO_PinState)(x))
#define DBG_GPIO_SET(gpio, n)       gpio->BSRR = n
#define DBG_GPIO_RST(gpio, n)       gpio->BRR = n
#define DBG_RTC_OUTPUT LL_RTC_ALARMOUT_DISABLE /* LL_RTC_ALARMOUT_ALMA on PC13 */
#define DBG(x)  do { x } while (0)

#ifdef LORA_TRACE
#include "rtc-board.h"

#define DBG_PRINTF(format, ...)     do{ \
                                        vcom_Send( "[%ul][%s:%4d] " format "", \
                                        RtcGetTimerValue(), \
                                        __FILE__, \
                                        __LINE__, \
                                        ##__VA_ARGS__);\
                                    }while(0==1)

#define DBG_PRINTF_LV(lv, format, ...)      \
    do{ \
        if (0 == (dbg_get_debug_map() & lv)) \
        { \
            break; \
        } \
        vcom_Send( "[%ul][%s:%4d] " format "", \
        HW_RTC_GetTimerMs(), \
        __FUNCTION__,\
        __LINE__, \
        ##__VA_ARGS__);\
    }while(0==1)

#define DBG_PRINTF_DATA(lv, ...)    do{ \
                                        if (0 == (dbg_get_debug_map() & lv)) \
                                        { \
                                            break; \
                                        } \
                                        vcom_Send(__VA_ARGS__);\
                                    }while(0==1)

#define DBG_PRINTF_CRITICAL(...)

#else /*TRACE*/
#define DBG_PRINTF(...)

#define DBG_PRINTF_CRITICAL(...)
#endif /*TRACE*/

#else /* DEBUG */

#define DBG_GPIO_WRITE(gpio, n, x)
#define DBG_GPIO_SET(gpio, n)
#define DBG_GPIO_RST(gpio, n)
#define DBG(x) do {  } while (0)
#define DBG_PRINTF(...)
#define DBG_PRINTF_CRITICAL(...)
#define DBG_RTC_OUTPUT LL_RTC_ALARMOUT_DISABLE

#endif /* DEBUG */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * CONSTANTS
 */


/*******************************************************************************
 * GLOBAL VARIABLES DECLEAR
 */

/*******************************************************************************
 * GLOBAL FUNCTIONS DECLEAR
 */
/**
 * @brief  Initializes the debug
 * @param  None
 * @retval None
 */
void DBG_Init(void);

/**
 * @brief  Error_Handler
 * @param  None
 * @retval None
 */
void Error_Handler(void);


/**
 * \brief get debug bit map
 * \return bit mask
 */
uint32_t dbg_get_debug_map(void);

/**
 * \brief set debug bits
 * \param [in] a_bit_mask : bit mask to set
 * \return none
 */
void dbg_set_debug_bit(uint32_t a_bit_mask);

/**
 * \brief clear debug bits
 * \param [in] a_bit_mask : bit mask to clear
 * \return none
 */
void dbg_clr_debug_bit(uint32_t a_bit_mask);

/**
 * end of group APP_DEBUG
 * @}
 */

#ifdef __cplusplus
}
#endif


#endif
