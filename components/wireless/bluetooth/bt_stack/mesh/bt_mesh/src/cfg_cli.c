/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef CONFIG_BT_MESH_CFG_CLI
#include <zephyr.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <zephyr/types.h>
#include <misc/util.h>
#include <misc/byteorder.h>

//#include <conn.h>
#include <api/mesh.h>
#include "../inc/net.h"
#include "beacon.h"
#include "api/mesh/cfg_cli.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"

#include "foundation.h"
//#include "bt_mesh_custom_log.h"

#include "mesh.h"
#define CID_NVAL 0xffff

struct comp_data {
	u8_t *status;
	struct net_buf_simple *comp;
};

static s32_t msg_timeout = K_SECONDS(2);

static struct bt_mesh_cfg_cli *cli;

#if defined(CONFIG_MESH_STACK_FOR_GENIE)
static cfg_cli_callback g_bt_mesh_cfg_cli_cb;
void bt_mesh_cfg_cli_set_callback(cfg_cli_callback callback)
{
     g_bt_mesh_cfg_cli_cb = callback;
}

cfg_cli_callback bt_mesh_cfg_cli_get_callback(void)
{
     return g_bt_mesh_cfg_cli_cb;
}
#endif

static void comp_data_status(struct bt_mesh_model *model,
			     struct bt_mesh_msg_ctx *ctx,
			     struct net_buf_simple *buf)
{
	struct comp_data *param;
	size_t to_copy;

	BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
	       ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
	       bt_hex(buf->data, buf->len));

	if (cli->op_pending != OP_DEV_COMP_DATA_STATUS) {
		BT_WARN("Unexpected Composition Data Status");
		return;
	}

	param = cli->op_param;

	*(param->status) = net_buf_simple_pull_u8(buf);
	to_copy  = min(net_buf_simple_tailroom(param->comp), buf->len);
	net_buf_simple_add_mem(param->comp, buf->data, to_copy);

	k_sem_give(&cli->op_sync);
}

static void state_status_u8(struct bt_mesh_model *model,
			    struct bt_mesh_msg_ctx *ctx,
			    struct net_buf_simple *buf,
			    u32_t expect_status)
{
	u8_t *status;

	BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
	       ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
	       bt_hex(buf->data, buf->len));

	if (cli->op_pending != expect_status) {
		BT_WARN("Unexpected Status (0x%08x != 0x%08x)",
			cli->op_pending, expect_status);
		return;
	}

	status = cli->op_param;
	*status = net_buf_simple_pull_u8(buf);

	k_sem_give(&cli->op_sync);
}

static void beacon_status(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
	state_status_u8(model, ctx, buf, OP_BEACON_STATUS);
}

static void ttl_status(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
	state_status_u8(model, ctx, buf, OP_DEFAULT_TTL_STATUS);
}

static void friend_status(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
	state_status_u8(model, ctx, buf, OP_FRIEND_STATUS);
}

static void gatt_proxy_status(struct bt_mesh_model *model,
			      struct bt_mesh_msg_ctx *ctx,
			      struct net_buf_simple *buf)
{
	state_status_u8(model, ctx, buf, OP_GATT_PROXY_STATUS);
}

struct relay_param {
	u8_t *status;
	u8_t *transmit;
};

static void relay_status(struct bt_mesh_model *model,
			 struct bt_mesh_msg_ctx *ctx,
			 struct net_buf_simple *buf)
{
	struct relay_param *param;

	BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
	       ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
	       bt_hex(buf->data, buf->len));

	if (cli->op_pending != OP_RELAY_STATUS) {
		BT_WARN("Unexpected Relay Status message");
		return;
	}

	param = cli->op_param;
	*param->status = net_buf_simple_pull_u8(buf);
	*param->transmit = net_buf_simple_pull_u8(buf);

	k_sem_give(&cli->op_sync);
}

struct net_key_param {
	u8_t *status;
	u16_t net_idx;
};

static void net_key_status(struct bt_mesh_model *model,
			   struct bt_mesh_msg_ctx *ctx,
			   struct net_buf_simple *buf)
{
	struct net_key_param *param;
	u16_t net_idx, app_idx;
	u8_t status;

	BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
	       ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
	       bt_hex(buf->data, buf->len));

	if (cli->op_pending != OP_NET_KEY_STATUS) {
		BT_WARN("Unexpected Net Key Status message");
		return;
	}

	status = net_buf_simple_pull_u8(buf);
	key_idx_unpack(buf, &net_idx, &app_idx);

	param = cli->op_param;
	if (param->net_idx != net_idx) {
		BT_WARN("Net Key Status key index does not match");
		return;
	}

	*param->status = status;

	k_sem_give(&cli->op_sync);
}

struct app_key_param {
	u8_t *status;
	u16_t net_idx;
	u16_t app_idx;
};

static void app_key_status(struct bt_mesh_model *model,
			   struct bt_mesh_msg_ctx *ctx,
			   struct net_buf_simple *buf)
{
	struct app_key_param *param;
	u16_t net_idx, app_idx;
	u8_t status;

	BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
	       ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
	       bt_hex(buf->data, buf->len));

	if (cli->op_pending != OP_APP_KEY_STATUS) {
		BT_WARN("Unexpected App Key Status message");
		return;
	}

	status = net_buf_simple_pull_u8(buf);
	key_idx_unpack(buf, &net_idx, &app_idx);

	param = cli->op_param;
	if (param->net_idx != net_idx || param->app_idx != app_idx) {
		BT_WARN("App Key Status key indices did not match");
		return;
	}

	*param->status = status;

	k_sem_give(&cli->op_sync);
}

struct mod_app_param {
	u8_t *status;
	u16_t elem_addr;
	u16_t mod_app_idx;
	u16_t mod_id;
	u16_t cid;
};

static void mod_app_status(struct bt_mesh_model *model,
			   struct bt_mesh_msg_ctx *ctx,
			   struct net_buf_simple *buf)
{
	u16_t elem_addr, mod_app_idx, mod_id, cid;
	struct mod_app_param *param;
	u8_t status;

	BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
	       ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
	       bt_hex(buf->data, buf->len));

	if (cli->op_pending != OP_MOD_APP_STATUS) {
		BT_WARN("Unexpected Model App Status message");
		return;
	}

	status = net_buf_simple_pull_u8(buf);
	elem_addr = net_buf_simple_pull_le16(buf);
	mod_app_idx = net_buf_simple_pull_le16(buf);

	if (buf->len >= 4) {
		cid = net_buf_simple_pull_le16(buf);
	} else {
		cid = CID_NVAL;
	}

	mod_id = net_buf_simple_pull_le16(buf);

	param = cli->op_param;
	if (param->elem_addr != elem_addr ||
	    param->mod_app_idx != mod_app_idx || param->mod_id != mod_id ||
	    param->cid != cid) {
		BT_WARN("Model App Status parameters did not match");
		return;
	}

	*param->status = status;

	k_sem_give(&cli->op_sync);
}

struct mod_pub_param {
	u16_t                       mod_id;
	u16_t                       cid;
	u16_t                       elem_addr;
	u8_t                       *status;
	struct bt_mesh_cfg_mod_pub *pub;
};

static void mod_pub_status(struct bt_mesh_model *model,
			   struct bt_mesh_msg_ctx *ctx,
			   struct net_buf_simple *buf)
{
	u16_t mod_id, cid, elem_addr;
	struct mod_pub_param *param;
	u8_t status;

	BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
	       ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
	       bt_hex(buf->data, buf->len));

	if (cli->op_pending != OP_MOD_PUB_STATUS) {
		BT_WARN("Unexpected Model Pub Status message");
		return;
	}

	param = cli->op_param;
	if (param->cid != CID_NVAL) {
		if (buf->len < 14) {
			BT_WARN("Unexpected Mod Pub Status with SIG Model");
			return;
		}

		cid = sys_get_le16(&buf->data[10]);
		mod_id = sys_get_le16(&buf->data[12]);
	} else {
		if (buf->len > 12) {
			BT_WARN("Unexpected Mod Pub Status with Vendor Model");
			return;
		}

		cid = CID_NVAL;
		mod_id = sys_get_le16(&buf->data[10]);
	}

	if (mod_id != param->mod_id || cid != param->cid) {
		BT_WARN("Mod Pub Model ID or Company ID mismatch");
		return;
	}

	status = net_buf_simple_pull_u8(buf);

	elem_addr = net_buf_simple_pull_le16(buf);
	if (elem_addr != param->elem_addr) {
		BT_WARN("Model Pub Status for unexpected element (0x%04x)",
			elem_addr);
		return;
	}

	*param->status = status;

	if (param->pub) {
		param->pub->addr = net_buf_simple_pull_le16(buf);
		param->pub->app_idx = net_buf_simple_pull_le16(buf);
		param->pub->cred_flag = (param->pub->app_idx & MESH_BIT(12));
		param->pub->app_idx &= MESH_BIT_MASK(12);
		param->pub->ttl = net_buf_simple_pull_u8(buf);
		param->pub->period = net_buf_simple_pull_u8(buf);
		param->pub->transmit = net_buf_simple_pull_u8(buf);
	}

	k_sem_give(&cli->op_sync);
}

struct mod_sub_param {
	u8_t *status;
	u16_t elem_addr;
	u16_t *sub_addr;
	u16_t *expect_sub;
	u16_t mod_id;
	u16_t cid;
};

static void mod_sub_status(struct bt_mesh_model *model,
			   struct bt_mesh_msg_ctx *ctx,
			   struct net_buf_simple *buf)
{
	u16_t elem_addr, sub_addr, mod_id, cid;
	struct mod_sub_param *param;
	u8_t status;

	BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
	       ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
	       bt_hex(buf->data, buf->len));

	if (cli->op_pending != OP_MOD_SUB_STATUS) {
		BT_WARN("Unexpected Model Subscription Status message");
		return;
	}

	status = net_buf_simple_pull_u8(buf);
	elem_addr = net_buf_simple_pull_le16(buf);
	sub_addr = net_buf_simple_pull_le16(buf);

	if (buf->len >= 4) {
		cid = net_buf_simple_pull_le16(buf);
	} else {
		cid = CID_NVAL;
	}

	mod_id = net_buf_simple_pull_le16(buf);

	param = cli->op_param;
	if (param->elem_addr != elem_addr || param->mod_id != mod_id ||
	    (param->expect_sub && *param->expect_sub != sub_addr) ||
	    param->cid != cid) {
		BT_WARN("Model Subscription Status parameters did not match");
		return;
	}

	if (param->sub_addr) {
		*param->sub_addr = sub_addr;
	}

	*param->status = status;

	k_sem_give(&cli->op_sync);
}

struct hb_sub_param {
	u8_t *status;
	struct bt_mesh_cfg_hb_sub *sub;
};

static void hb_sub_status(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
	struct hb_sub_param *param;

	BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
	       ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
	       bt_hex(buf->data, buf->len));

	if (cli->op_pending != OP_HEARTBEAT_SUB_STATUS) {
		BT_WARN("Unexpected Heartbeat Subscription Status message");
		return;
	}

	param = cli->op_param;

	*param->status = net_buf_simple_pull_u8(buf);

	param->sub->src = net_buf_simple_pull_le16(buf);
	param->sub->dst = net_buf_simple_pull_le16(buf);
	param->sub->period = net_buf_simple_pull_u8(buf);
	param->sub->count = net_buf_simple_pull_u8(buf);
	param->sub->min = net_buf_simple_pull_u8(buf);
	param->sub->max = net_buf_simple_pull_u8(buf);

	k_sem_give(&cli->op_sync);
}

struct hb_pub_param {
	u8_t *status;
	struct bt_mesh_cfg_hb_pub *pub;
};

static void hb_pub_status(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
	struct hb_pub_param *param;

	BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
	       ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
	       bt_hex(buf->data, buf->len));

	if (cli->op_pending != OP_HEARTBEAT_PUB_STATUS) {
		BT_WARN("Unexpected Heartbeat Publication Status message");
		return;
	}

	param = cli->op_param;

	*param->status = net_buf_simple_pull_u8(buf);

	if (param->pub) {
		param->pub->dst = net_buf_simple_pull_le16(buf);
		param->pub->count = net_buf_simple_pull_u8(buf);
		param->pub->period = net_buf_simple_pull_u8(buf);
		param->pub->ttl = net_buf_simple_pull_u8(buf);
		param->pub->feat = net_buf_simple_pull_u8(buf);
		param->pub->net_idx = net_buf_simple_pull_u8(buf);
	}

	k_sem_give(&cli->op_sync);
}

struct bt_mesh_model_op bt_mesh_cfg_cli_op[] = {
	{ OP_DEV_COMP_DATA_STATUS,   15,  comp_data_status },
	{ OP_BEACON_STATUS,          1,   beacon_status },
	{ OP_DEFAULT_TTL_STATUS,     1,   ttl_status },
	{ OP_FRIEND_STATUS,          1,   friend_status },
	{ OP_GATT_PROXY_STATUS,      1,   gatt_proxy_status },
	{ OP_RELAY_STATUS,           2,   relay_status },
	{ OP_NET_KEY_STATUS,         3,   net_key_status },
	{ OP_APP_KEY_STATUS,         4,   app_key_status },
	{ OP_MOD_APP_STATUS,         7,   mod_app_status },
	{ OP_MOD_PUB_STATUS,         12,  mod_pub_status },
	{ OP_MOD_SUB_STATUS,         7,   mod_sub_status },
	{ OP_HEARTBEAT_SUB_STATUS,   9,   hb_sub_status },
	{ OP_HEARTBEAT_PUB_STATUS,   10,  hb_pub_status },
	BT_MESH_MODEL_OP_END,
};

static int check_cli(void)
{
	if (!cli) {
		BT_ERR("No available Configuration Client context!");
		return -EINVAL;
	}

	return 0;
}

int bt_mesh_cfg_comp_data_get(u16_t net_idx, u16_t addr, u8_t page,
			      u8_t *status, struct net_buf_simple *comp)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 1 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

    err = check_cli();
    if (err) {
        return err;
    }

	bt_mesh_model_msg_init(msg, OP_DEV_COMP_DATA_GET);
	net_buf_simple_add_u8(msg, page);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

    return err;
}

static int get_state_u8(u16_t net_idx, u16_t addr, u32_t op, u32_t rsp,
			u8_t *val)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 0 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, op);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

    return err;
}

static int set_state_u8(u16_t net_idx, u16_t addr, u32_t op, u32_t rsp,
			u8_t new_val, u8_t *val)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 1 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, op);
	net_buf_simple_add_u8(msg, new_val);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

    return err;
}

int bt_mesh_cfg_beacon_get(u16_t net_idx, u16_t addr, u8_t *status)
{
	return get_state_u8(net_idx, addr, OP_BEACON_GET, OP_BEACON_STATUS,
			    status);
}

int bt_mesh_cfg_beacon_set(u16_t net_idx, u16_t addr, u8_t val, u8_t *status)
{
	return set_state_u8(net_idx, addr, OP_BEACON_SET, OP_BEACON_STATUS,
			    val, status);
}

int bt_mesh_cfg_ttl_get(u16_t net_idx, u16_t addr, u8_t *ttl)
{
	return get_state_u8(net_idx, addr, OP_DEFAULT_TTL_GET,
			    OP_DEFAULT_TTL_STATUS, ttl);
}

int bt_mesh_cfg_ttl_set(u16_t net_idx, u16_t addr, u8_t val, u8_t *ttl)
{
	return set_state_u8(net_idx, addr, OP_DEFAULT_TTL_SET,
			    OP_DEFAULT_TTL_STATUS, val, ttl);
}

int bt_mesh_cfg_friend_get(u16_t net_idx, u16_t addr, u8_t *status)
{
	return get_state_u8(net_idx, addr, OP_FRIEND_GET,
			    OP_FRIEND_STATUS, status);
}

int bt_mesh_cfg_friend_set(u16_t net_idx, u16_t addr, u8_t val, u8_t *status)
{
	return set_state_u8(net_idx, addr, OP_FRIEND_SET, OP_FRIEND_STATUS,
			    val, status);
}

int bt_mesh_cfg_gatt_proxy_get(u16_t net_idx, u16_t addr, u8_t *status)
{
	return get_state_u8(net_idx, addr, OP_GATT_PROXY_GET,
			    OP_GATT_PROXY_STATUS, status);
}

int bt_mesh_cfg_gatt_proxy_set(u16_t net_idx, u16_t addr, u8_t val,
			       u8_t *status)
{
	return set_state_u8(net_idx, addr, OP_GATT_PROXY_SET,
			    OP_GATT_PROXY_STATUS, val, status);
}

int bt_mesh_cfg_relay_get(u16_t net_idx, u16_t addr, u8_t *status,
			  u8_t *transmit)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 0 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_RELAY_GET);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

    return err;
}

int bt_mesh_cfg_relay_set(u16_t net_idx, u16_t addr, u8_t new_relay,
			  u8_t new_transmit, u8_t *status, u8_t *transmit)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 2 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_RELAY_SET);
	net_buf_simple_add_u8(msg, new_relay);
	net_buf_simple_add_u8(msg, new_transmit);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

    return err;
}

int bt_mesh_cfg_net_key_add(u16_t net_idx, u16_t addr, u16_t key_net_idx,
			    const u8_t net_key[16], u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 18 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_NET_KEY_ADD);
	net_buf_simple_add_le16(msg, key_net_idx);
	net_buf_simple_add_mem(msg, net_key, 16);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_krp_set(u16_t net_idx, u16_t addr, u16_t key_net_idx, u8_t phase, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 3 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;
	struct bt_mesh_subnet *sub;

	err = check_cli();
	if (err) {
		return err;
	}

	sub = bt_mesh_subnet_get(key_net_idx);
	if (!sub) {
		BT_ERR("Invalid Netkey Index\r\n");
		return err;
	}

	if (phase < BT_MESH_KR_PHASE_2 || phase > BT_MESH_KR_PHASE_3 ||
	    (sub->kr_phase == BT_MESH_KR_NORMAL &&
	     phase == BT_MESH_KR_PHASE_2)) {
		BT_WARN("Prohibited transition %u -> %u", sub->kr_phase, phase);
		return err;
	}
	if (sub->kr_phase == BT_MESH_KR_PHASE_1 &&
		phase == BT_MESH_KR_PHASE_2) {
		sub->kr_phase = BT_MESH_KR_PHASE_2;
		sub->kr_flag = 1;
		bt_mesh_net_beacon_update(sub);
	} else if ((sub->kr_phase == BT_MESH_KR_PHASE_1 ||
			sub->kr_phase == BT_MESH_KR_PHASE_2) &&
			phase == BT_MESH_KR_PHASE_3) {
			   bt_mesh_net_revoke_keys(sub);
		if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER) ||
		    IS_ENABLED(CONFIG_BT_MESH_FRIEND)) {
			friend_cred_refresh(ctx.net_idx);
		}
		sub->kr_phase = BT_MESH_KR_NORMAL;
		sub->kr_flag = 0;
		bt_mesh_net_beacon_update(sub);
	}
	bt_mesh_model_msg_init(msg, OP_KRP_SET);
	net_buf_simple_add_le16(msg, key_net_idx);
	net_buf_simple_add_u8(msg, phase);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_krp_get(u16_t net_idx, u16_t addr, u16_t key_net_idx, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 2 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_KRP_GET);
	net_buf_simple_add_le16(msg, key_net_idx);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_net_key_del(u16_t net_idx, u16_t addr, u16_t key_net_idx, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 2 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_NET_KEY_DEL);
	net_buf_simple_add_le16(msg, key_net_idx);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_net_key_get(u16_t net_idx, u16_t addr, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 18 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_NET_KEY_GET);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_net_key_update(u16_t net_idx, u16_t addr, u16_t key_net_idx,
			    const u8_t net_key[16], u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 18 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;
	struct bt_mesh_subnet *sub;

	err = check_cli();
	if (err) {
		return err;
	}

	sub = bt_mesh_subnet_get(key_net_idx);
	err = bt_mesh_net_keys_create(&sub->keys[1], net_key);

	if (!err && (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER) ||
		     IS_ENABLED(CONFIG_BT_MESH_FRIEND))) {
		err = friend_cred_update(sub);
	}

	sub->kr_phase = BT_MESH_KR_PHASE_1;

	bt_mesh_model_msg_init(msg, OP_NET_KEY_UPDATE);
	net_buf_simple_add_le16(msg, key_net_idx);
	net_buf_simple_add_mem(msg, net_key, 16);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_app_key_add(u16_t net_idx, u16_t addr, u16_t key_net_idx,
			    u16_t key_app_idx, const u8_t app_key[16],
			    u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(1 + 19 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_APP_KEY_ADD);
	key_idx_pack(msg, key_net_idx, key_app_idx);
	net_buf_simple_add_mem(msg, app_key, 16);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_app_key_del(u16_t net_idx, u16_t addr, u16_t key_net_idx,
			    u16_t key_app_idx, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 3 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_APP_KEY_DEL);
	key_idx_pack(msg, key_net_idx, key_app_idx);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_app_key_update(u16_t net_idx, u16_t addr, u16_t key_net_idx,
			    u16_t key_app_idx, const u8_t app_key[16],
			    u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 19 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_APP_KEY_UPDATE);
	key_idx_pack(msg, key_net_idx, key_app_idx);
	net_buf_simple_add_mem(msg, app_key, 16);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_app_key_get(u16_t net_idx, u16_t addr, u16_t key_net_idx, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 2 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_APP_KEY_GET);
	net_buf_simple_add_le16(msg, key_net_idx);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

static int mod_app_bind(u16_t net_idx, u16_t addr, u16_t elem_addr,
			u16_t mod_app_idx, u16_t mod_id, u16_t cid,
			u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 8 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_MOD_APP_BIND);
	net_buf_simple_add_le16(msg, elem_addr);
	net_buf_simple_add_le16(msg, mod_app_idx);

	if (cid != CID_NVAL) {
		net_buf_simple_add_le16(msg, cid);
	}

	net_buf_simple_add_le16(msg, mod_id);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_mod_app_bind(u16_t net_idx, u16_t addr, u16_t elem_addr,
			     u16_t mod_app_idx, u16_t mod_id, u8_t *status)
{
	return mod_app_bind(net_idx, addr, elem_addr, mod_app_idx, mod_id,
			    CID_NVAL, status);
}

int bt_mesh_cfg_mod_app_bind_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr,
				 u16_t mod_app_idx, u16_t mod_id, u16_t cid,
				 u8_t *status)
{
	if (cid == CID_NVAL) {
		return -EINVAL;
	}

	return mod_app_bind(net_idx, addr, elem_addr, mod_app_idx, mod_id, cid,
			    status);
}

static int mod_app_unbind(u16_t net_idx, u16_t addr, u16_t elem_addr,
			u16_t mod_app_idx, u16_t mod_id, u16_t cid,
			u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 8 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_MOD_APP_UNBIND);
	net_buf_simple_add_le16(msg, elem_addr);
	net_buf_simple_add_le16(msg, mod_app_idx);

	if (cid != CID_NVAL) {
		net_buf_simple_add_le16(msg, cid);
	}

	net_buf_simple_add_le16(msg, mod_id);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_mod_app_unbind(u16_t net_idx, u16_t addr, u16_t elem_addr,
			     u16_t mod_app_idx, u16_t mod_id, u8_t *status)
{
	return mod_app_unbind(net_idx, addr, elem_addr, mod_app_idx, mod_id,
			    CID_NVAL, status);
}

int bt_mesh_cfg_mod_app_unbind_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr,
				 u16_t mod_app_idx, u16_t mod_id, u16_t cid,
				 u8_t *status)
{
	if (cid == CID_NVAL) {
		return -EINVAL;
	}

	return mod_app_unbind(net_idx, addr, elem_addr, mod_app_idx, mod_id, cid,
			    status);
}

static int mod_app_get(u16_t net_idx, u16_t addr, u16_t elem_addr,
			 u16_t mod_id, u16_t cid, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 6 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	if (cid != CID_NVAL) {
		bt_mesh_model_msg_init(msg, OP_VND_MOD_APP_GET);
	} else {
		bt_mesh_model_msg_init(msg, OP_SIG_MOD_APP_GET);
	}

	net_buf_simple_add_le16(msg, elem_addr);

	if (cid != CID_NVAL) {
		net_buf_simple_add_le16(msg, cid);
	}

	net_buf_simple_add_le16(msg, mod_id);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_mod_app_get(u16_t net_idx, u16_t addr, u16_t elem_addr,
			       u16_t mod_id, u8_t *status)
{
	return mod_app_get(net_idx, addr, elem_addr, mod_id, CID_NVAL, status);
}

int bt_mesh_cfg_mod_app_get_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr,
				   u16_t mod_id, u16_t cid, u8_t *status)
{
	if (cid == CID_NVAL) {
		return -EINVAL;
	}

	return mod_app_get(net_idx, addr, elem_addr, mod_id, cid, status);
}

int bt_mesh_cfg_node_ident_get(u16_t net_idx, u16_t addr, u16_t elem_addr, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 2 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_NODE_IDENTITY_GET);
	net_buf_simple_add_le16(msg, net_idx);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_node_ident_set(u16_t net_idx, u16_t addr, u16_t elem_addr, u8_t ident_state, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 3 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_NODE_IDENTITY_SET);
	net_buf_simple_add_le16(msg, net_idx);
	net_buf_simple_add_u8(msg, ident_state);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_node_reset(u16_t net_idx, u16_t addr, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_NODE_RESET);
	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);

	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_network_transmit_get(u16_t net_idx, u16_t addr, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_NET_TRANSMIT_GET);
	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);

	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_network_transmit_set(u16_t net_idx, u16_t addr, u8_t transmit, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 1 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_NET_TRANSMIT_SET);

	net_buf_simple_add_u8(msg, transmit);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);

	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_lpn_timeout_get(u16_t net_idx, u16_t addr, u16_t lpn_addr, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 2 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_LPN_TIMEOUT_GET);

	net_buf_simple_add_le16(msg, lpn_addr);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);

	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

static int mod_sub(u32_t op, u16_t net_idx, u16_t addr, u16_t elem_addr,
		   u16_t sub_addr, u16_t mod_id, u16_t cid, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 8 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, op);
	net_buf_simple_add_le16(msg, elem_addr);

	if (op != OP_MOD_SUB_DEL_ALL && op != OP_MOD_SUB_GET && op != OP_MOD_SUB_GET_VND)
	{
		net_buf_simple_add_le16(msg, sub_addr);
	}

	if (cid != CID_NVAL) {
		net_buf_simple_add_le16(msg, cid);
	}

	net_buf_simple_add_le16(msg, mod_id);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_mod_sub_add(u16_t net_idx, u16_t addr, u16_t elem_addr,
			    u16_t sub_addr, u16_t mod_id, u8_t *status)
{
	return mod_sub(OP_MOD_SUB_ADD, net_idx, addr, elem_addr, sub_addr,
		       mod_id, CID_NVAL, status);
}

int bt_mesh_cfg_mod_sub_add_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr,
				 u16_t sub_addr, u16_t mod_id, u16_t cid,
				 u8_t *status)
{
	if (cid == CID_NVAL) {
		return -EINVAL;
	}

	return mod_sub(OP_MOD_SUB_ADD, net_idx, addr, elem_addr, sub_addr,
		       mod_id, cid, status);
}

int bt_mesh_cfg_mod_sub_del(u16_t net_idx, u16_t addr, u16_t elem_addr,
			    u16_t sub_addr, u16_t mod_id, u8_t *status)
{
	return mod_sub(OP_MOD_SUB_DEL, net_idx, addr, elem_addr, sub_addr,
		       mod_id, CID_NVAL, status);
}


int bt_mesh_cfg_mod_sub_del_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr,
				 u16_t sub_addr, u16_t mod_id, u16_t cid,
				 u8_t *status)
{
	if (cid == CID_NVAL) {
		return -EINVAL;
	}

	return mod_sub(OP_MOD_SUB_DEL, net_idx, addr, elem_addr, sub_addr,
		       mod_id, cid, status);
}


int bt_mesh_cfg_mod_sub_del_all(u16_t net_idx, u16_t addr, u16_t elem_addr,
			    u16_t sub_addr, u16_t mod_id, u8_t *status)
{
	return mod_sub(OP_MOD_SUB_DEL_ALL, net_idx, addr, elem_addr, sub_addr,
		       mod_id, CID_NVAL, status);
}

int bt_mesh_cfg_mod_sub_del_all_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr,
				 u16_t sub_addr, u16_t mod_id, u16_t cid,
				 u8_t *status)
{
	if (cid == CID_NVAL) {
		return -EINVAL;
	}

	return mod_sub(OP_MOD_SUB_DEL_ALL, net_idx, addr, elem_addr, sub_addr,
		       mod_id, cid, status);
}


int bt_mesh_cfg_mod_sub_get(u16_t net_idx, u16_t addr, u16_t elem_addr,
			    u16_t sub_addr, u16_t mod_id, u8_t *status)
{
	return mod_sub(OP_MOD_SUB_GET, net_idx, addr, elem_addr, sub_addr,
		       mod_id, CID_NVAL, status);
}
int bt_mesh_cfg_mod_sub_get_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr,
			    u16_t sub_addr, u16_t mod_id, u16_t cid, u8_t *status)
{
	if (cid == CID_NVAL) {
		return -EINVAL;
	}

	return mod_sub(OP_MOD_SUB_GET_VND, net_idx, addr, elem_addr, sub_addr,
		       mod_id, cid, status);
}
int bt_mesh_cfg_mod_sub_overwrite(u16_t net_idx, u16_t addr, u16_t elem_addr,
				  u16_t sub_addr, u16_t mod_id, u8_t *status)
{
	return mod_sub(OP_MOD_SUB_OVERWRITE, net_idx, addr, elem_addr,
		       sub_addr, mod_id, CID_NVAL, status);
}

int bt_mesh_cfg_mod_sub_overwrite_vnd(u16_t net_idx, u16_t addr,
				      u16_t elem_addr, u16_t sub_addr,
				      u16_t mod_id, u16_t cid, u8_t *status)
{
	if (cid == CID_NVAL) {
		return -EINVAL;
	}

	return mod_sub(OP_MOD_SUB_OVERWRITE, net_idx, addr, elem_addr,
		       sub_addr, mod_id, cid, status);
}

static int mod_sub_va(u32_t op, u16_t net_idx, u16_t addr, u16_t elem_addr,
		      const u8_t label[16], u16_t mod_id, u16_t cid,
		      u16_t *virt_addr, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 22 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	BT_DBG("net_idx 0x%04x addr 0x%04x elem_addr 0x%04x label %s",
	       net_idx, addr, elem_addr, label);
	BT_DBG("mod_id 0x%04x cid 0x%04x", mod_id, cid);

	bt_mesh_model_msg_init(msg, op);
	net_buf_simple_add_le16(msg, elem_addr);
	net_buf_simple_add_mem(msg, label, 16);

	if (cid != CID_NVAL) {
		net_buf_simple_add_le16(msg, cid);
	}

	net_buf_simple_add_le16(msg, mod_id);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_mod_sub_va_add(u16_t net_idx, u16_t addr, u16_t elem_addr,
			       const u8_t label[16], u16_t mod_id,
			       u16_t *virt_addr, u8_t *status)
{
	return mod_sub_va(OP_MOD_SUB_VA_ADD, net_idx, addr, elem_addr, label,
			  mod_id, CID_NVAL, virt_addr, status);
}

int bt_mesh_cfg_mod_sub_va_add_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr,
				   const u8_t label[16], u16_t mod_id,
				   u16_t cid, u16_t *virt_addr, u8_t *status)
{
	if (cid == CID_NVAL) {
		return -EINVAL;
	}

	return mod_sub_va(OP_MOD_SUB_VA_ADD, net_idx, addr, elem_addr, label,
			  mod_id, cid, virt_addr, status);
}

int bt_mesh_cfg_mod_sub_va_del(u16_t net_idx, u16_t addr, u16_t elem_addr,
			       const u8_t label[16], u16_t mod_id,
			       u16_t *virt_addr, u8_t *status)
{
	return mod_sub_va(OP_MOD_SUB_VA_DEL, net_idx, addr, elem_addr, label,
			  mod_id, CID_NVAL, virt_addr, status);
}

int bt_mesh_cfg_mod_sub_va_del_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr,
				   const u8_t label[16], u16_t mod_id,
				   u16_t cid, u16_t *virt_addr, u8_t *status)
{
	if (cid == CID_NVAL) {
		return -EINVAL;
	}

	return mod_sub_va(OP_MOD_SUB_VA_DEL, net_idx, addr, elem_addr, label,
			  mod_id, cid, virt_addr, status);
}

int bt_mesh_cfg_mod_sub_va_overwrite(u16_t net_idx, u16_t addr,
				     u16_t elem_addr, const u8_t label[16],
				     u16_t mod_id, u16_t *virt_addr,
				     u8_t *status)
{
	return mod_sub_va(OP_MOD_SUB_VA_OVERWRITE, net_idx, addr, elem_addr,
			  label, mod_id, CID_NVAL, virt_addr, status);
}

int bt_mesh_cfg_mod_sub_va_overwrite_vnd(u16_t net_idx, u16_t addr,
					 u16_t elem_addr, const u8_t label[16],
					 u16_t mod_id, u16_t cid,
					 u16_t *virt_addr, u8_t *status)
{
	if (cid == CID_NVAL) {
		return -EINVAL;
	}

	return mod_sub_va(OP_MOD_SUB_VA_OVERWRITE, net_idx, addr, elem_addr,
			  label, mod_id, cid, virt_addr, status);
}

static int mod_pub_get(u16_t net_idx, u16_t addr, u16_t elem_addr,
		       u16_t mod_id, u16_t cid,
		       struct bt_mesh_cfg_mod_pub *pub, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 6 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_MOD_PUB_GET);

	net_buf_simple_add_le16(msg, elem_addr);

	if (cid != CID_NVAL) {
		net_buf_simple_add_le16(msg, cid);
	}

	net_buf_simple_add_le16(msg, mod_id);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_mod_pub_get(u16_t net_idx, u16_t addr, u16_t elem_addr,
			    u16_t mod_id, struct bt_mesh_cfg_mod_pub *pub,
			    u8_t *status)
{
	return mod_pub_get(net_idx, addr, elem_addr, mod_id, CID_NVAL,
			   pub, status);
}

int bt_mesh_cfg_mod_pub_get_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr,
				u16_t mod_id, u16_t cid,
				struct bt_mesh_cfg_mod_pub *pub, u8_t *status)
{
	if (cid == CID_NVAL) {
		return -EINVAL;
	}

	return mod_pub_get(net_idx, addr, elem_addr, mod_id, CID_NVAL,
			   pub, status);
}

static int mod_pub_set(u16_t net_idx, u16_t addr, u16_t elem_addr,
		       u16_t mod_id, u16_t cid,
		       struct bt_mesh_cfg_mod_pub *pub, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 13 + 14 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	/* add MOD_PUB_VA_SET */
	if (pub->uuid == NULL) {
		bt_mesh_model_msg_init(msg, OP_MOD_PUB_SET);
	} else {
		bt_mesh_model_msg_init(msg, OP_MOD_PUB_VA_SET);
	}

	net_buf_simple_add_le16(msg, elem_addr);
	if (pub->uuid == NULL) {
		net_buf_simple_add_le16(msg, pub->addr);
	} else {
		net_buf_simple_add_mem(msg, pub->uuid, 16);
	}
	net_buf_simple_add_le16(msg, (pub->app_idx & (pub->cred_flag << 12)));
	net_buf_simple_add_u8(msg, pub->ttl);
	net_buf_simple_add_u8(msg, pub->period);
	net_buf_simple_add_u8(msg, pub->transmit);

	if (cid != CID_NVAL) {
		net_buf_simple_add_le16(msg, cid);
	}

	net_buf_simple_add_le16(msg, mod_id);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_mod_pub_set(u16_t net_idx, u16_t addr, u16_t elem_addr,
			    u16_t mod_id, struct bt_mesh_cfg_mod_pub *pub,
			    u8_t *status)
{
	return mod_pub_set(net_idx, addr, elem_addr, mod_id, CID_NVAL,
			   pub, status);
}

int bt_mesh_cfg_mod_pub_set_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr,
				u16_t mod_id, u16_t cid,
				struct bt_mesh_cfg_mod_pub *pub, u8_t *status)
{
	if (cid == CID_NVAL) {
		return -EINVAL;
	}

	return mod_pub_set(net_idx, addr, elem_addr, mod_id, cid,
			   pub, status);
}

int bt_mesh_cfg_hb_sub_set(u16_t net_idx, u16_t addr,
			   struct bt_mesh_cfg_hb_sub *sub, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 5 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_HEARTBEAT_SUB_SET);
	net_buf_simple_add_le16(msg, sub->src);
	net_buf_simple_add_le16(msg, sub->dst);
	net_buf_simple_add_u8(msg, sub->period);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_hb_sub_get(u16_t net_idx, u16_t addr,
			   struct bt_mesh_cfg_hb_sub *sub, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 0 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_HEARTBEAT_SUB_GET);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_hb_pub_set(u16_t net_idx, u16_t addr,
			   const struct bt_mesh_cfg_hb_pub *pub, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 9 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_HEARTBEAT_PUB_SET);
	net_buf_simple_add_le16(msg, pub->dst);
	net_buf_simple_add_u8(msg, pub->count);
	net_buf_simple_add_u8(msg, pub->period);
	net_buf_simple_add_u8(msg, pub->ttl);
	net_buf_simple_add_le16(msg, pub->feat);
	net_buf_simple_add_le16(msg, pub->net_idx);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

int bt_mesh_cfg_hb_pub_get(u16_t net_idx, u16_t addr,
			   struct bt_mesh_cfg_hb_pub *pub, u8_t *status)
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 0 + 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = BT_MESH_KEY_DEV,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = check_cli();
	if (err) {
		return err;
	}

	bt_mesh_model_msg_init(msg, OP_HEARTBEAT_PUB_GET);

	err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
	if (err) {
		BT_ERR("model_send() failed (err %d)", err);
		return err;
	}

	if (!status) {
		return 0;
	}

    return err;
}

s32_t bt_mesh_cfg_cli_timeout_get(void)
{
	return msg_timeout;
}

void bt_mesh_cfg_cli_timeout_set(s32_t timeout)
{
	msg_timeout = timeout;
}

static struct bt_mesh_cfg_cli default_cfg_cli = {
};
int bt_mesh_cfg_cli_init(struct bt_mesh_model *model, bool primary)
{
	if (!primary) {
		BT_ERR("Configuration Client only allowed in primary element");
		return -EINVAL;
	}
// TBD
#if 0
	if (!model->user_data) {
		BT_ERR("No Configuration Client context provided");
		return -EINVAL;
	}

	cli = model->user_data;
	cli->model = model;

    if (!model->user_data) {
        BT_ERR("No Configuration Client context provided");
        cli = &default_cfg_cli;
    } else {
        cli = model->user_data;
    }
#endif
    cli = &default_cfg_cli;
    cli->model = model;
  
	/* Configuration Model security is device-key based */
	model->keys[0] = BT_MESH_KEY_DEV;

	k_sem_init(&cli->op_sync, 0, 1);

	return 0;
}
#endif
