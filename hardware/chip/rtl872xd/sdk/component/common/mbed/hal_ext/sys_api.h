/** mbed Microcontroller Library
  ******************************************************************************
  * @file    sys_api.h
  * @author 
  * @version V1.0.0
  * @brief   This file provides following mbed system API:
  *				-JTAG OFF
  *				-LOGUART ON/OFF
  *				-OTA image switch
  *				-System Reset
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
#ifndef MBED_SYS_API_H
#define MBED_SYS_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup sys SYSTEM
 *  @ingroup    hal
 *  @brief      system functions
 *  @{
 */

///@name Ameba Common
///@{

/**
  * @brief  Turn off the JTAG function
  * @retval none  
  */
void sys_jtag_off(void);

/**
  * @brief  switch OTA image if the othe OTA image is valid
  * @retval none
  * @note for AmebaZ, sys_clear_ota_signature is the same with sys_recover_ota_signature
  */
void sys_clear_ota_signature(void);

/**
  * @brief  switch OTA image if the othe OTA image is valid
  * @retval none
  * @note for AmebaZ, sys_clear_ota_signature is the same with sys_recover_ota_signature
  */
void sys_recover_ota_signature(void);

/**
  * @brief  open log uart
  * @retval none
  */
void sys_log_uart_on(void);

/**
  * @brief  close log uart
  * @retval none
  */
void sys_log_uart_off(void);

/**
  * @brief  store or load adc calibration parameter 
  * @param  write:  this parameter can be one of the following values:
  *		@arg 0: load adc calibration parameter offset & gain from flash system data region
  *		@arg 1: store adc calibration parameter offset & gain to flash system data region
  * @param  offset: pointer to adc parameter offset
  * @param  gain: pointer to adc parameter gain
  * @retval none
  */
void sys_adc_calibration(u8 write, u16 *offset, u16 *gain);

/**
  * @brief  system software reset
  * @retval none
  */
void sys_reset(void);

///@}

#if defined(CONFIG_PLATFORM_8195A) && (CONFIG_PLATFORM_8195A == 1)
///@name Ameba1 Only 
///@{
/**
  * @brief check whether is sdram power on
  * @retval 1: power on
  *         0: power off
  */
u8   sys_is_sdram_power_on(void);

/**
  * @brief sdram power off
  * @retval none
  */
void sys_sdram_off(void);
///@}
#endif //CONFIG_PLATFORM_8195A

#if defined(CONFIG_PLATFORM_8711B) && (CONFIG_PLATFORM_8711B == 1)
///@name AmebaZ Only 
///@{
/**
  * @brief vector reset 
  * @retval none
  */
void sys_cpu_reset(void);
///@}
#endif //CONFIG_PLATFORM_8711B

/*\@}*/

#ifdef __cplusplus
}
#endif

#endif
