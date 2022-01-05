/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "amp_task.h"

#include "amp_list.h"
#include "amp_platform.h"
#include "aos_system.h"

#define MOD_STR "PY_TASK"
#define AMP_MSGQ_WAITIME (2000)
#define AMP_MSGQ_MAX_NUM 10

typedef struct {
    dlist_t node;
    void (*callback)(void);
} amp_source_node_t;

static dlist_t g_sources_list = AMP_DLIST_HEAD_INIT(g_sources_list);

static unsigned char g_init = 0;
static aos_queue_t amp_task_mq = { NULL }; /* JSEngine message queue */
static aos_mutex_t amp_task_mutex = { NULL }; /* JSEngine mutex */

static void queue_deinit(aos_queue_t *queue)
{
    aos_queue_free(queue);
}

static void mutex_deinit(aos_mutex_t *mutex)
{
    aos_mutex_free(mutex);
}

int32_t py_task_yield(uint32_t timeout)
{
    int32_t ret = 0;
    size_t revsize;

    amp_task_msg_t amp_msg;

    memset(&amp_msg, 0, sizeof(amp_msg));
    amp_msg.type = AMP_TASK_MSG_TYPE_MAX;

    if ((ret = aos_queue_recv(&amp_task_mq, timeout, &amp_msg, &revsize)) != 0) {
        return -1;
    }

    if (amp_msg.type == AMP_TASK_MSG_CALLBACK) {
        amp_msg.callback(amp_msg.param);
    }

    else if (amp_msg.type == AMP_TASK_MSG_EXIT) {
        return 1;
    }

    return 0;
}

int32_t py_task_schedule_call(amp_engine_call_t call, void *arg)
{
    amp_task_msg_t msg_buf;
    amp_task_msg_t *p_param = &msg_buf;

    p_param->callback = call;
    p_param->param = arg;
    p_param->type = AMP_TASK_MSG_CALLBACK;

    if (!g_init) {
        amp_warn(MOD_STR, "amp_task_mq has not been initlized");
        return -1;
    }

    aos_queue_send(&amp_task_mq, p_param, sizeof(amp_task_msg_t));

    return 0;
}

int32_t py_task_exit_call(amp_engine_call_t call, void *arg)
{
    amp_task_msg_t msg_buf;
    amp_task_msg_t *p_param = &msg_buf;

    memset(p_param, 0, sizeof(amp_task_msg_t));
    p_param->callback = call;
    p_param->param = arg;
    p_param->type = AMP_TASK_MSG_EXIT;

    if (!g_init) {
        amp_warn(MOD_STR, "amp_task_mq has not been initlized");
        return -1;
    }

    aos_mutex_lock(&amp_task_mutex, AOS_WAIT_FOREVER);
    aos_queue_send(&amp_task_mq, p_param, sizeof(amp_task_msg_t));
    aos_mutex_unlock(&amp_task_mutex);
    return 0;
}

int32_t py_task_init()
{
    if (g_init) {
        return 0;
    }

    if (aos_queue_new(&amp_task_mq, NULL, AMP_MSGQ_MAX_NUM * sizeof(amp_task_msg_t), sizeof(amp_task_msg_t)) != 0) {
        amp_error(MOD_STR, "create messageQ error");
        return -1;
    }
    if (aos_mutex_new(&amp_task_mutex) != 0) {
        amp_error(MOD_STR, "create mutex error");
        return -1;
    }

    g_init = 1;

    amp_debug(MOD_STR, "pyengine task init \r\n");
    return 0;
}

int32_t py_task_deinit()
{
    if (g_init) {
        queue_deinit(&amp_task_mq);
        mutex_deinit(&amp_task_mutex);
    }

    g_init = 0;

    amp_debug(MOD_STR, "pyengine task free");
    return 0;
}
