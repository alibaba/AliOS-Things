/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <api/mesh.h>
#include <mesh.h>
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"
#include "genie_service.h"

struct bt_mesh_model_pub g_ctl_srv_pub = {

};

#if CONFIG_MESH_MODEL_CTL_SRV //light ctl server
static void _ctl_prepare_buf(struct bt_mesh_model *p_model, struct net_buf_simple *p_msg, bool is_ack)
{
    sig_model_state_t *p_state = &((sig_model_element_state_t *)p_model->user_data)->state;
    u8_t remain_time_byte = 0;

    BT_DBG("color_temperature cur(0x%04x) tar(0x%04x)", p_state->color_temperature[TYPE_PRESENT], p_state->color_temperature[TYPE_TARGET]);
#ifdef CONFIG_MESH_MODEL_TRANS
    remain_time_byte = sig_model_transition_get_remain_time_byte(p_state, is_ack);

    BT_DBG("remain(0x%02x)", remain_time_byte);
#endif

    //prepear buff
    bt_mesh_model_msg_init(p_msg, OP_GENERIC_LIGHT_CTL_STATUS);
    if (is_ack && remain_time_byte == 0)
    {
        net_buf_simple_add_le16(p_msg, p_state->lightness[TYPE_TARGET]);
        net_buf_simple_add_le16(p_msg, p_state->color_temperature[TYPE_TARGET]);
    }
    else
    {
        net_buf_simple_add_le16(p_msg, p_state->lightness[TYPE_PRESENT]);
        net_buf_simple_add_le16(p_msg, p_state->color_temperature[TYPE_PRESENT]);

#ifdef CONFIG_MESH_MODEL_TRANS
        if (remain_time_byte)
        {
            net_buf_simple_add_le16(p_msg, p_state->lightness[TYPE_TARGET]);
            net_buf_simple_add_le16(p_msg, p_state->color_temperature[TYPE_TARGET]);
            net_buf_simple_add_u8(p_msg, remain_time_byte);
        }
#endif
    }
}

static void _ctl_status(struct bt_mesh_model *p_model,
                        struct bt_mesh_msg_ctx *p_ctx, bool is_ack)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 9 + 4);

    BT_DBG("addr(0x%04x)", bt_mesh_model_elem(p_model)->addr);

    _ctl_prepare_buf(p_model, p_msg, is_ack);
    p_ctx->send_ttl = GENIE_TRANSPORT_DEFAULT_TTL;
    if (bt_mesh_model_send(p_model, p_ctx, p_msg, NULL, NULL))
    {
        BT_ERR("Unable to send ctl Status");
    }
    BT_DBG("Success!!!");
}

static u8_t _ctl_analyze(struct bt_mesh_model *p_model,
                         u16_t src_addr, struct net_buf_simple *p_buf)
{
    u16_t lightness = 0;
    u16_t color_temperature = 0;
    u16_t uv = 0;
    u8_t tid = 0;
#ifdef CONFIG_MESH_MODEL_TRANS
    u8_t trans = 0;
    u8_t delay = 0;
#endif
    sig_model_element_state_t *p_elem = NULL;

    if (!p_model || !p_buf)
        return MESH_ANALYZE_ARGS_ERROR;

    p_elem = p_model->user_data;

    if (p_buf->len != 7 && p_buf->len != 9)
    {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR p_buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    //get message info
    lightness = net_buf_simple_pull_le16(p_buf);
    color_temperature = net_buf_simple_pull_le16(p_buf);
    uv = net_buf_simple_pull_le16(p_buf);
    tid = net_buf_simple_pull_u8(p_buf);

    GENIE_LOG_INFO("uv:%d lightness:%d", uv, lightness);
    (void)uv;
    (void)lightness;
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

    //check color_temperature
    if (color_temperature < COLOR_TEMPERATURE_MIN || color_temperature > COLOR_TEMPERATURE_MAX)
    {
        BT_ERR("MESH_ANALYZE_ARGS_ERROR color_temperature(0x%04x)", color_temperature);
        return MESH_ANALYZE_ARGS_ERROR;
    }

    if (genie_transport_check_tid(src_addr, tid, p_elem->element_id) != MESH_SUCCESS)
    {
        BT_ERR("MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", src_addr, tid);
        return MESH_TID_REPEAT;
    }

    p_elem->state.color_temperature[TYPE_TARGET] = color_temperature;
    p_elem->powerup.last_color_temperature = p_elem->state.color_temperature[TYPE_TARGET];
    BT_DBG("color_temperature cur(%04x) tar(%04x)", p_elem->state.color_temperature[TYPE_PRESENT], p_elem->state.color_temperature[TYPE_TARGET]);

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

    //only check color_temperature when ali simaple model
    if (p_elem->state.color_temperature[TYPE_PRESENT] != p_elem->state.color_temperature[TYPE_TARGET])
    {
        //only bind color_temperature when ali_simple_model is enable
        sig_model_event_set_indicate(SIG_MODEL_INDICATE_GEN_CTL);
        sig_model_generic_color_temperature_bind_ops(p_elem);
    }

    return MESH_SUCCESS;
}

static void _ctl_get(struct bt_mesh_model *p_model,
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
    msg.opcode = OP_GENERIC_LIGHT_CTL_GET;
    if (p_buf != NULL)
    {
        msg.len = p_buf->len;
        msg.data = (u8_t *)p_buf->data;
    }
    genie_transport_src_addr_set(p_ctx->addr);

    p_elem_state = (sig_model_element_state_t *)p_model->user_data;
    msg.element_id = p_elem_state->element_id;

    sig_model_event(SIG_MODEL_EVT_GENERIC_MESG, (void *)&msg);

    _ctl_status(p_model, p_ctx, 0);
}

static void _ctl_set(struct bt_mesh_model *p_model,
                     struct bt_mesh_msg_ctx *p_ctx,
                     struct net_buf_simple *p_buf)
{
    E_MESH_ERROR_TYPE ret = 0;
    sig_model_msg msg;
    sig_model_element_state_t *p_elem_state = NULL;

    if (!p_model || !p_model->user_data)
    {
        GENIE_LOG_ERR("param err");
        return;
    }

    memset(&msg, 0, sizeof(sig_model_msg));
    msg.opcode = OP_GENERIC_LIGHT_CTL_SET;
    if (p_buf != NULL)
    {
        msg.len = p_buf->len;
        msg.data = (u8_t *)p_buf->data;
    }
    genie_transport_src_addr_set(p_ctx->addr);

    p_elem_state = (sig_model_element_state_t *)p_model->user_data;
    msg.element_id = p_elem_state->element_id;

    sig_model_event(SIG_MODEL_EVT_GENERIC_MESG, (void *)&msg);

    ret = _ctl_analyze(p_model, p_ctx->addr, p_buf);

    BT_DBG("ret %d", ret);

    if (ret == MESH_SUCCESS || ret == MESH_TID_REPEAT)
    {
        _ctl_status(p_model, p_ctx, 1);
        if (ret == MESH_SUCCESS)
        {
            sig_model_event(SIG_MODEL_EVT_ANALYZE_MSG, (sig_model_element_state_t *)p_model->user_data);
        }
    }
}

static void _ctl_set_unack(struct bt_mesh_model *p_model,
                           struct bt_mesh_msg_ctx *p_ctx,
                           struct net_buf_simple *p_buf)
{
    BT_DBG("");
    E_MESH_ERROR_TYPE ret = 0;
    sig_model_msg msg;
    sig_model_element_state_t *p_elem_state = NULL;

    if (!p_model || !p_model->user_data)
    {
        GENIE_LOG_ERR("param err");
        return;
    }

    memset(&msg, 0, sizeof(sig_model_msg));
    msg.opcode = OP_GENERIC_LIGHT_CTL_SET_UNACK;
    if (p_buf != NULL)
    {
        msg.len = p_buf->len;
        msg.data = (u8_t *)p_buf->data;
    }
    genie_transport_src_addr_set(p_ctx->addr);

    p_elem_state = (sig_model_element_state_t *)p_model->user_data;
    msg.element_id = p_elem_state->element_id;

    sig_model_event(SIG_MODEL_EVT_GENERIC_MESG, (void *)&msg);

    ret = _ctl_analyze(p_model, p_ctx->addr, p_buf);

    if (ret == MESH_SUCCESS)
    {
        sig_model_event(SIG_MODEL_EVT_ANALYZE_MSG, (sig_model_element_state_t *)p_model->user_data);
    }
}

const struct bt_mesh_model_op g_ctl_srv_op[LIGHT_CTL_OPCODE_NUM] = {
    {OP_GENERIC_LIGHT_CTL_GET, 0, _ctl_get},
    {OP_GENERIC_LIGHT_CTL_SET, 7, _ctl_set},
    {OP_GENERIC_LIGHT_CTL_SET_UNACK, 7, _ctl_set_unack},
    BT_MESH_MODEL_OP_END,
};
#endif

void sig_model_light_ctl_update_by_onoff(sig_model_element_state_t *p_elem)
{
    sig_model_state_t *p_state = &p_elem->state;

    BT_DBG("onoff cur(%d) tar(%d)", p_state->onoff[TYPE_PRESENT], p_state->onoff[TYPE_TARGET]);

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    if (p_state->onoff[TYPE_PRESENT] == 0 && p_state->onoff[TYPE_TARGET] == 1)
    {
        //turn on, check temperature
        BT_DBG("color_temperature cur(%d) tar(%d)", p_state->color_temperature[TYPE_PRESENT], p_state->color_temperature[TYPE_TARGET]);
        if (p_state->color_temperature[TYPE_PRESENT] != p_state->color_temperature[TYPE_TARGET])
        {
            if (p_elem->powerup.last_color_temperature)
            {
                p_state->color_temperature[TYPE_TARGET] = p_elem->powerup.last_color_temperature;
            }
            else
            {
                p_state->color_temperature[TYPE_TARGET] = COLOR_TEMPERATURE_DEFAULT;
            }

            sig_model_event_set_indicate(SIG_MODEL_INDICATE_GEN_CTL);
        }
    }
#endif

    BT_DBG("color_temperature cur(%04x) tar(%04x)", p_state->color_temperature[TYPE_PRESENT], p_state->color_temperature[TYPE_TARGET]);
}
