/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "aos/kernel.h"
#include "kv_api.h"
#include "aos/vfs.h"

#include "ulog/ulog.h"
#include "network.h"

#include "netmgr.h"
// #include "wifi_service.h"

#include "infra_config.h"
#include "infra_compat.h"
#include "infra_defs.h"
#include "wrappers_defs.h"
#include "amp_system.h"

#define PLATFORM_WAIT_INFINITE (~0)
#define _SYSINFO_DEVICE_NAME "AliOS Things"
#define SYSINFO_VERSION "0.0.1"
#define _DEFAULT_THREAD_PRI AOS_DEFAULT_APP_PRI

typedef struct {
    aos_task_t task;
    int        detached;
    void      *arg;
    void *(*routine)(void *arg);
} task_context_t;

void *HAL_Malloc(uint32_t size)
{
    return aos_malloc(size);
}

void HAL_Free(void *ptr)
{
    aos_free(ptr);
}

void *HAL_Calloc(uint32_t nitems, uint32_t size)
{
    return aos_calloc((size_t)nitems, (size_t)size);
}

void *HAL_Realloc(void *ptr, uint32_t size)
{
    return aos_realloc(ptr, (size_t)size);
}

uint64_t HAL_UptimeMs(void)
{
    return aos_now_ms();
}

void HAL_SleepMs(uint32_t ms)
{
    aos_msleep(ms);
}

void HAL_Srandom(uint32_t seed)
{
    aos_srand(seed);
}

uint32_t HAL_Random(uint32_t region)
{
    return aos_rand();
}

void HAL_Printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
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

void HAL_Reboot(void)
{
    aos_reboot();
}

void *HAL_SemaphoreCreate(void)
{
    aos_sem_t sem;

    if (0 != aos_sem_new(&sem, 0)) {
        return NULL;
    }

    return sem;
}

void HAL_SemaphoreDestroy(void *sem)
{
    aos_sem_free((aos_sem_t *)&sem);
}

void HAL_SemaphorePost(void *sem)
{
    aos_sem_signal((aos_sem_t *)&sem);
}

int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms)
{
    if (PLATFORM_WAIT_INFINITE == timeout_ms) {
        return aos_sem_wait((aos_sem_t *)&sem, AOS_WAIT_FOREVER);
    } else {
        return aos_sem_wait((aos_sem_t *)&sem, timeout_ms);
    }
}

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

#define _DEFAULT_THREAD_NAME "AosThread"
#define _DEFAULT_THREAD_SIZE 4096
int HAL_ThreadCreate(
            void **thread_handle,
            void *(*work_routine)(void *),
            void *arg,
            amp_os_thread_param_t *amp_os_thread_param,
            int *stack_used)
{
    int ret = -1;
    if (stack_used != NULL) {
        *stack_used = 0;
    }
    char *tname;
    size_t ssiz;
    int    detach_state = 0;
    int    priority;

    if (amp_os_thread_param) {
        detach_state = amp_os_thread_param->detach_state;
    }
    if (!amp_os_thread_param || !amp_os_thread_param->name) {
        tname = _DEFAULT_THREAD_NAME;
    } else {
        tname = amp_os_thread_param->name;
    }

    if (!amp_os_thread_param || amp_os_thread_param->stack_size == 0) {
        ssiz = _DEFAULT_THREAD_SIZE;
    } else {
        ssiz = amp_os_thread_param->stack_size;
    }

    priority = amp_os_thread_param->priority;

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

    if(thread_handle != NULL){
        *thread_handle = (void *)task;
    }

    return ret;
}

int HAL_ThreadDelete(void *thread_handle)
{
    return 0;
}

int HAL_GetDefaultTaskPriority()
{
    return _DEFAULT_THREAD_PRI;
}

void *HAL_MutexCreate(void)
{
    aos_mutex_t mutex;
    if (0 != aos_mutex_new(&mutex)) {
        return NULL;
    }

    return mutex;
}

void HAL_MutexDestroy(void *mutex)
{
    if (NULL != mutex) {
        aos_mutex_free((aos_mutex_t *)&mutex);
    }
}

void HAL_MutexLock(void *mutex)
{
    if (NULL != mutex) {
        aos_mutex_lock((aos_mutex_t *)&mutex, AOS_WAIT_FOREVER);
    }
}

void HAL_MutexUnlock(void *mutex)
{
    if (NULL != mutex) {
        aos_mutex_unlock((aos_mutex_t *)&mutex);
    }
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
            printf("mutex create failed");
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
static int _timer_change(aos_timer_t *timer, int ms, amp_timer_type repeat)
{
    int ret;

    if (timer == NULL) {
        return -1;
    }

    ret = krhino_timer_change(timer, MS2TICK(ms), repeat ? MS2TICK(ms) : 0);
    return ret;
}

static timer_data_t *_get_timer_data(aos_timer_t *timer)
{
    if (timer == NULL) {
        printf("timer null");
        return NULL;
    }
    if (mutex == NULL) {
        mutex = HAL_MutexCreate();
        if (mutex == NULL) {
            printf("mutex create failed");
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
            printf("mutex create failed");
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
        printf("timer node not found");
        return;
    }
    if (node->cb == NULL) {
        printf("no timer cb");
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

void *HAL_GetTimerPrams(void *timerHandle)
{
    return timerHandle;
}

int32_t HAL_Timer_Start(void *t, int ms, amp_timer_type repeat)
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
    ret = _timer_change(t, ms, repeat);
    if (ret != 0) {
        printf("_timer_change failed %d", ret);
        return -1;
    }

    return aos_timer_start(t);
#endif
}

int32_t HAL_Timer_Stop(void *t)
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

int32_t HAL_Timer_Delete(void *timer)
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

/* queue wrapper data */
typedef struct {
    aos_queue_t queue;
    uint8_t *queue_buf;
    uint32_t item_size;
} amp_osal_queue_t;

void *HAL_MessageQCreate(int32_t queue_length, int32_t item_size)
{
    /* malloc queue wrapper */
    amp_osal_queue_t *osal_queue =
        (amp_osal_queue_t *)aos_malloc(sizeof(amp_osal_queue_t));
    if (osal_queue == NULL) {
        return NULL;
    }
    memset(osal_queue, 0, sizeof(amp_osal_queue_t));

    /* malloc queue buffer */
    osal_queue->queue_buf = (uint8_t *)aos_malloc(queue_length * item_size);
    if (osal_queue->queue_buf == NULL) {
        aos_free(osal_queue);
        return NULL;
    }
    osal_queue->item_size = item_size;

    /* create queue */
    if (0 != aos_queue_new(&osal_queue->queue, osal_queue->queue_buf,
                           queue_length * item_size, item_size)) {
        aos_free(osal_queue->queue_buf);
        aos_free(osal_queue);
        return NULL;
    }

    return (void *)osal_queue;
}

int32_t HAL_MessageQPut(void *mq, void *p_info, uint32_t size, uint32_t millisec)
{
    return aos_queue_send(&((amp_osal_queue_t *)mq)->queue, p_info,
                          ((amp_osal_queue_t *)mq)->item_size);
}

int32_t HAL_MessageQGet(void *mq, void *p_info, uint32_t size, uint32_t millisec)
{
    return aos_queue_recv(&((amp_osal_queue_t *)mq)->queue, millisec,
                          p_info, &size);
}

int32_t HAL_MessageQDelete(void *mq)
{
    aos_queue_free(&((amp_osal_queue_t *)mq)->queue);
    aos_free(((amp_osal_queue_t *)mq)->queue_buf);
    aos_free(mq);
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
    return _SYSINFO_DEVICE_NAME;
}

int32_t HAL_Get_ip(char *ip)
{
    // netmgr_wifi_get_ip(ip);

    if (0 == strcmp(ip, "0.0.0.0")) {
        return -1;
    }
    return 0;
}

int32_t HAL_Get_mac(unsigned char mac[8])
{
    // netmgr_ifconfig_info_t apconfig;

    // netmgr_get_ap_config(&apconfig);
    // memcpy(mac, apconfig.bssid, 6);

    return 0;
}

// static void (*g_network_status_cb)(int status, void *);
// static void *g_network_status_cb_arg;
// static void __wifi_service_event(aos_event_t *event, void *priv)
// {
// 	if (event->type != _WIFI)
// 		return;

//     if (event->code == CODE_WIFI_ON_GOT_IP) {
// 		if (g_network_status_cb)
// 			g_network_status_cb(1, g_network_status_cb_arg);
//     } else if (event->code == CODE_WIFI_ON_DISCONNECT) {
// 		if (g_network_status_cb)
// 			g_network_status_cb(0, g_network_status_cb_arg);
//     }
// }

int32_t HAL_network_status_registercb(void (*cb)(int status, void *), void *arg)
{
	// aos_register_event_filter(EV_WIFI, __wifi_service_event, NULL);
	// g_network_status_cb = cb;
	// g_network_status_cb_arg = arg;
    return 0;
}

int32_t HAL_Get_network_status(void)
{
	// netmgr_stats_t stat;

	// memset(&stat, 0, sizeof(stat));
	// netmgr_stats(INTERFACE_WIFI, &stat);
	// if (!stat.ip_available)
	// 	return 0;
    // return 1;
    return 0;
}

int HAL_GetWirelessInfo(struct _amp_wireless_info_t *info)
{
	return 0;
}

/**
 * @brief get wifi mac address
 * @param[out] mac: mac address，format: "01:23:45:67:89:ab"
 * @param[in] len: size of mac
 * @returns 0: succeed; -1: failed
 */
#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif
int hal_wifi_get_mac_address(char *mac, size_t len)
{
    int ret = 0;
    extern uint8_t* factory_section_get_wifi_address(void);
    uint8_t *_mac = factory_section_get_wifi_address();
    if(_mac == NULL)
        ret = -1;
    else
        snprintf(mac, len, MACSTR, MAC2STR(_mac));
    return ret;
}

char g_chip_id[32];
const char *HAL_Get_Device_Name(void)
{
    int len;
    unsigned char chip_id[16];
    tg_get_chipid(chip_id, 16);
    HAL_Snprintf(g_chip_id, 32, "%x%x%x%x%x%x", chip_id[0], chip_id[1], chip_id[2], chip_id[3], chip_id[4], chip_id[5]);
    return g_chip_id;
}

extern k_mm_head *g_kmm_head;
int32_t HAL_Heap_Memory_Info(amp_heap_info_t *heap_info)
{
    heap_info->heap_total = g_kmm_head->free_size + g_kmm_head->used_size;
    heap_info->heap_used = g_kmm_head->used_size;
    return 0;
}

int32_t HAL_Sysinit(void)
{
    return 0;
}

const char *HAL_Get_Module_Hardware_Version(void)
{
    return "Module_Hardware_version";
}

const char *HAL_Get_Module_Software_Version(void)
{
    return "Module_Software_version";
}
