/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#ifndef AOS_PWM_H
#define AOS_PWM_H

#ifdef AOS_KERNEL_BUILD
#include <aos/device.h>
#else
#include <stdint.h>
#endif

/**
 * @defgroup pwm_api PWM
 * @ingroup driver_api
 * @brief AOS API for PWM.
 * @{
 */

typedef enum {
    AOS_PWM_POLARITY_NORMAL,
    AOS_PWM_POLARITY_INVERSE,
} aos_pwm_polarity_t;

typedef struct {
    uint32_t period; /*in ns*/
    uint32_t duty_cycle; /*in ns*/
    aos_pwm_polarity_t polarity;
    bool enabled;
} aos_pwm_attr_t;

#if (defined(AOS_KERNEL_BUILD) && defined(AOS_COMP_DEVFS)) || !defined(AOS_KERNEL_BUILD)

#define AOS_PWM_IOC_GET_ATTR    0x5001
#define AOS_PWM_IOC_SET_ATTR    0x5002

#endif /* (defined(AOS_KERNEL_BUILD) && defined(AOS_COMP_DEVFS)) || !defined(AOS_KERNEL_BUILD) */

#ifdef AOS_KERNEL_BUILD

typedef aos_dev_ref_t aos_pwm_ref_t;  /**< PWM设备的引用 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 获取一个PWM设备的引用
 *
 * @param[out/in]  ref     PWM设备的引用
 * @param[in]   id      PWM设备ID
 *
 * @return  0: 成功  <0: 失败
 */
aos_status_t aos_pwm_get(aos_pwm_ref_t *ref, uint32_t id);

/**
 * 释放一个PWM设备的引用
 *
 * @param[out/in]   ref     PWM设备的引用
 *
 * @return      无
 */
void aos_pwm_put(aos_pwm_ref_t *ref);

/**
 * 应用pwm参数到pwm设备
 *
 * @param[in]   ref     PWM设备的引用
 * @param[in]   pwm参数    aos_pwm_attr_t
 *
 * @return      0: 成功  <0: 失败
 */
aos_status_t aos_pwm_set_attr(aos_pwm_ref_t *ref, aos_pwm_attr_t const *attr);

/**
 *获取当前pwm参数
 *
 * @param[in]   ref     PWM设备的引用
 * @param[in]   pwm参数    aos_pwm_attr_t
 *
 * @return      0: 成功 <0:失败
 */
aos_status_t aos_pwm_get_attr(aos_pwm_ref_t *ref, aos_pwm_attr_t *attr);

#ifdef __cplusplus
}
#endif

#endif /* AOS_KERNEL_BUILD */

/** @} */

#endif /* AOS_PWM_H */
