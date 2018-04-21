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
#include "platform.h"
#include "iot_import_product.h"
#if defined(__HUA_DONG_2_DAILY__)
#define DEMO_CASE_PRODUCT_KEY       "a1nmfrdo3MI"
#define DEMO_CASE_DEVICE_NAME       "light_demo_for_ilop_device_test"
#define DEMO_CASE_DEVICE_SECRET     "kobN5zg08IwlgbqSUeaxo0vbEsOiEI7b"
#elif defined(__HUA_DONG_2_PRE__)
#define DEMO_CASE_PRODUCT_KEY       "a1u36e7PPs1"
#define DEMO_CASE_DEVICE_NAME       "MxFRySXtCYbcR9vsYDyd"
#define DEMO_CASE_DEVICE_SECRET     "5U0GSuYZHsfINMB3n9bxUT2Yo1fJyo9S"
#else
#define DEMO_CASE_PRODUCT_KEY       "yfTuLfBJTiL"
#define DEMO_CASE_DEVICE_NAME       "TestDeviceForDemo"
#define DEMO_CASE_DEVICE_SECRET     "fSCl9Ns5YPnYN8Ocg0VEel1kXFnRlV6c"
#endif
#define DEMO_CASE_DEVICE_ID         DEMO_CASE_PRODUCT_KEY "." DEMO_CASE_DEVICE_NAME

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
    aos_mutex_free((aos_mutex_t *)&mutex);
}

void HAL_MutexLock(_IN_ void *mutex)
{
    aos_mutex_lock((aos_mutex_t *)&mutex, AOS_WAIT_FOREVER);
}

void HAL_MutexUnlock(_IN_ void *mutex)
{
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
    return aos_msleep(ms);
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

int HAL_GetPartnerID(_OU_ char pid_str[PID_STR_MAXLEN])
{
    memset(pid_str, 0x0, PID_STR_MAXLEN);
#ifdef _RHINO_SDK_DEMO__
    strcpy(pid_str, "example.demo.partner-id");
#endif
    return strlen(pid_str);
}

int HAL_GetModuleID(char mid_str[MID_STR_MAXLEN])
{
    memset(mid_str, 0x0, MID_STR_MAXLEN);
#ifdef _RHINO_SDK_DEMO__
    strcpy(mid_str, "example.demo.module-id");
#endif
    return strlen(mid_str);
}

char *HAL_GetChipID(_OU_ char cid_str[HAL_CID_LEN])
{
    memset(cid_str, 0x0, HAL_CID_LEN);
#ifdef _RHINO_SDK_DEMO__
    strncpy(cid_str, "rtl8188eu 12345678", HAL_CID_LEN);
    cid_str[HAL_CID_LEN - 1] = '\0';
#endif
    return cid_str;
}

int HAL_GetDeviceID(_OU_ char device_id[DEVICE_ID_MAXLEN])
{
    memset(device_id, 0x0, DEVICE_ID_MAXLEN);
#ifdef _RHINO_SDK_DEMO__
    strncpy(device_id, DEMO_CASE_DEVICE_ID, DEVICE_ID_MAXLEN);
    device_id[DEVICE_ID_MAXLEN - 1] = '\0';
#endif
    return strlen(device_id);
}

int HAL_GetDeviceName(_OU_ char device_name[DEVICE_NAME_MAXLEN])
{
    memset(device_name, 0x0, DEVICE_NAME_MAXLEN);
#ifdef _RHINO_SDK_DEMO__
    strncpy(device_name, DEMO_CASE_DEVICE_NAME, DEVICE_NAME_MAXLEN);
    device_name[DEVICE_NAME_MAXLEN - 1] = '\0';
#endif
    return strlen(device_name);
}

int HAL_GetDeviceSecret(_OU_ char device_secret[DEVICE_SECRET_MAXLEN])
{
    memset(device_secret, 0x0, DEVICE_SECRET_MAXLEN);
#ifdef _RHINO_SDK_DEMO__
    strncpy(device_secret, DEMO_CASE_DEVICE_SECRET, DEVICE_SECRET_MAXLEN);
    device_secret[DEVICE_SECRET_MAXLEN - 1] = '\0';
#endif
    return strlen(device_secret);
}

int HAL_GetFirmwareVesion(_OU_ char version[FIRMWARE_VERSION_MAXLEN])
{
    memset(version, 0x0, FIRMWARE_VERSION_MAXLEN);
#ifdef _RHINO_SDK_DEMO__
    strncpy(version, "1.0", FIRMWARE_VERSION_MAXLEN);
    version[FIRMWARE_VERSION_MAXLEN - 1] = '\0';
#endif
    return strlen(version);
}

int HAL_GetProductKey(_OU_ char product_key[PRODUCT_KEY_MAXLEN])
{
    memset(product_key, 0x0, PRODUCT_KEY_MAXLEN);
#ifdef _RHINO_SDK_DEMO__
    strncpy(product_key, DEMO_CASE_PRODUCT_KEY, PRODUCT_KEY_MAXLEN);
    product_key[PRODUCT_KEY_MAXLEN - 1] = '\0';
#endif
    return strlen(product_key);
}



void HAL_Srandom(uint32_t seed)
{
    srandom(seed);
}

uint32_t HAL_Random(uint32_t region)
{
    return (region > 0) ? (random() % region) : 0;
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

int HAL_ThreadCreate(
            _OU_ void **thread_handle,
            _IN_ void *(*work_routine)(void *),
            _IN_ void *arg,
            _IN_ hal_os_thread_param_t *hal_os_thread_param,
        _OU_ int *stack_used)
{
    int ret = -1;
    *stack_used = 0;
    const char *name;
    int stack_size;

    task_context_t *task = aos_malloc(sizeof(task_context_t));
    if (!task)
        return -1;
    memset(task, 0, sizeof(task_context_t));

    task->arg = arg;
    task->routine = work_routine;

    if(hal_os_thread_param == NULL){
        name = "HAL_default_name";
        stack_size=4096;
        task->detached = 1;
    }else{
        name = hal_os_thread_param->name;
        stack_size=hal_os_thread_param->stack_size; 
        task->detached = hal_os_thread_param->detach_state;      
    }

    ret = aos_task_new_ext(&task->task,
            name, task_wrapper, task,
            stack_size, DEFAULT_THREAD_PRI);

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

