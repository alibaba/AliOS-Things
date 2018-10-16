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


#ifndef OBSERVER_H_
#define OBSERVER_H_

#include <stdlib.h>
#include "sys/list.h"
#include "kernel/os/os.h"


typedef enum observer_types
{
	EVENT_OBSERVER,
	CALLBACK_OBSERVER,
	THREAD_OBSERVER,
} observer_types;

typedef enum observer_modes
{
	SINGLE_OBSERVER,
	CONTINUE_OBSERVER,
} observer_modes;

typedef enum observer_state
{
	OBSERVER_ILDE,
	OBSERVER_ATTACHED,
	OBSERVER_ATTACHED_ONCE,
	OBSERVER_DETACHED,
	OBSERVER_WORKING,
} observer_state;

typedef struct observer_base
{
	struct list_head node;
	uint32_t event;
	int state;
	void *arg;
	void (*trigger)(struct observer_base *base, uint32_t event, uint32_t arg);
} observer_base;

/*
typedef struct observable_base
{
	struct list_head head;
	int (*attach)(struct observable_base *, struct observer_base *);
	int (*detach)(struct observable_base *, struct observer_base *);
	int (*clear)(struct observable_base *);
	void (*notify)(struct observable_base *);
} observable_base;
*/

/* !!! only for class extension, xxx_observer_create already initialize observer */
/* for example:
 *    typedef struct button {
 *       observable_base obs;
 *       ...;
 *    } button;
 *
 *    button *button_create() {
 *         ...;
 *         observer_init(&bt.obs, ...);
 *         ...;
 *     }
 */
int observer_init(observer_base *base, uint32_t event,
				  void (*trigger)(struct observer_base *base, uint32_t event, uint32_t data), void *arg);


/* event observer */
typedef struct event_observer
{
    observer_base base;
    OS_Semaphore_t sem;
} event_observer;

/* callback observer */
typedef struct callback_observer
{
    observer_base base;
    void (*cb)(uint32_t event, uint32_t data, void *arg);
} callback_observer;

/* thread observer */
typedef struct thread_observer
{
    observer_base base;
    OS_Thread_t thd;
    void (*run)(uint32_t event, uint32_t data, void *arg);
    void (*exception)(int ret);
    uint32_t stack;
    OS_Priority prio;

    uint32_t event;
    uint32_t arg;
} thread_observer;


/*
 * !!! Notice: observer create already initialize observer
 */
/* event observer : event_wait() will wait until a event observed */
observer_base *event_observer_create(uint32_t event);

OS_Status event_wait(observer_base *base, OS_Time_t timeout);

/* callback observer : a event observed will trigger callback  */
observer_base *callback_observer_create(uint32_t event,
										void (*cb)(uint32_t event, uint32_t data, void *arg),
										void *arg);

/* thread observer : a event observed will create a thread to run, exception will throw if create failed  */
observer_base *thread_observer_create(uint32_t event,
									  void (*run)(uint32_t event, uint32_t data, void *arg),
									  void *arg,
									  uint32_t stackSize,
									  OS_Priority prio);

void thread_observer_throw(struct observer_base *base, void (*exception)(int ret));

int observer_destroy(observer_base *base);

#endif /* OBSERVER_H_ */
