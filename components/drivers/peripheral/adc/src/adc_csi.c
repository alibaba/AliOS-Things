/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <stdbool.h>
#include <aos/adc_core.h>
#include <aos/adc_csi.h>

static void adc_csi_unregister(aos_adc_t *adc)
{
    aos_adc_csi_t *aos_csi_adc = aos_container_of(adc, aos_adc_csi_t, aos_adc);

    csi_adc_uninit(&(aos_csi_adc->csi_adc));
}

static aos_status_t adc_csi_startup(aos_adc_t *adc)
{
    aos_adc_csi_t *aos_csi_adc = aos_container_of(adc, aos_adc_csi_t, aos_adc);

    return csi_adc_start(&(aos_csi_adc->csi_adc));
}

static void adc_csi_shutdown(aos_adc_t *adc)
{
    aos_adc_csi_t *aos_csi_adc = aos_container_of(adc, aos_adc_csi_t, aos_adc);

    (void)csi_adc_stop(&(aos_csi_adc->csi_adc));
}

static aos_status_t adc_csi_read(aos_adc_t *adc, int32_t channel, int32_t *data)
{
    aos_status_t ret;
    aos_adc_csi_t *aos_csi_adc = aos_container_of(adc, aos_adc_csi_t, aos_adc);

    ret = csi_adc_channel_enable(&(aos_csi_adc->csi_adc), (uint8_t)channel, true);
    if (ret < 0)
        return -EIO;

    ret = csi_adc_start(&(aos_csi_adc->csi_adc));
    if (ret != CSI_OK)
        return -EIO;
    *data = csi_adc_read(&(aos_csi_adc->csi_adc));
    (void)csi_adc_stop(&(aos_csi_adc->csi_adc));

    return 0;
}

static aos_status_t adc_csi_read_voltage(aos_adc_t *adc, int32_t channel, int32_t *data)
{
    aos_status_t ret;
    aos_adc_csi_t *aos_csi_adc = aos_container_of(adc, aos_adc_csi_t, aos_adc);

    ret = csi_adc_channel_enable(&(aos_csi_adc->csi_adc), (uint8_t)channel, true);
    if (ret < 0)
        return -EIO;

    ret = csi_adc_start(&(aos_csi_adc->csi_adc));
    if (ret != CSI_OK)
        return -EIO;
    *data = csi_adc_read_voltage(&(aos_csi_adc->csi_adc));
    (void)csi_adc_stop(&(aos_csi_adc->csi_adc));

    return 0;
}

static aos_status_t adc_csi_set_channel(aos_adc_t *adc, int32_t channel, bool enable)
{
    aos_adc_csi_t *aos_csi_adc = aos_container_of(adc, aos_adc_csi_t, aos_adc);

    return csi_adc_channel_enable(&(aos_csi_adc->csi_adc), (uint8_t)channel, enable);
}

static aos_status_t adc_csi_set_sample_time(aos_adc_t *adc, int32_t channel, uint32_t clock)
{
    aos_adc_csi_t *aos_csi_adc = aos_container_of(adc, aos_adc_csi_t, aos_adc);

    return csi_adc_channel_sampling_time(&(aos_csi_adc->csi_adc), (uint8_t)channel, (uint16_t)clock);
}

static aos_status_t adc_csi_set_mode(aos_adc_t *adc, aos_adc_mode_t mode)
{
    bool enable = (mode == AOS_ADC_MODE_CONTINUE);
    aos_adc_csi_t *aos_csi_adc = aos_container_of(adc, aos_adc_csi_t, aos_adc);

    return csi_adc_continue_mode(&(aos_csi_adc->csi_adc), enable);
}

static aos_status_t adc_csi_get_range(aos_adc_t *adc, int32_t channel, uint32_t *range)
{
    aos_adc_csi_t *aos_csi_adc = aos_container_of(adc, aos_adc_csi_t, aos_adc);

    return csi_adc_get_range(&(aos_csi_adc->csi_adc), (uint8_t)channel, range);
}

static const aos_adc_ops_t adc_csi_ops = {
    .unregister      = adc_csi_unregister,
    .startup         = adc_csi_startup,
    .shutdown        = adc_csi_shutdown,
    .read            = adc_csi_read,
    .read_voltage    = adc_csi_read_voltage,
    .set_channel     = adc_csi_set_channel,
    .set_sample_time = adc_csi_set_sample_time,
    .set_mode        = adc_csi_set_mode,
    .get_range       = adc_csi_get_range,
};

aos_status_t aos_adc_csi_register(aos_adc_csi_t *adc_csi)
{
    if (!adc_csi)
        return -EINVAL;

    adc_csi->aos_adc.ops = &adc_csi_ops;

    return aos_adc_register(&adc_csi->aos_adc);
}

aos_status_t aos_adc_csi_unregister(uint32_t id)
{
    return aos_adc_unregister(id);
}
