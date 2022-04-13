/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef USERVICE_INTERNAL_H
#define USERVICE_INTERNAL_H

#include <stdint.h>
#include <aos/list.h>
#include <aos/kernel.h>
#include <uservice/uservice.h>

#ifdef __cplusplus
extern "C" {
#endif

struct uservice {
    const char *name;
    void       *context;
    process_t   process_rpc;

    utask_t    *task;
    slist_t     next;
};

struct rpc_record {
    uint32_t cmd_id;
    uint32_t count;
    struct uservice *srv;
    slist_t next;
};

struct utask {
    aos_task_t  task;
    int8_t      running;

    uint8_t     queue_count;
#if defined(CONFIG_DEBUG) && defined(CONFIG_DEBUG_UTASK)
    uint8_t     queue_max_used;
    slist_t     rpc_reclist;
#endif
    void       *queue_buffer;
    aos_queue_t queue;
    aos_mutex_t mutex;
    rpc_t      *current_rpc;
    aos_sem_t   running_wait;

    slist_t     uservice_lists;
    slist_t     rpc_buffer_gc_cache;
    slist_t     node;
};

struct rpc_buffer {
    int       timeout_ms;
    uint8_t * buffer;
    uint16_t  buf_size;
    uint16_t  pos;
    aos_sem_t sem;

    slist_t  next;
};

#define TASK_LOCK(task) aos_mutex_lock(&(task->mutex), AOS_WAIT_FOREVER)
#define TASK_UNLOCK(task) aos_mutex_unlock(&(task->mutex))

void rpc_free(rpc_t *rpc);
int  rpc_wait(rpc_t *rpc);

typedef struct event_list {
    slist_t     events;
    aos_mutex_t mutex;
} event_list_t;

int  eventlist_init(event_list_t *evlist);
void eventlist_uninit(event_list_t *evlist);

int  eventlist_subscribe(event_list_t *evlist, uint32_t event_id, event_callback_t cb, void *context);
int  eventlist_unsubscribe(event_list_t *evlist, uint32_t event_id, event_callback_t cb, void *context);
int  eventlist_publish(event_list_t *evlist, uint32_t event_id, void *data);
int  eventlist_remove(event_list_t *evlist, uint32_t event_id);

int  eventlist_subscribe_fd(event_list_t *evlist, uint32_t fd, event_callback_t cb, void *context);
int  eventlist_unsubscribe_fd(event_list_t *evlist, uint32_t fd, event_callback_t cb, void *context);
int  eventlist_publish_fd(event_list_t *evlist, uint32_t fd, void *data);
int  eventlist_remove_fd(event_list_t *evlist, uint32_t fd);

int  eventlist_setfd(event_list_t *evlist, void *readfds);

#ifdef __cplusplus
}
#endif

#endif
