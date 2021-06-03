/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_GPIOC_H
#define AOS_GPIOC_H

#include <aos/device_core.h>
#include <aos/gpio.h>

struct aos_gpioc;

typedef struct {
    void (*unregister)(struct aos_gpioc *);
    aos_status_t (*set_mode)(struct aos_gpioc *, uint32_t);
    int (*get_value)(struct aos_gpioc *, uint32_t);
    void (*set_value)(struct aos_gpioc *, uint32_t);
} aos_gpioc_ops_t;

typedef struct {
    uint32_t mode;
    aos_gpio_irq_handler_t irq_handler;
    void *irq_arg;
    aos_sem_t irq_sem;
    aos_event_t irq_event;
    aos_task_t irq_task;
    int value;
} aos_gpioc_pin_t;

typedef struct aos_gpioc {
    aos_dev_t dev;
    const aos_gpioc_ops_t *ops;
    uint32_t num_pins;
    aos_spinlock_t lock;
    aos_gpioc_pin_t pins[0];
} aos_gpioc_t;

typedef aos_dev_ref_t aos_gpioc_ref_t;

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_gpioc_register(aos_gpioc_t *gpioc);
aos_status_t aos_gpioc_unregister(uint32_t id);
/**
 * @brief       Get a GPIO controller device.
 * @param[out]  ref     GPIO controller ref to operate
 * @param[in]   id      GPIO controller device ID
 * @return      0: on success; < 0: on failure
 */
aos_status_t aos_gpioc_get(aos_gpioc_ref_t *ref, uint32_t id);
/**
 * @brief       Release a GPIO controller device.
 * @param[in]   ref     GPIO controller ref to operate
 * @return      None
 */
void aos_gpioc_put(aos_gpioc_ref_t *ref);
/**
 * @brief       Get number of pins on a GPIO controller device.
 * @param[in]   ref     GPIO controller ref to operate
 * @return      >= 0: on success; < 0: on failure
 */
aos_status_t aos_gpioc_get_num_pins(aos_gpioc_ref_t *ref);
/**
 * @brief       Get the parameters associated with a GPIO pin.
 * @param[in]   ref     GPIO controller ref to operate
 * @param[in]   pin     pin to operate
 * @param[out]  mode    pin mode
 * @return      0: on success; < 0: on failure
 */
aos_status_t
aos_gpioc_get_mode(aos_gpioc_ref_t *ref, uint32_t pin, uint32_t *mode);
/**
 * @brief       Set the parameters associated with a GPIO pin.
 * @param[in]   ref     GPIO controller ref to operate
 * @param[in]   pin     pin to operate
 * @param[in]   mode    pin mode
 * @return      0: on success; < 0: on failure
 */
aos_status_t
aos_gpioc_set_mode(aos_gpioc_ref_t *ref, uint32_t pin, uint32_t mode);
/**
 * @brief       Set the parameters and IRQ settings associated with a GPIO pin.
 * @param[in]   ref             GPIO controller ref to operate
 * @param[in]   pin             pin to operate
 * @param[in]   mode            pin mode
 * @param[in]   irq_handler     IRQ handler
 * @param[in]   irq_arg         argument passed to IRQ handler
 * @return      0: on success; < 0: on failure
 */
aos_status_t aos_gpioc_set_mode_irq(aos_gpioc_ref_t *ref,
                                    uint32_t pin, uint32_t mode,
                                    aos_gpio_irq_handler_t irq_handler,
                                    void *irq_arg);
/**
 * @brief       Get the input level of a GPIO pin.
 * @param[in]   ref     GPIO controller ref to operate
 * @param[in]   pin     pin to operate
 * @return      0: low level; > 0: high level; < 0: on failure
 */
aos_status_t aos_gpioc_get_value(aos_gpioc_ref_t *ref, uint32_t pin);
/**
 * @brief       Set the output level of a GPIO pin.
 * @param[in]   ref     GPIO controller ref to operate
 * @param[in]   pin     pin to operate
 * @param[in]   val     output level
 * @return      0: on success; < 0: on failure
 */
aos_status_t aos_gpioc_set_value(aos_gpioc_ref_t *ref, uint32_t pin, int val);
/**
 * @brief       Toggle the output level of a GPIO pin.
 * @param[in]   ref     GPIO controller ref to operate
 * @param[in]   pin     pin to operate
 * @return      0: on success; < 0: on failure
 */
aos_status_t aos_gpioc_toggle(aos_gpioc_ref_t *ref, uint32_t pin);

#ifdef __cplusplus
}
#endif

#endif /* AOS_GPIOC_H */
