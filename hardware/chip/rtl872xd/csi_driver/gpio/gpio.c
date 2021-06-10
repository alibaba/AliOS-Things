/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "objects.h"
#include "pinmap.h"
#include "PinNames.h"
#include <drv/gpio.h>
#include <aos/gpioc_csi.h>

static void user_interrupt_handler(csi_gpio_t *gpio, uint32_t id)
{
    if (gpio->callback != NULL) {
        uint32_t pin_mask;

        pin_mask = (uint32_t)1 << ((id >> 16) & 0x1F);
        gpio->callback(gpio, pin_mask, gpio->arg);
    }
}

csi_error_t csi_gpio_init(csi_gpio_t *gpio, uint32_t port_idx)
{
    if(!gpio)
        return CSI_ERROR;

    if (port_idx != PORT_A && port_idx != PORT_B){
        printf("error\n");
        return CSI_ERROR;
    }

    gpio->dev.idx = port_idx;
    gpio->callback = NULL;
    gpio->arg = NULL;

    return CSI_OK;
}

void csi_gpio_uninit(csi_gpio_t *gpio)
{
    if (gpio->dev.idx != PORT_A && gpio->dev.idx != PORT_B) {
        return;
    }

    uint32_t pin_idx = 0;

    while(pin_idx < 32) {
        GPIO_DeInit(((gpio->dev.idx)<<5)|pin_idx);
        pin_idx++;
    }
}

csi_error_t csi_gpio_dir(csi_gpio_t *gpio, uint32_t pin_mask, csi_gpio_dir_t dir)
{
    if (gpio->dev.idx != PORT_A && gpio->dev.idx != PORT_B) {
        return CSI_ERROR;
    }

    uint32_t pin_idx = 0;

    while(pin_idx < sizeof(pin_mask) * 8) {
        if((pin_mask >> pin_idx) & 0x01) {
            GPIO_InitTypeDef GPIO_InitStruct;
            GPIO_InitStruct.GPIO_Pin = ((gpio->dev.idx)<<5)|pin_idx;
            printf("gpio pin:%d\n",pin_idx);
            GPIO_InitStruct.GPIO_Mode = dir;
            GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
            GPIO_Init(&GPIO_InitStruct);
        }
        pin_idx++;
    }
    return CSI_OK;
}

csi_error_t csi_gpio_mode(csi_gpio_t *gpio, uint32_t pin_mask, csi_gpio_mode_t mode)
{
    if (gpio->dev.idx != PORT_A && gpio->dev.idx != PORT_B) {
        return CSI_ERROR;
    }

    uint32_t GPIO_PuPd;
    uint32_t pin_idx = 0;

    switch (mode) {
        case GPIO_MODE_PULLNONE:/* No driver -> Input & High Impendance */
            GPIO_PuPd = GPIO_PuPd_NOPULL;
        break;

        case GPIO_MODE_PULLDOWN:
            GPIO_PuPd = GPIO_PuPd_DOWN;
        break;

        case GPIO_MODE_PULLUP:
            GPIO_PuPd = GPIO_PuPd_UP;
        break;

        default:
            GPIO_PuPd = GPIO_PuPd_NOPULL;
        break;
    }

    while(pin_idx < sizeof(pin_mask) * 8) {
        if((pin_mask >> pin_idx) & 0x01) {
            PAD_PullCtrl(((gpio->dev.idx)<<5)|pin_idx, GPIO_PuPd);
        }
        pin_idx++;
    }
    return CSI_OK;
}


csi_error_t csi_gpio_irq_mode(csi_gpio_t *gpio, uint32_t pin_mask, csi_gpio_irq_mode_t mode)
{
    if (gpio->dev.idx != PORT_A && gpio->dev.idx != PORT_B) {
        return CSI_ERROR;
    }

    uint32_t GPIO_ITTrigger;
    uint32_t GPIO_ITPolarity;
    uint32_t pin_idx = 0;

    if(gpio->dev.idx == PORT_A) {
        InterruptRegister(GPIO_INTHandler, GPIOA_IRQ, (uint32_t)GPIOA_BASE, 5);
        InterruptEn(GPIOA_IRQ, 5);
    } else if (gpio->dev.idx == PORT_B){
        InterruptRegister(GPIO_INTHandler, GPIOB_IRQ, (uint32_t)GPIOB_BASE, 5);
        InterruptEn(GPIOB_IRQ, 5);
    }

    switch(mode) {
        case GPIO_IRQ_MODE_RISING_EDGE:
            GPIO_ITTrigger = GPIO_INT_Trigger_EDGE;
            GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_HIGH;
        break;

        case GPIO_IRQ_MODE_FALLING_EDGE:
            GPIO_ITTrigger = GPIO_INT_Trigger_EDGE;
            GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
        break;

        case GPIO_IRQ_MODE_BOTH_EDGE:
            GPIO_ITTrigger = GPIO_INT_Trigger_BOTHEDGE;
            GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
        break;

        case GPIO_IRQ_MODE_LOW_LEVEL:
            GPIO_ITTrigger = GPIO_INT_Trigger_LEVEL;
            GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
        break;

        case GPIO_IRQ_MODE_HIGH_LEVEL:
            GPIO_ITTrigger = GPIO_INT_Trigger_LEVEL;
            GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_HIGH;
        break;

        default:
        break;
    }

    while(pin_idx < sizeof(pin_mask) * 8) {
        if((pin_mask >> pin_idx) & 0x01) {
            GPIO_INTMode(((gpio->dev.idx)<<5)|pin_idx, ENABLE, GPIO_ITTrigger, GPIO_ITPolarity, GPIO_INT_DEBOUNCE_ENABLE);
            if(gpio->callback != NULL) {
                GPIO_UserRegIrq(((gpio->dev.idx)<<5)|pin_idx, user_interrupt_handler, gpio);
            }
        }
        pin_idx++;
    }
    return CSI_OK;
}

csi_error_t csi_gpio_irq_enable(csi_gpio_t *gpio, uint32_t pin_mask, bool enable)
{
    if (gpio->dev.idx != PORT_A && gpio->dev.idx != PORT_B) {
        return CSI_ERROR;
    }

    uint32_t pin_idx = 0;

    while(pin_idx < sizeof(pin_mask) * 8) {
        if((pin_mask >> pin_idx) & 0x01) {
            GPIO_INTConfig(((gpio->dev.idx)<<5)|pin_idx, enable);
        }
        pin_idx++;
    }
    return CSI_OK;
}

void csi_gpio_write(csi_gpio_t *gpio, uint32_t pin_mask, csi_gpio_pin_state_t value)
{
    if (gpio->dev.idx != PORT_A && gpio->dev.idx != PORT_B) {
        return;
    }

    uint32_t pin_idx = 0;

    while(pin_idx < sizeof(pin_mask) * 8) {
        if((pin_mask >> pin_idx) & 0x01) {
            GPIO_WriteBit(((gpio->dev.idx)<<5)|pin_idx, value);
        }
        pin_idx++;
    }
}

void csi_gpio_toggle(csi_gpio_t *gpio, uint32_t pin_mask)
{
    if (gpio->dev.idx != PORT_A && gpio->dev.idx != PORT_B) {
        return;
    }

    uint32_t pin_idx = 0, value;

    while(pin_idx < sizeof(pin_mask) * 8) {
        if((pin_mask >> pin_idx) & 0x01) {
            value = GPIO_ReadDataBit(((gpio->dev.idx)<<5)|pin_idx);
            GPIO_WriteBit(((gpio->dev.idx)<<5)|pin_idx, !value);
        }
        pin_idx++;
    }
}

uint32_t csi_gpio_read(csi_gpio_t *gpio, uint32_t pin_mask)
{
    if (gpio->dev.idx != PORT_A && gpio->dev.idx != PORT_B) {
        return CSI_ERROR;
    }

    uint32_t pin_idx = 0, value = 0;

    while(pin_idx < sizeof(pin_mask) * 8) {
        if((pin_mask >> pin_idx) & 0x01) {
            value |= (GPIO_ReadDataBit(((gpio->dev.idx)<<5)|pin_idx) << pin_idx);
        }
        pin_idx++;
    }

    return value;
}

csi_error_t csi_gpio_attach_callback(csi_gpio_t *gpio, void *callback, void *arg)
{
    gpio->callback = callback;
    gpio->arg = arg;

    return CSI_OK;
}

void csi_gpio_detach_callback(csi_gpio_t *gpio)
{
    if (gpio->callback != NULL) {
        gpio->callback = NULL;
        gpio->arg = NULL;
    }
}

static int gpioc_csi_init(void)
{
    static aos_gpioc_csi_t gpioc_csi[2];
    int ret;

    gpioc_csi[0].gpioc.dev.id = 0;
    gpioc_csi[0].gpioc.num_pins = 32;
    gpioc_csi[0].default_input_cfg = AOS_GPIO_INPUT_CFG_HI;
    gpioc_csi[0].default_output_cfg = AOS_GPIO_OUTPUT_CFG_PP;
    ret = (int)aos_gpioc_csi_register(&gpioc_csi[0]);
    if (ret)
        return ret;

    gpioc_csi[1].gpioc.dev.id = 1;
    gpioc_csi[1].gpioc.num_pins = 32;
    gpioc_csi[1].default_input_cfg = AOS_GPIO_INPUT_CFG_HI;
    gpioc_csi[1].default_output_cfg = AOS_GPIO_OUTPUT_CFG_PP;
    ret = (int)aos_gpioc_csi_register(&gpioc_csi[1]);
    if (ret) {
        (void)aos_gpioc_csi_unregister(0);
        return ret;
    }

    return 0;
}

LEVEL1_DRIVER_ENTRY(gpioc_csi_init)
