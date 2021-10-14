/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <api/mesh.h>

#include "mesh_model/mesh_model.h"

#define TAG "GEN_LEVEL_SRV"
#if defined(CONFIG_BT_MESH_MODEL_GEN_LEVEL_SRV)

static int _generic_level_prepare_publication(struct bt_mesh_model *model);

struct bt_mesh_model_pub g_generic_level_pub = {
    .msg = NET_BUF_SIMPLE(2 + 5 + 4),
    .update = _generic_level_prepare_publication,
};

extern u8_t get_remain_byte(S_MESH_STATE *p_state, bool is_ack);
extern uint8_t mesh_check_tid(u16_t src_addr, u8_t tid);
extern bt_u32_t get_transition_time(u8_t byte);

static void _generic_level_prepear_buf(struct bt_mesh_model *model, struct net_buf_simple *msg, bool is_ack)
{
    S_ELEM_STATE *elem = model->user_data;
    u8_t remain_byte = 0;

    LOGD(TAG, "cur_level(0x%04x) tar_level(0x%04x) remain(0x%02x)", (u16_t)elem->state.level[T_CUR], (u16_t)elem->state.level[T_TAR], remain_byte);

    //prepear buff
    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x08));
    net_buf_simple_add_le16(msg, elem->state.level[T_CUR]);

    if (elem->state.level[T_TAR] != elem->state.level[T_CUR]) {
        remain_byte = get_remain_byte(&elem->state ,is_ack);
        net_buf_simple_add_le16(msg, elem->state.level[T_TAR]);
        net_buf_simple_add_u8(msg, remain_byte);
    }
}

static void _generic_level_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, bool is_ack)
{
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 5 + 4);

    //LOGD(TAG, "addr(0x%04x)", model->elem->addr);

    _generic_level_prepear_buf(model, msg, is_ack);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        LOGE(TAG, "Unable to send Level Status");
    }

    LOGD(TAG, "Success!!!");
}

static u8_t _generic_level_analyze(struct bt_mesh_model *model, u16_t src_addr, struct net_buf_simple *buf)
{
    s16_t level = 0;
    u8_t tid = 0;
    u8_t trans = 0;
    u8_t delay = 0;

    S_ELEM_STATE *elem = model->user_data;

    if (buf->len != 3 && buf->len != 5) {
        LOGE(TAG, "MESH_ANALYZE_SIZE_ERROR buf->len(%d)", buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    //get message info
    level = (s16_t)net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (buf->len) {
        trans = net_buf_simple_pull_u8(buf);
        delay = net_buf_simple_pull_u8(buf);
    } else {
        // trans = elem->powerup.def_trans;
        // delay = 0;
    }

    if ((trans & 0x3F) == 0x3F) {
        LOGE(TAG, "MESH_SET_TRANSTION_ERROR");
        return MESH_SET_TRANSTION_ERROR;
    }

    if (mesh_check_tid(src_addr, tid) != MESH_SUCCESS) {
        LOGE(TAG, "MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", src_addr, tid);
        return MESH_TID_REPEAT;
    }

#if 0

    if (level) {
        elem->powerup.last_level = elem->state.onoff[T_TAR] = level;
        elem->state.level[T_TAR] = level;
    } else {
    }

    //mesh_state_bound(GENERIC_LEVEL, T_TAR);
#endif

    elem->state.level[T_TAR] = level;
    elem->state.trans = trans;
    elem->state.delay = delay;

    if (elem->state.trans) {
        elem->state.trans_start_time = k_uptime_get() + elem->state.delay * 5;
        elem->state.trans_end_time = elem->state.trans_start_time + get_transition_time(elem->state.trans);
    }

    LOGD(TAG, "level(0x%04x) trans(0x%02x) delay(0x%02x)",
           elem->state.level[T_TAR], elem->state.trans, elem->state.delay);
    LOGD(TAG, "start(%d) end(%d)", (bt_u32_t)elem->state.trans_start_time, (bt_u32_t)elem->state.trans_end_time);

    //model_event(GEN_EVT_SDK_ANALYZE_MSG, (void *)elem);
    if (elem->state.trans || elem->state.delay) {
        if (elem->state.delay) {
            //model_event(GEN_EVT_SDK_DELAY_START, (void *)elem);
        } else {
            //model_event(GEN_EVT_SDK_TRANS_START, (void *)elem);
        }
    } else {
        elem->state.level[T_CUR] = elem->state.level[T_TAR];
        model_message message;
        message.source_addr = src_addr;
        message.status_data = buf;
        message.user_data = elem;
        model_event(BT_MESH_MODEL_LEVEL_SET, (void *)&message);
    }

    return MESH_SUCCESS;
}

static int _generic_level_prepare_publication(struct bt_mesh_model *model)
{
    struct net_buf_simple *msg;

    if (!model) {
        return -1;
    }

    msg = model->pub->msg;

    LOGD(TAG, "addr(0x%04x)", model->pub->addr);

    if (model->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        _generic_level_prepear_buf(model, msg, 0);
    }

    return 0;
}

int ble_mesh_generic_level_publication(struct bt_mesh_model *model)
{
    struct net_buf_simple *msg;
    int16_t ret;

    if (!model) {
        return -1;
    }

    msg = model->pub->msg;

    LOGD(TAG, "addr(0x%04x)", model->pub->addr);

    if (model->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        _generic_level_prepear_buf(model, msg, 0);

        ret = bt_mesh_model_publish(model);

        if (ret) {
            LOGE(TAG, "bt_mesh_model_publish err %d\n", ret);
            return ret;
        }

        LOGD(TAG, "Success!!!");
    }

    return 0;
}

static void _generic_level_get(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    LOGD(TAG, "");

    _generic_level_status(model, ctx, 0);
}

static void _generic_level_set(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    E_MESH_ERROR_TYPE ret = _generic_level_analyze(model, ctx->addr, buf);

    LOGD(TAG, "ret %d", ret);

    if (ret == MESH_SUCCESS || ret == MESH_TID_REPEAT) {
        _generic_level_status(model, ctx, 1);
    }
}

static void _generic_level_set_unack(struct bt_mesh_model *model,
                                 struct bt_mesh_msg_ctx *ctx,
                                 struct net_buf_simple *buf)
{
    LOGD(TAG, "");

    _generic_level_analyze(model, ctx->addr, buf);
}

static bt_s32_t _format_32to16(bt_s32_t value)
{
    if (value < (s16_t)0x8000) {
        value = (s16_t)0x8000;
    } else if (value > (s16_t)0x7FFF) {
        value = (s16_t)0x7FFF;
    }

    return value;
}

static u8_t _generic_delta_analyze(struct bt_mesh_model *model, u16_t src_addr, struct net_buf_simple *buf)
{
    bt_s32_t delta = 0;
    bt_s32_t target = 0;
    u8_t tid = 0;
    u8_t trans = 0;
    u8_t delay = 0;

    S_ELEM_STATE *elem = model->user_data;

    if (buf->len != 5 && buf->len != 7) {
        LOGE(TAG, "MESH_ANALYZE_SIZE_ERROR buf->len(%d)", buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    //get message info
    delta = (bt_s32_t)net_buf_simple_pull_le32(buf);
    delta = _format_32to16(delta);
    tid = net_buf_simple_pull_u8(buf);

    if (buf->len) {
        trans = net_buf_simple_pull_u8(buf);
        delay = net_buf_simple_pull_u8(buf);
    } else {
        //trans = elem->powerup.def_trans;
        //delay = 0;
    }

    if ((trans & 0x3F) == 0x3F) {
        LOGE(TAG, "MESH_SET_TRANSTION_ERROR");
        return MESH_SET_TRANSTION_ERROR;
    }

    if (mesh_check_tid(src_addr, tid) == MESH_TID_REPEAT) {
        //target = elem->state.trans_level + delta;
    } else {
        //elem->state.trans_level = elem->state.level[T_CUR];
        target = elem->state.level[T_CUR] + delta;
    }

    target = _format_32to16(target);
    elem->state.level[T_TAR] = (s16_t)target;
    elem->state.trans = trans;
    elem->state.delay = delay;

    if (elem->state.trans) {
        elem->state.trans_start_time = k_uptime_get() + elem->state.delay * 5;
        elem->state.trans_end_time = elem->state.trans_start_time + get_transition_time(elem->state.trans);
    }

    LOGD(TAG, "delta(0x%04x)(%d) tar_level(0x%04x) trans(0x%02x) delay(0x%02x)",
           delta, delta, elem->state.level[T_TAR], elem->state.trans, elem->state.delay);
    LOGD(TAG, "start(%d) end(%d)", (bt_u32_t)elem->state.trans_start_time, (bt_u32_t)elem->state.trans_end_time);

    if (elem->state.trans || elem->state.delay) {
        if (elem->state.delay) {
            //model_event(GEN_EVT_SDK_DELAY_START, (void *)elem);
        } else {
            //model_event(GEN_EVT_SDK_TRANS_START, (void *)elem);
        }
    } else {
        elem->state.level[T_CUR] = elem->state.level[T_TAR];
        model_message message;
        message.source_addr = src_addr;
        message.status_data = buf;
        message.user_data = elem;
        model_event(BT_MESH_MODEL_LEVEL_DELTA_SET, (void *)&message);
    }

    return MESH_SUCCESS;
}

static void _generic_delta_set(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    E_MESH_ERROR_TYPE ret = _generic_delta_analyze(model, ctx->addr, buf);

    LOGD(TAG, "ret %d", ret);

    if (ret == MESH_SUCCESS || ret == MESH_TID_REPEAT) {
        _generic_level_status(model, ctx, 1);
    }
}

static void _generic_delta_set_unack(struct bt_mesh_model *model,
                                 struct bt_mesh_msg_ctx *ctx,
                                 struct net_buf_simple *buf)
{
    LOGD(TAG, "");

    _generic_delta_analyze(model, ctx->addr, buf);
}

static u8_t _generic_level_move_analyze(struct bt_mesh_model *model, u16_t src_addr, struct net_buf_simple *buf)
{
    s16_t delta_move = 0;
    u8_t tid = 0;
    u8_t trans = 0;
    u8_t delay = 0;

    S_ELEM_STATE *elem = model->user_data;

    if (buf->len != 3 && buf->len != 5) {
        LOGE(TAG, "MESH_ANALYZE_SIZE_ERROR buf->len(%d)", buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    //get message info
    delta_move = (s16_t)net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (buf->len) {
        trans = net_buf_simple_pull_u8(buf);
        delay = net_buf_simple_pull_u8(buf);
    } else {
        //trans = elem->powerup.def_trans;
        delay = 0;
    }

    if (mesh_check_tid(src_addr, tid) != MESH_SUCCESS) {
        LOGE(TAG, "MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", src_addr, tid);
        return MESH_TID_REPEAT;
    }

    if ((trans & 0x3F) == 0x3F) {
        LOGE(TAG, "MESH_SET_TRANSTION_ERROR");
        return MESH_SET_TRANSTION_ERROR;
    }

    if (delta_move > 0) {
        elem->state.level[T_TAR] = (s16_t)0x7FFF;
    } else {
        elem->state.level[T_TAR] = (s16_t)0x8000;
    }

    elem->state.trans = trans;
    elem->state.delay = delay;

    if (elem->state.trans) {
        elem->state.trans_start_time = k_uptime_get() + elem->state.delay * 5;
        elem->state.trans_end_time = elem->state.trans_start_time + get_transition_time(elem->state.trans);
    }

    LOGD(TAG, "delta_move(0x%04x)(%d) tar_level(0x%04x) trans(0x%02x) delay(0x%02x)",
           delta_move, delta_move, elem->state.level[T_TAR], elem->state.trans, elem->state.delay);
    LOGD(TAG, "start(%d) end(%d)", (bt_u32_t)elem->state.trans_start_time, (bt_u32_t)elem->state.trans_end_time);


    if (elem->state.trans || elem->state.delay) {
        if (elem->state.delay) {
            //model_event(GEN_EVT_SDK_DELAY_START, (void *)elem);
        } else {
            //model_event(GEN_EVT_SDK_TRANS_START, (void *)elem);
        }
    } else {
        model_message message;
        message.source_addr = src_addr;
        message.status_data = buf;
        message.user_data = elem;
        model_event(BT_MESH_MODEL_LEVEL_MOVE_SET, (void *)&message);
    }

    return MESH_SUCCESS;
}

static void _generic_move_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    E_MESH_ERROR_TYPE ret = _generic_level_move_analyze(model, ctx->addr, buf);

    LOGD(TAG, "ret %d", ret);

    if (ret == MESH_SUCCESS || ret == MESH_TID_REPEAT) {
        _generic_level_status(model, ctx, 1);
    }
}

static void _generic_move_set_unack(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
    LOGD(TAG, "");

    _generic_level_move_analyze(model, ctx->addr, buf);
}

const struct bt_mesh_model_op g_generic_level_op[GEN_LV_OPC_NUM] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x05), 0, _generic_level_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x06), 3, _generic_level_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x07), 3, _generic_level_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x09), 5, _generic_delta_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x0a), 5, _generic_delta_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x0b), 3, _generic_move_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x0c), 3, _generic_move_set_unack },
    BT_MESH_MODEL_OP_END,
};

#endif
