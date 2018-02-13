/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include "hal/soc/soc.h"
#include "driver/chip/hal_gpio.h"

static GPIO_InitParam gpios[GPIOA_PIN_NUM];
static GPIO_IrqParam eints[GPIOA_PIN_NUM];
/**@brief Initialises a GPIO pin
 *
 * @note  Prepares a GPIO pin for use.
 *
 * @param gpio          : the gpio pin which should be initialised
 * @param configuration : A structure containing the required
 *                        gpio configuration
 *
 * @return    0         : on success.
 * @return    EIO       : if an error occurred with any step
 */
int32_t hal_gpio_init(gpio_dev_t *gpio)
{
	if (gpio && gpio->port < GPIOA_PIN_NUM) {
		memset(&gpios[gpio->port], 0, sizeof(GPIO_InitParam));
		gpios[gpio->port].driving = GPIO_DRIVING_LEVEL_2;
		switch (gpio->config) {
			case INPUT_HIGH_IMPEDANCE:
				gpios[gpio->port].mode = GPIOx_Pn_F7_DISABLE;
			case INPUT_PULL_UP:
				gpios[gpio->port].mode = GPIOx_Pn_F0_INPUT;
				gpios[gpio->port].pull = GPIO_PULL_UP;
			case INPUT_PULL_DOWN:
				gpios[gpio->port].mode = GPIOx_Pn_F0_INPUT;
				gpios[gpio->port].pull = GPIO_PULL_DOWN;
				break;
			case OUTPUT_OPEN_DRAIN_PULL_UP:
				gpios[gpio->port].mode = GPIOx_Pn_F1_OUTPUT;
				gpios[gpio->port].pull = GPIO_PULL_UP;
				break;
			case OUTPUT_PUSH_PULL:
				gpios[gpio->port].mode = GPIOx_Pn_F1_OUTPUT;;
				gpios[gpio->port].pull = GPIO_PULL_NONE;
				break;
			case OUTPUT_OPEN_DRAIN_NO_PULL:
				gpios[gpio->port].pull = GPIO_PULL_NONE;
				gpios[gpio->port].mode = GPIOx_Pn_F1_OUTPUT;;
				break;
			default:
				break;
		}
		HAL_GPIO_Init(GPIO_PORT_A, gpio->port, &gpios[gpio->port]);
		return 0;
	}
	printf("[hal_gpio]: gpio[%d] init failed\n", gpio ? gpio->port : -1);
	return -1;
}

/**@brief Sets an output GPIO pin high
 *
 * @note  Using this function on a gpio pin which is set to input mode is undefined.
 *
 * @param gpio    : the gpio pin which should be set high
 *
 * @return    0   : on success.
 * @return    EIO : if an error occurred with any step
 */
int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
	if (gpio && gpio->port < GPIOA_PIN_NUM) {
		HAL_GPIO_WritePin(GPIO_PORT_A, gpio->port, GPIO_PIN_HIGH);
		return 0;
	}
	printf("[hal_gpio]: gpio[%d] output highlevel failed\n", gpio ? gpio->port : -1);
	return -1;
}


/**@brief Sets an output GPIO pin low
 *
 * @note  Using this function on a gpio pin which is set to input mode is undefined.
 *
 * @param gpio      : the gpio pin which should be set low
 *
 * @return    0     : on success.
 * @return    EIO   : if an error occurred with any step
 */
int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
	if (gpio && gpio->port < GPIOA_PIN_NUM) {
		HAL_GPIO_WritePin(GPIO_PORT_A, gpio->port, GPIO_PIN_LOW);
		return 0;
	}
	printf("[hal_gpio]: gpio[%d] output lowlevel failed\n", gpio ? gpio->port : -1);
	return -1;
}

/** Trigger an output GPIO pin
 *
 * Trigger an output GPIO pin's output. Using this function on a
 * gpio pin which is set to input mode is undefined.
 *
 * @param gpio      : the gpio pin which should be set low
 *
 * @return    0     : on success.
 * @return    EIO   : if an error occurred with any step
 */
int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
	// TODO:
	return -1;
}

/**@brief Get the state of an input GPIO pin
 *
 * @note Get the state of an input GPIO pin. Using this function on a
 * gpio pin which is set to output mode will return an undefined value.
 *
 * @param gpio     : the gpio pin which should be read
 * @param value    : gpio value
 * @return    0    : on success
 * @return    EIO  : if an error occurred with any step
 */
int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
	if (gpio && gpio->port < GPIOA_PIN_NUM) {
		*value = HAL_GPIO_ReadPin(GPIO_PORT_A, gpio->port);
		return 0;
	}
	printf("[hal_gpio]: gpio[%d] get input value failed\n", gpio ? gpio->port : -1);
	return -1;
}


/**@brief Enables an interrupt trigger for an input GPIO pin
 *
 * @note Enables an interrupt trigger for an input GPIO pin.
 * Using this function on a gpio pin which is set to
 * output mode is undefined.
 *
 * @param gpio    : the gpio pin which will provide the interrupt trigger
 * @param trigger : the type of trigger (rising/falling edge)
 * @param handler : a function pointer to the interrupt handler
 * @param arg     : an argument that will be passed to the
 *                  interrupt handler
 *
 * @return    0   : on success.
 * @return    EIO : if an error occurred with any step
 */
int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                            gpio_irq_handler_t handler, void *arg)
{
	if (gpio && gpio->port < GPIOA_PIN_NUM) {
		GPIO_InitParam param = {0};

		memset(&eints[gpio->port], 0, sizeof(GPIO_IrqParam));
		
		param.driving = GPIO_DRIVING_LEVEL_1;
		param.mode = GPIOx_Pn_F6_EINT;
		param.pull = GPIO_PULL_UP;
		HAL_GPIO_Init(GPIO_PORT_A, gpio->port, &param);

		if (trigger == IRQ_TRIGGER_RISING_EDGE) {
			eints[gpio->port].event = GPIO_IRQ_EVT_RISING_EDGE;
		} else if (trigger == IRQ_TRIGGER_FALLING_EDGE) {
			eints[gpio->port].event = GPIO_IRQ_EVT_FALLING_EDGE;
		} else if (trigger == IRQ_TRIGGER_BOTH_EDGES) {
			eints[gpio->port].event = GPIO_IRQ_EVT_BOTH_EDGE;
		}
		eints[gpio->port].callback = handler;
		eints[gpio->port].arg = arg;
		HAL_GPIO_EnableIRQ(GPIO_PORT_A, gpio->port, &eints[gpio->port]);
		return 0;
	}
	printf("[hal_gpio]: gpio[%d] enable irq failed\n", gpio ? gpio->port : -1);
	return -1;
}


/**@brief Disables an interrupt trigger for an input GPIO pin
 *
 * @note Disables an interrupt trigger for an input GPIO pin.
 * Using this function on a gpio pin which has not been set up
 * using @ref hal_gpio_input_irq_enable is undefined.
 *
 * @param gpio      : the gpio pin which provided the interrupt trigger
 *
 * @return    0     : on success.
 * @return    EIO   : if an error occurred with any step
 */
int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
	if (gpio && gpio->port < GPIOA_PIN_NUM) {
		HAL_GPIO_DisableIRQ(GPIO_PORT_A, gpio->port);
		memset(&eints[gpio->port], 0, sizeof(GPIO_IrqParam));
		return 0;
	}
	
	printf("[hal_gpio]: gpio[%d] disable irq failed\n", gpio ? gpio->port : -1);
	return -1;
}


/**@brief Disables an interrupt trigger for an input GPIO pin
 *
 * @note Disables an interrupt trigger for an input GPIO pin.
 * Using this function on a gpio pin which has not been set up
 * using @ref hal_gpio_input_irq_enable is undefined.
 *
 * @param gpio     : the gpio pin which provided the interrupt trigger
 *
 * @return    0    : on success.
 * @return    EIO  : if an error occurred with any step
 */
int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
	return 0;
}


/**@brief DeInitialises a GPIO pin
 *
 * @note  Set a GPIO pin in default state.
 *
 * @param  gpio     : the gpio pin which should be deinitialised
 *
 * @return    0     : on success.
 * @return    EIO   : if an error occurred with any step
 */
int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{
	if (gpio && gpio->port < GPIOA_PIN_NUM) {
		HAL_GPIO_DeInit(GPIO_PORT_A, gpio->port);
		memset(&gpios[gpio->port], 0, sizeof(GPIO_InitParam));
		return 0;
	}
	return -1;
}
