/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <aos/aos.h>
#include <ota_platform_os.h>

#define DEFAULT_THREAD_PRI   10

void *ota_malloc(uint32_t size)
{
    return aos_malloc(size);
}

void ota_free(void *ptr)
{
    aos_free(ptr);
}

void *ota_mutex_init(void)
{
    aos_mutex_t *mutex = (aos_mutex_t *)aos_malloc(sizeof(aos_mutex_t));

    if (NULL == mutex) {
        return NULL;
    }

    if (0 != aos_mutex_new(mutex)) {
        aos_free(mutex);
        return NULL;
    }

    return mutex;
}

void ota_mutex_lock(void *mutex)
{
    aos_mutex_lock((aos_mutex_t *)mutex, AOS_WAIT_FOREVER);
}

void ota_mutex_unlock(void *mutex)
{
    aos_mutex_unlock((aos_mutex_t *)mutex);
}

void ota_mutex_destroy(void *mutex)
{
    aos_mutex_free((aos_mutex_t *)mutex);
    aos_free(mutex);
}

void *ota_semaphore_init(void)
{
    aos_sem_t *sem = (aos_sem_t *)aos_malloc(sizeof(aos_sem_t));

    if (NULL == sem) {
        return NULL;
    }

    if (0 != aos_sem_new(sem, 0)) {
        aos_free(sem);
        return NULL;
    }

    return sem;
}

int ota_semaphore_wait( void *sem, int32_t timeout_ms)
{
    return aos_sem_wait((aos_sem_t *)sem, timeout_ms);
}

void ota_semaphore_post(void *sem)
{
    aos_sem_signal((aos_sem_t *)sem);
}

void ota_semaphore_destroy(void *sem)
{
    aos_sem_free((aos_sem_t *)sem);
    aos_free(sem);
}

uint32_t ota_get_time_ms(void)
{
    struct timeval tv = { 0 };
    uint32_t time_ms;

    gettimeofday(&tv, NULL);

    time_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return time_ms;
}

void ota_reboot(void)
{
    aos_reboot();
}

void ota_thread_exit(void *thread)
{
    aos_task_exit(0);
}

