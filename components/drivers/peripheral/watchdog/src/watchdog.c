/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <aos/watchdog_core.h>
#ifdef AOS_COMP_DEVFS
#include <stdio.h>
#include <inttypes.h>
#endif

aos_status_t aos_watchdog_get(aos_watchdog_ref_t *ref, uint32_t id)
{
    return aos_dev_get(ref, AOS_DEV_TYPE_WATCHDOG, id);
}

void aos_watchdog_put(aos_watchdog_ref_t *ref)
{
    aos_dev_put(ref);
}

aos_status_t aos_watchdog_get_info(aos_watchdog_ref_t *ref, aos_watchdog_info_t *info)
{
    aos_watchdog_t *watchdog;

    if (!ref || !aos_dev_ref_is_valid(ref) || !info)
        return -EINVAL;

    watchdog = aos_container_of(ref->dev, aos_watchdog_t, dev);
    info->flags = watchdog->flags;
    aos_dev_lock(ref->dev);
    info->timeout = watchdog->timeout;
    aos_dev_unlock(ref->dev);

    return 0;
}

aos_status_t aos_watchdog_set_timeout(aos_watchdog_ref_t *ref, uint32_t timeout)
{
    aos_watchdog_t *watchdog;
    uint32_t old_timeout;
    aos_status_t ret;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    watchdog = aos_container_of(ref->dev, aos_watchdog_t, dev);
    aos_dev_lock(ref->dev);
    old_timeout = watchdog->timeout;
    watchdog->timeout = timeout;
    ret = watchdog->ops->set_timeout(watchdog);
    if (ret) {
        watchdog->timeout = old_timeout;
        aos_dev_unlock(ref->dev);
        return ret;
    }

    aos_dev_unlock(ref->dev);

    return 0;
}

aos_status_t aos_watchdog_get_timeleft(aos_watchdog_ref_t *ref, uint32_t *timeleft)
{
    aos_watchdog_t *watchdog;

    if (!ref || !aos_dev_ref_is_valid(ref) || !timeleft)
        return -EINVAL;

    watchdog = aos_container_of(ref->dev, aos_watchdog_t, dev);

    if (!watchdog->ops->get_timeleft)
        return -ENOTSUP;

    aos_dev_lock(ref->dev);

    if (watchdog->timeout == 0) {
        aos_dev_unlock(ref->dev);
        return -ENOTSUP;
    }

    *timeleft = watchdog->ops->get_timeleft(watchdog);
    aos_dev_unlock(ref->dev);

    return 0;
}

aos_status_t aos_watchdog_kick(aos_watchdog_ref_t *ref)
{
    aos_watchdog_t *watchdog;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    watchdog = aos_container_of(ref->dev, aos_watchdog_t, dev);
    aos_dev_lock(ref->dev);

    if (watchdog->timeout == 0) {
        aos_dev_unlock(ref->dev);
        return -ENOTSUP;
    }

    watchdog->ops->kick(watchdog);
    aos_dev_unlock(ref->dev);

    return 0;
}

static void dev_watchdog_unregister(aos_dev_t *dev)
{
    aos_watchdog_t *watchdog = aos_container_of(dev, aos_watchdog_t, dev);

    if (watchdog->ops->unregister)
        watchdog->ops->unregister(watchdog);
}

static aos_status_t dev_watchdog_get(aos_dev_ref_t *ref)
{
    return 0;
}

static void dev_watchdog_put(aos_dev_ref_t *ref)
{
    aos_watchdog_t *watchdog = aos_container_of(ref->dev, aos_watchdog_t, dev);

    if (aos_dev_ref_is_last(ref) && (watchdog->flags & AOS_WATCHDOG_F_DISABLE_ON_PUT)) {
        watchdog->timeout = 0;
        (void)watchdog->ops->set_timeout(watchdog);
    }
}

static const aos_dev_ops_t dev_watchdog_ops = {
    .unregister = dev_watchdog_unregister,
    .get        = dev_watchdog_get,
    .put        = dev_watchdog_put,
};

#ifdef AOS_COMP_DEVFS

static aos_status_t devfs_watchdog_ioctl(aos_devfs_file_t *file, int cmd, uintptr_t arg)
{
    aos_watchdog_ref_t *ref = aos_devfs_file2ref(file);
    aos_status_t ret;

    switch (cmd) {
    case AOS_WATCHDOG_IOC_GET_INFO:
        {
            aos_watchdog_info_t info;
            if (!aos_devfs_file_is_readable(file)) {
                ret = -EPERM;
                break;
            }
            ret = aos_watchdog_get_info(ref, &info);
            if (ret)
                break;
            if (!aos_umem_check((const void *)arg, sizeof(info))) {
                ret = -EFAULT;
                break;
            }
            ret = aos_umem_copy((void *)arg, &info, sizeof(info)) ? -EFAULT : 0;
        }
        break;
    case AOS_WATCHDOG_IOC_SET_TIMEOUT:
        {
            uint32_t timeout;
            if (!aos_devfs_file_is_writable(file)) {
                ret = -EPERM;
                break;
            }
            if (!aos_umem_check((const void *)arg, sizeof(timeout))) {
                ret = -EFAULT;
                break;
            }
            if (aos_umem_copy(&timeout, (const void *)arg, sizeof(timeout))) {
                ret = -EFAULT;
                break;
            }
            ret = aos_watchdog_set_timeout(ref, timeout);
        }
        break;
    case AOS_WATCHDOG_IOC_GET_TIMELEFT:
        {
            uint32_t timeleft;
            if (!aos_devfs_file_is_readable(file)) {
                ret = -EPERM;
                break;
            }
            ret = aos_watchdog_get_timeleft(ref, &timeleft);
            if (ret)
                break;
            if (!aos_umem_check((const void *)arg, sizeof(timeleft))) {
                ret = -EFAULT;
                break;
            }
            ret = aos_umem_copy((void *)arg, &timeleft, sizeof(timeleft)) ? -EFAULT : 0;
        }
        break;
    case AOS_WATCHDOG_IOC_KICK:
        if (!aos_devfs_file_is_writable(file)) {
            ret = -EPERM;
            break;
        }
        ret = aos_watchdog_kick(ref);
        break;
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

static const aos_devfs_file_ops_t devfs_watchdog_ops = {
    .ioctl      = devfs_watchdog_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

#endif /* AOS_COMP_DEVFS */

aos_status_t aos_watchdog_register(aos_watchdog_t *watchdog)
{
#ifdef AOS_COMP_DEVFS
    int name_len;
#endif

    if (!watchdog)
        return -EINVAL;

    if (!watchdog->ops || !watchdog->ops->set_timeout || !watchdog->ops->kick)
        return -EINVAL;

    watchdog->dev.type = AOS_DEV_TYPE_WATCHDOG;
    watchdog->dev.ops = &dev_watchdog_ops;
#ifdef AOS_COMP_DEVFS
    aos_devfs_node_init(&watchdog->dev.devfs_node);
    watchdog->dev.devfs_node.ops = &devfs_watchdog_ops;
    name_len = snprintf(watchdog->dev.devfs_node.name, sizeof(watchdog->dev.devfs_node.name),
                        "watchdog%" PRIu32, watchdog->dev.id);
    if (name_len < 0 || name_len >= sizeof(watchdog->dev.devfs_node.name))
        return -EINVAL;
#endif

    return aos_dev_register(&watchdog->dev);
}

aos_status_t aos_watchdog_register_argumented(aos_watchdog_t *watchdog, uint32_t id,
                                              const aos_watchdog_ops_t *ops, uint32_t flags)
{
    if (!watchdog)
        return -EINVAL;

    watchdog->dev.id = id;
    watchdog->ops = ops;
    watchdog->flags = flags;

    return aos_watchdog_register(watchdog);
}

aos_status_t aos_watchdog_unregister(uint32_t id)
{
    return aos_dev_unregister(AOS_DEV_TYPE_WATCHDOG, id);
}
