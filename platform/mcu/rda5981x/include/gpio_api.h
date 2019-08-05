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
#ifndef MBED_GPIO_API_H
#define MBED_GPIO_API_H

#include <stdint.h>
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_NUM 28

static gpio_t gpio_obj[GPIO_NUM];
static const uint16_t gpio_map[GPIO_NUM] = {
    [0] = GPIO_PIN0,
    [1] = GPIO_PIN1,
    [2] = GPIO_PIN2,
    [3] = GPIO_PIN3,
    [4] = GPIO_PIN4,
    [5] = GPIO_PIN5,
    [6] = GPIO_PIN6,
    [7] = GPIO_PIN7,
    [8] = GPIO_PIN8,
    [9] = GPIO_PIN9,
    [10] = GPIO_PIN10,
    [11] = GPIO_PIN11,
    [12] = GPIO_PIN12,
    [13] = GPIO_PIN13,
    [14] = GPIO_PIN14,
    [15] = GPIO_PIN15,
    [16] = GPIO_PIN16,
    [17] = GPIO_PIN17,
    [18] = GPIO_PIN18,
    [19] = GPIO_PIN19,
    [20] = GPIO_PIN20,
    [21] = GPIO_PIN21,
    [22] = GPIO_PIN22,
    [23] = GPIO_PIN23,
    [24] = GPIO_PIN24,
    [25] = GPIO_PIN25,
    [26] = GPIO_PIN26,
    [27] = GPIO_PIN27,
};

typedef enum {
    IRQ_NONE,
    IRQ_RISE,
    IRQ_FALL
} gpio_irq_event;

typedef enum {
    GPIO_IRQ_CH0,
    GPIO_IRQ_CH1,
    CHANNEL_NUM
} GPIO_IRQ_IDX_T;

typedef void (*gpio_irq_handler)(uint32_t id, gpio_irq_event event, uint32_t arg);

/**
 * \defgroup hal_gpio GPIO HAL functions
 * @{
 */

/** Set the given pin as GPIO
 *
 * @param pin The pin to be set as GPIO
 * @return The GPIO port mask for this pin
 **/
uint32_t gpio_set(PinName pin);
/* Checks if gpio object is connected (pin was not initialized with NC)
 * @param pin The pin to be set as GPIO
 * @return 0 if port is initialized with NC
 **/
int gpio_is_connected(const gpio_t *obj);

/** Initialize the GPIO pin
 *
 * @param obj The GPIO object to initialize
 * @param pin The GPIO pin to initialize
 */
void gpio_init(gpio_t *obj, PinName pin);

/** Set the input pin mode
 *
 * @param obj  The GPIO object
 * @param mode The pin mode to be set
 */
void gpio_mode(gpio_t *obj, PinMode mode);

/** Set the pin direction
 *
 * @param obj       The GPIO object
 * @param direction The pin direction to be set
 */
void gpio_dir(gpio_t *obj, PinDirection direction);

/** Set the output value
 *
 * @param obj   The GPIO object
 * @param value The value to be set
 */
void gpio_write(gpio_t *obj, int value);

/** Read the input value
 *
 * @param obj The GPIO object
 * @return An integer value 1 or 0
 */
int gpio_read(gpio_t *obj);

// the following functions are generic and implemented in the common gpio.c file
// TODO: fix, will be moved to the common gpio header file

/** Init the input pin and set mode to PullDefault
 *
 * @param obj The GPIO object
 * @param pin The pin name
 */
void gpio_init_in(gpio_t* gpio, PinName pin);

/** Init the input pin and set the mode
 *
 * @param obj  The GPIO object
 * @param pin  The pin name
 * @param mode The pin mode to be set
 */
void gpio_init_in_ex(gpio_t* gpio, PinName pin, PinMode mode);

/** Init the output pin as an output, with predefined output value 0
 *
 * @param obj The GPIO object
 * @param pin The pin name
 * @return An integer value 1 or 0
 */
void gpio_init_out(gpio_t* gpio, PinName pin);

/** Init the pin as an output and set the output value
 *
 * @param obj   The GPIO object
 * @param pin   The pin name
 * @param value The value to be set
 */
void gpio_init_out_ex(gpio_t* gpio, PinName pin, int value);

/** Init the pin to be in/out
 *
 * @param obj       The GPIO object
 * @param pin       The pin name
 * @param direction The pin direction to be set
 * @param mode      The pin mode to be set
 * @param value     The value to be set for an output pin
 */
void gpio_init_inout(gpio_t* gpio, PinName pin, PinDirection direction, PinMode mode, int value);

/** Init the pin to support irq
 *
 * @param obj       The GPIO object
 * @param handler   irq callback handler
 * @param arg       argument of irq callback handler
 */
int gpio_irq_init(gpio_t *obj, uint32_t handler, uint32_t arg);

/** Enables or disable an interrupt trigger and set trigger type
 *
 * @param obj       The GPIO object
 * @param event     trigger type
 * @param enable    Enables or disable an interrupt trigger
 */
void gpio_irq_set(gpio_t *obj, gpio_irq_event event, uint32_t enable);

/** Disables an interrupt trigger for an input GPIO pin
 *
 * @param obj       The GPIO object
 */
void gpio_irq_free(gpio_t *obj);

/** Enable gpio interrupt trigger
 */
void gpio_irq_enable();

/** Disable gpio interrupt trigger
 */
void gpio_irq_disable();


/**@}*/

#ifdef __cplusplus
}
#endif

#endif
