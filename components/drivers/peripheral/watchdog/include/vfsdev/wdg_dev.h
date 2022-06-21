/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _IO_WDG_H_
#define _IO_WDG_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup watchdog_device_api
 *  watchdog设备节点API定义，主要是ioctl函数所用的命令id定义说明
 *
 *  @{
 */

#define IOC_WDG_BASE 'W'
#define IOC_WDG_RELOAD IOC_WDG_BASE + 1   /**< 复位看门狗 */
#define IOC_WDG_FEED   IOC_WDG_BASE + 2   /**< 喂狗，和IOC_WDG_RELOAD相同功能 */
#define IOC_WDG_START  IOC_WDG_BASE + 3   /**< 开启看门狗，并不是所有板子都支持动态开启和关闭看门狗的功能 */
#define IOC_WDG_STOP   IOC_WDG_BASE + 4   /**< 关闭看门狗，并不是所有板子都支持动态开启和关闭看门狗的功能 */
/** @} */

#ifdef __cplusplus
}
#endif
#endif //_IO_WDG_H_
