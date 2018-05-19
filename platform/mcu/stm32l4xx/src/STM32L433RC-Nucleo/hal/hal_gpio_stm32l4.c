/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"
#include "soc_init.h"
#include "hal/hal.h"
#include "hal_gpio_stm32l4.h"

static int32_t gpio_para_transform(gpio_dev_t *gpio, GPIO_InitTypeDef * init_str);
static int32_t get_gpio_group(gpio_dev_t *gpio, GPIO_TypeDef **GPIOx);
static uint32_t get_gpio_pin(uint8_t pin);
//static GPIO_InitTypeDef  GPIO_InitStruct;
int32_t gpio_has_priv(gpio_dev_t *gpio, GPIO_InitTypeDef * init_str);

int32_t hal_gpio_init(gpio_dev_t *gpio)
{
    int32_t ret = -1;
    GPIO_TypeDef *GPIOx = NULL;
    GPIO_InitTypeDef  GPIO_InitStruct;

    if (gpio == NULL) {
        return -1;
    }

    ret = get_gpio_group(gpio, &GPIOx);
    if (ret){
        return ret;
    }

    if (GPIOx == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    } else if (GPIOx == GPIOB) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    } else if (GPIOx == GPIOC) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    } else {
        return -1;
    }
    
    ret = gpio_para_transform(gpio, &GPIO_InitStruct);
    if (ret){
        return ret;
    }

    GPIO_InitStruct.Pin = get_gpio_pin(gpio->port);
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
    ret |= gpio_has_priv(gpio, &GPIO_InitStruct);
    
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

int32_t gpio_get_int_num(IRQn_Type *pirqn, uint8_t port)
{
    uint32_t gpio_pin = 0;

    if (pirqn == NULL)
        return -1;

    gpio_pin = get_gpio_pin(port);
    switch (gpio_pin) {
        case GPIO_PIN_0:
            *pirqn = EXTI0_IRQn;
            break;
        case GPIO_PIN_5:
        case GPIO_PIN_6:
        case GPIO_PIN_7:
        case GPIO_PIN_8:
        case GPIO_PIN_9:
            *pirqn = EXTI9_5_IRQn;
            break;
        case GPIO_PIN_10:
        case GPIO_PIN_11:
        case GPIO_PIN_12:
        case GPIO_PIN_13:
        case GPIO_PIN_14:
        case GPIO_PIN_15:
            *pirqn = EXTI15_10_IRQn;
            break;
        default:
            return -1;
    }

    return 0;
}

int32_t gpio_has_priv(gpio_dev_t *gpio, GPIO_InitTypeDef * init_str)
{
    int32_t ret = 0;

    if (gpio->priv == NULL) {
        return -1;
    }
    switch (gpio->config) {
        case IRQ_MODE:
            switch (*(gpio_irq_trigger_t *)gpio->priv) {
                case IRQ_TRIGGER_RISING_EDGE:
                    init_str->Mode      = GPIO_MODE_IT_RISING;
                    init_str->Pull      = GPIO_NOPULL;
                    break;
                case IRQ_TRIGGER_FALLING_EDGE:
                    init_str->Mode      = GPIO_MODE_IT_FALLING;
                    init_str->Pull      = GPIO_NOPULL;
                    break;
                case IRQ_TRIGGER_BOTH_EDGES:
                    init_str->Mode      = GPIO_MODE_IT_RISING_FALLING;
                    init_str->Pull      = GPIO_NOPULL;
                    break;
                default:
                    ret = -1;
                    break;
            }
            break;
        case OUTPUT_PUSH_PULL:
        case OUTPUT_OPEN_DRAIN_NO_PULL:
        case OUTPUT_OPEN_DRAIN_PULL_UP:
            switch (*(uint8_t *)gpio->priv) {
                case 1:
                    hal_gpio_output_high(gpio);
                    break;
                case 0:
                    hal_gpio_output_low(gpio);
                    break;
                default:
                    ret = -1;
                    break;
            }
            break;
        default:
            break;
    }

    return ret;
}

int32_t gpio_para_transform(gpio_dev_t *gpio, GPIO_InitTypeDef * init_str)
{
    int32_t ret = 0;
    IRQn_Type pirqn = 0;

    switch (gpio->config) {
        case ANALOG_MODE:
            init_str->Mode      = GPIO_MODE_ANALOG;
            break;
        case IRQ_MODE:
            ret = gpio_get_int_num(&pirqn, gpio->port);
            if (ret == 0) {
                HAL_NVIC_SetPriority(pirqn, 0, 0);
                HAL_NVIC_EnableIRQ(pirqn);
            }
            ret |= gpio_has_priv(gpio, init_str);
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
            init_str->Pull      = GPIO_NOPULL;
            init_str->Speed     = GPIO_SPEED_FREQ_LOW;
            break;        
        case OUTPUT_OPEN_DRAIN_NO_PULL:
            init_str->Mode      = GPIO_MODE_OUTPUT_OD;
            init_str->Pull      = GPIO_NOPULL;
            init_str->Speed     = GPIO_SPEED_FREQ_LOW;
            break;
        case OUTPUT_OPEN_DRAIN_PULL_UP:
            init_str->Mode      = GPIO_MODE_OUTPUT_OD;
            init_str->Pull      = GPIO_PULLUP;
            init_str->Speed     = GPIO_SPEED_FREQ_LOW;
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

uint32_t get_gpio_pin(uint8_t pin)
{
    uint32_t result = 1;
    uint8_t pin_t = pin % PINS_IN_GROUP;

    result = 1u << pin_t;

    return result;
}
