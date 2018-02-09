/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
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
/******************************************************************************
 * @file     zx29_gpio.c
 * @brief    CSI Source File for GPIO Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <stdbool.h>
#include <stdio.h>
#include <drv_gpio.h>
#include <drv_intc.h>
#include "zx29_gpio.h"
#include <io.h>
#include <pinmux.h>
#include <soc.h>
#include <csi_core.h>

#define ERR_GPIO(errno) (CSI_DRV_ERRNO_GPIO_BASE | errno)
#define GPIO_NULL_PARAM_CHK(para)                         \
    do {                                        \
        if (para == NULL) {                     \
            return ERR_GPIO(DRV_ERROR_PARAMETER);   \
        }                                       \
    } while (0)
typedef void *gpio_port_handle_t;

//typedef struct {
//    uint32_t base;              ///< handle register base
//    uint32_t irq;               ///< irq of this handle
//    uint32_t pin_num;           ///< pin number of this handle
//    gpio_mode_e mode;           ///< gpio mode
//    gpio_direction_e dir;       ///< gpio direction
//    uint32_t mask;              ///< gpio mask bit
//    uint32_t value;             ///< gpio value
//} zx29_gpio_priv_t;

typedef struct {
    uint8_t     idx;
    int32_t pin_name;
    uint8_t irq;
    gpio_event_cb_t cb;
} zx29_gpio_pin_priv_t;

extern int32_t target_gpio_pin_init(int32_t gpio_pin, uint32_t *port_idx, uint8_t *irq);

//static zx29_gpio_priv_t gpio_handle[CONFIG_GPIO_NUM];
static zx29_gpio_pin_priv_t gpio_pin_handle[CONFIG_GPIO_PIN_NUM];

//
// Functions
//

static int32_t gpio_pull_mode(uint32_t pin, gpio_mode_e mode)
{
    uint32_t pinmux_io_reg;

    if (pin > SI_GPIO_MAX) {
        return -1;
    }

    uint8_t pull_mode;
    if (mode == GPIO_MODE_PULLNONE) {
        pull_mode = 1;
    } else if (mode == GPIO_MODE_PULLDOWM) {
        pull_mode = 3;
    } else if (mode == GPIO_MODE_PULLUP) {
        pull_mode = 5;
    } else {
        pull_mode = mode;
    }

    if (BIT_WIDTH(pin_info[pin].io_cfg_field)) {
        if (pin < GPIO_AO_NR) {
            pinmux_io_reg = PINMUX_IO_CFG_REG(pin_info[pin].io_cfg_field);
        } else {
            pinmux_io_reg = PINMUX_IO_CFG_PD_REG(pin_info[pin].io_cfg_field);
        }

        __write_bits(BIT_WIDTH(pin_info[pin].io_cfg_field),
                     BIT_OFFSET(pin_info[pin].io_cfg_field),
                     (volatile uint32_t *)pinmux_io_reg,
                     pull_mode);
    }

    return 0;
}

static int32_t gpio_set_direction(
    uint32_t pin,
    gpio_direction_e dir

)
{
    uint32_t addr  = GPIO_DIR_REG(pin);
    uint32_t value = getreg32((volatile uint32_t *)addr);

    if (dir == GPIO_DIRECTION_INPUT) {
        value &= ~(1 << (pin % 16));
    } else {
        value |= (1 << (pin % 16));
    }

    putreg32(value, (volatile uint32_t *)addr);

    return 0;
}

/*
 * Read the statu of the Port choosed.
 * Parameters:
 *   port:  use to choose a I/O port among Port A, B, or C.
 * return: the value of the corresponding Port.
 */

int32_t gpio_read(uint32_t pin, uint32_t *value)
{
    if (pin > SI_GPIO_MAX) {
        return -1;
    }

    uint32_t val = getreg32((volatile uint32_t *)GPIO_IN_REG(pin));

    *value = (val >> (pin % 16)) & 0x1;

    return 0;
}


/*
 * Write an output value to corresponding Port.
 * Parameters:
 *   port:  use to choose a I/O port among Port A, B, or C.
 *   output: value that will be written to the corresponding Port.
 * return: SUCCESS
 */

static int32_t gpio_write(uint32_t pin, uint8_t value)
{
    if (pin > SI_GPIO_MAX) {
        return -1;
    }

    uint32_t val = 0;
    uint32_t addr = 0;

    if (value == 0) {
        addr = GPIO_OUT_LOW_REG(pin);
    } else {
        addr = GPIO_OUT_HIGH_REG(pin);
    }

    val = getreg32((volatile uint32_t *)addr);
    val |= (1 << (pin % 16));
    putreg32(val, (volatile uint32_t *)addr);

    return 0;
}

#if 0
/**
 * Configure a GPIO gpio_set_irq_mode.
 * @param[in]   pin         the addr store the pin num.
 * @param[in]   _irqmode    the irqmode of gpio
 * @return      zero on success. -1 on falure.
 */
static int32_t gpio_set_irq_mode(gpio_pin_handle_t pin, gpio_irq_mode_e irq_mode)
{
    zx29_gpio_pin_priv_t *gpio_pin_priv = pin;

    zx29_gpio_control_reg_t *gpio_control_reg = (zx29_gpio_control_reg_t *)((((zx29_gpio_priv_t *)gpio_pin_priv->handle)->base) + 0x30);
    uint32_t offset = gpio_pin_priv->idx;
    uint32_t mask = 1 << offset;

    switch (irq_mode) {
        /* rising edge interrupt mode */
        case GPIO_IRQ_MODE_RISING_EDGE:
            gpio_control_reg->INTTYPE_LEVEL |= mask;
            gpio_control_reg->INT_POLARITY |= mask;
            break;

        /* falling edge interrupt mode */
        case GPIO_IRQ_MODE_FALLING_EDGE:
            gpio_control_reg->INTTYPE_LEVEL |= mask;
            gpio_control_reg->INT_POLARITY &= (~mask);
            break;

        /* low level interrupt mode */
        case GPIO_IRQ_MODE_LOW_LEVEL:
            gpio_control_reg->INTTYPE_LEVEL &= (~mask);
            gpio_control_reg->INT_POLARITY &= (~mask);
            break;

        /* high level interrupt mode */
        case GPIO_IRQ_MODE_HIGH_LEVEL:
            gpio_control_reg->INTTYPE_LEVEL &= (~mask);
            gpio_control_reg->INT_POLARITY |= mask;
            break;

        /* double edge interrupt mode */
        case GPIO_IRQ_MODE_DOUBLE_EDGE:
            return ERR_GPIO(DRV_ERROR_UNSUPPORTED);

        default:
            return ERR_GPIO(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/*
 * Clear one or more interrupts of PortA.
 * Parameters:
 *   pinno:
 * return: SUCCESS.
 */

static void gpio_irq_clear(gpio_pin_handle_t pin, uint32_t idx)
{
    zx29_gpio_pin_priv_t *gpio_pin_priv = pin;
    zx29_gpio_control_reg_t *gpio_control_reg = (zx29_gpio_control_reg_t *)((((zx29_gpio_priv_t *)gpio_pin_priv->handle)->base) + 0x30);

    gpio_control_reg->PORTA_EOI = idx;
}


/*
 * Enable one or more interrupts of PortA.
 * Parameters:
 *   pinno:
 * return: SUCCESS.
 */
static void gpio_irq_enable(gpio_pin_handle_t pin)
{
    zx29_gpio_pin_priv_t *gpio_pin_priv = pin;
    zx29_gpio_control_reg_t *gpio_control_reg = (zx29_gpio_control_reg_t *)((((zx29_gpio_priv_t *)gpio_pin_priv->handle)->base) + 0x30);
    uint32_t offset = gpio_pin_priv->idx;
    uint32_t val = gpio_control_reg->INTEN;
    val |= (1 << offset);
    gpio_control_reg->INTEN = val;
}


/*
 * Disable one or more interrupts of PortA.
 * Parameters:
 *   pinno:
 * return: SUCCESS.
 */

static void gpio_irq_disable(gpio_pin_handle_t pin)
{
    zx29_gpio_pin_priv_t *gpio_pin_priv = pin;
    uint32_t offset = gpio_pin_priv->idx;
    zx29_gpio_control_reg_t *gpio_control_reg = (zx29_gpio_control_reg_t *)((((zx29_gpio_priv_t *)gpio_pin_priv->handle)->base) + 0x30);
    uint32_t val = gpio_control_reg->INTEN;
    val &= ~(1 << offset);
    gpio_control_reg->INTEN = val;
}

#endif

void zx29_gpio_irqhandler(int idx)
{
    zx29_gpio_pin_priv_t *gpio_pin_priv = (zx29_gpio_pin_priv_t *)&gpio_pin_handle[idx];

    /* execute the callback function */
    if ((gpio_event_cb_t)(gpio_pin_priv->cb)) {
        ((gpio_event_cb_t)(gpio_pin_priv->cb))(idx);
    }

    csi_intc_ack_irq(gpio_pin_priv->irq);
}

/**
  \brief       Initialize GPIO module. 1. Initializes the resources needed for the GPIO handle 2.registers event callback function
                3.get gpio_port_handle
  \param[in]   port      port_name.
  \return      gpio_port_handle
*/
gpio_port_handle_t csi_gpio_port_initialize(int32_t port)
{
#if 0
    uint32_t i;
    zx29_gpio_priv_t *gpio_priv;

    for (i = 0; i <= port; i++) {
        /* obtain the gpio port information */
        uint32_t base = 0u;
        uint32_t pin_num;
        uint32_t irq;
        uint32_t idx = target_gpio_port_init(i, &base, &irq, &pin_num);

        if (idx < 0 || idx >= CONFIG_GPIO_NUM) {
            return NULL;
        }

        gpio_priv = &gpio_handle[idx];

        gpio_priv->base = base;
        gpio_priv->irq  = irq;
        gpio_priv->pin_num  = pin_num;
    }

    csi_vic_enable_irq(gpio_priv->irq);

    return (gpio_port_handle_t)gpio_priv;
#endif
    return NULL;
}

/**
  \brief       De-initialize GPIO handle. stops operation and releases the software resources used by the handle
  \param[in]   handle   gpio port handle to operate.
  \return      error code
*/
int32_t csi_gpio_port_uninitialize(gpio_port_handle_t handle)
{
#if 0
    GPIO_NULL_PARAM_CHK(handle);

    zx29_gpio_priv_t *gpio_priv = handle;

    csi_vic_disable_irq(gpio_priv->irq);

    return 0;
#endif
    return ERR_GPIO(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       config multiple pin within one handle
  \param[in]   handle    gpio port handle to operate.
  \param[in]   mask      the bitmask to identify which bits in the handle should be included (0 - ignore)
  \param[in]   mode      \ref gpio_mode_e
  \param[in]   dir       \ref gpio_direction_e
  \return      error code
*/
int32_t csi_gpio_port_config(gpio_port_handle_t handle, uint32_t mask, gpio_mode_e mode, gpio_direction_e dir)
{
#if 0

    if (mask < 0) {
        return ERR_GPIO(DRV_ERROR_PARAMETER);
    }

    GPIO_NULL_PARAM_CHK(handle);

    zx29_gpio_priv_t *gpio_priv = handle;
    /*config the gpio mode direction mask bits */
    gpio_priv->mode = mode;
    gpio_priv->dir = dir;
    gpio_priv->mask = mask;
    uint32_t ret = gpio_set_direction(gpio_priv, dir);

    return ret;
#endif
    return ERR_GPIO(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Write value to the handle(write value to multiple pins on one handle at the same time)
  \param[in]   handle    gpio port handle to operate.
  \param[in]   mask      The bitmask to identify which bits in the handle should be included (0 - ignore)
  \param[in]   value     the value to be set
  \return      error code
*/
int32_t csi_gpio_port_write(gpio_port_handle_t handle, uint32_t mask, uint32_t value)
{
#if 0

    if (mask < 0 || value < 0) {
        return ERR_GPIO(DRV_ERROR_PARAMETER);
    }

    GPIO_NULL_PARAM_CHK(handle);

    uint32_t port_value = mask & value;

    zx29_gpio_priv_t *gpio_priv = handle;
    gpio_priv->value = port_value;
    gpio_write(gpio_priv, mask);

    return 0;
#endif
    return ERR_GPIO(DRV_ERROR_UNSUPPORTED);

}

/**
  \brief       Read the current value on the handle(read value of multiple pins on one handle at the same time)
  \param[in]   handle    gpio port handle to operate.
  \param[in]   mask      The bitmask to identify which bits in the handle should be included (0 - ignore)
  \param[out]  value     an integer with each bit corresponding to an associated handle pin setting
  \return      error code
*/
int32_t csi_gpio_port_read(gpio_port_handle_t handle, uint32_t mask, uint32_t *value)
{
#if 0

    if (mask < 0) {
        return ERR_GPIO(DRV_ERROR_PARAMETER);
    }

    GPIO_NULL_PARAM_CHK(handle);
    GPIO_NULL_PARAM_CHK(value);

    uint32_t port_value = 0;

    gpio_read(handle, &port_value);
    *value = (mask & port_value);

    return 0;

#endif
    return ERR_GPIO(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Initialize GPIO handle.
  \param[in]   gpio_pin    Pointer to the int32_t.
  \param[in]   cb_event    Pointer to \ref gpio_event_cb_t.
  \return      gpio_pin_handle
*/
gpio_pin_handle_t csi_gpio_pin_initialize(int32_t gpio_pin, gpio_event_cb_t cb_event)
{
    /* obtain the gpio pin information */
    uint32_t port_idx;
    uint8_t irq;
    uint32_t pin_idx = target_gpio_pin_init(gpio_pin, &port_idx, &irq);

//    uint32_t idx = pin_idx;
//    uint32_t i;

//    for (i = 0; i < port_idx; i++) {
//        idx += (gpio_handle[i].pin_num);
//    }

//    zx29_gpio_pin_priv_t *gpio_pin_priv = &(gpio_pin_handle[idx]);
//    gpio_pin_priv->handle = (gpio_port_handle_t)&gpio_handle[port_idx];
    zx29_gpio_pin_priv_t *gpio_pin_priv = &(gpio_pin_handle[pin_idx]);


    gpio_pin_priv->idx = pin_idx;
    gpio_pin_priv->cb = cb_event;
    gpio_pin_priv->irq = irq;

    return (gpio_pin_handle_t)gpio_pin_priv;
}

/**
  \brief       De-initialize GPIO pin handle. stops operation and releases the software resources used by the handle
  \param[in]   handle   gpio pin handle to operate.
  \return      error code
*/
int32_t csi_gpio_pin_uninitialize(gpio_pin_handle_t handle)
{
    if (handle == NULL) {
        return ERR_GPIO(DRV_ERROR_PARAMETER);
    }

    zx29_gpio_pin_priv_t *gpio_pin_priv = (zx29_gpio_pin_priv_t *)handle;
    gpio_pin_priv->cb = NULL;
    csi_vic_disable_irq(gpio_pin_priv->irq);
    return 0;
}

/**
  \brief       config pin mode
  \param[in]   pin       gpio pin handle to operate.
  \param[in]   mode      \ref gpio_mode_e
  \return      error code
*/
int32_t csi_gpio_pin_config_mode(gpio_pin_handle_t handle,
                            gpio_mode_e mode)
{
    GPIO_NULL_PARAM_CHK(handle);

    uint32_t ret = 0;
    /* config the gpio pin mode direction mask bits */
    zx29_gpio_pin_priv_t *gpio_pin_priv = handle;

    ret = gpio_pull_mode(gpio_pin_priv->idx, mode);

    if (ret) {
        return ret;
    }

    return 0;
}

/**
  \brief       config pin direction
  \param[in]   pin       gpio pin handle to operate.
  \param[in]   dir       \ref gpio_direction_e
  \return      error code
*/
int32_t csi_gpio_pin_config_direction(gpio_pin_handle_t handle,
                            gpio_direction_e dir)
{
    GPIO_NULL_PARAM_CHK(handle);

    uint32_t ret = 0;
    /* config the gpio pin mode direction mask bits */
    zx29_gpio_pin_priv_t *gpio_pin_priv = handle;

    ret = gpio_set_direction(gpio_pin_priv->idx, dir);

    if (ret) {
        return ret;
    }

    return 0;
}

/**
  \brief       config pin
  \param[in]   handle       gpio pin handle to operate.
  \param[in]   mode      \ref gpio_mode_e
  \param[in]   dir       \ref gpio_direction_e
  \return      error code
*/
int32_t csi_gpio_pin_config(gpio_pin_handle_t handle,
                            gpio_mode_e mode,
                            gpio_direction_e dir)
{
    GPIO_NULL_PARAM_CHK(handle);

    uint32_t ret = 0;
    /* config the gpio pin mode direction mask bits */
    zx29_gpio_pin_priv_t *gpio_pin_priv = handle;

    ret = gpio_set_direction(gpio_pin_priv->idx, dir);

    if (ret) {
        return ret;
    }

    ret = gpio_pull_mode(gpio_pin_priv->idx, mode);

    if (ret) {
        return ret;
    }

    return 0;

}

/**
  \brief       Set one or zero to the selected GPIO pin.
  \param[in]   handle       gpio pin handle to operate.
  \param[in]   value     the value to be set
  \return      error code
*/
int32_t csi_gpio_pin_write(gpio_pin_handle_t handle, bool value)
{
    GPIO_NULL_PARAM_CHK(handle);

    if ((int32_t)value < 0) {
        return ERR_GPIO(DRV_ERROR_PARAMETER);
    }

    zx29_gpio_pin_priv_t *gpio_pin_priv = handle;
//    uint8_t offset = gpio_pin_priv->idx;
//    uint32_t port_value = value << offset;

    gpio_write(gpio_pin_priv->idx, value);

    return 0;

}

/**
  \brief       Get the value of  selected GPIO pin.
  \param[in]   handle       gpio pin handle to operate.
  \param[out]  value     buf to store the pin value
  \return      error code
*/
int32_t csi_gpio_pin_read(gpio_pin_handle_t handle, bool *value)
{
    GPIO_NULL_PARAM_CHK(handle);

    if (value <= 0) {
        return ERR_GPIO(DRV_ERROR_PARAMETER);
    }

    zx29_gpio_pin_priv_t *gpio_pin_priv = handle;
    uint32_t port_value;
//    uint8_t offset = gpio_pin_priv->idx;

    gpio_read(gpio_pin_priv->idx, &port_value);
    *value = port_value;
//    *value = (port_value & (1 << offset)) >> offset;

    return 0;
}

/**
  \brief       set GPIO interrupt mode.
  \param[in]   handle       gpio pin handle to operate.
  \param[in]   mode      the irq mode to be set
  \param[in]   enable    the enable flag
  \return      error code
*/
int32_t csi_gpio_pin_set_irq(gpio_pin_handle_t handle, gpio_irq_mode_e mode, bool enable)
{
    zx29_gpio_pin_priv_t *gpio_pin_priv = handle;

    if (enable) {
        switch (mode) {
            case GPIO_IRQ_MODE_LOW_LEVEL:
                csi_intc_set_attribute(gpio_pin_priv->irq, 0, INT_MODE_LOW_LEVEL);
                break;

            case GPIO_IRQ_MODE_HIGH_LEVEL:
                csi_intc_set_attribute(gpio_pin_priv->irq, 0, INT_MODE_HIGH_LEVEL);
                break;

            case GPIO_IRQ_MODE_RISING_EDGE:
                csi_intc_set_attribute(gpio_pin_priv->irq, 0, INT_MODE_RISING_EDGE);
                break;

            case GPIO_IRQ_MODE_FALLING_EDGE:
                csi_intc_set_attribute(gpio_pin_priv->irq, 0, INT_MODE_FALLING_EDGE);
                break;

            case GPIO_IRQ_MODE_DOUBLE_EDGE:
                csi_intc_set_attribute(gpio_pin_priv->irq, 0, INT_MODE_DOUBLE_EDGE);
                break;

            default:
                return ERR_GPIO(DRV_ERROR_PARAMETER);
        }

        csi_intc_enable_irq(gpio_pin_priv->irq);
    } else {
        csi_intc_disable_irq(gpio_pin_priv->irq);
    }

    return 0;
}

