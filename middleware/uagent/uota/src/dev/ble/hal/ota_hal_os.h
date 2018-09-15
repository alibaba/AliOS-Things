/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef OTA_PLATFORM_OS_H_
#define OTA_PLATFORM_OS_H_
#include <stdint.h>

#ifndef OTA_WITH_LINKKIT
#define ota_snprintf snprintf
#else
#define ota_snprintf      HAL_Snprintf
#define ota_malloc        HAL_Malloc
#define ota_free          HAL_Free
#define ota_calloc        HAL_Calloc
#define ota_realloc       HAL_Realloc
#define ota_msleep        HAL_SleepMs
#define ota_thread_create HAL_ThreadCreate
#define ota_thread_exit   HAL_ThreadDelete
#define ota_kv_set        HAL_Kv_Set
#define ota_kv_get        HAL_Kv_Get
#define ota_timer_create  HAL_Timer_Create
#define ota_timer_start   HAL_Timer_Start
#define ota_reboot        HAL_Reboot
#endif

/*memory*/
void *ota_malloc(uint32_t size);
void *ota_realloc(void *ptr, uint32_t size);
void *ota_calloc(uint32_t n, uint32_t size);
void ota_free(void *ptr);

/*Thread*/
int ota_thread_create(
    void **thread_handle,
    void *(*work_routine)(void *),
    void *arg,
    unsigned int stack_size,
    int *stack_used);
void ota_thread_exit(void *thread);

/*Timer*/
void *ota_timer_create(const char *name, void (*func)(void *), void *user_data);
int ota_timer_start(void *t, int ms);
void ota_msleep(uint32_t ms);

/*KV store*/
int ota_kv_set(const char *key, const void *val, int len, int sync);
int ota_kv_get(const char *key, void *buffer, int *buffer_len);

void ota_reboot(void);
#endif
