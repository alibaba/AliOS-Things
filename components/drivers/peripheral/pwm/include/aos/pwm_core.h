/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#ifndef _AOS_PWM_CORE_H
#define _AOS_PWM_CORE_H

#include <stdint.h>
#include <aos/kernel.h>
#include <aos/device_core.h>
#include <aos/pwm.h>

/** @defgroup driver_api driver
 *  @ingroup aos_components
 * @{
 */

/** @} */

/**
 * @defgroup aos_pwm_driver PWM驱动操作
 * @ingroup driver_api
 * 给PWM驱动提供PWM设备注册/注销操作.
 *
 * @{
 */

struct aos_pwm_ops;

typedef struct aos_pwm {
    aos_dev_t dev;

    const struct aos_pwm_ops *ops; /**< PWM设备的底层操作, 设备注册前必须赋值 */
    uint32_t period; /**< PWM设备周期(单位:ns) */
    uint32_t duty_cycle; /**< PWM设备占空比(单位:ns) */
    uint32_t polarity;
    bool enabled;
    void *priv; /**< 私有数据 */
} aos_pwm_t;

typedef struct aos_pwm_ops {
    /**< PWM设备的注销操作（可选） */
    void (*unregister)(aos_pwm_t *pwm);
    /**< PWM设备的前期准备 */
    aos_status_t (*startup)(aos_pwm_t *pwm);
    /**< PWM设备的停止波形输出 */
    void (*shutdown)(aos_pwm_t *pwm);
    /**< 设置PWM设备的频率 */
    aos_status_t (*apply)(aos_pwm_t *pwm, aos_pwm_attr_t const *attr);
} aos_pwm_ops_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 注册一个PWM设备
 *
 * @param[in]   pwm      要注册的PWM设备，调用者应该分配内存并初始化设置它的ID
 *
 * @return      0: 成功; <0: 失败
 */
aos_status_t aos_pwm_register(aos_pwm_t *pwm);

/**
 * 注销一个PWM设备
 *
 * @param[in]   id      PWM设备的ID
 *
 * @return      0: 成功; <0: 失败
 */
aos_status_t aos_pwm_unregister(uint32_t id);

/** @} */
#ifdef __cplusplus
}
#endif

#endif /* _AOS_PWM_CORE_H */
