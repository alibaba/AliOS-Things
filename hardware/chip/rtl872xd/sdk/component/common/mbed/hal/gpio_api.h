/** mbed Microcontroller Library
  ******************************************************************************
  * @file    gpio_api.h
  * @author 
  * @version V1.0.0
  * @brief   This file provides following mbed GPIO API
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
#ifndef MBED_GPIO_API_H
#define MBED_GPIO_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup gpio GPIO
 *  @ingroup    hal
 *  @brief      gpio functions
 *  @{
 */


///@name Ameba Common
///@{

/**
  * @brief  Initializes the GPIO device, include mode/direction/pull control registers.
  * @param  obj: gpio object define in application software.
  * @param  pin: PinName according to pinmux spec.
  * @retval none  
  */
void gpio_init(gpio_t *obj, PinName pin);


/**
  * @brief  Set the given pin as GPIO.
  * @param  pin: PinName according to pinmux spec.
  * @retval : The given pin with GPIO function  
  */
uint32_t gpio_set(PinName pin);

/**
  * @brief  Set GPIO mode.
  * @param  obj: gpio object define in application software.
  * @param  mode: this parameter can be one of the following values:
  *		@arg PullNone: HighZ, user can input high or low use this pin
  *		@arg OpenDrain(is OpenDrain output): no pull + OUT + GPIO[gpio_bit] = 0  
  *		@arg PullDown: pull down
  *		@arg PullUp: pull up
  * @retval none  
  */
void gpio_mode(gpio_t *obj, PinMode mode);

/**
  * @brief  Set GPIO direction.
  * @param  obj: gpio object define in application software.
  * @param  direction: this parameter can be one of the following values:
  *		@arg PIN_INPUT: this pin is input
  *		@arg PIN_OUTPUT: this pin is output  
  * @retval none  
  */
void gpio_dir(gpio_t *obj, PinDirection direction);

/**
  * @brief  Sets value to the selected output port pin.
  * @param  obj: gpio object define in application software.
  * @param  value: specifies the value to be written to the selected pin
  * 	This parameter can be one of the following values:
  *		@arg 0: Pin state set to low
  *		@arg 1: Pin state set to high 
  * @retval none  
  */
void gpio_write(gpio_t *obj, int value);

/**
  * @brief  Reads the specified gpio port pin.
  * @param  obj: gpio object define in application software.
  * @retval 1: pin state is high
  * @retval 0: pin state is low
  */
int  gpio_read(gpio_t *obj);

///@}

/*\@}*/

#ifdef __cplusplus
}
#endif

#endif//MBED_GPIO_API_H
