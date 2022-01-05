/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include "kv_conf.h"
#include "kv_adapt.h"
#include "aos/kernel.h"
#include "aos_hal_flash.h"

static aos_task_t kv_task;
int32_t kv_start_task(const char *name, void (*fn)(void *), void *arg,
                      uint32_t stack)
{
    return aos_task_new_ext(&kv_task, name, fn, arg, stack, KV_TASK_PRIORITY);
}

void kv_delete_task(void)
{
    aos_task_exit(0);
}

int32_t kv_flash_read(uint32_t offset, void *buf, uint32_t nbytes)
{
    return aos_hal_flash_read(HAL_PARTITION_PARAMETER_2, &offset, buf, nbytes);
}

int32_t kv_flash_write(uint32_t offset, void *buf, uint32_t nbytes)
{
    return aos_hal_flash_write(HAL_PARTITION_PARAMETER_2, &offset, buf, nbytes);
}

int32_t kv_flash_erase(uint32_t offset, uint32_t size)
{
    return aos_hal_flash_erase(HAL_PARTITION_PARAMETER_2, offset, size);
}

void *kv_lock_create(void)
{
    aos_mutex_t mutex;
    aos_mutex_new(&mutex);
    return (void *)mutex;
}

int32_t kv_lock_free(void *lock)
{
    aos_mutex_free((aos_mutex_t *)&lock);
    return 0;
}

int32_t kv_lock(void *lock)
{
    return aos_mutex_lock((aos_mutex_t *)&lock, AOS_WAIT_FOREVER);
}

int32_t kv_unlock(void *lock)
{
    return aos_mutex_unlock((aos_mutex_t *)&lock);
}

void *kv_sem_create(void)
{
    aos_sem_t sem;
    aos_sem_new(&sem, 0);
    return (void *)sem;
}

int32_t kv_sem_free(void *sem)
{
    aos_sem_free((aos_sem_t *)&sem);
    return 0;
}

int32_t kv_sem_wait(void *sem)
{
    return aos_sem_wait((aos_sem_t *)&sem, AOS_WAIT_FOREVER);
}

int32_t kv_sem_post_all(void *sem)
{
    aos_sem_signal_all((aos_sem_t *)&sem);
    return 0;
}

void *kv_malloc(uint32_t size)
{
    return aos_malloc(size);
}

void kv_free(void *ptr)
{
    aos_free(ptr);
}
