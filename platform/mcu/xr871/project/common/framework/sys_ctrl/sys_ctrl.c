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
#include "sys/list.h"
#include "kernel/os/os.h"
#include "observer.h"
#include "publisher.h"
#include "event_queue.h"
#include "sys_ctrl.h"


#define SYS_CTRL_DEBUG(fmt, arg...)		//printf("[SYS_CTRL debug] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define SYS_CTRL_ALERT(fmt, arg...)		printf("[SYS_CTRL alert] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define SYS_CTRL_ERROR(fmt, arg...)		printf("[SYS_CTRL error] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define SYS_CTRL_NOWAY()				printf("[SYS_CTRL should not be here] <%s : %d> \n", __func__, __LINE__)
#define SYS_CTRL_NOTSUPPORT() 			SYS_CTRL_ALERT("not support command")


static event_queue *g_sys_queue = NULL;
static publisher_base *g_sys_publisher = NULL;

static int compare(uint32_t newEvent, uint32_t obsEvent)
{
	if (!CMP_EVENT_TYPE(newEvent, obsEvent) && (EVENT_SUBTYPE(obsEvent) == ALL_SUBTYPE || newEvent == obsEvent))
		return 0;

	SYS_CTRL_DEBUG("cmp -1, newEvent: 0x%x, obsEvent: 0x%x", newEvent, obsEvent);

	return -1;
}

int sys_ctrl_create(void)
{
	uint32_t queue_len = PRJCONF_SYS_CTRL_QUEUE_LEN;

	if (g_sys_queue == NULL)
#if SYS_CTRL_PRIO_QUEUE
		g_sys_queue = prio_event_queue_create(queue_len);
#else
		g_sys_queue = normal_event_queue_create(queue_len);
#endif
	if (g_sys_queue == NULL)
	{
		SYS_CTRL_ERROR("create queue failed");
		return -1;
	}

	if (g_sys_publisher == NULL)
		g_sys_publisher = publisher_create(g_sys_queue, compare, PRJCONF_SYS_CTRL_PRIO, PRJCONF_SYS_CTRL_STACK_SIZE);
	if (g_sys_publisher == NULL)
	{
		SYS_CTRL_ERROR("create publisher failed");
		return -1;
	}

	return 0;
}

int sys_ctrl_attach(observer_base *obs)
{
	if (g_sys_publisher == NULL)
		return -1;

	return g_sys_publisher->attach(g_sys_publisher, obs);
}

int sys_ctrl_detach(observer_base *obs)
{
	if (g_sys_publisher == NULL)
		return -1;

	return g_sys_publisher->detach(g_sys_publisher, obs);
}

static __inline int event_send(event_queue *queue, uint16_t type, uint16_t subtype, uint32_t data, void (*destruct)(uint32_t data), uint32_t wait_ms)
{
	struct event_msg msg = {MK_EVENT(type, subtype), data, destruct};
	return queue->send(queue, &msg, wait_ms);
}

/*
static void freeMsgData(uint32_t data)
{
	free((void *)data);
}
*/

int sys_event_send(uint16_t type, uint16_t subtype, uint32_t data, uint32_t wait_ms)
{
	if (g_sys_queue == NULL)
		return -1;

	return event_send(g_sys_queue, type, subtype, data, NULL, wait_ms);
}

/*int sys_event_send_with_free(uint16_t type, uint16_t subtype, uint32_t data, uint32_t wait_ms)
{
	if (g_sys_queue == NULL)
		return -1;

	return event_send(g_sys_queue, type, subtype, data, freeMsgData, wait_ms);
}*/

int sys_event_send_with_destruct(uint16_t type, uint16_t subtype, uint32_t data, void (*destruct)(uint32_t data), uint32_t wait_ms)
{
	if (g_sys_queue == NULL)
		return -1;

	return event_send(g_sys_queue, type, subtype, data, destruct, wait_ms);
}
