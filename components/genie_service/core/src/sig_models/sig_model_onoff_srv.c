/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <api/mesh.h>
#include "sig_models/sig_model.h"

#define BT_DBG_ENABLED 1 //IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"

struct bt_mesh_model_pub g_gen_onoff_pub = {
#ifndef CONFIG_BT_MESH_NOPUB
    .msg = NET_BUF_SIMPLE(2 + 3 + 4),
#endif
};

static void _gen_onoff_prepear_buf(struct bt_mesh_model *p_model, struct net_buf_simple *p_msg, bool is_ack)
{
    sig_model_state_t *p_state = &((sig_model_element_state_t *)p_model->user_data)->state;
    u8_t remain_time_byte = 0;

    BT_DBG("onoff cur(%d) tar(%d)", p_state->onoff[TYPE_PRESENT], p_state->onoff[TYPE_TARGET]);
#ifdef CONFIG_MESH_MODEL_TRANS
    remain_time_byte = sig_model_transition_get_remain_time_byte(p_state, is_ack);

    BT_DBG("remain(0x%02x)", remain_time_byte);
#endif

    //prepear buff
    bt_mesh_model_msg_init(p_msg, OP_GENERIC_ONOFF_STATUS);

    if (is_ack && remain_time_byte == 0)
    {
        net_buf_simple_add_u8(p_msg, p_state->onoff[TYPE_TARGET]);
    }
    else
    {
        net_buf_simple_add_u8(p_msg, p_state->onoff[TYPE_PRESENT]);
#ifdef CONFIG_MESH_MODEL_TRANS
        if (remain_time_byte)
        {
            net_buf_simple_add_u8(p_msg, p_state->onoff[TYPE_TARGET]);
            net_buf_simple_add_u8(p_msg, remain_time_byte);
        }
#endif
    }
}

static void _gen_onoff_status(struct bt_mesh_model *p_model,
                              struct bt_mesh_msg_ctx *p_ctx, bool is_ack)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 3 + 4);

    BT_DBG("addr(0x%04x)", bt_mesh_model_elem(p_model)->addr);

    _gen_onoff_prepear_buf(p_model, p_msg, is_ack);
    p_ctx->send_ttl = GENIE_TRANSPORT_DEFAULT_TTL;
    if (bt_mesh_model_send(p_model, p_ctx, p_msg, NULL, NULL))
    {
        BT_ERR("Unable to send OnOff Status");
    }
    BT_DBG("Success!!!");
}

struct net_buf_simple *p_ack_buff;

static E_MESH_ERROR_TYPE _gen_onoff_analyze(struct bt_mesh_model *p_model,
                                            u16_t src_addr, struct net_buf_simple *p_buf)
{
    u8_t onoff = 0;
    u8_t tid = 0;
#ifdef CONFIG_MESH_MODEL_TRANS
    u8_t trans = 0;
    u8_t delay = 0;
#endif
    sig_model_element_state_t *p_elem = NULL;

    if (!p_model || !p_buf)
    {
        return MESH_ANALYZE_ARGS_ERROR;
    }

    p_elem = p_model->user_data;

    if (p_buf->len != 2 && p_buf->len != 4)
    {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    //get message info
    onoff = net_buf_simple_pull_u8(p_buf);
    tid = net_buf_simple_pull_u8(p_buf);
#ifdef CONFIG_MESH_MODEL_TRANS
    if (p_buf->len)
    {
        trans = net_buf_simple_pull_u8(p_buf);
        delay = net_buf_simple_pull_u8(p_buf);
    }
    else
    {
        trans = 0;
        delay = 0;
    }
#endif

    if (onoff >> 1)
    {
        BT_ERR("MESH_ANALYZE_ARGS_ERROR onoff(0x%02x)", onoff);
        return MESH_ANALYZE_ARGS_ERROR;
    }

#ifdef CONFIG_MESH_MODEL_TRANS
    if ((trans & 0x3F) == 0x3F)
    {
        BT_ERR("MESH_SET_TRANSTION_ERROR");
        return MESH_SET_TRANSTION_ERROR;
    }
#endif

    if (genie_transport_check_tid(src_addr, tid, p_elem->element_id) != MESH_SUCCESS)
    {
        BT_ERR("MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", src_addr, tid);
        return MESH_TID_REPEAT;
    }
    genie_transport_src_addr_set(src_addr);

    p_elem->state.onoff[TYPE_TARGET] = onoff;
    BT_DBG("onoff cur(%d) tar(%d)", p_elem->state.onoff[TYPE_PRESENT], p_elem->state.onoff[TYPE_TARGET]);
#ifdef CONFIG_MESH_MODEL_TRANS
    p_elem->state.trans = trans;
    p_elem->state.delay = delay;
    if (p_elem->state.trans)
    {
        p_elem->state.trans_start_time = k_uptime_get() + p_elem->state.delay * 5;
        p_elem->state.trans_end_time = p_elem->state.trans_start_time + sig_model_transition_get_transition_time(p_elem->state.trans);
    }
    BT_DBG("trans(0x%02x) delay(0x%02x)", p_elem->state.trans, p_elem->state.delay);
#endif

    if (p_elem->state.onoff[TYPE_PRESENT] != p_elem->state.onoff[TYPE_TARGET])
    {
        sig_model_event_set_indicate(SIG_MODEL_INDICATE_GEN_ONOFF);
        sig_model_generic_onoff_bind_ops(p_elem);
    }

    return MESH_SUCCESS;
}

void gen_onoff_publication(struct bt_mesh_model *p_model)
{
}

static void _gen_onoff_get(struct bt_mesh_model *p_model,
                           struct bt_mesh_msg_ctx *p_ctx,
                           struct net_buf_simple *p_buf)
{
    sig_model_msg msg;
    sig_model_element_state_t *p_elem_state = NULL;

    if (!p_model || !p_model->user_data)
    {
        GENIE_LOG_ERR("param err");
        return;
    }

    memset(&msg, 0, sizeof(sig_model_msg));
    msg.opcode = OP_GENERIC_ONOFF_GET;
    if (p_buf != NULL)
    {
        msg.len = p_buf->len;
        msg.data = (u8_t *)p_buf->data;
    }
    genie_transport_src_addr_set(p_ctx->addr);

    p_elem_state = (sig_model_element_state_t *)p_model->user_data;
    msg.element_id = p_elem_state->element_id;

    sig_model_event(SIG_MODEL_EVT_GENERIC_MESG, (void *)&msg);

#ifndef CONFIG_GENIE_MESH_NO_AUTO_REPLY
    _gen_onoff_status(p_model, p_ctx, 0);
#endif
}

static void _gen_onoff_set(struct bt_mesh_model *p_model,
                           struct bt_mesh_msg_ctx *p_ctx,
                           struct net_buf_simple *p_buf)
{
    sig_model_msg msg;
    sig_model_element_state_t *p_elem_state = NULL;

    memset(&msg, 0, sizeof(sig_model_msg));

    msg.opcode = OP_GENERIC_ONOFF_SET;
    if (p_buf != NULL)
    {
        msg.len = p_buf->len;
        msg.data = (u8_t *)p_buf->data;
    }

    E_MESH_ERROR_TYPE ret = _gen_onoff_analyze(p_model, p_ctx->addr, p_buf);
    if (ret == MESH_SUCCESS)
    {
        p_elem_state = (sig_model_element_state_t *)p_model->user_data;
        msg.element_id = p_elem_state->element_id;
        sig_model_event(SIG_MODEL_EVT_GENERIC_MESG, (void *)&msg);
    }

#ifndef CONFIG_GENIE_MESH_NO_AUTO_REPLY
    if (ret == MESH_SUCCESS || ret == MESH_TID_REPEAT)
    {
        _gen_onoff_status(p_model, p_ctx, 1);
        if (ret == MESH_SUCCESS)
        {
            sig_model_event(SIG_MODEL_EVT_ANALYZE_MSG, (sig_model_element_state_t *)p_model->user_data);
        }
    }
#endif
}

static void _gen_onoff_set_unack(struct bt_mesh_model *p_model,
                                 struct bt_mesh_msg_ctx *p_ctx,
                                 struct net_buf_simple *p_buf)
{
    sig_model_msg msg;
    sig_model_element_state_t *p_elem_state = NULL;

    memset(&msg, 0, sizeof(sig_model_msg));
    msg.opcode = OP_GENERIC_ONOFF_SET_UNACK;
    if (p_buf != NULL)
    {
        msg.len = p_buf->len;
        msg.data = (u8_t *)p_buf->data;
    }

    E_MESH_ERROR_TYPE ret = _gen_onoff_analyze(p_model, p_ctx->addr, p_buf);
    if (ret == MESH_SUCCESS)
    {
        p_elem_state = (sig_model_element_state_t *)p_model->user_data;
        msg.element_id = p_elem_state->element_id;
        sig_model_event(SIG_MODEL_EVT_GENERIC_MESG, (void *)&msg);
#ifndef CONFIG_GENIE_MESH_NO_AUTO_REPLY
        sig_model_event(SIG_MODEL_EVT_ANALYZE_MSG, (sig_model_element_state_t *)p_model->user_data);
#endif
    }
}

const struct bt_mesh_model_op g_gen_onoff_op[] = {
    {OP_GENERIC_ONOFF_GET, 0, _gen_onoff_get},
    {OP_GENERIC_ONOFF_SET, 2, _gen_onoff_set},
    {OP_GENERIC_ONOFF_SET_UNACK, 2, _gen_onoff_set_unack},
    BT_MESH_MODEL_OP_END,
};

#ifdef CONFIG_MESH_MODEL_CTL_SRV
void sig_model_onoff_update_by_color_temperature(sig_model_element_state_t *p_elem)
{
    sig_model_state_t *p_state = &p_elem->state;

    if (p_state->onoff[TYPE_TARGET] == 0)
    {
        //turn on
        p_state->onoff[TYPE_TARGET] = 1;

        sig_model_generic_onoff_bind_ops(p_elem);
    }
}
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
void sig_model_onoff_update_by_lightness(sig_model_element_state_t *p_elem, E_VALUE_TYPE type)
{
    sig_model_state_t *p_state = &p_elem->state;

    if (type == TYPE_TARGET)
    {
        if (p_state->lightness[TYPE_PRESENT] == 0 && p_state->lightness[TYPE_TARGET] != 0)
        {
            //turn on
            p_state->onoff[TYPE_TARGET] = 1;
        }
        else if (p_state->lightness[TYPE_PRESENT] != 0 && p_state->lightness[TYPE_TARGET] == 0)
        {
            //turn off
            p_state->onoff[TYPE_TARGET] = 0;
        }
    }
    else if (type == TYPE_PRESENT)
    {
        if (p_state->lightness[TYPE_PRESENT] == 0 && p_state->lightness[TYPE_TARGET] == 0)
        {
            //turn off
            p_state->onoff[TYPE_PRESENT] = 0;
        }
    }
}
#endif
