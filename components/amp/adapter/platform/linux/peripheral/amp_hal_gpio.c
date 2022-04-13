/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "amp_hal_gpio.h"

int32_t amp_hal_gpio_init(gpio_dev_t *gpio)
{
    return 0;
}

int32_t amp_hal_gpio_output_high(gpio_dev_t *gpio)
{
    return 0;
}

int32_t amp_hal_gpio_output_low(gpio_dev_t *gpio)
{
    return 0;
}

int32_t amp_hal_gpio_output_toggle(gpio_dev_t *gpio)
{
    return 0;
}

int32_t amp_hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    return 0;
}

int32_t amp_hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                            gpio_irq_handler_t handler, void *arg)
{
    return 0;
}

int32_t amp_hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    return 0;
}

int32_t amp_hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    return 0;
}

int32_t amp_hal_gpio_finalize(gpio_dev_t *gpio)
{
    return 0;
}

