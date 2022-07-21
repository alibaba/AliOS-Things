/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#ifndef AOS_ADC_H
#define AOS_ADC_H

#ifdef AOS_KERNEL_BUILD
#include <aos/device.h>
#else
#include <stdint.h>
#endif

/**
 * @defgroup adc_api ADC
 * @ingroup driver_api
 * @brief AOS API for ADC.
 * @{
 */

typedef enum {
    AOS_ADC_MODE_SINGLE,   /**< 单次采样模式 */
    AOS_ADC_MODE_CONTINUE, /**< 连续采样模式 */
} aos_adc_mode_t;

#if (defined(AOS_KERNEL_BUILD) && defined(AOS_COMP_DEVFS)) || !defined(AOS_KERNEL_BUILD)

typedef struct {
    int32_t channel;
    uint32_t time;
} aos_adc_set_sample_time_args_t;

typedef struct {
    int32_t channel;
    uint32_t range;
} aos_adc_get_range_args_t;

typedef struct {
    int32_t channel;
    int32_t data;
} aos_adc_read_args_t;

#define AOS_ADC_IOC_SET_SAMPLE_TIME     0x4101
#define AOS_ADC_IOC_SET_MODE            0x4102
#define AOS_ADC_IOC_GET_RESOLUTION      0x4103
#define AOS_ADC_IOC_GET_RANGE           0x4104
#define AOS_ADC_IOC_READ                0x4105
#define AOS_ADC_IOC_READ_VOLTAGE        0x4106

#endif /* (defined(AOS_KERNEL_BUILD) && defined(AOS_COMP_DEVFS)) || !defined(AOS_KERNEL_BUILD) */

#ifdef AOS_KERNEL_BUILD

typedef aos_dev_ref_t aos_adc_ref_t;  /**< ADC设备的引用 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 获取一个ADC设备的引用
 *
 * @param[out]  ref     ADC设备的引用
 * @param[in]   id      ADC设备ID
 *
 * @return  0: 成功  <0: 失败
 */
aos_status_t aos_adc_get(aos_adc_ref_t *ref, uint32_t id);

/**
 * 释放一个ADC设备的引用
 *
 * @param[in]   ref     ADC设备的引用
 *
 * @return      无
 */
void aos_adc_put(aos_adc_ref_t *ref);

/**
 * 设置ADC设备的一个通道的采样时间
 *
 * @param[in]   ref     ADC设备的引用
 * @param[in]   channel >=0: 一个通道的ID -1: ADC设备的所有通道
 * @param[in]   time    采样时间（单位：us）
 *
 * @return      0: 成功  <0: 失败
 */
aos_status_t aos_adc_set_sample_time(aos_adc_ref_t *ref, int32_t channel, uint32_t time);

/**
 * 设置ADC设备的工作模式
 *
 * @param[in]   ref     ADC设备的引用
 * @param[in]   mode    工作模式, 当前支持的模式:\n
 *                      @ref AOS_ADC_MODE_SINGLE: 单次采样模式\n
 *                      @ref AOS_ADC_MODE_CONTINUE: 连续采样模式\n
 *
 * @return      0: 成功  <0: 失败
 */
aos_status_t aos_adc_set_mode(aos_adc_ref_t *ref, aos_adc_mode_t mode);

/**
 * 获取ADC设备的分辨率
 *
 * @param[in]   ref         ADC设备的引用
 * @param[out]  resolution  ADC设备的分辨率 (单位:bits)
 *
 * @return      0: 成功  <0: 失败
 */
aos_status_t aos_adc_get_resolution(aos_adc_ref_t *ref, uint32_t *resolution);

/**
 * 获取ADC设备的电压量程（单位:mV）
 *
 * @param[in]   ref         ADC设备的引用
 * @param[in]   channel     >=0: 一个通道的ID -1: ADC设备的所有通道（相同的量程）
 * @param[out]  range       ADC设备的电压量程（单位:mV）
 *
 * @return      0: 成功  <0: 失败
 */
aos_status_t aos_adc_get_range(aos_adc_ref_t *ref, int32_t channel, uint32_t *range);

/**
 * 从ADC设备的一个通道读取原始数据 (同步方式)
 *
 * @param[in]   ref      ADC设备的引用
 * @param[in]   channel  通道ID
 * @param[out]  data     输出的ADC原始数据
 *
 * @return      0: 成功  <0: 失败
 */
aos_status_t aos_adc_read(aos_adc_ref_t *ref, int32_t channel, int32_t *data);

/**
 * 从ADC设备的一个通道读取转换后的电压值（单位: mV） (同步方式)
 *
 * @param[in]   ref      ADC设备的引用
 * @param[in]   channel  通道ID
 * @param[out]  data     输出的ADC转换后电压值（单位: mV）
 *
 * @return      0: 成功  <0: 失败
 */
aos_status_t aos_adc_read_voltage(aos_adc_ref_t *ref, int32_t channel, int32_t *data);

#ifdef __cplusplus
}
#endif

#endif /* AOS_KERNEL_BUILD */

/** @} */

#endif /* AOS_ADC_H */
