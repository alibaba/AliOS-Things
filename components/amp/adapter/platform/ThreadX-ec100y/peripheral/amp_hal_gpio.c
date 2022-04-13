/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "amp_defines.h"
#include "ql_gpio.h"
#include "amp_hal_gpio.h"


#define MOD_STR		"GPIO"

int32_t amp_hal_gpio_init(gpio_dev_t *gpio)
{
    int32_t ret = -1;
    PIN_DIRECTION_E pin_dir;
    PIN_PULL_E pin_pull;
    PIN_LEVEL_E pin_level;

    if (gpio == NULL) {
        amp_error(MOD_STR, "gpio is null  \n");
        return ret;
    }

    if (gpio->port >= GPIO_PIN_NO_MAX) {
        amp_error(MOD_STR, "gpio port is invalid \n");
        return ret;
    }
    amp_debug(MOD_STR, "gpio config %d\n", gpio->config);
    switch(gpio->config) {
        case IRQ_MODE:
            pin_dir  = PIN_DIRECTION_IN;
            pin_pull = PIN_PULL_PU;
            break;
        case INPUT_PULL_UP:
            pin_dir  = PIN_DIRECTION_IN;
            pin_pull = PIN_PULL_PU;
            break;
        case INPUT_PULL_DOWN:
            pin_dir  = PIN_DIRECTION_IN;
            pin_pull = PIN_PULL_PD;
            break;
        case INPUT_HIGH_IMPEDANCE:
            pin_dir  = PIN_DIRECTION_IN;
            break;
        case OUTPUT_PUSH_PULL:
        case OUTPUT_OPEN_DRAIN_NO_PULL:
        case OUTPUT_OPEN_DRAIN_PULL_UP:
            pin_dir  = PIN_DIRECTION_OUT;
            pin_pull = PIN_PULL_PU;
            break;
        default: break;
    }
    amp_debug(MOD_STR, "gpio init %d %d %d\n", gpio->port, pin_dir, pin_pull);
    ret = ql_gpio_init(gpio->port, pin_dir, pin_pull, PIN_LEVEL_LOW);
    if (ret != 0) {
        amp_error(MOD_STR, "gpio port init failed! \n");
        return ret;
    }

    return ret;
}

int32_t amp_hal_gpio_output_high(gpio_dev_t *gpio)
{
    int32_t ret = -1;

    if (gpio == NULL) {
        amp_error(MOD_STR, "gpio is null  \n");
        return ret;
    }
    // ret = ql_gpio_set_direction(gpio->port, PIN_DIRECTION_OUT);
    // if (ret != 0) {
    //     amp_error(MOD_STR, "gpio pin direction set failed! \n");
    //     return ret;
    // }
    ret = ql_gpio_set_level(gpio->port, PIN_LEVEL_HIGH);
    if (ret != 0) {
        amp_error(MOD_STR, "gpio pin level set failed! \n");
        return ret;
    }
    return ret;
}

int32_t amp_hal_gpio_output_low(gpio_dev_t *gpio)
{
    int32_t ret = -1;

    if (gpio == NULL) {
        amp_error(MOD_STR, "gpio is null  \n");
        return ret;
    }
    // ret = ql_gpio_set_direction(gpio->port, PIN_DIRECTION_OUT);
    // if (ret != 0) {
    //     amp_error(MOD_STR, "gpio pin direction set failed! \n");
    //     return ret;
    // }
    ret = ql_gpio_set_level(gpio->port, PIN_LEVEL_LOW);
    if (ret != 0) {
        amp_error(MOD_STR, "gpio pin level set failed! \n");
        return ret;
    }
    return ret;
}

int32_t amp_hal_gpio_output_toggle(gpio_dev_t *gpio)
{
    int32_t val,ret = -1;

    if (gpio == NULL) {
        amp_error(MOD_STR, "gpio is null  \n");
        return ret;
    }

    ret = ql_gpio_get_level(gpio->port, val);
    if (ret != 0) {
        amp_error(MOD_STR, "gpio pin level get failed! \n");
        return ret;
    }
    val ^= 1;
    // ret = ql_gpio_set_direction(gpio->port, PIN_DIRECTION_OUT);
    // if (ret != 0) {
    //     amp_error(MOD_STR, "gpio pin direction set failed! \n");
    //     return ret;
    // }
    ret = ql_gpio_set_level(gpio->port, val);
    if (ret != 0) {
        amp_error(MOD_STR, "gpio pin level set failed! \n");
        return ret;
    }

    return ret;
}

int32_t amp_hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    int32_t ret = -1;

    if(NULL != gpio && NULL != value) {
        // ret = ql_gpio_set_direction(gpio->port, PIN_DIRECTION_IN);
        // if (ret != 0) {
        //     amp_error(MOD_STR, "gpio pin direction set failed! \n");
        //     return ret;
        // }
        ret = ql_gpio_get_level(gpio->port, value);
        if (ret != 0) {
            amp_error(MOD_STR, "gpio pin level get failed! \n");
            return ret;
        }
    }
    return ret;
}

int32_t amp_hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                            gpio_irq_handler_t handler, void *arg)
{
    int32_t ret = -1;
    PIN_EDGE_E	pin_edge;

    // ret = ql_gpio_set_direction(gpio->port, PIN_DIRECTION_IN);
    // if (ret != 0) {
    //     amp_error(MOD_STR, "gpio pin direction set failed! \n");
    //     return ret;
    // }
    // ql_eint_register(gpio->port, (unsigned int)handler, (unsigned int)arg);
    switch(trigger) {
        case IRQ_TRIGGER_RISING_EDGE:
            pin_edge = PIN_RISING_EDGE;
            break;
        case IRQ_TRIGGER_FALLING_EDGE:
            pin_edge = PIN_FALLING_EDGE;
            break;
        case IRQ_TRIGGER_BOTH_EDGES:
            pin_edge = PIN_BOTH_EDGE;
            break;
        default: break;
    }
    
    ret = ql_eint_register(gpio->port, pin_edge, PIN_PULL_PU, handler, NULL);
    if (ret != 0) {
        amp_error(MOD_STR, "gpio external interrupt register failed! \n");
        return ret;
    }
    ret = ql_eint_enable(gpio->port, pin_edge);
    if (ret != 0) {
        amp_error(MOD_STR, "gpio external interrupt enable failed!\n");
        return ret;
    }
    return ret;
}

int32_t amp_hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    int32_t ret = -1;

    ret = ql_eint_disable(gpio->port);
    if (ret != 0) {
        return ret;
    }
    return 0;
}

int32_t amp_hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    return 0;
}

int32_t amp_hal_gpio_finalize(gpio_dev_t *gpio)
{
    int32_t ret = -1;

    ret = ql_gpio_deinit(gpio->port);
    if (ret != 0) {
        return ret;
    }
    return ret;
}

