/*******************************************************************************
* Copyright (c) 2014, Realtek Semiconductor Corp.
* All rights reserved.
*
* This module is a confidential and proprietary property of RealTek and
* possession or use of this module requires written permission of RealTek.
*******************************************************************************/
#ifndef MBED_EXT_POWER_MODE_API_EXT_H
#define MBED_EXT_POWER_MODE_API_EXT_H
#if (defined(CONFIG_PLATFORM_8195BHP) && (CONFIG_PLATFORM_8195BHP == 1))  ||  (defined(CONFIG_PLATFORM_8195BLP) && (CONFIG_PLATFORM_8195BLP == 1))
///@name AmebaPro Only 
///@{
#include "device.h"

#if defined (CONFIG_PLATFORM_8195B)
/** @addtogroup power mode POWER MODE
 *  @ingroup    hal
 *  @brief      power mode functions
 *  @{
 */

#if defined (CONFIG_PLATFORM_8195BLP)
//SLP
#define     SLP_STIMER    BIT0
#define     SLP_GTIMER    BIT1
#define     SLP_GPIO      BIT2
#define     SLP_PWM       BIT3
#define     SLP_UART      BIT4
#define     SLP_HSTIMER   BIT5
#define     SLP_WLAN      BIT6
#define     SLP_I2C       BIT7
#define     SLP_ADC       BIT8
#define     SLP_COMP      BIT9
#define     SLP_SGPIO     BIT10

//DSTBY
#define     DSTBY_STIMER  BIT0
#define     DSTBY_GTIMER  BIT1
#define     DSTBY_GPIO    BIT2
#define     DSTBY_PWM     BIT3
#define     DSTBY_UART    BIT4
#define     DSTBY_HSTIMER BIT5
#define     DSTBY_WLAN    BIT6
#define     DSTBY_I2C     BIT7
#define     DSTBY_ADC     BIT8
#define     DSTBY_COMP    BIT9
#define     DSTBY_SGPIO   BIT10

//DS wake event
#define DS_STIMER   BIT0
#define DS_GPIO     BIT1
#define DS_ADP      BIT2
#define DS_RTC      BIT3
#elif defined (CONFIG_PLATFORM_8195BHP)
//SLP
#define     SLP_GTIMER    BIT1
#define     SLP_GPIO      BIT2
#define     SLP_PWM       BIT3
#define     SLP_UART      BIT4
#define     SLP_MII   	  BIT5
#define     SLP_WLAN      BIT6
#define     SLP_I2C       BIT7
#define     SLP_ADC       BIT8
#define     SLP_USB       BIT9
#define     SLP_SGPIO     BIT10
#define     SLP_SDIO      BIT11

//DSTBY
#define     DSTBY_GTIMER  BIT1
#define     DSTBY_GPIO    BIT2
#endif

///@name Ameba Common
///@{

enum {
    AON_128K = CLK_128K,    // AON CLK 128kHz
    AON_100K = CLK_100K,    // AON CLK 100kHz
};

/** 
 *  @brief The function for ls sleep mode.
 *         
 *  @param[in]  Option, To slect AON Timer,GPIO...etc
 *                - bit[4]: the UART Wake up event.
 *                - bit[3]: the PWM Wake up event.
 *                - bit[2]: the GPIO A13 as a Wake up event.
 *                - bit[1]: the GTimer Wake up event. Only active at 128kHz
 *                - bit[0]: the AON Timer Wake up event.
 *  @param[in]  SDuration, wake up after SDuration value. Uint: us
 *  @param[in]  Clock, 1: 100kHz, 0: 128kHz.
 *  @param[in]  GpioOption, GPIOA0~A13 as a wake up trigger.
 *
 *  @returns void
 */
void SleepCG (u16 Option, u32 SDuration, u8 Clock, u8 GpioOption);

#if defined (CONFIG_PLATFORM_8195BHP)

/** 
 *  @brief The function for hs power gated.
 *         
 *  @param[in]  Option, To slect HS Timer and GPIO
 *                - bit[2]: the GPIO Wake up event.
 *                - bit[1]: the GTimer Wake up event.
 *  @param[in]  SDuration, wake up after SDuration value. Uint: us
 *  @param[in]  Memsel, 1: Enable memory, 0: Disable memory.
 *  @param[in]  GpioOption, Select GPIO pin as a wake up trigger.
 *
 *  @returns void
 */
void SleepPG (u16 Option, u32 SDuration, u8 Memsel, u8 GpioOption);

#endif
#if defined (CONFIG_PLATFORM_8195BLP)

/** 
 *  @brief The function for ls standby mode.
 *         
 *  @param[in]  Option, To slect AON Timer,GPIO...etc
 *                - bit[4]: the UART Wake up event.
 *                - bit[3]: the PWM Wake up event.
 *                - bit[2]: the GPIO Wake up event.
 *                - bit[1]: the GTimer Wake up event. Not ready
 *                - bit[0]: the AON Timer Wake up event.
 *  @param[in]  SDuration, wake up after SDuration value. Uint: us
 *  @param[in]  Memsel, 1: Enable memory, 0: Disable memory.
 *  @param[in]  GpioOption, GPIOA0~A13 as a wake up trigger.
 *
 *  @returns void
 */
void Standby (u16 Option, u32 SDuration, u8 Memsel, u8 GpioOption);

/**
  * @brief The stubs functions table to exports POWER MODE HAL functions in ROM.
  */

//extern const hal_power_mode_func_stubs_t hal_power_mode_stubs;

/** 
 *  @brief The function for ls deep sleep mode.
 *         
 *  @param[in]  Option, To slect AON Timer,GPIO,ADP and RTC.
 *                - bit[2]: the ADP Wake up event.
 *                - bit[1]: the GPIO Wake up event.
 *                - bit[0]: the AON Timer Wake up event.
 *  @param[in]  SDuration, wake up after SDuration value. Uint: us
 *  @param[in]  Memsel, 1: Enable memory, 0: Disable memory.
 *  @param[in]  Clock, 1: 100kHz, 0: 128kHz.
 *
 *  @returns void
 */
void DeepSleep (u8  Option, u32 SDuration, u8 Memsel, u8 Clock);

/** 
 *  @brief The function for ls deep sleep mode.
 *         
 *  @param[in] Option The RTC wake up event.
 *                - bit[3]: Wake up per day.
 *                - bit[2]: Wake up per hour.
 *                - bit[1]: Wake up per minute.
 *                - bit[0]: Wake up per second.
 *  @param[in]  Memsel, 1: Enable memory, 0: Disable memory.
 *
 *  @returns void
 */
void DeepSleep_RTC (u8 Option, u8 Memsel);

///@}

/*\@}*/

#endif
#endif
///@}
#endif  // end of "#if defined(CONFIG_PLATFORM_8195BHP) || defined(CONFIG_PLATFORM_8195BLP)"
#endif
