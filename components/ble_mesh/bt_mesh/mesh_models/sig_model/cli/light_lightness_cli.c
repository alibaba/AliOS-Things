/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <api/mesh.h>

#include "bt_errno.h"
#include "mesh_model/mesh_model.h"

#define TAG "LIGHT_LIGHTNESS_CLI"

#if defined(CONFIG_BT_MESH_MODEL_LIGHT_LIGHTNESS_CLI)

extern u8_t bt_mesh_default_ttl_get(void);

int light_lightness_cli_publication(struct bt_mesh_model *model);

struct bt_mesh_model_pub g_light_lightness_cli_pub = {
    .msg = NET_BUF_SIMPLE(2 + 5 + 4),
};

static void _light_lightness_status(struct bt_mesh_model *model,
                                    struct bt_mesh_msg_ctx *ctx,
                                    struct net_buf_simple *buf)
{
    LOGD(TAG, "");
    model_message message;
    message.source_addr = ctx->addr;
    message.status_data = buf;
    model_event(BT_MESH_MODEL_LIGHTNESS_STATUS, &message);
    return;
}

static void _light_lightness_linear_status(struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf)
{
    LOGD(TAG, "");
    model_message message;
    message.source_addr = ctx->addr;
    message.status_data = buf;
    model_event(BT_MESH_MODEL_LIGHTNESS_LINEAR_STATUS, &message);
}

static void _light_lightness_last_status(struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf)
{
    LOGD(TAG, "");
    model_message message;
    message.source_addr = ctx->addr;
    message.status_data = buf;
    model_event(BT_MESH_MODEL_LIGHTNESS_LAST_STATUS, &message);
    return;
}

static void _light_lightness_default_status(struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf)
{
    LOGD(TAG, "");
    model_message message;
    message.source_addr = ctx->addr;
    message.status_data = buf;
    model_event(BT_MESH_MODEL_LIGHTNESS_DEF_STATUS, &message);
    return;
}

static void _light_lightness_range_status(struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf)
{
    LOGD(TAG, "");
    model_message message;
    message.source_addr = ctx->addr;
    message.status_data = buf;
    model_event(BT_MESH_MODEL_LIGHTNESS_RANGE_STATUS, &message);

    return;
}



int ble_mesh_light_lightness_get(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr, struct bt_mesh_model *model)
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

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x4b));

    ctx.addr = unicast_addr;

    ctx.net_idx = netkey_idx;

    ctx.app_idx = appkey_idx;

    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "lightness get send fail %d", err);
        return err;
    }

    return 0;
}

int ble_mesh_light_lightness_set(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr, struct bt_mesh_model *model, set_lightness_arg *send_arg, bool ack)
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
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x4c));
    } else {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x4d));
    }

    net_buf_simple_add_le16(msg, send_arg->lightness);
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
        LOGE(TAG, "lightness send fail %d", err);
        return err;
    }

    LOGI(TAG, "lightness level %x, TID %d", send_arg->lightness, send_arg->tid);
    send_arg->tid++;
    return 0;
}


int ble_mesh_light_lightness_linear_get(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr, struct bt_mesh_model *model)
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

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x4f));

    ctx.addr = unicast_addr;

    ctx.net_idx = netkey_idx;

    ctx.app_idx = appkey_idx;

    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "lightness get send fail %d", err);
        return err;
    }

    return 0;
}

int ble_mesh_light_lightness_linear_set(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr, struct bt_mesh_model *model, set_lightness_arg *send_arg, bool ack)

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
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x50));
    } else {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x51));
    }

    net_buf_simple_add_le16(msg, send_arg->lightness_linear);
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
        LOGE(TAG, "lightness linear send fail %d", err);
        return err;
    }

    LOGI(TAG, "lightness linear level %x, TID %d", send_arg->lightness_linear, send_arg->tid);
    send_arg->tid++;
    return 0;
}

int ble_mesh_light_lightness_last_get(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr, struct bt_mesh_model *model)
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

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x53));

    ctx.addr = unicast_addr;

    ctx.net_idx = netkey_idx;

    ctx.app_idx = appkey_idx;

    ctx.send_ttl = bt_mesh_default_ttl_get();

    err = bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "lightness get send fail %d", err);
        return err;
    }

    return 0;
}

int ble_mesh_light_lightness_def_get(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr, struct bt_mesh_model *model)
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

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x55));

    ctx.addr = unicast_addr;

    ctx.net_idx = netkey_idx;

    ctx.app_idx = appkey_idx;

    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "lightness get send fail %d", err);
        return err;
    }

    return 0;
}


int ble_mesh_light_lightness_def_set(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr, struct bt_mesh_model *model, set_lightness_arg *send_arg, bool ack)
{
    int err;
    struct bt_mesh_msg_ctx ctx = {0};

    if (model == NULL || send_arg == NULL) {
        return -EINVAL;
    }

    if (0x0000 == unicast_addr) {
        return -EADDRNOTAVAIL;
    }

    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 2 + 4);

    if (ack) {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x59));
    } else {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x5a));
    }

    net_buf_simple_add_le16(msg, send_arg->def);

    ctx.addr = unicast_addr;
    ctx.net_idx = netkey_idx;
    ctx.app_idx = appkey_idx;
    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "lightness def set send fail %d", err);
        return err;
    }

    LOGI(TAG, "lightness def %x", send_arg->def);
    return 0;
}

int ble_mesh_light_lightness_range_get(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr, struct bt_mesh_model *model)
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

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x57));

    ctx.addr = unicast_addr;

    ctx.net_idx = netkey_idx;

    ctx.app_idx = appkey_idx;

    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "lightness get send fail %d", err);
        return err;
    }

    return 0;
}


int ble_mesh_light_lightness_range_set(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr, struct bt_mesh_model *model, set_lightness_arg *send_arg, bool ack)
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
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x5b));
    } else {
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x5c));
    }

    net_buf_simple_add_le16(msg, send_arg->range_min);
    net_buf_simple_add_le16(msg, send_arg->range_max);

    ctx.addr = unicast_addr;
    ctx.net_idx = netkey_idx;
    ctx.app_idx = appkey_idx;
    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "lightness range set send fail %d", err);
        return err;
    }

    LOGI(TAG, "lightness range set min %x, max %x", send_arg->range_min, send_arg->range_max);
    return 0;
}

const struct bt_mesh_model_op g_light_lightness_cli_op[LIGHT_LIGHTNESS_CLI_OPC_NUM] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x4E), 2, _light_lightness_status},
    { BT_MESH_MODEL_OP_2(0x82, 0x52), 2, _light_lightness_linear_status},
    { BT_MESH_MODEL_OP_2(0x82, 0x54), 2, _light_lightness_last_status},
    { BT_MESH_MODEL_OP_2(0x82, 0x56), 2, _light_lightness_default_status},
    { BT_MESH_MODEL_OP_2(0x82, 0x58), 2, _light_lightness_range_status},
    BT_MESH_MODEL_OP_END,
};

#endif
