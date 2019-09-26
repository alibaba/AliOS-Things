/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_config.h"

#if (RHINO_CONFIG_HAL_GPIO_SYSCALL > 0)

#include <stdint.h>
#include "aos/hal/gpio.h"
#include "hal_gpio_syscall_arg.h"

uint32_t sys_hal_gpio_init_stub(void *arg)
{
    hal_gpio_init_syscall_arg_t *_arg = arg;

    return hal_gpio_init(_arg->gpio);
}

uint32_t sys_hal_gpio_output_high_stub(void *arg)
{
    hal_gpio_output_high_syscall_arg_t *_arg = arg;

    return hal_gpio_output_high(_arg->gpio);
}

uint32_t sys_hal_gpio_output_low_stub(void *arg)
{
    hal_gpio_output_low_syscall_arg_t *_arg = arg;

    return hal_gpio_output_low(_arg->gpio);
}

uint32_t sys_hal_gpio_output_toggle_stub(void *arg)
{
    hal_gpio_output_toggle_syscall_arg_t *_arg = arg;

    return hal_gpio_output_toggle(_arg->gpio);
}

uint32_t sys_hal_gpio_input_get_stub(void *arg)
{
    hal_gpio_input_get_syscall_arg_t *_arg = arg;

    return hal_gpio_input_get(_arg->gpio, _arg->value);
}

uint32_t sys_hal_gpio_enable_irq_stub(void *arg)
{
    hal_gpio_enable_irq_syscall_arg_t *_arg = arg;

    return hal_gpio_enable_irq(_arg->gpio, _arg->trigger,
                               _arg->handler, _arg->arg);
}
uint32_t sys_hal_gpio_disable_irq_stub(void *arg)
{
    hal_gpio_disable_irq_syscall_arg_t *_arg = arg;

    return hal_gpio_disable_irq(_arg->gpio);
}

uint32_t sys_hal_gpio_clear_irq_stub(void *arg)
{
    hal_gpio_clear_irq_syscall_arg_t *_arg = arg;

    return hal_gpio_clear_irq(_arg->gpio);
}

uint32_t sys_hal_gpio_finialize_stub(void *arg)
{
    hal_gpio_finalize_syscall_arg_t *_arg = arg;

    return hal_gpio_finalize(_arg->gpio);
}

#endif /* RHINO_CONFIG_HAL_GPIO_SYSCALL */

