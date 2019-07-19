/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_GPIO_SYSCALL_ARG_H
#define HAL_GPIO_SYSCALL_ARG_H

#include "aos/hal/gpio.h"

typedef struct {
    gpio_dev_t *gpio;
} hal_gpio_init_syscall_arg_t;

typedef struct {
    gpio_dev_t *gpio;
} hal_gpio_output_high_syscall_arg_t;

typedef struct {
    gpio_dev_t *gpio;
} hal_gpio_output_low_syscall_arg_t;

typedef struct {
    gpio_dev_t *gpio;
} hal_gpio_output_toggle_syscall_arg_t;

typedef struct {
    gpio_dev_t *gpio;
    uint32_t   *value;
} hal_gpio_input_get_syscall_arg_t;

typedef struct {
    gpio_dev_t         *gpio;
    gpio_irq_trigger_t  trigger;
    gpio_irq_handler_t  handler;
    void *arg;
} hal_gpio_enable_irq_syscall_arg_t;

typedef struct {
    gpio_dev_t *gpio;
} hal_gpio_disable_irq_syscall_arg_t;

typedef struct {
    gpio_dev_t *gpio;
} hal_gpio_clear_irq_syscall_arg_t;

typedef struct {
    gpio_dev_t *gpio;
} hal_gpio_finalize_syscall_arg_t;

#endif /* HAL_GPIO_SYSCALL_ARG_H */

