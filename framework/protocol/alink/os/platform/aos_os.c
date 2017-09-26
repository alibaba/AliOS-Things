/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <sys/time.h>
#include <time.h>
#include <aos/aos.h>
#include "platform.h"
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

#define TAG "alink_os"

#define DEFAULT_THREAD_PRI   10

typedef void (*thread_entry_t)(void *arg);

typedef void *(*start_routine_cb)(void *);

typedef struct {
    start_routine_cb start_routine;
    void            *arg;
} platform_thread_arg_t;

void platform_printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}


void *platform_malloc(uint32_t size)
{
    return aos_malloc(size);
}

void platform_free(void *ptr)
{
    aos_free(ptr);
}

void *platform_mutex_init(void)
{
    aos_mutex_t mutex;

    if (0 != aos_mutex_new(&mutex)) {
        return NULL;
    }

    return mutex.hdl;
}

void platform_mutex_lock(void *mutex)
{
    aos_mutex_lock((aos_mutex_t *)&mutex, AOS_WAIT_FOREVER);
}

void platform_mutex_unlock(void *mutex)
{
    aos_mutex_unlock((aos_mutex_t *)&mutex);
}

void platform_mutex_destroy(void *mutex)
{
    aos_mutex_free((aos_mutex_t *)&mutex);
}

void *platform_semaphore_init(void)
{
    aos_sem_t sem;

    if (0 != aos_sem_new(&sem, 0)) {
        return NULL;
    }

    return sem.hdl;
}

int platform_semaphore_wait(_IN_ void *sem, _IN_ uint32_t timeout_ms)
{
    if (PLATFORM_WAIT_INFINITE == timeout_ms) {
        return aos_sem_wait((aos_sem_t *)&sem, AOS_WAIT_FOREVER);
    } else {
        return aos_sem_wait((aos_sem_t *)&sem, timeout_ms);
    }
}

void platform_semaphore_post(void *sem)
{
    aos_sem_signal((aos_sem_t *)&sem);
}

void platform_semaphore_destroy(void *sem)
{
    aos_sem_free((aos_sem_t *)&sem);
}

void platform_msleep(_IN_ uint32_t ms)
{
    aos_msleep(ms);
}

uint32_t platform_get_time_ms(void)
{
    return aos_now_ms();
}

uint64_t platform_get_utc_time(_INOUT_ uint64_t *p_utc)
{
    return (uint64_t)time((time_t *)p_utc);
}

os_time_struct *platform_local_time_r(const _IN_ uint64_t *p_utc,
                                      _OUT_ os_time_struct *p_result)
{
    return (os_time_struct *)localtime_r((time_t *)p_utc, (struct tm *)p_result);
}

int platform_thread_get_stack_size(_IN_ const char *thread_name)
{
    /* Note: not used by linux platform, rtos tune stack size here */
    if (0 == strcmp(thread_name, "wsf_receive_worker")) {
        LOGD(TAG, "get wsf receive worker");
        return 8192;
    } else if (0 == strcmp(thread_name, "wsf_send_worker")) {
        LOGD(TAG, "get wsf send worker\n");
        return 4096;
    } else if (0 == strcmp(thread_name, "wsf_callback_worker")) {
        LOGD(TAG, "get wsf callback worker\n");
        return 4096;
    } else if (!strcmp(thread_name, "yunio_alink_worker")) {
        /* Yunio only need small stack */
        return 4096;
    } else if (0 == strcmp(thread_name, "fota_thread")) {
        LOGD(TAG, "get fota thread\n");
        return 4096;
    } else if (0 == strcmp(thread_name, "cota_thread")) {
        LOGD(TAG, "get cota thread\n");
        return 4096;
    } else if (0 == strcmp(thread_name, "alcs_thread")) {
        LOGD(TAG, "get alcs thread\n");
        return 8192;
    } else if (0 == strcmp(thread_name, "work queue")) {
        LOGD(TAG, "get work queue thread\n");
        return 4096;
    } else if (0 == strcmp(thread_name, "ifttt_update_system_utc")) {
        LOGD(TAG, "get ifttt_update_system_utc thread\n");
        return 8192;
    } else if (0 == strcmp(thread_name, "asr_websocket_thread")) {
        LOGD(TAG, "get asr_websocket_thread thread\n");
        return 8192;
    }

    //assert(0);
}

int platform_thread_create(
    _OUT_ void **thread,
    _IN_ const char *name,
    _IN_ void *(*start_routine)(void *),
    _IN_ void *arg,
    _IN_ void *stack,
    _IN_ uint32_t stack_size,
    _OUT_ int *stack_used)
{
    int ret = -1;
    *stack_used = 0;
    (void)stack;
    aos_task_t task;

    ret = aos_task_new_ext(&task, name, (thread_entry_t)start_routine, arg, stack_size,
                           DEFAULT_THREAD_PRI);

    *thread = start_routine;

    return ret;
}

void platform_thread_exit(void *thread)
{
    aos_task_exit(0);
}

