/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _IO_ADC_H_
#define _IO_ADC_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup driver_api driver
 *  @ingroup aos_components
 * @{
 */

/**
 * @}
 */

/** @defgroup adc_device_api adc
 *  @ingroup driver_api
 * @{
 */

/* ADC DEVICE IOCTL CMD宏定义 */
#define IOC_ADC_BASE 'A'
#define IOC_ADC_START IOC_ADC_BASE + 1 /**< 启动ADC设备*/
#define IOC_ADC_STOP IOC_ADC_BASE + 2  /**< 停止ADC设备*/
#define IOC_ADC_GET_VALUE IOC_ADC_BASE + 3 /**< 从ADC设备读取数据 */

/* ADC 读取数据的参数 */
typedef struct io_adc_arg {
    unsigned int value;   /**< 出参，读取到的数据 */
    unsigned int timeout; /**< 入参，超时时间（单位：ms）*/
} io_adc_arg_t;

/**
 * 注册ADC设备驱动到VFS框架
 *
 * @return  0：成功，否则失败
 */
int vfs_dac_drv_init (void);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif /*_IO_ADC_H_*/
