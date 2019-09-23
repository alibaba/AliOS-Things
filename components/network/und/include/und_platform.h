/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UND_PLATFORM_H
#define UND_PLATFORM_H

#include <stdio.h>
#include <string.h>

#include "linkkit/wrappers/wrappers.h"

#ifdef __cplusplus
extern "C" {
#endif

#define und_platform_malloc(size)     HAL_Malloc(size)
#define und_platform_free(ptr)        HAL_Free(ptr)
#define und_platform_memset           memset
#define und_platform_strlen           strlen
#define und_platform_snprintf         snprintf
#define und_platform_mutex_lock       HAL_MutexLock
#define und_platform_mutex_unlock     HAL_MutexUnlock
#define und_platform_mutex_create     HAL_MutexCreate
#define und_platform_mutex_destroy    HAL_MutexDestroy
#define und_platform_timer_create     HAL_Timer_Create
#define und_platform_timer_start      HAL_Timer_Start
#define und_platform_timer_stop       HAL_Timer_Stop
#define und_platform_timer_delete     HAL_Timer_Delete
#define und_platform_kv_set           HAL_Kv_Set
#define und_platform_kv_get           HAL_Kv_Get
#define und_platform_kv_del           HAL_Kv_Del
#define und_platform_get_product_key  HAL_GetProductKey
#define und_platform_get_device_name  HAL_GetDeviceName
#define und_platform_uptime_ms        HAL_UptimeMs

#ifdef __cplusplus
}
#endif

#endif /* UND_PLATFORM_H */
