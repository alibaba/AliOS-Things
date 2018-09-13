/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kernel/os/os.h"
#include "sys/list.h"
#include "sys/param.h"
#include "sys/defs.h"
#include "container.h"
#include "event_queue.h"

#define EVTMSG_DEBUG(fmt, arg...)	//printf("[event_msg debug] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define EVTMSG_ALERT(fmt, arg...)	printf("[event_msg alert] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define EVTMSG_ERROR(fmt, arg...)	printf("[event_msg error] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define EVTMSG_NOWAY()				printf("[event_msg should not be here] <%s : %d> \n", __func__, __LINE__)
#define EVTMSG_NOTSUPPORT() 		EVTMSG_ALERT("not support command")



typedef struct prio_event_queue
{
	event_queue base;
	container_base *container;
} prio_event_queue;

static int complare_event_msg(uint32_t newArg, uint32_t oldArg)
{
	event_msg *newMsg = (event_msg *)newArg;
	event_msg *oldMsg = (event_msg *)oldArg;

	return newMsg->event < oldMsg->event;
}

static int prio_event_queue_deinit(struct event_queue *base)
{
	prio_event_queue *impl = __containerof(base, prio_event_queue, base);

	int ret = impl->container->deinit(impl->container);
	if (ret == 0)
		free(impl);

	return ret;
}

static int prio_event_send(struct event_queue *base, struct event_msg *msg, uint32_t wait_ms)
{
	prio_event_queue *impl = __containerof(base, prio_event_queue, base);

	EVTMSG_DEBUG("send event: 0x%x", msg->event);

	/* TODO: cache some to faster create msg and less fragmented memory */
	struct event_msg *newMsg = malloc(sizeof(*newMsg));
	if (newMsg == NULL)
		return -1;
	memcpy(newMsg, msg, sizeof(*newMsg));

	int ret = impl->container->push(impl->container, (uint32_t)newMsg, wait_ms);
	if (ret != 0)
	{
		free(newMsg);
		EVTMSG_ALERT("send event timeout");
		return -2;
	}

	return 0;
}

static int prio_event_recv(struct event_queue *base, struct event_msg *msg, uint32_t wait_ms)
{
	prio_event_queue *impl = __containerof(base, prio_event_queue, base);
	event_msg *newMsg;

	int ret = impl->container->pop(impl->container, (uint32_t *)&newMsg, wait_ms);
	if (ret != 0)
		return -2;

	memcpy(msg, newMsg, sizeof(*newMsg));
	free(newMsg);

	EVTMSG_DEBUG("recv event: 0x%x", msg->event);

	return 0;
}

struct event_queue *prio_event_queue_create(uint32_t queue_len)
{
	prio_event_queue *impl = malloc(sizeof(*impl));
	if (impl == NULL)
		return NULL;
	memset(impl, 0, sizeof(*impl));

	impl->container = sorted_list_create(queue_len, complare_event_msg);
	if (impl->container == NULL)
		goto out;
	impl->base.send = prio_event_send;
	impl->base.recv = prio_event_recv;
	impl->base.deinit = prio_event_queue_deinit;

	return &impl->base;

out:
	EVTMSG_ERROR("sorted_list_create failed");
	free(impl);
	return NULL;
}


typedef struct normal_event_queue
{
	event_queue base;
	OS_Queue_t queue;
} normal_event_queue;

static int normal_event_queue_deinit(struct event_queue *base)
{
	normal_event_queue *impl = __containerof(base, normal_event_queue, base);
	OS_Status ret;

#if CTRL_MSG_VALIDITY_CHECK
	if (!OS_QueueIsValid(&impl->queue)) {
		EVTMSG_ALERT("%s(), invalid queue %p", __func__, g_ctrl_msg_queue.handle);
		return 0;
	}
#endif

	ret = OS_QueueDelete(&impl->queue);
	if (ret != OS_OK) {
		EVTMSG_ERROR("%s() failed, err 0x%x", __func__, ret);
		return -1;
	}

	free(impl);

	EVTMSG_DEBUG("%s()", __func__);
	return 0;
}

static int normal_event_send(struct event_queue *base, struct event_msg *msg, uint32_t wait_ms)
{
	normal_event_queue *impl = __containerof(base, normal_event_queue, base);
	OS_Status ret;

	EVTMSG_DEBUG("send event: 0x%x", msg->event);

#if CTRL_MSG_VALIDITY_CHECK
	if (!OS_QueueIsValid(&impl->queue)) {
		EVTMSG_ALERT("%s(), invalid queue %p", __func__, g_ctrl_msg_queue.handle);
		return -1;
	}
#endif

	ret = OS_QueueSend(&impl->queue, msg, wait_ms);
	if (ret != OS_OK) {
		EVTMSG_ERROR("%s() failed, err 0x%x", __func__, ret);
		return -1;
	}
	return 0;
}

static int normal_event_recv(struct event_queue *base, struct event_msg *msg, uint32_t wait_ms)
{
	normal_event_queue *impl = __containerof(base, normal_event_queue, base);
	OS_Status ret;

#if CTRL_MSG_VALIDITY_CHECK
	if (!OS_QueueIsValid(&g_ctrl_msg_queue)) {
		EVTMSG_ALERT("%s(), invalid queue %p", __func__, g_ctrl_msg_queue.handle);
		return -1;
	}
#endif

	ret = OS_QueueReceive(&impl->queue, msg, wait_ms);
	if (ret != OS_OK) {
		EVTMSG_ERROR("%s() failed, err 0x%x", __func__, ret);
		return -1;
	}

	EVTMSG_DEBUG("recv event: 0x%x", msg->event);

	return 0;
}

struct event_queue *normal_event_queue_create(uint32_t queue_len)
{
	normal_event_queue *impl = malloc(sizeof(*impl));
	if (impl == NULL)
		return NULL;
	memset(impl, 0, sizeof(*impl));

	impl->base.send = normal_event_send;
	impl->base.recv = normal_event_recv;
	impl->base.deinit = normal_event_queue_deinit;


#if CTRL_MSG_VALIDITY_CHECK
	if (OS_QueueIsValid(&impl->queue)) {
		EVTMSG_ALERT("control message queue already inited\n");
		return -1;
	}
#endif

	if (OS_QueueCreate(&impl->queue, queue_len, sizeof(struct event_msg)) != OS_OK) {
		EVTMSG_ERROR("%s() failed!", __func__);
		goto out;
	}
	EVTMSG_DEBUG("%s()", __func__);

	return &impl->base;

out:
	if (impl != NULL)
		free(impl);
	return NULL;
}


