/** mbed Microcontroller Library
  ******************************************************************************
  * @file    i2c_ex_api.h
  * @author 
  * @version V1.0.0
  * @brief   This file provides mbed API for I2C.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2006-2013 ARM Limited
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
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


