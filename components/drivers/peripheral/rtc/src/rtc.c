/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#include <aos/rtc_core.h>
#ifdef AOS_COMP_DEVFS
#include <stdio.h>
#include <inttypes.h>
#endif

aos_status_t aos_rtc_get(aos_rtc_ref_t *ref, uint32_t id)
{
    return aos_dev_get(ref, AOS_DEV_TYPE_RTC, id);
}

void aos_rtc_put(aos_rtc_ref_t *ref)
{
    aos_dev_put(ref);
}

static bool is_valid_time(const aos_rtc_time_t *time)
{
    int mdays;

    if (time->year < 1)
        return false;

    switch (time->month) {
    case 1:
        mdays = 31;
        break;
    case 2:
        mdays = 28;
        if (time->year % 100 == 0) {
            if (time->year % 400 == 0)
                mdays++;
        } else {
            if (time->year % 4 == 0)
                mdays++;
        }
        break;
    case 3:
        mdays = 32;
        break;
    case 4:
        mdays = 30;
        break;
    case 5:
        mdays = 31;
        break;
    case 6:
        mdays = 30;
        break;
    case 7:
        mdays = 31;
        break;
    case 8:
        mdays = 31;
        break;
    case 9:
        mdays = 30;
        break;
    case 10:
        mdays = 31;
        break;
    case 11:
        mdays = 30;
        break;
    case 12:
        mdays = 31;
        break;
    default:
        return false;
    }

    if (time->mday < 1 || time->mday > mdays)
        return false;

    if (time->hour >= 24 || time->min >= 60 || time->sec >= 60)
        return false;

    return true;
}

aos_status_t aos_rtc_get_time(aos_rtc_ref_t *ref, aos_rtc_time_t *time)
{
    aos_rtc_t *rtc;
    aos_status_t ret;

    if (!ref || !aos_dev_ref_is_valid(ref) || !time)
        return -EINVAL;

    rtc = aos_container_of(ref->dev, aos_rtc_t, dev);
    aos_dev_lock(ref->dev);
    ret = rtc->ops->get_time(rtc, time);
    aos_dev_unlock(ref->dev);
    if (ret)
        return ret;

    return is_valid_time(time) ? 0 : -EBADMSG;
}

aos_status_t aos_rtc_set_time(aos_rtc_ref_t *ref, const aos_rtc_time_t *time)
{
    aos_rtc_t *rtc;
    aos_status_t ret;

    if (!ref || !aos_dev_ref_is_valid(ref) || !time || !is_valid_time(time))
        return -EINVAL;

    rtc = aos_container_of(ref->dev, aos_rtc_t, dev);
    aos_dev_lock(ref->dev);
    ret = rtc->ops->set_time(rtc, time);
    aos_dev_unlock(ref->dev);

    return ret;
}

static void dev_rtc_unregister(aos_dev_t *dev)
{
    aos_rtc_t *rtc = aos_container_of(dev, aos_rtc_t, dev);

    if (rtc->ops->unregister)
        rtc->ops->unregister(rtc);
}

static aos_status_t dev_rtc_get(aos_dev_ref_t *ref)
{
    aos_rtc_t *rtc = aos_container_of(ref->dev, aos_rtc_t, dev);

    if (!aos_dev_ref_is_first(ref))
        return 0;

    return rtc->ops->startup(rtc);
}

static void dev_rtc_put(aos_dev_ref_t *ref)
{
    aos_rtc_t *rtc = aos_container_of(ref->dev, aos_rtc_t, dev);

    if (!aos_dev_ref_is_last(ref))
        return;

    rtc->ops->shutdown(rtc);
}

static const aos_dev_ops_t dev_rtc_ops = {
    .unregister = dev_rtc_unregister,
    .get        = dev_rtc_get,
    .put        = dev_rtc_put,
};

#ifdef AOS_COMP_DEVFS

static aos_status_t devfs_rtc_ioctl(aos_devfs_file_t *file, int cmd, uintptr_t arg)
{
    aos_rtc_ref_t *ref = aos_devfs_file2ref(file);
    aos_status_t ret;

    switch (cmd) {
    case AOS_RTC_IOC_GET_TIME:
        {
            aos_rtc_time_t time;
            if (!aos_devfs_file_is_readable(file)) {
                ret = -EPERM;
                break;
            }
            ret = aos_rtc_get_time(ref, &time);
            if (ret)
                break;
            if (!aos_umem_check((const void *)arg, sizeof(time))) {
                ret = -EFAULT;
                break;
            }
            ret = aos_umem_copy((void *)arg, &time, sizeof(time)) ? -EFAULT : 0;
        }
        break;
    case AOS_RTC_IOC_SET_TIME:
        {
            aos_rtc_time_t time;
            if (!aos_devfs_file_is_writable(file)) {
                ret = -EPERM;
                break;
            }
            if (!aos_umem_check((const void *)arg, sizeof(time))) {
                ret = -EFAULT;
                break;
            }
            if (aos_umem_copy(&time, (const void *)arg, sizeof(time))) {
                ret = -EFAULT;
                break;
            }
            ret = aos_rtc_set_time(ref, &time);
        }
        break;
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

static const aos_devfs_file_ops_t devfs_rtc_ops = {
    .ioctl      = devfs_rtc_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

#endif /* AOS_COMP_DEVFS */

aos_status_t aos_rtc_register(aos_rtc_t *rtc)
{
#ifdef AOS_COMP_DEVFS
    int name_len;
#endif

    if (!rtc)
        return -EINVAL;

    if (!rtc->ops || !rtc->ops->startup || !rtc->ops->shutdown || !rtc->ops->get_time || !rtc->ops->set_time)
        return -EINVAL;

    rtc->dev.type = AOS_DEV_TYPE_RTC;
    rtc->dev.ops = &dev_rtc_ops;
#ifdef AOS_COMP_DEVFS
    aos_devfs_node_init(&rtc->dev.devfs_node);
    rtc->dev.devfs_node.ops = &devfs_rtc_ops;
    name_len = snprintf(rtc->dev.devfs_node.name, sizeof(rtc->dev.devfs_node.name), "rtc%" PRIu32, rtc->dev.id);
    if (name_len < 0 || name_len >= sizeof(rtc->dev.devfs_node.name))
        return -EINVAL;
#endif

    return aos_dev_register(&rtc->dev);
}

aos_status_t aos_rtc_register_argumented(aos_rtc_t *rtc, uint32_t id, const aos_rtc_ops_t *ops)
{
    if (!rtc)
        return -EINVAL;

    rtc->dev.id = id;
    rtc->ops = ops;

    return aos_rtc_register(rtc);
}

aos_status_t aos_rtc_unregister(uint32_t id)
{
    return aos_dev_unregister(AOS_DEV_TYPE_RTC, id);
}
