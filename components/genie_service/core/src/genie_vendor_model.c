/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <api/mesh.h>
#include <mesh/access.h>
#include "misc/dlist.h"
//nclude "access.h"
#include "genie_service.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_VENDOR_MODEL)
#include "common/log.h"

/**
 * vendor model publish context, alloc maximum message buffer size
 * */
struct bt_mesh_model_pub genie_model_pub = {
    //max = 8 seg package
    .msg = NET_BUF_SIMPLE(GENIE_MODEL_MTU), // allocate maximum payload size
};

int genie_model_handle_mesg(genie_transport_model_param_t *p_msg)
{
    uint8_t *p_data = NULL;

    if (!p_msg)
    {
        BT_ERR("p_msg is null");
        return -1;
    }

    if (p_msg->len < 2)
    {
        return genie_down_msg(GENIE_DOWN_MESG_VENDOR_TYPE, CONFIG_MESH_VENDOR_COMPANY_ID, p_msg);
    }

    if (!p_msg->data || p_msg->len == 0)
    {
        BT_ERR("invalid model mesg");
        return -1;
    }

    p_data = p_msg->data;
    BT_INFO("opcode:0x%x, tid:%d, len:%d", p_msg->opid, p_msg->tid, p_msg->len);

    if (p_data && p_msg->len)
    {
        BT_INFO("payload: %s", bt_hex(p_data, p_msg->len));
    }

#ifdef MESH_MODEL_VENDOR_TIMER
    if (genie_time_handle_model_mesg(p_msg) != 0)
    {
        return 0; //This is genie time message
    }
#endif

    genie_down_msg(GENIE_DOWN_MESG_VENDOR_TYPE, CONFIG_MESH_VENDOR_COMPANY_ID, p_msg);

    return 0;
}

/** @def genie_model_analyze
 *
 *  @brief analyze the received message and notify genie SDK
 *
 *  @param pointer to the received message (vendor model, context and the message buffer) and opid
 *
 *  @return if success return 0; if fails return error no.
 */
static s16_t genie_model_analyze(struct bt_mesh_model *p_model,
                                 struct bt_mesh_msg_ctx *p_ctx,
                                 struct net_buf_simple *p_buf,
                                 u8_t opid)
{
    genie_transport_model_param_t msg;
    sig_model_element_state_t *p_elem = NULL;

    if (!p_model || !p_buf || !p_model->user_data)
        return MESH_ANALYZE_ARGS_ERROR;

    if (p_buf->len < 3)
    {
        BT_ERR("invalid buf len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    p_elem = p_model->user_data;

    memset(&msg, 0, sizeof(genie_transport_model_param_t));

    msg.opid = opid;
    msg.tid = net_buf_simple_pull_u8(p_buf);

    if (genie_transport_check_tid(p_ctx->addr, msg.tid, p_elem->element_id) != MESH_SUCCESS)
    {
        BT_ERR("MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", p_ctx->addr, msg.tid);
        return MESH_TID_REPEAT;
    }
    genie_transport_src_addr_set(p_ctx->addr);

    msg.len = p_buf->len;
    BT_INFO("opcode:0x%x, tid:%d, len:%d", msg.opid, msg.tid, msg.len);
    if (msg.len)
    {
        msg.data = (u8_t *)p_buf->data;
        net_buf_simple_pull(p_buf, msg.len);
        BT_DBG("payload:%s", bt_hex(msg.data, msg.len));
    }
    else
    {
        msg.data = NULL;
    }

    msg.p_model = p_model;
    genie_event(GENIE_EVT_VENDOR_MODEL_MSG, (void *)&msg);

    return 0;
}

/** @def genie_model_get
 *
 *  @brief handle VENDOR_OP_ATTR_GET_STATUS message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void genie_model_get(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    genie_model_analyze(model, ctx, buf, VENDOR_OP_ATTR_GET_STATUS);
}

/** @def genie_model_set_ack
 *
 *  @brief handle VENDOR_OP_ATTR_SET_ACK message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void genie_model_set_ack(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
    genie_model_analyze(model, ctx, buf, VENDOR_OP_ATTR_SET_ACK);
}

/** @def genie_model_set_unack
 *
 *  @brief handle VENDOR_OP_ATTR_SET_UNACK message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void genie_model_set_unack(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf)
{
    genie_model_analyze(model, ctx, buf, VENDOR_OP_ATTR_SET_UNACK);
}

/** @def genie_model_confirm
 *
 *  @brief handle VENDOR_OP_ATTR_CONFIME message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void genie_model_confirm(struct bt_mesh_model *p_model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
    u8_t tid = 0x0;
    genie_transport_model_param_t msg;

    if (!p_model || !p_model->user_data)
    {
        GENIE_LOG_ERR("param err");
        return;
    }

    if (buf->len != 1)
    {
        BT_ERR("invalid buf len(%d)", buf->len);
        return;
    }

    tid = net_buf_simple_pull_u8(buf);
    genie_transport_ack(tid);

    memset(&msg, 0, sizeof(genie_transport_model_param_t));
    msg.opid = VENDOR_OP_ATTR_CONFIME;
    msg.tid = tid;
    msg.len = 0;
    msg.data = NULL;

    msg.p_model = p_model;
    genie_event(GENIE_EVT_VENDOR_MODEL_MSG, (void *)&msg);
}

static void genie_model_get_indicate(struct bt_mesh_model *p_model,
                                     struct bt_mesh_msg_ctx *ctx,
                                     struct net_buf_simple *buf)
{
    u8_t tid = 0x0;
    genie_transport_model_param_t msg;
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(3 + 1 + 4); //opcode + tid + TransMIC

    if (!p_model || !p_model->user_data)
    {
        GENIE_LOG_ERR("param err");
        return;
    }

    tid = net_buf_simple_pull_u8(buf);
    genie_transport_ack(tid);

    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_OP_3(VENDOR_OP_ATTR_CONFIME, CONFIG_MESH_VENDOR_COMPANY_ID));
    net_buf_simple_add_u8(p_msg, tid);
    ctx->send_ttl = GENIE_TRANSPORT_DEFAULT_TTL;
    if (bt_mesh_model_send(p_model, ctx, p_msg, NULL, NULL))
    {
        BT_ERR("send comfirm fail");
    }

    memset(&msg, 0, sizeof(genie_transport_model_param_t));
    msg.opid = VENDOR_OP_ATTR_INDICATE;
    msg.tid = tid;
    msg.len = 0;
    msg.data = NULL;
    genie_transport_src_addr_set(ctx->addr);
    msg.p_model = p_model;

    genie_event(GENIE_EVT_VENDOR_MODEL_MSG, (void *)&msg);
}

/** @def genie_model_confirm_tg
 *
 *  @brief handle VENDOR_OP_ATTR_CONFIME_TG message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void genie_model_confirm_tg(struct bt_mesh_model *p_model,
                                   struct bt_mesh_msg_ctx *ctx,
                                   struct net_buf_simple *buf)
{
    u8_t tid = 0x0;
    genie_transport_model_param_t msg;

    if (!p_model || !p_model->user_data)
    {
        GENIE_LOG_ERR("param err");
        return;
    }

    tid = net_buf_simple_pull_u8(buf);
    genie_transport_ack(tid);

    memset(&msg, 0, sizeof(genie_transport_model_param_t));
    msg.opid = VENDOR_OP_ATTR_CONFIME_TG;
    msg.tid = tid;
    msg.len = buf->len;

    if (msg.len)
    {
        msg.data = (u8_t *)buf->data;
        net_buf_simple_pull(buf, msg.len);
        BT_DBG("payload:%s", bt_hex(msg.data, msg.len));
    }
    else
    {
        msg.data = NULL;
    }

    msg.p_model = p_model;
    genie_event(GENIE_EVT_VENDOR_MODEL_MSG, (void *)&msg);
}

/** @def genie_model_transparent
 *
 *  @brief handle VENDOR_OP_ATTR_TRANS_MSG message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void genie_model_transparent(struct bt_mesh_model *model,
                                    struct bt_mesh_msg_ctx *ctx,
                                    struct net_buf_simple *buf)
{
    genie_model_analyze(model, ctx, buf, VENDOR_OP_ATTR_TRANS_MSG);
}

/** @def genie_model_transparent_ack
 *
 *  @brief handle VENDOR_OP_ATTR_TRANS_ACK message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void genie_model_transparent_ack(struct bt_mesh_model *p_model,
                                        struct bt_mesh_msg_ctx *ctx,
                                        struct net_buf_simple *buf)
{
    u8_t tid = 0x0;
    genie_transport_model_param_t msg;

    if (!p_model || !p_model->user_data)
    {
        GENIE_LOG_ERR("param err");
        return;
    }

    if (buf->len != 1)
    {
        BT_ERR("invalid buf len(%d)", buf->len);
        return;
    }

    tid = net_buf_simple_pull_u8(buf);
    genie_transport_ack(tid);

    memset(&msg, 0, sizeof(genie_transport_model_param_t));
    msg.opid = VENDOR_OP_ATTR_TRANS_ACK;
    msg.tid = tid;
    msg.len = 0;
    msg.data = NULL;

    msg.p_model = p_model;
    genie_event(GENIE_EVT_VENDOR_MODEL_MSG, (void *)&msg);
}

/** @def ggenie_model_alibaba_op
 *
 *  @brief vendor model operations struct
 *
 */
const struct bt_mesh_model_op genie_model_op[VENDOR_MODEL_OPC_NUM] = {
    {BT_MESH_MODEL_OP_3(VENDOR_OP_ATTR_GET_STATUS, CONFIG_MESH_VENDOR_COMPANY_ID), 2, genie_model_get},
    {BT_MESH_MODEL_OP_3(VENDOR_OP_ATTR_SET_ACK, CONFIG_MESH_VENDOR_COMPANY_ID), 2, genie_model_set_ack},
    {BT_MESH_MODEL_OP_3(VENDOR_OP_ATTR_SET_UNACK, CONFIG_MESH_VENDOR_COMPANY_ID), 2, genie_model_set_unack},
    {BT_MESH_MODEL_OP_3(VENDOR_OP_ATTR_INDICATE, CONFIG_MESH_VENDOR_COMPANY_ID), 1, genie_model_get_indicate},
    {BT_MESH_MODEL_OP_3(VENDOR_OP_ATTR_CONFIME, CONFIG_MESH_VENDOR_COMPANY_ID), 1, genie_model_confirm},
    {BT_MESH_MODEL_OP_3(VENDOR_OP_ATTR_CONFIME_TG, CONFIG_MESH_VENDOR_COMPANY_ID), 1, genie_model_confirm_tg},
    {BT_MESH_MODEL_OP_3(VENDOR_OP_ATTR_TRANS_MSG, CONFIG_MESH_VENDOR_COMPANY_ID), 1, genie_model_transparent},
    {BT_MESH_MODEL_OP_3(VENDOR_OP_ATTR_TRANS_ACK, CONFIG_MESH_VENDOR_COMPANY_ID), 1, genie_model_transparent_ack},
    BT_MESH_MODEL_OP_END,
};
