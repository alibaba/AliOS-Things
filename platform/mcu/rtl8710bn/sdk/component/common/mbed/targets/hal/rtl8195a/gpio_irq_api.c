/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */
#include "objects.h"
#include "pinmap.h"

//static uint32_t channel_ids[32] = {0};

//static gpio_irq_handler irq_handler;

#if CONFIG_GPIO_EN
#include "gpio_irq_api.h"
#include "gpio_irq_ex_api.h"

int gpio_irq_init(gpio_irq_t *obj, PinName pin, gpio_irq_handler handler, uint32_t id) 
{
    uint32_t pin_name;

    if (pin == NC) return -1;

    obj->pin = pin;
    pin_name = HAL_GPIO_GetPinName((u32)pin);; // get the IP pin name
    obj->hal_pin.pin_name = pin_name;
    obj->hal_pin.pin_mode = INT_FALLING;    // default use Falling trigger
    obj->hal_port_num = HAL_GPIO_GET_PORT_BY_NAME(pin_name);
    obj->hal_pin_num = HAL_GPIO_GET_PIN_BY_NAME(pin_name);
    HAL_GPIO_Irq_Init(&obj->hal_pin);
    HAL_GPIO_UserRegIrq(&obj->hal_pin, (VOID*) handler, (VOID*) id);

    return 0;
}

void gpio_irq_free(gpio_irq_t *obj) 
{
    HAL_GPIO_UserUnRegIrq(&obj->hal_pin);
    HAL_GPIO_DeInit(&obj->hal_pin);
}

void gpio_irq_set(gpio_irq_t *obj, gpio_irq_event event, uint32_t enable) 
{
    switch((uint32_t)event) {
        case IRQ_RISE:
            obj->hal_pin.pin_mode = INT_RISING;
            break;

        case IRQ_FALL:
            obj->hal_pin.pin_mode = INT_FALLING;
            break;

        case IRQ_LOW:
            obj->hal_pin.pin_mode = INT_LOW;
            break;

        case IRQ_HIGH:
            obj->hal_pin.pin_mode = INT_HIGH;
            break;

        case IRQ_NONE:
            // ?
            break;
            
        default:
            break;
    }
//    HAL_GPIO_Irq_Init(&obj->hal_pin);
    HAL_GPIO_Init_8195a(&obj->hal_pin);

    HAL_GPIO_IntCtrl(&obj->hal_pin, enable);
}

void gpio_irq_enable(gpio_irq_t *obj) 
{
    HAL_GPIO_UnMaskIrq(&obj->hal_pin);
}

void gpio_irq_disable(gpio_irq_t *obj) 
{
    HAL_GPIO_MaskIrq(&obj->hal_pin);
}

void gpio_irq_deinit(gpio_irq_t *obj) 
{
    HAL_GPIO_DeInit(&obj->hal_pin);
}

void gpio_irq_pull_ctrl(gpio_irq_t *obj, PinMode pull_type)
{
    HAL_GPIO_PullCtrl((u32) obj->pin, (u32)pull_type);
}

void gpio_irq_set_event(gpio_irq_t *obj, gpio_irq_event event)
{
    uint32_t reg_value;
    uint32_t level_edge;
    uint32_t polarity;
    uint8_t pin_num;

    pin_num = obj->hal_pin_num & 0x1f;  // Max 31
    
    switch (event) {
        case IRQ_LOW:
            level_edge = 0; // level trigger
            polarity = 0;   // active low
            break;
            
        case IRQ_HIGH:
            level_edge = 0; // level trigger
            polarity = 1;   // active high
            break;
            
        case IRQ_FALL:
            level_edge = 1; // edge trigger
            polarity = 0;   // active low
            break;
            
        case IRQ_RISE:
            level_edge = 1; // edge trigger
            polarity = 1;   // active high
            break;
            
        default:
            DBG_GPIO_ERR("Unknow Interrupt Trigger Type(%d)\n", event);
            return;
    }

    // Config Level or Edge trigger
    reg_value =  HAL_READ32(GPIO_REG_BASE, GPIO_INT_TYPE);
    reg_value &= ~(1 << pin_num);    
    reg_value |= (level_edge << pin_num);
    HAL_WRITE32(GPIO_REG_BASE, GPIO_INT_TYPE, reg_value);

    // Config Low active or Gigh active
    reg_value =  HAL_READ32(GPIO_REG_BASE, GPIO_INT_POLARITY);
    reg_value &= ~(1 << pin_num);    
    reg_value |= (polarity << pin_num);
    HAL_WRITE32(GPIO_REG_BASE, GPIO_INT_POLARITY, reg_value);
}

#endif
