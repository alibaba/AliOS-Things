/** mbed Microcontroller Library
  ******************************************************************************
  * @file    wdt_api.h
  * @author 
  * @version V1.0.0
  * @brief   This file provides following mbed WDT API
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2015, Realtek Semiconductor Corp.
  * All rights reserved.
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  ****************************************************************************** 
  */
#ifndef MBED_WATCHDOG_API_H
#define MBED_WATCHDOG_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup wdt WDT
 *  @ingroup    hal
 *  @brief      wdt functions
 *  @{
 */


///@name Ameba Common
///@{

typedef void (*wdt_irq_handler)(uint32_t id);
/**
 * @brief   Initializes the watch dog, include time setting, mode register
 * @param   timeout_ms: the watch-dog timer timeout value, in ms.
 *           default action of timeout is to reset the whole system.
 * @retval none        
 */
void watchdog_init(uint32_t timeout_ms);

/**
 * @brief  Start the watchdog counting
 * @param  None
 * @retval none       
 */
void watchdog_start(void);

/**
 * @brief  Stop the watchdog counting
 * @param  None
 * @retval none       
 */
void watchdog_stop(void);

/**
 * @brief  Refresh the watchdog counting to prevent WDT timeout
 * @param  None
 * @retval none          
 */
void watchdog_refresh(void);

/**
 * @brief   Switch the watchdog timer to interrupt mode and
 *           register a watchdog timer timeout interrupt handler.
 *           The interrupt handler will be called when the watch-dog 
 *           timer is timeout.
 * @param   handler: the callback function for WDT timeout interrupt.
 * @param   id: the parameter for the callback function
 * @retval  none           
 */
void watchdog_irq_init(wdt_irq_handler handler, uint32_t id);
///@}

/*\@}*/

#ifdef __cplusplus
}
#endif

#endif
