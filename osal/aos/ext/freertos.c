/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include "aos/kernel.h"

#define ms2tick(ms) (((ms)+portTICK_PERIOD_MS-1)/portTICK_PERIOD_MS)

void aos_reboot(void)
{
}

int aos_get_hz(void)
{
    return 100;
}

const char *aos_version_get(void)
{
    return "aos-linux-xxx";
}

#define AOS_MAGIC 0x20171020
typedef struct {
    StaticTask_t fTask;
    uint32_t key_bitmap;
    void *keys[4];
    void *stack;
    char name[32];
    uint32_t magic;
} AosStaticTask_t;

struct targ {
    AosStaticTask_t *task;
    void (*fn)(void *);
    void *arg;
};

static void dfl_entry(void *arg)
{
    struct targ *targ = arg;
    void (*fn)(void *) = targ->fn;
    void *farg = targ->arg;
    free(targ);

    fn(farg);

    vTaskDelete(NULL);
}

void vPortCleanUpTCB(void *pxTCB)
{
    AosStaticTask_t *task = (AosStaticTask_t *)pxTCB;

    if (task->magic != AOS_MAGIC)
        return;

    free(task->stack);
    free(task);
}

int aos_task_new(const char *name, void (*fn)(void *), void *arg,
                 int stack_size)
{
    xTaskHandle xHandle;
    AosStaticTask_t *task = malloc(sizeof(AosStaticTask_t));
    struct targ *targ = malloc(sizeof(*targ));
    void *stack = malloc(stack_size);

    bzero(stack, stack_size);
    bzero(task, sizeof(*task));
    task->key_bitmap = 0xfffffff0;
    task->stack = stack;
    strncpy(task->name, name, sizeof(task->name)-1);
    task->magic = AOS_MAGIC;

    targ->task = task;
    targ->fn = fn;
    targ->arg = arg;

    stack_size /= sizeof(StackType_t);
    xHandle = xTaskCreateStatic(dfl_entry, name, stack_size, targ,
                                10, stack, &task->fTask);
    if (xHandle == NULL) {
        free(task);
        free(stack);
        free(targ);
    }
    return xHandle ? 0 : -1;
}

int aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *), void *arg,
                     int stack_size, int prio)
{
    return aos_task_new(name, fn, arg, stack_size);
}

void aos_task_exit(int code)
{
    vTaskDelete(NULL);
}

const char *aos_task_name(void)
{
    AosStaticTask_t *task = (AosStaticTask_t *)xTaskGetCurrentTaskHandle();
    if (task->magic != AOS_MAGIC)
        return "unknown";
    return task->name;
}

int aos_task_key_create(aos_task_key_t *key)
{
    AosStaticTask_t *task = (AosStaticTask_t *)xTaskGetCurrentTaskHandle();
    int i;

    if (task->magic != AOS_MAGIC)
        return -1;

    for (i=0;i<4;i++) {
        if (task->key_bitmap & (1 << i))
            continue;

        task->key_bitmap |= 1 << i;
        *key = i;
        return 0;
    }

    return -1;
}

void aos_task_key_delete(aos_task_key_t key)
{
    AosStaticTask_t *task = (AosStaticTask_t *)xTaskGetCurrentTaskHandle();
    if (task->magic != AOS_MAGIC)
        return;
    task->key_bitmap &= ~(1 << key);
}

int aos_task_setspecific(aos_task_key_t key, void *vp)
{
    AosStaticTask_t *task = (AosStaticTask_t *)xTaskGetCurrentTaskHandle();
    if (key >= 4)
        return -1;

    if (task->magic != AOS_MAGIC)
        return -1;

    task->keys[key] = vp;
    return 0;
}

void *aos_task_getspecific(aos_task_key_t key)
{
    AosStaticTask_t *task = (AosStaticTask_t *)xTaskGetCurrentTaskHandle();
    if (key >= 4)
        return NULL;

    if (task->magic != AOS_MAGIC)
        return NULL;

    return task->keys[key];
}

int aos_mutex_new(aos_mutex_t *mutex)
{
    SemaphoreHandle_t mux = xSemaphoreCreateMutex();
    mutex->hdl = mux;
    return mux != NULL ? 0 : -1;
}

void aos_mutex_free(aos_mutex_t *mutex)
{
    vSemaphoreDelete(mutex->hdl);
}

int aos_mutex_lock(aos_mutex_t *mutex, unsigned int ms)
{
    if (mutex) {
        xSemaphoreTake(mutex->hdl, ms == AOS_WAIT_FOREVER ? portMAX_DELAY : ms2tick(ms));
    }
    return 0;
}

int aos_mutex_unlock(aos_mutex_t *mutex)
{
    if (mutex) {
        xSemaphoreGive(mutex->hdl);
    }
    return 0;
}

int aos_mutex_is_valid(aos_mutex_t *mutex)
{
    return mutex->hdl != NULL;
}

int aos_sem_new(aos_sem_t *sem, int count)
{
    SemaphoreHandle_t s = xSemaphoreCreateCounting(128, count);
    sem->hdl = s;
    return 0;
}

void aos_sem_free(aos_sem_t *sem)
{
    if (sem == NULL) {
        return;
    }

    vSemaphoreDelete(sem->hdl);
}

int aos_sem_wait(aos_sem_t *sem, unsigned int ms)
{
    if (sem == NULL) {
        return -1;
    }

    int ret = xSemaphoreTake(sem->hdl, ms == AOS_WAIT_FOREVER ? portMAX_DELAY : ms2tick(ms));
    return ret == pdPASS ? 0 : -1;
}

void aos_sem_signal(aos_sem_t *sem)
{
    if (sem == NULL) {
        return;
    }

    xSemaphoreGive(sem->hdl);
}

int aos_sem_is_valid(aos_sem_t *sem)
{
    return sem && sem->hdl != NULL;
}

void aos_sem_signal_all(aos_sem_t *sem)
{
    aos_sem_signal(sem);
}

int aos_queue_new(aos_queue_t *queue, void *buf, unsigned int size, int max_msg)
{
    xQueueHandle q;
    q = xQueueCreate(size / max_msg, max_msg);
    queue->hdl = q;
    return 0;
}

void aos_queue_free(aos_queue_t *queue)
{
    vQueueDelete(queue->hdl);
}

int aos_queue_send(aos_queue_t *queue, void *msg, unsigned int size)
{
    return xQueueSend(queue->hdl, msg, portMAX_DELAY) == pdPASS ? 0 : -1;
}

int aos_queue_recv(aos_queue_t *queue, unsigned int ms, void *msg,
                   unsigned int *size)
{
    return xQueueReceive(queue->hdl, msg, ms == AOS_WAIT_FOREVER ? portMAX_DELAY : ms2tick(ms))
           == pdPASS ? 0 : -1;
}

int aos_queue_is_valid(aos_queue_t *queue)
{
    return queue && queue->hdl != NULL;
}

void *aos_queue_buf_ptr(aos_queue_t *queue)
{
    return NULL;
}

int aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *),
                  void *arg, int ms, int repeat)
{
    return -1;
}

void aos_timer_free(aos_timer_t *timer)
{
}

int aos_timer_start(aos_timer_t *timer)
{
    return -1;
}

int aos_timer_stop(aos_timer_t *timer)
{
    return -1;
}

int aos_timer_change(aos_timer_t *timer, int ms)
{
    return -1;
}

int aos_workqueue_create(aos_workqueue_t *workqueue, int pri, int stack_size)
{
    return -1;
}

struct work {
    void (*fn)(void *);
    void *arg;
    int dly;
};

int aos_work_init(aos_work_t *work, void (*fn)(void *), void *arg, int dly)
{
    struct work *w = malloc(sizeof(*w));
    w->fn = fn;
    w->arg = arg;
    w->dly = dly;
    work->hdl = w;
    return 0;
}

void aos_work_destroy(aos_work_t *work)
{
    free(work->hdl);
}

int aos_work_run(aos_workqueue_t *workqueue, aos_work_t *work)
{
    return aos_work_sched(work);
}

static void worker_entry(void *arg)
{
    struct work *w = arg;
    if (w->dly) {
        usleep(w->dly * 1000);
    }
    w->fn(w->arg);
}

int aos_work_sched(aos_work_t *work)
{
    struct work *w = work->hdl;
    return aos_task_new("worker", worker_entry, w, 8192);
}

int aos_work_cancel(aos_work_t *work)
{
    return -1;
}

void *aos_zalloc(unsigned int size)
{
    return calloc(size, 1);
}

void *aos_malloc(unsigned int size)
{
    return malloc(size);
}

void *aos_calloc(unsigned int nitems, unsigned int size)
{
    return calloc(nitems, size);
}

void *aos_realloc(void *mem, unsigned int size)
{
    return realloc(mem, size);
}

void aos_alloc_trace(void *addr, size_t allocator)
{
}

void aos_free(void *mem)
{
    free(mem);
}

#ifndef timercmp
#define timercmp(a, b, op) ({ \
    unsigned long long _v1 = (a)->tv_sec * 1000000ULL + (a)->tv_usec; \
    unsigned long long _v2 = (b)->tv_sec * 1000000ULL + (b)->tv_usec; \
    _v1 op _v2; \
})

void timersub(struct timeval *a, struct timeval *b, struct timeval *res)
{
    res->tv_usec = a->tv_usec - b->tv_usec;
    res->tv_sec = a->tv_sec - b->tv_sec;
    if (res->tv_usec < 0) {
        res->tv_usec += 1000000;
        res->tv_sec -= 1;
    }
}
#endif

static struct timeval sys_start_time;
long long aos_now(void)
{
    struct timeval tv;
    long long ns;
    gettimeofday(&tv, NULL);
    timersub(&tv, &sys_start_time, &tv);
    ns = tv.tv_sec * 1000000LL + tv.tv_usec;
    return ns * 1000LL;
}

long long aos_now_ms(void)
{
    struct timeval tv;
    long long ms;
    gettimeofday(&tv, NULL);
    timersub(&tv, &sys_start_time, &tv);
    ms = tv.tv_sec * 1000LL + tv.tv_usec / 1000;
    return ms;
}

void aos_msleep(int ms)
{
    usleep(ms * 1000);
}

void aos_init(void)
{
    gettimeofday(&sys_start_time, NULL);
}

void aos_start(void)
{
}

void dumpsys_task_func(void)
{
#if configUSE_TRACE_FACILITY
    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize, x;
    uint32_t ulTotalRunTime, ulStatsAsPercentage;

    uxArraySize = uxTaskGetNumberOfTasks();
    pxTaskStatusArray = malloc( uxArraySize * sizeof( TaskStatus_t ) );
    if( pxTaskStatusArray == NULL )
        return;

    uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulTotalRunTime );
    // For percentage calculations.
    ulTotalRunTime /= 100UL;

    // Avoid divide by zero errors.
    if( ulTotalRunTime > 0 )
    {
        // For each populated position in the pxTaskStatusArray array,
        // format the raw data as human readable ASCII data
        for( x = 0; x < uxArraySize; x++ )
        {
            // What percentage of the total run time has the task used?
            // This will always be rounded down to the nearest integer.
            // ulTotalRunTimeDiv100 has already been divided by 100.
            ulStatsAsPercentage = pxTaskStatusArray[ x ].ulRunTimeCounter / ulTotalRunTime;

            if( ulStatsAsPercentage > 0UL )
            {
                printf("%s\t\t%u\t\t%u%%\r\n", pxTaskStatusArray[ x ].pcTaskName, pxTaskStatusArray[ x ].ulRunTimeCounter, ulStatsAsPercentage );
            }
            else
            {
                printf("%s\t\t%u\t\t<1%%\r\n", pxTaskStatusArray[ x ].pcTaskName, pxTaskStatusArray[ x ].ulRunTimeCounter );
            }
        }
    }

    free( pxTaskStatusArray );
#endif
}
