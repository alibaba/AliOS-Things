/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <aos/gpioc.h>

static void dev_gpioc_unregister(aos_dev_t *dev)
{
    aos_gpioc_t *gpioc = aos_container_of(dev, aos_gpioc_t, dev);

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

        pin->mode = AOS_GPIO_DIR_NONE;
        pin->irq_handler = NULL;
        pin->irq_arg = NULL;
        pin->value = 0;
    }

    return aos_dev_register(&gpioc->dev);
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

aos_status_t aos_gpioc_set_mode_irq(aos_gpioc_ref_t *ref,
                                    uint32_t pin, uint32_t mode,
                                    aos_gpio_irq_handler_t irq_handler,
                                    void *irq_arg)
{
    aos_gpioc_t *gpioc;
    uint32_t old_mode;
    aos_status_t ret;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpioc = aos_container_of(ref->dev, aos_gpioc_t, dev);

    if (pin >= gpioc->num_pins)
        return -EINVAL;

    aos_dev_lock(ref->dev);
    old_mode = gpioc->pins[pin].mode;
    gpioc->pins[pin].mode = mode;

    if ((mode & AOS_GPIO_DIR_MASK) == AOS_GPIO_DIR_OUTPUT)
        gpioc->pins[pin].value = !!(mode & AOS_GPIO_OUTPUT_INIT_HIGH);
    else
        gpioc->pins[pin].value = 0;

    ret = gpioc->ops->set_mode(gpioc, pin);
    if (ret) {
        gpioc->pins[pin].mode = old_mode;
        aos_dev_unlock(ref->dev);
        return ret;
    }

    aos_dev_unlock(ref->dev);

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
