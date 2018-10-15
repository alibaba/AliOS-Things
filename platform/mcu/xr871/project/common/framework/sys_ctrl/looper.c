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
#include "event_queue.h"
#include "looper.h"

#define LOOPER_DEBUG(fmt, arg...)	//printf("[Looper debug] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define LOOPER_ALERT(fmt, arg...)	printf("[Looper alert] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define LOOPER_ERROR(fmt, arg...)	printf("[Looper error] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define LOOPER_NOWAY()				printf("[Looper should not be here] <%s : %d> \n", __func__, __LINE__)
#define LOOPER_NOTSUPPORT() 		LOOPER_ALERT("not support command")

struct looper_base
{
	struct event_queue *queue;
	looper_extend extend;
	OS_Thread_t thd;
	void *arg;
	void *msg;
};

static void main_looper(void *arg)
{
	looper_base *base = (looper_base *)arg;
	looper_extend extend;
	struct event_msg *msg = base->msg;
	int ret;
	uint32_t t0, t1;

	while (1)
	{
		ret = base->queue->recv(base->queue, msg, -1);
		if (ret != 0) {
			LOOPER_ERROR("queue error");
			continue;
		}

		if (msg->execute)
		{
			t0 = OS_TicksToMSecs(OS_GetTicks());
			msg->execute(msg);
			t1 = OS_TicksToMSecs(OS_GetTicks());
#define HANDLER_OVERTIME 1000
			if ((t1 - t0 > HANDLER_OVERTIME) && (OS_TimeAfter(t1, t0)))
				LOOPER_ALERT("handler: %p run %d ms", msg->execute, t1 - t0);
		}

		extend = base->extend;
		while (extend != NULL)
		{
			t0 = OS_TicksToMSecs(OS_GetTicks());
			extend = (looper_extend)base->extend(msg, base->arg);
			t1 = OS_TicksToMSecs(OS_GetTicks());

#define LOOPER_EXTEND_OVERTIME 5000
			if ((t1 - t0 > LOOPER_EXTEND_OVERTIME) && (OS_TimeAfter(t1, t0)))
				LOOPER_ALERT("looper extend function run %d ms", t1 - t0);
		}

		if (msg->destruct != NULL)
			msg->destruct(msg);
	}
}

looper_base *looper_create(struct event_queue *queue)
{
	looper_base *base = malloc(sizeof(*base));
	if (base == NULL)
		return NULL;
	memset(base, 0, sizeof(*base));

	base->queue = queue;
	base->extend = NULL;
	base->arg = NULL;
	base->msg = malloc(sizeof(struct event_msg));

	if (OS_ThreadCreate(&base->thd, "Looper", main_looper,
						base, OS_PRIORITY_NORMAL, 2 * 1024) != OS_OK)
	{
		LOOPER_ERROR("thread create error, maybe no RAM to create");
		goto failed;
	}
	return base;

failed:
	if (base != NULL)
		free(base);
	return NULL;
}

struct looper_factory *set_thread_param(struct looper_factory *ctor, OS_Priority prio, uint32_t stack)
{
	ctor->prio = prio;
	ctor->stack = stack;
	return ctor;
}

struct looper_factory *set_msg_size(struct looper_factory *ctor, uint32_t size)
{
	if (ctor->looper->msg)
		free(ctor->looper->msg);
	ctor->looper->msg = malloc(size);
	return ctor;
}

struct looper_factory *set_extend_function(struct looper_factory *ctor, looper_extend extend, void *arg)
{
	ctor->looper->extend = extend;
	ctor->looper->arg = arg;
	return ctor;
}

looper_base *create_looper(struct looper_factory *ctor)
{
	looper_base *looper = ctor->looper;
	OS_Priority prio = ctor->prio;
	uint32_t stack = ctor->stack;

	free(ctor);

	if (looper->msg == NULL)
		looper->msg = malloc(sizeof(struct event_msg));

	if (OS_ThreadCreate(&looper->thd, "Looper", main_looper,
						looper, prio, stack) != OS_OK)
	{
		LOOPER_ERROR("thread create error, maybe no RAM to create");
		goto failed;
	}
	return looper;

failed:
	if (looper->msg)
		free(looper->msg);
	if (looper != NULL)
		free(looper);
	return NULL;
}

struct looper_factory *looper_factory_create(struct event_queue *queue)
{
	looper_base *base = malloc(sizeof(*base));
	if (base == NULL)
		return NULL;
	memset(base, 0, sizeof(*base));

	looper_factory *ctor = malloc(sizeof(*ctor));
	if (ctor == NULL)
		goto failed;
	memset(ctor, 0, sizeof(*ctor));

	base->queue = queue;
	base->extend = NULL;
	base->arg = NULL;
	base->msg = NULL;

	ctor->looper = base;
	ctor->prio = OS_PRIORITY_NORMAL;
	ctor->stack = 2 * 1024;
	ctor->set_thread_param = set_thread_param;
	ctor->set_msg_size = set_msg_size;
	ctor->set_extend_function = set_extend_function;
	ctor->create_looper = create_looper;

	return ctor;

failed:
	if (ctor != NULL)
		free(base);
	if (base->msg != NULL)
		free(base->msg);
	if (base != NULL)
		free(base);
	return NULL;
}

