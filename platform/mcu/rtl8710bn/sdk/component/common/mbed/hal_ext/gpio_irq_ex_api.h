/* mbed Microcontroller Library
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
 */
#ifndef MBED_GPIO_IRQ_EX_API_H
#define MBED_GPIO_IRQ_EX_API_H

#include "device.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup gpio_irq_ex_api GPIO_IRQ_EX
 *  @ingroup    hal
 *  @brief      gpio IRQ extented functions
 *  @{
 */
 
///@name Ameba Common 
///@{
typedef enum {
    IRQ_LOW = 3,
    IRQ_HIGH =4
} gpio_irq_event_ex;

/**
  * @brief  Deinitializes the GPIO device interrupt mode, include mode/trigger/polarity registers.
  * @param  obj: gpio irq object define in application software.
  * @retval none
  */
void gpio_irq_deinit(gpio_irq_t *obj);

/**
  * @brief  Sets pull type to the selected interrupt pin.
  * @param  obj: gpio irq object define in application software.
  * @param  pull_type: this parameter can be one of the following values:
  *		@arg PullNone: HighZ, user can input high or low use this pin
  *		@arg OpenDrain(is OpenDrain output): no pull + OUT + GPIO[gpio_bit] = 0  
  *		@arg PullDown: pull down
  *		@arg PullUp: pull up 
  * @retval none  
  */
void gpio_irq_pull_ctrl(gpio_irq_t *obj, PinMode pull_type);
///@}

/*\@}*/

#ifdef __cplusplus
}
#endif

#endif  // end of #ifndef MBED_GPIO_IRQ_EX_API_H