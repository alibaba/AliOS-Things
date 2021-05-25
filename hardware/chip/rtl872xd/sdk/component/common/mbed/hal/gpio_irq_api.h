/** mbed Microcontroller Library
  ******************************************************************************
  * @file    gpio_irq_api.h
  * @author 
  * @version V1.0.0
  * @brief   This file provides following mbed GPIO IRQ API
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
#ifndef MBED_GPIO_IRQ_API_H
#define MBED_GPIO_IRQ_API_H

#include "device.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup gpio_irq_api GPIO_IRQ
 *  @ingroup    hal
 *  @brief      gpio IRQ functions
 *  @{
 */
 
///@name Ameba Common
///@{
typedef enum {
    IRQ_NONE,
    IRQ_RISE,
    IRQ_FALL
} gpio_irq_event;

typedef void (*gpio_irq_handler)(uint32_t id, gpio_irq_event event);

/**
  * @brief  Initializes the GPIO device interrupt mode, include mode/trigger/polarity registers.
  * @param  obj: gpio irq object define in application software.
  * @param  pin: PinName according to pinmux spec.
  * @param  handler: Interrupt handler to be assigned to the specified pin.
  * @param  id: handler id.
  * @retval none
  * @note this API only works for Port A pins
  */
int gpio_irq_init(gpio_irq_t *obj, PinName pin, gpio_irq_handler handler, uint32_t id);

/**
  * @brief  Deinitializes the GPIO device interrupt mode, include mode/trigger/polarity registers.
  * @param  obj: gpio irq object define in application software.
  * @retval none
  * @note this API only works for Port A pins
  */
void gpio_irq_free(gpio_irq_t *obj);

/**
  * @brief  Enable/Disable gpio interrupt.
  * @param  obj: gpio irq object define in application software.
  * @param  event: gpio interrupt event, this parameter can be one of the following values:
  *		@arg IRQ_RISE: rising edge interrupt event
  *		@arg IRQ_FALL: falling edge interrupt event
  *		@arg IRQ_LOW: low level interrupt event
  *		@arg IRQ_HIGH: high level interrupt event
  *		@arg IRQ_NONE: no interrupt event
  * @param  enable: this parameter can be one of the following values:
  *		@arg 0 disable gpio interrupt
  *		@arg 1 enable gpio interrupt
  * @retval none  
  */
void gpio_irq_set(gpio_irq_t *obj, gpio_irq_event event, uint32_t enable);

/**
  * @brief  Enable gpio interrupt.
  * @param  obj: gpio irq object define in application software.
  * @retval none  
  */
void gpio_irq_enable(gpio_irq_t *obj);

/**
  * @brief  Disable gpio interrupt.
  * @param  obj: gpio irq object define in application software.
  * @retval none  
  */
void gpio_irq_disable(gpio_irq_t *obj);

/**
  * @brief  Enable the specified gpio interrupt event.
  * @param  obj: gpio irq object define in application software.
  * @param  event: gpio interrupt event, this parameter can be one of the following values:
  *		@arg IRQ_RISE: rising edge interrupt event
  *		@arg IRQ_FALL: falling edge interrupt event
  *		@arg IRQ_LOW: low level interrupt event
  *		@arg IRQ_HIGH: high level interrupt event
  *		@arg IRQ_NONE: no interrupt event
  * @retval none  
  */
void gpio_irq_set_event(gpio_irq_t *obj, gpio_irq_event event);

///@}

/*\@}*/

#ifdef __cplusplus
}
#endif

#endif
