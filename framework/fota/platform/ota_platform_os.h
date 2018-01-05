/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef OTA_PLATFORM_OS_H_
#define OTA_PLATFORM_OS_H_
#include<stdint.h>

void *ota_malloc(uint32_t size);

void ota_free(void *ptr);

void *ota_mutex_init(void);

void ota_mutex_lock(void *mutex);

void ota_mutex_unlock(void *mutex);

void ota_mutex_destroy(void *mutex);

void *ota_semaphore_init(void);

int ota_semaphore_wait(void *sem, int32_t timeout_ms);

void ota_semaphore_post(void *sem);

void ota_semaphore_destroy(void *sem);

uint32_t ota_get_time_ms(void);

int8_t ota_thread_create(const char *name,
                         void *(*start_routine)(void *), void *arg, int stack_size, int prio);

void ota_thread_exit(void *thread);

void ota_reboot(void);
#endif
