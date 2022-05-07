/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#ifndef AOS_BOARD_HAAS700
#endif
#include "aos_hal_gpio.h"

#ifdef CONFIG_GPIO_NUM
#define PLATFORM_GPIO_NUM CONFIG_GPIO_NUM
#else
#define PLATFORM_GPIO_NUM 40
#endif


int32_t aos_hal_gpio_init(gpio_dev_t *gpio)
{
    if (!gpio || gpio->port >= PLATFORM_GPIO_NUM)
        return -1;

    if (gpio->gpioc != NULL) {
        LOGE("GPIO_HAL", "gpio has inited;\n");
        return -1;
    }

    uint32_t mode;
    uint32_t ret;

    aos_gpioc_ref_t *gpioc;
    gpioc = aos_malloc(sizeof(aos_gpioc_ref_t));
    memset(gpioc, 0, sizeof(gpioc));
    int32_t pin_index = gpio->port;
    int32_t gpioc_index = 0;

    while (pin_index > 0) {
        ret = aos_gpioc_get(gpioc, gpioc_index);
        if (ret) {
            LOGE("GPIO_HAL", "get gpioc failed,  gpioc_index = %d, ret = %d;\n", gpioc_index, ret);
            aos_free(gpioc);
            gpioc = NULL;
            return ret;
        }

        ret = aos_gpioc_get_num_pins(gpioc);
        if (ret <= 0) {
            LOGE("GPIO_HAL", "get gpioc pin num failed, %d;\n", ret);
            aos_free(gpioc);
            gpioc = NULL;
            return ret;
        }
        if (ret <= pin_index) {
            gpioc_index++;
            pin_index -= ret;
            aos_gpioc_put(gpioc);
        } else {
            break;
        }
    }

    mode = AOS_GPIO_DIR_OUTPUT;
    mode |= AOS_GPIO_OUTPUT_CFG_DEFAULT;

    gpio->gpioc = gpioc;
    gpio->pin_index = pin_index;
    gpio->gpioc_index = gpioc_index;

    switch (gpio->config) {
    /**< Used as a function pin, input and output analog */
    case ANALOG_MODE:
        break;
    /**< Used to trigger interrupt */
    case IRQ_MODE:
        mode = AOS_GPIO_DIR_INPUT;
        return 0;
        break;
    /**< Input with an internal pull-up resistor - use with devices
     * that actively drive the signal low - e.g. button connected to ground */
    case INPUT_PULL_UP:
        mode = AOS_GPIO_DIR_INPUT;
        mode |= AOS_GPIO_INPUT_CFG_PU;
        break;
    /**< Input with an internal pull-down resistor - use with devices
     * that actively drive the signal high - e.g. button connected to a power rail */
    case INPUT_PULL_DOWN:
        mode = AOS_GPIO_DIR_INPUT;
        mode |= AOS_GPIO_INPUT_CFG_PD;
        break;
    /**< Input - must always be driven, either actively or by an external pullup resistor */
    case INPUT_HIGH_IMPEDANCE:
        mode = AOS_GPIO_DIR_INPUT;
        mode |= AOS_GPIO_INPUT_CFG_HI;
        break;
    /**< Output actively driven high and actively driven low -
     * must not be connected to other active outputs - e.g. LED output */
    case OUTPUT_PUSH_PULL:
        mode = AOS_GPIO_DIR_OUTPUT;
        mode |= AOS_GPIO_OUTPUT_CFG_PP;
        break;
    /**< Output actively driven low but is high-impedance when set high -
     * can be connected to other open-drain/open-collector outputs. Needs an external
     * pull-up resistor */
    case OUTPUT_OPEN_DRAIN_NO_PULL:
        mode = AOS_GPIO_DIR_OUTPUT;
        mode |= AOS_GPIO_OUTPUT_CFG_ODNP;
        break;
    /**< Output actively driven low and is pulled high with an internal resistor when set
     * high can be connected to other open-drain/open-collector outputs. */
    case OUTPUT_OPEN_DRAIN_PULL_UP:
        mode = AOS_GPIO_DIR_OUTPUT;
        mode |= AOS_GPIO_OUTPUT_CFG_ODPU;
        break;
    /**< Alternate Function Open Drain Mode. */
    case OUTPUT_OPEN_DRAIN_AF:
        break;
    /**< Alternate Function Push Pull Mode. */
    case OUTPUT_PUSH_PULL_AF:
    default:
        break;
    }

    ret = aos_gpioc_set_mode(gpioc, pin_index, mode);
    if (ret) {
        LOGE("GPIO_HAL", "%s: set gpio mode failed, %d;\n", __func__, ret);
        aos_gpioc_put(&gpioc);
        aos_free(gpioc);
        gpioc = NULL;
        return ret;
    }

    return 0;
}

int32_t aos_hal_gpio_output_high(gpio_dev_t *gpio)
{
#ifndef AOS_BOARD_HAAS700
    if (!gpio || gpio->port >= PLATFORM_GPIO_NUM)
        return -1;

    return aos_gpioc_set_value(gpio->gpioc, gpio->pin_index, 1);

#else
    return -1;
#endif
}

int32_t aos_hal_gpio_output_low(gpio_dev_t *gpio)
{
#ifndef AOS_BOARD_HAAS700
    if (!gpio || gpio->port >= PLATFORM_GPIO_NUM)
        return -1;

    return aos_gpioc_set_value(gpio->gpioc, gpio->pin_index, 0);

#else
    return -1;
#endif
}

int32_t aos_hal_gpio_output_toggle(gpio_dev_t *gpio)
{
#ifndef AOS_BOARD_HAAS700
    return aos_gpioc_toggle(gpio->gpioc, gpio->pin_index);

#else
    return -1;
#endif
}


int32_t aos_hal_gpio_get(gpio_dev_t *gpio)
{
    return aos_gpioc_get_value(gpio->gpioc, gpio->pin_index);
}


int32_t aos_hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
#ifndef AOS_BOARD_HAAS700
    int ret = -1;
    ret = aos_gpioc_get_value(gpio->gpioc, gpio->pin_index);
    *value = ret > 0 ? 1 : 0;
    return ret;
#else
    return -1;
#endif
}

int32_t aos_hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                                aos_gpio_irq_handler_t handler, void *arg)
{
#ifndef AOS_BOARD_HAAS700

    uint32_t ret = -1;
    uint32_t mode = 0;

    mode = AOS_GPIO_DIR_INPUT;

    switch (trigger) {
    case IRQ_TRIGGER_RISING_EDGE:
        mode |= AOS_GPIO_IRQ_TRIG_EDGE_RISING;
        break;
    case IRQ_TRIGGER_FALLING_EDGE:
        mode |= AOS_GPIO_IRQ_TRIG_EDGE_FALLING;
        break;
    case IRQ_TRIGGER_LEVEL_HIGH:
        mode |= AOS_GPIO_IRQ_TRIG_LEVEL_HIGH;
        break;
    case IRQ_TRIGGER_LEVEL_LOW:
        mode |= AOS_GPIO_IRQ_TRIG_LEVEL_LOW;
        break;
    case IRQ_TRIGGER_BOTH_EDGES:
        mode |= AOS_GPIO_IRQ_TRIG_EDGE_BOTH;
        break;
    default:
        break;
    }

    switch (gpio->config) {
    case INPUT_PULL_UP:
        mode |= AOS_GPIO_INPUT_CFG_PU;
        break;
    /**< Input with an internal pull-down resistor - use with devices
     * that actively drive the signal high - e.g. button connected to a power rail */
    case INPUT_PULL_DOWN:
        mode |= AOS_GPIO_INPUT_CFG_PD;
        break;
    /**< Input - must always be driven, either actively or by an external pullup resistor */
    case INPUT_HIGH_IMPEDANCE:
        mode |= AOS_GPIO_INPUT_CFG_HI;
        break;
    default:
        mode |= AOS_GPIO_INPUT_CFG_DEFAULT;
    }

    LOGE("GPIO_HAL", "aos_gpioc_set_mode_irq mode =%x, gpioc_index=%d, pin_index=%d\n", mode, gpio->gpioc_index, gpio->pin_index);
    ret = aos_gpioc_set_mode_irq(gpio->gpioc, gpio->pin_index, mode, handler, arg);
    if (ret) {
        LOGE("GPIO_HAL", "aos_gpioc_set_mode_irq failed %d\n", ret);
    }
    return ret;
#else
    return -1;
#endif
}

int32_t aos_hal_gpio_disable_irq(gpio_dev_t *gpio)
{
#ifndef AOS_BOARD_HAAS700
    // gpio_irq_config_t config;
    // int r;

    // if (!gpio || gpio->port >= PLATFORM_GPIO_NUM)
    //     return -1;
    // config.id = gpio->port;
    // config.config = GPIO_IRQ_DISABLE;
    // config.cb = NULL;
    // config.arg = NULL;
    // r = ioctl(gpio->fd, IOC_GPIO_SET_IRQ, (unsigned long)&config);

    // return r < 0 ? -1 : 0;
    return 0;
#else
    return -1;
#endif
}

int32_t aos_hal_gpio_clear_irq(gpio_dev_t *gpio)
{
#ifndef AOS_BOARD_HAAS700
    return aos_hal_gpio_disable_irq(gpio);
#else
    return -1;
#endif
}

int32_t aos_hal_gpio_finalize(gpio_dev_t *gpio)
{
    if (!gpio || gpio->port >= PLATFORM_GPIO_NUM)
        return -1;
    aos_gpioc_put(gpio->gpioc);
    aos_free(gpio->gpioc);
    gpio->gpioc = NULL;
    return 0;
}
