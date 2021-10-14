/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _IO_PWM_H_
#define _IO_PWM_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup pwm_device_api
 *  @ingroup driver_api
 *  PWM设备节点API定义，主要是ioctl函数所用的命令id定义说明
 *
 *  @{
 */

#define IOC_PWM_BASE 'T'
#define IOC_PWM_ON IOC_PWM_BASE + 1         /**< 使能PWM的输出功能 */
#define IOC_PWM_OFF IOC_PWM_BASE + 2        /**< 关闭PWM的输出功能 */
#define IOC_PWM_FREQ IOC_PWM_BASE + 3       /**< 设置PWM的输出频率 */
#define IOC_PWM_DUTY_CYCLE IOC_PWM_BASE + 4 /**< 设置PWM的输出占空比 */

/** @} */

#ifdef __cplusplus
}
#endif
#endif //_IO_PWM_H_
