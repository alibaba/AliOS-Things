/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdbool.h>
#include <stdlib.h>

#if defined(__ICCARM__) || defined(__CC_ARM)
#include <sys/errno.h>
#else
#include <errno.h>
#endif

#include "aos/kernel.h"
#include "aos/vfs.h"
#include "aos/yloop.h"

#include "event_device.h"
#include "yloop.h"
#include "k_config.h"

typedef struct {
    dlist_t       node;
    aos_event_cb  cb;
    void         *priv;
    uint16_t      type_filter;
} event_list_node_t;

static struct {
    void *handle;
    int   fd;
} local_event = {
    .fd = -1,
};

static dlist_t g_local_event_list = AOS_DLIST_INIT(g_local_event_list);

static void handle_events(input_event_t *event);
static int  input_add_event(int fd, input_event_t *event);
static void event_read_cb(int fd, void *param);

extern yloop_ctx_t    *g_main_ctx;
/* Handle events
 * just dispatch
 */
void handle_events(input_event_t *event)
{
    if (event->type == EV_RPC) {
        aos_call_t handler = (aos_call_t)event->value;
        void *arg = (void *)event->extra;
        handler(arg);

        return;
    }

    event_list_node_t *event_node = NULL;
    dlist_for_each_entry(&g_local_event_list, event_node, event_list_node_t, node) {
        if (event_node->type_filter != EV_ALL
            && event_node->type_filter != event->type) {
            continue;
        }
        (event_node->cb)(event, event_node->priv);
    }
}

static int input_add_event(int fd, input_event_t *event)
{
    bool urgent = event->type & EV_FLAG_URGENT;
    event->type &= ~EV_FLAG_URGENT;
    int cmd;

    if (urgent) {
        cmd = MK_CMD(IOCTL_WRITE_URGENT, sizeof(*event));
    } else {
        cmd = MK_CMD(IOCTL_WRITE_NORMAL, sizeof(*event));
    }

    return aos_ioctl(fd, cmd, (unsigned long)event);
}

void event_read_cb(int fd, void *param)
{
    input_event_t event;
    int ret = aos_read(fd, &event, sizeof(event));
    if (ret == sizeof(event)) {
        handle_events(&event);
    }
}

int aos_event_service_init(void)
{
#ifdef _WIN32
    int fd = aos_open("C:\\event.bin", 0);
#else
    int fd = aos_open("/dev/event", 0);
#endif

    if (local_event.fd < 0) {
        local_event.fd = fd;
    }
    aos_poll_read_fd(fd, event_read_cb, NULL);
    aos_loop_set_eventfd(fd);

    return 0;
}

void aos_event_service_deinit(int fd)
{
    aos_cancel_poll_read_fd(fd, event_read_cb, NULL);
    aos_close(fd);
}

int aos_post_event(uint16_t type, uint16_t code, unsigned long value)
{
    input_event_t event = {
        .type  = type,
        .code  = code,
        .value = value,
    };

    return input_add_event(local_event.fd, &event);
}

int aos_register_event_filter(uint16_t type, aos_event_cb cb, void *priv)
{
    if (cb == NULL) {
        return -EINVAL;
    }

    event_list_node_t *event_node = aos_malloc(sizeof(event_list_node_t));
    if (NULL == event_node) {
        return -ENOMEM;
    }

    event_node->cb           = cb;
    event_node->type_filter  = type;
    event_node->priv         = priv;

    dlist_add_tail(&event_node->node, &g_local_event_list);

    return 0;
}

int aos_unregister_event_filter(uint16_t type, aos_event_cb cb, void *priv)
{
    event_list_node_t *event_node = NULL;

    if (dlist_empty(&g_local_event_list)) {
        return -EINVAL;
    }

    dlist_for_each_entry(&g_local_event_list, event_node, event_list_node_t, node) {
        if (event_node->type_filter != type) {
            continue;
        }

        if (event_node->cb != cb) {
            continue;
        }

        if (event_node->priv != priv) {
            continue;
        }

        dlist_del(&event_node->node);
        aos_free(event_node);
        return 0;
    }

    return -EINVAL;
}

/*
 * schedule a callback in aos loop main thread
 */
static int _schedule_call(aos_loop_t *loop, aos_call_t fun, void *arg,
                          bool urgent)
{

    if (fun == NULL || g_main_ctx == NULL) {
        return -EINVAL;
    }

    input_event_t event = {
        .type = EV_RPC,
        .value = (unsigned long)fun,
        .extra = (unsigned long)arg,
    };
    int fd = aos_loop_get_eventfd(loop);
    if (fd < 0) {
        fd = local_event.fd;
    }

    if (urgent) {
        event.type |= EV_FLAG_URGENT;
    }
    return input_add_event(fd, &event);
}

int aos_loop_schedule_urgent_call(aos_loop_t *loop, aos_call_t fun, void *arg)
{
    return _schedule_call(loop, fun, arg, true);
}

int aos_loop_schedule_call(aos_loop_t *loop, aos_call_t fun, void *arg)
{
    return _schedule_call(loop, fun, arg, false);
}

int aos_schedule_call(aos_call_t fun, void *arg)
{
    return _schedule_call(NULL, fun, arg, false);
}
