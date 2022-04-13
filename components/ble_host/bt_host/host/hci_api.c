/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ble_os.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <atomic.h>
#include <misc/util.h>
#include <misc/slist.h>
#include <misc/byteorder.h>
#include <misc/stack.h>
#include <misc/__assert.h>

#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_vs.h>
#include <bluetooth/hci_driver.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_DEBUG_HCI_CORE)
#include "common/log.h"

#include "common/rpa.h"
#include "keys.h"
#include "monitor.h"
#include "hci_core.h"
#include "hci_ecc.h"
#include "ecc.h"

#include "conn_internal.h"
#include "l2cap_internal.h"
#include "gatt_internal.h"
#include "smp.h"
#include "bluetooth/crypto.h"
#include "settings.h"
//#include "soc.h" //only for phy6220
#include "hci_api.h"

#if defined(CONFIG_BT_USE_HCI_API)

#define __hci_api_weak__ __attribute__((weak))

struct hci_debug_counter_t g_hci_debug_counter = {0};

struct cmd_state_set {
	atomic_t *target;
	int bit;
	bool val;
};

struct cmd_data {
	/** HCI status of the command completion */
	u8_t  status;

	/** The command OpCode that the buffer contains */
	u16_t opcode;

	/** The state to update when command completes with success. */
	struct cmd_state_set *state;

	/** Used by bt_hci_cmd_send_sync. */
	struct k_sem *sync;
};

extern void cmd_state_set_init(struct cmd_state_set *state, atomic_t *target, int bit,
			bool val);
static struct cmd_data cmd_data[CONFIG_BT_HCI_CMD_COUNT];

#define cmd(buf) (&cmd_data[net_buf_id(buf)])

#define HCI_CMD_TIMEOUT	  K_SECONDS(10)


/* HCI command buffers. Derive the needed size from BT_BUF_RX_SIZE since
 * the same buffer is also used for the response.
 */
#define CMD_BUF_SIZE BT_BUF_RX_SIZE
NET_BUF_POOL_DEFINE(hci_cmd_pool, CONFIG_BT_HCI_CMD_COUNT,
                    CMD_BUF_SIZE, BT_BUF_USER_DATA_MIN, NULL);


/* Number of commands controller can accept */
static struct k_sem		ncmd_sem;

/* Last sent HCI command */
static struct net_buf		*sent_cmd;

/* Queue for outgoing HCI commands */
static struct kfifo		     cmd_tx_queue;

static inline void handle_event(u8_t event, struct net_buf *buf,
				const struct event_handler *handlers,
				size_t num_handlers)
{
	size_t i;

	for (i = 0; i < num_handlers; i++) {
		const struct event_handler *handler = &handlers[i];

		if (handler->event != event) {
			continue;
		}

		if (buf->len < handler->min_len) {
			BT_ERR("Too small (%u bytes) event 0x%02x",
			       buf->len, event);
			return;
		}

		handler->handler(buf);
		return;
	}

	BT_WARN("Unhandled event 0x%02x len %u: %s", event,
		buf->len, bt_hex(buf->data, buf->len));
}

void hci_num_completed_packets(struct net_buf *buf)
{
	struct bt_hci_evt_num_completed_packets *evt = (void *)buf->data;
	int i;

	BT_DBG("num_handles %u", evt->num_handles);

	for (i = 0; i < evt->num_handles; i++) {
		u16_t handle, count;

		handle = sys_le16_to_cpu(evt->h[i].handle);
		count = sys_le16_to_cpu(evt->h[i].count);

		hci_api_num_complete_packets(1, &handle, &count);
	}
}

struct net_buf *bt_buf_get_cmd_complete(k_timeout_t timeout)
{
	struct net_buf *buf;
	unsigned int key;

	key = irq_lock();
	buf = sent_cmd;
	sent_cmd = NULL;
	irq_unlock(key);

	BT_DBG("sent_cmd %p", buf);

	if (buf) {
		bt_buf_set_type(buf, BT_BUF_EVT);
		buf->len = 0U;
		net_buf_reserve(buf, BT_BUF_RESERVE);

		return buf;
	}

	return bt_buf_get_rx(BT_BUF_EVT, timeout);
}

static void hci_cmd_done(u16_t opcode, u8_t status, struct net_buf *buf)
{
    BT_DBG("opcode 0x%04x status 0x%02x buf %p", opcode, status, buf);

    if (net_buf_pool_get(buf->pool_id) != &hci_cmd_pool) {
        BT_WARN("opcode 0x%04x pool id %u pool %p != &hci_cmd_pool %p",
                opcode, buf->pool_id, net_buf_pool_get(buf->pool_id),
                &hci_cmd_pool);
        return;
    }

    if (cmd(buf)->opcode != opcode) {
        BT_WARN("OpCode 0x%04x completed instead of expected 0x%04x",
                opcode, cmd(buf)->opcode);
    }

	if (cmd(buf)->state && !status) {
		struct cmd_state_set *update = cmd(buf)->state;

		atomic_set_bit_to(update->target, update->bit, update->val);
	}

    /* If the command was synchronous wake up bt_hci_cmd_send_sync() */
    if (cmd(buf)->sync) {
        cmd(buf)->status = status;
        k_sem_give(cmd(buf)->sync);
    }
}

void hci_cmd_complete(struct net_buf *buf)
{
	struct bt_hci_evt_cmd_complete *evt;
	u8_t status, ncmd;
	u16_t opcode;

	evt = net_buf_pull_mem(buf, sizeof(*evt));
	ncmd = evt->ncmd;
	opcode = sys_le16_to_cpu(evt->opcode);

	BT_DBG("opcode 0x%04x", opcode);

	/* All command return parameters have a 1-byte status in the
	 * beginning, so we can safely make this generalization.
	 */
	status = buf->data[0];

	hci_cmd_done(opcode, status, buf);

	/* Allow next command to be sent */
	if (ncmd) {
		k_sem_give(&ncmd_sem);
	}
}

void hci_cmd_status(struct net_buf *buf)
{
	struct bt_hci_evt_cmd_status *evt;
	u16_t opcode;
	u8_t ncmd;

	evt = net_buf_pull_mem(buf, sizeof(*evt));
	opcode = sys_le16_to_cpu(evt->opcode);
	ncmd = evt->ncmd;

	BT_DBG("opcode 0x%04x", opcode);

	hci_cmd_done(opcode, evt->status, buf);

	/* Allow next command to be sent */
	if (ncmd) {
		k_sem_give(&ncmd_sem);
	}
}

void send_cmd(void)
{
	struct net_buf *buf;
	int err;

	/* Get next command */
	BT_DBG("calling net_buf_get");
	buf = net_buf_get(&cmd_tx_queue, K_NO_WAIT);
	BT_ASSERT(buf);

	/* Wait until ncmd > 0 */
	BT_DBG("calling sem_take_wait");
	k_sem_take(&ncmd_sem, K_FOREVER);

	/* Clear out any existing sent command */
	if (sent_cmd) {
		BT_ERR("Uncleared pending sent_cmd");
		net_buf_unref(sent_cmd);
		sent_cmd = NULL;
	}

	sent_cmd = net_buf_ref(buf);

	BT_DBG("Sending command 0x%04x (buf %p) to driver",
	       cmd(buf)->opcode, buf);

	err = bt_send(buf);
	if (err) {
		BT_ERR("Unable to send to driver (err %d)", err);
		k_sem_give(&ncmd_sem);
		hci_cmd_done(cmd(buf)->opcode, BT_HCI_ERR_UNSPECIFIED, buf);
		net_buf_unref(sent_cmd);
		sent_cmd = NULL;
		net_buf_unref(buf);
	}
}

u16_t bt_hci_get_cmd_opcode(struct net_buf *buf)
{
    return cmd(buf)->opcode;
}

struct net_buf *bt_hci_cmd_create(u16_t opcode, u8_t param_len)
{
    struct bt_hci_cmd_hdr *hdr;
    struct net_buf *buf;

    BT_DBG("opcode 0x%04x param_len %u", opcode, param_len);

    buf = net_buf_alloc(&hci_cmd_pool, K_FOREVER);
    __ASSERT_NO_MSG(buf);

    BT_DBG("buf %p", buf);

	net_buf_reserve(buf, BT_BUF_RESERVE);

    bt_buf_set_type(buf, BT_BUF_CMD);

	cmd(buf)->opcode = opcode;
	cmd(buf)->sync = NULL;
	cmd(buf)->state = NULL;

	hdr = net_buf_add(buf, sizeof(*hdr));
	hdr->opcode = sys_cpu_to_le16(opcode);
	hdr->param_len = param_len;

	return buf;
}

int bt_hci_cmd_send(u16_t opcode, struct net_buf *buf)
{
	if (!buf) {
		buf = bt_hci_cmd_create(opcode, 0);
		if (!buf) {
			return -ENOBUFS;
		}
	}

	BT_DBG("opcode 0x%04x len %u", opcode, buf->len);

    /* Host Number of Completed Packets can ignore the ncmd value
     * and does not generate any cmd complete/status events.
     */
    if (opcode == BT_HCI_OP_HOST_NUM_COMPLETED_PACKETS) {
        int err;

        err = bt_send(buf);
        if (err) {
            BT_ERR("Unable to send to driver (err %d)", err);
            net_buf_unref(buf);
        }

        return err;
    }

    net_buf_put(&cmd_tx_queue, buf);

    return 0;
}

int bt_hci_cmd_send_sync(u16_t opcode, struct net_buf *buf,
			 struct net_buf **rsp)
{
	struct k_sem sync_sem;
	u8_t status;
	int err;

	if (!buf) {
		buf = bt_hci_cmd_create(opcode, 0);
		if (!buf) {
			return -ENOBUFS;
		}
	}

	BT_DBG("buf %p opcode 0x%04x len %u", buf, opcode, buf->len);

	k_sem_init(&sync_sem, 0, 1);
	cmd(buf)->sync = &sync_sem;

	/* Make sure the buffer stays around until the command completes */
	net_buf_ref(buf);

	net_buf_put(&cmd_tx_queue, buf);

	err = k_sem_take(&sync_sem, HCI_CMD_TIMEOUT);
	BT_ASSERT_MSG(err == 0, "k_sem_take failed with err %d", err);

    k_sem_delete(&sync_sem);

	status = cmd(buf)->status;
	if (status) {
		BT_WARN("opcode 0x%04x status 0x%02x", opcode, status);
		net_buf_unref(buf);

		switch (status) {
		case BT_HCI_ERR_CONN_LIMIT_EXCEEDED:
			return -ECONNREFUSED;
		default:
			return -EIO;
		}
	}

	BT_DBG("rsp %p opcode 0x%04x len %u", buf, opcode, buf->len);

	if (rsp) {
		*rsp = buf;
	} else {
		net_buf_unref(buf);
	}

	return 0;
}

__hci_api_weak__ int hci_api_le_scan_enable(u8_t enable, u8_t filter_dup)
{
    struct bt_hci_cp_le_set_scan_enable *cp;
    struct net_buf *buf;
    int err;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_SCAN_ENABLE, sizeof(*cp));

    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));

    if (enable == BT_HCI_LE_SCAN_ENABLE) {
        cp->filter_dup = atomic_test_bit(bt_dev.flags,
                                         BT_DEV_SCAN_FILTER_DUP);
    } else {
        cp->filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_DISABLE;
    }

    cp->enable = enable;

    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_SCAN_ENABLE, buf, NULL);

    return err;
}

__hci_api_weak__  int hci_api_le_scan_param_set(u8_t scan_type, u16_t interval, u16_t window, u8_t addr_type, u8_t filter_policy)
{
    struct bt_hci_cp_le_set_scan_param set_param;
    struct net_buf *buf;

    memset(&set_param, 0, sizeof(set_param));

    set_param.scan_type = scan_type;
    set_param.addr_type = addr_type;

    /* for the rest parameters apply default values according to
     *  spec 4.2, vol2, part E, 7.8.10
     */
    set_param.interval = sys_cpu_to_le16(interval);
    set_param.window = sys_cpu_to_le16(window);
    set_param.filter_policy = filter_policy;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_SCAN_PARAM, sizeof(set_param));

    if (!buf) {
        return -ENOBUFS;
    }

    net_buf_add_mem(buf, &set_param, sizeof(set_param));

    bt_hci_cmd_send(BT_HCI_OP_LE_SET_SCAN_PARAM, buf);

    return 0;
}

__hci_api_weak__  int hci_api_le_get_max_data_len(uint16_t *tx_octets, uint16_t *tx_time)
{
    struct bt_hci_rp_le_read_max_data_len *rp;
    struct net_buf *rsp;
    int err;

    if (tx_octets == NULL || tx_time == NULL) {
        return -EINVAL;
    }

    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_READ_MAX_DATA_LEN, NULL, &rsp);

    if (err) {
        return err;
    }

    rp = (void *)rsp->data;
    *tx_octets = sys_le16_to_cpu(rp->max_tx_octets);
    *tx_time = sys_le16_to_cpu(rp->max_tx_time);
    net_buf_unref(rsp);
    return 0;
}

__hci_api_weak__  int hci_api_le_get_default_data_len(uint16_t *tx_octets, uint16_t *tx_time)
{
    struct bt_hci_rp_le_read_default_data_len *rp;
    struct net_buf *rsp;
    int err;

    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_READ_DEFAULT_DATA_LEN, NULL, &rsp);

    if (err) {
        return err;
    }

    rp = (void *)rsp->data;
    *tx_octets = sys_le16_to_cpu(rp->max_tx_octets);
    *tx_time = sys_le16_to_cpu(rp->max_tx_time);
    net_buf_unref(rsp);

    return 0;
}

__hci_api_weak__  int hci_api_le_set_default_data_len(uint16_t tx_octets, uint16_t tx_time)
{
    struct bt_hci_cp_le_write_default_data_len *cp;
    struct net_buf *buf;
    int err;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_WRITE_DEFAULT_DATA_LEN, sizeof(*cp));

    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    cp->max_tx_octets = sys_cpu_to_le16(tx_octets);
    cp->max_tx_time = sys_cpu_to_le16(tx_time);
    err = bt_hci_cmd_send(BT_HCI_OP_LE_WRITE_DEFAULT_DATA_LEN, buf);

    if (err) {
        return err;
    }

    return 0;
}

__hci_api_weak__  int hci_api_le_set_data_len(int16_t conn_handle, uint16_t tx_octets, uint16_t tx_time)
{
    struct bt_hci_cp_le_set_data_len *cp;
    struct net_buf *buf;
    int err;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_DATA_LEN, sizeof(*cp));

    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(conn_handle);
    cp->tx_octets = sys_cpu_to_le16(tx_octets);
    cp->tx_time = sys_cpu_to_le16(tx_time);
    err = bt_hci_cmd_send(BT_HCI_OP_LE_SET_DATA_LEN, buf);

    if (err) {
        return err;
    }

    return 0;
}

static int hci_vs_set_bd_add(uint8_t addr[6])
{
#if defined(CONFIG_BT_HCI_VS_EXT)
    struct net_buf *buf;
    int err;

    buf = bt_hci_cmd_create(BT_HCI_OP_VS_WRITE_BD_ADDR, sizeof(*addr));

    if (!buf) {
        return -ENOBUFS;
    }

    net_buf_add_mem(buf, addr, sizeof(*addr));

    err = bt_hci_cmd_send_sync(BT_HCI_OP_VS_WRITE_BD_ADDR, buf, NULL);

    if (err) {
        return err;
    }

    return 0;
#else
    return -ENOTSUP;
#endif
}

__hci_api_weak__  int hci_api_le_set_bdaddr(uint8_t bdaddr[6])
{
    return hci_vs_set_bd_add(bdaddr);
}

__hci_api_weak__  int hci_api_reset()
{
    int err;
    struct net_buf *rsp;
    u8_t status;
    /* Send HCI_RESET */
    err = bt_hci_cmd_send_sync(BT_HCI_OP_RESET, NULL, &rsp);

    if (err) {
        return err;
    }

    status = rsp->data[0];
    net_buf_unref(rsp);
    return status;
}

__hci_api_weak__  int hci_api_read_local_feature(uint8_t feature[8])
{
    int err;
    struct net_buf *rsp;

    err = bt_hci_cmd_send_sync(BT_HCI_OP_READ_LOCAL_FEATURES, NULL, &rsp);

    if (err) {
        return err;
    }

    struct bt_hci_rp_read_local_features *rp = (void *)rsp->data;

    if (rp->status) {
        net_buf_unref(rsp);
        return rp->status;
    }

    memcpy(feature, rp->features, 8);
    net_buf_unref(rsp);
    return 0;
}

__hci_api_weak__  int hci_api_read_local_version_info(uint8_t   *hci_version,
        uint8_t            *lmp_version,
        uint16_t               *hci_revision,
        uint16_t               *lmp_subversion,
        uint16_t           *manufacturer)
{
    int err;
    struct net_buf *rsp;

    err = bt_hci_cmd_send_sync(BT_HCI_OP_READ_LOCAL_VERSION_INFO, NULL, &rsp);

    if (err) {
        return err;
    }

    struct bt_hci_rp_read_local_version_info *rp = (void *)rsp->data;

    if (rp->status) {
        net_buf_unref(rsp);
        return rp->status;
    }

    *hci_version = rp->hci_version;
    *lmp_version = rp->lmp_version;
    *hci_revision = rp->hci_revision;
    *lmp_subversion = rp->lmp_subversion;
    *manufacturer = rp->manufacturer;
    net_buf_unref(rsp);
    return 0;
}
__hci_api_weak__  int hci_api_read_bdaddr(u8_t bdaddr[6])
{
    int err;
    struct net_buf *rsp;

    err = bt_hci_cmd_send_sync(BT_HCI_OP_READ_BD_ADDR, NULL, &rsp);

    if (err) {
        return err;
    }

    struct bt_hci_rp_read_bd_addr *rp = (void *)rsp->data;

    if (rp->status) {
        net_buf_unref(rsp);
        return rp->status;
    }

    if (!bt_addr_cmp(&rp->bdaddr, BT_ADDR_ANY) ||
        !bt_addr_cmp(&rp->bdaddr, BT_ADDR_NONE)) {
        net_buf_unref(rsp);
        return -ENODATA;
    }

    memcpy(bdaddr, &rp->bdaddr, 6);
    net_buf_unref(rsp);
    return 0;
}

__hci_api_weak__  int hci_api_read_local_support_command(uint8_t supported_commands[64])
{
    int err;
    struct net_buf *rsp;

    err = bt_hci_cmd_send_sync(BT_HCI_OP_READ_SUPPORTED_COMMANDS, NULL, &rsp);

    if (err) {
        return err;
    }

    struct bt_hci_rp_read_supported_commands *rp = (void *)rsp->data;

    if (rp->status) {
        net_buf_unref(rsp);
        return rp->status;
    }

    memcpy(supported_commands, rp->commands, 64);
    net_buf_unref(rsp);
    return 0;
}

__hci_api_weak__  int hci_api_set_host_buffer_size(uint16_t acl_mtu, uint8_t  sco_mtu,
        uint16_t acl_pkts, uint16_t sco_pkts)
{
    struct bt_hci_cp_host_buffer_size *hbs;
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_HOST_BUFFER_SIZE, sizeof(*hbs));

    if (!buf) {
        return -ENOBUFS;
    }

    hbs = net_buf_add(buf, sizeof(*hbs));
    memset(hbs, 0, sizeof(*hbs));
    hbs->acl_mtu = sys_cpu_to_le16(acl_mtu);
    hbs->acl_pkts = sys_cpu_to_le16(acl_pkts);

    return bt_hci_cmd_send_sync(BT_HCI_OP_HOST_BUFFER_SIZE, buf, NULL);
}

__hci_api_weak__  int hci_api_set_host_flow_enable(uint8_t enable)
{
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_SET_CTL_TO_HOST_FLOW, 1);

    if (!buf) {
        return -ENOBUFS;
    }

    net_buf_add_u8(buf, enable);

    return bt_hci_cmd_send_sync(BT_HCI_OP_SET_CTL_TO_HOST_FLOW, buf, NULL);
}

__hci_api_weak__  int hci_api_le_read_local_feature(uint8_t feature[8])
{
    int err;
    struct net_buf *rsp;

    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_READ_LOCAL_FEATURES, NULL, &rsp);

    if (err) {
        return err;
    }

    struct bt_hci_rp_le_read_local_features *rp = (void *)rsp->data;

    if (rp->status) {
        net_buf_unref(rsp);
        return rp->status;
    }

    memcpy(feature, rp->features, 8);
    net_buf_unref(rsp);
    return 0;
}

__hci_api_weak__  int hci_api_le_read_buffer_size_complete(uint16_t *le_max_len, uint8_t *le_max_num)
{
    int err;
    struct net_buf *rsp;

    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_READ_BUFFER_SIZE, NULL, &rsp);

    if (err) {
        return err;
    }

    struct bt_hci_rp_le_read_buffer_size *rp = (void *)rsp->data;

    if (rp->status) {
        net_buf_unref(rsp);
        return rp->status;
    }

    *le_max_len = sys_le16_to_cpu(rp->le_max_len);
    *le_max_num = rp->le_max_num;

    net_buf_unref(rsp);
    return 0;
}

__hci_api_weak__  int hci_api_le_read_support_states(uint64_t *states)
{
    int err;
    struct net_buf *rsp;

    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_READ_SUPP_STATES, NULL, &rsp);

    if (err) {
        return err;
    }

    struct bt_hci_rp_le_read_supp_states *rp = (void *)rsp->data;

    if (rp->status) {
        net_buf_unref(rsp);
        return rp->status;
    }

    *states = sys_get_le64(rp->le_states);
    net_buf_unref(rsp);
    return 0;
}

__hci_api_weak__  int hci_api_le_read_rl_size(uint8_t *rl_size)
{
    int err;
    struct net_buf *rsp;

    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_READ_RL_SIZE, NULL, &rsp);

    if (err) {
        return err;
    }

    struct bt_hci_rp_le_read_rl_size *rp = (void *)rsp->data;

    if (rp->status) {
        net_buf_unref(rsp);
        return rp->status;
    }

    *rl_size = rp->rl_size;
    net_buf_unref(rsp);
    return 0;
}

__hci_api_weak__  int hci_api_le_set_event_mask(uint64_t mask)
{
    struct bt_hci_cp_le_set_event_mask *cp_mask;
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_EVENT_MASK, sizeof(*cp_mask));

    if (!buf) {
        return -ENOBUFS;
    }

    cp_mask = net_buf_add(buf, sizeof(*cp_mask));
    sys_put_le64(mask, cp_mask->events);
    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_EVENT_MASK, buf, NULL);
}

__hci_api_weak__  int hci_api_set_event_mask(uint64_t mask)
{
    struct bt_hci_cp_set_event_mask *ev;
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_SET_EVENT_MASK, sizeof(*ev));

    if (!buf) {
        return -ENOBUFS;
    }

    ev = net_buf_add(buf, sizeof(*ev));
    sys_put_le64(mask, ev->events);
    return bt_hci_cmd_send_sync(BT_HCI_OP_SET_EVENT_MASK, buf, NULL);
}

__hci_api_weak__ int hci_api_num_complete_packets(u8_t num_handles,
        u16_t   *handles,
        u16_t   *counts)
{
    int i;
    for (i = 0; i < num_handles; i++) {
        u16_t handle, count;

        handle = handles[i];
        count = counts[i];

        BT_DBG("handle %u count %u", handle, count);

        extern void bt_hci_num_complete_packets(u16_t handle, u16_t count);
        bt_hci_num_complete_packets(handle, count);
    }
    return 0;
}

__hci_api_weak__ int hci_api_white_list_size(uint8_t *size)
{
    int err;
    struct net_buf *rsp;
    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_READ_WL_SIZE, NULL, &rsp);

    if (err) {
        return err;
    }

    struct bt_hci_rp_le_read_wl_size *rp = (void *)rsp->data;

    if (rp->status) {
        net_buf_unref(rsp);
        return rp->status;
    }

    *size = rp->wl_size;

    net_buf_unref(rsp);
    return 0;
}


__hci_api_weak__ int hci_api_white_list_add(uint8_t peer_addr_type, uint8_t peer_addr[6])
{
    struct  bt_hci_cp_le_add_dev_to_wl *wl_addr;
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_ADD_DEV_TO_WL, sizeof(*wl_addr));

    if(!buf) {
        return -ENOBUFS;
    }

    wl_addr = net_buf_add(buf, sizeof(*wl_addr));

    wl_addr->addr.type = peer_addr_type;
    memcpy(wl_addr->addr.a.val,peer_addr,sizeof(wl_addr->addr.a.val));

    return  bt_hci_cmd_send_sync(BT_HCI_OP_LE_ADD_DEV_TO_WL, buf, NULL);
}

__hci_api_weak__ int hci_api_white_list_remove(uint8_t peer_addr_type, uint8_t peer_addr[6])
{
    struct  bt_hci_cp_le_add_dev_to_wl *wl_addr;
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_REM_DEV_FROM_WL, sizeof(*wl_addr));

    if(!buf) {
        return -ENOBUFS;
    }

    wl_addr = net_buf_add(buf, sizeof(*wl_addr));

    wl_addr->addr.type = peer_addr_type;
    memcpy(wl_addr->addr.a.val,peer_addr,sizeof(wl_addr->addr.a.val));

    return  bt_hci_cmd_send_sync(BT_HCI_OP_LE_REM_DEV_FROM_WL, buf, NULL);
}

__hci_api_weak__ int hci_api_white_list_clear()
{
    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_CLEAR_WL, NULL, NULL);
}


#if defined(CONFIG_BT_HCI_VS_EXT)
#if defined(CONFIG_BT_DEBUG)
static const char *vs_hw_platform(u16_t platform)
{
    static const char *const plat_str[] = {
        "reserved", "Intel Corporation", "Nordic Semiconductor",
        "NXP Semiconductors"
    };

    if (platform < ARRAY_SIZE(plat_str)) {
        return plat_str[platform];
    }

    return "unknown";
}

static const char *vs_hw_variant(u16_t platform, u16_t variant)
{
    static const char *const nordic_str[] = {
        "reserved", "nRF51x", "nRF52x"
    };

    if (platform != BT_HCI_VS_HW_PLAT_NORDIC) {
        return "unknown";
    }

    if (variant < ARRAY_SIZE(nordic_str)) {
        return nordic_str[variant];
    }

    return "unknown";
}

static const char *vs_fw_variant(u8_t variant)
{
    static const char *const var_str[] = {
        "Standard Bluetooth controller",
        "Vendor specific controller",
        "Firmware loader",
        "Rescue image",
    };

    if (variant < ARRAY_SIZE(var_str)) {
        return var_str[variant];
    }

    return "unknown";
}
#endif /* CONFIG_BT_DEBUG */

static void hci_vs_init(void)
{
    union {
        struct bt_hci_rp_vs_read_version_info *info;
        struct bt_hci_rp_vs_read_supported_commands *cmds;
        struct bt_hci_rp_vs_read_supported_features *feat;
    } rp;
    struct net_buf *rsp;
    int err;

    /* If heuristics is enabled, try to guess HCI VS support by looking
     * at the HCI version and identity address. We haven't tried to set
     * a static random address yet at this point, so the identity will
     * either be zeroes or a valid public address.
     */
    if (IS_ENABLED(CONFIG_BT_HCI_VS_EXT_DETECT) &&
        (bt_dev.hci_version < BT_HCI_VERSION_5_0 ||
         (!atomic_test_bit(bt_dev.flags, BT_DEV_USER_ID_ADDR) &&
          bt_addr_le_cmp(&bt_dev.id_addr[0], BT_ADDR_LE_ANY)))) {
        BT_WARN("Controller doesn't seem to support Zephyr vendor HCI");
        return;
    }

    err = bt_hci_cmd_send_sync(BT_HCI_OP_VS_READ_VERSION_INFO, NULL, &rsp);

    if (err) {
        BT_WARN("Vendor HCI extensions not available");
        return;
    }

#if defined(CONFIG_BT_DEBUG)
    rp.info = (void *)rsp->data;
    BT_INFO("HW Platform: %s (0x%04x)",
            vs_hw_platform(sys_le16_to_cpu(rp.info->hw_platform)),
            sys_le16_to_cpu(rp.info->hw_platform));
    BT_INFO("HW Variant: %s (0x%04x)",
            vs_hw_variant(sys_le16_to_cpu(rp.info->hw_platform),
                          sys_le16_to_cpu(rp.info->hw_variant)),
            sys_le16_to_cpu(rp.info->hw_variant));
    BT_INFO("Firmware: %s (0x%02x) Version %u.%u Build %u",
            vs_fw_variant(rp.info->fw_variant), rp.info->fw_variant,
            rp.info->fw_version, sys_le16_to_cpu(rp.info->fw_revision),
            sys_le32_to_cpu(rp.info->fw_build));
#endif /* CONFIG_BT_DEBUG */

    net_buf_unref(rsp);

    err = bt_hci_cmd_send_sync(BT_HCI_OP_VS_READ_SUPPORTED_COMMANDS,
                               NULL, &rsp);

    if (err) {
        BT_WARN("Failed to read supported vendor features");
        return;
    }

    rp.cmds = (void *)rsp->data;
    memcpy(bt_dev.vs_commands, rp.cmds->commands, BT_DEV_VS_CMDS_MAX);
    net_buf_unref(rsp);

    err = bt_hci_cmd_send_sync(BT_HCI_OP_VS_READ_SUPPORTED_FEATURES,
                               NULL, &rsp);

    if (err) {
        BT_WARN("Failed to read supported vendor commands");
        return;
    }

    rp.feat = (void *)rsp->data;
    memcpy(bt_dev.vs_features, rp.feat->features, BT_DEV_VS_FEAT_MAX);
    net_buf_unref(rsp);
}

#endif /* CONFIG_BT_HCI_VS_EXT */

__hci_api_weak__  int hci_api_vs_init()
{
#if defined(CONFIG_BT_HCI_VS_EXT)
    hci_vs_init();
    return 0;
#else
    return 0;
#endif
}

__hci_api_weak__  int hci_api_le_adv_enable(uint8_t enable)
{
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_ADV_ENABLE, 1);

    if (!buf) {
        return -ENOBUFS;
    }

    net_buf_add_u8(buf, enable);

    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_ADV_ENABLE, buf, NULL);
}

__hci_api_weak__  int hci_api_le_adv_param(uint16_t        min_interval,
        uint16_t        max_interval,
        uint8_t         type,
        uint8_t         own_addr_type,
        uint8_t  direct_addr_type,
        uint8_t  direct_addr[6],
        uint8_t         channel_map,
        uint8_t         filter_policy)
{
    struct bt_hci_cp_le_set_adv_param set_param;
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_ADV_PARAM, sizeof(set_param));

    if (!buf) {
        return -ENOBUFS;
    }

    set_param.min_interval = min_interval;
    set_param.max_interval = max_interval;
    set_param.type = type;
    set_param.own_addr_type = own_addr_type;
    set_param.direct_addr.type = direct_addr_type;
    memcpy(set_param.direct_addr.a.val, direct_addr, 6);
    set_param.channel_map = channel_map;
    set_param.filter_policy = filter_policy;
    net_buf_add_mem(buf, &set_param, sizeof(set_param));

    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_ADV_PARAM, buf, NULL);
}

__hci_api_weak__  int hci_api_le_set_random_addr(const uint8_t addr[6])
{
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_RANDOM_ADDRESS, 6);

    if (!buf) {
        return -ENOBUFS;
    }

    net_buf_add_mem(buf, addr, 6);

    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_RANDOM_ADDRESS, buf, NULL);
}

__hci_api_weak__  int hci_api_le_set_ad_data(uint8_t len, uint8_t data[31])
{
    struct bt_hci_cp_le_set_adv_data *set_data;
    struct net_buf *buf;
    buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_ADV_DATA, sizeof(*set_data));

    if (!buf) {
        return -ENOBUFS;
    }

    set_data = net_buf_add(buf, sizeof(*set_data));
    set_data->len = len;
    memcpy(set_data->data, data, 31);
    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_ADV_DATA, buf, NULL);
}

__hci_api_weak__  int hci_api_le_set_sd_data(uint8_t len, uint8_t data[31])
{
    struct bt_hci_cp_le_set_adv_data *set_data;
    struct net_buf *buf;
    buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_SCAN_RSP_DATA, sizeof(*set_data));

    if (!buf) {
        return -ENOBUFS;
    }

    set_data = net_buf_add(buf, sizeof(*set_data));
    set_data->len = len;
    memcpy(set_data->data, data, 31);
    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_SCAN_RSP_DATA, buf, NULL);
}

__hci_api_weak__ int hci_api_le_create_conn(uint16_t scan_interval,
        uint16_t scan_window,
        uint8_t filter_policy,
        uint8_t peer_addr_type,
        const uint8_t peer_addr[6],
        uint8_t own_addr_type,
        uint16_t conn_interval_min,
        uint16_t conn_interval_max,
        uint16_t conn_latency,
        uint16_t supervision_timeout,
        uint16_t min_ce_len,
        uint16_t max_ce_len)
{
    struct net_buf *buf;
    struct bt_hci_cp_le_create_conn *cp;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_CREATE_CONN, sizeof(*cp));

    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    memset(cp, 0, sizeof(*cp));

    /* Interval == window for continuous scanning */
    cp->scan_interval = sys_cpu_to_le16(scan_interval);
    cp->scan_window = cp->scan_interval;

    cp->peer_addr.type = peer_addr_type;
    memcpy(cp->peer_addr.a.val, peer_addr, 6);
    cp->own_addr_type = own_addr_type;
    cp->conn_interval_min = sys_cpu_to_le16(conn_interval_min);
    cp->conn_interval_max = sys_cpu_to_le16(conn_interval_max);
    cp->conn_latency = sys_cpu_to_le16(conn_latency);
    cp->supervision_timeout = sys_cpu_to_le16(supervision_timeout);

    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_CREATE_CONN, buf, NULL);
}

__hci_api_weak__ int hci_api_le_create_conn_cancel()
{
    return bt_hci_cmd_send(BT_HCI_OP_LE_CREATE_CONN_CANCEL,
                           NULL);
}

__hci_api_weak__ int hci_api_le_disconnect(uint16_t conn_hanlde, uint8_t reason)
{
    struct net_buf *buf;
    struct bt_hci_cp_disconnect *disconn;

    buf = bt_hci_cmd_create(BT_HCI_OP_DISCONNECT, sizeof(*disconn));

    if (!buf) {
        return -ENOBUFS;
    }

    disconn = net_buf_add(buf, sizeof(*disconn));
    disconn->handle = sys_cpu_to_le16(conn_hanlde);
    disconn->reason = reason;

    return bt_hci_cmd_send(BT_HCI_OP_DISCONNECT, buf);
}

__hci_api_weak__ int hci_api_le_read_remote_features(uint16_t conn_handle)
{
    struct bt_hci_cp_le_read_remote_features *cp;
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_READ_REMOTE_FEATURES,
                            sizeof(*cp));

    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(conn_handle);
    return bt_hci_cmd_send(BT_HCI_OP_LE_READ_REMOTE_FEATURES, buf);
}

__hci_api_weak__ int hci_api_le_read_remote_version(uint16_t conn_handle)
{
	struct bt_hci_cp_read_remote_version_info *cp;
	struct net_buf *buf;

	buf = bt_hci_cmd_create(BT_HCI_OP_READ_REMOTE_VERSION_INFO,
				sizeof(*cp));
	if (!buf) {
		return -ENOBUFS;
	}

	cp = net_buf_add(buf, sizeof(*cp));
	cp->handle = sys_cpu_to_le16(conn_handle);

	return bt_hci_cmd_send_sync(BT_HCI_OP_READ_REMOTE_VERSION_INFO, buf,
				    NULL);
}

__hci_api_weak__ int hci_api_host_num_complete_packets(uint8_t num_handles, struct handle_count *phc)
{
    struct net_buf *buf;
    struct bt_hci_cp_host_num_completed_packets *cp;
    struct bt_hci_handle_count *hc;

    buf = bt_hci_cmd_create(BT_HCI_OP_HOST_NUM_COMPLETED_PACKETS,
                            sizeof(*cp) + sizeof(*hc));

    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    cp->num_handles = sys_cpu_to_le16(num_handles);

    hc = net_buf_add(buf, sizeof(*hc));
    hc->handle = sys_cpu_to_le16(phc[0].handle);
    hc->count  = sys_cpu_to_le16(phc[0].count);

    return bt_hci_cmd_send(BT_HCI_OP_HOST_NUM_COMPLETED_PACKETS, buf);
}

__hci_api_weak__ int hci_api_le_conn_updata(uint16_t conn_handle,
        uint16_t conn_interval_min,
        uint16_t conn_interval_max,
        uint16_t conn_latency,
        uint16_t supervision_timeout,
        uint16_t min_ce_len,
        uint16_t max_ce_len)
{
    struct hci_cp_le_conn_update *conn_update;
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_CONN_UPDATE,
                            sizeof(*conn_update));

    if (!buf) {
        return -ENOBUFS;
    }

    conn_update = net_buf_add(buf, sizeof(*conn_update));
    memset(conn_update, 0, sizeof(*conn_update));
    conn_update->handle = sys_cpu_to_le16(conn_handle);
    conn_update->conn_interval_min = sys_cpu_to_le16(conn_interval_min);
    conn_update->conn_interval_max = sys_cpu_to_le16(conn_interval_max);
    conn_update->conn_latency = sys_cpu_to_le16(conn_latency);
    conn_update->supervision_timeout = sys_cpu_to_le16(supervision_timeout);

    return bt_hci_cmd_send(BT_HCI_OP_LE_CONN_UPDATE, buf);
}

__hci_api_weak__ int hci_api_le_start_encrypt(uint16_t conn_handle,
        u8_t rand[8],
        u8_t ediv[2],
        uint8_t  ltk[16])
{
    struct bt_hci_cp_le_start_encryption *cp;
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_START_ENCRYPTION, sizeof(*cp));

    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(conn_handle);
    memcpy(&cp->rand, rand, sizeof(cp->rand));
    memcpy(&cp->ediv, ediv, sizeof(cp->ediv));
    memcpy(cp->ltk, ltk, sizeof(cp->ltk));

    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_START_ENCRYPTION, buf, NULL);
}

__hci_api_weak__ int hci_api_le_enctypt_ltk_req_reply(uint16_t conn_handle, uint8_t  ltk[16])
{
    struct net_buf *buf;
    struct bt_hci_cp_le_ltk_req_reply *cp;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_LTK_REQ_REPLY,
                            sizeof(*cp));

    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = conn_handle;

    memcpy(cp->ltk, ltk, 16);

    return bt_hci_cmd_send(BT_HCI_OP_LE_LTK_REQ_REPLY, buf);
}

__hci_api_weak__ int hci_api_le_enctypt_ltk_req_neg_reply(uint16_t conn_handle)
{
    struct bt_hci_cp_le_ltk_req_neg_reply *cp;
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_LTK_REQ_NEG_REPLY, sizeof(*cp));

    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = conn_handle;

    return bt_hci_cmd_send(BT_HCI_OP_LE_LTK_REQ_NEG_REPLY, buf);
}

__hci_api_weak__ int hci_api_le_rand(uint8_t random_data[8])
{
    struct bt_hci_rp_le_rand *rp;
    struct net_buf *rsp;
    int err;
    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_RAND, NULL, &rsp);

    if (err) {
        return err;
    }

    rp = (void *)rsp->data;
    memcpy(random_data, rp->rand, sizeof(rp->rand));
    return 0;
}

__hci_api_weak__ int hci_api_le_enc(uint8_t key[16], uint8_t plaintext[16], uint8_t ciphertext[16])
{
    struct bt_hci_cp_le_encrypt *cp;
    struct bt_hci_rp_le_encrypt *rp;
    int err;
    struct net_buf *buf;
    struct net_buf *rsp;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_ENCRYPT, sizeof(*cp));

    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    memcpy(cp->key, key, sizeof(cp->key));
    memcpy(cp->plaintext, plaintext, sizeof(cp->plaintext));

    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_ENCRYPT, buf, &rsp);

    if (err) {
        return err;
    }

    rp = (void *)rsp->data;
    memcpy(ciphertext, rp->enc_data, sizeof(rp->enc_data));

    return 0;
}

__hci_api_weak__ int hci_api_le_set_phy(u16_t  handle,
                                        u8_t   all_phys,
                                        u8_t   tx_phys,
                                        u8_t   rx_phys,
                                        u16_t  phy_opts)
{
    struct bt_hci_cp_le_set_phy *cp;
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_PHY, sizeof(*cp));

    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(handle);
    cp->all_phys = all_phys;
    cp->tx_phys = tx_phys;
    cp->rx_phys = rx_phys;
    cp->phy_opts = phy_opts;
    return bt_hci_cmd_send(BT_HCI_OP_LE_SET_PHY, buf);
}

__hci_api_weak__ int hci_api_le_conn_param_req_reply(uint16_t handle,
        uint16_t interval_min,
        uint16_t interval_max,
        uint16_t latency,
        uint16_t timeout,
        uint16_t min_ce_len,
        uint16_t max_ce_len)
{
    struct bt_hci_cp_le_conn_param_req_reply *cp;
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_CONN_PARAM_REQ_REPLY, sizeof(*cp));

    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    memset(cp, 0, sizeof(*cp));

    cp->handle = sys_cpu_to_le16(handle);
    cp->interval_min = sys_cpu_to_le16(interval_min);
    cp->interval_max = sys_cpu_to_le16(interval_max);
    cp->latency = sys_cpu_to_le16(latency);
    cp->timeout = sys_cpu_to_le16(timeout);

    return bt_hci_cmd_send(BT_HCI_OP_LE_CONN_PARAM_REQ_REPLY, buf);
}


__hci_api_weak__ int hci_api_le_conn_param_neg_reply(uint16_t handle,
        uint8_t reason)
{
    struct bt_hci_cp_le_conn_param_req_neg_reply *cp;
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_CONN_PARAM_REQ_NEG_REPLY,
                            sizeof(*cp));

    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(handle);
    cp->reason = sys_cpu_to_le16(reason);

    return bt_hci_cmd_send(BT_HCI_OP_LE_CONN_PARAM_REQ_NEG_REPLY, buf);
}

__hci_api_weak__ int hci_api_le_add_dev_to_rl(uint8_t type,
        uint8_t peer_id_addr[6],
        uint8_t peer_irk[16],
        uint8_t local_irk[16])
{
    struct bt_hci_cp_le_add_dev_to_rl *cp;
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_ADD_DEV_TO_RL, sizeof(*cp));

    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    memset(cp, 0, sizeof(*cp));

    cp->peer_id_addr.type = type;
    memcpy(cp->peer_id_addr.a.val, peer_id_addr, 6);

    if (peer_irk) {
        memcpy(cp->peer_irk, peer_irk, 16);
    }

    if (local_irk) {
        memcpy(cp->local_irk, local_irk, 16);
    }

    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_ADD_DEV_TO_RL, buf, NULL);
}

__hci_api_weak__ int hci_api_le_remove_dev_from_rl(uint8_t type,
        const uint8_t peer_id_addr[6])
{
    struct bt_hci_cp_le_rem_dev_from_rl *cp;
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_REM_DEV_FROM_RL, sizeof(*cp));

    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    memset(cp, 0, sizeof(*cp));

    cp->peer_id_addr.type = type;
    memcpy(cp->peer_id_addr.a.val, peer_id_addr, 6);

    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_REM_DEV_FROM_RL, buf, NULL);
}

__hci_api_weak__ int hci_api_le_clear_rl()
{
    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_CLEAR_RL, NULL, NULL);
}

__hci_api_weak__ int hci_api_le_set_addr_res_enable(uint8_t enable)
{
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_ADDR_RES_ENABLE, 1);

    if (!buf) {
        return -ENOBUFS;
    }

    net_buf_add_u8(buf, enable);

    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_ADDR_RES_ENABLE,
                                buf, NULL);
}

__hci_api_weak__ int hci_api_le_set_privacy_mode(uint8_t type,
        uint8_t id_addr[6],
        uint8_t mode)
{
    struct bt_hci_cp_le_set_privacy_mode cp;
    struct net_buf *buf;

    cp.id_addr.type = type;
    memcpy(cp.id_addr.a.val, id_addr, 6);

    cp.mode = mode;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_PRIVACY_MODE, sizeof(cp));

    if (!buf) {
        return -ENOBUFS;
    }

    net_buf_add_mem(buf, &cp, sizeof(cp));

    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_PRIVACY_MODE, buf, NULL);
}

__hci_api_weak__ int hci_api_le_gen_p256_pubkey()
{
    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_P256_PUBLIC_KEY, NULL, NULL);
}

__hci_api_weak__ int hci_api_le_gen_dhkey(uint8_t remote_pk[64])
{
    struct bt_hci_cp_le_generate_dhkey *cp;
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_GENERATE_DHKEY, sizeof(*cp));

    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    memcpy(cp->key, remote_pk, sizeof(cp->key));

    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_GENERATE_DHKEY, buf, NULL);
}

__hci_api_weak__ int hci_api_read_buffer_size(uint16_t *acl_max_len,
        uint8_t  *sco_max_len,
        uint16_t *acl_max_num,
        uint16_t *sco_max_num)
{
    struct bt_hci_rp_read_buffer_size *rp;
    struct net_buf *rsp;
    int err;

    /* Use BR/EDR buffer size if LE reports zero buffers */
    err = bt_hci_cmd_send_sync(BT_HCI_OP_READ_BUFFER_SIZE, NULL, &rsp);

    if (err) {
        return err;
    }

    rp = (void *)rsp->data;

    if (rp->status) {
        net_buf_unref(rsp);
        return rp->status;
    }

    *acl_max_len = sys_le16_to_cpu(rp->acl_max_len);
    *sco_max_len = rp->sco_max_len;
    *acl_max_num = sys_le16_to_cpu(rp->acl_max_num);
    *sco_max_num = sys_le16_to_cpu(rp->sco_max_num);
    net_buf_unref(rsp);

    return 0;
}

__hci_api_weak__ int hci_api_le_write_host_supp(uint8_t le, uint8_t simul)
{
    struct bt_hci_cp_write_le_host_supp *cp_le;
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_WRITE_LE_HOST_SUPP,
                            sizeof(*cp_le));

    if (!buf) {
        return -ENOBUFS;
    }

    cp_le = net_buf_add(buf, sizeof(*cp_le));

    /* Explicitly enable LE for dual-mode controllers */
    cp_le->le = le;
    cp_le->simul = simul;
    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_WRITE_LE_HOST_SUPP, buf, NULL);
}

static void process_events(struct k_poll_event *ev, int count)
{
	BT_DBG("count %d", count);

	for (; count; ev++, count--) {
		BT_DBG("ev->state %u", ev->state);

		switch (ev->state) {
		case K_POLL_STATE_SIGNALED:
			break;
		case K_POLL_STATE_FIFO_DATA_AVAILABLE:
			if (ev->tag == BT_EVENT_CMD_TX) {
				send_cmd();
			} else if (IS_ENABLED(CONFIG_BT_CONN)) {
				struct bt_conn *conn;

				if (ev->tag == BT_EVENT_CONN_TX_QUEUE) {
					conn = CONTAINER_OF(ev->fifo,
							    struct bt_conn,
							    tx_queue);
					bt_conn_process_tx(conn);
				}
			}
			break;
		case K_POLL_STATE_NOT_READY:
			break;
		default:
			BT_WARN("Unexpected k_poll event state %u", ev->state);
			break;
		}
	}
}

static void hci_data_buf_overflow(struct net_buf *buf)
{
	struct bt_hci_evt_data_buf_overflow *evt = (void *)buf->data;

	BT_WARN("Data buffer overflow (link type 0x%02x)", evt->link_type);
	(void)evt;
}

static const struct event_handler prio_events[] = {
	EVENT_HANDLER(BT_HCI_EVT_CMD_COMPLETE, hci_cmd_complete,
		      sizeof(struct bt_hci_evt_cmd_complete)),
	EVENT_HANDLER(BT_HCI_EVT_CMD_STATUS, hci_cmd_status,
		      sizeof(struct bt_hci_evt_cmd_status)),
#if defined(CONFIG_BT_CONN)
	EVENT_HANDLER(BT_HCI_EVT_DATA_BUF_OVERFLOW,
		      hci_data_buf_overflow,
		      sizeof(struct bt_hci_evt_data_buf_overflow)),
	EVENT_HANDLER(BT_HCI_EVT_NUM_COMPLETED_PACKETS,
		      hci_num_completed_packets,
		      sizeof(struct bt_hci_evt_num_completed_packets)),
#endif /* CONFIG_BT_CONN */
};

__hci_api_weak__ int bt_recv_prio(struct net_buf *buf)
{
	struct bt_hci_evt_hdr *hdr;

	bt_monitor_send(bt_monitor_opcode(buf), buf->data, buf->len);

	BT_ASSERT(bt_buf_get_type(buf) == BT_BUF_EVT);
	BT_ASSERT(buf->len >= sizeof(*hdr));

    hdr = net_buf_pull_mem(buf, sizeof(*hdr));
	BT_ASSERT(bt_hci_evt_is_prio(hdr->evt));

	handle_event(hdr->evt, buf, prio_events, ARRAY_SIZE(prio_events));

	net_buf_unref(buf);

    return 0;
}

#if defined(CONFIG_BT_CONN)
/* command FIFO + conn_change signal + MAX_CONN * 2 (tx & tx_notify) */
#define EV_COUNT (2 + (CONFIG_BT_MAX_CONN * 2))
#else
/* command FIFO */
#define EV_COUNT 1
#endif

static void hci_tx_thread(void *p1)
{
	static struct k_poll_event events[EV_COUNT] = {
		K_POLL_EVENT_STATIC_INITIALIZER(K_POLL_TYPE_FIFO_DATA_AVAILABLE,
						K_POLL_MODE_NOTIFY_ONLY,
						&cmd_tx_queue,
						BT_EVENT_CMD_TX),
	};

    BT_DBG("Started");

	while (1) {
		int ev_count, err;

		events[0].state = K_POLL_STATE_NOT_READY;
		ev_count = 1;

		if (IS_ENABLED(CONFIG_BT_CONN)) {
			ev_count += bt_conn_prepare_events(&events[1]);
		}

        BT_DBG("Calling k_poll with %d events", ev_count);

#ifndef YULONG_HCI
		err = k_poll(events, ev_count, K_FOREVER);
#endif

		BT_ASSERT(err == 0);

		process_events(events, ev_count);

		/* Make sure we don't hog the CPU if there's all the time
		 * some ready events.
		 */
		k_yield();
	}
}

__hci_api_weak__ int hci_api_init()
{

#if !defined(CONFIG_BT_WAIT_NOP)
    k_sem_init(&ncmd_sem,1,1);
#else
    k_sem_init(&ncmd_sem,0,1);
#endif

    NET_BUF_POOL_INIT(hci_cmd_pool);

    k_fifo_init(&cmd_tx_queue);

#ifdef CONFIG_BT_TINYCRYPT_ECC
    bt_hci_ecc_init();
#endif

    return 0;
}

__hci_api_weak__ int hci_api_le_ext_adv_enable(uint8_t enable, uint16_t set_num, struct ext_adv_set_t adv_sets[])
{
	struct net_buf *buf;
	struct cmd_state_set state;
	int i;
	buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_EXT_ADV_ENABLE, 2 + 4 * set_num);
	if (!buf) {
		return -ENOBUFS;
	}

	net_buf_add_u8(buf, enable);

	net_buf_add_u8(buf, set_num);

	for (i = 0; i < set_num; i++)
	{
		net_buf_add_u8(buf, adv_sets[i].adv_handle);
		net_buf_add_le16(buf,sys_cpu_to_le16(adv_sets[i].duration));
		net_buf_add_u8(buf, adv_sets[i].max_ext_adv_evts);
	}

	//cmd_state_set_init(&state, adv->flags, BT_ADV_ENABLED, enable);
	cmd(buf)->state = &state;

	return  bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_EXT_ADV_ENABLE, buf, NULL);
}

__hci_api_weak__ int hci_api_le_set_adv_random_addr(uint8_t handle, const uint8_t addr[6])
{
    struct bt_hci_cp_le_set_adv_set_random_addr *cp;
	struct net_buf *buf;

	buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_ADV_SET_RANDOM_ADDR,
				sizeof(*cp));
	if (!buf) {
		return -ENOBUFS;
	}

	cp = net_buf_add(buf, sizeof(*cp));

	cp->handle = handle;
	memcpy(&cp->bdaddr, addr, 6);

	return bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_ADV_SET_RANDOM_ADDR, buf,
				   NULL);
}

__hci_api_weak__ int hci_api_le_ext_scan_enable(uint8_t enable,uint8_t filter_dup,uint16_t duration, uint16_t period)
{
    struct bt_hci_cp_le_set_ext_scan_enable *cp;
	struct cmd_state_set state;
	struct net_buf *buf;

	buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_EXT_SCAN_ENABLE, sizeof(*cp));
	if (!buf) {
		return -ENOBUFS;
	}

	cp = net_buf_add(buf, sizeof(*cp));

	cp->enable = enable;
    cp->filter_dup = filter_dup;
	cp->duration = sys_cpu_to_le16(duration);
	cp->period = 0;

	cmd_state_set_init(&state, bt_dev.flags, BT_DEV_SCANNING, enable == BT_HCI_LE_SCAN_ENABLE);
	cmd(buf)->state = &state;

	return bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_EXT_SCAN_ENABLE, buf, NULL);
}

__hci_api_weak__ int hci_api_le_ext_scan_param_set(uint8_t own_addr_type,uint8_t filter_policy,uint8_t scan_phys, struct ext_scan_param_t params[])
{
    struct bt_hci_cp_le_set_ext_scan_param *set_param;
	struct net_buf *buf;

    u8_t phys_num = (scan_phys & BT_HCI_LE_EXT_SCAN_PHY_1M)? 1 : 0 + (scan_phys & BT_HCI_LE_EXT_SCAN_PHY_CODED)? 1 : 0;

	buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_EXT_SCAN_PARAM,
				sizeof(*set_param) +
				phys_num * sizeof(struct bt_hci_ext_scan_phy));
	if (!buf) {
		return -ENOBUFS;
	}

	set_param = net_buf_add(buf, sizeof(*set_param));
	set_param->own_addr_type = own_addr_type;
	set_param->phys = scan_phys;
    set_param->filter_policy = filter_policy;

	return bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_EXT_SCAN_PARAM, buf, NULL);
}


__hci_api_weak__ int hci_api_le_read_phy(uint16_t conn_handle, 	uint8_t  *tx_phy , uint8_t *rx_phy)
{
    struct bt_hci_cp_le_read_phy *cp;
	struct bt_hci_rp_le_read_phy *rp;
	struct net_buf *buf, *rsp;
	int err;

	buf = bt_hci_cmd_create(BT_HCI_OP_LE_READ_PHY, sizeof(*cp));
	if (!buf) {
		return -ENOBUFS;
	}

	cp = net_buf_add(buf, sizeof(*cp));
	cp->handle = sys_cpu_to_le16(conn_handle);

	err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_READ_PHY, buf, &rsp);
	if (err) {
		return err;
	}

	rp = (void *)rsp->data;
	*tx_phy = rp->tx_phy;
	*rx_phy = rp->rx_phy;
	net_buf_unref(rsp);
    return 0;
}

__hci_api_weak__ int hci_api_le_create_conn_ext(uint8_t filter_policy,
                                      uint8_t own_addr_type,
                                      uint8_t peer_addr_type,
                                      uint8_t peer_addr[6],
                                      uint8_t init_phys,
                                      struct ext_conn_phy_params_t params[])
{
    struct bt_hci_cp_le_ext_create_conn *cp;
    struct bt_hci_ext_conn_phy *phy;
    struct cmd_state_set state;
    struct net_buf *buf;
    u8_t num_phys;
    struct ext_conn_phy_params_t *pparams = params;

    num_phys = (init_phys & BT_HCI_LE_EXT_SCAN_PHY_1M) ? 1:0 + (init_phys & BT_HCI_LE_EXT_SCAN_PHY_CODED) ? 1:0;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_EXT_CREATE_CONN, sizeof(*cp) + num_phys * sizeof(*phy));
    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    (void)memset(cp, 0, sizeof(*cp));
    cp->filter_policy = filter_policy;
    cp->peer_addr.type = peer_addr_type;
    memcpy(cp->peer_addr.a.val, peer_addr, 6);

    cp->own_addr_type = own_addr_type;
    cp->phys = init_phys;

    if (init_phys & BT_HCI_LE_EXT_SCAN_PHY_1M) {
        phy = net_buf_add(buf, sizeof(*phy));
        phy->scan_interval = sys_cpu_to_le16(pparams->scan_interval);
        phy->scan_window = sys_cpu_to_le16(pparams->scan_window);
        phy->conn_interval_min = sys_cpu_to_le16(pparams->conn_interval_min);
        phy->conn_interval_max = sys_cpu_to_le16(pparams->conn_interval_max);
        phy->conn_latency = sys_cpu_to_le16(pparams->conn_latency);
        phy->supervision_timeout = sys_cpu_to_le16(pparams->supervision_timeout);
        phy->min_ce_len = 0;
        phy->max_ce_len = 0;
        pparams++;
    }

    if (init_phys & BT_HCI_LE_EXT_SCAN_PHY_CODED) {
        phy = net_buf_add(buf, sizeof(*phy));
        phy->scan_interval = sys_cpu_to_le16(pparams->scan_interval);
        phy->scan_window = sys_cpu_to_le16(pparams->scan_window);
        phy->conn_interval_min = sys_cpu_to_le16(pparams->conn_interval_min);
        phy->conn_interval_max = sys_cpu_to_le16(pparams->conn_interval_max);
        phy->conn_latency = sys_cpu_to_le16(pparams->conn_latency);
        phy->supervision_timeout = sys_cpu_to_le16(pparams->supervision_timeout);
        phy->min_ce_len = 0;
        phy->max_ce_len = 0;
    }

    cmd_state_set_init(&state, bt_dev.flags, BT_DEV_INITIATING, true);
    cmd(buf)->state = &state;

    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_EXT_CREATE_CONN, buf, NULL);
}

__hci_api_weak__ int hci_api_le_rpa_timeout_set(uint16_t timeout)
{
	struct net_buf *buf;
    struct bt_hci_cp_le_set_rpa_timeout *cp;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_RPA_TIMEOUT,
                sizeof(*cp));
    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    cp->rpa_timeout = sys_cpu_to_le16(timeout);
    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_RPA_TIMEOUT, buf,
                   NULL);
}

__hci_api_weak__ int hci_api_le_set_ext_ad_data(uint8_t handle, uint8_t op, uint8_t  frag_pref, uint8_t  len, uint8_t  data[251])
{
    struct bt_hci_cp_le_set_ext_adv_data *set_data;
	struct net_buf *buf;

	buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_EXT_ADV_DATA, sizeof(*set_data));
	if (!buf) {
		return -ENOBUFS;
	}

	set_data = net_buf_add(buf, sizeof(*set_data));
	(void)memset(set_data, 0, sizeof(*set_data));

	set_data->handle = handle;
	set_data->op = op;
	set_data->frag_pref = frag_pref;
    set_data->len = len;
    memcpy(set_data->data, data, len);

	return bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_EXT_ADV_DATA, buf, NULL);
}

__hci_api_weak__ int hci_api_le_set_ext_sd_data(uint8_t handle, uint8_t op, uint8_t  frag_pref, uint8_t  len, uint8_t  data[251])
{
    struct bt_hci_cp_le_set_ext_scan_rsp_data *set_data;
	struct net_buf *buf;

	buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_EXT_SCAN_RSP_DATA, sizeof(*set_data));
	if (!buf) {
		return -ENOBUFS;
	}

	set_data = net_buf_add(buf, sizeof(*set_data));
	(void)memset(set_data, 0, sizeof(*set_data));

	set_data->handle = handle;
	set_data->op = op;
	set_data->frag_pref = frag_pref;
    set_data->len = len;
    memcpy(set_data->data, data, len);

	return bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_EXT_SCAN_RSP_DATA, buf, NULL);
}

__hci_api_weak__ int hci_api_le_remove_adv_set(uint8_t handle)
{
	struct bt_hci_cp_le_remove_adv_set *cp;
	struct net_buf *buf;

	buf = bt_hci_cmd_create(BT_HCI_OP_LE_REMOVE_ADV_SET, sizeof(*cp));
	if (!buf) {
		BT_WARN("No HCI buffers");
		return -ENOBUFS;
	}

	cp = net_buf_add(buf, sizeof(*cp));
	cp->handle = handle;

	return bt_hci_cmd_send_sync(BT_HCI_OP_LE_REMOVE_ADV_SET, buf, NULL);
}

__hci_api_weak__ int hci_api_le_set_host_chan_classif(u8_t ch_map[5])
{
    struct bt_hci_cp_le_set_host_chan_classif *cp;
	struct net_buf *buf;

	buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_HOST_CHAN_CLASSIF,
				sizeof(*cp));
	if (!buf) {
		return -ENOBUFS;
	}

	cp = net_buf_add(buf, sizeof(*cp));

	memcpy(&cp->ch_map[0], &ch_map[0], 5);

	return bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_HOST_CHAN_CLASSIF,
				    buf, NULL);
}

#endif
