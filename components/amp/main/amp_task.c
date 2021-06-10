/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "amp_platform.h"
#include "amp_defines.h"
#include "amp_config.h"
#include "aos_system.h"
#include "amp_task.h"
#include "amp_list.h"

#define MOD_STR                 "AMP_TASK"
#define AMP_MSGQ_WAITIME        (2000)
#define AMP_MSGQ_MAX_NUM        10

typedef struct {
    dlist_t node;
    void (*callback)(void);
} amp_source_node_t;

static dlist_t g_sources_list = AMP_DLIST_HEAD_INIT(g_sources_list);

static aos_queue_t amp_task_mq = NULL; /* JSEngine message queue */
static aos_mutex_t amp_task_mutex   = NULL;     /* JSEngine mutex */

void amp_module_free(void)
{
    amp_source_node_t *source_node;
    dlist_t *temp;

    dlist_for_each_entry_safe(&g_sources_list, temp, source_node, amp_source_node_t, node) {
        source_node->callback();
        dlist_del(&source_node->node);
        aos_free(source_node);
    }
}

int32_t amp_module_free_register(void (*callback)(void))
{
    amp_source_node_t *source_node;

    if (!callback) {
        return -1;
    }

    source_node = aos_malloc(sizeof(amp_source_node_t));
    if (!source_node) {
        return -1;
    }
    source_node->callback = callback;
    dlist_add_tail(&source_node->node, &g_sources_list);
    return 0;
}

int32_t amp_task_yield(uint32_t timeout)
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

    else if(amp_msg.type == AMP_TASK_MSG_EXIT) {
        return 1;
    }

    return 0;
}

static void amp_task_timer_cb_handler(void *timer, void *arg)
{
    amp_task_msg_t *p_amp_msg = (amp_task_msg_t *)arg;

    if (amp_task_mq == NULL) {
        return;
    }

    aos_queue_send(&amp_task_mq, p_amp_msg, sizeof(amp_task_msg_t));
}

aos_timer_t *amp_task_timer_action(uint32_t ms, amp_engine_call_t action, void *arg,
                            amp_timer_type_t type)
{
    int ret = -1;

    aos_timer_t *timer_id = (aos_timer_t *)aos_malloc(sizeof(aos_timer_t));
    if (timer_id == NULL) {
        return NULL;
    }

    amp_task_msg_t *p_param =
        (amp_task_msg_t *)aos_calloc(1, sizeof(amp_task_msg_t));

    if (!p_param) return NULL;

    if (amp_task_mq == NULL) {
        goto fail;
    }

    p_param->callback = action;
    p_param->param    = arg;
    p_param->type     = AMP_TASK_MSG_CALLBACK;

    if (type == AMP_TIMER_REPEAT) {
        ret = aos_timer_create(timer_id, amp_task_timer_cb_handler, p_param, ms, AOS_TIMER_REPEAT);
    } else if (type == AMP_TIMER_ONCE) {
        ret = aos_timer_create(timer_id, amp_task_timer_cb_handler, p_param, ms, 0);
    } else {
        goto fail;
    }

    if (ret != 0) goto fail;

    ret = aos_timer_start(timer_id);
    if (ret) {
        aos_timer_free(timer_id);
        goto fail;
    }

    return timer_id;

fail:
    aos_free(p_param);
    aos_free(timer_id);
    return NULL;
}

int32_t amp_task_schedule_call(amp_engine_call_t call, void *arg)
{
    amp_task_msg_t msg_buf;
    amp_task_msg_t *p_param = &msg_buf;

    if (amp_task_mq == NULL) {
        return -1;
    }

    p_param->callback = call;
    p_param->param    = arg;
    p_param->type     = AMP_TASK_MSG_CALLBACK;
    if (amp_task_mq == NULL) {
        amp_warn(MOD_STR, "amp_task_mq has not been initlized");
        return -1;
    }
    aos_queue_send(&amp_task_mq, p_param, sizeof(amp_task_msg_t));
    return 0;
}

int32_t amp_task_exit_call(amp_engine_call_t call, void *arg)
{
    amp_task_msg_t msg_buf;
    amp_task_msg_t *p_param = &msg_buf;

    memset(p_param, 0, sizeof(amp_task_msg_t));
    p_param->callback = call;
    p_param->param    = arg;
    p_param->type     = AMP_TASK_MSG_EXIT;
    if (amp_task_mq == NULL) {
        amp_warn(MOD_STR, "amp_task_mq has not been initlized");
        return -1;
    }
    aos_mutex_lock(&amp_task_mutex, AOS_WAIT_FOREVER);
    aos_queue_send(&amp_task_mq, p_param, sizeof(amp_task_msg_t));
    aos_mutex_unlock(&amp_task_mutex);
    return 0;
}

int32_t amp_task_init()
{
    if (amp_task_mq != NULL) {
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

    amp_debug(MOD_STR, "jsengine task init");
    return 0;
}

int32_t amp_task_deinit()
{
    if (amp_task_mq != NULL) {
        aos_queue_free(&amp_task_mq);
        amp_task_mq = NULL;
    }

    if (amp_task_mutex != NULL) {
        aos_mutex_free(&amp_task_mutex);
        amp_task_mutex = NULL;
    }

    /* free all jsengine heap */
    jsengine_exit();

    amp_debug(MOD_STR, "jsengine task free");
    return 0;
}


