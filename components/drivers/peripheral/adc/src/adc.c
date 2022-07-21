/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#include <aos/adc_core.h>
#ifdef AOS_COMP_DEVFS
#include <stdio.h>
#include <inttypes.h>
#endif

#define AOS_ADC_CHECK_NULL(x) \
    do { \
        if ((x) == NULL) { \
            return -EINVAL; \
        } \
    } while (0)

aos_status_t aos_adc_get(aos_adc_ref_t *ref, uint32_t id)
{
    AOS_ADC_CHECK_NULL(ref);

    return aos_dev_get(ref, AOS_DEV_TYPE_ADC, id);
}

void aos_adc_put(aos_adc_ref_t *ref)
{
    if (ref == NULL)
        return;

    aos_dev_put(ref);
}

aos_status_t aos_adc_set_sample_time(aos_adc_ref_t *ref, int32_t channel, uint32_t time)
{
    aos_status_t ret;
    aos_adc_t *adc;

    AOS_ADC_CHECK_NULL(ref);
    adc = aos_container_of(ref->dev, aos_adc_t, dev);
    if (adc->ops->set_sample_time == NULL)
        return -ENOTSUP;

    aos_dev_lock(ref->dev);
    /* Convert time(unit:us) into the number of adc clock cycle. */
    ret = adc->ops->set_sample_time(adc, channel, (time * adc->freq) / 1000000U);
    aos_dev_unlock(ref->dev);
    return ret;
}

aos_status_t aos_adc_set_mode(aos_adc_ref_t *ref, aos_adc_mode_t mode)
{
    aos_status_t ret;
    aos_adc_t *adc;

    AOS_ADC_CHECK_NULL(ref);
    adc = aos_container_of(ref->dev, aos_adc_t, dev);
    if (adc->ops->set_mode == NULL)
        return -ENOTSUP;

    aos_dev_lock(ref->dev);
    ret = adc->ops->set_mode(adc, mode);
    aos_dev_unlock(ref->dev);
    return ret;
}

aos_status_t aos_adc_get_resolution(aos_adc_ref_t *ref, uint32_t *resolution)
{
    aos_adc_t *adc;

    AOS_ADC_CHECK_NULL(ref);
    AOS_ADC_CHECK_NULL(resolution);

    adc = aos_container_of(ref->dev, aos_adc_t, dev);
    AOS_ADC_CHECK_NULL(adc);
    *resolution = adc->resolution;

    return 0;
}

aos_status_t aos_adc_get_range(aos_adc_ref_t *ref, int32_t channel, uint32_t *range)
{
    aos_status_t ret;
    aos_adc_t *adc;

    AOS_ADC_CHECK_NULL(ref);
    AOS_ADC_CHECK_NULL(range);

    adc = aos_container_of(ref->dev, aos_adc_t, dev);
    AOS_ADC_CHECK_NULL(adc);
    if (adc->ops->get_range == NULL)
        return -ENOTSUP;

    aos_dev_lock(ref->dev);
    ret = adc->ops->get_range(adc, channel, range);
    aos_dev_unlock(ref->dev);
    return ret;
}

aos_status_t aos_adc_read(aos_adc_ref_t *ref, int32_t channel, int32_t *data)
{
    aos_status_t ret;
    aos_adc_t *adc;

    AOS_ADC_CHECK_NULL(ref);
    AOS_ADC_CHECK_NULL(data);

    adc = aos_container_of(ref->dev, aos_adc_t, dev);
    AOS_ADC_CHECK_NULL(adc);

    aos_dev_lock(ref->dev);
    ret = adc->ops->read(adc, channel, data);
    aos_dev_unlock(ref->dev);
    return ret;
}

aos_status_t aos_adc_read_voltage(aos_adc_ref_t *ref, int32_t channel, int32_t *data)
{
    aos_status_t ret;
    aos_adc_t *adc;

    AOS_ADC_CHECK_NULL(ref);
    AOS_ADC_CHECK_NULL(data);

    adc = aos_container_of(ref->dev, aos_adc_t, dev);
    AOS_ADC_CHECK_NULL(adc);

    aos_dev_lock(ref->dev);
    ret = adc->ops->read_voltage(adc, channel, data);
    aos_dev_unlock(ref->dev);
    return ret;
}

static void dev_adc_unregister(aos_dev_t *dev)
{
    aos_adc_t *adc = aos_container_of(dev, aos_adc_t, dev);

    if (adc->ops->unregister)
        adc->ops->unregister(adc);
}

static aos_status_t dev_adc_get(aos_dev_ref_t *ref)
{
    aos_adc_t *adc = aos_container_of(ref->dev, aos_adc_t, dev);

    if (!aos_dev_ref_is_first(ref))
        return 0;

    return adc->ops->startup(adc);
}

static void dev_adc_put(aos_dev_ref_t *ref)
{
    aos_adc_t *adc = aos_container_of(ref->dev, aos_adc_t, dev);

    if (!aos_dev_ref_is_last(ref))
        return;

    adc->ops->shutdown(adc);
}

static const aos_dev_ops_t dev_adc_ops = {
    .unregister = dev_adc_unregister,
    .get = dev_adc_get,
    .put = dev_adc_put,
};

#ifdef AOS_COMP_DEVFS

static aos_status_t devfs_adc_ioctl(aos_devfs_file_t *file, int cmd, uintptr_t arg)
{
    aos_adc_ref_t *ref = aos_devfs_file2ref(file);
    aos_status_t ret;

    switch (cmd) {
    case AOS_ADC_IOC_SET_SAMPLE_TIME:
        {
            aos_adc_set_sample_time_args_t args;
            if (!aos_devfs_file_is_writable(file)) {
                ret = -EPERM;
                break;
            }
            if (!aos_umem_check((const void *)arg, sizeof(args))) {
                ret = -EFAULT;
                break;
            }
            if (aos_umem_copy(&args, (const void *)arg, sizeof(args))) {
                ret = -EFAULT;
                break;
            }
            ret = aos_adc_set_sample_time(ref, args.channel, args.time);
        }
        break;
    case AOS_ADC_IOC_SET_MODE:
        {
            aos_adc_mode_t mode;
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
            ret = aos_adc_set_mode(ref, mode);
        }
        break;
    case AOS_ADC_IOC_GET_RESOLUTION:
        {
            uint32_t resolution;
            if (!aos_devfs_file_is_readable(file)) {
                ret = -EPERM;
                break;
            }
            ret = aos_adc_get_resolution(ref, &resolution);
            if (ret)
                break;
            if (!aos_umem_check((const void *)arg, sizeof(resolution))) {
                ret = -EFAULT;
                break;
            }
            ret = aos_umem_copy((void *)arg, &resolution, sizeof(resolution)) ? -EFAULT : 0;
        }
        break;
    case AOS_ADC_IOC_GET_RANGE:
        {
            aos_adc_get_range_args_t args;
            if (!aos_devfs_file_is_readable(file)) {
                ret = -EPERM;
                break;
            }
            if (!aos_umem_check((const void *)arg, sizeof(args))) {
                ret = -EFAULT;
                break;
            }
            if (aos_umem_copy(&args, (const void *)arg, sizeof(args))) {
                ret = -EFAULT;
                break;
            }
            ret = aos_adc_get_range(ref, args.channel, &args.range);
            if (ret)
                break;
            ret = aos_umem_copy((void *)arg, &args, sizeof(args)) ? -EFAULT : 0;
        }
        break;
    case AOS_ADC_IOC_READ:
        {
            aos_adc_read_args_t args;
            if (!aos_devfs_file_is_readable(file)) {
                ret = -EPERM;
                break;
            }
            if (!aos_umem_check((const void *)arg, sizeof(args))) {
                ret = -EFAULT;
                break;
            }
            if (aos_umem_copy(&args, (const void *)arg, sizeof(args))) {
                ret = -EFAULT;
                break;
            }
            ret = aos_adc_read(ref, args.channel, &args.data);
            if (ret)
                break;
            ret = aos_umem_copy((void *)arg, &args, sizeof(args)) ? -EFAULT : 0;
        }
        break;
    case AOS_ADC_IOC_READ_VOLTAGE:
        {
            aos_adc_read_args_t args;
            if (!aos_devfs_file_is_readable(file)) {
                ret = -EPERM;
                break;
            }
            if (!aos_umem_check((const void *)arg, sizeof(args))) {
                ret = -EFAULT;
                break;
            }
            if (aos_umem_copy(&args, (const void *)arg, sizeof(args))) {
                ret = -EFAULT;
                break;
            }
            ret = aos_adc_read_voltage(ref, args.channel, &args.data);
            if (ret)
                break;
            ret = aos_umem_copy((void *)arg, &args, sizeof(args)) ? -EFAULT : 0;
        }
        break;
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

static const aos_devfs_file_ops_t devfs_adc_ops = {
    .ioctl      = devfs_adc_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

#endif /* AOS_COMP_DEVFS */

aos_status_t aos_adc_register(aos_adc_t *adc)
{
#ifdef AOS_COMP_DEVFS
    int name_len;
#endif

    AOS_ADC_CHECK_NULL(adc);

    /* startup/shutdown/read must be set. */
    if ((adc->ops == NULL) || (adc->ops->startup == NULL) ||
        (adc->ops->shutdown == NULL) || (adc->ops->read == NULL) ||
        (adc->ops->read_voltage == NULL))
        return -EINVAL;

    if ((adc->resolution == 0) || (adc->freq == 0))
        return -EINVAL;

    adc->dev.type = AOS_DEV_TYPE_ADC;
    adc->dev.ops = &dev_adc_ops;
#ifdef AOS_COMP_DEVFS
    aos_devfs_node_init(&adc->dev.devfs_node);
    adc->dev.devfs_node.ops = &devfs_adc_ops;
    name_len = snprintf(adc->dev.devfs_node.name, sizeof(adc->dev.devfs_node.name), "adc%" PRIu32, adc->dev.id);
    if (name_len < 0 || name_len >= sizeof(adc->dev.devfs_node.name))
        return -EINVAL;
#endif

    return aos_dev_register(&(adc->dev));
}

aos_status_t aos_adc_unregister(uint32_t id)
{
    return aos_dev_unregister(AOS_DEV_TYPE_ADC, id);
}
