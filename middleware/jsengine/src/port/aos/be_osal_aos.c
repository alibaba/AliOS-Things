/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <k_api.h>
#include "aos/init.h"
#include "aos/kernel.h"
#include "aos/yloop.h"

#include "be_port_osal.h"

typedef struct {
    aos_task_t task;
    char *name;
    void *arg;
    void *(*routine)(void *arg);
} be_osal_task_t;

typedef struct {
    aos_timer_t timer;
    os_timer_cb cb;
    void *arg;
} be_osal_timer_t;

typedef struct {
    aos_queue_t queue;
    uint8_t *queue_buf;
    uint32_t item_size;
} be_osal_queue_t;

uint32_t be_osal_task_get_default_priority(void)
{
    return AOS_DEFAULT_APP_PRI;
}

static void task_wrapper(void *arg)
{
    be_osal_task_t *osal_task = arg;
    if (osal_task == NULL) {
        return;
    }
    osal_task->routine(osal_task->arg);

    aos_free(osal_task);
}

/***************************************************
 * @fn          be_osal_create_task()
 *
 * @brief       create task
 *
 * @param[in]       name：任务名称
 *              handler: 任务回调函数
 *              data：传给handler的指针
 *              size：任务栈大小
 *              task_handle: 用于保存task handle的二维指针，
 *              如不需在另一个task显示的销毁该任务，可以传入NULL
 *
 * @return		成功 0 失败 -1
 **************************************************/
int32_t be_osal_create_task(const char *name, be_osal_task_handler_t task,
                            void *data, uint32_t size, uint32_t priority,
                            void **task_handle)
{
    be_osal_task_t *osal_task =
        (be_osal_task_t *)aos_malloc(sizeof(be_osal_task_t));
    osal_task->arg = data;
    osal_task->name = name;
    osal_task->routine = task;

    if (0 != aos_task_new_ext(&osal_task->task, name, task_wrapper, (void *)osal_task,
                              size, priority)) {
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

/* 如果是task退出的话，handle为NULL，如果是删除其他task，需要指定handle */
/***************************************************
 * @fn          be_osal_delete_task
 *
 * @brief       删除task
 *
 * @param handle：be_osal_create_task创建task时传入task_handle得到的handle
 *			  如在本task任务中退出，可传入NULL
 *
 * @return	  无.
 **************************************************/
void be_osal_delete_task(void *handle)
{
    be_osal_task_t *osal_task = (be_osal_task_t *)handle;
    if (NULL == handle) {
        aos_task_exit(0);
        aos_free(osal_task);
    }

    aos_task_delete(osal_task->name);
    aos_free(osal_task);
}

/***************************************************
 * @fn		  be_osal_get_taskname
 *
 * @brief	  获取任务task名称
 *
 * @param	  无
 *
 *
 * @return	  成功：返回该buffer的指针，失败返回null
 **************************************************/
char *be_osal_get_taskname(void)
{
    return (char *)aos_task_name();
}

/***************************************************
 * @fn			be_osal_get_clocktime
 *
 * @brief		获取当前系统运行的时钟时间
 *
 * @param		无
 *
 * @return		字符串.
 **************************************************/
uint32_t be_osal_get_clocktime()
{
    return aos_now_ms();
}

/***************************************************
 * @fn 		 be_osal_new_mutex
 *
 * @brief     初始化一个互斥信号量
 *
 * @param     无
 *
 * @return 	 void指针
 **************************************************/
void *be_osal_new_mutex(void)
{
    aos_mutex_t *pmutex = aos_calloc(1, sizeof(aos_mutex_t));
    if (aos_mutex_new(pmutex) == 0) {
        return pmutex;
    }
    aos_free(pmutex);
    return (NULL);
}

/***************************************************
 * @fn 		 be_osal_del_mutex
 *
 * @brief     删除一个互斥信号量
 *
 * @param     be_osal_new_mutex返回的mutex指针
 *
 * @return 	 成功返回0，失败返回其他
 **************************************************/
int32_t be_osal_del_mutex(void *mutex)
{
    aos_mutex_free(mutex);
}

/***************************************************
 * @fn 		 be_osal_lock_mutex
 *
 * @brief     获取mutex互斥锁，获取不到时会被lock住
 *
 * @param     mutex：互斥锁
 *　　　　　       ms: 超时时间，在ｍｓ时间内获取不到则返回
 *
 * @return 	 无.
 **************************************************/
int32_t be_osal_lock_mutex(void *mutex, uint32_t ms)
{
    if (mutex == NULL) return (-3);

    return aos_mutex_lock(mutex, ms);
}

int32_t be_osal_unlock_mutex(void *mutex)
{
    return aos_mutex_unlock(mutex);
}

int32_t be_osal_delay(uint32_t ms)
{
    aos_msleep(ms);
    return 0;
}

void be_osal_delay10us(void)
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

/* AliOS Things 的timer回调函数有2个参数，所以使用该函数进行封装 */
static void timer_cb(void *timer, void *user)
{
    be_osal_timer_t *osal_timer = (be_osal_timer_t *)user;

    osal_timer->cb(osal_timer->arg);
}

/***************************************************
 * @fn		 be_osal_timer_start
 *
 * @brief	 开启或者重启一个定时器
 *
 * @param	 osTimerId：定时器Id
 *			 millisec：定时器的定时时间
 *
 * @return	 成功返回0，失败返回-1
 **************************************************/
osTimerId be_osal_timer_create(os_timer_cb callback, be_osal_timer_type type,
                               void *arg)
{
    be_osal_timer_t *osal_timer =
        (be_osal_timer_t *)aos_malloc(sizeof(be_osal_timer_t));
    if (osal_timer == NULL) {
        return NULL;
    }

    osal_timer->cb  = callback;
    osal_timer->arg = arg;

    /* set repeat time to 1000ms default */
    if (0 != aos_timer_new_ext(&osal_timer->timer, timer_cb, osal_timer, 1000,
                               (int32_t)type, 0)) {
        aos_free(osal_timer);
        return NULL;
    }

    /* wrapper for timer */
    return osal_timer;
}

/***************************************************
 * @fn		 be_osal_timer_start
 *
 * @brief	 开启或者重启一个定时器
 *
 * @param	 osTimerId：定时器Id
 *			 millisec：定时器的定时时间
 *
 * @return	 成功返回0，失败返回-1
 **************************************************/

int32_t be_osal_timer_start(osTimerId timer_id, uint32_t millisec)
{
    /* set repeat time */
    if (0 !=
        aos_timer_change(&((be_osal_timer_t *)timer_id)->timer, millisec)) {
        return -1;
    }

    return aos_timer_start(&((be_osal_timer_t *)timer_id)->timer);
}

/***************************************************
 * @fn		 be_osal_timer_stop
 *
 * @brief	 停止一个定时器
 *
 * @param	 osTimerId：定时器Id
 *
 * @return	 成功返回0，失败返回-1
 **************************************************/

int32_t be_osal_timer_stop(osTimerId timer_id)
{
    return aos_timer_stop(&((be_osal_timer_t *)timer_id)->timer);
}

/***************************************************
 * @fn		 be_osal_timer_delete
 *
 * @brief	 删除一个定时器
 *
 * @param	 osTimerId：定时器Id
 *
 * @return	 成功返回0，失败返回-1
 **************************************************/

int32_t be_osal_timer_delete(osTimerId timer_id)
{
    /* free the aos timer */
    aos_timer_free(&((be_osal_timer_t *)timer_id)->timer);

    /* free the timer wrapper */
    aos_free(timer_id);
}

/**************************************************
 * @fn 		 be_osal_get_timer_param
 *
 * @brief     在timer回调函数中获取be_osal_timer_create的arg参数
 *
 * @param     timerHandle定时器回调函数的handle参数
 *
 *
 * @return 	 成功返回一个指针指向arg参数
 **************************************************/

void *be_osal_get_timer_param(void *timerHandle)
{
    return timerHandle;
}

/**************************************************
 * @fn 		 be_osal_messageQ_create
 *
 * @brief     创建一个消息队列
 *
 * @param     queue_length: 消息对列的长度
 *            item_size: 消息队列成员的大小
 *
 * @return 	 成功返回对列的handler指针　失败返回NULL
 **************************************************/
osMessageQId be_osal_messageQ_create(int32_t queue_length, int32_t item_size)
{
    /* malloc queue wrapper */
    be_osal_queue_t *osal_queue =
        (be_osal_queue_t *)aos_malloc(sizeof(be_osal_queue_t));
    if (osal_queue == NULL) {
        return NULL;
    }
    memset(osal_queue, 0, sizeof(be_osal_queue_t));

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

/**************************************************
 * @fn 		 be_osal_messageQ_put
 *
 * @brief     往消息队列里面发送消息
 *
 * @param     queue_id: 消息对列的handler ID
 *            p_info: 待发送的消息buffer指针
 *            millisec: 如果消息队列满时的等待时间ms
 *
 * @return 	 发送成功返回0 失败返回其他
 **************************************************/
int32_t be_osal_messageQ_put(osMessageQId queue_id, void *p_info,
                             uint32_t millisec)
{
    return aos_queue_send(&((be_osal_queue_t *)queue_id)->queue, p_info,
                          ((be_osal_queue_t *)queue_id)->item_size);
}

/**************************************************
 * @fn 		 be_osal_messageQ_get
 *
 * @brief     从指定的消息队列中读取消息
 *
 * @param     queue_id: 消息对列的handler ID
 *            p_info: 待发送的消息buffer指针
 *            millisec: 如果消息队列为空时的等待时间ms
 *
 * @return 	 成功返回0　失败返回-1
 **************************************************/
int32_t be_osal_messageQ_get(osMessageQId queue_id, void *p_info,
                             uint32_t millisec)
{
    uint32_t size = 0;

    return aos_queue_recv(&((be_osal_queue_t *)queue_id)->queue, millisec,
                          p_info, &size);
}
/***************************************************
 * @fn		 be_osal_messageQ_delete
 *
 * @brief	 删除一个消息队列
 *
 * @param	 queue_id: 消息对列的handler ID
 *
 * @return	 成功返回0，失败返回-1
 **************************************************/
int32_t be_osal_messageQ_delete(osMessageQId queue_id)
{
    aos_free(((be_osal_queue_t *)queue_id)->queue_buf);
    aos_free(queue_id);
    return 0;
}

#if 0
int32_t be_osal_post_delayed_action(int32_t ms, be_osal_call_cb action, void *arg)
{
    return aos_post_delayed_action(ms, action, arg);
}

void be_osal_cancel_delayed_action(int ms, be_osal_call_cb action, void *arg)
{
    return aos_cancel_delayed_action(ms, action, arg);
}
#endif
