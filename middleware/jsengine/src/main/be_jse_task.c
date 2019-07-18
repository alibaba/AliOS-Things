/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/*
 *文件功能说明：实现bone engine jse main task的消息事件处理机制，
  所有jse回调事件最终都需要发送消息到该jse main task处理（重要）
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "be_jse_task.h"
#include "be_log.h"
#include "be_port_osal.h"
#include "hal/system.h"

#define JSE_MAIN_TASK_TICK (500)
#define JSE_MSGQ_WAITIME (2000)
#define JSE_MSGQ_MAX_NUM 64
#define JSE_TASK_MUTEX_WAITIME 5000

static osMessageQId jse_task_mq = NULL; /*JSE MAIN TASK的消息队列*/
static void *jse_task_mutex     = NULL; /*JSE MAIN TASK的互斥锁*/

typedef struct {
    bone_engine_call_t action;
    void *arg;
} shedule_timer_msg_t;

int32_t bone_engine_task_yield(uint32_t timeout)
{
    jse_task_msg_t jse_msg;

    int32_t ret = be_osal_messageQ_get(jse_task_mq, &jse_msg, timeout);
    if (ret != 0) return -1;

    if (jse_msg.type == JSE_TASK_MSG_CALLBACK) {
        jse_msg.callback(jse_msg.param);
    }

    return 0;
}

void be_jse_timer_cb_handler(void *arg)
{
    jse_task_msg_t *p_jse_msg = (jse_task_msg_t *)be_osal_get_timer_param(arg);

    be_osal_lock_mutex(jse_task_mutex, JSE_TASK_MUTEX_WAITIME);
    be_osal_messageQ_put(jse_task_mq, p_jse_msg, JSE_MSGQ_WAITIME);
    be_osal_unlock_mutex(jse_task_mutex);
}

void *be_jse_task_timer_action(uint32_t ms, bone_engine_call_t action,
                               void *arg, jse_timer_type type)
{
    osTimerId timer_id = NULL;
    jse_task_msg_t *p_param =
        (jse_task_msg_t *)calloc(1, sizeof(jse_task_msg_t));

    if (!p_param) return NULL;

    p_param->callback = action;
    p_param->param    = arg;
    p_param->type     = JSE_TASK_MSG_CALLBACK;

    if (type == JSE_TIMER_REPEAT) {
        timer_id = be_osal_timer_create(be_jse_timer_cb_handler,
                                        TimerRunPeriodic, p_param);
    } else if (type == JSE_TIMER_ONCE) {
        timer_id = be_osal_timer_create(be_jse_timer_cb_handler, TimerRunOnce,
                                        p_param);
    } else {
        goto fail;
    }

    if (!timer_id) goto fail;

    int32_t ret = be_osal_timer_start(timer_id, ms);
    if (ret) {
        be_osal_timer_delete(timer_id);
        goto fail;
    }

    return timer_id;

fail:
    free(p_param);
    return NULL;
}

int32_t be_jse_task_cancel_timer(void *timerid)
{
    if (!timerid) return -1;

    return be_osal_timer_delete((osTimerId)timerid);
}

int32_t be_jse_task_schedule_call(bone_engine_call_t call, void *arg)
{
    jse_task_msg_t msg_buf;
    jse_task_msg_t *p_param = &msg_buf;

    if (!p_param) return -1;

    p_param->callback = call;
    p_param->param    = arg;
    p_param->type     = JSE_TASK_MSG_CALLBACK;
    if (jse_task_mq == NULL) {
        be_warn("jse_task", "jse_task_mq has not been initlized");
        return -1;
    }
    be_osal_lock_mutex(jse_task_mutex, JSE_TASK_MUTEX_WAITIME);
    be_osal_messageQ_put(jse_task_mq, p_param, JSE_MSGQ_WAITIME);
    be_osal_unlock_mutex(jse_task_mutex);
    return 0;
}

int32_t bone_engine_task_init()
{
    if (jse_task_mq == NULL) {
        jse_task_mq =
            be_osal_messageQ_create(JSE_MSGQ_MAX_NUM, sizeof(jse_task_msg_t));
        jse_task_mutex = be_osal_new_mutex();
    }
    return 0;
}
