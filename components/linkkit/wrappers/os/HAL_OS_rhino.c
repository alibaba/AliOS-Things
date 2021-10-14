/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "aos/kernel.h"
#include "aos/kv.h"
#include <k_api.h>

#include "linkkit/infra/infra_compat.h"
#include "linkkit/wrappers/wrappers_defs.h"

#define DEFAULT_THREAD_PRI AOS_DEFAULT_APP_PRI

#define _RHINO_SDK_DEMO__  1

#define PLATFORM_LOG(format, ...)                                    \
    do {                                                             \
        printf("Linux:%s:%d %s()| " format "\n", __FILE__, __LINE__, \
               __func__, ##__VA_ARGS__);                         \
    } while (0);

#ifndef TAG
#define TAG "HAL_OS"
#endif

void *HAL_MutexCreate(void)
{
    aos_mutex_t mutex;
    if (0 != aos_mutex_new(&mutex)) {
        return NULL;
    }
    return mutex;
}

int HAL_MutexDestroy(_IN_ void *mutex)
{
    if (NULL != mutex) {
        aos_mutex_free((aos_mutex_t *)&mutex);
    }
    return 0;
}

int HAL_MutexLock(_IN_ void *mutex)
{
    if (NULL != mutex) {
        return aos_mutex_lock((aos_mutex_t *)&mutex, AOS_WAIT_FOREVER);
    } else
        return -EINVAL;
}

int HAL_MutexUnlock(_IN_ void *mutex)
{
    if (NULL != mutex) {
        return aos_mutex_unlock((aos_mutex_t *)&mutex);
    } else
        return -EINVAL;
}

void *HAL_Malloc(_IN_ uint32_t size)
{
    return aos_malloc(size);
}

void HAL_Free(_IN_ void *ptr)
{
    aos_free(ptr);
}

void HAL_Reboot(void)
{
    aos_reboot();
}

uint64_t HAL_UptimeMs(void)
{
    return aos_now_ms();
}

void HAL_SleepMs(_IN_ uint32_t ms)
{
    aos_msleep(ms);
}

int HAL_Snprintf(_IN_ char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

int HAL_Vsnprintf(_IN_ char *str, _IN_ const int len, _IN_ const char *format,
                  va_list ap)
{
    return vsnprintf(str, len, format, ap);
}

void HAL_Srandom(uint32_t seed)
{
    // srandom(seed);
    srand(seed);
}

uint32_t HAL_Random(uint32_t region)
{
    uint32_t output;
#if defined(CONFIG_TEE_CA)
    csi_tee_rand_generate((uint8_t *)&output, 4);
#else
    static uint32_t rnd = 0x12345;

    output = rnd * 0xFFFF777;
    rnd = output;
#endif
    return (region > 0) ? (output % region) : 0;
}

void *HAL_SemaphoreCreate(void)
{
    aos_sem_t sem;

    if (0 != aos_sem_new(&sem, 0)) {
        return NULL;
    }

    return sem;
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
    if (task == NULL) {
        return;
    }
    task->routine(task->arg);

    aos_free(task);
    task = NULL;
}

#define DEFAULT_THREAD_NAME "AosThread"
#define DEFAULT_THREAD_SIZE 4096
int HAL_ThreadCreate(_OU_ void **thread_handle,
                     _IN_ void *(*work_routine)(void *), _IN_ void *arg,
                     _IN_ hal_os_thread_param_t *hal_os_thread_param,
                     _OU_ int *stack_used)
{
    int ret = -1;
    if (stack_used != NULL) {
        *stack_used = 0;
    }
    char *tname;
    size_t ssiz;
    int detach_state = 0;
    int priority;

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

    if (!hal_os_thread_param || hal_os_thread_param->priority == 0) {
        priority = DEFAULT_THREAD_PRI;
    } else if (hal_os_thread_param->priority < os_thread_priority_idle ||
               hal_os_thread_param->priority > os_thread_priority_realtime) {
        priority = DEFAULT_THREAD_PRI;
    } else {
        priority = DEFAULT_THREAD_PRI - hal_os_thread_param->priority;
    }

    task_context_t *task = aos_malloc(sizeof(task_context_t));
    if (!task) {
        return -1;
    }
    memset(task, 0, sizeof(task_context_t));

    task->arg = arg;
    task->routine = work_routine;
    task->detached = detach_state;

    ret = aos_task_new_ext(&task->task, tname, task_wrapper, task, ssiz,
                           priority);

    *thread_handle = (void *)task;

    return ret;
}

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

int HAL_Config_Write(const char *buffer, int length)
{
    if (!buffer || length <= 0) {
        return -1;
    }

    return aos_kv_set("alink", buffer, length, 1);
}

int HAL_Config_Read(char *buffer, int length)
{
    if (!buffer || length <= 0) {
        return -1;
    }

    return aos_kv_get("alink", buffer, &length);
}

int HAL_Kv_Set(const char *key, const void *val, int len, int sync)
{
    int ret = 0;
    ret = aos_kv_set(key, val, len, sync);
    return ret;
}

int HAL_Kv_Get(const char *key, void *buffer, int *buffer_len)
{
    int ret = 0;
    ret = aos_kv_get(key, buffer, buffer_len);
    return ret;
}

int HAL_Kv_Del(const char *key)
{
    int ret = 0;
    ret = aos_kv_del(key);
    return ret;
}

/*
 * timer API
 *
 * */
typedef void (*hal_timer_cb)(void *);

typedef struct time_data {
    sys_time_t expect_time;
    void *user_data;
    hal_timer_cb cb;
    struct time_data *next;
    int isactive;
} timer_data_t;

static timer_data_t *data_list = NULL;
static void *mutex = NULL;

static int _list_insert(timer_data_t *data)
{
    if (data == NULL) {
        return -1;
    }
    if (mutex == NULL) {
        if (mutex == NULL) {
            LOGE(TAG, "mutex create failed");
            return -1;
        }
    }

    HAL_MutexLock(mutex);
    data->next = data_list;
    data_list = data;
    HAL_MutexUnlock(mutex);
    return 0;
}

static void awss_timer_task(void *args)
{
    int sleep_time;
    int no_task_cnt = 0;

    while (1) {
        if (!data_list) {
            if (++no_task_cnt > 1000) {
                no_task_cnt = 1000;
                sleep_time = 1000;
                goto SLEEP;
            }
        } else {
            no_task_cnt = 0;
        }

        sleep_time = 10;
        HAL_MutexLock(mutex);

        timer_data_t *cur = data_list;
        while (cur != NULL) {
            if (cur->isactive &&
                cur->expect_time + 1 <= krhino_sys_tick_get()) {
                cur->cb(cur->user_data);
                cur->isactive = 0;
            }
            cur = cur->next;
        }

        HAL_MutexUnlock(mutex);

    SLEEP:
        aos_msleep(sleep_time);
    }
}

void *HAL_Timer_Create(const char *name, void (*func)(void *), void *user_data)
{
    static aos_task_t *tsk = NULL;

    if (!tsk) {
        mutex = HAL_MutexCreate();
        tsk = (aos_task_t *)aos_malloc(sizeof(aos_task_t));
        aos_task_new_ext(tsk, "AWSS_TIMER", awss_timer_task, NULL, 1024 * 4,
                         20);
    }

    timer_data_t *node = (timer_data_t *)aos_malloc(sizeof(timer_data_t));
    if (node == NULL) {
        return NULL;
    }

    memset(node, 0, sizeof(timer_data_t));
    node->user_data = user_data;
    node->cb = func;

    _list_insert(node);
    return node;
}

int HAL_Timer_Delete(void *timer)
{
    CHECK_PARAM(timer, -1);

    timer_data_t *cur = data_list;
    timer_data_t *pre = data_list;

    HAL_MutexLock(mutex);
    while (cur != NULL) {
        if (cur == timer) {
            if (cur == pre) {
                data_list = cur->next;
            } else {
                pre->next = cur->next;
            }
            aos_free(cur);
            HAL_MutexUnlock(mutex);
            return 0;
        }
        pre = cur;
        cur = cur->next;
    }
    HAL_MutexUnlock(mutex);

    LOGE(TAG, "time not found");
    return -1;
}

int HAL_Timer_Start(void *timer, int ms)
{
    CHECK_PARAM(timer, -1);

    if (timer == NULL) {
        return -1;
    }

    HAL_MutexLock(mutex);
    timer_data_t *cur = data_list;
    while (cur != NULL) {
        if (cur == timer) {
            cur->expect_time = krhino_sys_tick_get() + krhino_ms_to_ticks(ms);
            cur->isactive = 1;
            HAL_MutexUnlock(mutex);
            return 0;
        }
        cur = cur->next;
    }
    HAL_MutexUnlock(mutex);

    return -1;
}

int HAL_Timer_Stop(void *timer)
{
    CHECK_PARAM(timer, -1);

    if (timer == NULL) {
        return -1;
    }

    HAL_MutexLock(mutex);
    timer_data_t *cur = data_list;
    while (cur != NULL) {
        if (cur == timer) {
            cur->isactive = 0;
            HAL_MutexUnlock(mutex);
            return 0;
        }
        cur = cur->next;
    }
    HAL_MutexUnlock(mutex);

    return -1;
}

static int64_t delta_ms = 0;
void HAL_UTC_Set(long long ms)
{
    delta_ms = ms - aos_now_ms();
}

long long HAL_UTC_Get(void)
{
    long long ret = aos_now_ms() + delta_ms;
    return ret;
}

int get_aos_hex_version(const char *str, unsigned char hex[VERSION_NUM_SIZE])
{
    char *p = NULL;
    char *q = NULL;
    int i = 0;
    char str_ver[32] = { 0 };
    if (str == NULL) {
        return -1;
    }
    if (hex == NULL) {
        return -1;
    }
    strncpy(str_ver, str, sizeof(str_ver) - 1);
    p = strtok(str_ver, "-");
    for (i = 0; i < 2; i++) {
        if (p == NULL) {
            return -1;
        }
        p = strtok(NULL, "-");
    }

    q = strtok(p, ".");
    for (i = 0; i < 4; i++) {
        if (q == NULL) {
            break;
        } else {
            hex[i] = atoi(q);
        }
        q = strtok(NULL, ".");
    }
    return 0;
}

/**
 * 激活使用，提供kernel版本号字节数组
 */
void aos_get_version_hex(unsigned char version[VERSION_NUM_SIZE])
{
    memset(version, 0, VERSION_NUM_SIZE);
    get_aos_hex_version(aos_version_get(), version);
}

/**
 * 激活使用，提供用字节数组表示mac地址，非字符串
 */
void aos_get_mac_hex(unsigned char mac[MAC_ADDRESS_SIZE])
{
    memset(mac, 0, MAC_ADDRESS_SIZE);
    hal_wifi_get_mac_addr(NULL, mac);
    // return mac;
}

/**
 * 激活使用，提供用字节数组表示芯片ID，非字符串
 */
void aos_get_chip_code(unsigned char chip_code[CHIP_CODE_SIZE])
{
    memset(chip_code, 0, CHIP_CODE_SIZE);
    // MCU_ID import by -D option MCU_FAMILY
    strncpy(chip_code, "test", 4);

    return;
}
