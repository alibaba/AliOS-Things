/** mbed Microcontroller Library
  ******************************************************************************
  * @file    i2c_ex_api.h
  * @author 
  * @version V1.0.0
  * @brief   This file provides mbed API for I2C.
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
#ifndef I2C_EX_API_H
#define I2C_EX_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup i2c_ex I2C_EX
 *  @ingroup    hal
 *  @brief      i2c extended functions
 *  @{
 */



///@name Ameba Common
///@{

typedef enum {
    I2C_TX_COMPLETE     = 0,
    I2C_RX_COMPLETE     = 1,
    I2C_RD_REQ_COMMAND  = 2,
    I2C_ERR_OCCURRED    = 3,
}I2CCallback;

/**
  * @brief  Enable/Disable i2c Device
  * @param  obj: i2c object define in application software.
  * @param  enable: this parameter can be one of the following values:
  *		@arg 0 disable
  *		@arg 1 enable
  * @retval : result  
  */
int i2c_enable_control(i2c_t *obj, int enable);

/**
  * @brief  Enable i2c master RESTART function
  * @param  obj: i2c object define in application software.
  * @retval none  
  */
void i2c_restart_enable(i2c_t *obj);

/**
  * @brief  Disable i2c Master RESTART function
  * @param  obj: i2c object define in application software.
  * @retval none  
  */
void i2c_restart_disable(i2c_t *obj);

///@}

/*\@}*/

#ifdef __cplusplus
}
#endif

#endif


