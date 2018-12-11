/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/hal/gpio.h"

#ifdef HAL_GPIO_MODULE_ENABLED

int32_t hal_gpio_init(gpio_dev_t *gpio)
{

}

int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{

}

int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{

}

int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{

}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{

}

int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,gpio_irq_handler_t handler, void *arg)
{

}

int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{

}

int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{

}

int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{

}

#endif /* HAL_GPIO_MODULE_ENABLED */

