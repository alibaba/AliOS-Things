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
    void (*enable_irq)(struct aos_gpioc *, uint32_t);
    void (*disable_irq)(struct aos_gpioc *, uint32_t);
    int (*get_value)(struct aos_gpioc *, uint32_t);
    void (*set_value)(struct aos_gpioc *, uint32_t);
} aos_gpioc_ops_t;

typedef struct {
    uint32_t id;
    uint32_t mode;
    aos_gpio_irq_handler_t irq_handler;
    void *irq_arg;
    aos_sem_t irq_sem;
    aos_event_t irq_event;
    aos_task_t irq_task;
    bool hard_irq_en;
    int value;
} aos_gpioc_pin_t;

typedef struct aos_gpioc {
    aos_dev_t dev;
    aos_spinlock_t lock;

    /* must be initialized before registration */
    const aos_gpioc_ops_t *ops;
    uint32_t num_pins;

    aos_gpioc_pin_t pins[0];
} aos_gpioc_t;

typedef aos_dev_ref_t aos_gpioc_ref_t;

#define AOS_GPIOC_IRQ_EVENT_P   ((uint32_t)1 << 0)
#define AOS_GPIOC_IRQ_EVENT_N   ((uint32_t)1 << 1)

#define aos_gpioc_hard_irq_handler(gpioc, pin, polarity) \
    do { \
        uint32_t mode; \
        uint32_t trig; \
        uint32_t mask; \
        aos_irqsave_t flags; \
        flags = aos_spin_lock_irqsave(&(gpioc)->lock); \
        if (!(gpioc)->pins[pin].hard_irq_en) { \
            aos_spin_unlock_irqrestore(&(gpioc)->lock, flags); \
            break; \
        } \
        mode = (gpioc)->pins[pin].mode; \
        if ((mode & AOS_GPIO_DIR_MASK) == AOS_GPIO_DIR_INPUT) \
            trig = mode & AOS_GPIO_IRQ_TRIG_MASK; \
        else \
            trig = AOS_GPIO_IRQ_TRIG_NONE; \
        if (trig == AOS_GPIO_IRQ_TRIG_LEVEL_HIGH || \
            trig == AOS_GPIO_IRQ_TRIG_LEVEL_LOW) { \
            (gpioc)->pins[pin].hard_irq_en = false; \
            (gpioc)->ops->disable_irq(gpioc, pin); \
        } \
        if (trig == AOS_GPIO_IRQ_TRIG_EDGE_BOTH) { \
            if (polarity) \
                mask = AOS_GPIOC_IRQ_EVENT_P; \
            else \
                mask = AOS_GPIOC_IRQ_EVENT_N; \
        } else if (trig == AOS_GPIO_IRQ_TRIG_EDGE_RISING || \
                   trig == AOS_GPIO_IRQ_TRIG_LEVEL_HIGH) { \
            mask = AOS_GPIOC_IRQ_EVENT_P; \
        } else if (trig == AOS_GPIO_IRQ_TRIG_EDGE_FALLING || \
                   trig == AOS_GPIO_IRQ_TRIG_LEVEL_LOW) { \
            mask = AOS_GPIOC_IRQ_EVENT_N; \
        } else { \
            mask = 0; \
        } \
        if (mask) \
            aos_event_set(&(gpioc)->pins[pin].irq_event, mask, AOS_EVENT_OR); \
        aos_spin_unlock_irqrestore(&(gpioc)->lock, flags); \
    } while (0)

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
 * @brief       Get the input or output level of a GPIO pin.
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
