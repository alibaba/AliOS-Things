/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"
#include "soc_init.h"
#include "hal.h"
#include "stm32l4xx_hal.h"
#include "hal_gpio_stm32l4.h"

static int32_t gpio_para_transform(gpio_dev_t *gpio, GPIO_InitTypeDef * init_str);
static int32_t get_gpio_group(gpio_dev_t *gpio, GPIO_TypeDef **GPIOx);
static uint16_t get_gpio_pin(uint8_t pin);
static GPIO_InitTypeDef  GPIO_InitStruct;

int32_t hal_gpio_init(gpio_dev_t *gpio)
{
    int32_t ret = -1;
    GPIO_TypeDef *GPIOx = NULL;

    if (gpio == NULL) {
		    return -1;
		}

    ret = get_gpio_group(gpio, &GPIOx);
	  if (ret == 0) {
			ret = gpio_para_transform(gpio, &GPIO_InitStruct);
			if (ret == 0) {
					GPIO_InitStruct.Pin = get_gpio_pin(gpio->port);
					HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
			}
	  }

		return ret;
}

int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
	  uint16_t pin = 0;	
    GPIO_TypeDef* GPIOx = NULL;
    int32_t ret = 0;

	  if (gpio == NULL) {
		    return -1;
		}

    ret = get_gpio_group(gpio, &GPIOx);
		if (ret == 0) {
				pin = get_gpio_pin(gpio->port);
        HAL_GPIO_WritePin(GPIOx, pin, GPIO_PIN_SET);
		};

		return ret;
}

int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
	  uint16_t pin = 0;	
    GPIO_TypeDef* GPIOx = NULL;
    int32_t ret = 0;

	  if (gpio == NULL) {
		    return -1;
		}

    ret = get_gpio_group(gpio, &GPIOx);
		if (ret == 0) {
				pin = get_gpio_pin(gpio->port);
        HAL_GPIO_WritePin(GPIOx, pin, GPIO_PIN_RESET);
		};

		return ret;
}

int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
	  uint16_t pin = 0;	
    GPIO_TypeDef* GPIOx = NULL;
    int32_t ret = 0;

	  if (gpio == NULL) {
		    return -1;
		}

    ret = get_gpio_group(gpio, &GPIOx);
		if (ret == 0) {
				pin = get_gpio_pin(gpio->port);
        HAL_GPIO_TogglePin(GPIOx, pin);		
		};

		return ret;
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
	  uint16_t pin = 0;	
    GPIO_TypeDef* GPIOx = NULL;
    int32_t ret = 0;

	  if (gpio == NULL) {
		    return -1;
		}

    ret = get_gpio_group(gpio, &GPIOx);
		if (ret == 0) {
				pin = gpio->port % PINS_IN_GROUP;
        *value = HAL_GPIO_ReadPin(GPIOx, pin);
		};

		return ret;
}

int32_t gpio_para_transform(gpio_dev_t *gpio, GPIO_InitTypeDef * init_str)
{
    int32_t ret = 0;

	  switch (gpio->config) {
        case ANALOG_MODE:
            init_str->Mode      = GPIO_MODE_ANALOG;
				    break;
        case INPUT_PULL_UP:
            init_str->Mode      = GPIO_MODE_INPUT;
            init_str->Pull      = GPIO_PULLUP;
            break;
        case INPUT_PULL_DOWN:
            init_str->Mode      = GPIO_MODE_INPUT;
            init_str->Pull      = GPIO_PULLDOWN;
            break;
        case INPUT_HIGH_IMPEDANCE:
            init_str->Mode      = GPIO_MODE_INPUT;
            init_str->Pull      = GPIO_NOPULL;
            break;
        case OUTPUT_PUSH_PULL:
            init_str->Mode      = GPIO_MODE_OUTPUT_PP;
            init_str->Pull      = GPIO_PULLUP;
            break;				
        case OUTPUT_OPEN_DRAIN_NO_PULL:
            init_str->Mode      = GPIO_MODE_OUTPUT_OD;
            init_str->Pull      = GPIO_NOPULL;				
            break;
        case OUTPUT_OPEN_DRAIN_PULL_UP:
            init_str->Mode      = GPIO_MODE_OUTPUT_OD;
            init_str->Pull      = GPIO_PULLUP;				
            break;					
			  default:
            ret = -1;
					  break;
		}	

		return ret;
}

int32_t get_gpio_group(gpio_dev_t *gpio, GPIO_TypeDef **GPIOx)
{
	  uint16_t group = 0;
    int32_t ret = 0;

	  if (gpio == NULL) {
		    return -1;
		}

		group = gpio->port / PINS_IN_GROUP; 

    switch (group) {
        case GROUP_GPIOA:
            *GPIOx = GPIOA;
				    break;
        case GROUP_GPIOB:
            *GPIOx = GPIOB;
				    break;
        case GROUP_GPIOC:
            *GPIOx = GPIOC;
				    break;
        case GROUP_GPIOD:
            *GPIOx = GPIOD;
				    break;
        case GROUP_GPIOE:
            *GPIOx = GPIOE;
				    break;
			  default:
					ret = -1;
					  break;
		}

		return ret;
}

uint16_t get_gpio_pin(uint8_t pin)
{
	  uint16_t i = 0;
    uint16_t result = 1;
    uint8_t pin_t = pin % PINS_IN_GROUP;

    for (i = 0; i < pin_t; i++) {
		    result *= 2;
		}

		return result;
}
