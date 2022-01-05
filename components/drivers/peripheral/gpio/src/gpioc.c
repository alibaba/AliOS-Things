/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <aos/gpioc_core.h>

#ifndef AOS_GPIO_IRQ_TASK_DEFAULT_PRIO
#define AOS_GPIO_IRQ_TASK_DEFAULT_PRIO  30
#endif
#ifndef AOS_GPIO_IRQ_TASK_STACK_SIZE
#define AOS_GPIO_IRQ_TASK_STACK_SIZE    4096
#endif

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

static void get_mode_irq(aos_gpioc_t *gpioc, uint32_t pin, uint32_t *mode,
                         aos_gpio_irq_handler_t *irq_handler, void **irq_arg)
{
    if (mode)
        *mode = gpioc->pins[pin].mode;

    if (irq_handler)
        *irq_handler = gpioc->pins[pin].irq_handler;

    if (irq_arg)
        *irq_arg = gpioc->pins[pin].irq_arg;
}

aos_status_t aos_gpioc_get_mode_irq(aos_gpioc_ref_t *ref, uint32_t pin, uint32_t *mode,
                                    aos_gpio_irq_handler_t *irq_handler, void **irq_arg)
{
    aos_gpioc_t *gpioc;

    if (!ref || !aos_dev_ref_is_valid(ref) || (!mode && !irq_handler && !irq_arg))
        return -EINVAL;

    gpioc = aos_container_of(ref->dev, aos_gpioc_t, dev);

    if (pin >= gpioc->num_pins)
        return -EINVAL;

    aos_dev_lock(ref->dev);
    get_mode_irq(gpioc, pin, mode, irq_handler, irq_arg);
    aos_dev_unlock(ref->dev);

    return 0;
}

static bool is_in_irq(aos_gpioc_t *gpioc, uint32_t pin)
{
    aos_task_t self_task;
    aos_task_t irq_task;

    self_task = aos_task_self();
    aos_dev_lock(&gpioc->dev);
    irq_task = gpioc->pins[pin].irq_task;
    aos_dev_unlock(&gpioc->dev);

    return self_task == irq_task;
}

static inline uint32_t mode_to_irq_trig(uint32_t mode)
{
    uint32_t trig;

    if ((mode & AOS_GPIO_DIR_MASK) == AOS_GPIO_DIR_INPUT)
        trig = mode & AOS_GPIO_IRQ_TRIG_MASK;
    else
        trig = AOS_GPIO_IRQ_TRIG_NONE;

    return trig;
}

static void irq_task_func(void *arg)
{
    aos_gpioc_pin_t *pin = (aos_gpioc_pin_t *)arg;
    aos_gpioc_t *gpioc = pin->gpioc;

    while (1) {
        uint32_t trig;
        uint32_t val;
        bool dying;

        (void)aos_event_get(&pin->irq_event, 0xFFFFFFFF, AOS_EVENT_OR, &val, AOS_WAIT_FOREVER);
        (void)aos_sem_wait(&pin->irq_sem, AOS_WAIT_FOREVER);

        if (aos_event_get(&pin->irq_event, 0xFFFFFFFF, AOS_EVENT_OR_CLEAR, &val, AOS_NO_WAIT))
            val = 0;

        pin->irq_new_task = pin->irq_task;
        trig = mode_to_irq_trig(pin->mode);

        switch (trig) {
        case AOS_GPIO_IRQ_TRIG_EDGE_RISING:
            for (int i = 0; i < 16; i++) {
                uint32_t mask = (val << (2 * i)) & 0x3;
                if (mask == 0x2)
                    pin->irq_handler(1, pin->irq_arg);
                else
                    break;
            }
            break;
        case AOS_GPIO_IRQ_TRIG_EDGE_FALLING:
            for (int i = 0; i < 16; i++) {
                uint32_t mask = (val << (2 * i)) & 0x3;
                if (mask == 0x1)
                    pin->irq_handler(0, pin->irq_arg);
                else
                    break;
            }
            break;
        case AOS_GPIO_IRQ_TRIG_EDGE_BOTH:
            for (int i = 0; i < 16; i++) {
                uint32_t mask = (val << (2 * i)) & 0x3;
                if (mask == 0x2)
                    pin->irq_handler(1, pin->irq_arg);
                else if (mask == 0x1)
                    pin->irq_handler(0, pin->irq_arg);
                else
                    break;
            }
            break;
        case AOS_GPIO_IRQ_TRIG_LEVEL_HIGH:
            if (val == 0x2)
                pin->irq_handler(1, pin->irq_arg);
            break;
        case AOS_GPIO_IRQ_TRIG_LEVEL_LOW:
            if (val == 0x1)
                pin->irq_handler(0, pin->irq_arg);
            break;
        default:
            break;
        }

        if (trig == AOS_GPIO_IRQ_TRIG_LEVEL_HIGH || trig == AOS_GPIO_IRQ_TRIG_LEVEL_LOW) {
            uint32_t pin_id = ((uintptr_t)pin - (uintptr_t)gpioc->pins) / sizeof(*pin);
            aos_irqsave_t flags;
            flags = aos_spin_lock_irqsave(&gpioc->lock);
            pin->hard_irq_en = true;
            gpioc->ops->enable_irq(gpioc, pin_id);
            aos_spin_unlock_irqrestore(&gpioc->lock, flags);
        }

        if (pin->irq_new_task == pin->irq_task) {
            dying = false;
        } else {
            aos_dev_lock(&gpioc->dev);
            pin->irq_task = pin->irq_new_task;
            aos_dev_unlock(&gpioc->dev);
            dying = true;
        }

        pin->irq_new_task = NULL;
        aos_sem_signal(&pin->irq_sem);

        if (dying)
            break;
    }

    aos_task_exit(0);
}

static aos_status_t set_mode_irq(aos_gpioc_t *gpioc, uint32_t pin, bool in_irq, uint32_t mode,
                                 aos_gpio_irq_handler_t irq_handler, void *irq_arg)
{
    uint32_t old_mode;
    int old_value;
    aos_task_t new_task = NULL;
    aos_status_t ret;

    switch (mode & AOS_GPIO_DIR_MASK) {
    case AOS_GPIO_DIR_NONE:
        irq_handler = NULL;
        break;
    case AOS_GPIO_DIR_INPUT:
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
        break;
    case AOS_GPIO_DIR_OUTPUT:
        switch (mode & AOS_GPIO_OUTPUT_CFG_MASK) {
        case AOS_GPIO_OUTPUT_CFG_DEFAULT:
        case AOS_GPIO_OUTPUT_CFG_PP:
        case AOS_GPIO_OUTPUT_CFG_ODNP:
        case AOS_GPIO_OUTPUT_CFG_ODPU:
            break;
        default:
            return -EINVAL;
        }
        irq_handler = NULL;
        break;
    default:
        return -EINVAL;
    }

    if (!irq_handler)
        irq_arg = NULL;

    old_mode = gpioc->pins[pin].mode;
    old_value = gpioc->pins[pin].value;

    if (mode_to_irq_trig(old_mode) != AOS_GPIO_IRQ_TRIG_NONE) {
        aos_irqsave_t flags;
        flags = aos_spin_lock_irqsave(&gpioc->lock);
        (void)aos_event_set(&gpioc->pins[pin].irq_event, 0, AOS_EVENT_AND);
        if (gpioc->pins[pin].hard_irq_en) {
            gpioc->pins[pin].hard_irq_en = false;
            gpioc->ops->disable_irq(gpioc, pin);
        }
        aos_spin_unlock_irqrestore(&gpioc->lock, flags);
    }

    if ((irq_handler != NULL && gpioc->pins[pin].irq_handler == NULL) ||
        (irq_handler != NULL && gpioc->pins[pin].irq_handler != NULL &&
         (mode & AOS_GPIO_IRQ_PRIO_MASK) != (old_mode & AOS_GPIO_IRQ_PRIO_MASK))) {
        uint32_t prio = ((mode & AOS_GPIO_IRQ_PRIO_MASK) == AOS_GPIO_IRQ_PRIO_DEFAULT) ?
                        AOS_GPIO_IRQ_TASK_DEFAULT_PRIO : ((mode & AOS_GPIO_IRQ_PRIO_MASK) >> 16);
        ret = aos_task_new_ext(&new_task, "gpio_irq_task", irq_task_func, &gpioc->pins[pin],
                               AOS_GPIO_IRQ_TASK_STACK_SIZE, prio);
        if (ret) {
            if ((in_irq && (mode_to_irq_trig(old_mode) == AOS_GPIO_IRQ_TRIG_EDGE_RISING ||
                            mode_to_irq_trig(old_mode) == AOS_GPIO_IRQ_TRIG_EDGE_FALLING ||
                            mode_to_irq_trig(old_mode) == AOS_GPIO_IRQ_TRIG_EDGE_BOTH)) ||
                (!in_irq && mode_to_irq_trig(old_mode) != AOS_GPIO_IRQ_TRIG_NONE)) {
                aos_irqsave_t flags;
                flags = aos_spin_lock_irqsave(&gpioc->lock);
                gpioc->pins[pin].hard_irq_en = true;
                gpioc->ops->enable_irq(gpioc, pin);
                aos_spin_unlock_irqrestore(&gpioc->lock, flags);
            }
            return ret;
        }
    }

    gpioc->pins[pin].mode = mode;
    gpioc->pins[pin].value = ((mode & AOS_GPIO_DIR_MASK) == AOS_GPIO_DIR_OUTPUT) ?
                             !!(mode & AOS_GPIO_OUTPUT_INIT_HIGH) : 0;
    ret = gpioc->ops->set_mode(gpioc, pin);
    if (ret) {
        gpioc->pins[pin].mode = old_mode;
        gpioc->pins[pin].value = old_value;
        if (new_task)
            (void)aos_task_delete(&new_task);
        if ((in_irq && (mode_to_irq_trig(old_mode) == AOS_GPIO_IRQ_TRIG_EDGE_RISING ||
                        mode_to_irq_trig(old_mode) == AOS_GPIO_IRQ_TRIG_EDGE_FALLING ||
                        mode_to_irq_trig(old_mode) == AOS_GPIO_IRQ_TRIG_EDGE_BOTH)) ||
            (!in_irq && mode_to_irq_trig(old_mode) != AOS_GPIO_IRQ_TRIG_NONE)) {
            aos_irqsave_t flags;
            flags = aos_spin_lock_irqsave(&gpioc->lock);
            gpioc->pins[pin].hard_irq_en = true;
            gpioc->ops->enable_irq(gpioc, pin);
            aos_spin_unlock_irqrestore(&gpioc->lock, flags);
        }
        return ret;
    }

    if (irq_handler != NULL && gpioc->pins[pin].irq_handler != NULL) {
        if (new_task) {
            if (in_irq) {
                /* gpioc->pins[pin].irq_new_task != NULL because gpioc->pins[pin].irq_handler != NULL */
                if (gpioc->pins[pin].irq_new_task != gpioc->pins[pin].irq_task)
                    (void)aos_task_delete(&gpioc->pins[pin].irq_new_task);
                gpioc->pins[pin].irq_new_task = new_task;
            } else {
                /* gpioc->pins[pin].irq_task != NULL because gpioc->pins[pin].irq_handler != NULL */
                (void)aos_task_delete(&gpioc->pins[pin].irq_task);
                gpioc->pins[pin].irq_task = new_task;
            }
        }
    } else if (irq_handler != NULL && gpioc->pins[pin].irq_handler == NULL) {
        if (in_irq) {
            /* gpioc->pins[pin].irq_new_task == NULL because gpioc->pins[pin].irq_handler == NULL */
            gpioc->pins[pin].irq_new_task = new_task;
        } else {
            /* gpioc->pins[pin].irq_task == NULL because gpioc->pins[pin].irq_handler == NULL */
            gpioc->pins[pin].irq_task = new_task;
        }
    } else if (irq_handler == NULL && gpioc->pins[pin].irq_handler != NULL) {
        if (in_irq) {
            /* gpioc->pins[pin].irq_new_task != NULL because gpioc->pins[pin].irq_handler != NULL */
            if (gpioc->pins[pin].irq_new_task != gpioc->pins[pin].irq_task)
                (void)aos_task_delete(&gpioc->pins[pin].irq_new_task);
            gpioc->pins[pin].irq_new_task = NULL;
        } else {
            /* gpioc->pins[pin].irq_task != NULL because gpioc->pins[pin].irq_handler != NULL */
            (void)aos_task_delete(&gpioc->pins[pin].irq_task);
            gpioc->pins[pin].irq_task = NULL;
        }
    }

    gpioc->pins[pin].irq_handler = irq_handler;
    gpioc->pins[pin].irq_arg = irq_arg;

    if ((in_irq && (mode_to_irq_trig(mode) == AOS_GPIO_IRQ_TRIG_EDGE_RISING ||
                    mode_to_irq_trig(mode) == AOS_GPIO_IRQ_TRIG_EDGE_FALLING ||
                    mode_to_irq_trig(mode) == AOS_GPIO_IRQ_TRIG_EDGE_BOTH)) ||
        (!in_irq && mode_to_irq_trig(mode) != AOS_GPIO_IRQ_TRIG_NONE)) {
        aos_irqsave_t flags;
        flags = aos_spin_lock_irqsave(&gpioc->lock);
        gpioc->pins[pin].hard_irq_en = true;
        gpioc->ops->enable_irq(gpioc, pin);
        aos_spin_unlock_irqrestore(&gpioc->lock, flags);
    }

    return 0;
}

aos_status_t aos_gpioc_set_mode_irq(aos_gpioc_ref_t *ref, uint32_t pin, uint32_t mode,
                                    aos_gpio_irq_handler_t irq_handler, void *irq_arg)
{
    aos_gpioc_t *gpioc;
    bool in_irq;
    aos_status_t ret;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpioc = aos_container_of(ref->dev, aos_gpioc_t, dev);

    if (pin >= gpioc->num_pins)
        return -EINVAL;

    in_irq = is_in_irq(gpioc, pin);
    if (in_irq)
        (void)aos_sem_wait(&gpioc->pins[pin].irq_sem, AOS_WAIT_FOREVER);

    aos_dev_lock(ref->dev);
    ret = set_mode_irq(gpioc, pin, in_irq, mode, irq_handler, irq_arg);
    aos_dev_unlock(ref->dev);

    if (in_irq)
        aos_sem_signal(&gpioc->pins[pin].irq_sem);

    return ret;
}

aos_status_t aos_gpioc_get_mode(aos_gpioc_ref_t *ref, uint32_t pin, uint32_t *mode)
{
    return aos_gpioc_get_mode_irq(ref, pin, mode, NULL, NULL);
}

aos_status_t aos_gpioc_set_mode(aos_gpioc_ref_t *ref, uint32_t pin, uint32_t mode)
{
    aos_gpioc_t *gpioc;
    bool in_irq;
    aos_gpio_irq_handler_t irq_handler;
    void *irq_arg;
    aos_status_t ret;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpioc = aos_container_of(ref->dev, aos_gpioc_t, dev);

    if (pin >= gpioc->num_pins)
        return -EINVAL;

    in_irq = is_in_irq(gpioc, pin);
    if (in_irq)
        (void)aos_sem_wait(&gpioc->pins[pin].irq_sem, AOS_WAIT_FOREVER);

    aos_dev_lock(ref->dev);
    get_mode_irq(gpioc, pin, NULL, &irq_handler, &irq_arg);
    ret = set_mode_irq(gpioc, pin, in_irq, mode, irq_handler, irq_arg);
    aos_dev_unlock(ref->dev);

    if (in_irq)
        aos_sem_signal(&gpioc->pins[pin].irq_sem);

    return ret;
}

aos_status_t aos_gpioc_get_irq_trigger(aos_gpioc_ref_t *ref, uint32_t pin, uint32_t *trig)
{
    uint32_t mode;
    aos_status_t ret;

    if (!trig)
        return -EINVAL;

    ret = aos_gpioc_get_mode_irq(ref, pin, &mode, NULL, NULL);
    if (ret)
        return ret;

    if ((mode & AOS_GPIO_DIR_MASK) != AOS_GPIO_DIR_INPUT)
        return -ENOTSUP;

    *trig = mode & AOS_GPIO_IRQ_TRIG_MASK;

    return 0;
}

aos_status_t aos_gpioc_set_irq_trigger(aos_gpioc_ref_t *ref, uint32_t pin, uint32_t trig)
{
    aos_gpioc_t *gpioc;
    bool in_irq;
    uint32_t mode;
    aos_gpio_irq_handler_t irq_handler;
    void *irq_arg;
    aos_status_t ret;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpioc = aos_container_of(ref->dev, aos_gpioc_t, dev);

    if (pin >= gpioc->num_pins)
        return -EINVAL;

    in_irq = is_in_irq(gpioc, pin);
    if (in_irq)
        (void)aos_sem_wait(&gpioc->pins[pin].irq_sem, AOS_WAIT_FOREVER);

    aos_dev_lock(ref->dev);
    get_mode_irq(gpioc, pin, &mode, &irq_handler, &irq_arg);

    if ((mode & AOS_GPIO_DIR_MASK) != AOS_GPIO_DIR_INPUT) {
        aos_dev_unlock(ref->dev);
        if (in_irq)
            aos_sem_signal(&gpioc->pins[pin].irq_sem);
        return -ENOTSUP;
    }

    mode &= ~AOS_GPIO_DIR_MASK;
    mode |= trig & AOS_GPIO_DIR_MASK;
    ret = set_mode_irq(gpioc, pin, in_irq, mode, irq_handler, irq_arg);
    aos_dev_unlock(ref->dev);

    if (in_irq)
        aos_sem_signal(&gpioc->pins[pin].irq_sem);

    return ret;
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

static void dev_gpioc_unregister(aos_dev_t *dev)
{
    aos_gpioc_t *gpioc = aos_container_of(dev, aos_gpioc_t, dev);

    for (uint32_t i = 0; i < gpioc->num_pins; i++) {
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

    if (!gpioc)
        return -EINVAL;

    if (!gpioc->ops || !gpioc->ops->set_mode || !gpioc->ops->enable_irq || !gpioc->ops->disable_irq ||
        !gpioc->ops->get_value || !gpioc->ops->set_value)
        return -EINVAL;

    if (gpioc->num_pins == 0 || gpioc->num_pins > (uint32_t)INT32_MAX)
        return -EINVAL;

    if (!gpioc->pins)
        return -EINVAL;

    gpioc->dev.type = AOS_DEV_TYPE_GPIOC;
    gpioc->dev.ops = &dev_gpioc_ops;
#ifdef AOS_COMP_VFS
    gpioc->dev.vfs_helper.name[0] = '\0';
    gpioc->dev.vfs_helper.ops = NULL;
#endif
    aos_spin_lock_init(&gpioc->lock);

    for (uint32_t i = 0; i < gpioc->num_pins; i++) {
        aos_gpioc_pin_t *pin = &gpioc->pins[i];

        pin->gpioc = gpioc;
        pin->mode = AOS_GPIO_DIR_NONE;
        pin->irq_handler = NULL;
        pin->irq_arg = NULL;
        pin->irq_task = NULL;
        pin->irq_new_task = NULL;
        pin->hard_irq_en = false;
        pin->value = 0;

        ret = aos_sem_new(&pin->irq_sem, 1);
        if (ret) {
            for (uint32_t j = 0; j < i; j++) {
                aos_sem_free(&gpioc->pins[j].irq_sem);
                aos_event_free(&gpioc->pins[j].irq_event);
            }
            return ret;
        }

        ret = aos_event_new(&pin->irq_event, 0);
        if (ret) {
            aos_sem_free(&gpioc->pins[i].irq_sem);
            for (uint32_t j = 0; j < i; j++) {
                aos_sem_free(&gpioc->pins[j].irq_sem);
                aos_event_free(&gpioc->pins[j].irq_event);
            }
            return ret;
        }
    }

    ret = aos_dev_register(&gpioc->dev);
    if (ret) {
        for (uint32_t i = 0; i < gpioc->num_pins; i++) {
            aos_sem_free(&gpioc->pins[i].irq_sem);
            aos_event_free(&gpioc->pins[i].irq_event);
        }
        return ret;
    }

    return 0;
}

aos_status_t aos_gpioc_register_argumented(aos_gpioc_t *gpioc, uint32_t id, const aos_gpioc_ops_t *ops,
                                           uint32_t num_pins, aos_gpioc_pin_t *pins)
{
    if (!gpioc)
        return -EINVAL;

    gpioc->dev.id = id;
    gpioc->ops = ops;
    gpioc->num_pins = num_pins;
    gpioc->pins = pins;

    return aos_gpioc_register(gpioc);
}

aos_status_t aos_gpioc_unregister(uint32_t id)
{
    return aos_dev_unregister(AOS_DEV_TYPE_GPIOC, id);
}
