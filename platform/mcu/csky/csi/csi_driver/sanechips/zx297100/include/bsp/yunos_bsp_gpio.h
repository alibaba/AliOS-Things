/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef YUNOS_BSP_GPIO_H
#define YUNOS_BSP_GPIO_H

#include <drvs_pinmux.h>
enum irq_mode {
    RISING_EDGE,        ///< rising edge trigger
    FALLING_EDGE,       ///< falling edge trigger
    DOUBLE_EDGE,        ///< double edge trigger
    LOW_LEVEL,          ///< low level trigger
    HIGH_LEVEL          ///< high level trigger
};

enum gpio_mode
{
    PULLING_NONE = 1,
    PULLING_DOWN = 3,
    PULLING_UP   = 5,
};

enum gpio_direction
{
    GPIO_IN,
    GPIO_OUT
};

enum gpio_value
{
    GPIO_VALUE_LOW = 0,
    GPIO_VALUE_HIGH= 1
};

typedef int (*gpio_interrupt_t)(int irqno);

/**
 * @param[in]   get         the value of  selected GPIO pin
 * @param[in]   value       the addr of gpio value
 * @return      OK or error code
 */
int yunos_bsp_gpio_get_value(uint32_t pin, uint32_t *value);

/**
 * @param[in]   set         one or zero to the selected GPIO pin.
 * @param[in]   value       the gpio value
 * @return      zero on success.
 */
int yunos_bsp_gpio_set_value(uint32_t pin, enum gpio_value value);

/**
 * Set the mode of the gpio.
 * @param[in]   pin         gpio number.
 * @param[in]   dir         the direction of gpio .
 * @param[in]   mode        the mode of gpio .
 * @return      It will return OK while getting type succsessfully. Otherwise return !OK.
 */
int yunos_bsp_gpio_set_mode(uint32_t pin, enum gpio_direction dir, enum gpio_mode mode);

/**
 * Register the gpio interrupt.
 * @param[in]   pin         gpio number.
 * @param[in]   irq_mode    the irq mode of the gpio.
 * @param[in]   handler     the interrupt handler function.
 * @return      It will return OK while getting type succsessfully. Otherwise return !OK.
 */
int yunos_bsp_gpio_register_interrupt(uint32_t pin, enum irq_mode irqmode, gpio_interrupt_t handler);

/**
 * Unregister the gpio interrupt.
 * @param[in]   pin         gpio number.
 * @return      It will return OK while getting type succsessfully. Otherwise return !OK.
 */
int yunos_bsp_gpio_unregister_interrupt(uint32_t pin);

/**
 * Set the function of the gpio.
 * @param[in]   pin         gpio number. 
 * @param[in]   func        the function of the gpio.
 * @return      It will return OK while getting type succsessfully. Otherwise return !OK.
 */
int yunos_bsp_gpio_set_func(uint32_t pin, enum gpio_func func);
/**
* This function is used for set gpio15 led func
* @param[in]  value gpio value
* @return the operation status, 0 is OK, others is error
*/
int yunos_bsp_gpio15_light(enum gpio_value value);
#endif /* YUNOS_BSP_GPIO_H */
