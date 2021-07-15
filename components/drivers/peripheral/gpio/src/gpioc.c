/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <aos/gpioc.h>

#define IRQ_TASK_STACK_SIZE     4096
#define IRQ_TASK_DEFAULT_PRIO   30

static uint32_t mode_to_irq_trig(uint32_t mode)
{
    uint32_t trig;

    if ((mode & AOS_GPIO_DIR_MASK) == AOS_GPIO_DIR_INPUT)
        trig = mode & AOS_GPIO_IRQ_TRIG_MASK;
    else
        trig = AOS_GPIO_IRQ_TRIG_NONE;

    return trig;
}

static void dev_gpioc_unregister(aos_dev_t *dev)
{
    aos_gpioc_t *gpioc = aos_container_of(dev, aos_gpioc_t, dev);
    uint32_t i;

    for (i = 0; i < gpioc->num_pins; i++) {
        aos_gpioc_pin_t *pin = &gpioc->pins[i];

        if (mode_to_irq_trig(pin->mode) != AOS_GPIO_IRQ_TRIG_NONE) {
            aos_irqsave_t flags;

            /* no deadlock here */
            (void)aos_sem_wait(&pin->irq_sem, AOS_WAIT_FOREVER);
            (void)aos_task_delete(&pin->irq_task);
            flags = aos_spin_lock_irqsave(&gpioc->lock);

            if (pin->hard_irq_en) {
                pin->hard_irq_en = false;
                gpioc->ops->disable_irq(gpioc, i);
            }

            aos_spin_unlock_irqrestore(&gpioc->lock, flags);
        }

        pin->mode = AOS_GPIO_DIR_NONE;
        (void)gpioc->ops->set_mode(gpioc, i);
        aos_sem_free(&pin->irq_sem);
        aos_event_free(&pin->irq_event);
    }

    if (gpioc->ops->unregister)
        gpioc->ops->unregister(gpioc);
}

static aos_status_t dev_gpioc_get(aos_dev_ref_t *ref)
{
    return 0;
}

static void dev_gpioc_put(aos_dev_ref_t *ref)
{
}

static const aos_dev_ops_t dev_gpioc_ops = {
    .unregister = dev_gpioc_unregister,
    .get        = dev_gpioc_get,
    .put        = dev_gpioc_put,
};

aos_status_t aos_gpioc_register(aos_gpioc_t *gpioc)
{
    aos_status_t ret;
    uint32_t i;

    if (!gpioc)
        return -EINVAL;

    gpioc->dev.type = AOS_DEV_TYPE_GPIOC;
    gpioc->dev.ops = &dev_gpioc_ops;
#ifdef AOS_COMP_VFS
    gpioc->dev.vfs_helper.name[0] = '\0';
    gpioc->dev.vfs_helper.ops = NULL;
#endif
    aos_spin_lock_init(&gpioc->lock);

    for (i = 0; i < gpioc->num_pins; i++) {
        aos_gpioc_pin_t *pin = &gpioc->pins[i];

        pin->id = i;
        pin->mode = AOS_GPIO_DIR_NONE;
        pin->irq_handler = NULL;
        pin->irq_arg = NULL;
        pin->hard_irq_en = false;
        pin->value = 0;

        ret = aos_sem_new(&pin->irq_sem, 1);
        if (ret) {
            uint32_t j;

            for (j = 0; j < i; j++) {
                aos_sem_free(&gpioc->pins[j].irq_sem);
                aos_event_free(&gpioc->pins[j].irq_event);
            }

            return ret;
        }

        ret = aos_event_new(&pin->irq_event, 0);
        if (ret) {
            uint32_t j;

            aos_sem_free(&gpioc->pins[i].irq_sem);

            for (j = 0; j < i; j++) {
                aos_sem_free(&gpioc->pins[j].irq_sem);
                aos_event_free(&gpioc->pins[j].irq_event);
            }

            return ret;
        }

    }

    ret = aos_dev_register(&gpioc->dev);
    if (ret) {
        for (i = 0; i < gpioc->num_pins; i++) {
            aos_sem_free(&gpioc->pins[i].irq_sem);
            aos_event_free(&gpioc->pins[i].irq_event);
        }

        return ret;
    }

    return 0;
}

aos_status_t aos_gpioc_unregister(uint32_t id)
{
    return aos_dev_unregister(AOS_DEV_TYPE_GPIOC, id);
}

aos_status_t aos_gpioc_get(aos_gpioc_ref_t *ref, uint32_t id)
{
    return aos_dev_get(ref, AOS_DEV_TYPE_GPIOC, id);
}

void aos_gpioc_put(aos_gpioc_ref_t *ref)
{
    aos_dev_put(ref);
}

aos_status_t aos_gpioc_get_num_pins(aos_gpioc_ref_t *ref)
{
    aos_gpioc_t *gpioc;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpioc = aos_container_of(ref->dev, aos_gpioc_t, dev);

    return gpioc->num_pins;
}

aos_status_t
aos_gpioc_get_mode(aos_gpioc_ref_t *ref, uint32_t pin, uint32_t *mode)
{
    aos_gpioc_t *gpioc;

    if (!ref || !aos_dev_ref_is_valid(ref) || !mode)
        return -EINVAL;

    gpioc = aos_container_of(ref->dev, aos_gpioc_t, dev);

    if (pin >= gpioc->num_pins)
        return -EINVAL;

    aos_dev_lock(ref->dev);
    *mode = gpioc->pins[pin].mode;
    aos_dev_unlock(ref->dev);

    return 0;
}

aos_status_t
aos_gpioc_set_mode(aos_gpioc_ref_t *ref, uint32_t pin, uint32_t mode)
{
    return aos_gpioc_set_mode_irq(ref, pin, mode, NULL, NULL);
}

static void irq_task_func(void *arg)
{
    aos_gpioc_pin_t *pin = (aos_gpioc_pin_t *)arg;
    aos_gpioc_t *gpioc;

    gpioc = aos_container_of(pin, aos_gpioc_t, pins[pin->id]);

    while (1) {
        uint32_t event_mask;
        uint32_t event_value;
        uint32_t trig;

        event_mask = AOS_GPIOC_IRQ_EVENT_P | AOS_GPIOC_IRQ_EVENT_N;
        (void)aos_event_get(&pin->irq_event, event_mask, AOS_EVENT_OR_CLEAR,
                            &event_value, AOS_WAIT_FOREVER);
        (void)aos_sem_wait(&pin->irq_sem, AOS_WAIT_FOREVER);
        trig = mode_to_irq_trig(pin->mode);
        if (trig == AOS_GPIO_IRQ_TRIG_NONE) {
            aos_sem_signal(&pin->irq_sem);
            continue;
        }

        if ((trig == AOS_GPIO_IRQ_TRIG_LEVEL_HIGH ||
             trig == AOS_GPIO_IRQ_TRIG_EDGE_RISING ||
             trig == AOS_GPIO_IRQ_TRIG_EDGE_BOTH) &&
            (event_value & AOS_GPIOC_IRQ_EVENT_P))
            pin->irq_handler(1, pin->irq_arg);

        if ((trig == AOS_GPIO_IRQ_TRIG_LEVEL_LOW ||
             trig == AOS_GPIO_IRQ_TRIG_EDGE_FALLING ||
             trig == AOS_GPIO_IRQ_TRIG_EDGE_BOTH) &&
            (event_value & AOS_GPIOC_IRQ_EVENT_N))
            pin->irq_handler(0, pin->irq_arg);

        if (trig == AOS_GPIO_IRQ_TRIG_LEVEL_HIGH ||
            trig == AOS_GPIO_IRQ_TRIG_LEVEL_LOW) {
            aos_irqsave_t flags;

            flags = aos_spin_lock_irqsave(&gpioc->lock);
            pin->hard_irq_en = true;
            gpioc->ops->enable_irq(gpioc, pin->id);
            aos_spin_unlock_irqrestore(&gpioc->lock, flags);
        }

        aos_sem_signal(&pin->irq_sem);
    }
}

aos_status_t aos_gpioc_set_mode_irq(aos_gpioc_ref_t *ref,
                                    uint32_t pin, uint32_t mode,
                                    aos_gpio_irq_handler_t irq_handler,
                                    void *irq_arg)
{
    aos_gpioc_t *gpioc;
    uint32_t old_mode;
    aos_gpio_irq_handler_t old_irq_handler;
    void *old_irq_arg;
    aos_task_t old_task;
    aos_status_t ret;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpioc = aos_container_of(ref->dev, aos_gpioc_t, dev);

    if (pin >= gpioc->num_pins)
        return -EINVAL;

    if ((mode & AOS_GPIO_DIR_MASK) == AOS_GPIO_DIR_NONE) {
    } else if ((mode & AOS_GPIO_DIR_MASK) == AOS_GPIO_DIR_INPUT) {
        switch (mode & AOS_GPIO_INPUT_CFG_MASK) {
            case AOS_GPIO_INPUT_CFG_DEFAULT:
            case AOS_GPIO_INPUT_CFG_HI:
            case AOS_GPIO_INPUT_CFG_PU:
            case AOS_GPIO_INPUT_CFG_PD:
                break;
            default:
                return -EINVAL;
        }

        switch (mode & AOS_GPIO_IRQ_TRIG_MASK) {
            case AOS_GPIO_IRQ_TRIG_NONE:
                if (irq_handler)
                    return -EINVAL;
                break;
            case AOS_GPIO_IRQ_TRIG_EDGE_RISING:
            case AOS_GPIO_IRQ_TRIG_EDGE_FALLING:
            case AOS_GPIO_IRQ_TRIG_EDGE_BOTH:
            case AOS_GPIO_IRQ_TRIG_LEVEL_HIGH:
            case AOS_GPIO_IRQ_TRIG_LEVEL_LOW:
                if (!irq_handler)
                    return -EINVAL;
                break;
            default:
                return -EINVAL;
        }
    } else if ((mode & AOS_GPIO_DIR_MASK) == AOS_GPIO_DIR_OUTPUT) {
        switch (mode & AOS_GPIO_OUTPUT_CFG_MASK) {
            case AOS_GPIO_OUTPUT_CFG_DEFAULT:
            case AOS_GPIO_OUTPUT_CFG_PP:
            case AOS_GPIO_OUTPUT_CFG_ODNP:
            case AOS_GPIO_OUTPUT_CFG_ODPU:
                break;
            default:
                return -EINVAL;
        }
    } else {
        return -EINVAL;
    }

    (void)aos_sem_wait(&gpioc->pins[pin].irq_sem, AOS_WAIT_FOREVER);
    old_mode = gpioc->pins[pin].mode;
    old_irq_handler = gpioc->pins[pin].irq_handler;
    old_irq_arg = gpioc->pins[pin].irq_arg;

    if (mode_to_irq_trig(old_mode) != AOS_GPIO_IRQ_TRIG_NONE) {
        aos_irqsave_t flags;

        old_task = gpioc->pins[pin].irq_task;
        flags = aos_spin_lock_irqsave(&gpioc->lock);
        (void)aos_event_set(&gpioc->pins[pin].irq_event, 0, AOS_EVENT_AND);

        if (gpioc->pins[pin].hard_irq_en) {
            gpioc->pins[pin].hard_irq_en = false;
            gpioc->ops->disable_irq(gpioc, pin);
        }

        aos_spin_unlock_irqrestore(&gpioc->lock, flags);
    }

    if (mode_to_irq_trig(mode) != AOS_GPIO_IRQ_TRIG_NONE) {
        uint32_t prio;

        if ((mode & AOS_GPIO_IRQ_PRIO_MASK) == AOS_GPIO_IRQ_PRIO_DEFAULT)
            prio = IRQ_TASK_DEFAULT_PRIO;
        else
            prio = (mode & AOS_GPIO_IRQ_PRIO_MASK) >> 16;

        ret = aos_task_new_ext(&gpioc->pins[pin].irq_task, "gpio_irq_task",
                               irq_task_func, &gpioc->pins[pin],
                               IRQ_TASK_STACK_SIZE, prio);
        if (ret) {
            if (mode_to_irq_trig(old_mode) != AOS_GPIO_IRQ_TRIG_NONE) {
                aos_irqsave_t flags;

                gpioc->pins[pin].irq_task = old_task;
                flags = aos_spin_lock_irqsave(&gpioc->lock);
                gpioc->pins[pin].hard_irq_en = true;
                gpioc->ops->enable_irq(gpioc, pin);
                aos_spin_unlock_irqrestore(&gpioc->lock, flags);
            }

            aos_sem_signal(&gpioc->pins[pin].irq_sem);
            return ret;
        }
    }

    aos_dev_lock(ref->dev);
    gpioc->pins[pin].mode = mode;
    gpioc->pins[pin].irq_handler = irq_handler;
    gpioc->pins[pin].irq_arg = irq_arg;

    if ((mode & AOS_GPIO_DIR_MASK) == AOS_GPIO_DIR_OUTPUT)
        gpioc->pins[pin].value = !!(mode & AOS_GPIO_OUTPUT_INIT_HIGH);
    else
        gpioc->pins[pin].value = 0;

    ret = gpioc->ops->set_mode(gpioc, pin);
    if (ret) {
        gpioc->pins[pin].mode = old_mode;
        gpioc->pins[pin].irq_handler = old_irq_handler;
        gpioc->pins[pin].irq_arg = old_irq_arg;
        aos_dev_unlock(ref->dev);

        if (mode_to_irq_trig(mode) != AOS_GPIO_IRQ_TRIG_NONE)
            (void)aos_task_delete(&gpioc->pins[pin].irq_task);

        if (mode_to_irq_trig(old_mode) != AOS_GPIO_IRQ_TRIG_NONE) {
            aos_irqsave_t flags;

            gpioc->pins[pin].irq_task = old_task;
            flags = aos_spin_lock_irqsave(&gpioc->lock);
            gpioc->pins[pin].hard_irq_en = true;
            gpioc->ops->enable_irq(gpioc, pin);
            aos_spin_unlock_irqrestore(&gpioc->lock, flags);
        }

        aos_sem_signal(&gpioc->pins[pin].irq_sem);
        return ret;
    }

    aos_dev_unlock(ref->dev);

    if (mode_to_irq_trig(old_mode) != AOS_GPIO_IRQ_TRIG_NONE)
        (void)aos_task_delete(&old_task);

    if (mode_to_irq_trig(mode) != AOS_GPIO_IRQ_TRIG_NONE) {
        aos_irqsave_t flags;

        flags = aos_spin_lock_irqsave(&gpioc->lock);
        gpioc->pins[pin].hard_irq_en = true;
        gpioc->ops->enable_irq(gpioc, pin);
        aos_spin_unlock_irqrestore(&gpioc->lock, flags);
    }

    aos_sem_signal(&gpioc->pins[pin].irq_sem);

    return 0;
}

aos_status_t aos_gpioc_get_value(aos_gpioc_ref_t *ref, uint32_t pin)
{
    aos_gpioc_t *gpioc;
    uint32_t mode;
    aos_status_t ret;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpioc = aos_container_of(ref->dev, aos_gpioc_t, dev);

    if (pin >= gpioc->num_pins)
        return -EINVAL;

    aos_dev_lock(ref->dev);
    mode = gpioc->pins[pin].mode;

    if ((mode & AOS_GPIO_DIR_MASK) == AOS_GPIO_DIR_INPUT)
        ret = (aos_status_t)!!gpioc->ops->get_value(gpioc, pin);
    else if ((mode & AOS_GPIO_DIR_MASK) == AOS_GPIO_DIR_OUTPUT)
        ret = (aos_status_t)gpioc->pins[pin].value;
    else
        ret = -ENOTSUP;

    aos_dev_unlock(ref->dev);

    return ret;
}

aos_status_t aos_gpioc_set_value(aos_gpioc_ref_t *ref, uint32_t pin, int val)
{
    aos_gpioc_t *gpioc;
    uint32_t mode;

    if (!ref || !aos_dev_ref_is_valid(ref) || val < 0)
        return -EINVAL;

    gpioc = aos_container_of(ref->dev, aos_gpioc_t, dev);

    if (pin >= gpioc->num_pins)
        return -EINVAL;

    aos_dev_lock(ref->dev);
    mode = gpioc->pins[pin].mode;

    if ((mode & AOS_GPIO_DIR_MASK) != AOS_GPIO_DIR_OUTPUT) {
        aos_dev_unlock(ref->dev);
        return -ENOTSUP;
    }

    val = !!val;
    if (gpioc->pins[pin].value == val) {
        aos_dev_unlock(ref->dev);
        return 0;
    }

    gpioc->pins[pin].value = val;
    gpioc->ops->set_value(gpioc, pin);
    aos_dev_unlock(ref->dev);

    return 0;
}

aos_status_t aos_gpioc_toggle(aos_gpioc_ref_t *ref, uint32_t pin)
{
    aos_gpioc_t *gpioc;
    uint32_t mode;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpioc = aos_container_of(ref->dev, aos_gpioc_t, dev);

    if (pin >= gpioc->num_pins)
        return -EINVAL;

    aos_dev_lock(ref->dev);
    mode = gpioc->pins[pin].mode;

    if ((mode & AOS_GPIO_DIR_MASK) != AOS_GPIO_DIR_OUTPUT) {
        aos_dev_unlock(ref->dev);
        return -ENOTSUP;
    }

    gpioc->pins[pin].value = !gpioc->pins[pin].value;
    gpioc->ops->set_value(gpioc, pin);
    aos_dev_unlock(ref->dev);

    return 0;
}
