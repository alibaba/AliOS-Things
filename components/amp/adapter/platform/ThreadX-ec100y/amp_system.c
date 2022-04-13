/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "ql_data_call.h"
#include "ql_rtos.h"
#include "ql_power.h"

#include "amp_system.h"

#define PLATFORM_WAIT_INFINITE (~0)
#define SYSINFO_DEVICE_NAME "EC100Y"
#define SYSINFO_VERSION "0.0.1"
#define DEFAULT_TASK_PRIORITY 100

typedef struct {
    void (*callback)(int status, void *);
    void *arg;
} network_status_notify_t;

char firmware_version[64] = {0};
char product_id[64] = {0};
static char dev_imei[20] = {0};

int g_nw_status = -1;
static network_status_notify_t *notify;

void *HAL_Malloc(uint32_t size)
{
    void *ptr = NULL;

    if (size == 0) {
        return NULL;
    }

    if ((ptr = malloc(size)) == NULL) {
        return NULL;
    }

    return ptr;
}

void HAL_Free(void *ptr)
{
    if (ptr == NULL) {
        return;
    }
    size_t ptr_size = *(size_t *)((char *)ptr - 8);
    free(ptr);
}

void *HAL_Calloc(uint32_t nitems, uint32_t size)
{
    void *mem = HAL_Malloc(nitems * size);
    if (mem)
        memset(mem, 0, nitems * size);
    return mem;
}

void *HAL_Realloc(void *ptr, uint32_t size)
{
    void *ptr_new = NULL;

    if (ptr == NULL) {
        if ((ptr_new = HAL_Malloc(size)) == NULL) {
            return NULL;
        }
        return ptr_new;
    }

    if (size == 0) {
        HAL_Free(ptr);
        return NULL;
    }

    size_t ptr_size = *(size_t *)((char *)ptr - 8);

    if ((ptr_new = HAL_Malloc(size)) == NULL) {
        return NULL;
    }

    if (size <= ptr_size) {
        memcpy(ptr_new, ptr, size);
        HAL_Free(ptr);
        return ptr_new;
    }

    if (size > ptr_size) {
        memcpy(ptr_new, ptr, ptr_size);
        HAL_Free(ptr);
        return ptr_new;
    }

    return ptr_new;
}

uint64_t HAL_UptimeMs(void)
{
    uint64_t time_ms = ql_rtos_get_systicks();
    time_ms *= 5;
    return time_ms;
}

void HAL_SleepMs(uint32_t ms)
{
    ql_rtos_task_sleep_ms(ms);
}

void HAL_Srandom(uint32_t seed)
{
    srand(seed);
}

uint32_t HAL_Random(uint32_t region)
{
    int ret;
    while (1) {
        ret = rand();
        if (ret >= 0)
            break;
    }
    return (region > 0) ? (ret % region) : ret;
}

void HAL_Printf(const char *fmt, ...)
{
    char buffer[300] = {0};

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    uart_printf("%s", buffer);
}

int HAL_Snprintf(char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

int HAL_Vsnprintf(char *str, const int len, const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}

void HAL_LogPrint(const char *tag, int level, const char *format, ...)
{
    char buffer[200];
    int ret = 0;

    if (tag)
        ret += snprintf(buffer, sizeof(buffer), "<%d> %s ", level, tag);
    else
        ret += snprintf(buffer, sizeof(buffer), "<%d> ", level);
    va_list args;
    va_start(args, format);
    ret += vsnprintf(buffer + ret, sizeof(buffer) - ret, format, args);
    va_end(args);

    HAL_Printf("%s", buffer);
}

void HAL_Reboot(void)
{
}

void *HAL_SemaphoreCreate(void)
{
    ql_sem_t sem = NULL;
    ql_rtos_semaphore_create(&sem, 0);
    return sem;
}

void HAL_SemaphoreDestroy(void *sem)
{
    ql_rtos_semaphore_delete((ql_sem_t)sem);
}

void HAL_SemaphorePost(void *sem)
{
    ql_rtos_semaphore_release((ql_sem_t)sem);
}

int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms)
{
    uint32_t _timeout_ms;
    if (timeout_ms == PLATFORM_WAIT_INFINITE)
        _timeout_ms = QL_WAIT_FOREVER;
    else if (timeout_ms == 0)
        _timeout_ms = QL_NO_WAIT;
    else
        _timeout_ms = timeout_ms;
    ql_rtos_semaphore_wait((ql_sem_t)sem, _timeout_ms);
    return 0;
}

typedef struct {
    ql_task_t ql_task;
    void *(*func)(void *arg);
    void *arg;
} amp_thread_t;

static void thread_common_func(void *arg)
{
    amp_thread_t *amp_thread = (amp_thread_t *)arg;

    if (!amp_thread)
        return;

    if (amp_thread->func)
        amp_thread->func(amp_thread->arg);
}

int HAL_ThreadCreate(
            void **thread_handle,
            void *(*work_routine)(void *),
            void *arg,
            amp_os_thread_param_t *amp_os_thread_param,
            int *stack_used)
{
    amp_thread_t *amp_thread;

    amp_thread = HAL_Malloc(sizeof(amp_thread_t));
    if (!amp_thread)
        return -1;

    amp_thread->func = work_routine;
    amp_thread->arg = arg;

    ql_rtos_task_create(&amp_thread->ql_task, amp_os_thread_param->stack_size,
        amp_os_thread_param->priority,
        amp_os_thread_param->name,
        thread_common_func,
        amp_thread);

    return 0;
}

int HAL_ThreadDelete(void *thread_handle)
{
    return 0;
}

int HAL_GetDefaultTaskPriority()
{
    return DEFAULT_TASK_PRIORITY;
}

void *HAL_MutexCreate(void)
{
    ql_mutex_t mutex = NULL;
    ql_rtos_mutex_create(&mutex);
    return mutex;
}

void HAL_MutexDestroy(void *mutex)
{
    if (!mutex)
        return;
    ql_rtos_mutex_delete((ql_mutex_t)mutex);
}

void HAL_MutexLock(void *mutex)
{
    ql_rtos_mutex_lock((ql_mutex_t)mutex, QL_WAIT_FOREVER);
}

void HAL_MutexUnlock(void *mutex)
{
    ql_rtos_mutex_unlock((ql_mutex_t)mutex);
}

typedef struct {
    ql_timer_t ql_timer;
    void (*cb)(void *);
    void *user_data;
} amp_timer_t;

static void timer_common_cb(uint32_t args)
{
    amp_timer_t *amp_timer = (amp_timer_t *)args;

    if (amp_timer && amp_timer->cb)
        amp_timer->cb(amp_timer->user_data);
}

void *HAL_Timer_Create(const char *name, void (*func)(void *), void *user_data)
{
    amp_timer_t *amp_timer;
    int ret;

    amp_timer = HAL_Malloc(sizeof(amp_timer_t));
    if (!amp_timer)
        return NULL;
    memset(amp_timer, 0, sizeof(amp_timer_t));

    ret = ql_rtos_timer_create(&amp_timer->ql_timer);
    if (ret) {
        HAL_Printf("%s: create timer failed\n", __func__);
        HAL_Free(amp_timer);
    }
    amp_timer->cb = func;
    amp_timer->user_data = user_data;
    return amp_timer;
}

void *HAL_GetTimerPrams(void *timerHandle)
{
    return timerHandle;
}

int HAL_Timer_Start(void *timer, int ms, amp_timer_type repeat)
{
    amp_timer_t *amp_timer = (amp_timer_t *)timer;

    if (!amp_timer)
        return -1;

    ql_rtos_timer_start(amp_timer->ql_timer, ms,
        repeat == TimerRunPeriodic ? 1 : 0, timer_common_cb, amp_timer);
    return 0;
}

int HAL_Timer_Stop(void *timer)
{
    amp_timer_t *amp_timer = (amp_timer_t *)timer;

    if (!amp_timer)
        return -1;

    ql_rtos_timer_stop(amp_timer->ql_timer);
    return 0;
}

int HAL_Timer_Delete(void *timer)
{
    amp_timer_t *amp_timer = (amp_timer_t *)timer;

    if (!amp_timer)
        return -1;

    ql_rtos_timer_delete(amp_timer->ql_timer);
    HAL_Free(amp_timer);
    return 0;
}

void *HAL_MessageQCreate(int32_t queue_length, int32_t item_size)
{
    ql_queue_t queue;
    ql_rtos_queue_create(&queue, item_size, queue_length);
    return queue;
}

int32_t HAL_MessageQPut(void *mq, void *p_info, uint32_t size, uint32_t millisec)
{
    ql_rtos_queue_release(mq, size, p_info, millisec);
    return 0;
}

int32_t HAL_MessageQGet(void *mq, void *p_info, uint32_t size, uint32_t millisec)
{
    ql_rtos_queue_wait(mq, p_info, size, millisec);
    return 0;
}

int32_t HAL_MessageQDelete(void *mq)
{
    ql_rtos_queue_delete(mq);
    return 0;
}

int HAL_GetFirmwareVersion(char *version)
{
    return 0;
}

const char *HAL_Get_System_Version(void)
{
    return SYSINFO_VERSION;
}

const char *HAL_Get_system_platform_type(void)
{
    return SYSINFO_DEVICE_NAME;
}

int32_t HAL_Get_ip(char *ip)
{
    return 0;
}

int32_t HAL_Get_mac(unsigned char mac[8])
{
    memset(mac, 0, 8);
    char default_value = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    memcpy(mac, default_value, 8);
    return 0;
}

static void ql_nw_status_callback(int profile_idx, int nw_status)
{
    g_nw_status = nw_status;

    if (notify->callback != NULL) {
        notify->callback(nw_status, notify->arg);
    }
}

static network_status_cb_flag = 0;
int32_t HAL_network_status_registercb(void (*cb)(int status, void *), void *arg)
{
    if (!notify) {
        notify = HAL_Malloc(sizeof(network_status_notify_t));
        if (!notify){
            return -1;
        }
    }

    notify->callback = cb;
    notify->arg = arg;

    if (!network_status_cb_flag) {
        network_status_cb_flag = 1;
        ql_wan_start(ql_nw_status_callback);
        ql_set_auto_connect(1, TRUE);
        ql_start_data_call(1, 0, "", NULL, NULL, 0);
    }

    return 0;
}

int32_t HAL_Get_network_status(void)
{
    return g_nw_status;
}

int32_t HAL_Heap_Memory_Info(amp_heap_info_t *heap_info)
{
    return 0;
}

int32_t HAL_Sysinit(void)
{
    return 0;
}

const char *HAL_Get_Device_Name(void)
{
    memset(dev_imei, 0, sizeof(dev_imei));
    ql_dev_get_imei(dev_imei, sizeof(dev_imei));
    return dev_imei;
}

const char *HAL_Get_Module_Hardware_Version(void)
{
    ql_dev_get_product_id(product_id, sizeof(product_id));
    return product_id;
}

const char *HAL_Get_Module_Software_Version(void)
{
    ql_dev_get_firmware_version(firmware_version, sizeof(firmware_version));
    return firmware_version;
}