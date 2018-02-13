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

#if (defined(__CONFIG_ARCH_DUAL_CORE) && defined(__CONFIG_ARCH_APP_CORE))

#include "ducc_os.h"
#include "sys/ducc/ducc_addr.h"
#include "sys/ducc/ducc_app.h"
#include "sys/ducc/ducc_net.h"
#include "sys/mbuf.h"
#include "sys/image.h"
#include "net/wlan/wlan.h"

#include "ducc_debug.h"
#include "ducc_mbox.h"
#include "ducc.h"
#include "pm/pm.h"


/* resouce for normal functions */
#define DUCC_APP_NORMAL_THREAD_PRIO			OS_PRIORITY_ABOVE_NORMAL
#define DUCC_APP_NORMAL_THREAD_STACK_SIZE	(8 * 1024)
static ducc_thread_t g_ducc_app_normal_thread;
static ducc_mutex_t g_ducc_app_normal_mutex;

/* resouce for functions about TX/RX */
#define DUCC_APP_DATA_THREAD_PRIO			OS_PRIORITY_NORMAL
#define DUCC_APP_DATA_THREAD_STACK_SIZE		(8 * 1024)
static ducc_thread_t g_ducc_app_data_thread;
static ducc_mutex_t g_ducc_app_data_mutex;

/* marcos for request */
#define DUCC_APP_REQ_SEND(id, r)	ducc_mbox_send(id, r)
#define DUCC_APP_REQ_RECV(id)		ducc_mbox_recv(id, DUCC_WAIT_FOREVER)

#define DUCC_APP_REQ_WAIT(id)		ducc_req_wait(id)
#define DUCC_APP_REQ_RELEASE(id)	ducc_req_release(id)

/* convert pointer from net core to app core */
#define DUCC_APP_PTR(p)	((void *)DUCC_NETMEM_NET2APP(p))

static ducc_cb_func ducc_app_cb = NULL;

#ifdef CONFIG_PM

static int8_t g_ducc_hw_mbox_suspending = 0;

static int ducc_hw_mbox_suspend(struct soc_device *dev, enum suspend_state_t state)
{
	g_ducc_hw_mbox_suspending = 1;

	switch (state) {
	case PM_MODE_SLEEP:
		break;
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
	case PM_MODE_POWEROFF:
		ducc_mbox_deinit(DUCC_ID_NET2APP_DATA, 0, 1);
		ducc_mbox_deinit(DUCC_ID_APP2NET_DATA, 1, 1);

		ducc_mbox_deinit(DUCC_ID_NET2APP_NORMAL, 0, 1);
		ducc_mbox_deinit(DUCC_ID_APP2NET_NORMAL, 1, 1);
		DUCC_DBG("%s okay\n", __func__);
		break;
	default:
		break;
	}

	return 0;
}

static int ducc_hw_mbox_resume(struct soc_device *dev, enum suspend_state_t state)
{
	switch (state) {
	case PM_MODE_SLEEP:
		break;
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
		ducc_mbox_init(DUCC_ID_APP2NET_NORMAL, 1, 1);
		ducc_mbox_init(DUCC_ID_NET2APP_NORMAL, 0, 1);

		ducc_mbox_init(DUCC_ID_APP2NET_DATA, 1, 1);
		ducc_mbox_init(DUCC_ID_NET2APP_DATA, 0, 1);

		__asm(" dsb \n");
		__asm(" isb \n");
		DUCC_DBG("%s okay\n", __func__);
		break;
	default:
		break;
	}

	g_ducc_hw_mbox_suspending = 0;

	return 0;
}

void hw_mbox_print_regs(void)
{
	//hex_dump_bytes(&hw_mbox_reg_store, sizeof(hw_mbox_reg_store));
}

static struct soc_device_driver ducc_hw_mbox_drv = {
	.name = "hw_mbox",
	.suspend = ducc_hw_mbox_suspend,
	.resume = ducc_hw_mbox_resume,
};

static struct soc_device ducc_hw_mbox_dev = {
	.name = "hw_mbox",
	.driver = &ducc_hw_mbox_drv,
};

#define DUCC_HW_MBOX_DEV (&ducc_hw_mbox_dev)
#else
#define DUCC_HW_MBOX_DEV NULL
#endif

int ducc_app_ioctl(enum ducc_app_cmd cmd, void *param)
{
	struct ducc_req req;
	ducc_mutex_t *mutex;
	uint32_t send_id, wait_id;

	DUCC_APP_DBG("send req %d\n", cmd);
#ifdef CONFIG_PM
	if (g_ducc_hw_mbox_suspending) {
		DUCC_ERR("send req %d when suspending\n", cmd);
		return -1;
	}
#endif

	if (DUCC_APP_IS_DATA_CMD(cmd)) {
		mutex = &g_ducc_app_data_mutex;
		send_id = DUCC_ID_APP2NET_DATA;
		wait_id = DUCC_ID_NET2APP_DATA;
	} else {
		mutex = &g_ducc_app_normal_mutex;
		send_id = DUCC_ID_APP2NET_NORMAL;
		wait_id = DUCC_ID_NET2APP_NORMAL;
	}

	ducc_mutex_lock(mutex);

#if DUCC_SIMULATE_HW_MBOX
	req.id = send_id;
#endif
	req.cmd = (uint32_t)cmd;
	req.param = (uint32_t)param;
	req.result = -1;

	do {
		if (DUCC_APP_REQ_SEND(send_id, &req) < 0) {
			DUCC_WARN("send req %d failed\n", cmd);
			break;
		}

		DUCC_APP_DBG("wait req %d\n", cmd);
		if (DUCC_APP_REQ_WAIT(wait_id) < 0) {
			DUCC_WARN("wait req %d failed\n", cmd);
			break;
		}
	} while (0);

	DUCC_APP_DBG("wait req %d done\n", cmd);

	ducc_mutex_unlock(mutex);

	return req.result;
}

static volatile uint32_t ducc_app_normal_task_term;

static void ducc_app_normal_task(void *arg)
{
	uint32_t recv_id = DUCC_ID_NET2APP_NORMAL;
	uint32_t send_id = DUCC_ID_APP2NET_NORMAL;
	void *net_req;
	struct ducc_req *req;
	struct ducc_param_efuse *efuse;

	while (1) {
		net_req = DUCC_APP_REQ_RECV(recv_id);

		if (ducc_app_normal_task_term)
			break;

		if (net_req == NULL) {
			DUCC_WARN("invalid net req\n");
			continue;
		}

		req = DUCC_APP_PTR(net_req);
#if DUCC_SIMULATE_HW_MBOX
		if (req->id != recv_id) {
			DUCC_WARN("invalid net req, id 0x%x\n", req->id);
			continue;
		}
#endif
		DUCC_APP_DBG("exec req %d\n", req->cmd);

		switch (req->cmd) {
#if (__CONFIG_MBUF_IMPL_MODE == 1)
		case DUCC_NET_CMD_MBUF_GET:
		{
			struct ducc_param_mbuf_get *p = DUCC_APP_PTR(req->param);
			struct mbuf *m = mb_get(p->len, p->tx);
			if (m) {
				MBUF_APP2NET(m);
				p->mbuf = m;
				req->result = 0;
			} else {
				req->result = -1;
			}
			break;
		}
		case DUCC_NET_CMD_MBUF_FREE:
		{
			struct mbuf *m = (struct mbuf *)req->param;
			MBUF_NET2APP(m);
			mb_free(m);
			req->result = 0;
			break;
		}
#endif /* (__CONFIG_MBUF_IMPL_MODE == 1) */
		case DUCC_NET_CMD_POWER_NOTIFY:
			if (ducc_app_cb)
				ducc_app_cb(req->cmd, req->param);
			break;
		case DUCC_NET_CMD_BIN_READ:
			if (ducc_app_cb) {
				struct ducc_param_wlan_bin *p = DUCC_APP_PTR(req->param);
				p->buf = (void *)DUCC_NETMEM_NET2APP(p->buf);
				req->result = ducc_app_cb(DUCC_NET_CMD_BIN_READ, (uint32_t)p);
				p->buf = (void *)DUCC_NETMEM_APP2NET(p->buf);
			} else {
				req->result = 0;
			}
			break;
		case DUCC_NET_CMD_EFUSE_READ:
			efuse = DUCC_APP_PTR(req->param);
			efuse->data = (void *)DUCC_NETMEM_NET2APP(efuse->data);
			req->result = ducc_app_cb(DUCC_NET_CMD_EFUSE_READ, (uint32_t)efuse);
			efuse->data = (void *)DUCC_NETMEM_APP2NET(efuse->data);
			break;
		case DUCC_NET_CMD_SYS_EVENT:
		case DUCC_NET_CMD_WLAN_EVENT:
			if (ducc_app_cb)
				ducc_app_cb(req->cmd, req->param);
			req->result = 0;
			break;
		default:
			DUCC_WARN("invalid command %d\n", req->cmd);
			break;
		};

		DUCC_APP_DBG("exec req %d done\n", req->cmd);

		DUCC_APP_REQ_SEND(send_id, DUCC_RELEASE_REQ_VAL(send_id));
	}

	ducc_app_normal_task_term = 0;
	ducc_thread_exit(&g_ducc_app_normal_thread);
}

static volatile uint32_t ducc_app_data_task_term;

static void ducc_app_data_task(void *arg)
{
	uint32_t recv_id = DUCC_ID_NET2APP_DATA;
	uint32_t send_id = DUCC_ID_APP2NET_DATA;
	void *net_req;
	struct ducc_req *req;

	while (1) {
		net_req = DUCC_APP_REQ_RECV(recv_id);

		if (ducc_app_data_task_term)
			break;

		if (net_req == NULL) {
			DUCC_WARN("invalid net req\n");
			continue;
		}

		req = DUCC_APP_PTR(net_req);
#if DUCC_SIMULATE_HW_MBOX
		if (req->id != recv_id) {
			DUCC_WARN("invalid net req, id 0x%x\n", req->id);
			continue;
		}
#endif
		DUCC_APP_DBG("exec req %d\n", req->cmd);

		switch (req->cmd) {
		case DUCC_NET_CMD_WLAN_INPUT:
		{
			struct ducc_param_wlan_input *p = DUCC_APP_PTR(req->param);
#if (__CONFIG_MBUF_IMPL_MODE == 0)
			req->result = (ethernetif_raw_input(p->nif,
			                                    DUCC_APP_PTR(p->data),
			                                    p->len) == ERR_OK ? 0 : -1);
#elif (__CONFIG_MBUF_IMPL_MODE == 1)
			struct mbuf *m;
			struct pbuf *pb;
			m = p->mbuf;
			MBUF_NET2APP(m);
			pb = mb_mbuf2pbuf(m); /* data including Ethernet header */
			mb_free(m); /* useless now, should be freed */
			req->result = (ethernetif_input(p->nif, pb) == ERR_OK ? 0 : -1);
#endif /* __CONFIG_MBUF_IMPL_MODE */
			break;
		}
		case DUCC_NET_CMD_WLAN_MONITOR_INPUT:
		{
			struct ducc_param_wlan_mon_input *p = DUCC_APP_PTR(req->param);
			wlan_monitor_input(p->nif, DUCC_APP_PTR(p->data), p->len,
			                   p->info ? DUCC_APP_PTR(p->info) : NULL);
			req->result = 0;
			break;
		}
		default:
			DUCC_WARN("invalid command %d\n", req->cmd);
			break;
		};

		DUCC_APP_DBG("exec req %d done\n", req->cmd);

		DUCC_APP_REQ_SEND(send_id, DUCC_RELEASE_REQ_VAL(send_id));
	}

	ducc_app_data_task_term = 0;
	ducc_thread_exit(&g_ducc_app_data_thread);
}

int ducc_app_start(struct ducc_app_param *param)
{
	ducc_app_cb = param->cb;

	ducc_mutex_create(&g_ducc_app_normal_mutex);
	ducc_req_init(DUCC_ID_NET2APP_NORMAL);
	ducc_mbox_init(DUCC_ID_APP2NET_NORMAL, 1, 0);
	ducc_mbox_init(DUCC_ID_NET2APP_NORMAL, 0, 0);

	ducc_mutex_create(&g_ducc_app_data_mutex);
	ducc_req_init(DUCC_ID_NET2APP_DATA);
	ducc_mbox_init(DUCC_ID_APP2NET_DATA, 1, 0);
	ducc_mbox_init(DUCC_ID_NET2APP_DATA, 0, 0);

	ducc_app_normal_task_term = 0;
	if (ducc_thread_create(&g_ducc_app_normal_thread,
	                       ducc_app_normal_task,
	                       NULL,
	                       DUCC_APP_NORMAL_THREAD_PRIO,
	                       DUCC_APP_NORMAL_THREAD_STACK_SIZE) != 0) {
		DUCC_ERR("create thread failed\n");
		return -1;
	}

	ducc_app_data_task_term = 0;
	if (ducc_thread_create(&g_ducc_app_data_thread,
	                       ducc_app_data_task,
	                       NULL,
	                       DUCC_APP_DATA_THREAD_PRIO,
	                       DUCC_APP_DATA_THREAD_STACK_SIZE) != 0) {
		DUCC_ERR("create thread failed\n");
		return -1;
	}
#ifdef CONFIG_PM
	pm_register_ops(DUCC_HW_MBOX_DEV);
#endif

	return 0;
}

int ducc_app_stop(void)
{
#ifdef CONFIG_PM
	pm_unregister_ops(DUCC_HW_MBOX_DEV);
#endif

	ducc_app_data_task_term = 1;
	ducc_mbox_msg_callback(DUCC_ID_NET2APP_DATA, NULL);
	while (ducc_app_data_task_term)
		OS_MSleep(2);

	ducc_app_normal_task_term = 1;
	ducc_mbox_msg_callback(DUCC_ID_NET2APP_NORMAL, NULL);
	while (ducc_app_normal_task_term)
		OS_MSleep(2);

	ducc_mbox_deinit(DUCC_ID_NET2APP_DATA, 0, 0);
	ducc_mbox_deinit(DUCC_ID_APP2NET_DATA, 1, 0);
	ducc_req_deinit(DUCC_ID_NET2APP_DATA);
	ducc_mutex_delete(&g_ducc_app_data_mutex);

	ducc_mbox_deinit(DUCC_ID_NET2APP_NORMAL, 0, 0);
	ducc_mbox_deinit(DUCC_ID_APP2NET_NORMAL, 1, 0);
	ducc_req_deinit(DUCC_ID_NET2APP_NORMAL);
	ducc_mutex_delete(&g_ducc_app_normal_mutex);

	ducc_app_cb = NULL;

	return 0;
}

#endif /* (defined(__CONFIG_ARCH_DUAL_CORE) && defined(__CONFIG_ARCH_APP_CORE)) */
