/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <api/mesh.h>
#include "bt_errno.h"
#include "mesh_model/mesh_model.h"

#define TAG "GEN_ONOFF_CLI"

#if defined(CONFIG_BT_MESH_MODEL_GEN_ONOFF_CLI)

extern u8_t bt_mesh_default_ttl_get(void);

struct bt_mesh_model_pub g_generic_onoff_cli_pub = {
    .msg = NET_BUF_SIMPLE(2 + 3 + 4),
};

static void _generic_onoff_cli_prepear_buf(struct bt_mesh_model *model, struct net_buf_simple *msg, bool is_ack)
{
    if (is_ack) {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x02));
    } else {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x03));
    }
}



static void _generic_onoff_status(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{

    LOGD(TAG, "");
    model_message message;
    message.source_addr = ctx->addr;
    message.status_data = buf;
    model_event(BT_MESH_MODEL_ONOFF_STATUS, &message);
}

int ble_mesh_generic_onoff_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model)
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
    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x01));

    ctx.addr = unicast_addr;
    ctx.net_idx = netkey_idx;
    ctx.app_idx = appkey_idx;
    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);
    if (err) {
        LOGE(TAG, "generic onoff get send fail %d", err);
        return err;
    }

    return 0;
}

int ble_mesh_generic_onoff_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model, set_onoff_arg *send_arg, bool ack)
{
    int err;
    struct bt_mesh_msg_ctx ctx = {0};
    //uint8_t extra_size = send_arg->send_trans ? 4 : 2;

    if (model == NULL || send_arg == NULL) {
        return -EINVAL;
    }

    if (0x0000 == unicast_addr) {
        return -EADDRNOTAVAIL;
    }

    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 4 + 4);

    if (ack) {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x02));
    } else {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x03));
    }

    net_buf_simple_add_u8(msg, send_arg->onoff);
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
        LOGE(TAG,  "generic off send fail %d", err);
        return err;
    }

    LOGD(TAG, TAG, "SEND LED %s, TID %d", send_arg->onoff ? "ON" : "OFF", send_arg->tid);
    send_arg->tid++;
    return 0;
}

int ble_mesh_generic_onoff_cli_publish(struct bt_mesh_model *model, set_onoff_arg *send_arg, bool ack)
{
    struct net_buf_simple *msg;
    int err;

    if (!model || !send_arg || !model->pub || model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return -1;
    }

    msg = model->pub->msg;

    LOGD(TAG, "addr(0x%04x)", model->pub->addr);

    if (model->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        _generic_onoff_cli_prepear_buf(model, msg, ack);

        net_buf_simple_add_u8(msg, send_arg->onoff);
        net_buf_simple_add_u8(msg, send_arg->tid);

        if (send_arg->send_trans) {
            net_buf_simple_add_u8(msg, send_arg->trans);
            net_buf_simple_add_u8(msg, send_arg->delay);
        }

        err = bt_mesh_model_publish(model);

        if (err) {
            LOGE(TAG, "bt_mesh_model_publish err %d\n", err);
            return -1;
        }
        send_arg->tid++;

        LOGD(TAG, "Success!!!");
    }

    return 0;
}

const struct bt_mesh_model_op g_generic_onoff_cli_op[GEN_ONOFF_CLI_OPC_NUM] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x04), 0, _generic_onoff_status},
    BT_MESH_MODEL_OP_END,
};

#endif

