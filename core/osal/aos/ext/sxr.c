/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/errno.h>
#include <aos/kernel.h>
#include <k_config.h>
#include "sxs_srl.h"
#include "sxs_type.h"
#include "sxs_lib.h"
#include "sxr_ops.h"
#include "sxr_csp.h"
#include "sxr_mutx.h"
#include "sxs_rmt.h"
#include "sxs_idle.h"
#include "sxr_tksd.hp"
#include "sxs_io.h"
#include "sxr_tim.h"

#define AOS_CONFIG_TASK_INFO_NUM RHINO_CONFIG_TASK_INFO_NUM

extern const char* sysinfo_kernel_version;

static unsigned int used_bitmap;
static u8 g_task_load = SXR_NO_TASK;
static u8 g_task_id[SXR_NB_MAX_TASK];

void aos_reboot(void)
{
}

int aos_get_hz(void)
{
    return HAL_TICK1S;
}

const char *aos_version_get(void)
{
    return sysinfo_kernel_version;
}

static u8 get_task_id(void)
{
    u8 id = SXR_NO_TASK;

    if (g_task_load == SXR_NO_TASK) {
        memset(g_task_id, SXR_NO_TASK, SXR_NB_MAX_TASK);
        g_task_load = 0;
    }

    if (g_task_load == SXR_NB_MAX_TASK) {
        return SXR_NO_TASK;
    }

    for (id = 0; id < SXR_NB_MAX_TASK; id++) {
        if (g_task_id[id] == SXR_NO_TASK) {
            return id;
        }
    }

    if (id == SXR_NB_MAX_TASK) {
        return SXR_NO_TASK;
    }

    return SXR_NO_TASK;
}

int aos_task_new(const char *name, void (*fn)(void *), void *arg,
                 int stack_size)
{
    u8 id, index;
    index = get_task_id();
    if (index == SXR_NO_TASK) {
        return -1;
    }

    sxr_TaskDesc_t *ptask = (sxr_TaskDesc_t *)sxr_Malloc(sizeof(sxr_TaskDesc_t) +
                                                         sizeof(void *) * RHINO_CONFIG_TASK_INFO_NUM);
    if (ptask == NULL) {
        return -1;
    }

    ptask->Name = (name == NULL) ? (const ascii *)"noname" : (const ascii *)name;
    ptask->Priority = AOS_DEFAULT_APP_PRI;
    ptask->StackSize = stack_size >> 2;
    ptask->TaskBody = fn;
    ptask->TaskExit = NULL;

    id = sxr_NewTask(ptask);
    sxr_StartTask(id, arg);

    u32 Status = sxr_EnterSc ();
    g_task_id[index] = id;
    g_task_load++;
    sxr_ExitSc (Status);
    return 0;
}

int aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *), void *arg,
                     int stack_size, int prio)
{
    u8 id, index;

    index = get_task_id();
    if (id == SXR_NO_TASK) {
        return -1;
    }

    sxr_TaskDesc_t *ptask = (sxr_TaskDesc_t *)sxr_Malloc(sizeof(sxr_TaskDesc_t) +
                                                         sizeof(void *) * RHINO_CONFIG_TASK_INFO_NUM);
    if (ptask == NULL) {
        return -1;
    }

    ptask->Name = (name == NULL) ? (const ascii *)"noname" : (const ascii *)name;
    ptask->Priority = prio;
    ptask->StackSize = stack_size >> 2;
    ptask->TaskBody = fn;
    ptask->TaskExit = NULL;

    id = sxr_NewTask(ptask);
    sxr_StartTask(id, arg);

    u32 Status = sxr_EnterSc ();
    g_task_id[index] = id;
    g_task_load++;
    sxr_ExitSc (Status);

    return 0;
}

void aos_task_exit(int code)
{
    sxr_TaskDesc_t *desc;
    (void)code;

    desc = sxr_GetCurrentTaskDesc();
    sxr_Free(desc);
    sxr_StopTask(sxr_GetCurrentTaskId());
}

void aos_task_free(void)
{
    u8 i;
    for (i = 0; i < SXR_NB_MAX_TASK; i++) {
        if (g_task_id[i] != SXR_NO_TASK) {
            if (sxr_GetTaskState(g_task_id[i]) == SXR_STOPPED_TSK) {
                sxr_FreeTask(g_task_id[i]);
                u32 Status = sxr_EnterSc ();
                g_task_id[i] = SXR_NO_TASK;
                g_task_load--;
                sxr_ExitSc (Status);

            }
        }
    }
}

const char *aos_task_name(void)
{
    return sxr_GetTaskName();
}

int aos_task_key_create(aos_task_key_t *key)
{
    int i;

    for (i = AOS_CONFIG_TASK_INFO_NUM - 1; i >= 0; i--) {
        if (!((1 << i) & used_bitmap)) {
            used_bitmap |= 1 << i;
            *key = i;

            return 0;
        }
    }

    return -EINVAL;
}

void aos_task_key_delete(aos_task_key_t key)
{
    if (key >= AOS_CONFIG_TASK_INFO_NUM) {
        return;
    }

    used_bitmap &= ~(1 << key);
}

int aos_task_setspecific(aos_task_key_t key, void *vp)
{
    sxr_TaskDesc_t *desc = NULL;

    if (key >= AOS_CONFIG_TASK_INFO_NUM) {
        return -EINVAL;
    }

    desc = sxr_GetCurrentTaskDesc();

    u32 status = sxr_EnterSc();

    desc->user_info[key] = vp;

    sxr_ExitSc(status);

    return 0;
}

void *aos_task_getspecific(aos_task_key_t key)
{
    void *vp = NULL;
    sxr_TaskDesc_t *desc = NULL;

    if (key >= AOS_CONFIG_TASK_INFO_NUM) {
        return NULL;
    }

    desc = sxr_GetCurrentTaskDesc();

    vp = desc->user_info[key];

    return vp;
}

typedef struct {
    u8 Id;
    u8 UserId;
} aos_to_sxr_t;

int aos_mutex_new(aos_mutex_t *mutex)
{
    aos_to_sxr_t *pmutex = (aos_to_sxr_t *)sxr_Malloc(sizeof(aos_to_sxr_t));
    if (pmutex == NULL) {
        return -1;
    }

    if (mutex == NULL) {
        sxr_Free(pmutex);
        return -EINVAL;
    }

    pmutex->Id = sxr_NewMutex();
    pmutex->UserId = 0xFF;

    mutex->hdl = (void *)pmutex;

    return 0;
}

void aos_mutex_free(aos_mutex_t *mutex)
{
    if (mutex == NULL) {
        return;
    }

    sxr_FreeMutex(((aos_to_sxr_t *)(mutex->hdl))->Id);
    sxr_Free(mutex->hdl);

    mutex->hdl = NULL;
}

int aos_mutex_lock(aos_mutex_t *mutex, unsigned int timeout)
{
    if (mutex == NULL) {
        return -EINVAL;
    }

    ((aos_to_sxr_t *)(mutex->hdl))->UserId = sxr_TakeMutex(((aos_to_sxr_t *)(mutex->hdl))->Id);

    return 0;
}

int aos_mutex_unlock(aos_mutex_t *mutex)
{
    if (mutex == NULL) {
        return -EINVAL;
    }

    sxr_ReleaseMutex(((aos_to_sxr_t *)(mutex->hdl))->Id, ((aos_to_sxr_t *)(mutex->hdl))->UserId);

    return 0;
}

int aos_mutex_is_valid(aos_mutex_t *mutex)
{
    aos_to_sxr_t *pmutex;

    if (mutex == NULL) {
        return 0;
    }

    pmutex = mutex->hdl;

    if (pmutex == NULL) {
        return 0;
    }

    return 1;
}

int aos_sem_new(aos_sem_t *sem, int count)
{
    aos_to_sxr_t *psem = (aos_to_sxr_t *)sxr_Malloc(sizeof(aos_to_sxr_t));
    if (psem == NULL) {
        return -1;
    }

    if (sem == NULL) {
        sxr_Free(psem);
        return -EINVAL;
    }

    psem->Id = sxr_NewSemaphore(count);

    sem->hdl = (void *)psem;

    return 0;
}

void aos_sem_free(aos_sem_t *sem)
{
    if (sem == NULL) {
        return;
    }

    sxr_FreeSemaphore(((aos_to_sxr_t *)(sem->hdl))->Id);
    sxr_Free(sem->hdl);

    sem->hdl = NULL;
}

int aos_sem_wait(aos_sem_t *sem, unsigned int timeout)
{
    if (sem == NULL) {
        return -EINVAL;
    }

    sxr_TakeSemaphore(((aos_to_sxr_t *)(sem->hdl))->Id);

    return 0;
}

void aos_sem_signal(aos_sem_t *sem)
{
    if (sem == NULL) {
        return;
    }

    sxr_ReleaseSemaphore(((aos_to_sxr_t *)(sem->hdl))->Id);
}

int aos_sem_is_valid(aos_sem_t *sem)
{
    aos_to_sxr_t *psem;

    if (sem == NULL) {
        return 0;
    }

    psem = sem->hdl;

    if (psem == NULL) {
        return 0;
    }

    return 1;
}

void aos_sem_signal_all(aos_sem_t *sem)
{
    if (sem == NULL) {
        return;
    }

    sxr_ReleaseSemaphore(((aos_to_sxr_t *)(sem->hdl))->Id);
}


typedef struct {
    void *msg;
    unsigned int size;
} queue_event_t;

int aos_queue_new(aos_queue_t *queue, void *buf, unsigned int size, int max_msg)
{
    aos_to_sxr_t *pqueue = (aos_to_sxr_t *)sxr_Malloc(sizeof(aos_to_sxr_t));
    if (pqueue == NULL) {
        return -1;
    }

    if (queue == NULL) {
        sxr_Free(pqueue);
        return -EINVAL;
    }

    pqueue->Id = sxr_NewMailBox();
    queue->hdl = (void *)pqueue;

    return 0;
}

void aos_queue_free(aos_queue_t *queue)
{
    if (queue == NULL) {
        return;
    }

    sxr_FreeMailBox(((aos_to_sxr_t *)(queue->hdl))->Id);
    sxr_Free(queue->hdl);

    queue->hdl = NULL;
}

int aos_queue_send(aos_queue_t *queue, void *msg, unsigned int size)
{
    if ((queue == NULL) || (msg == NULL)) {
        return -EINVAL;
    }

    queue_event_t *evt = (queue_event_t *)sxr_Malloc(sizeof(queue_event_t ));
    evt->msg = msg;
    evt->size = size;

    sxr_Send(evt, ((aos_to_sxr_t *)(queue->hdl))->Id, SXR_SEND_MSG);

    return 0;
}

int aos_queue_recv(aos_queue_t *queue, unsigned int ms, void *msg, unsigned int *size)
{
    if (queue == NULL) {
        return -EINVAL;
    }

    u32 event = 0;
    queue_event_t *evt = (queue_event_t *)sxr_Wait(&event, ((aos_to_sxr_t *)(queue->hdl))->Id);
    if (evt == NULL) {
        return -1;
    }

    memcpy(msg, evt->msg, evt->size);
    *size = evt->size;
    sxr_Free(evt);
    return 0;
}

int aos_queue_is_valid(aos_queue_t *queue)
{
    aos_to_sxr_t *pqueue;

    if (queue == NULL) {
        return 0;
    }

    pqueue = queue->hdl;

    if (pqueue == NULL) {
        return 0;
    }

    return 1;
}

#define AOS_TIMER_UNUSED 0
#define AOS_TIMER_USED  1
#define AOS_MAX_TIMERS 20
struct aos_timer_array_t {
    aos_timer_t *timer;
    void (*callbackFunc)(void *, void *);
    void *arg;
    UINT8 flags;
    int ms;
    int repeat;
};
static struct aos_timer_array_t aos_timer_array[AOS_MAX_TIMERS];

void aos_Timer_Initialise(void)
{
    memset(aos_timer_array, 0, sizeof(aos_timer_array));
}

void aos_timer_callback(void *pArgs)
{
    struct aos_timer_array_t *timer = (struct aos_timer_array_t *)pArgs;
    if (timer->repeat == 1) {
        sxr_StartFunctionTimer((timer->ms) * HAL_TICK1S / 1000,
                               aos_timer_callback, timer, 0);
    }
    timer->callbackFunc(timer->timer, timer->arg);
}

int aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *),
                  void *arg, int ms, int repeat)
{
    UINT32 csStatus;
    UINT8 i;

    if (timer == NULL) {
        return -EINVAL;
    }

    for (i = 0; i < AOS_MAX_TIMERS; i++) {
        if (aos_timer_array[i].flags == AOS_TIMER_UNUSED) {
            csStatus = hal_SysEnterCriticalSection();
            if (aos_timer_array[i].flags == AOS_TIMER_UNUSED) {
                aos_timer_array[i].callbackFunc = fn;
                aos_timer_array[i].timer = timer;
                aos_timer_array[i].arg = arg;
                aos_timer_array[i].ms = ms;
                aos_timer_array[i].repeat = repeat;
                aos_timer_array[i].flags = AOS_TIMER_USED;

                sxr_StartFunctionTimer((aos_timer_array[i].ms) * HAL_TICK1S / 1000,
                                       aos_timer_callback, &aos_timer_array[i], 0);
                hal_SysExitCriticalSection(csStatus);
                break;
            }
            hal_SysExitCriticalSection(csStatus);
        }
    }

    if (i == AOS_MAX_TIMERS) {
        return -1;
    }

    return 0;
}

void aos_timer_free(aos_timer_t *timer)
{
    UINT32 csStatus;
    UINT8 i;

    if (timer == NULL) {
        return;
    }

    for (i = 0; i < AOS_MAX_TIMERS; i++) {
        if (aos_timer_array[i].flags == AOS_TIMER_USED) {
            csStatus = hal_SysEnterCriticalSection();
            if ((UINT32)aos_timer_array[i].timer == (UINT32)timer) {
                memset(&aos_timer_array[i], 0, sizeof(struct aos_timer_array_t));
                hal_SysExitCriticalSection(csStatus);
                break;
            }
            hal_SysExitCriticalSection(csStatus);
        }
    }

    if (i == AOS_MAX_TIMERS) {
        return;
    }

    return;
}

int aos_timer_start(aos_timer_t *timer)
{
    UINT32 sc;
    UINT8 i;

    if (timer == NULL) {
        return -EINVAL;
    }

    for (i = 0; i < AOS_MAX_TIMERS; i++) {
        if (aos_timer_array[i].flags == AOS_TIMER_USED) {
            sc = hal_SysEnterCriticalSection();
            if ((UINT32)aos_timer_array[i].timer == (UINT32)timer) {
                sxr_StartFunctionTimer((aos_timer_array[i].ms) * HAL_TICK1S / 1000,
                                       aos_timer_callback, &aos_timer_array[i], 0);
                hal_SysExitCriticalSection(sc);
                break;
            }
            hal_SysExitCriticalSection(sc);
        }
    }

    if (i == AOS_MAX_TIMERS) {
        return -1;
    }

    return 0;
}

int aos_timer_stop(aos_timer_t *timer)
{
    UINT32 sc;
    UINT8 i;

    if (timer == NULL) {
        return -EINVAL;
    }

    for (i = 0; i < AOS_MAX_TIMERS; i++) {
        if (aos_timer_array[i].flags == AOS_TIMER_USED) {
            sc = hal_SysEnterCriticalSection();
            if ((UINT32)aos_timer_array[i].timer == (UINT32)timer) {
                sxr_StopFunctionTimer2(aos_timer_callback,
                                       &aos_timer_array[i]);
                hal_SysExitCriticalSection(sc);
                break;
            }
            hal_SysExitCriticalSection(sc);
        }
    }

    if (i == AOS_MAX_TIMERS) {
        return -1;
    }

    return 0;
}

int aos_timer_change(aos_timer_t *timer, int ms)
{
    UINT32 sc;
    UINT8 i;

    if (timer == NULL) {
        return -EINVAL;
    }

    for (i = 0; i < AOS_MAX_TIMERS; i++) {
        if (aos_timer_array[i].flags == AOS_TIMER_USED) {
            sc = hal_SysEnterCriticalSection();
            if ((UINT32)aos_timer_array[i].timer == (UINT32)timer) {
                aos_timer_array[i].ms = ms;
                hal_SysExitCriticalSection(sc);
                break;
            }
            hal_SysExitCriticalSection(sc);
        }
    }

    if (i == AOS_MAX_TIMERS) {
        return -1;
    }

    return 0;
}

void *aos_zalloc(unsigned int size)
{
    void *tmp = NULL;

    if (size == 0) {
        return NULL;
    }

    tmp = sxr_Malloc(size);

    if (tmp) {
        memset(tmp, 0, size);
    }

    return tmp;
}

void *aos_malloc(unsigned int size)
{
    void *tmp = NULL;

    if (size == 0) {
        return NULL;
    }

    tmp = sxr_Malloc(size);

    return tmp;
}

void aos_free(void *mem)
{
    if (mem == NULL) {
        return;
    }

    sxr_Free(mem);
}

long long aos_now(void)
{
    return aos_now_ms() * 1000 * 1000;
}

long long aos_now_ms(void)
{
    HAL_TIM_RTC_TIME_T RtcTime;

    if (hal_TimRtcGetTime(&RtcTime) == HAL_ERR_NO) {
        return RtcTime.sec * 1000 + RtcTime.min * 1000 * 60 + RtcTime.hour * 1000 * 60 * 60;
    } else {
        return 0;
    }
}

void aos_msleep(int ms)
{
    sxr_Sleep(ms MS_WAITING);
}

