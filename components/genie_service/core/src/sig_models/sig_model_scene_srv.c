/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include <api/mesh.h>
#include "sig_models/sig_model.h"

#include "access.h"
#include "common/log.h"

struct bt_mesh_model_pub g_scene_pub = {
    .msg = NET_BUF_SIMPLE(2 + 5 + 4),
};

static void _scene_prepare_buf(struct bt_mesh_model *p_model, struct net_buf_simple *p_msg, bool is_ack)
{
    sig_model_state_t *p_state = &((sig_model_element_state_t *)p_model->user_data)->state;

    bt_mesh_model_msg_init(p_msg, OP_GENERIC_SCENE_STATUS);
    net_buf_simple_add_le16(p_msg, p_state->scene[TYPE_PRESENT]);
}

static void _scene_status(struct bt_mesh_model *p_model,
                          struct bt_mesh_msg_ctx *p_ctx, bool is_ack)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(1 + 2 + 4);

    BT_DBG("addr(0x%04x)", bt_mesh_model_elem(p_model)->addr);

    _scene_prepare_buf(p_model, p_msg, is_ack);
    p_ctx->send_ttl = GENIE_TRANSPORT_DEFAULT_TTL;
    if (bt_mesh_model_send(p_model, p_ctx, p_msg, NULL, NULL))
    {
        BT_ERR("Unable to send scene Status");
    }
    BT_DBG("Success!!!");
}

int genie_model_scene_changed(u16_t last_scene, u16_t target_scene)
{
    return 0;
}

static E_MESH_ERROR_TYPE _scene_analyze(struct bt_mesh_model *p_model,
                                        u16_t src_addr, struct net_buf_simple *p_buf)
{
    s16_t scene = 0;
    u8_t tid = 0;
    sig_model_element_state_t *p_elem = NULL;

    if (!p_model || !p_buf)
        return MESH_ANALYZE_ARGS_ERROR;

    p_elem = p_model->user_data;

    if (p_buf->len != 3 && p_buf->len != 5)
    {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    scene = (s16_t)net_buf_simple_pull_le16(p_buf);
    tid = net_buf_simple_pull_u8(p_buf);

    if (genie_transport_check_tid(src_addr, tid, p_elem->element_id) != MESH_SUCCESS)
    {
        BT_ERR("MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", src_addr, tid);
        return MESH_TID_REPEAT;
    }
    genie_transport_src_addr_set(src_addr);

    if (p_elem->state.scene[TYPE_TARGET] != scene)
    {
        genie_model_scene_changed(p_elem->state.scene[TYPE_TARGET], scene);
        p_elem->state.scene[TYPE_TARGET] = scene;
        sig_model_event_set_indicate(SIG_MODEL_INDICATE_GEN_SCENE);
    }
    else
    {
        GENIE_LOG_WARN("Dup light sence");
    }

    GENIE_LOG_INFO("light sence(0x%02x)", p_elem->state.scene);

    return MESH_SUCCESS;
}

static void _scene_get(struct bt_mesh_model *p_model,
                       struct bt_mesh_msg_ctx *p_ctx,
                       struct net_buf_simple *p_buf)
{
    sig_model_msg msg;
    sig_model_element_state_t *p_elem_state = NULL;

    memset(&msg, 0, sizeof(sig_model_msg));
    msg.opcode = OP_GENERIC_SCENE_GET;

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
    _scene_status(p_model, p_ctx, 0);
#endif
}

static void _scene_set(struct bt_mesh_model *p_model,
                       struct bt_mesh_msg_ctx *p_ctx,
                       struct net_buf_simple *p_buf)
{
    sig_model_msg msg;
    sig_model_element_state_t *p_elem_state = NULL;

    memset(&msg, 0, sizeof(sig_model_msg));
    msg.opcode = OP_GENERIC_SCENE_SET;
    if (p_buf != NULL)
    {
        msg.len = p_buf->len;
        msg.data = (u8_t *)p_buf->data;
    }

    E_MESH_ERROR_TYPE ret = _scene_analyze(p_model, p_ctx->addr, p_buf);

    BT_DBG("ret %d", ret);

    if (ret == MESH_SUCCESS)
    {
        p_elem_state = (sig_model_element_state_t *)p_model->user_data;
        msg.element_id = p_elem_state->element_id;
        sig_model_event(SIG_MODEL_EVT_GENERIC_MESG, (void *)&msg);
    }

#ifndef CONFIG_GENIE_MESH_NO_AUTO_REPLY
    if (ret == MESH_SUCCESS || ret == MESH_TID_REPEAT)
    {
        _scene_status(p_model, p_ctx, 1);
        if (ret == MESH_SUCCESS)
        {
            sig_model_event(SIG_MODEL_EVT_ANALYZE_MSG, (sig_model_element_state_t *)p_model->user_data);
        }
    }
#endif
}

static void _scene_set_unack(struct bt_mesh_model *p_model,
                             struct bt_mesh_msg_ctx *p_ctx,
                             struct net_buf_simple *p_buf)
{
    sig_model_msg msg;
    sig_model_element_state_t *p_elem_state = NULL;

    memset(&msg, 0, sizeof(sig_model_msg));
    msg.opcode = OP_GENERIC_SCENE_SET_UNACK;
    if (p_buf != NULL)
    {
        msg.len = p_buf->len;
        msg.data = (u8_t *)p_buf->data;
    }

    E_MESH_ERROR_TYPE ret = _scene_analyze(p_model, p_ctx->addr, p_buf);

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

const struct bt_mesh_model_op g_scene_op[GENIE_SCENE_OP_NUM] = {
    {OP_GENERIC_SCENE_GET, 0, _scene_get},
    {OP_GENERIC_SCENE_SET, 2, _scene_set},
    {OP_GENERIC_SCENE_SET_UNACK, 2, _scene_set_unack},
    BT_MESH_MODEL_OP_END,
};
