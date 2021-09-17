/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <errno.h>
#include <stdbool.h>
#include <aos/kernel.h>
#include <aos/list.h>
#include <aos/adc.h>
#include <aos/adc_core.h>

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

aos_status_t aos_adc_register(aos_adc_t *adc)
{
    aos_status_t ret;

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
#ifdef AOS_COMP_VFS
    /* TODO: support vfs ops. */
    adc->dev.vfs_helper.name[0] = '\0';
    adc->dev.vfs_helper.ops = NULL;
#endif

    return aos_dev_register(&(adc->dev));
}

aos_status_t aos_adc_unregister(uint32_t id)
{
    return aos_dev_unregister(AOS_DEV_TYPE_ADC, id);
}
