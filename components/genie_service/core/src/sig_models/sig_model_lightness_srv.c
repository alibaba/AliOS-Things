/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <api/mesh.h>
#include "sig_models/sig_model.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)

#include "common/log.h"
#include "access.h"

struct bt_mesh_model_pub g_lightness_pub = {
    .msg = NET_BUF_SIMPLE(2 + 5 + 4),
};

static void _lightness_prepare_buf(struct bt_mesh_model *p_model, struct net_buf_simple *p_msg, bool is_ack)
{
    sig_model_state_t *p_state = &((sig_model_element_state_t *)p_model->user_data)->state;
    u8_t remain_time_byte = 0;

    BT_DBG("lightness cur(0x%04x) tar(0x%04x)", p_state->lightness[TYPE_PRESENT], p_state->lightness[TYPE_TARGET]);
#ifdef CONFIG_MESH_MODEL_TRANS
    remain_time_byte = sig_model_transition_get_remain_time_byte(p_state, is_ack);

    BT_DBG("remain(0x%02x)", remain_time_byte);
#endif

    //prepear buff
    bt_mesh_model_msg_init(p_msg, OP_GENERIC_LIGHTNESS_STATUS);
    if (is_ack && remain_time_byte == 0)
    {
        net_buf_simple_add_le16(p_msg, p_state->lightness[TYPE_TARGET]);
    }
    else
    {
        net_buf_simple_add_le16(p_msg, p_state->lightness[TYPE_PRESENT]);

#ifdef CONFIG_MESH_MODEL_TRANS
        if (remain_time_byte)
        {
            net_buf_simple_add_le16(p_msg, p_state->lightness[TYPE_TARGET]);
            net_buf_simple_add_u8(p_msg, remain_time_byte);
        }
#endif
    }
}

static void _lightness_status(struct bt_mesh_model *p_model,
                              struct bt_mesh_msg_ctx *p_ctx, bool is_ack)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 5 + 4);

    BT_DBG("addr(0x%04x)", bt_mesh_model_elem(p_model)->addr);

    _lightness_prepare_buf(p_model, p_msg, is_ack);
    p_ctx->send_ttl = GENIE_TRANSPORT_DEFAULT_TTL;
    if (bt_mesh_model_send(p_model, p_ctx, p_msg, NULL, NULL))
    {
        BT_ERR("Unable to send lightness Status");
    }
    BT_DBG("Success!!!");
}

static E_MESH_ERROR_TYPE _lightness_analyze(struct bt_mesh_model *p_model,
                                            u16_t src_addr, struct net_buf_simple *p_buf)
{
    s16_t lightness = 0;
    u8_t tid = 0;
#ifdef CONFIG_MESH_MODEL_TRANS
    u8_t trans = 0;
    u8_t delay = 0;
#endif
    sig_model_element_state_t *p_elem = NULL;

    if (!p_model || !p_buf)
        return MESH_ANALYZE_ARGS_ERROR;

    p_elem = p_model->user_data;

    if (p_buf->len != 3 && p_buf->len != 5)
    {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    lightness = (s16_t)net_buf_simple_pull_le16(p_buf);
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

    //light_set_target_with_ctl(lightness, p_elem->state.color_temperature[TYPE_TARGET]);

    p_elem->state.lightness[TYPE_TARGET] = lightness;
    if (p_elem->state.lightness[TYPE_TARGET])
    {
        p_elem->powerup.last_lightness = p_elem->state.lightness[TYPE_TARGET];
    }
    BT_DBG("lightness cur(%04x) tar(%04x)", p_elem->state.lightness[TYPE_PRESENT], p_elem->state.lightness[TYPE_TARGET]);
#ifdef CONFIG_MESH_MODEL_TRANS
    p_elem->state.trans = trans;
    p_elem->state.delay = delay;
    if (p_elem->state.trans)
    {
        p_elem->state.trans_start_time = k_uptime_get() + p_elem->state.delay * 5; // delay is in 5 millisecond steps
        p_elem->state.trans_end_time = p_elem->state.trans_start_time + sig_model_transition_get_transition_time(p_elem->state.trans);
    }
    BT_DBG("trans(0x%02x) delay(0x%02x)", p_elem->state.trans, p_elem->state.delay);
#endif

    if (p_elem->state.lightness[TYPE_PRESENT] != p_elem->state.lightness[TYPE_TARGET])
    {
        sig_model_event_set_indicate(SIG_MODEL_INDICATE_GEN_LIGHTNESS);
        sig_model_generic_lightness_bind_ops(p_elem, TYPE_TARGET);
    }

    return MESH_SUCCESS;
}

static void _lightness_get(struct bt_mesh_model *p_model,
                           struct bt_mesh_msg_ctx *p_ctx,
                           struct net_buf_simple *p_buf)
{
    BT_DBG("");

    sig_model_msg msg;
    sig_model_element_state_t *p_elem_state = NULL;

    if (!p_model || !p_model->user_data)
    {
        GENIE_LOG_ERR("param err");
        return;
    }

    memset(&msg, 0, sizeof(sig_model_msg));
    msg.opcode = OP_GENERIC_LIGHTNESS_GET;
    if (p_buf != NULL)
    {
        msg.len = p_buf->len;
        msg.data = (u8_t *)p_buf->data;
    }
    genie_transport_src_addr_set(p_ctx->addr);

    p_elem_state = (sig_model_element_state_t *)p_model->user_data;
    msg.element_id = p_elem_state->element_id;

    sig_model_event(SIG_MODEL_EVT_GENERIC_MESG, (void *)&msg);

    _lightness_status(p_model, p_ctx, 0);
}

static void _lightness_set(struct bt_mesh_model *p_model,
                           struct bt_mesh_msg_ctx *p_ctx,
                           struct net_buf_simple *p_buf)
{
    E_MESH_ERROR_TYPE ret = 0;
    sig_model_msg msg;
    sig_model_element_state_t *p_elem_state = NULL;

    if (!p_model || !p_model->user_data)
    {
        printf("param err");
        return;
    }

    memset(&msg, 0, sizeof(sig_model_msg));
    msg.opcode = OP_GENERIC_LIGHTNESS_SET;
    if (p_buf != NULL)
    {
        msg.len = p_buf->len;
        msg.data = (u8_t *)p_buf->data;
    }
    genie_transport_src_addr_set(p_ctx->addr);

    p_elem_state = (sig_model_element_state_t *)p_model->user_data;
    msg.element_id = p_elem_state->element_id;

    sig_model_event(SIG_MODEL_EVT_GENERIC_MESG, (void *)&msg);

    ret = _lightness_analyze(p_model, p_ctx->addr, p_buf);

    BT_DBG("ret %d", ret);

    if (ret == MESH_SUCCESS || ret == MESH_TID_REPEAT)
    {
        _lightness_status(p_model, p_ctx, 1);
        if (ret == MESH_SUCCESS)
        {
            sig_model_event(SIG_MODEL_EVT_ANALYZE_MSG, (sig_model_element_state_t *)p_model->user_data);
        }
    }
}

static void _lightness_set_unack(struct bt_mesh_model *p_model,
                                 struct bt_mesh_msg_ctx *p_ctx,
                                 struct net_buf_simple *p_buf)
{
    BT_DBG("");

    sig_model_msg msg;
    sig_model_element_state_t *p_elem_state = NULL;

    if (!p_model || !p_model->user_data)
    {
        GENIE_LOG_ERR("param err");
        return;
    }

    memset(&msg, 0, sizeof(sig_model_msg));
    msg.opcode = OP_GENERIC_LIGHTNESS_SET_UNACK;
    if (p_buf != NULL)
    {
        msg.len = p_buf->len;
        msg.data = (u8_t *)p_buf->data;
    }
    genie_transport_src_addr_set(p_ctx->addr);

    p_elem_state = (sig_model_element_state_t *)p_model->user_data;
    msg.element_id = p_elem_state->element_id;

    sig_model_event(SIG_MODEL_EVT_GENERIC_MESG, (void *)&msg);

    E_MESH_ERROR_TYPE ret = _lightness_analyze(p_model, p_ctx->addr, p_buf);

    if (ret == MESH_SUCCESS)
    {
        sig_model_event(SIG_MODEL_EVT_ANALYZE_MSG, (sig_model_element_state_t *)p_model->user_data);
    }
}

const struct bt_mesh_model_op g_lightness_op[LIGHTNESS_OPC_NUM] = {

    /* Light Lightness */
    {OP_GENERIC_LIGHTNESS_GET, 0, _lightness_get},
    {OP_GENERIC_LIGHTNESS_SET, 2, _lightness_set}, /* bind operation executed after action done in genie_mesh */
    {OP_GENERIC_LIGHTNESS_SET_UNACK, 2, _lightness_set_unack},
    BT_MESH_MODEL_OP_END,
};

void sig_model_lightness_update_by_onoff(sig_model_element_state_t *p_elem)
{
    sig_model_state_t *p_state = &p_elem->state;

    BT_DBG("onoff cur(%d) tar(%d)", p_state->onoff[TYPE_PRESENT], p_state->onoff[TYPE_TARGET]);

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    if (p_state->onoff[TYPE_PRESENT] == 0 && p_state->onoff[TYPE_TARGET] == 1)
    {
        //turn on, set default lightness
        if (p_elem->powerup.last_lightness)
        {
            p_state->lightness[TYPE_TARGET] = p_elem->powerup.last_lightness;
        }
        else
        {
            p_state->lightness[TYPE_TARGET] = LIGHTNESS_DEFAULT;
        }
    }
    else if (p_state->onoff[TYPE_PRESENT] == 1 && p_state->onoff[TYPE_TARGET] == 0)
    {
        //turn off, set zero
        p_state->lightness[TYPE_TARGET] = 0;
    }
#endif

    BT_DBG("lightness cur(%04x) tar(%04x)", p_state->lightness[TYPE_PRESENT], p_state->lightness[TYPE_TARGET]);
}
