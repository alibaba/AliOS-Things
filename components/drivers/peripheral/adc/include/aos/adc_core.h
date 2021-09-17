/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#ifndef _AOS_ADC_CORE_H
#define _AOS_ADC_CORE_H

#include <stdint.h>
#include <aos/kernel.h>
#include <aos/device_core.h>
#include <aos/adc.h>

/** @defgroup driver_api driver
 *  @ingroup aos_components
 * @{
 */

/** @} */

/**
 * @defgroup aos_adc_driver ADC驱动操作
 * @ingroup driver_api
 * 给ADC驱动提供ADC设备注册/注销操作.
 *
 * @{
 */

struct aos_adc_ops;

typedef struct aos_adc {
    aos_dev_t dev;

    const struct aos_adc_ops *ops; /**< ADC设备的底层操作, 设备注册前必须赋值 */
    uint32_t resolution; /**< ADC设备分辨率(单位:bits), 设备注册前必须赋值 */
    uint32_t freq; /**< ADC设备时钟频率(单位:HZ), 设备注册前必须赋值 */
    void *priv; /**< 私有数据 */
} aos_adc_t;

typedef struct aos_adc_ops {
    /**< ADC设备的注销操作（可选） */
    void (*unregister)(aos_adc_t *adc);
    /**< ADC设备的开始采样 */
    aos_status_t (*startup)(aos_adc_t *adc);
    /**< ADC设备的停止采样 */
    void (*shutdown)(aos_adc_t *adc);
    /**< 读取ADC设备的原始数据 */
    aos_status_t (*read)(aos_adc_t *adc, int32_t channel, int32_t *data);
    /**< 读取ADC设备的转换后的电压值（单位: mV）*/
    aos_status_t (*read_voltage)(aos_adc_t *adc, int32_t channel, int32_t *data);
    /**< 设置ADC设备的通道（开启与关闭）（可选）*/
    aos_status_t (*set_channel)(aos_adc_t *adc, int32_t channel, bool enable);
    /**< 设置ADC设备的采样时间（单位: 时钟周期的倍数）（可选）*/
    aos_status_t (*set_sample_time)(aos_adc_t *adc, int32_t channel, uint32_t clock);
    /**< 设置ADC设备的工作模式（可选）*/
    aos_status_t (*set_mode)(aos_adc_t *adc, aos_adc_mode_t clock);
    /**< 获取ADC设备的电压量程 */
    aos_status_t (*get_range)(aos_adc_t *adc, int32_t channel, int32_t *range);
} aos_adc_ops_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 注册一个ADC设备
 *
 * @param[in]   adc      要注册的ADC设备，调用者应该分配内存并初始化设置它的ID
 *
 * @return      0: 成功; <0: 失败
 */
aos_status_t aos_adc_register(aos_adc_t *adc);

/**
 * 注销一个ADC设备
 *
 * @param[in]   id      ADC设备的ID
 *
 * @return      0: 成功; <0: 失败
 */
aos_status_t aos_adc_unregister(uint32_t id);

/** @} */
#ifdef __cplusplus
}
#endif

#endif /* _AOS_ADC_CORE_H */
