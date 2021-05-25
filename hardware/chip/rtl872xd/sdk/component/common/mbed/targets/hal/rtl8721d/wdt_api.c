/** mbed Microcontroller Library
  ******************************************************************************
  * @file    wdt_api.c
  * @author 
  * @version V1.0.0
  * @date    2016-08-01
  * @brief   This file provides mbed API for WDG.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */
  
#include "objects.h"
#include "ameba_soc.h"
#include "wdt_api.h"
#include "cmsis.h"

/** @addtogroup AmebaD_Mbed_API 
  * @{
  */

/** @defgroup MBED_WDG
 *  @brief      MBED_WDG driver modules.
 *  @{
 */

/** @defgroup MBED_WDG_Exported_Functions MBED_WDG Exported Functions
  * @{
  */

/**
 * @brief   Initializes the watch dog, include time setting, mode register
 * @param   timeout_ms: the watch-dog timer timeout value, in ms.
 *           default action of timeout is to reset the whole system.
 * @retval none        
 */
void watchdog_init(uint32_t timeout_ms)
{
	WDG_InitTypeDef WDG_InitStruct;
	u32 CountProcess;
	u32 DivFacProcess;
	
	WDG_Scalar(timeout_ms, &CountProcess, &DivFacProcess);

	WDG_InitStruct.CountProcess = CountProcess;
	WDG_InitStruct.DivFacProcess = DivFacProcess;

	WDG_Init(&WDG_InitStruct);
}

/**
 * @brief   Start the watchdog counting
 * @param   None
 * @retval none       
 */
void watchdog_start(void)
{
	WDG_Cmd(ENABLE);
}

/**
 * @brief   Stop the watchdog counting
 * @param   None
 * @retval none       
 */
void watchdog_stop(void)
{
	WDG_Cmd(DISABLE);
}

/**
 * @brief   Refresh the watchdog counting to prevent WDT timeout
 * @param   None
 * @retval none          
 */
void watchdog_refresh(void)
{
	WDG_Refresh();
}

/**
 * @brief   Switch the watchdog timer to interrupt mode and
 *           register a watchdog timer timeout interrupt handler.
 *           The interrupt handler will be called when the watch-dog 
 *           timer is timeout.
 * @param   handler: the callback function for WDT timeout interrupt.
 * @param   id: the parameter for the callback function
 * @retval none           
 */
void watchdog_irq_init(wdt_irq_handler handler, uint32_t id)
{
	WDG_IrqInit((VOID*)handler, (u32)id);
}
/** 
  * @}
  */

/** 
  * @}
  */

/** 
  * @}
  */