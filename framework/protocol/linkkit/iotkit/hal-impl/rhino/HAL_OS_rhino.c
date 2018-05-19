/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <aos/aos.h>

#include "iot_import.h"
#include "iot_import_product.h"


#define DEFAULT_THREAD_PRI          AOS_DEFAULT_APP_PRI

#define _RHINO_SDK_DEMO__ 1

#define PLATFORM_LINUX_LOG(format, ...) \
    do { \
        printf("Linux:%s:%d %s()| "format"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
        fflush(stdout);\
    }while(0);

void *HAL_MutexCreate(void)
{
    aos_mutex_t mutex;

    if (0 != aos_mutex_new(&mutex)) {
        return NULL;
    }

    return mutex.hdl;
}

void HAL_MutexDestroy(_IN_ void *mutex)
{
    if (NULL != mutex)
        aos_mutex_free((aos_mutex_t *)&mutex);
}

void HAL_MutexLock(_IN_ void *mutex)
{
    if (NULL != mutex)
        aos_mutex_lock((aos_mutex_t *)&mutex, AOS_WAIT_FOREVER);
}

void HAL_MutexUnlock(_IN_ void *mutex)
{
    if (NULL != mutex)
        aos_mutex_unlock((aos_mutex_t *)&mutex);
}

void *HAL_Malloc(_IN_ uint32_t size)
{
    return aos_malloc(size);
}

void HAL_Free(_IN_ void *ptr)
{
    aos_free(ptr);
}

int aliot_platform_ota_start(const char *md5, uint32_t file_size)
{
    printf("this interface is NOT support yet.");
    return -1;
}

int aliot_platform_ota_write(_IN_ char *buffer, _IN_ uint32_t length)
{
    printf("this interface is NOT support yet.");
    return -1;
}

int aliot_platform_ota_finalize(_IN_ int stat)
{
    printf("this interface is NOT support yet.");
    return -1;
}

uint64_t HAL_UptimeMs(void)
{
    return aos_now_ms();
}

void HAL_SleepMs(_IN_ uint32_t ms)
{
    aos_msleep(ms);
}

uint64_t aliot_platform_time_left(uint64_t t_end, uint64_t t_now)
{
    uint64_t t_left;

    if (t_end > t_now) {
        t_left = t_end - t_now;
    } else {
        t_left = 0;
    }

    return t_left;
}


int HAL_Snprintf(_IN_ char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}


int HAL_Vsnprintf(_IN_ char *str, _IN_ const int len, _IN_ const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}


void HAL_Printf(_IN_ const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}


void HAL_Srandom(uint32_t seed)
{
    //srandom(seed);
    srand(seed);
}

uint32_t HAL_Random(uint32_t region)
{
    return (region > 0) ? (rand() % region) : 0;
    //return 0;
}


#ifdef USE_LPTHREAD
void *HAL_SemaphoreCreate(void)
{
    aos_sem_t sem;

    if (0 != aos_sem_new(&sem, 0)) {
        return NULL;
    }

    return sem.hdl;
}

void HAL_SemaphoreDestroy(_IN_ void *sem)
{
    aos_sem_free((aos_sem_t *)&sem);
}

void HAL_SemaphorePost(_IN_ void *sem)
{
    aos_sem_signal((aos_sem_t *)&sem);
}

int HAL_SemaphoreWait(_IN_ void *sem, _IN_ uint32_t timeout_ms)
{
    if (PLATFORM_WAIT_INFINITE == timeout_ms) {
        return aos_sem_wait((aos_sem_t *)&sem, AOS_WAIT_FOREVER);
    } else {
        return aos_sem_wait((aos_sem_t *)&sem, timeout_ms);
    }
}

typedef struct {
    aos_task_t task;
    int detached;
    void *arg;
    void *(*routine)(void *arg);
} task_context_t;

static void task_wrapper(void *arg)
{
    task_context_t *task = arg;

    task->routine(task->arg);

    if(task) {
        aos_free(task);
        task = NULL;
    }
}

#define DEFAULT_THREAD_NAME "AosThread"
#define DEFAULT_THREAD_SIZE 4096
int HAL_ThreadCreate(
            _OU_ void **thread_handle,
            _IN_ void *(*work_routine)(void *),
            _IN_ void *arg,
            _IN_ hal_os_thread_param_t *hal_os_thread_param,
        _OU_ int *stack_used)
{
    int ret = -1;
    *stack_used = 0;
    char *tname;
    size_t ssiz;
    int detach_state=0;

    if (hal_os_thread_param) {
        detach_state = hal_os_thread_param->detach_state;
    } 
    if (!hal_os_thread_param || !hal_os_thread_param->name) {
        tname = DEFAULT_THREAD_NAME;
    } else {
        tname = hal_os_thread_param->name;
    }

    if (!hal_os_thread_param || hal_os_thread_param->stack_size == 0) {
        ssiz = DEFAULT_THREAD_SIZE;
    } else {
        ssiz = hal_os_thread_param->stack_size;
    }


    task_context_t *task = aos_malloc(sizeof(task_context_t));
    if (!task)
        return -1;
    memset(task, 0, sizeof(task_context_t));

    task->arg = arg;
    task->routine = work_routine;
    task->detached = detach_state;

    ret = aos_task_new_ext(&task->task,
            tname, task_wrapper, task,
            ssiz, DEFAULT_THREAD_PRI);

    *thread_handle = (void *)task;

    return ret;
}

void HAL_ThreadDetach(_IN_ void *thread_handle)
{
    task_context_t *task = thread_handle;
    task->detached = 1;
}

void HAL_ThreadDelete(_IN_ void *thread_handle)
{
    if(thread_handle) {
        aos_free(thread_handle);
        thread_handle = NULL;
    }
    aos_task_exit(0);
}

#endif


void HAL_Firmware_Persistence_Start(void)
{

}

int HAL_Firmware_Persistence_Write(_IN_ char *buffer, _IN_ uint32_t length)
{

    return 0;
}

int HAL_Firmware_Persistence_Stop(void)
{
    return 0;
}



int HAL_Config_Write(const char *buffer, int length) {
    if (!buffer || length <= 0) {
        return -1;
    }

    return aos_kv_set("alink", buffer, length, 1);
}

int HAL_Config_Read(char *buffer, int length) {
    if (!buffer || length <= 0) {
        return -1;
    }

    return aos_kv_get("alink", buffer, &length);
}

