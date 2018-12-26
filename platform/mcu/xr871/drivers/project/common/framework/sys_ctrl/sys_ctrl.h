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


#ifndef SYS_CTRL_H_
#define SYS_CTRL_H_

#include <stdlib.h>
#include "event_queue.h"
#include "publisher.h"
#include "observer.h"

#define SYS_CTRL_PRIO_QUEUE (0)

#define ALL_SUBTYPE (0xFFFF)

typedef enum ctrl_msg_type{
	CTRL_MSG_TYPE_SYSTEM = 0,
	CTRL_MSG_TYPE_NETWORK,
	CTRL_MSG_TYPE_VKEY,
	CTRL_MSG_VOLUME,
	CTRL_MSG_TYPE_SDCARD,
	CTRL_MSG_TYPE_FS,
	CTRL_MSG_TYPE_AUDIO,
	CTRL_MSG_TYPE_HANDLER,
} ctrl_msg_type;

typedef enum key_msg_subtype {
	CTRL_MSG_SUB_TYPE_AD_BUTTON = 0,
	CTRL_MSG_SUB_TYPE_GPIO_BUTTON,
	CTRL_MSG_SUB_TYPE_ALL = ALL_SUBTYPE,
} key_msg_subtype;

/** @brief Get type from event */
#define EVENT_TYPE(event)  ((uint16_t)((event) >> 16))

/** @brief Get subtype from event */
#define EVENT_SUBTYPE(event)  ((uint16_t)((event) & 0xFFFF))

/** @brief Make event from type and subtype */
#define MK_EVENT(type, subtype) \
    (((uint32_t)(type) << 16) | ((uint32_t)(subtype) & 0xFFFF))

#define CMP_EVENT_TYPE(event1, event2)  ((event1 ^ event2) & 0xFFFF0000)

/** @brief Create a observer, the callback will trigger when the event come */
static __inline observer_base *sys_callback_observer_create(uint16_t type,
															uint16_t subtype,
															void (*cb)(uint32_t event, uint32_t data, void *arg),
															void *arg) {
	return callback_observer_create(MK_EVENT(type, subtype), cb, arg);
}

/** @brief Destory a observer */
#define sys_callback_observer_destroy(base) observer_destroy(base)

/** @brief Create system control module, it support message distribute and run handler */
int sys_ctrl_create(void);

/** @brief Attach/regist a observer but the observer will only trigger once */
int sys_ctrl_touch(observer_base *obs);

/** @brief Attach/regist a observer permanent */
int sys_ctrl_attach(observer_base *obs);

/** @brief Detach/unregist a observer, the touched observer no need to detach */
int sys_ctrl_detach(observer_base *obs);

/** @brief Send a event with data, if the queue is full it will wait until timeout */
int sys_event_send(uint16_t type, uint16_t subtype, uint32_t data, uint32_t wait_ms);


/** @brief Send a event with data, and the data need to destruct at last,
		   if the queue is full it will wait until timeout */
int sys_event_send_with_destruct(uint16_t type, uint16_t subtype, void *data,
								 void (*destruct)(event_msg *), uint32_t wait_ms);

/** @brief Send a event with data, and the data need to free at last,
		   if the queue is full it will wait until timeout */
int sys_event_send_with_free(uint16_t type, uint16_t subtype, void *data, uint32_t wait_ms);


/** @brief Request system control to handle exec */
int sys_handler_send(void (*exec)(event_msg *), uint32_t data, uint32_t wait_ms);


/** @brief Request system control to handle exec and wait until the exec finish */
int sys_handler_send_wait_finish(void (*exec)(event_msg *), uint32_t data, uint32_t wait_ms);

#endif /* SYS_CTRL_H_ */
