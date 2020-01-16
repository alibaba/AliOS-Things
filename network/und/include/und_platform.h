/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UND_PLATFORM_H
#define UND_PLATFORM_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "und_mqtt.h"

#ifdef __cplusplus
extern "C" {
#endif

/* HAL list implemented by chipset or module manufacturer */
extern void HAL_Free(void *ptr);
extern void *HAL_Malloc(uint32_t size);
extern void *HAL_MutexCreate(void);
extern void HAL_MutexDestroy(void *mutex);
extern void HAL_MutexLock(void *mutex);
extern void HAL_MutexUnlock(void *mutex);
extern void *HAL_Timer_Create(const char *name, void (*func)(void *), void *user_data);
extern int HAL_Timer_Delete(void *timer);
extern int HAL_Timer_Start(void *t, int ms);
extern int HAL_Timer_Stop(void *t);
extern int HAL_Kv_Set(const char *key, const void *val, int len, int sync);
extern int HAL_Kv_Get(const char *key, void *buffer, int *buffer_len);
extern int HAL_Kv_Del(const char *key);
extern int HAL_GetProductKey(char *product_key);
extern int HAL_GetDeviceName(char *device_name);

/* MQTT API list implmented by Linkkit SDK */
extern int IOT_MQTT_CheckStateNormal(void *handle);
extern int IOT_MQTT_Unsubscribe(void *handle, const char *topic_filter);
extern int IOT_MQTT_Publish_Simple(void *handle, const char *topic_name, int qos, void *data, int len);
extern int IOT_MQTT_Subscribe(void *handle, const char *topic_filter,
                              int qos, void *topic_handle_func,
                              void *pcontext);

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

#ifdef __cplusplus
}
#endif

#endif /* UND_PLATFORM_H */
