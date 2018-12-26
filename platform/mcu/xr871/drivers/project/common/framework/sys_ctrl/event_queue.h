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


#ifndef EVENT_QUEUE_H_
#define EVENT_QUEUE_H_

#include <stdint.h>

typedef struct event_msg event_msg;
struct event_msg {
	void (*execute)(event_msg *msg);
	void (*destruct)(event_msg *msg);
	uint32_t 	event	/* subtype = 0 means all subtype */;
	uint32_t	data;
	uint32_t	extra[0];
};

typedef struct event_queue
{
//	int (*init)(struct event_queue *base, uint32_t queue_len);
	int (*deinit)(struct event_queue *base);
	int (*send)(struct event_queue *base, struct event_msg *msg, uint32_t wait_ms);
	int (*recv)(struct event_queue *base, struct event_msg *msg, uint32_t wait_ms);
} event_queue;


struct event_queue *prio_event_queue_create(uint32_t queue_len, uint32_t msg_size);

struct event_queue *normal_event_queue_create(uint32_t queue_len, uint32_t msg_size);

#endif /* EVENT_QUEUE_H_ */
