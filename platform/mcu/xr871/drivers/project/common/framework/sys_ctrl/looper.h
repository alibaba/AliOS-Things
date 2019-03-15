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


#ifndef LOOPER_H_
#define LOOPER_H_

#include "sys/list.h"
#include "kernel/os/os.h"
#include "event_queue.h"

typedef void *(*looper_extend)(struct event_msg *, void *);
#define LOOP_HANDLE(handler, msg, arg) (looper_extend)(handler)(msg, arg)

typedef struct looper_base looper_base;

typedef struct looper_factory
{
	looper_base *looper;
	OS_Priority prio;
	uint32_t stack;
	struct looper_factory *(*set_thread_param)(struct looper_factory *ctor, OS_Priority prio, uint32_t stack);
	struct looper_factory *(*set_msg_size)(struct looper_factory *ctor, uint32_t size);
	struct looper_factory *(*set_extend_function)(struct looper_factory *ctor, looper_extend extend, void *arg);
	looper_base *(*create_looper)(struct looper_factory *ctor);
} looper_factory;

/* a simple looper create function. */
looper_base *looper_create(struct event_queue *queue);

/* a factory config looper for create looper. */
struct looper_factory *looper_factory_create(struct event_queue *queue);


#endif /* LOOPER_H_ */
