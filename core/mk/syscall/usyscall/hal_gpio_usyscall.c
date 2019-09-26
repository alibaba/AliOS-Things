/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "k_config.h"
#include "syscall.h"
#include "hal_gpio_syscall_arg.h"
#include "syscall_no.h"

int32_t hal_gpio_init(gpio_dev_t *gpio)
{
    hal_gpio_init_syscall_arg_t _arg;

    _arg.gpio = gpio;

    return (int32_t)SYSCALL(SYS_HAL_GPIO_INIT, (void*)&_arg);
}

int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
    hal_gpio_output_high_syscall_arg_t _arg;

    _arg.gpio = gpio;

    return (int32_t)SYSCALL(SYS_HAL_GPIO_OUTPUT_HIGH, (void*)&_arg);
}

int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
     hal_gpio_output_low_syscall_arg_t _arg;

    _arg.gpio = gpio;

    return (int32_t)SYSCALL(SYS_HAL_GPIO_OUTPUT_LOW, (void*)&_arg);

}

int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
    hal_gpio_output_toggle_syscall_arg_t _arg;

    _arg.gpio = gpio;

    return (int32_t)SYSCALL(SYS_HAL_GPIO_OUTPUT_TOGGLE, (void*)&_arg);

}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    hal_gpio_input_get_syscall_arg_t _arg;

    _arg.gpio  = gpio;
    _arg.value = value;

    return (int32_t)SYSCALL(SYS_HAL_GPIO_INPUT_GET, (void*)&_arg);
}

int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                            gpio_irq_handler_t handler, void *arg)
{
    hal_gpio_enable_irq_syscall_arg_t _arg;

    _arg.gpio    = gpio;
    _arg.trigger = trigger;
    _arg.handler = handler;
    _arg.arg     = arg;

    return (int32_t)SYSCALL(SYS_HAL_GPIO_ENABLE_IRQ, (void*)&_arg);
}
int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    hal_gpio_disable_irq_syscall_arg_t _arg;

    _arg.gpio = gpio;

    return (int32_t)SYSCALL(SYS_HAL_GPIO_DISABLE_IRQ, (void*)&_arg);
}
int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    hal_gpio_clear_irq_syscall_arg_t _arg;

    _arg.gpio = gpio;

    return (int32_t)SYSCALL(SYS_HAL_GPIO_CLEAR_IRQ, (void*)&_arg);
}
int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{
    hal_gpio_clear_irq_syscall_arg_t _arg;

    _arg.gpio = gpio;

    return (int32_t)SYSCALL(SYS_HAL_GPIO_FINALIZE, (void*)&_arg);
}

