/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <aos/kernel.h>
#include <uservice/event.h>
#include <sys/select.h>

#include "internal.h"

#define FD_MASK (1UL << 31)

typedef struct event {
    uint32_t event_id;
    slist_t  sub_list;

    slist_t next;
} event_t;

typedef struct event_subscription {
    event_callback_t ecb;
    void *context;

    slist_t next;
} event_subscription_t;

static event_t *find_event(event_list_t *evlist, uint32_t event_id)
{
    event_t *node;
    slist_for_each_entry(&evlist->events, node, event_t, next) {
        if (node->event_id == event_id) {
            return node;
        }
    }

    return NULL;
}

static event_subscription_t *find_event_sub(event_t *ev, event_callback_t cb, void *context)
{
    event_subscription_t *node;
    slist_for_each_entry(&ev->sub_list, node, event_subscription_t, next) {
        if (node->ecb == cb && node->context == context) {
            return node;
        }
    }

    return NULL;
}

int eventlist_init(event_list_t *evlist)
{
    aos_assert(evlist);

    memset(evlist, 0, sizeof(event_list_t));
    slist_init(&evlist->events);

    if (aos_mutex_new(&evlist->mutex) != 0) {
        return -1;
    }

    return 0;
}

void eventlist_uninit(event_list_t *evlist)
{
    aos_assert(evlist);
    event_t *node;
    slist_t *tmp_1;

    aos_mutex_lock(&evlist->mutex, AOS_WAIT_FOREVER);

    slist_for_each_entry_safe(&evlist->events, tmp_1, node, event_t, next) {
        event_subscription_t *node_sub;
        slist_t *tmp_2;
        slist_for_each_entry_safe(&node->sub_list, tmp_2, node_sub, event_subscription_t, next) {
            slist_del(&node_sub->next, &node->sub_list);
            aos_free(node_sub);
        }
        slist_del(&node->next, &evlist->events);
        aos_free(node);
    }

    aos_mutex_unlock(&evlist->mutex);

    aos_mutex_free(&evlist->mutex);
}

static int __yoc_event_subscribe(event_list_t *evlist, uint32_t event_id, event_callback_t cb, void *context)
{
    int ret = -1;

    aos_mutex_lock(&evlist->mutex, AOS_WAIT_FOREVER);
    event_t *ev = find_event(evlist, event_id);

    if (ev == NULL) {
        ev = (event_t *)aos_zalloc(sizeof(event_t));

        if (ev != NULL) {
            ev->event_id = event_id;
            slist_init(&ev->sub_list);
            slist_add(&ev->next, &evlist->events);
        }
    }

    if (ev != NULL) {
        event_subscription_t *e_sub;
        e_sub = find_event_sub(ev, cb, context);
        if (e_sub == NULL) {
            e_sub = (event_subscription_t *)aos_zalloc(sizeof(event_subscription_t));

            if (e_sub) {
                e_sub->ecb = cb;
                e_sub->context = context;
                slist_add(&e_sub->next, &ev->sub_list);

                ret = 0;
            }
        }
    }

    aos_mutex_unlock(&evlist->mutex);

    return ret;
}

int eventlist_subscribe_fd(event_list_t *evlist, uint32_t fd, event_callback_t cb, void *context)
{
    aos_assert(evlist && cb);
    if (fd & FD_MASK) {
        return -1;
    }

    fd |= FD_MASK;

    return __yoc_event_subscribe(evlist, fd, cb, context);
}

int eventlist_subscribe(event_list_t *evlist, uint32_t event_id, event_callback_t cb, void *context)
{
    aos_assert(evlist && cb);
    if (event_id & FD_MASK) {
        return -1;
    }

    return __yoc_event_subscribe(evlist, event_id, cb, context);
}

static int __yoc_event_unsubscribe(event_list_t *evlist, uint32_t event_id, event_callback_t cb, void *context)
{
    int ret = -1;

    aos_mutex_lock(&evlist->mutex, AOS_WAIT_FOREVER);

    event_t *ev = find_event(evlist, event_id);

    if (ev) {
        event_subscription_t *e_sub = find_event_sub(ev, cb, context);

        if (e_sub) {
            slist_del(&e_sub->next, &ev->sub_list);
            aos_free(e_sub);

            if (slist_empty(&ev->sub_list)) {
                slist_del(&ev->next, &evlist->events);
                aos_free(ev);
            }

            ret = 0;
        }
    }

    aos_mutex_unlock(&evlist->mutex);

    return ret;
}


static int __yoc_event_remove(event_list_t *evlist, uint32_t event_id)
{
    int ret = -1;

    aos_mutex_lock(&evlist->mutex, AOS_WAIT_FOREVER);

    event_t *ev = find_event(evlist, event_id);

    if (ev) {
        event_subscription_t *node;
        slist_t               *tmp;
        slist_for_each_entry_safe(&ev->sub_list, tmp, node, event_subscription_t, next) {
            slist_del(&ev->next, &evlist->events);
            aos_free(ev);
        }
        slist_del(&ev->next, &evlist->events);
        aos_free(ev);
    }

    aos_mutex_unlock(&evlist->mutex);

    return ret;
}

int eventlist_unsubscribe_fd(event_list_t *evlist, uint32_t fd, event_callback_t cb, void *context)
{
    aos_assert(evlist && cb);
    if (fd & FD_MASK) {
        return -1;
    }

    fd |= FD_MASK;

    return __yoc_event_unsubscribe(evlist, fd, cb, context);
}

int eventlist_unsubscribe(event_list_t *evlist, uint32_t event_id, event_callback_t cb, void *context)
{
    aos_assert(evlist && cb);
    if (event_id & FD_MASK) {
        return -1;
    }

    return __yoc_event_unsubscribe(evlist, event_id, cb, context);
}

static void __event_publish(event_list_t *evlist, uint32_t event_id, void *data)
{
    aos_mutex_lock(&evlist->mutex, AOS_WAIT_FOREVER);

    event_t *ev = find_event(evlist, event_id);

    if (ev) {
        event_subscription_t *node;
        slist_for_each_entry(&ev->sub_list, node, event_subscription_t, next) {
            if (node->ecb) {
                node->ecb(ev->event_id, data, node->context);
            }
        }
    }

    aos_mutex_unlock(&evlist->mutex);
}

int eventlist_publish(event_list_t *evlist, uint32_t event_id, void *data)
{
    aos_assert(evlist);
    if (event_id & FD_MASK) {
        return -1;
    }

    __event_publish(evlist, event_id, data);
    return 0;
}

int eventlist_publish_fd(event_list_t *evlist, uint32_t fd, void *data)
{
    aos_assert(evlist);
    if (fd & FD_MASK) {
        return -1;
    }

    fd |= FD_MASK;
    __event_publish(evlist, fd, data);

    return 0;
}

int eventlist_remove(event_list_t *evlist, uint32_t event_id)
{
    aos_assert(evlist);
    if (event_id & FD_MASK) {
        return -1;
    }
    __yoc_event_remove(evlist, event_id);

    return 0;
}

int  eventlist_remove_fd(event_list_t *evlist, uint32_t fd)
{
    aos_assert(evlist);
    if (fd & FD_MASK) {
        return -1;
    }

    fd |= FD_MASK;
    __yoc_event_remove(evlist, fd);

    return 0;
}

int eventlist_setfd(event_list_t *evlist, void *data)
{
    aos_assert(evlist && data);
    int max_fd = 0;

    fd_set *readfds = (fd_set *)data;

    FD_ZERO(readfds);

    aos_mutex_lock(&evlist->mutex, AOS_WAIT_FOREVER);
    event_t *node;
    slist_for_each_entry(&evlist->events, node, event_t, next) {
        if (node->event_id > FD_MASK) {
            uint32_t fd = node->event_id & (~FD_MASK);
            FD_SET(fd, readfds);

            if (fd > max_fd) {
                max_fd = fd;
            }
        }
    }
    aos_mutex_unlock(&evlist->mutex);

    return max_fd;
}
