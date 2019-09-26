/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_GPIO_KSYSCALL_H
#define HAL_GPIO_KSYSCALL_H

uint32_t sys_hal_gpio_init_stub(void *arg);

uint32_t sys_hal_gpio_output_high_stub(void *arg);

uint32_t sys_hal_gpio_output_low_stub(void *arg);

uint32_t sys_hal_gpio_output_toggle_stub(void *arg);

uint32_t sys_hal_gpio_input_get_stub(void *arg);

uint32_t sys_hal_gpio_enable_irq_stub(void *arg);

uint32_t sys_hal_gpio_disable_irq_stub(void *arg);

uint32_t sys_hal_gpio_clear_irq_stub(void *arg);

uint32_t sys_hal_gpio_finialize_stub(void *arg);

#endif/* HAL_GPIO_KSYSCALL_H */

