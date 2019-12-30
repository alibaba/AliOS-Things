/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UND_PLATFORM_H
#define UND_PLATFORM_H

#include <stdio.h>
#include <string.h>

#ifdef BUILD_AOS
#include "aos/kernel.h"
#include "aos/kv.h"
#else
#include "linkkit/wrappers/wrappers.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BUILD_AOS
#define aos_malloc(size)     HAL_Malloc(size)
#define aos_free(ptr)        HAL_Free(ptr)
#define aos_kv_set           HAL_Kv_Set
#define aos_kv_get           HAL_Kv_Get
#define aos_kv_del           HAL_Kv_Del
#endif

#define aos_timer_create     HAL_Timer_Create
#define aos_timer_start      HAL_Timer_Start
#define aos_timer_stop       HAL_Timer_Stop
#define aos_timer_delete     HAL_Timer_Delete
#define aos_memset           memset
#define aos_strlen           strlen
#define aos_snprintf         HAL_Snprintf

#define undp_mutex_lock      HAL_MutexLock
#define undp_mutex_unlock    HAL_MutexUnlock
#define undp_mutex_new       HAL_MutexCreate
#define undp_mutex_free      HAL_MutexDestroy
#define undp_get_product_key HAL_GetProductKey
#define undp_get_device_name HAL_GetDeviceName

#ifdef __cplusplus
}
#endif

#endif /* UND_PLATFORM_H */
