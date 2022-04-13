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
#define IOC_WDG_RELOAD IOC_WDG_BASE + 1 /**< 复位看门狗 */

/** @} */

#ifdef __cplusplus
}
#endif
#endif //_IO_WDG_H_
