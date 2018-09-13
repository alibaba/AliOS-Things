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
#include "sys/list.h"
#include "kernel/os/os.h"
#include "observer.h"

#define OBSERVER_DEBUG(fmt, arg...)	//printf("[Observer debug] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define OBSERVER_ALERT(fmt, arg...)	printf("[Observer alert] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define OBSERVER_ERROR(fmt, arg...)	printf("[Observer error] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define OBSERVER_NOWAY()			printf("[Observer should not be here] <%s : %d> \n", __func__, __LINE__)
#define OBSERVER_NOTSUPPORT() 		OBSERVER_ALERT("not support command")


typedef struct event_observer
{
	observer_base base;
	OS_Semaphore_t sem;
} event_observer;

static void event_trigger(struct observer_base *base, uint32_t event, uint32_t arg)
{
	event_observer *impl = __containerof(base, event_observer, base);

	OS_SemaphoreRelease(&impl->sem);
}

observer_base *event_observer_create(uint32_t event)
{
	event_observer *obs = malloc(sizeof(*obs));
	if (obs == NULL)
		return NULL;
	memset(obs, 0, sizeof(*obs));

	obs->base.event = event;
//	obs->base.type = EVENT_OBSERVER;
	obs->base.trigger = event_trigger;
	INIT_LIST_HEAD(&obs->base.node);
	OS_SemaphoreCreateBinary(&obs->sem);

	return &obs->base;
}

OS_Status event_wait(observer_base *base, OS_Time_t timeout)
{
	event_observer *impl = __containerof(base, event_observer, base);

	return OS_SemaphoreWait(&impl->sem, timeout);
}


typedef struct callback_observer
{
	observer_base base;
	void (*cb)(uint32_t event, uint32_t arg);
} callback_observer;

static void trigger_callback(struct observer_base *base, uint32_t event, uint32_t arg)
{
	callback_observer *impl = __containerof(base, callback_observer, base);

	impl->cb(event, arg);
}

observer_base *callback_observer_create(uint32_t event, void (*cb)(uint32_t event, uint32_t arg))
{
	callback_observer *obs = malloc(sizeof(*obs));
	if (obs == NULL)
		return NULL;
	memset(obs, 0, sizeof(*obs));

	obs->base.event = event;
//	obs->base.type = CALLBACK_OBSERVER;
	obs->base.trigger = trigger_callback;
	INIT_LIST_HEAD(&obs->base.node);
	obs->cb = cb;

	return &obs->base;
}


typedef struct thread_observer
{
	observer_base base;
	OS_Thread_t thd;
	void (*run)(uint32_t event, uint32_t arg);
	void (*exception)(int ret);
	uint32_t stack;
	OS_Priority prio;

	uint32_t event;
	uint32_t arg;
} thread_observer;

static void wrap_thread(void *arg)
{
	thread_observer *impl = (thread_observer *)arg;

	impl->run(impl->event, impl->arg);

	OS_ThreadDelete(&impl->thd);
}

static void trigger_thread(struct observer_base *base, uint32_t event, uint32_t arg)
{
	thread_observer *impl = __containerof(base, thread_observer, base);
	OS_Status ret;

	impl->event = event;
	impl->arg = arg;

	if (OS_ThreadIsValid(&impl->thd))
	{
		OBSERVER_ALERT("thread still running or not init");
		return;
	}

	if ((ret = OS_ThreadCreate(&impl->thd, "Trigger Thread", wrap_thread, impl, impl->prio, impl->stack)) != OS_OK)
	{
		OBSERVER_ERROR("thread create error, maybe no RAM to create");
		if (impl->exception != NULL)
			impl->exception(ret);
	}
}

void thread_observer_throw(struct observer_base *base, void (*exception)(int ret))
{
	thread_observer *impl = __containerof(base, thread_observer, base);

	impl->exception = exception;
}

/* TODO: thread_observer_copy_data(struct observer_base *base, int (*copy)(uint32_t data)) */

observer_base *thread_observer_create(uint32_t event, void (*run)(uint32_t event, uint32_t arg), uint32_t stackSize, OS_Priority prio)
{
	thread_observer *obs = malloc(sizeof(*obs));
	if (obs == NULL)
		return NULL;
	memset(obs, 0, sizeof(*obs));

	/* TODO: need mode in case of trigger serval times */
	obs->base.event = event;
//	obs->base.type = THREAD_OBSERVER;
	obs->base.trigger = trigger_thread;
	INIT_LIST_HEAD(&obs->base.node);
	obs->run = run;
	obs->stack = stackSize;
	obs->prio = prio;

	return &obs->base;
}

int observer_destroy(observer_base *base)
{
	if (base == NULL)
		return -1;
	if (base->state != OBSERVER_ILDE)
		return -1;
	free(base);
	return 0;
}


