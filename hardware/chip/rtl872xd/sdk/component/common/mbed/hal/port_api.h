/** mbed Microcontroller Library
  ******************************************************************************
  * @file    port_api.h
  * @author 
  * @version V1.0.0
  * @brief   This file provides following mbed GPIO PORT API
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
#ifndef MBED_PORTMAP_H
#define MBED_PORTMAP_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup port PORT
 *  @ingroup    hal
 *  @brief      port functions
 *  @{
 */

///@name Ameba Common
///@{

typedef struct port_s port_t;

/**
  * @brief  Initializes the GPIO device port, include data direction registers.
  * @param  obj: gpio port object define in application software.
  * @param  port: PortName according to pinmux spec, this parameter can be one of the following values:
  *		@arg PortA: port A, has 32 pins
  *		@arg PortB: port B, has 7 pins
  * @param  mask: One bit one gpio pin, select one or multiple pins of the specified port.
  * @param  dir: gpio port direction, this parameter can be one of the following values:
  *		@arg PIN_INPUT: port pins are input
  *		@arg PIN_OUTPUT: port pins are output
  * @retval none
  */
void port_init(port_t *obj, PortName port, int mask, PinDirection dir);

/**
  * @brief  Get GPIO port pin name
  * @param  port: PortName according to pinmux spec, this parameter can be one of the following values:
  *		@arg PortA: port number is A, has 32 pins
  *		@arg PortB: port number is B, has 7 pins
  * @param  pin_n: pin number.
  * @retval none
  * @note  pin_n must be set to a value in the 0~31 range when PortA
  * @note  pin_n must be set to a value in the 0~6 range when PortB
  */
PinName port_pin(PortName port, int pin_n);

/**
  * @brief  Configure GPIO port pins pull up/pull down.
  * @param  obj: gpio port object define in application software.
  * @param  mode: this parameter can be one of the following values:
  *		@arg PullNone: HighZ
  *		@arg OpenDrain(is OpenDrain output): no pull + OUT + GPIO[gpio_bit] = 0  
  *		@arg PullDown: pull down
  *		@arg PullUp: pull up
  * @retval none  
  */
void port_mode(port_t *obj, PinMode mode);

/**
  * @brief  Set GPIO port pins data direction.
  * @param  obj: gpio port object define in application software.
  * @param  dir: this parameter can be one of the following values:
  *		@arg PIN_INPUT: port pins are input
  *		@arg PIN_OUTPUT: port pins are output  
  * @retval none  
  */
void port_dir(port_t *obj, PinDirection dir);

/**
  * @brief  Sets value to the selected port pins.
  * @param  obj: gpio port object define in application software.
  * @param  value: One bit one gpio pin, set value to one or multiple pins of the specified port.
  * @retval none  
  * @note corresponding bit is 1, pin state set to high; corresponding bit is 0, pin state set to low
  */
void port_write(port_t *obj, int value);

/**
  * @brief  Reads the specified gpio port pins.
  * @param  obj: gpio port object define in application software.
  * @retval : state of the specified gpio port pins
  * @note corresponding bit is 1, pin state is high; corresponding bit is 0, pin state is low
  */
int port_read(port_t *obj);

///@}
/*\@}*/
#ifdef __cplusplus
}
#endif

#endif
