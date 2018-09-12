/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include "ota_hal_os.h"
#include "ota_log.h"
#include <aos/aos.h>
#include <aos/yloop.h>
#include <hal/hal.h>

/*Memory realloc*/
void *ota_realloc(void *ptr, uint32_t size)
{
    return (void *)aos_realloc(ptr, size);
}

/*Memory calloc*/
void *ota_calloc(uint32_t n, uint32_t size)
{
    return (void *)aos_calloc(n, size);

}

/*Reboot*/
void ota_reboot(void)
{
    aos_reboot();
}

/*Memory malloc*/
void *ota_malloc(uint32_t size)
{
    return aos_malloc(size);

}

/*Memory free*/
void ota_free(void *ptr)
{
    aos_free(ptr);
}

/*Sleep ms*/
void ota_msleep(uint32_t ms)
{
    aos_msleep(ms);
}

typedef struct
{
    aos_task_t task;
    int        detached;
    void *     arg;
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

#define DEFAULT_THREAD_NAME "OTABLEThread"
#define DEFAULT_THREAD_SIZE 4096
#define DEFAULT_THREAD_PRI 32
/*Thread create*/
int ota_thread_create(void **thread_handle, void *(*work_routine)(void *),
                      void *arg, unsigned int stack_size, int *stack_used)
{
    int ret = -1;
    char *                 tname;
    uint32_t               ssiz;
    int                    detach_state        = 0;
    tname = DEFAULT_THREAD_NAME;

    if (stack_size == 0) {
        ssiz = DEFAULT_THREAD_SIZE;
    } else {
        ssiz = stack_size;
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
                           DEFAULT_THREAD_PRI);

    *thread_handle = (void *)task;
    return ret;
}

/*Thread exit*/
void ota_thread_exit(void *thread)
{
    aos_task_exit(0);
}

/*KV set*/
int ota_kv_set(const char *key, const void *val, int len, int sync)
{
    int ret = 0;
    ret     = aos_kv_set(key, val, len, sync);
    return ret;
}

/*KV get*/
int ota_kv_get(const char *key, void *buffer, int *buffer_len)
{
    int ret = 0;
    ret     = aos_kv_get(key, buffer, buffer_len);
    return ret;
}

typedef struct
{
    const char *name;
    int         ms;
    aos_call_t  cb;
    void *      data;
} schedule_timer_t;

static void schedule_timer(void *p)
{
    if (p == NULL) {
        return;
    }

    schedule_timer_t *pdata = p;
    aos_post_delayed_action(pdata->ms, pdata->cb, pdata->data);
}

/*Timer create*/
void *ota_timer_create(const char *name, void (*func)(void *), void *user_data)
{
    schedule_timer_t *timer =
      (schedule_timer_t *)aos_malloc(sizeof(schedule_timer_t));
    if (timer == NULL) {
        return NULL;
    }

    timer->name = name;
    timer->cb   = func;
    timer->data = user_data;

    return timer;
}

/*Timer start*/
int ota_timer_start(void *t, int ms)
{
    if (t == NULL) {
        return -1;
    }
    schedule_timer_t *timer = t;
    timer->ms               = ms;
    return aos_schedule_call(schedule_timer, t);
}

