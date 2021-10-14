/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <api/mesh.h>

#include "bt_errno.h"
#include "mesh_model/mesh_model.h"

#define TAG "LIGHT_CTL_CLI"
#if defined(CONFIG_BT_MESH_MODEL_LIGHT_CTL_CLI)

struct bt_mesh_model_pub g_ctl_cli_pub = {
    .msg = NET_BUF_SIMPLE(2 + 9 + 4),
};


extern u8_t bt_mesh_default_ttl_get(void);

static void _light_ctl_status(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    LOGD(TAG, "");
    model_message message;
    message.source_addr = ctx->addr;
    message.status_data = buf;
    model_event(BT_MESH_MODEL_LIGHT_CTL_STATUS, &message);
    return;
}

static void _light_ctl_temp_status(struct bt_mesh_model *model,
                                   struct bt_mesh_msg_ctx *ctx,
                                   struct net_buf_simple *buf)
{
    LOGD(TAG, "");
    model_message message;
    message.source_addr = ctx->addr;
    message.status_data = buf;
    model_event(BT_MESH_MODEL_LIGHT_CTL_TEMP_STATUS, &message);
    return;
}

static void _light_ctl_default_status(struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      struct net_buf_simple *buf)
{
    LOGD(TAG, "");
    model_message message;
    message.source_addr = ctx->addr;
    message.status_data = buf;
    model_event(BT_MESH_MODEL_LIGHT_CTL_DEF_STATUS, &message);
    return;
}

static void _light_ctl_range_status(struct bt_mesh_model *model,
                                    struct bt_mesh_msg_ctx *ctx,
                                    struct net_buf_simple *buf)
{
    LOGD(TAG, "");
    model_message message;
    message.source_addr = ctx->addr;
    message.status_data = buf;
    model_event(BT_MESH_MODEL_LIGHT_CTL_TEMP_RANGE_STATUS, &message);
    return;
}



int ble_mesh_light_ctl_set(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr, struct bt_mesh_model *model, set_light_ctl_arg *send_arg, bool ack)

{
    int err;
    struct bt_mesh_msg_ctx ctx = {0};

    if (model == NULL || send_arg == NULL) {
        return -EINVAL;
    }

    if (0x0000 == unicast_addr) {
        return -EADDRNOTAVAIL;
    }

    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 9 + 4);

    if (ack) {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x5e));
    } else {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x5f));
    }

    net_buf_simple_add_le16(msg, send_arg->lightness);
    net_buf_simple_add_le16(msg, send_arg->temperature);
    net_buf_simple_add_le16(msg, send_arg->delta_uv);
    net_buf_simple_add_u8(msg, send_arg->tid);

    if (send_arg->trans) {
        net_buf_simple_add_u8(msg, send_arg->trans);
        net_buf_simple_add_u8(msg, send_arg->delay);
    }

    ctx.addr = unicast_addr;
    ctx.net_idx = netkey_idx;
    ctx.app_idx = appkey_idx;
    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "lightness range set send fail %d", err);
        return err;
    }

    LOGI(TAG, "light ctl lightness %x,temp %x, delta uv %x, TID %x,trans %x,delay %x", send_arg->lightness, send_arg->temperature, \
         send_arg->delta_uv, send_arg->tid, send_arg->trans, send_arg->delay);
    return 0;
}


int ble_mesh_light_ctl_get(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr, struct bt_mesh_model *model)

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

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x5D));

    ctx.addr = unicast_addr;

    ctx.net_idx = netkey_idx;

    ctx.app_idx = appkey_idx;

    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "light ctl get send fail %d", err);
        return err;
    }

    return 0;
}

int ble_mesh_light_ctl_temp_set(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr, struct bt_mesh_model *model, set_light_ctl_arg *send_arg, bool ack)
{
    int err;
    struct bt_mesh_msg_ctx ctx = {0};

    if (model == NULL || send_arg == NULL) {
        return -EINVAL;
    }

    if (0x0000 == unicast_addr) {
        return -EADDRNOTAVAIL;
    }

    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 7 + 4);

    if (ack) {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x64));
    } else {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x65));
    }

    net_buf_simple_add_le16(msg, send_arg->temperature);
    net_buf_simple_add_le16(msg, send_arg->delta_uv);
    net_buf_simple_add_u8(msg, send_arg->tid);

    if (send_arg->trans) {
        net_buf_simple_add_u8(msg, send_arg->trans);
        net_buf_simple_add_u8(msg, send_arg->delay);
    }


    ctx.addr = unicast_addr;
    ctx.net_idx = netkey_idx;
    ctx.app_idx = appkey_idx;
    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "light ctl set send fail %d", err);
        return err;
    }

    LOGI(TAG, "light ctl set temp %x, delta uv %x, TID %x,trans %x,delay %x", send_arg->temperature, \
         send_arg->delta_uv, send_arg->tid, send_arg->trans, send_arg->delay);
    return 0;
}

int ble_mesh_light_ctl_temp_get(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr, struct bt_mesh_model *model)
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

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x61));

    ctx.addr = unicast_addr;

    ctx.net_idx = netkey_idx;

    ctx.app_idx = appkey_idx;

    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "light ctl temp get send fail %d", err);
        return err;
    }

    return 0;
}

int ble_mesh_light_ctl_def_set(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr, struct bt_mesh_model *model, set_light_ctl_arg *send_arg, bool ack)
{
    int err;
    struct bt_mesh_msg_ctx ctx = {0};

    if (model == NULL || send_arg == NULL) {
        return -EINVAL;
    }

    if (0x0000 == unicast_addr) {
        return -EADDRNOTAVAIL;
    }

    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 6 + 4);

    if (ack) {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x69));
    } else {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x6A));
    }

    net_buf_simple_add_le16(msg, send_arg->lightness);
    net_buf_simple_add_le16(msg, send_arg->temperature);
    net_buf_simple_add_le16(msg, send_arg->delta_uv);

    ctx.addr = unicast_addr;
    ctx.net_idx = netkey_idx;
    ctx.app_idx = appkey_idx;
    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "light ctl def set send fail %d", err);
        return err;
    }

    LOGI(TAG, "light ctl def set set lightness %x, temperature %x, delta_uv %x", send_arg->lightness, send_arg->temperature, send_arg->delta_uv);
    return 0;
}

int ble_mesh_light_ctl_def_get(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr, struct bt_mesh_model *model)
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

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x67));

    ctx.addr = unicast_addr;

    ctx.net_idx = netkey_idx;

    ctx.app_idx = appkey_idx;

    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "light ctl def get send fail %d", err);
        return err;
    }

    return 0;
}

int ble_mesh_light_ctl_temp_range_set(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr, struct bt_mesh_model *model, set_light_ctl_arg *send_arg, bool ack)
{
    int err;
    struct bt_mesh_msg_ctx ctx = {0};

    if (model == NULL || send_arg == NULL) {
        return -EINVAL;
    }

    if (0x0000 == unicast_addr) {
        return -EADDRNOTAVAIL;
    }

    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 4 + 4);

    if (ack) {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x6b));
    } else {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x6c));
    }

    net_buf_simple_add_le16(msg, send_arg->range_min);
    net_buf_simple_add_le16(msg, send_arg->range_max);

    ctx.addr = unicast_addr;
    ctx.net_idx = netkey_idx;
    ctx.app_idx = appkey_idx;
    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "light ctl temp range set send fail %d", err);
        return err;
    }

    LOGI(TAG, "light ctl temp range set min %x, max %x", send_arg->range_min, send_arg->range_max);
    return 0;
}

int ble_mesh_light_ctl_temp_range_get(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr, struct bt_mesh_model *model)
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

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x62));

    ctx.addr = unicast_addr;

    ctx.net_idx = netkey_idx;

    ctx.app_idx = appkey_idx;

    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "light ctl temp range get send fail %d", err);
        return err;
    }

    return 0;
}


const struct bt_mesh_model_op light_ctl_cli_op[LIGHT_CTL_CLI_OPC_NUM] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x60), 2, _light_ctl_status},
    { BT_MESH_MODEL_OP_2(0x82, 0x66), 2, _light_ctl_temp_status},
    { BT_MESH_MODEL_OP_2(0x82, 0x68), 2, _light_ctl_default_status},
    { BT_MESH_MODEL_OP_2(0x82, 0x63), 2, _light_ctl_range_status},
    BT_MESH_MODEL_OP_END,
};

#endif
