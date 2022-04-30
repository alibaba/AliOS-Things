/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_GPIO_CORE_H
#define AOS_GPIO_CORE_H

#include <aos/device_core.h>
#include <aos/gpioc.h>
#include <aos/gpio.h>

typedef struct {
    aos_dev_t dev;
    aos_gpioc_ref_t gpioc_ref;

    /* must be initialized before registration */
    uint32_t gpioc_id;
    uint32_t pin;
} aos_gpio_t;

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_gpio_register(aos_gpio_t *gpio);
aos_status_t aos_gpio_register_argumented(aos_gpio_t *gpio, uint32_t id, uint32_t gpioc_id, uint32_t pin);
aos_status_t aos_gpio_unregister(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* AOS_GPIO_CORE_H */
