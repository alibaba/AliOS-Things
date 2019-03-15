/*
 * Copyright (c) 2016 Nordic Semiconductor ASA
 * Copyright (c) 2016 Vinayak Kariappa Chettimada
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <zephyr.h>

#include <device.h>
#include <clock_control.h>
#include <atomic.h>

#include <misc/util.h>
#include <misc/stack.h>
#include <misc/byteorder.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <drivers/bluetooth/hci_driver.h>

#ifdef CONFIG_CLOCK_CONTROL_NRF5
#include <drivers/clock_control/nrf5_clock_control.h>
#endif

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_DEBUG_HCI_DRIVER)
#include "common/log.h"

#include "util/util.h"
#include "hal/ccm.h"
#include "hal/radio.h"
#include "ll_sw/pdu.h"
#include "ll_sw/ctrl.h"
#include "ll.h"
#include "hci_internal.h"
#include "init.h"
#include "hal/debug.h"

#define NODE_RX(_node) CONTAINER_OF(_node, struct radio_pdu_node_rx, \
				    hdr.onion.node)

#ifndef CONFIG_CONTROLLER_IN_ONE_TASK
#define CONFIG_BT_CTLR_RX_PRIO_STACK_SIZE 512
#define CONFIG_BT_RX_STACK_SIZE 512
#define CONFIG_BT_CTLR_RX_PRIO 18
static K_SEM_DEFINE(sem_prio_recv, 0, UINT_MAX);
static K_SEM_DEFINE(sem_fifo_recv, 0, UINT_MAX);
static K_FIFO_DEFINE(recv_fifo);

struct k_thread prio_recv_thread_data;
static BT_STACK_NOINIT(prio_recv_thread_stack,
                       CONFIG_BT_CTLR_RX_PRIO_STACK_SIZE);
struct k_thread recv_thread_data;
static BT_STACK_NOINIT(recv_thread_stack, CONFIG_BT_RX_STACK_SIZE);
#endif

static inline struct net_buf *process_node(struct radio_pdu_node_rx *node_rx);

int hci_driver_recv(void)
{
    while (1) {
        struct radio_pdu_node_rx *node_rx = NULL;
        struct net_buf *buf = NULL;
        u8_t num_cmplt = 0;
        u16_t handle;

        while ((num_cmplt = radio_rx_get(&node_rx, &handle))) {
#if defined(CONFIG_BT_CONN)
            struct net_buf *buf;

            buf = bt_buf_get_rx(BT_BUF_EVT, K_FOREVER);
            if (buf == NULL) {
                break;
            }
            hci_num_cmplt_encode(buf, handle, num_cmplt);
            bt_recv_prio(buf);
#endif
        }

        if (node_rx) {
            radio_rx_dequeue();
            buf = process_node(node_rx);
        }

        if (buf) {
            if (buf->len) {
                bt_recv(buf);
            } else {
                net_buf_unref(buf);
            }
            continue;
        }
        break;
    }
    return 0;
}

#ifndef CONFIG_CONTROLLER_IN_ONE_TASK
static void prio_recv_thread(void *p1, void *p2, void *p3)
{
        while (1) {
                struct radio_pdu_node_rx *node_rx;
                u8_t num_cmplt;
                u16_t handle;

                while ((num_cmplt = radio_rx_get(&node_rx, &handle))) {
#if defined(CONFIG_BT_CONN)
                        struct net_buf *buf;

                        buf = bt_buf_get_rx(BT_BUF_EVT, K_FOREVER);
                        hci_num_cmplt_encode(buf, handle, num_cmplt);
                        BT_DBG("Num Complete: 0x%04x:%u", handle, num_cmplt);
                        bt_recv_prio(buf);
                        k_yield();
#endif
                }

                if (node_rx) {

                        radio_rx_dequeue();

                        BT_DBG("RX node enqueue");
                        k_fifo_put(&recv_fifo, node_rx);
                        k_sem_give(&sem_fifo_recv);

                        continue;
                }

                BT_DBG("sem take...");
                k_sem_take(&sem_prio_recv, K_FOREVER);
                BT_DBG("sem taken");

#if defined(CONFIG_INIT_STACKS)
                if (k_uptime_get_32() - prio_ts > K_SECONDS(5)) {
                        STACK_ANALYZE("prio recv thread stack",
                                      prio_recv_thread_stack);
                        prio_ts = k_uptime_get_32();
                }
#endif
        }
}
#endif

static inline struct net_buf *encode_node(struct radio_pdu_node_rx *node_rx,
					  s8_t class)
{
	struct net_buf *buf = NULL;

	/* Check if we need to generate an HCI event or ACL data */
	switch (class) {
	case HCI_CLASS_EVT_DISCARDABLE:
	case HCI_CLASS_EVT_REQUIRED:
	case HCI_CLASS_EVT_CONNECTION:
		if (class == HCI_CLASS_EVT_DISCARDABLE) {
			buf = bt_buf_get_rx(BT_BUF_EVT, K_NO_WAIT);
		} else {
			buf = bt_buf_get_rx(BT_BUF_EVT, K_FOREVER);
		}
		if (buf) {
			hci_evt_encode(node_rx, buf);
		}
		break;
#if defined(CONFIG_BT_CONN)
	case HCI_CLASS_ACL_DATA:
		/* generate ACL data */
		buf = bt_buf_get_rx(BT_BUF_ACL_IN, K_FOREVER);
		hci_acl_encode(node_rx, buf);
		break;
#endif
	default:
		LL_ASSERT(0);
		break;
	}

	radio_rx_fc_set(node_rx->hdr.handle, 0);
	node_rx->hdr.onion.next = 0;
	radio_rx_mem_release(&node_rx);

	return buf;
}

static inline struct net_buf *process_node(struct radio_pdu_node_rx *node_rx)
{
	s8_t class = hci_get_class(node_rx);
	struct net_buf *buf = NULL;

	/* process regular node from radio */
	buf = encode_node(node_rx, class);

	return buf;
}

#ifndef CONFIG_CONTROLLER_IN_ONE_TASK
static void recv_thread(void *p1, void *p2, void *p3)
{

        while (1) {
                struct radio_pdu_node_rx *node_rx = NULL;
                struct net_buf *buf = NULL;

                BT_DBG("blocking");
                k_sem_take(&sem_fifo_recv, K_FOREVER);
                node_rx = k_fifo_get(&recv_fifo, K_FOREVER);
                BT_DBG("unblocked");

                if (node_rx && !buf) {
                        /* process regular node from radio */
                        buf = process_node(node_rx);
                }

                if (buf) {
                        if (buf->len) {
                                BT_DBG("Packet in: type:%u len:%u",
                                        bt_buf_get_type(buf), buf->len);
                                bt_recv(buf);
                        } else {
                                net_buf_unref(buf);
                        }
                }

                k_yield();
        }
}
#endif

static int cmd_handle(struct net_buf *buf)
{
    struct net_buf *evt;

    evt = hci_cmd_handle(buf);
    if (evt) {
        BT_DBG("Replying with event of %u bytes", evt->len);
        bt_recv_prio(evt);
    }
    return 0;
}

#if defined(CONFIG_BT_CONN)
static int acl_handle(struct net_buf *buf)
{
	struct net_buf *evt;
	int err;

	err = hci_acl_handle(buf, &evt);
	if (evt) {
		BT_DBG("Replying with event of %u bytes", evt->len);
		bt_recv_prio(evt);
	}

	return err;
}
#endif /* CONFIG_BT_CONN */

static int hci_driver_send(struct net_buf *buf)
{
	u8_t type;
	int err;

	BT_DBG("enter");

	if (!buf->len) {
		BT_ERR("Empty HCI packet");
		return -EINVAL;
	}

	type = bt_buf_get_type(buf);
	switch (type) {
#if defined(CONFIG_BT_CONN)
	case BT_BUF_ACL_OUT:
		err = acl_handle(buf);
		break;
#endif /* CONFIG_BT_CONN */
	case BT_BUF_CMD:
		err = cmd_handle(buf);
		break;
	default:
		BT_ERR("Unknown HCI type %u", type);
		return -EINVAL;
	}

	if (!err) {
            net_buf_unref(buf);
	}

	BT_DBG("exit: %d", err);

	return err;
}

void pkt_recv_callback(void)
{
#ifdef CONFIG_CONTROLLER_IN_ONE_TASK
    //unsigned int key;
    extern struct k_poll_signal g_pkt_recv;

    //key = irq_lock();
    g_pkt_recv.signaled = 1;
    //irq_unlock(key);

    event_callback(K_POLL_TYPE_DATA_RECV);
#else
    k_sem_give(&sem_prio_recv);
#endif
}

static int hci_driver_open(void)
{
	u32_t err;

	DEBUG_INIT();
#ifndef CONFIG_CONTROLLER_IN_ONE_TASK
        k_sem_init(&sem_prio_recv, 0, UINT_MAX);
        k_sem_init(&sem_fifo_recv, 0, UINT_MAX);
#endif
	err = ll_init(pkt_recv_callback);
	if (err) {
		BT_ERR("LL initialization failed: %u", err);
		return err;
	}
	hci_init(NULL);

#ifndef CONFIG_CONTROLLER_IN_ONE_TASK
    k_fifo_init(&recv_fifo);

    k_thread_create(&prio_recv_thread_data, prio_recv_thread_stack,
                    K_THREAD_STACK_SIZEOF(prio_recv_thread_stack),
                    prio_recv_thread, NULL, NULL, NULL,
                    K_PRIO_COOP(CONFIG_BT_CTLR_RX_PRIO), 0, K_NO_WAIT);

    k_thread_create(&recv_thread_data, recv_thread_stack,
                    K_THREAD_STACK_SIZEOF(recv_thread_stack),
                    recv_thread, NULL, NULL, NULL,
                    K_PRIO_COOP(CONFIG_BT_RX_PRIO), 0, K_NO_WAIT);
#endif

	return 0;
}

static const struct bt_hci_driver drv = {
	.name = "Controller",
	.bus = BT_HCI_DRIVER_BUS_VIRTUAL,
	.open = hci_driver_open,
	.send = hci_driver_send,
        .recv = hci_driver_recv,
};

static int _hci_driver_init(struct device *unused)
{
	ARG_UNUSED(unused);

	bt_hci_driver_register(&drv);

	return 0;
}

int hci_driver_init()
{
    bt_hci_driver_register(&drv);
    return 0;
}

SYS_INIT(_hci_driver_init, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE);
