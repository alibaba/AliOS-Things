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
    event_callback(K_POLL_TYPE_DATA_RECV);
}

static int hci_driver_open(void)
{
	u32_t err;

	DEBUG_INIT();
	err = ll_init(pkt_recv_callback);
	if (err) {
		BT_ERR("LL initialization failed: %u", err);
		return err;
	}
	hci_init(NULL);

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
