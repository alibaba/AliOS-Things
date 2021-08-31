/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_GPIOC_CORE_H
#define AOS_GPIOC_CORE_H

#include <aos/device_core.h>
#include <aos/gpioc.h>

#define AOS_GPIOC_IRQ_EVENT_P   ((uint32_t)1 << 0)
#define AOS_GPIOC_IRQ_EVENT_N   ((uint32_t)1 << 1)

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

#define aos_gpioc_hard_irq_handler(gpioc, pin, polarity) \
    do { \
        aos_gpioc_t *_gpioc = (gpioc); \
        uint32_t _pin = (pin); \
        uint32_t mode; \
        uint32_t trig; \
        uint32_t mask; \
        aos_irqsave_t flags; \
        flags = aos_spin_lock_irqsave(&_gpioc->lock); \
        if (!_gpioc->pins[_pin].hard_irq_en) { \
            aos_spin_unlock_irqrestore(&_gpioc->lock, flags); \
            break; \
        } \
        mode = _gpioc->pins[_pin].mode; \
        if ((mode & AOS_GPIO_DIR_MASK) == AOS_GPIO_DIR_INPUT) \
            trig = mode & AOS_GPIO_IRQ_TRIG_MASK; \
        else \
            trig = AOS_GPIO_IRQ_TRIG_NONE; \
        if (trig == AOS_GPIO_IRQ_TRIG_LEVEL_HIGH || trig == AOS_GPIO_IRQ_TRIG_LEVEL_LOW) { \
            _gpioc->pins[_pin].hard_irq_en = false; \
            _gpioc->ops->disable_irq(_gpioc, _pin); \
        } \
        if (trig == AOS_GPIO_IRQ_TRIG_EDGE_BOTH) { \
            mask = (polarity) ? AOS_GPIOC_IRQ_EVENT_P : AOS_GPIOC_IRQ_EVENT_N; \
        } else if (trig == AOS_GPIO_IRQ_TRIG_EDGE_RISING || trig == AOS_GPIO_IRQ_TRIG_LEVEL_HIGH) { \
            mask = AOS_GPIOC_IRQ_EVENT_P; \
        } else if (trig == AOS_GPIO_IRQ_TRIG_EDGE_FALLING || trig == AOS_GPIO_IRQ_TRIG_LEVEL_LOW) { \
            mask = AOS_GPIOC_IRQ_EVENT_N; \
        } else { \
            mask = 0; \
        } \
        if (mask) \
            aos_event_set(&_gpioc->pins[pin].irq_event, mask, AOS_EVENT_OR); \
        aos_spin_unlock_irqrestore(&_gpioc->lock, flags); \
    } while (0)

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_gpioc_register(aos_gpioc_t *gpioc);
aos_status_t aos_gpioc_unregister(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* AOS_GPIOC_CORE_H */
