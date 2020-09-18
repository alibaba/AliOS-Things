/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <api/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"

struct bt_mesh_model_pub g_gen_onoff_pub = {
    .msg = NET_BUF_SIMPLE(2 + 3 + 4),
};

static void _gen_onoff_prepear_buf(struct bt_mesh_model *model, struct net_buf_simple *msg, bool is_ack)
{
    S_ELEM_STATE *elem = model->user_data;
    u8_t remain_byte = get_remain_byte(&elem->state, is_ack);

    BT_DBG("cur_onoff(0x%02x) tar_onoff(0x%02x) remain(0x%02x)", elem->state.onoff[T_CUR], elem->state.onoff[T_TAR], remain_byte);

    //prepear buff
    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x04));
    
    net_buf_simple_add_u8(msg, elem->state.onoff[T_CUR]);
    
    if(remain_byte){
        net_buf_simple_add_u8(msg, elem->state.onoff[T_TAR]);
        net_buf_simple_add_u8(msg, remain_byte);
    }
}

static void _gen_onoff_status(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx, bool is_ack)
{
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 3 + 4);
    
    BT_DBG("addr(0x%04x)", model->elem->addr);

    _gen_onoff_prepear_buf(model, msg, is_ack);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        BT_ERR("Unable to send OnOff Status");
    }
    BT_DBG("Success!!!");
}

static E_MESH_ERROR_TYPE _gen_onoff_analyze(struct bt_mesh_model *model, u16_t src_addr, struct net_buf_simple *buf)
{
    u8_t onoff = 0;
    u8_t tid = 0;
    u8_t trans = 0;
    u8_t delay = 0;
    E_MESH_ERROR_TYPE ret = MESH_SUCCESS;
    S_ELEM_STATE *elem = model->user_data;

    if(buf->len != 2 && buf->len != 4) {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR buf->len(%d)", buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    //get message info
    onoff = net_buf_simple_pull_u8(buf);
    tid = net_buf_simple_pull_u8(buf);
    if(buf->len) {
        trans = net_buf_simple_pull_u8(buf);
        delay = net_buf_simple_pull_u8(buf);
    } else {
        trans = elem->powerup.def_trans;
        delay = 0;
    }
    
    if(onoff >> 1) {
        BT_ERR("MESH_ANALYZE_ARGS_ERROR onoff(0x%02x)", onoff);
        return MESH_ANALYZE_ARGS_ERROR;
    }

    if((trans & 0x3F) == 0x3F) {
        BT_ERR("MESH_SET_TRANSTION_ERROR");
        return MESH_SET_TRANSTION_ERROR;
    }

    if(mesh_check_tid(src_addr, tid) != MESH_SUCCESS) {
        BT_ERR("MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", src_addr, tid);
        return MESH_TID_REPEAT;
    }
    
    elem->powerup.last_onoff = elem->state.onoff[T_CUR];

    elem->state.onoff[T_TAR] = onoff;
    elem->state.trans = trans;
    elem->state.delay = delay;
    if(elem->state.trans) {
        elem->state.trans_start_time = k_uptime_get() + elem->state.delay*5;
        elem->state.trans_end_time = elem->state.trans_start_time + get_transition_time(elem->state.trans);
    }

    BT_DBG("tar_onoff(0x%02x) trans(0x%02x) delay(0x%02x)",
        elem->state.onoff[T_TAR], elem->state.trans, elem->state.delay);

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

void gen_onoff_publication(struct bt_mesh_model *model)
{
    struct net_buf_simple *msg = model->pub->msg;
    int err;

    if(!model) return;

    BT_DBG("addr(0x%04x)", model->pub->addr);

    /*
     * If a server has a publish address, it is required to
     * publish status on a state change
     *
     * See Mesh Profile Specification 3.7.6.1.2
     *
     * Only publish if there is an assigned address
     */
    if (model->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        _gen_onoff_prepear_buf(model, msg, 0);
        
        err = bt_mesh_model_publish(model);
        if (err) {
            BT_ERR("bt_mesh_model_publish err %d\n", err);
        }
        BT_DBG("Success!!!");
    }
}

static void _gen_onoff_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    BT_DBG("");

    _gen_onoff_status(model, ctx, 0);
}

static void _gen_onoff_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    E_MESH_ERROR_TYPE ret = _gen_onoff_analyze(model, ctx->addr, buf);

    BT_DBG("ret %d", ret);
    
    if(ret == MESH_SUCCESS || ret == MESH_TID_REPEAT) {
        _gen_onoff_status(model, ctx, 1);
    }
}

static void _gen_onoff_set_unack(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
    BT_DBG("");
    
    _gen_onoff_analyze(model, ctx->addr, buf);
}

const struct bt_mesh_model_op g_gen_onoff_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x01), 0, _gen_onoff_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x02), 2, _gen_onoff_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x03), 2, _gen_onoff_set_unack },
    BT_MESH_MODEL_OP_END,
};

