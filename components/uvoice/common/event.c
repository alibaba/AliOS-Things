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


#ifdef MUSICBOX_APP
#define EVENT_TASK_STACK_SIZE	2048
#else
#define EVENT_TASK_STACK_SIZE	4096
#endif
#define EVENT_TASK_PRIORITY		UVOICE_TASK_PRI_LOWER

struct uvoice_event_node {
	uint16_t type_filter;
	uvoice_list_t list;
	uvoice_event_cb cb;
	void *data;
};

static uvoice_list_t uvoice_event_client_list =
	UVOICE_LIST_INIT(uvoice_event_client_list);
static os_task_t uvoice_event_task;

#ifdef UVOICE_EVENT_BY_BUFFER_QUEUE
static os_queue_t *uvoice_event_buffqueue;
#elif defined(UVOICE_EVENT_BY_MESSAGE_QUEUE)
#define UVOICE_EVENT_MESSAGE_TYPE	1001
static uvoice_msgqueue_t *uvoice_event_msgqueue;
#endif

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

#ifdef UVOICE_EVENT_BY_BUFFER_QUEUE
	while (1) {
        ret = os_queue_recv(uvoice_event_buffqueue,
			OS_WAIT_FOREVER, &event, &size);
		if (ret || size != sizeof(event)) {
			os_msleep(10);
			continue;
		}
		uvoice_event_handle(&event);
	}
#elif defined(UVOICE_EVENT_BY_MESSAGE_QUEUE)
	while (1) {
        ret = uvoice_msgqueue_recv(uvoice_event_msgqueue,
			UVOICE_EVENT_MESSAGE_TYPE,
			&event, sizeof(event), OS_WAIT_FOREVER);
		if (ret < 0 || ret != sizeof(event))
			continue;
		uvoice_event_handle(&event);
	}
#endif
}

int uvoice_event_init(void)
{
#ifdef UVOICE_EVENT_ENABLE
#ifdef UVOICE_EVENT_BY_BUFFER_QUEUE
	os_queue_t *queue;
	char *event_buffer;
	int ret;

	if (uvoice_event_buffqueue) {
		M_LOGW("uvoice event exist\n");
		return -1;
	}

	queue = snd_zalloc(sizeof(os_queue_t), AFM_EXTN);
	if (!queue) {
		M_LOGE("alloc queue failed !\n");
		return -1;
	}

	event_buffer = snd_zalloc(
		sizeof(uvoice_event_t) * 8, AFM_EXTN);
	if (!event_buffer) {
		M_LOGE("alloc msg buff failed !\n");
		snd_free(queue);
		return -1;
	}

	ret = os_queue_new(queue, event_buffer,
		sizeof(uvoice_event_t) * 8,
		sizeof(uvoice_event_t));
	if (ret) {
		M_LOGE("create event queue failed %d!\n", ret);
		snd_free(event_buffer);
		snd_free(queue);
		return -1;
	}

	uvoice_event_buffqueue = queue;

	ret = os_task_create(&uvoice_event_task,
		"uvoice_event_task",
		uvoice_event_loop,
		NULL,
		EVENT_TASK_STACK_SIZE,
		EVENT_TASK_PRIORITY);
	if (ret) {
		M_LOGE("create event task failed %d!\n", ret);
		uvoice_event_buffqueue = NULL;
		os_queue_free(queue);
		snd_free(event_buffer);
		snd_free(queue);
		return -1;
	}
#elif defined(UVOICE_EVENT_BY_MESSAGE_QUEUE)
	uvoice_msgqueue_t *msgqueue;
	int ret;

	if (uvoice_event_msgqueue) {
		M_LOGW("uvoice event exist\n");
		return -1;
	}

	msgqueue = uvoice_msgqueue_create(".", 101);
	if (!msgqueue) {
		M_LOGE("create msgqueue failed !\n");
		return -1;
	}
	uvoice_event_msgqueue = msgqueue;
	ret = os_task_create(&uvoice_event_task, "uvoice_event_task",
		uvoice_event_loop,
		NULL,
		EVENT_TASK_STACK_SIZE,
		EVENT_TASK_PRIORITY);
	if (ret) {
		M_LOGE("create event task failed %d!\n", ret);
		uvoice_msgqueue_release(msgqueue);
		uvoice_event_msgqueue = NULL;
		return -1;
	}
#endif
#endif

	return 0;
}

int uvoice_event_post(uint16_t type, uint16_t code, int value)
{
#ifdef UVOICE_EVENT_ENABLE

#ifdef UVOICE_EVENT_BY_BUFFER_QUEUE
	uvoice_event_t event = {
		.type  = type,
		.code  = code,
		.value = value,
	};
	int ret;

	if (!uvoice_event_buffqueue) {
		M_LOGD("no event buffqueue\n");
		return -1;
	}

	ret = os_queue_send(uvoice_event_buffqueue,
		&event, sizeof(uvoice_event_t));
	if (ret) {
		M_LOGE("send event failed %d!\n", ret);
		return -1;
	}
	return 0;
#elif defined(UVOICE_EVENT_BY_MESSAGE_QUEUE)
	uvoice_event_t event = {
		.msgtype = UVOICE_EVENT_MESSAGE_TYPE,
		.type  = type,
		.code  = code,
		.value = value,
	};
	int ret;

	if (!uvoice_event_msgqueue) {
		M_LOGD("no event msgqueue\n");
		return -1;
	}

	ret = uvoice_msgqueue_send(uvoice_event_msgqueue,
		&event, sizeof(uvoice_event_t), 0);
	if (ret < 0) {
		M_LOGE("send event failed %d!\n", ret);
		return -1;
	}
	return 0;
#endif

#else
	return os_event_post(type, code, value);
#endif
}

int uvoice_event_register(uint16_t type, uvoice_event_cb cb, void *data)
{
#ifdef UVOICE_EVENT_ENABLE

#if defined(UVOICE_EVENT_BY_BUFFER_QUEUE) || defined(UVOICE_EVENT_BY_MESSAGE_QUEUE)
	struct uvoice_event_node *node;

#ifdef UVOICE_EVENT_BY_BUFFER_QUEUE
	if (!uvoice_event_buffqueue) {
		M_LOGD("no event buffqueue\n");
		return -1;
	}
#elif defined(UVOICE_EVENT_BY_MESSAGE_QUEUE)
	if (!uvoice_event_msgqueue) {
		M_LOGD("no event msgqueue\n");
		return -1;
	}
#endif

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

#if defined(UVOICE_EVENT_BY_BUFFER_QUEUE) || defined(UVOICE_EVENT_BY_MESSAGE_QUEUE)
	struct uvoice_event_node *node = NULL;
	uvoice_list_t *temp = NULL;
	int ret = -1;

	uvoice_list_for_each_entry_safe(&uvoice_event_client_list,
		temp, node, struct uvoice_event_node, list) {
		if (node) {
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
	}

	return ret;
#endif

#else
	return os_event_unregister(type, cb, data);
#endif
}

