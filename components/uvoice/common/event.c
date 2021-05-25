/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "uvoice_os.h"
#include "uvoice_types.h"
#include "uvoice_event.h"
#include "uvoice_list.h"

#include "uvoice_message.h"

// #define UVOICE_EVENT_TASK_ENABLE

#ifdef MUSICBOX_APP
#define EVENT_TASK_STACK_SIZE    2048
#else
#define EVENT_TASK_STACK_SIZE    8192
#endif
#define EVENT_TASK_PRIORITY        UVOICE_TASK_PRI_LOWEST

#define EVENT_MESSAGE_QUEUE_MAX    10


struct uvoice_event_node {
    uint16_t type_filter;
    uvoice_list_t list;
    uvoice_event_cb cb;
    void *data;
};

static uvoice_list_t uvoice_event_client_list =
    UVOICE_LIST_INIT(uvoice_event_client_list);
static os_task_t uvoice_event_task;

static os_message_queue_t uvoice_event_msgqueue;

static void uvoice_event_handle(uvoice_event_t *event)
{
    struct uvoice_event_node *node = NULL;
    uvoice_list_for_each_entry(&uvoice_event_client_list,
        node, struct uvoice_event_node, list) {
        if (node->type_filter != event->type)
            continue;
        node->cb(event, node->data);
    }
}

static void uvoice_event_loop(void *arg)
{
    uvoice_event_t event;
    unsigned int size = 0;
    int ret;

    while (1) {
        ret = os_message_queue_recv(uvoice_event_msgqueue,
            &event, sizeof(event), OS_WAIT_FOREVER);
        if (ret < 0)
            continue;
        if (event.type == 0xffff && event.code == 0xffff && event.value == 0xccccffff) {
            M_LOGW("uvoice event loop exit");
            break;
        }
        uvoice_event_handle(&event);
    }

    os_task_exit(uvoice_event_task);
}

int uvoice_event_init(void)
{
#ifdef UVOICE_EVENT_ENABLE

#ifdef UVOICE_EVENT_TASK_ENABLE
    uvoice_msgqueue_t *msgqueue;
    int ret;

    if (uvoice_event_msgqueue) {
        M_LOGW("uvoice event exist\n");
        return -1;
    }

    uvoice_event_msgqueue = os_message_queue_create(EVENT_MESSAGE_QUEUE_MAX,
        sizeof(uvoice_event_t));
    if (!uvoice_event_msgqueue) {
        M_LOGE("create msgqueue failed !\n");
        return -1;
    }

    ret = os_task_create(&uvoice_event_task, "uvoice_event_task",
        uvoice_event_loop,
        NULL,
        EVENT_TASK_STACK_SIZE,
        EVENT_TASK_PRIORITY);
    if (ret) {
        M_LOGE("create event task failed %d!\n", ret);
        os_message_queue_free(uvoice_event_msgqueue);
        uvoice_event_msgqueue = NULL;
        return -1;
    }
#endif

#endif

    return 0;
}

int uvoice_event_deinit(void)
{
#ifdef UVOICE_EVENT_ENABLE

    struct uvoice_event_node *node = NULL;
    uvoice_list_t *temp = NULL;

#ifdef UVOICE_EVENT_TASK_ENABLE

    uvoice_event_post(0xffff, 0xffff, 0xccccffff);
    os_msleep(10);
    if (uvoice_event_msgqueue) {
        os_message_queue_free(uvoice_event_msgqueue);
        uvoice_event_msgqueue = NULL;
    }
#endif

    uvoice_list_for_each_entry_safe(&uvoice_event_client_list,
        temp, node, struct uvoice_event_node, list) {
        uvoice_list_del(&node->list);
        snd_free(node);
    }

#endif

    return 0;
}

int uvoice_event_post(uint16_t type, uint16_t code, int value)
{
#ifdef UVOICE_EVENT_ENABLE

    uvoice_event_t event = {
        .type  = type,
        .code  = code,
        .value = value,
    };

#ifdef UVOICE_EVENT_TASK_ENABLE
    int ret;

    if (!uvoice_event_msgqueue) {
        M_LOGD("no event msgqueue\n");
        return -1;
    }

    ret = os_message_queue_send(uvoice_event_msgqueue,
        &event, sizeof(uvoice_event_t), 0);
    if (ret < 0) {
        M_LOGE("send event failed %d!\n", ret);
        return -1;
    }
#else
    struct uvoice_event_node *node = NULL;
    uvoice_list_for_each_entry(&uvoice_event_client_list,
        node, struct uvoice_event_node, list) {
        if (node->type_filter != type)
            continue;
        node->cb(&event, node->data);
    }
#endif

    return 0;

#else
    return os_event_post(type, code, value);
#endif
}

int uvoice_event_register(uint16_t type, uvoice_event_cb cb, void *data)
{
#ifdef UVOICE_EVENT_ENABLE

    struct uvoice_event_node *node;

#ifdef UVOICE_EVENT_TASK_ENABLE
    if (!uvoice_event_msgqueue) {
        M_LOGD("no event msgqueue\n");
        return -1;
    }
#endif

    if (!cb) {
        M_LOGE("callback function null !\n");
        return -1;
    }

    node = snd_zalloc(sizeof(struct uvoice_event_node), AFM_EXTN);
    if (!node) {
        M_LOGE("alloc event node failed !\n");
        return -1;
    }

    node->cb = cb;
    node->type_filter = type;
    node->data = data;

    uvoice_list_add_tail(&node->list, &uvoice_event_client_list);
    return 0;
#else
    return os_event_register(type, cb, data);
#endif
}

int uvoice_event_unregister(uint16_t type, uvoice_event_cb cb,
    void *data)
{
#ifdef UVOICE_EVENT_ENABLE

    struct uvoice_event_node *node = NULL;
    uvoice_list_t *temp = NULL;
    int ret = -1;

    uvoice_list_for_each_entry_safe(&uvoice_event_client_list,
        temp, node, struct uvoice_event_node, list) {
        if (node->type_filter != type)
            continue;

        if (node->cb != cb)
            continue;

        if (node->data != data)
            continue;

        uvoice_list_del(&node->list);
        snd_free(node);
        ret = 0;
        break;
    }

    return ret;

#else
    return os_event_unregister(type, cb, data);
#endif
}

