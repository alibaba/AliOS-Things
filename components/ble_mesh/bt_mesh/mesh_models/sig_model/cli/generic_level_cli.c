/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <api/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "bt_errno.h"
#include "mesh_model/mesh_model.h"

#define TAG "GEN_LEVEL_CLI"
#if defined(CONFIG_BT_MESH_MODEL_GEN_LEVEL_CLI)

extern u8_t bt_mesh_default_ttl_get(void);


struct bt_mesh_model_pub g_generic_level_cli_pub = {
    .msg = NET_BUF_SIMPLE(2 + 5 + 4),
};

/*
static void _generic_level_cli_prepear_buf(struct bt_mesh_model *model, struct net_buf_simple *msg, bool is_ack)
{
    return;
}*/

static void _generic_level_cli_status(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf)
{
    model_message message;
    message.source_addr = ctx->addr;
    message.status_data = buf;
    model_event(BT_MESH_MODEL_LEVEL_STATUS, &message);
    return;
}


int ble_mesh_generic_level_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model)
{
    int err;
    struct bt_mesh_msg_ctx ctx = {0};

    if (model == NULL) {
        return -EINVAL;
    }

    if (0x0000 == unicast_addr) {
        return -EADDRNOTAVAIL;
    }

    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 0 + 4);

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x05));

    ctx.addr = unicast_addr;

    ctx.net_idx = netkey_idx;

    ctx.app_idx = appkey_idx;
    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "generic level get send fail %d", err);
        return err;
    }

    return 0;
}

int ble_mesh_generic_level_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model, set_level_arg *send_arg, bool ack)
{
    int err;
    struct bt_mesh_msg_ctx ctx = {0};
    //uint8_t extra_size = send_arg->send_trans ? 5 : 3;

    if (model == NULL || send_arg == NULL) {
        return -EINVAL;
    }

    if (0x0000 == unicast_addr) {
        return -EADDRNOTAVAIL;
    }

    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 5 + 4);

    if (ack) {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x06));
    } else {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x07));
    }

    net_buf_simple_add_le16(msg, send_arg->level);
    net_buf_simple_add_u8(msg, send_arg->tid);

    if (send_arg->send_trans) {
        net_buf_simple_add_u8(msg, send_arg->trans);
        net_buf_simple_add_u8(msg, send_arg->delay);
    }

    ctx.addr = unicast_addr;
    ctx.net_idx = netkey_idx;
    ctx.app_idx = appkey_idx;
    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "generic level send fail %d", err);
        return err;
    }

    LOGI(TAG, "SEND level %x, TID %d", send_arg->level, send_arg->tid);
    send_arg->tid++;
    return 0;
}

int ble_mesh_generic_level_delta_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model, set_level_arg *send_arg, bool ack)

{
    int err;
    struct bt_mesh_msg_ctx ctx = {0};
    //uint8_t extra_size = send_arg->send_trans ? 7 : 5;

    if (model == NULL || send_arg == NULL) {
        return -EINVAL;
    }

    if (0x0000 == unicast_addr) {
        return -EADDRNOTAVAIL;
    }

    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 7 + 4);

    if (ack) {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x09));
    } else {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x0a));
    }

    net_buf_simple_add_le32(msg, send_arg->delta);
    net_buf_simple_add_u8(msg, send_arg->tid);

    if (send_arg->send_trans) {
        net_buf_simple_add_u8(msg, send_arg->trans);
        net_buf_simple_add_u8(msg, send_arg->delay);
    }

    ctx.addr = unicast_addr;
    ctx.net_idx = netkey_idx;
    ctx.app_idx = appkey_idx;
    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "generic level delta send fail %d", err);
        return err;
    }

    LOGI(TAG, "SEND level delta %x, TID %d", send_arg->delta, send_arg->tid);
    send_arg->tid++;
    return 0;
}

int ble_mesh_generic_level_move_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model,set_level_arg *send_arg, bool ack)
{
    int err;
    struct bt_mesh_msg_ctx ctx = {0};
    //uint8_t extra_size = send_arg->send_trans ? 5 : 3;

    if (model == NULL || send_arg == NULL) {
        return -EINVAL;
    }

    if (0x0000 == unicast_addr) {
        return -EADDRNOTAVAIL;
    }

    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 5 + 4);

    if (ack) {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x0b));
    } else {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x0c));
    }

    net_buf_simple_add_le16(msg, send_arg->move);
    net_buf_simple_add_u8(msg, send_arg->tid);

    if (send_arg->send_trans) {
        net_buf_simple_add_u8(msg, send_arg->trans);
        net_buf_simple_add_u8(msg, send_arg->delay);
    }

    ctx.addr = unicast_addr;
    ctx.net_idx = netkey_idx;
    ctx.app_idx = appkey_idx;
    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "generic level move send fail %d", err);
        return err;
    }

    LOGI(TAG, "SEND level move %x, TID %d", send_arg->move, send_arg->tid);
    send_arg->tid++;
    return 0;
}

const struct bt_mesh_model_op g_generic_level_cli_op[GEN_LEVEL_CLI_OPC_NUM] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x08), 2, _generic_level_cli_status},
    BT_MESH_MODEL_OP_END,
};

#endif

