/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//#include <aos/osal_debug.h>
#include <aos/list.h>
#include <uservice/uservice.h>
#include <uservice/event.h>
#include <sys/socket.h>

#include "internal.h"

#define FD_MAX_STEMP 8

static struct event_call {
    uservice_t  *svr;
    event_list_t event;
    dlist_t      timeouts;
    int          event_id;
    void        *data;
    aos_task_t   select_task;
    aos_sem_t    select_sem;
    aos_event_t  wait_event;
} ev_service;

struct event_param {
    uint32_t event_id;
    union {
        event_callback_t cb;
        uint32_t         timeout;
    };
    void   *data;
    dlist_t next;
};

enum {
    CMD_SUB_FD_EVENT,
    CMD_SUB_EVENT,
    CMD_REMOVE_FD_EVENT,
    CMD_REMOVE_EVENT,
    CMD_PUBLISH_EVENT,
    CMD_PUBLISH_FD_EVENT,
};

#define EVENT_SUBSCRIBE 0x0000FF00
#define EVENT_UNSUBSCRIBE 0x000000FF

static void select_task_entry(void *arg);

static int process_rpc(void *context, rpc_t *rpc)
{
    int                 size = -1;
    struct event_param *param = (struct event_param *)rpc_get_buffer(rpc, &size);

    aos_assert(size == sizeof(struct event_param));

    switch (rpc->cmd_id) {
    case CMD_SUB_FD_EVENT:
        eventlist_subscribe_fd(&ev_service.event, param->event_id, param->cb, param->data);
        aos_sem_signal(&ev_service.select_sem);
        break;

    case CMD_REMOVE_FD_EVENT:
        eventlist_unsubscribe_fd(&ev_service.event, param->event_id, param->cb, param->data);
        break;

    case CMD_PUBLISH_FD_EVENT:
        eventlist_publish_fd(&ev_service.event, param->event_id, param->data);
        break;

    case CMD_SUB_EVENT:
        eventlist_subscribe(&ev_service.event, param->event_id, param->cb, param->data);
        break;

    case CMD_REMOVE_EVENT:
        eventlist_unsubscribe(&ev_service.event, param->event_id, param->cb, param->data);
        break;

    case CMD_PUBLISH_EVENT:
        if (param->timeout > 0) {
            struct event_param *timer = aos_malloc(sizeof(struct event_param));
            if (timer == NULL)
                break;

            timer->timeout = aos_now_ms() + param->timeout;
            timer->event_id = param->event_id;
            timer->data = param->data;

            struct event_param *node;
            dlist_for_each_entry(&ev_service.timeouts, node, struct event_param, next) {
                if (timer->timeout < node->timeout)
                    break;
            }
            dlist_add_tail(&timer->next, &node->next);
            aos_sem_signal(&ev_service.select_sem);
        } else {
            eventlist_publish(&ev_service.event, param->event_id, param->data);
        }

        break;
    }

    rpc_reply(rpc);

    return 0;
}

int event_service_init(utask_t *task)
{
    if (task == NULL)
        task = utask_new("event_svr", 2*1024, QUEUE_MSG_COUNT * 5, AOS_DEFAULT_APP_PRI);

    if (task == NULL)
        return -1;

    eventlist_init(&ev_service.event);
    dlist_init(&ev_service.timeouts);
    if (aos_sem_new(&ev_service.select_sem, 0) < 0) {
        utask_destroy(task);
        return -1;
    }
    if (aos_event_new(&ev_service.wait_event, 0) < 0) {
        utask_destroy(task);
        aos_sem_free(&ev_service.select_sem);
        return -1;
    }

    ev_service.svr = uservice_new("event_svr", process_rpc, NULL);
    if (ev_service.svr == NULL) {
        utask_destroy(task);
        aos_sem_free(&ev_service.select_sem);
        aos_event_free(&ev_service.select_sem);
        return -1;
    }

    aos_task_new_ext(&ev_service.select_task, "select", select_task_entry, NULL,
                     1024, AOS_DEFAULT_APP_PRI);
    utask_add(task, ev_service.svr);

    return 0;
}

static void event_call(struct event_param *param, int cmd_id, int sync)
{
    rpc_t rpc;

    if (rpc_init(&rpc, cmd_id, sync ? AOS_WAIT_FOREVER : 0) == 0) {
        rpc_put_buffer(&rpc, param, sizeof(struct event_param ));
        uservice_call(ev_service.svr, &rpc);
        rpc_deinit(&rpc);
    }
}

void event_publish_fd(uint32_t event_id, void *data, int sync)
{
    struct event_param param;

    param.event_id = event_id;
    param.data     = data;
    param.cb       = NULL;

    event_call(&param, CMD_PUBLISH_FD_EVENT, sync);
}

void event_subscribe_fd(uint32_t fd, event_callback_t cb, void *context)
{
    aos_assert(cb);
    struct event_param param;
    param.event_id = fd;
    param.cb       = cb;
    param.data     = context;

    event_call(&param, CMD_SUB_FD_EVENT, 0);
}

void event_unsubscribe_fd(uint32_t event_id, event_callback_t cb, void *context)
{
    struct event_param param;
    param.event_id = event_id;
    param.cb       = cb;
    param.data     = context;

    event_call(&param, CMD_REMOVE_FD_EVENT, 0);
}

void event_publish(uint32_t event_id, void *data)
{
    struct event_param param;

    param.event_id = event_id;
    param.data     = data;
    param.timeout  = 0;

    event_call(&param, CMD_PUBLISH_EVENT, 0);
}

void event_publish_delay(uint32_t event_id, void *data, int timeout)
{
    struct event_param param;
    param.event_id = event_id;
    param.data     = data;
    param.timeout  = timeout;

    event_call(&param, CMD_PUBLISH_EVENT, 0);
}

void event_subscribe(uint32_t event_id, event_callback_t cb, void *context)
{
    aos_assert(cb);
    struct event_param param;
    param.event_id = event_id;
    param.cb       = cb;
    param.data     = context;

    event_call(&param, CMD_SUB_EVENT, 0);
}

void event_unsubscribe(uint32_t event_id, event_callback_t cb, void *context)
{
    aos_assert(cb);
    struct event_param param;

    param.event_id = event_id;
    param.cb       = cb;
    param.data     = context;

    event_call(&param, CMD_REMOVE_EVENT, 0);
}

static int do_time_event()
{
    int delayed_ms = -1;

    struct event_param *node;
    dlist_t            *tmp;

    uservice_lock(ev_service.svr);
    dlist_for_each_entry_safe(&ev_service.timeouts, tmp, node, struct event_param, next) {
        long long now = aos_now_ms();

        if (now >= node->timeout) {
            event_publish(node->event_id, node->data);
            dlist_del(&node->next);
            aos_free(node);
        } else {
            delayed_ms = node->timeout - now;
            break;
        }
    }
    uservice_unlock(ev_service.svr);

    return delayed_ms;
}

#define SELECT_TIMEOUT (10)
static void select_task_entry(void *arg)
{
#ifdef CONFIG_LWIP_V200
    aos_task_exit(0);
#else
    event_list_t *evlist = &ev_service.event;
    utask_t *task = ev_service.svr->task;

#if defined(CONFIG_SAL) || defined(CONFIG_TCPIP)
    extern void sys_init(void);
    sys_init();
#endif

    while (1) {
        fd_set         readfds;
        struct timeval timeout;

        int time_ms = do_time_event();

        int max_fd = eventlist_setfd(evlist, &readfds);

        timeout.tv_sec   = time_ms / 1000;
        timeout.tv_usec  = (time_ms % 1000) * 1000;
        int ret = select2(max_fd + 1, &readfds, NULL, NULL, time_ms == -1 ? NULL : &timeout, &ev_service.select_sem);
        if (ret > 0) {
            for (int fd = 0; fd <= max_fd; fd++) {
                if (FD_ISSET(fd, &readfds)) {
                    if(aos_queue_get_count(&task->queue) < (task->queue_count*3/4)) {
                        event_publish_fd(fd, NULL, 1);
                        // eventlist_remove_fd(evlist, fd);
                    }
                }
            }
        }
    }
    aos_task_exit(0);
#endif
}
