/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>

//#include <aos/osal_debug.h>
#include <aos/kernel.h>

#include "internal.h"

#define TAG "uSrv"


void uservice_lock(uservice_t *srv)
{
    aos_assert(srv);
    if (srv->task)
        TASK_LOCK(srv->task);
}

void uservice_unlock(uservice_t *srv)
{
    aos_assert(srv);
    if (srv->task)
        TASK_UNLOCK(srv->task);
}

int uservice_call(uservice_t *srv, rpc_t *rpc)
{
    aos_assert(srv);
    aos_assert(srv->task);

    rpc->srv = srv;

#ifdef CONFIG_DEBUG
    if(rpc->data && rpc->data->timeout_ms != 0) {
        aos_assert(aos_task_self() != srv->task->task);
    }
#endif

    int count = 10;
    while (count--) {
        if (aos_queue_send(&srv->task->queue, rpc, sizeof(rpc_t)) == 0) {
            return rpc_wait(rpc);
        } else {
            if ( count == 1) {
                printf("uService %s queue full,send id:%d cur id: %d", srv->name,rpc->cmd_id, srv->task->current_rpc->cmd_id);
            }
            //if (!aos_irq_context())
                aos_msleep(100);
        }
    }

    return -1;
}

int uservice_call_sync(uservice_t *srv, int cmd, void *param, void *resp, size_t size)
{
    aos_assert(srv);

    rpc_t rpc;
    int ret;

    ret = rpc_init(&rpc, cmd, AOS_WAIT_FOREVER);

    if (ret < 0)
        return ret;

    if (param)
        rpc_put_point(&rpc, param);

    ret = uservice_call(srv, &rpc);

    if (resp != NULL && size > 0 && ret == 0) {
        int count;
        void *data = rpc_get_buffer(&rpc, &count);
        aos_assert(count == size && data != NULL);
        if (data)
            memcpy(resp, data, size);
    }

    rpc_deinit(&rpc);

    return ret;
}

int uservice_call_async(uservice_t *srv, int cmd, void *param, size_t param_size)
{
    aos_assert(srv);

    rpc_t rpc;
    int ret;

    ret = rpc_init(&rpc, cmd, 0);

    if (ret == 0) {
        if (param && param_size > 0)
            rpc_put_buffer(&rpc, param, param_size);

        ret = uservice_call(srv, &rpc);
        rpc_deinit(&rpc);
    }

    return ret;
}

uservice_t *uservice_new(const char *name, process_t process_rpc, void *context)
{
    aos_assert(process_rpc && name);
    uservice_t *srv = aos_zalloc(sizeof(uservice_t));

    if (srv == NULL)
        return NULL;

    srv->name        = name;
    srv->context     = context;
    srv->process_rpc = process_rpc;

    return srv;
}

void uservice_destroy(uservice_t *srv)
{
    aos_assert(srv);

    aos_free(srv);
}

int uservice_process(void *context, rpc_t *rpc, const rpc_process_t rpcs[])
{
    for (int i = 0; rpcs[i].process != NULL; i++) {
        if (rpcs[i].cmd_id == rpc->cmd_id) {
            if (rpcs[i].process(context, rpc) >= 0)
                rpc_reply(rpc);
            return 0;
        }
    }

    return -1;
}

static void uservice_event_process(uint32_t event_id, const void *data, void *context)
{
    uservice_t *srv = (uservice_t*)context;
    rpc_t rpc;

    aos_assert(srv);

    if (rpc_init(&rpc, event_id, 0) == 0) {
        rpc_put_point(&rpc, data);
        uservice_call(srv, &rpc);
        rpc_deinit(&rpc);
    }
}

void uservice_subscribe(uservice_t *srv, uint32_t event_id)
{
    aos_assert(srv);
    event_subscribe(event_id, uservice_event_process, srv);
}
