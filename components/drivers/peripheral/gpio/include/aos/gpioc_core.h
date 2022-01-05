/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_GPIOC_CORE_H
#define AOS_GPIOC_CORE_H

#include <aos/device_core.h>
#include <aos/gpioc.h>

struct aos_gpioc_ops;
struct aos_gpioc_pin;

typedef struct {
    aos_dev_t dev;
    aos_spinlock_t lock;

    /* must be initialized before registration */
    const struct aos_gpioc_ops *ops;
    uint32_t num_pins;
    struct aos_gpioc_pin *pins;
} aos_gpioc_t;

typedef struct aos_gpioc_ops {
    void (*unregister)(aos_gpioc_t *);
    aos_status_t (*set_mode)(aos_gpioc_t *, uint32_t);
    void (*enable_irq)(aos_gpioc_t *, uint32_t);
    void (*disable_irq)(aos_gpioc_t *, uint32_t);
    int (*get_value)(aos_gpioc_t *, uint32_t);
    void (*set_value)(aos_gpioc_t *, uint32_t);
} aos_gpioc_ops_t;

typedef struct aos_gpioc_pin {
    aos_gpioc_t *gpioc;
    uint32_t mode;
    aos_gpio_irq_handler_t irq_handler;
    void *irq_arg;
    aos_sem_t irq_sem;
    aos_event_t irq_event;
    aos_task_t irq_task;
    aos_task_t irq_new_task;
    bool hard_irq_en;
    int value;
} aos_gpioc_pin_t;

#define aos_gpioc_hard_irq_handler(gpioc, pin, polarity) \
    do { \
        aos_gpioc_t *_gpioc = (gpioc); \
        uint32_t _pin; \
        uint32_t mode; \
        uint32_t trig; \
        uint32_t mask; \
        uint32_t val; \
        aos_irqsave_t flags; \
        flags = aos_spin_lock_irqsave(&_gpioc->lock); \
        _pin = (pin); \
        if (!_gpioc->pins[_pin].hard_irq_en) { \
            aos_spin_unlock_irqrestore(&_gpioc->lock, flags); \
            break; \
        } \
        mode = _gpioc->pins[_pin].mode; \
        if ((mode & AOS_GPIO_DIR_MASK) == AOS_GPIO_DIR_INPUT) \
            trig = mode & AOS_GPIO_IRQ_TRIG_MASK; \
        else \
            trig = AOS_GPIO_IRQ_TRIG_NONE; \
        switch (trig) { \
        case AOS_GPIO_IRQ_TRIG_EDGE_RISING: \
        case AOS_GPIO_IRQ_TRIG_EDGE_FALLING: \
        case AOS_GPIO_IRQ_TRIG_EDGE_BOTH: \
            if (trig == AOS_GPIO_IRQ_TRIG_EDGE_RISING) \
                mask = 0x2; \
            else if (trig == AOS_GPIO_IRQ_TRIG_EDGE_FALLING) \
                mask = 0x1; \
            else \
                mask = (polarity) ? 0x2 : 0x1; \
            if (aos_event_get(&_gpioc->pins[_pin].irq_event, 0xFFFFFFFF, AOS_EVENT_OR, &val, AOS_NO_WAIT)) \
                val = 0; \
            for (int i = 0; i < 16; i++) { \
                if (!(val & ((uint32_t)0x3 << (2 * i)))) { \
                    (void)aos_event_set(&_gpioc->pins[_pin].irq_event, mask << (2 * i), AOS_EVENT_OR); \
                    break; \
                } \
            } \
            break; \
        case AOS_GPIO_IRQ_TRIG_LEVEL_HIGH: \
        case AOS_GPIO_IRQ_TRIG_LEVEL_LOW: \
            _gpioc->pins[_pin].hard_irq_en = false; \
            _gpioc->ops->disable_irq(_gpioc, _pin); \
            mask = (trig == AOS_GPIO_IRQ_TRIG_LEVEL_HIGH) ? 0x2 : 0x1; \
            (void)aos_event_set(&_gpioc->pins[_pin].irq_event, mask, AOS_EVENT_OR); \
            break; \
        default: \
            break; \
        } \
        aos_spin_unlock_irqrestore(&_gpioc->lock, flags); \
    } while (0)

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_gpioc_register(aos_gpioc_t *gpioc);
aos_status_t aos_gpioc_register_argumented(aos_gpioc_t *gpioc, uint32_t id, const aos_gpioc_ops_t *ops,
                                           uint32_t num_pins, aos_gpioc_pin_t *pins);
aos_status_t aos_gpioc_unregister(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* AOS_GPIOC_CORE_H */
