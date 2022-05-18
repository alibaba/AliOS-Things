/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <aos/gpio_core.h>
#ifdef AOS_COMP_DEVFS
#include <stdio.h>
#include <inttypes.h>
#endif

aos_status_t aos_gpio_get(aos_gpio_ref_t *ref, uint32_t id)
{
    return aos_dev_get(ref, AOS_DEV_TYPE_GPIO, id);
}

void aos_gpio_put(aos_gpio_ref_t *ref)
{
    aos_dev_put(ref);
}

aos_status_t
aos_gpio_get_mode_irq(aos_gpio_ref_t *ref, uint32_t *mode, aos_gpio_irq_handler_t *irq_handler, void **irq_arg)
{
    aos_gpio_t *gpio;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpio = aos_container_of(ref->dev, aos_gpio_t, dev);

    return aos_gpioc_get_mode_irq(&gpio->gpioc_ref, gpio->pin, mode, irq_handler, irq_arg);
}

aos_status_t
aos_gpio_set_mode_irq(aos_gpio_ref_t *ref, uint32_t mode, aos_gpio_irq_handler_t irq_handler, void *irq_arg)
{
    aos_gpio_t *gpio;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpio = aos_container_of(ref->dev, aos_gpio_t, dev);

    return aos_gpioc_set_mode_irq(&gpio->gpioc_ref, gpio->pin, mode, irq_handler, irq_arg);
}

aos_status_t aos_gpio_get_mode(aos_gpio_ref_t *ref, uint32_t *mode)
{
    aos_gpio_t *gpio;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpio = aos_container_of(ref->dev, aos_gpio_t, dev);

    return aos_gpioc_get_mode(&gpio->gpioc_ref, gpio->pin, mode);
}

aos_status_t aos_gpio_set_mode(aos_gpio_ref_t *ref, uint32_t mode)
{
    aos_gpio_t *gpio;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpio = aos_container_of(ref->dev, aos_gpio_t, dev);

    return aos_gpioc_set_mode(&gpio->gpioc_ref, gpio->pin, mode);
}

aos_status_t aos_gpio_get_irq_trigger(aos_gpio_ref_t *ref, uint32_t *trig)
{
    aos_gpio_t *gpio;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpio = aos_container_of(ref->dev, aos_gpio_t, dev);

    return aos_gpioc_get_irq_trigger(&gpio->gpioc_ref, gpio->pin, trig);
}

aos_status_t aos_gpio_set_irq_trigger(aos_gpio_ref_t *ref, uint32_t trig)
{
    aos_gpio_t *gpio;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpio = aos_container_of(ref->dev, aos_gpio_t, dev);

    return aos_gpioc_set_irq_trigger(&gpio->gpioc_ref, gpio->pin, trig);
}

aos_status_t aos_gpio_get_value(aos_gpio_ref_t *ref)
{
    aos_gpio_t *gpio;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpio = aos_container_of(ref->dev, aos_gpio_t, dev);

    return aos_gpioc_get_value(&gpio->gpioc_ref, gpio->pin);
}

aos_status_t aos_gpio_set_value(aos_gpio_ref_t *ref, int val)
{
    aos_gpio_t *gpio;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpio = aos_container_of(ref->dev, aos_gpio_t, dev);

    return aos_gpioc_set_value(&gpio->gpioc_ref, gpio->pin, val);
}

aos_status_t aos_gpio_toggle(aos_gpio_ref_t *ref)
{
    aos_gpio_t *gpio;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpio = aos_container_of(ref->dev, aos_gpio_t, dev);

    return aos_gpioc_toggle(&gpio->gpioc_ref, gpio->pin);
}

aos_status_t aos_gpio_poll_irq(aos_gpio_ref_t *ref, uint32_t timeout)
{
    aos_gpio_t *gpio;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpio = aos_container_of(ref->dev, aos_gpio_t, dev);

    return aos_gpioc_poll_irq(&gpio->gpioc_ref, gpio->pin, timeout);
}

aos_status_t aos_gpio_enter_irq(aos_gpio_ref_t *ref, uint32_t timeout)
{
    aos_gpio_t *gpio;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpio = aos_container_of(ref->dev, aos_gpio_t, dev);

    return aos_gpioc_enter_irq(&gpio->gpioc_ref, gpio->pin, timeout);
}

aos_status_t aos_gpio_exit_irq(aos_gpio_ref_t *ref)
{
    aos_gpio_t *gpio;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    gpio = aos_container_of(ref->dev, aos_gpio_t, dev);

    return aos_gpioc_exit_irq(&gpio->gpioc_ref, gpio->pin);
}

static void dev_gpio_unregister(aos_dev_t *dev)
{
}

static aos_status_t dev_gpio_get(aos_dev_ref_t *ref)
{
    aos_gpio_t *gpio = aos_container_of(ref->dev, aos_gpio_t, dev);
    uint32_t num_pins;
    aos_status_t ret;

    if (!aos_dev_ref_is_first(ref))
        return 0;

    ret = aos_gpioc_get(&gpio->gpioc_ref, gpio->gpioc_id);
    if (ret)
        return ret;

    ret = aos_gpioc_get_num_pins(&gpio->gpioc_ref);
    if (ret) {
        aos_gpioc_put(&gpio->gpioc_ref);
        return ret;
    } else {
        num_pins = (uint32_t)ret;
    }

    if (gpio->pin >= num_pins) {
        aos_gpioc_put(&gpio->gpioc_ref);
        return -EINVAL;
    }

    return 0;
}

static void dev_gpio_put(aos_dev_ref_t *ref)
{
    aos_gpio_t *gpio = aos_container_of(ref->dev, aos_gpio_t, dev);

    if (!aos_dev_ref_is_last(ref))
        return;

    aos_gpioc_put(&gpio->gpioc_ref);
}

static const aos_dev_ops_t dev_gpio_ops = {
    .unregister = dev_gpio_unregister,
    .get        = dev_gpio_get,
    .put        = dev_gpio_put,
};

#ifdef AOS_COMP_DEVFS

static void devfs_gpio_irq_handler(int polarity, void *arg)
{
    aos_devfs_poll_wakeup(&((aos_gpio_t *)arg)->dev.devfs_node, AOS_DEVFS_WQ_RD);
}

static aos_status_t devfs_gpio_ioctl(aos_devfs_file_t *file, int cmd, uintptr_t arg)
{
    aos_gpio_ref_t *ref = aos_devfs_file2ref(file);
    aos_status_t ret;

    switch (cmd) {
    case AOS_GPIO_IOC_GET_MODE:
        {
            uint32_t mode;
            if (!aos_devfs_file_is_readable(file)) {
                ret = -EPERM;
                break;
            }
            if (!aos_umem_check((const void *)arg, sizeof(mode))) {
                ret = -EFAULT;
                break;
            }
            ret = aos_gpio_get_mode(ref, &mode);
            if (ret)
                break;
            if (((mode & AOS_GPIO_DIR_MASK) == AOS_GPIO_DIR_INPUT ||
                 (mode & AOS_GPIO_DIR_MASK) == AOS_GPIO_DIR_BOTH) &&
                (mode & AOS_GPIO_IRQ_TRIG_MASK) != AOS_GPIO_IRQ_TRIG_NONE)
                mode &= ~AOS_GPIO_IRQ_ALTERNATE;
            ret = aos_umem_copy((void *)arg, &mode, sizeof(mode)) ? -EFAULT : 0;
        }
        break;
    case AOS_GPIO_IOC_SET_MODE:
        {
            uint32_t mode;
            aos_gpio_irq_handler_t irq_handler = NULL;
            void *irq_arg = NULL;
            if (!aos_devfs_file_is_writable(file)) {
                ret = -EPERM;
                break;
            }
            if (!aos_umem_check((const void *)arg, sizeof(mode))) {
                ret = -EFAULT;
                break;
            }
            if (aos_umem_copy(&mode, (const void *)arg, sizeof(mode))) {
                ret = -EFAULT;
                break;
            }
            if (((mode & AOS_GPIO_DIR_MASK) == AOS_GPIO_DIR_INPUT ||
                 (mode & AOS_GPIO_DIR_MASK) == AOS_GPIO_DIR_BOTH) &&
                (mode & AOS_GPIO_IRQ_TRIG_MASK) != AOS_GPIO_IRQ_TRIG_NONE) {
                mode |= AOS_GPIO_IRQ_ALTERNATE;
                irq_handler = devfs_gpio_irq_handler;
                irq_arg = aos_container_of(ref->dev, aos_gpio_t, dev);
            }
            ret = aos_gpio_set_mode_irq(ref, mode, irq_handler, irq_arg);
        }
        break;
    case AOS_GPIO_IOC_TOGGLE:
        if (!aos_devfs_file_is_writable(file)) {
            ret = -EPERM;
            break;
        }
        ret = aos_gpio_toggle(ref);
        break;
    case AOS_GPIO_IOC_ENTER_IRQ:
        if (!aos_devfs_file_is_readable(file)) {
            ret = -EPERM;
            break;
        }
        ret = aos_gpio_enter_irq(ref, (aos_devfs_file_get_mode(file) & O_NONBLOCK) ? AOS_NO_WAIT : AOS_WAIT_FOREVER);
        break;
    case AOS_GPIO_IOC_EXIT_IRQ:
        if (!aos_devfs_file_is_writable(file)) {
            ret = -EPERM;
            break;
        }
        ret = aos_gpio_exit_irq(ref);
        break;
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

static aos_status_t devfs_gpio_poll(aos_devfs_file_t *file, aos_devfs_poll_request_t *req)
{
    aos_gpio_ref_t *ref = aos_devfs_file2ref(file);
    aos_status_t ret = POLLOUT;
    aos_status_t r;

    aos_dev_lock(ref->dev);
    aos_devfs_poll_add(&ref->dev->devfs_node, AOS_DEVFS_WQ_RD, req);
    r = aos_gpio_poll_irq(ref, AOS_NO_WAIT);
    aos_dev_unlock(ref->dev);

    if (r >= 0) {
        ret |= POLLIN;
    } else if (r == -EAGAIN) {
        ret |= 0;
    } else if (r == -ENOTSUP) {
        /* configuration without IRQ */
        ret |= POLLIN;
    } else {
        /* on failure */
        ret = r;
    }

    return ret;
}

static ssize_t devfs_gpio_read(aos_devfs_file_t *file, void *buf, size_t count)
{
    aos_gpio_ref_t *ref = aos_devfs_file2ref(file);
    uint8_t tmp_buf[1];
    aos_status_t r;

    if (!aos_devfs_file_is_readable(file))
        return -EPERM;

    if (count < 1)
        return -EINVAL;

    if (buf && !aos_umem_check(buf, 1))
        return -EFAULT;

    r = aos_gpio_get_value(ref);
    if (r < 0)
        return (ssize_t)r;

    tmp_buf[0] = (r == 0) ? 0 : 1;

    if (aos_umem_copy(buf, tmp_buf, 1))
        return -EFAULT;

    return 1;
}

static ssize_t devfs_gpio_write(aos_devfs_file_t *file, const void *buf, size_t count)
{
    aos_gpio_ref_t *ref = aos_devfs_file2ref(file);
    uint8_t tmp_buf[1];
    aos_status_t r;

    if (!aos_devfs_file_is_writable(file))
        return -EPERM;

    if (count < 1)
        return -EINVAL;

    if (buf && !aos_umem_check(buf, 1))
        return -EFAULT;

    if (aos_umem_copy(tmp_buf, buf, 1))
        return -EFAULT;

    r = aos_gpio_set_value(ref, tmp_buf[0] == 0 ? 0 : 1);

    return (r < 0) ? (ssize_t)r : 1;
}

static const aos_devfs_file_ops_t devfs_gpio_ops = {
    .ioctl      = devfs_gpio_ioctl,
    .poll       = devfs_gpio_poll,
    .mmap       = NULL,
    .read       = devfs_gpio_read,
    .write      = devfs_gpio_write,
    .lseek      = NULL,
};

#endif /* AOS_COMP_DEVFS */

aos_status_t aos_gpio_register(aos_gpio_t *gpio)
{
#ifdef AOS_COMP_DEVFS
    int name_len;
#endif

    if (!gpio)
        return -EINVAL;

    gpio->dev.type = AOS_DEV_TYPE_GPIO;
    gpio->dev.ops = &dev_gpio_ops;
#ifdef AOS_COMP_DEVFS
    aos_devfs_node_init(&gpio->dev.devfs_node);
    gpio->dev.devfs_node.ops = &devfs_gpio_ops;
    name_len = snprintf(gpio->dev.devfs_node.name, sizeof(gpio->dev.devfs_node.name), "gpio%" PRIu32, gpio->dev.id);
    if (name_len < 0 || name_len >= sizeof(gpio->dev.devfs_node.name))
        return -EINVAL;
#endif

    return aos_dev_register(&gpio->dev);
}

aos_status_t aos_gpio_register_argumented(aos_gpio_t *gpio, uint32_t id, uint32_t gpioc_id, uint32_t pin)
{
    if (!gpio)
        return -EINVAL;

    gpio->dev.id = id;
    gpio->gpioc_id = gpioc_id;
    gpio->pin = pin;

    return aos_gpio_register(gpio);
}

aos_status_t aos_gpio_unregister(uint32_t id)
{
    return aos_dev_unregister(AOS_DEV_TYPE_GPIO, id);
}
