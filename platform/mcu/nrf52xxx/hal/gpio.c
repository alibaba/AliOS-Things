/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include "aos/hal/gpio.h"
#include "nrf_gpio.h"

/*use port elem in gpio_dev_t as pin number for nrf52 soc*/

int32_t hal_gpio_init(gpio_dev_t *gpio)
{
    if (gpio->port > NUMBER_OF_PINS)
        return -1;

    // only set the dir according to the
    // input gpio pin configuration
    switch (gpio->config) {
    case INPUT_PULL_UP:
         nrf_gpio_cfg_input(gpio->port, NRF_GPIO_PIN_PULLUP);
         break;
    case INPUT_PULL_DOWN:
         nrf_gpio_cfg_input(gpio->port, NRF_GPIO_PIN_PULLDOWN);
         break;
    case INPUT_HIGH_IMPEDANCE:
         // high impedance not support
         nrf_gpio_cfg_input(gpio->port, NRF_GPIO_PIN_NOPULL);
         break;
    case OUTPUT_PUSH_PULL:
    case OUTPUT_OPEN_DRAIN_NO_PULL:
    case OUTPUT_OPEN_DRAIN_PULL_UP:
          nrf_gpio_cfg_output(gpio->port);
          break;
    default:
          break;
    }

    return 0;
}

int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{
    if (gpio->port > NUMBER_OF_PINS)
        return -1;

    nrf_gpio_cfg_default(gpio->port);
    return 0;
}

int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{

    if (gpio->port > NUMBER_OF_PINS)
        return -1;

    nrf_gpio_pin_set(gpio->port);
    return 0;
}

int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{

    if (gpio->port > NUMBER_OF_PINS)
        return -1;

    nrf_gpio_pin_clear(gpio->port);
    return 0;
}

int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{

    if (gpio->port > NUMBER_OF_PINS)
        return -1;

    nrf_gpio_pin_toggle(gpio->port);
    return 0;
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    if (gpio->port > NUMBER_OF_PINS)
        return -1;

    if (!value)
        return -1;

    *value = nrf_gpio_pin_read(gpio->port);
    return 0;
}

int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                            gpio_irq_handler_t handler, void *arg)
{
    return 0;
}

int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    return 0;
}

int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    return 0;
}
