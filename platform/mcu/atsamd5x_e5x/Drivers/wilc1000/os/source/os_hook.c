/**  namma
 *
 * \file
 *
 * \brief
 *
 * Copyright (c) 2016-2018 Atmel Corporation. All rights reserved.
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

#include <os/include/os_hook.h>
//#include <FreeRTOS.h>
//#include <queue.h>
//#include <semphr.h>
//#include <task.h>
#include <lwip/tcpip.h>
#include <k_api.h>

#define QUEUE_MAX_MSG_SIZE      (sizeof(hif_msg_t))
#define QUEUE_MAX_MSG_COUNT		(32)
#define QUEUE_SIZE				(QUEUE_MAX_MSG_SIZE * QUEUE_MAX_MSG_COUNT)


extern void wilc_netif_tx_from_queue(hif_msg_t *msg);
extern uint8_t m2m_wifi_handle_events(void *p);


aos_task_t hif_task;
/* HIF thread. */
ktask_t * hif_thread;



/* HIF thread queue. */
aos_queue_t  hif_queue;


 aos_sem_t sdio_rw_sem;

 aos_sem_t os_isr_sem;
 
 hif_msg_t queue_buf[32];

/* HIF thread binary semaphore. */
static aos_sem_t hif_notify_sem;

/* HIF initialized. */
static uint8_t has_init = 0;

/**
 * \brief HIF thread entry function.
 */
static void os_hif_task(void *pv)
{
	while(1) {
		hif_msg_t msg;
		int size_recv;
		
		if (aos_queue_recv(&hif_queue, AOS_WAIT_FOREVER, &msg, &size_recv) == RHINO_SUCCESS) {
			/* WiFi init. */
			if (msg.id == MSG_START) {
				if (msg.handler) 
					msg.handler(msg.priv);				
			}
			
			/* Incoming data packet. */
			else if (msg.id == MSG_RX) {
				//aos_sem_wait(&os_isr_sem, AOS_WAIT_FOREVER);
				m2m_wifi_handle_events(NULL);
				//aos_sem_signal(&os_isr_sem);

			} 
			/* Outgoing data packet. */
			else if ((msg.id == MSG_TX_STA) || (msg.id == MSG_TX_AP)) {
				//aos_sem_wait(&os_isr_sem, AOS_WAIT_FOREVER);
				wilc_netif_tx_from_queue(&msg);
				//aos_sem_signal(&os_isr_sem);
			}
			/* WiFi command. */
			else if (msg.id == MSG_CMD) {

				if (msg.handler) 
					msg.handler(msg.priv);
			} 
			/* Error. */
			else {
				printf("Warning: Wrong id  msg id %lu \r\n", msg.id);
			}
		}
	}
}

/**
 * \brief Put interrupt request in netif queue for later processing.
 */
void os_hook_isr(void)
{
	//signed long woken = pdFALSE;
	hif_msg_t msg;

	msg.id = MSG_RX;
	msg.pbuf = NULL;
	msg.payload_size = 0;
	msg.payload = NULL;
	if (aos_queue_send(&hif_queue, &msg, sizeof(msg)) != RHINO_SUCCESS) {
		printf("os_hook_isr: queue full error!\n");	
	}
	//else {
	//	portEND_SWITCHING_ISR(woken);
	//}
}

/**
 * \brief Initialize netif thread.
 */
void os_hook_init(void)
{
	int ret;

	if (!has_init) {
		aos_sem_new(&hif_notify_sem, 1);
		//aos_sem_new(&sdio_rw_sem, 1);
		//aos_sem_new(&os_isr_sem, 1);
		aos_sem_wait(&hif_notify_sem, AOS_WAIT_FOREVER);
		ret = aos_queue_new(&hif_queue, queue_buf, QUEUE_SIZE, QUEUE_MAX_MSG_SIZE);
		
		//xTaskCreate(os_hif_task, (const signed char *)"WiFiHIF", 1024, NULL, (configMAX_PRIORITIES - 1), &hif_thread);		
		aos_task_new_ext(&hif_task, "WiFiHIF", os_hif_task, NULL, 1024, RHINO_CONFIG_USER_PRI_MAX);	
		
		has_init = 1;
	}
}

void os_hook_set_handle(void *task)
{
	//hif_thread = task;
	hif_thread = krhino_cur_task_get();
}

/**
 * \brief Run handler function in the netif thread context and return immediately.
 */
uint8_t os_hook_dispatch_no_wait(wifi_task_cb handler, void *p)
{
	hif_msg_t msg;
	if (hif_thread == krhino_cur_task_get()) {
		handler(p);
	} 
	else {
		msg.id = MSG_CMD;
		msg.handler = handler;
		msg.priv = p;
		return aos_queue_send(&hif_queue, &msg, sizeof(msg));
	}
	return 0;
}

/**
 * \brief Run handler function in the netif thread context and wait for function return.
 */
void os_hook_dispatch_wait(wifi_task_cb handler, struct params_dispatch *p, void* pv)
{
	hif_msg_t msg;
	if (hif_thread == krhino_cur_task_get()) {
		p->signal_semaphore = 0;
		handler(pv);
	}
	else {
		p->signal_semaphore = 1;
		msg.id = MSG_CMD;
		msg.handler = handler;
		msg.priv = pv;
		aos_queue_send(&hif_queue, &msg, sizeof(msg));
		
		aos_sem_wait(&hif_notify_sem, AOS_WAIT_FOREVER);
	}
}

/**
 * \brief Start WiFi in the netif thread context and wait for function return.
 */
void os_hook_send_start(wifi_task_cb handler, struct params_dispatch *p, void* pv)
{
	hif_msg_t msg;
 
	p->signal_semaphore = 1;
	msg.id = MSG_START;
	msg.handler = handler;
	msg.priv = pv;
	aos_queue_send(&hif_queue, &msg, sizeof(msg));
	aos_sem_wait(&hif_notify_sem, AOS_WAIT_FOREVER);
}

/**
 * \brief Release lock to notify calling thread that netif job is done.
 */
void os_hook_notify(void)
{
	aos_sem_signal(&hif_notify_sem);
}
