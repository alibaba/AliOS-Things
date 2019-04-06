/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "aos/kernel.h"
#include "aos/kv.h"
#include <hal/wifi.h>


#include "activation.h"
#include "chip_code.h"
#include "iot_import.h"
#include "iot_import_product.h"
#include "utils_sysinfo.h"


#define DEFAULT_THREAD_PRI AOS_DEFAULT_APP_PRI

#define _RHINO_SDK_DEMO__ 1

#define PLATFORM_LOG(format, ...)                              \
    do {                                                             \
        HAL_Printf("Linux:%s:%d %s()| " format "\n", __FILE__, __LINE__, \
                   __FUNCTION__, ##__VA_ARGS__);                         \
    } while (0);

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
    if (NULL != mutex) {
        aos_mutex_free((aos_mutex_t *)&mutex);
    }
}

void HAL_MutexLock(_IN_ void *mutex)
{
    if (NULL != mutex) {
        aos_mutex_lock((aos_mutex_t *)&mutex, AOS_WAIT_FOREVER);
    }
}

void HAL_MutexUnlock(_IN_ void *mutex)
{
    if (NULL != mutex) {
        aos_mutex_unlock((aos_mutex_t *)&mutex);
    }
}

void *HAL_Malloc(_IN_ uint32_t size)
{
    return aos_malloc(size);
}

void *HAL_Realloc(_IN_ void *ptr, _IN_ uint32_t size)
{
    return aos_realloc(ptr, size);
}

void *HAL_Calloc(_IN_ uint32_t nmemb, _IN_ uint32_t size)
{
    return aos_calloc(nmemb, size);
}

void HAL_Free(_IN_ void *ptr)
{
    aos_free(ptr);
}

void HAL_Reboot(void)
{
    aos_reboot();
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


int HAL_Vsnprintf(_IN_ char *str, _IN_ const int len, _IN_ const char *format,
                  va_list ap)
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
    // srandom(seed);
    srand(seed);
}

uint32_t HAL_Random(uint32_t region)
{
    return (region > 0) ? (rand() % region) : 0;
    // return 0;
}


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
    int        detached;
    void      *arg;
    void *(*routine)(void *arg);
} task_context_t;

static void task_wrapper(void *arg)
{
    task_context_t *task = arg;

    task->routine(task->arg);

    if (task) {
        aos_free(task);
        task = NULL;
    }
}

#define DEFAULT_THREAD_NAME "AosThread"
#define DEFAULT_THREAD_SIZE 4096
int HAL_ThreadCreate(_OU_ void **thread_handle,
                     _IN_ void *(*work_routine)(void *), _IN_ void *arg,
                     _IN_ hal_os_thread_param_t *hal_os_thread_param,
                     _OU_ int                   *stack_used)
{
    int ret = -1;
    if (stack_used != NULL) {
        *stack_used = 0;
    }
    char *tname;
    size_t ssiz;
    int    detach_state = 0;
    int    priority;

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

    task->arg      = arg;
    task->routine  = work_routine;
    task->detached = detach_state;

    ret = aos_task_new_ext(&task->task, tname, task_wrapper, task, ssiz,
                           priority);

    *thread_handle = (void *)task;

    return ret;
}

void HAL_ThreadDetach(_IN_ void *thread_handle)
{
    task_context_t *task = thread_handle;
    task->detached       = 1;
}

void HAL_ThreadDelete(_IN_ void *thread_handle)
{
    if (thread_handle) {
       /*can`t delete other task*/
    } else {
        aos_task_exit(0);
    }
}

void HAL_Firmware_Persistence_Start(void) {}

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
    int   ret = 0;
    ret = aos_kv_set(key, val, len, sync);
    return ret;
}

int HAL_Kv_Get(const char *key, void *buffer, int *buffer_len)
{
    int   ret = 0;
    ret = aos_kv_get(key, buffer, buffer_len);
    return ret;
}

int HAL_Kv_Del(const char *key)
{
    int   ret = 0;
    ret = aos_kv_del(key);
    return ret;
}

#ifdef USE_YLOOP
#include "aos/yloop.h"

typedef struct {
    const char *name;
    int         ms;
    aos_call_t  cb;
    void       *data;
} schedule_timer_t;


static void schedule_timer(void *p)
{
    if (p == NULL) {
        return;
    }

    schedule_timer_t *pdata = p;
    aos_post_delayed_action(pdata->ms, pdata->cb, pdata->data);
}

static void schedule_timer_cancel(void *p)
{
    if (p == NULL) {
        return;
    }

    schedule_timer_t *pdata = p;
    aos_cancel_delayed_action(-1, pdata->cb, pdata->data);
}

static void schedule_timer_delete(void *p)
{
    if (p == NULL) {
        return;
    }

    schedule_timer_t *pdata = p;
    aos_cancel_delayed_action(-1, pdata->cb, pdata->data);
    aos_free(p);
}
#else
typedef void (*hal_timer_cb)(void *);

typedef struct time_data {
    void *user_data;
    hal_timer_cb  cb;
    aos_timer_t  *timer;
    struct time_data *next;
} timer_data_t;

static timer_data_t *data_list = NULL;
static void *mutex = NULL;

static int _list_insert(timer_data_t *data)
{
    if (data == NULL) {
        return -1;
    }
    if (mutex == NULL) {
        mutex = HAL_MutexCreate();
        if (mutex == NULL) {
            PLATFORM_LOG("mutex create failed");
            return -1;
        }
    }

    HAL_MutexLock(mutex);
    data->next = data_list;
    data_list = data;
    HAL_MutexUnlock(mutex);
    return 0;
}

#include "k_api.h"

#define MS2TICK(ms) krhino_ms_to_ticks(ms)
static int _timer_change(aos_timer_t *timer, int ms)
{
    int ret;

    if (timer == NULL) {
        return -1;
    }

    ret = krhino_timer_change(timer->hdl, MS2TICK(ms), 0);
    return ret;
}

static timer_data_t *_get_timer_data(aos_timer_t *timer)
{
    if (timer == NULL) {
        PLATFORM_LOG("timer null");
        return NULL;
    }
    if (mutex == NULL) {
        mutex = HAL_MutexCreate();
        if (mutex == NULL) {
            PLATFORM_LOG("mutex create failed");
            return NULL;
        }
    }

    HAL_MutexLock(mutex);

    timer_data_t *cur = data_list;
    while (cur != NULL) {
        if (cur->timer == timer) {
            HAL_MutexUnlock(mutex);
            return cur;
        }
        cur = cur->next;
    }
    HAL_MutexUnlock(mutex);
    return NULL;
}

static int _list_remove(aos_timer_t  *timer)
{
    timer_data_t *cur = data_list;
    timer_data_t *pre = data_list;

    if (timer == NULL) {
        return -1;
    }

    if (mutex == NULL) {
        mutex = HAL_MutexCreate();
        if (mutex == NULL) {
            PLATFORM_LOG("mutex create failed");
            return -1;
        }
    }

    HAL_MutexLock(mutex);
    while (cur != NULL) {

        if (cur->timer == timer) {
            if (cur == pre) {
                data_list = cur->next;
            } else {
                pre->next = cur->next;
            }
            aos_free(cur->timer);
            aos_free(cur);
            break;
        }
        pre = cur;
        cur = cur->next;
    }
    HAL_MutexUnlock(mutex);
    return 0;

}

static void timer_cb(void *timer, void *user)
{
    timer_data_t *node;

    node = _get_timer_data(user);
    if (node == NULL) {
        PLATFORM_LOG("timer node not found");
        return;
    }
    if (node->cb == NULL) {
        PLATFORM_LOG("no timer cb");
        return;
    }

    node->cb(node->user_data);
}

#endif
void *HAL_Timer_Create(const char *name, void (*func)(void *), void *user_data)
{
#ifdef USE_YLOOP
    schedule_timer_t *timer =
                (schedule_timer_t *)aos_malloc(sizeof(schedule_timer_t));
    if (timer == NULL) {
        return NULL;
    }

    timer->name = name;
    timer->cb   = func;
    timer->data = user_data;

    return timer;
#else

    aos_timer_t *timer = (aos_timer_t *)aos_malloc(sizeof(aos_timer_t));
    if (timer == NULL) {
        return NULL;
    }
    memset(timer, 0, sizeof(aos_timer_t));
    timer_data_t *node = (timer_data_t *)aos_malloc(sizeof(timer_data_t));
    if (node == NULL) {
        aos_free(timer);
        return NULL;
    }
    memset(node, 0, sizeof(timer_data_t));
    node->timer = timer;
    node->user_data = user_data;
    node->cb = func;

    _list_insert(node);
    aos_timer_new_ext(timer, timer_cb, timer, 2000, 0, 0);
    return timer;

#endif
}

int HAL_Timer_Start(void *t, int ms)
{
#ifdef USE_YLOOP
    if (t == NULL) {
        return -1;
    }
    schedule_timer_t *timer = t;
    timer->ms               = ms;
    return aos_schedule_call(schedule_timer, t);
#else
    int ret;
    ret = _timer_change(t, ms);
    if (ret != 0) {
        PLATFORM_LOG("_timer_change failed %d", ret);
        return -1;
    }

    return aos_timer_start(t);
#endif
}

int HAL_Timer_Stop(void *t)
{
#ifdef USE_YLOOP
    if (t == NULL) {
        return -1;
    }

    return aos_schedule_call(schedule_timer_cancel, t);
#else
    return aos_timer_stop(t);
#endif
}

int HAL_Timer_Delete(void *timer)
{
#ifdef USE_YLOOP
    if (timer == NULL) {
        return -1;
    }
    return aos_schedule_call(schedule_timer_delete, timer);
#else
    aos_timer_free(timer);
    _list_remove(timer);
    return 0;
#endif
}


static int64_t delta_ms = 0;
void           HAL_UTC_Set(long long ms)
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
    char *p           = NULL;
    char *q           = NULL;
    int   i           = 0;
    char  str_ver[32] = { 0 };
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
    // MCU_ID import by -D option
    chip_code_st *p_chip_code_obj = get_chip_code(MCU_FAMILY);
    if (p_chip_code_obj != NULL) {
        chip_code[0] = (uint8_t)(p_chip_code_obj->vendor >> 8);
        chip_code[1] = (uint8_t)p_chip_code_obj->vendor;
        chip_code[2] = (uint8_t)(p_chip_code_obj->id >> 8);
        chip_code[3] = (uint8_t)p_chip_code_obj->id;
    }
    // return chip_code;
}

int HAL_GetNetifInfo(char *nif_str)
{
    memset(nif_str, 0x0, NIF_STRLEN_MAX);
#ifdef __DEMO__
    /* if the device have only WIFI, then list as follow, note that the len MUST NOT exceed NIF_STRLEN_MAX */
    const char *net_info = "WiFi|03ACDEFF0032";
    strncpy(nif_str, net_info, strlen(net_info));
    /* if the device have ETH, WIFI, GSM connections, then list all of them as follow, note that the len MUST NOT exceed NIF_STRLEN_MAX */
    // const char *multi_net_info = "ETH|0123456789abcde|WiFi|03ACDEFF0032|Cellular|imei_0123456789abcde|iccid_0123456789abcdef01234|imsi_0123456789abcde|msisdn_86123456789ab");
    // strncpy(nif_str, multi_net_info, strlen(multi_net_info));
#else
    uint8_t mac[6];

    memset(mac, 0x0, 6);
    hal_wifi_get_mac_addr(NULL, mac);
    snprintf(nif_str, NIF_STRLEN_MAX, "WiFi|%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#endif
    return strlen(nif_str);
}
