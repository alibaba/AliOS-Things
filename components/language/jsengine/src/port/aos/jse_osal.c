/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "k_api.h"
#include "aos/init.h"
#include "aos/kernel.h"
#include "aos/yloop.h"

#include "jse_osal.h"

/* task wrapper data */
typedef struct {
    aos_task_t task;
    char *name;
    void *arg;
    jse_osal_task_handler_t routine;
} jse_osal_task_t;

/* timer wrapper data */
typedef struct {
    aos_timer_t timer;
    os_timer_cb cb;
    void *arg;
} jse_osal_timer_t;

/* queue wrapper data */
typedef struct {
    aos_queue_t queue;
    uint8_t *queue_buf;
    uint32_t item_size;
} jse_osal_queue_t;

/* task wrapper, free task wrapper data when task exit */
static void task_wrapper(void *arg)
{
    jse_osal_task_t *osal_task = arg;
    if (osal_task == NULL) {
        return;
    }
    osal_task->routine(osal_task->arg);

    aos_free(osal_task);
}

/* timer wrapper */
static void timer_wrapper(void *timer, void *user)
{
    jse_osal_timer_t *osal_timer = (jse_osal_timer_t *)user;

    osal_timer->cb(osal_timer->arg);
}

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
uint32_t jse_osal_task_get_default_priority(void)
{
    return AOS_DEFAULT_APP_PRI;
}

/**
 * @brief       create task
 *
 * @param[in]   name: task name
 * @param[in]   handler: task function
 * @param[in]   data: task argument
 * @param[in]   size: task stack size
 * @param[in]   task_handle: task handler
 *
 * @return      0: success, -1: failed
 */
int32_t jse_osal_create_task(const char *name, jse_osal_task_handler_t task,
                            void *data, uint32_t size, uint32_t priority,
                            void **task_handle)
{
    jse_osal_task_t *osal_task =
        (jse_osal_task_t *)aos_malloc(sizeof(jse_osal_task_t));
    osal_task->arg     = data;
    osal_task->name    = (char *)name;
    osal_task->routine = task;

    if (0 != aos_task_new_ext(&osal_task->task, name, task_wrapper,
                              (void *)osal_task, size, priority)) {
        if (NULL != task_handle) {
            *task_handle = NULL;
        }
        aos_free(osal_task);
        return -1;
    }

    if (NULL != task_handle) {
        *task_handle = (void *)osal_task;
    }
    return 0;
}

/**
 * @brief       delete task
 *
 * @param[in]   handle: task handler
 *
 */
void jse_osal_delete_task(void *handle)
{
    jse_osal_task_t *osal_task = (jse_osal_task_t *)handle;
    if (NULL == handle) {
        aos_task_exit(0);
        aos_free(osal_task);
    }

    aos_task_delete(osal_task->name);
    aos_free(osal_task);
}

/**
 * @brief       get task name, call this function in task.
 *
 * @return      task name(string), NULL if no task name.
 */
char *jse_osal_get_taskname(void)
{
    return (char *)aos_task_name();
}

/**
 * @brief       get clock time
 *
 * @return      clock time(ms)
 */
uint32_t jse_osal_get_clocktime(void)
{
    return aos_now_ms();
}

/**
 * @brief       create mutex
 *
 * @return      mutex handler
 */
void *jse_osal_new_mutex(void)
{
    aos_mutex_t *pmutex = aos_calloc(1, sizeof(aos_mutex_t));
    if (aos_mutex_new(pmutex) == 0) {
        return pmutex;
    }
    aos_free(pmutex);
    return (NULL);
}

/**
 * @brief       delete mutex
 *
 * @param[in]   mutex: mutex handler
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_del_mutex(void *mutex)
{
    aos_mutex_free(mutex);
}

/**
 * @brief       lock mutex
 *
 * @param[in]   mutex: mutex handler
 * @param[in]   ms: timeout
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_lock_mutex(void *mutex, uint32_t ms)
{
    if (mutex == NULL) return (-3);

    return aos_mutex_lock(mutex, ms);
}

/**
 * @brief       unlock mutex
 *
 * @param[in]   mutex: mutex handler
 * @param[in]   ms: timeout
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_unlock_mutex(void *mutex)
{
    return aos_mutex_unlock(mutex);
}

/**
 * @brief       delay ms
 *
 * @param[in]   ms: delay time(ms)
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_delay(uint32_t ms)
{
    aos_msleep(ms);
    return 0;
}

/**
 * @brief       delay us
 *
 * @param[in]   ms: delay time(us)
 *
 * @return      void
 */
void jse_osal_delay10us(void)
{
#if !defined STM32L496xx && !defined CONFIG_MX108
    uint32_t i = 0;
    uint32_t j = 0;
    for (j = 0; j < 3; ++j) {
        for (i = 0; i < 85; i++) {
            asm("nop");
        }
    }
#endif
}

/**
 * @brief       create an timer
 *
 * @param[in]   callback: timer timeout callback function
 * @param[in]   type: timer type, one-shot timer or repeating timer
 * @param[in]   arg: callback function argument
 *
 * @return      timer handler
 */
osTimerId jse_osal_timer_create(os_timer_cb callback, jse_osal_timer_type type,
                               void *arg)
{
    jse_osal_timer_t *osal_timer =
        (jse_osal_timer_t *)aos_malloc(sizeof(jse_osal_timer_t));
    if (osal_timer == NULL) {
        return NULL;
    }

    osal_timer->cb  = callback;
    osal_timer->arg = arg;

    /* set repeat time to 1000ms default */
    if (0 != aos_timer_new_ext(&osal_timer->timer, timer_wrapper, osal_timer, 1000,
                               (int32_t)type, 0)) {
        aos_free(osal_timer);
        return NULL;
    }

    /* wrapper for timer */
    return osal_timer;
}

/**
 * @brief       start an timer
 *
 * @param[in]   timer_id: timer handler
 * @param[in]   millisec: timeout(ms)
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_timer_start(osTimerId timer_id, uint32_t millisec)
{
    /* set repeat time */
    if (0 !=
        aos_timer_change(&((jse_osal_timer_t *)timer_id)->timer, millisec)) {
        return -1;
    }

    return aos_timer_start(&((jse_osal_timer_t *)timer_id)->timer);
}

/**
 * @brief       stop an timer
 *
 * @param[in]   timer_id: timer handler
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_timer_stop(osTimerId timer_id)
{
    return aos_timer_stop(&((jse_osal_timer_t *)timer_id)->timer);
}

/**
 * @brief       delete an timer
 *
 * @param[in]   timer_id: timer handler
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_timer_delete(osTimerId timer_id)
{
    /* free the aos timer */
    aos_timer_free(&((jse_osal_timer_t *)timer_id)->timer);

    /* free the timer wrapper */
    aos_free(timer_id);
}

/**
 * @brief       get an timer parameter
 *
 * @param[in]   timerHandle: timer handler
 *
 * @return      timer parameter
 */
void *jse_osal_get_timer_param(void *timerHandle)
{
    return timerHandle;
}

/**
 * @brief       create queue
 *
 * @param[in]   queue_length: queue length
 * @param[in]   item_size: queue item size
 *
 * @return      queue handler
 */
osMessageQId jse_osal_messageQ_create(int32_t queue_length, int32_t item_size)
{
    /* malloc queue wrapper */
    jse_osal_queue_t *osal_queue =
        (jse_osal_queue_t *)aos_malloc(sizeof(jse_osal_queue_t));
    if (osal_queue == NULL) {
        return NULL;
    }
    memset(osal_queue, 0, sizeof(jse_osal_queue_t));

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

    return osal_queue;
}

/**
 * @brief       push data into queue
 *
 * @param[in]   queue_id: queue handler
 * @param[in]   p_info: buffer
 * @param[in]   millisec: timeout(ms)
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_messageQ_put(osMessageQId queue_id, void *p_info,
                             uint32_t millisec)
{
    return aos_queue_send(&((jse_osal_queue_t *)queue_id)->queue, p_info,
                          ((jse_osal_queue_t *)queue_id)->item_size);
}

/**
 * @brief       get data from queue
 *
 * @param[in]   queue_id: queue handler
 * @param[out]  p_info: store data to buffer
 * @param[in]   millisec: timeout(ms)
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_messageQ_get(osMessageQId queue_id, void *p_info,
                             uint32_t millisec)
{
    uint32_t size = 0;

    return aos_queue_recv(&((jse_osal_queue_t *)queue_id)->queue, millisec,
                          p_info, &size);
}

/**
 * @brief       delete queue
 *
 * @param[in]   queue_id: queue handler
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_messageQ_delete(osMessageQId queue_id)
{
    aos_queue_free(&((jse_osal_queue_t *)queue_id)->queue);
    aos_free(((jse_osal_queue_t *)queue_id)->queue_buf);
    aos_free(queue_id);
    return 0;
}

#if 0
int32_t jse_osal_post_delayed_action(int32_t ms, jse_osal_call_cb action, void *arg)
{
    return aos_post_delayed_action(ms, action, arg);
}

void jse_osal_cancel_delayed_action(int ms, jse_osal_call_cb action, void *arg)
{
    return aos_cancel_delayed_action(ms, action, arg);
}
#endif
