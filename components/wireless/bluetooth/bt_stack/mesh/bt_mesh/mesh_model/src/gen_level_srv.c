/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <api/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"

struct bt_mesh_model_pub g_gen_level_pub = {
    .msg = NET_BUF_SIMPLE(2 + 5 + 4),
};

static void _gen_level_prepear_buf(struct bt_mesh_model *model, struct net_buf_simple *msg, bool is_ack)
{
    S_ELEM_STATE *elem = model->user_data;
    u8_t remain_byte = get_remain_byte(is_ack);

    BT_DBG("cur_level(0x%04x) tar_level(0x%04x) remain(0x%02x)", (u16_t)elem->state.level[T_CUR], (u16_t)elem->state.level[T_TAR], remain_byte);

    //prepear buff
    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x08));
    
    net_buf_simple_add_le16(msg, elem->state.level[T_CUR]);

    if(remain_byte){
        net_buf_simple_add_le16(msg, elem->state.level[T_TAR]);
        net_buf_simple_add_u8(msg, remain_byte);
    }
}

static void _gen_level_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, bool is_ack)
{
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 5 + 4);
    
    BT_DBG("addr(0x%04x)", model->elem->addr);

    _gen_level_prepear_buf(model, msg, is_ack);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        BT_ERR("Unable to send Level Status");
    }
    BT_DBG("Success!!!");
}

static u8_t _gen_level_analyze(struct bt_mesh_model *model, u16_t src_addr, struct net_buf_simple *buf)
{
    s16_t level = 0;
    u8_t tid = 0;
    u8_t trans = 0;
    u8_t delay = 0;
    E_MESH_ERROR_TYPE ret = MESH_SUCCESS;
    S_ELEM_STATE *elem = model->user_data;

    if(buf->len != 3 && buf->len != 5) {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR buf->len(%d)", buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }
    
    //get message info
    level = (s16_t)net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);
    if(buf->len) {
        trans = net_buf_simple_pull_u8(buf);
        delay = net_buf_simple_pull_u8(buf);
    } else {
        trans = elem->powerup.def_trans;
        delay = 0;
    }

    if((trans & 0x3F) == 0x3F) {
        BT_ERR("MESH_SET_TRANSTION_ERROR");
        return MESH_SET_TRANSTION_ERROR;
    }
    
    if(mesh_check_tid(src_addr, tid) != MESH_SUCCESS) {
        BT_ERR("MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", src_addr, tid);
        return MESH_TID_REPEAT;
    }

#if 0
    if(level) {
        elem->powerup.last_level = elem->state.onoff[T_TAR] = level;
        elem->state.level[T_TAR] = level;
    } else {
    }
    //mesh_state_bound(GENERIC_LEVEL, T_TAR);
#endif

    elem->state.level[T_TAR] = level;
    elem->state.trans = trans;
    elem->state.delay = delay;
    if(elem->state.trans) {
        elem->state.trans_start_time = k_uptime_get() + elem->state.delay*5;
        elem->state.trans_end_time = elem->state.trans_start_time + get_transition_time(elem->state.trans);
    }
    
    BT_DBG("level(0x%04x) trans(0x%02x) delay(0x%02x)",
        elem->state.level[T_TAR], elem->state.trans, elem->state.delay);
    BT_DBG("start(%d) end(%d)", (u32_t)elem->state.trans_start_time, (u32_t)elem->state.trans_end_time);

    genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (void *)elem);

    if(elem->state.trans || elem->state.delay) {
        if(elem->state.delay) {
            genie_event(GENIE_EVT_SDK_DELAY_START, (void *)elem);
        } else {
            genie_event(GENIE_EVT_SDK_TRANS_START, (void *)elem);
        }
    } else {
        genie_event(GENIE_EVT_SDK_ACTION_DONE, (void *)elem);
    }

    return MESH_SUCCESS;
}

void gen_level_publication(struct bt_mesh_model *model)
{
    struct net_buf_simple *msg = model->pub->msg;
    int err;

    if(!model) return;

    BT_DBG("addr(0x%04x)", model->pub->addr);

    if (model->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        _gen_level_prepear_buf(model, msg, 0);
        
        err = bt_mesh_model_publish(model);
        if (err) {
            BT_ERR("bt_mesh_model_publish err %d\n", err);
        }
        BT_DBG("Success!!!");
    }
}

static void _gen_level_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    BT_DBG("");

    _gen_level_status(model, ctx, 0);
}

static void _gen_level_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    E_MESH_ERROR_TYPE ret = _gen_level_analyze(model, ctx->addr, buf);

    BT_DBG("ret %d", ret);

    if(ret == MESH_SUCCESS || ret == MESH_TID_REPEAT) {
        _gen_level_status(model, ctx, 1);
    }
}

static void _gen_level_set_unack(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
    BT_DBG("");

    _gen_level_analyze(model, ctx->addr, buf);
}

static s32_t _format_32to16(s32_t value)
{
    if(value < (s16_t)0x8000) {
        value = (s16_t)0x8000;
    } else if(value > (s16_t)0x7FFF){
        value = (s16_t)0x7FFF;
    }
    return value;
}

static u8_t _gen_delta_analyze(struct bt_mesh_model *model, u16_t src_addr, struct net_buf_simple *buf)
{
    s32_t delta = 0;
    s32_t target = 0;
    u8_t tid = 0;
    u8_t trans = 0;
    u8_t delay = 0;
    E_MESH_ERROR_TYPE ret = MESH_SUCCESS;
    S_ELEM_STATE *elem = model->user_data;

    if(buf->len != 5 && buf->len != 7) {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR buf->len(%d)", buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }
    
    //get message info
    delta = (s32_t)net_buf_simple_pull_le32(buf);
    delta = _format_32to16(delta);
    tid = net_buf_simple_pull_u8(buf);
    if(buf->len) {
        trans = net_buf_simple_pull_u8(buf);
        delay = net_buf_simple_pull_u8(buf);
    } else {
        trans = elem->powerup.def_trans;
        delay = 0;
    }

    if((trans & 0x3F) == 0x3F) {
        BT_ERR("MESH_SET_TRANSTION_ERROR");
        return MESH_SET_TRANSTION_ERROR;
    }

    if(mesh_check_tid(src_addr, tid) == MESH_TID_REPEAT) {
        target = elem->state.trans_level + delta;
    } else {
        elem->state.trans_level = elem->state.level[T_CUR];
        target = elem->state.level[T_CUR] + delta;
    }

    target = _format_32to16(target);
    elem->state.level[T_TAR] = (s16_t)target;
    elem->state.trans = trans;
    elem->state.delay = delay;
    if(elem->state.trans) {
        elem->state.trans_start_time = k_uptime_get() + elem->state.delay*5;
        elem->state.trans_end_time = elem->state.trans_start_time + get_transition_time(elem->state.trans);
    }

    BT_DBG("delta(0x%04x)(%d) tar_level(0x%04x) trans(0x%02x) delay(0x%02x)",
        delta, delta, elem->state.level[T_TAR], elem->state.trans, elem->state.delay);
    BT_DBG("start(%d) end(%d)", (u32_t)elem->state.trans_start_time, (u32_t)elem->state.trans_end_time);
    
    genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (void *)elem);

    if(elem->state.trans || elem->state.delay) {
        if(elem->state.delay) {
            genie_event(GENIE_EVT_SDK_DELAY_START, (void *)elem);
        } else {
            genie_event(GENIE_EVT_SDK_TRANS_START, (void *)elem);
        }
    } else {
        genie_event(GENIE_EVT_SDK_ACTION_DONE, (void *)elem);
    }

    return MESH_SUCCESS;
}

static void _gen_delta_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    E_MESH_ERROR_TYPE ret = _gen_delta_analyze(model, ctx->addr, buf);

    BT_DBG("ret %d", ret);

    if(ret == MESH_SUCCESS || ret == MESH_TID_REPEAT) {
        _gen_level_status(model, ctx, 1);
    }
}

static void _gen_delta_set_unack(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
    BT_DBG("");

    _gen_delta_analyze(model, ctx->addr, buf);
}

static u8_t _gen_level_move_analyze(struct bt_mesh_model *model, u16_t src_addr, struct net_buf_simple *buf)
{
    s16_t delta_move = 0;
    u8_t tid = 0;
    u8_t trans = 0;
    u8_t delay = 0;
    E_MESH_ERROR_TYPE ret = MESH_SUCCESS;
    S_ELEM_STATE *elem = model->user_data;

    if(buf->len != 3 && buf->len != 5) {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR buf->len(%d)", buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }
    
    //get message info
    delta_move = (s16_t)net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);
    if(buf->len) {
        trans = net_buf_simple_pull_u8(buf);
        delay = net_buf_simple_pull_u8(buf);
    } else {
        trans = elem->powerup.def_trans;
        delay = 0;
    }

    if(mesh_check_tid(src_addr, tid) != MESH_SUCCESS) {
        BT_ERR("MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", src_addr, tid);
        return MESH_TID_REPEAT;
    }

    if((trans & 0x3F) == 0x3F) {
        BT_ERR("MESH_SET_TRANSTION_ERROR");
        return MESH_SET_TRANSTION_ERROR;
    }

#if 0
    if(l_trans) {
        l_trans = 0x3F;
    }

    mesh_state_bound(GENERIC_LEVEL, T_TAR);
#endif
    if(delta_move > 0) {
        elem->state.level[T_TAR] = (s16_t)0x7FFF;
    } else {
        elem->state.level[T_TAR] = (s16_t)0x8000;
    }
    elem->state.trans = trans;
    elem->state.delay = delay;
    if(elem->state.trans) {
        elem->state.trans_start_time = k_uptime_get() + elem->state.delay*5;
        elem->state.trans_end_time = elem->state.trans_start_time + get_transition_time(elem->state.trans);
    }

    BT_DBG("delta_move(0x%04x)(%d) tar_level(0x%04x) trans(0x%02x) delay(0x%02x)",
        delta_move, delta_move, elem->state.level[T_TAR], elem->state.trans, elem->state.delay);
    BT_DBG("start(%d) end(%d)", (u32_t)elem->state.trans_start_time, (u32_t)elem->state.trans_end_time);
    
    genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (void *)elem);

    if(elem->state.trans || elem->state.delay) {
        if(elem->state.delay) {
            genie_event(GENIE_EVT_SDK_DELAY_START, (void *)elem);
        } else {
            genie_event(GENIE_EVT_SDK_TRANS_START, (void *)elem);
        }
    } else {
        genie_event(GENIE_EVT_SDK_ACTION_DONE, (void *)elem);
    }

    return MESH_SUCCESS;
}

static void _gen_move_set(struct bt_mesh_model *model,
                         struct bt_mesh_msg_ctx *ctx,
                         struct net_buf_simple *buf)
{
    E_MESH_ERROR_TYPE ret = _gen_level_move_analyze(model, ctx->addr, buf);

    BT_DBG("ret %d", ret);
    
    if(ret == MESH_SUCCESS || ret == MESH_TID_REPEAT) {
        _gen_level_status(model, ctx, 1);
    }
}

static void _gen_move_set_unack(struct bt_mesh_model *model,
                               struct bt_mesh_msg_ctx *ctx,
                               struct net_buf_simple *buf)
{
    BT_DBG("");
    
    _gen_level_move_analyze(model, ctx->addr, buf);
}

const struct bt_mesh_model_op g_gen_level_op[GEN_LV_OPC_NUM] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x05), 0, _gen_level_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x06), 3, _gen_level_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x07), 3, _gen_level_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x09), 5, _gen_delta_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x0a), 5, _gen_delta_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x0b), 3, _gen_move_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x0c), 3, _gen_move_set_unack },
    BT_MESH_MODEL_OP_END,
};

void bound_level_by_onoff(S_ELEM_STATE *elem)
{
    if(elem->state.onoff[T_CUR] == 1 && elem->state.onoff[T_TAR] == 1)
    {
        elem->state.level[T_CUR] = elem->powerup.last_level;
    }
}


