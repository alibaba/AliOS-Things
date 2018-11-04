/**
 *
 * \file
 *
 * \brief
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#ifndef OS_HOOK_H_INCLUDED
#define OS_HOOK_H_INCLUDED

//#include "compiler.h"

typedef void (*wifi_task_cb)(void *arg);

struct params_dispatch {
	unsigned char signal_semaphore;
	signed char retval;
	void * param;
};

#define MSG_RX 1
#define MSG_TX_STA 2
#define MSG_TX_AP 3
#define MSG_QUIT 4
#define MSG_CMD 5
#define MSG_START 6

typedef struct {
	unsigned int id;
	void *pbuf;
	void * payload;
	unsigned int payload_size;
	wifi_task_cb	handler;
	void * priv;
} hif_msg_t ;

void os_hook_isr(void);
void os_hook_init(void);
void os_hook_set_handle(void *task);
unsigned char os_hook_dispatch_no_wait(wifi_task_cb handler, void *p);
void os_hook_dispatch_wait(wifi_task_cb handler, struct params_dispatch *p,void* pv);
void os_hook_notify(void);
void os_hook_deinit(void);
void os_hook_send_start(wifi_task_cb handler, struct params_dispatch *p, void* pv);
void os_hook_send_stop(wifi_task_cb handler, struct params_dispatch *p, void* pv);

#endif /* OS_HOOK_H_INCLUDED */
