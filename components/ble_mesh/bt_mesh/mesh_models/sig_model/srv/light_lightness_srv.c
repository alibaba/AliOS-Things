/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <api/mesh.h>

#include "mesh_model/mesh_model.h"

#define TAG "LIGHT_LIGHTNESS_SRV"
#if defined(CONFIG_BT_MESH_MODEL_LIGHT_LIGHTNESS_SRV)

extern u8_t get_remain_byte(S_MESH_STATE *p_state, bool is_ack);
extern uint8_t mesh_check_tid(u16_t src_addr, u8_t tid);
extern bt_u32_t get_transition_time(u8_t byte);
static int _light_lightness_prepare_publication(struct bt_mesh_model *model);

struct bt_mesh_model_pub g_light_lightness_pub = {
    .msg = NET_BUF_SIMPLE(2 + 5 + 4),
    .update = _light_lightness_prepare_publication,
};



static void _light_lightness_prepare_buf(struct bt_mesh_model *model, struct net_buf_simple *p_msg, bool is_ack)
{
    S_ELEM_STATE *elem = model->user_data;
    u8_t remain_byte = 0;

    LOGD(TAG, "cur_actual(0x%04x) tar_actual(0x%04x) remain(0x%02x)",
         elem->state.lightness_actual[T_CUR], elem->state.lightness_actual[T_TAR], remain_byte);

    //prepear buf
    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_OP_2(0x82, 0x4E));

    net_buf_simple_add_le16(p_msg, elem->state.lightness_actual[T_CUR]);

    if (elem->state.lightness_actual[T_CUR] != elem->state.lightness_actual[T_TAR]) {
        remain_byte =  get_remain_byte(&elem->state, is_ack);

        if (remain_byte) {
            net_buf_simple_add_le16(p_msg, elem->state.lightness_actual[T_TAR]);
            net_buf_simple_add_u8(p_msg, remain_byte);
        }
    }

}

static int _light_lightness_prepare_publication(struct bt_mesh_model *model)
{
    struct net_buf_simple *msg;

    if (!model) {
        return -1;
    }

    msg = model->pub->msg;

    LOGD(TAG, "addr(0x%04x)", model->pub->addr);

    if (model->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        _light_lightness_prepare_buf(model, msg, 0);
    }

    return 0;
}

int ble_mesh_light_lightness_publication(struct bt_mesh_model *model)
{
    struct net_buf_simple *msg;
    int16_t ret;

    if (!model) {
        return -1;
    }

    msg = model->pub->msg;

    LOGD(TAG, "addr(0x%04x)", model->pub->addr);

    if (model->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        _light_lightness_prepare_buf(model, msg, 0);

        ret = bt_mesh_model_publish(model);

        if (ret) {
            LOGE(TAG, "bt_mesh_model_publish err %d\n", ret);
            return ret;
        }

        LOGD(TAG, "Success!!!");
    }

    return 0;
}


static void _light_lightness_status(struct bt_mesh_model *model,
                                    struct bt_mesh_msg_ctx *ctx, bool is_ack)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 5 + 4);

    //LOGD(TAG, "addr(0x%04x)", model->elem->addr);

    _light_lightness_prepare_buf(model, p_msg, is_ack);

    if (bt_mesh_model_send(model, ctx, p_msg, NULL, NULL)) {
        LOGE(TAG, "Unable to send lightness Status");
    }

    LOGD(TAG, "Success!!!");
}

static E_MESH_ERROR_TYPE _light_lightness_analyze(struct bt_mesh_model *model, u16_t src_addr, struct net_buf_simple *buf)
{
    u16_t lightness = 0;
    u8_t tid = 0;
    u8_t trans = 0;
    u8_t delay = 0;

    S_ELEM_STATE *elem = NULL;

    if (!model || !buf) {
        return MESH_ANALYZE_ARGS_ERROR;
    }

    elem = model->user_data;

    if (buf->len != 3 && buf->len != 5) {
        LOGE(TAG, "MESH_ANALYZE_SIZE_ERROR buf->len(%d)", buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    lightness = (u16_t)net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (buf->len) {
        trans = net_buf_simple_pull_u8(buf);
        delay = net_buf_simple_pull_u8(buf);
    } else {
        // trans = elem->powerup.def_trans;
        delay = 0;
    }

    if ((trans & 0x3F) == 0x3F) {
        LOGE(TAG, "MESH_SET_TRANSTION_ERROR");
        return MESH_SET_TRANSTION_ERROR;
    }

    if (mesh_check_tid(src_addr, tid) != MESH_SUCCESS) {
        LOGE(TAG, "MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", src_addr, tid);
        return MESH_TID_REPEAT;
    }

    if (lightness < elem->powerup.lightness_range.range_min || lightness > elem->powerup.lightness_range.range_max) {
        LOGE(TAG, "MESH_ANALYZE_ARGS_ERROR lightness(0x%04x) min(0x%04x) max(0x%04x)", lightness, elem->powerup.lightness_range.range_min, elem->powerup.lightness_range.range_max);
        return MESH_ANALYZE_ARGS_ERROR;
    }

    elem->state.lightness_actual[T_TAR] = lightness;
    elem->state.trans = trans;
    elem->state.delay = delay;

    if (elem->state.trans) {
        elem->state.trans_start_time = k_uptime_get() + elem->state.delay * 5; // delay is in 5 millisecond steps
        elem->state.trans_end_time = elem->state.trans_start_time + get_transition_time(elem->state.trans);
    }

    LOGD(TAG, "tar_actual(0x%04x) trans(0x%02x) delay(0x%02x)",
         elem->state.lightness_actual[T_TAR], elem->state.trans, elem->state.delay);

    if (elem->state.trans || elem->state.delay) {
        if (elem->state.delay) {
            //model_event(GEN_EVT_SDK_DELAY_START, (void *)elem);
        } else {
            //model_event(GEN_EVT_SDK_TRANS_START, (void *)elem);
        }
    } else {
        elem->state.lightness_actual[T_CUR] = elem->state.lightness_actual[T_TAR];
        model_message message;
        message.source_addr = src_addr;
        message.status_data = NULL;
        message.user_data = elem;
        model_event(BT_MESH_MODEL_LIGHTNESS_SET, (void *)&message);
    }

    return MESH_SUCCESS;
}


static void _light_lightness_get(struct bt_mesh_model *model,
                                 struct bt_mesh_msg_ctx *ctx,
                                 struct net_buf_simple *buf)
{
    _light_lightness_status(model, ctx, 0);
}


static void _light_lightness_set(struct bt_mesh_model *model,
                                 struct bt_mesh_msg_ctx *ctx,
                                 struct net_buf_simple *buf)
{
    if (_light_lightness_analyze(model, ctx->addr, buf) == MESH_SUCCESS) {
        _light_lightness_status(model, ctx, 1);
    }
}

static void _light_lightness_set_unack(struct bt_mesh_model *model,
                                       struct bt_mesh_msg_ctx *ctx,
                                       struct net_buf_simple *buf)
{

    if (_light_lightness_analyze(model, ctx->addr, buf) == MESH_SUCCESS) {
    }
}

static void _light_lightness_linear_prepare_buf(struct bt_mesh_model *model, struct net_buf_simple *p_msg, bool is_ack)
{
    u8_t remain_byte = 0;

    S_ELEM_STATE *elem;

    S_MESH_STATE *mesh_state = NULL;

    if (!model || !model->user_data || !p_msg) {
        return;
    }

    elem = model->user_data;
    mesh_state = &elem->state;

    LOGD(TAG, "cur_linear(0x%04x) tar_linear(0x%04x) remain(0x%02x)",
         mesh_state->lightness_linear[T_CUR], mesh_state->lightness_linear[T_TAR], remain_byte);

    //prepare buff
    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_OP_2(0x82, 0x52));

    net_buf_simple_add_le16(p_msg, mesh_state->lightness_linear[T_CUR]);

    if (mesh_state->lightness_linear[T_CUR] != mesh_state->lightness_linear[T_TAR]) {
        remain_byte   = get_remain_byte(&elem->state, is_ack);

        if (remain_byte) {
            net_buf_simple_add_le16(p_msg, mesh_state->lightness_linear[T_TAR]);
            net_buf_simple_add_u8(p_msg, remain_byte);
        }
    }

}

static void _light_lightness_linear_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, u8_t is_ack)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 5 + 4);

    if (!model) {
        return;
    }

    //LOGD(TAG, "addr(0x%04x)", model->elem->addr);

    _light_lightness_linear_prepare_buf(model, p_msg, is_ack);

    if (bt_mesh_model_send(model, ctx, p_msg, NULL, NULL)) {
        LOGE(TAG, "Unable to send linear Status");
    }

    LOGD(TAG, "Success!!!");
}

/*
 * Ethan
 * 1. <done> add struct bt_mesh_model *model
 * 2. <done> replace g_mesh_state with elem->state
 * 3. <done> replace mesh_check_TID with mesh_check_tid
 * 4. <done> bind operation in genie_event
 * 5. <done> send event to genie_event
 * */
static E_MESH_ERROR_TYPE _light_lightness_linear_analyze(struct bt_mesh_model *model, u16_t src_addr, struct net_buf_simple *buf)
{
    u16_t linear = 0;
    u8_t tid = 0;
    u8_t l_trans = 0;
    u8_t l_delay = 0;
    S_ELEM_STATE *elem = NULL;
    S_MESH_STATE *mesh_state = NULL;


    if (!model || !model->user_data || !buf) {
        return MESH_ANALYZE_ARGS_ERROR;
    }

    elem = model->user_data;
    mesh_state = &elem->state;

    if (buf->len != 3 && buf->len != 5) {
        LOGE(TAG, "MESH_ANALYZE_SIZE_ERROR buf->len(%d)", buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    linear = net_buf_simple_pull_le16(buf);

    tid = net_buf_simple_pull_u8(buf);

    if (mesh_check_tid(src_addr, tid) != MESH_SUCCESS) {
        LOGE(TAG, "MESH_TID_REPEAT src_addr(0x%04x) TID(0x%02x)", src_addr, tid);
        return MESH_TID_REPEAT;
    }

    if (buf->len) {
        l_trans = net_buf_simple_pull_u8(buf);
        l_delay = net_buf_simple_pull_u8(buf);
    }

    if ((l_trans & 0x3F) == 0x3F) {
        LOGE(TAG, "MESH_SET_TRANSTION_ERROR");
        return MESH_SET_TRANSTION_ERROR;
    }

    mesh_state->lightness_linear[T_TAR] = linear;

    //model_bind_operation(B_LIGHTNESS_LINEAR_ID, elem, T_TAR);

    //mesh_state->trans = l_trans ? l_trans : elem->powerup.def_trans;
    mesh_state->delay = l_delay;

    if (mesh_state->trans) {
        mesh_state->trans_start_time = k_uptime_get_32() + mesh_state->delay * 5;
        mesh_state->trans_end_time = mesh_state->trans_start_time + get_transition_time(mesh_state->trans);
    }

    LOGD(TAG, "tar_linear(0x%04x) trans(0x%02x) delay(0x%02x)",
         mesh_state->lightness_linear[T_TAR], mesh_state->trans, mesh_state->delay);

    //model_event(GEN_EVT_SDK_ANALYZE_MSG, (void *)elem);

    if (mesh_state->trans || mesh_state->delay) {
        if (mesh_state->delay) {
            //model_event(GEN_EVT_SDK_DELAY_START, (void *)elem);
        } else {
            //model_event(GEN_EVT_SDK_TRANS_START, (void *)elem);
        }
    } else {
        elem->state.lightness_linear[T_CUR] = elem->state.lightness_linear[T_TAR];
        model_message message;
        message.source_addr = src_addr;
        message.status_data = NULL;
        message.user_data = elem;
        model_event(BT_MESH_MODEL_LIGHTNESS_LINEAR_SET, (void *)&message);
    }

    return MESH_SUCCESS;
}


static void _light_lightness_linear_get(struct bt_mesh_model *model,
                                        struct bt_mesh_msg_ctx *ctx,
                                        struct net_buf_simple *buf)
{
    LOGD(TAG, "");

    _light_lightness_linear_status(model, ctx, 0);
}

/*
 * Ethan
 * 1. <done> remove pub_need and lightness_linear_action, mesh_publication related operations
 * */
static void _light_lightness_linear_set(struct bt_mesh_model *model,
                                        struct bt_mesh_msg_ctx *ctx,
                                        struct net_buf_simple *buf)
{
    LOGD(TAG, "");

    if (_light_lightness_linear_analyze(model, ctx->addr, buf) == MESH_SUCCESS) {
        _light_lightness_linear_status(model, ctx, 1);
    }
}

/*
 * Ethan
 * 1. <done> remove pub_need and lightness_linear_action, mesh_publication related operations
 * */
static void _light_lightness_linear_set_unack(struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf)
{
    LOGD(TAG, "");

    if (_light_lightness_linear_analyze(model, ctx->addr, buf) == MESH_SUCCESS) {
#if 0 // Ethan
        pub_need = lightness_linear_action(model);

        if (pub_need && model->pub->addr != ctx->addr) {
            mesh_publication(model->elem, MESH_PUB_LIGHTNESS);
        }

#endif
    }
}

static void _light_lightness_last_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 2 + 4);
    S_ELEM_STATE *elem = NULL;
    S_MESH_POWERUP *mesh_powerup = NULL;

    if (!model || !model->user_data || !ctx) {
        return;
    }

    elem = model->user_data;
    mesh_powerup = &elem->powerup;

    LOGD(TAG, "lightness_last0x%04x)", mesh_powerup->lightness_last);

    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_OP_2(0x82, 0x54));
    net_buf_simple_add_le16(p_msg, mesh_powerup->lightness_last);

    if (bt_mesh_model_send(model, ctx, p_msg, NULL, NULL)) {
        LOGE(TAG, "Unable to send last Status");
    }

    LOGD(TAG, "Success!!!");
}

static void _light_lightness_last_get(struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      struct net_buf_simple *buf)
{
    LOGD(TAG, "");

    _light_lightness_last_status(model, ctx);
}

static void _light_lightness_defatult_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 2 + 4);
    S_ELEM_STATE *elem = NULL;
    S_MESH_POWERUP *mesh_powerup = NULL;

    if (!model || !model->user_data || !ctx) {
        return;
    }

    elem = model->user_data;
    mesh_powerup = &elem->powerup;

    LOGD(TAG, "lightness_actual_default(0x%04x)",  mesh_powerup->lightness_actual_default);

    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_OP_2(0x82, 0x56));
    net_buf_simple_add_le16(p_msg, mesh_powerup->lightness_actual_default);

    if (bt_mesh_model_send(model, ctx, p_msg, NULL, NULL)) {
        LOGE(TAG, "Unable to send default Status");
    }

    LOGD(TAG, "Success!!!");
}

static void _light_lightness_default_get(struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf)
{
    LOGD(TAG, "");

    _light_lightness_defatult_status(model, ctx);
}

static void _light_lightness_range_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 5 + 4);
    S_ELEM_STATE *elem = NULL;
    S_MESH_POWERUP *mesh_powerup = NULL;

    if (!model || !model->user_data || !ctx) {
        return;
    }

    elem = model->user_data;
    mesh_powerup = &elem->powerup;

    LOGD(TAG, "min(0x%04x) max(0x%04x)", mesh_powerup->lightness_range.range_min, mesh_powerup->lightness_range.range_max);

    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_OP_2(0x82, 0x58));
    net_buf_simple_add_u8(p_msg, mesh_powerup->lightness_range.code);
    net_buf_simple_add_le16(p_msg, mesh_powerup->lightness_range.range_min);
    net_buf_simple_add_le16(p_msg, mesh_powerup->lightness_range.range_max);

    if (bt_mesh_model_send(model, ctx, p_msg, NULL, NULL)) {
        LOGE(TAG, "Unable to send range Status");
    }

    LOGD(TAG, "Success!!!");
}

static void _light_lightness_range_get(struct bt_mesh_model *model,
                                       struct bt_mesh_msg_ctx *ctx,
                                       struct net_buf_simple *buf)
{
    LOGD(TAG, "");

    _light_lightness_range_status(model, ctx);
}

/*
 * Ethan
 * 1. <done> add struct bt_mesh_model *model
 * 2. <done> think about how to store lightness default/range related parameters instead of g_mesh_powerup
 *
 * */
//light lightness setup server
static E_MESH_ERROR_TYPE _light_lightness_default_analyze(struct bt_mesh_model *model, u16_t src_addr, struct net_buf_simple *buf)
{
    S_ELEM_STATE *elem = NULL;
    S_MESH_POWERUP *mesh_powerup = NULL;
    uint16_t default_temp = 0;

    if (!model || !model->user_data || !buf) {
        return MESH_ANALYZE_ARGS_ERROR;
    }

    elem = model->user_data;
    mesh_powerup = &elem->powerup;

    if (buf->len != 2) {
        LOGE(TAG, "MESH_ANALYZE_SIZE_ERROR buf->len(%d)", buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    default_temp = net_buf_simple_pull_le16(buf);


    if (default_temp < mesh_powerup->lightness_range.range_min || default_temp > mesh_powerup->lightness_range.range_max) {
        LOGE(TAG, "MESH_ANALYZE_ARGS_ERROR tar(0x%04x) min(0x%04x) max(0x%04x)",
             default_temp, mesh_powerup->lightness_range.range_min, mesh_powerup->lightness_range.range_max);
        return MESH_ANALYZE_ARGS_ERROR;
    }

    mesh_powerup->lightness_actual_default = default_temp;
    LOGD(TAG, "default_actual(0x%04x)", mesh_powerup->lightness_actual_default);

    model_message message;
    message.source_addr = src_addr;
    message.status_data = NULL;
    message.user_data = elem;
    model_event(BT_MESH_MODEL_LIGHTNESS_DEF_SET, (void *)&message);
    //model_bind_operation(B_LIGHTNESS_DFT_ID, elem, T_CUR);
    return MESH_SUCCESS;
}

static void _light_lightness_default_set(struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf)
{
    LOGD(TAG, "");

    if (_light_lightness_default_analyze(model, ctx->addr, buf) == MESH_SUCCESS) {
        _light_lightness_defatult_status(model, ctx);
    }
}

static void _light_lightness_default_set_unack(struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf)
{
    LOGD(TAG, "");

    _light_lightness_default_analyze(model, ctx->addr, buf);
}

static E_MESH_ERROR_TYPE _light_lightness_range_analyze(struct bt_mesh_model *model, u16_t src_addr, struct net_buf_simple *buf)
{
    u16_t min = 0;
    u16_t max = 0;
    S_ELEM_STATE *elem = NULL;
    S_MESH_POWERUP *mesh_powerup = NULL;

    if (!model || !buf) {
        return MESH_ANALYZE_ARGS_ERROR;
    }

    elem = model->user_data;
    mesh_powerup = &elem->powerup;

    if (buf->len != 4) {
        LOGE(TAG, "MESH_ANALYZE_SIZE_ERROR buf->len(%d)", buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    min = net_buf_simple_pull_le16(buf);
    max = net_buf_simple_pull_le16(buf);

    if (!min || !max || min > max) {
        LOGE(TAG, "MESH_ANALYZE_ARGS_ERROR min(0x%04x) max(0x%04x)", min, max);
        return MESH_ANALYZE_ARGS_ERROR;
    }

    mesh_powerup->lightness_range.range_min = min;
    mesh_powerup->lightness_range.range_max = max;
    mesh_powerup->lightness_range.code = 0;
    LOGD(TAG, "min_actual(0x%04x) max_actual(0x%04x)",
         mesh_powerup->lightness_range.range_min, mesh_powerup->lightness_range.range_max);

    model_message message;
    message.source_addr = src_addr;
    message.status_data = NULL;
    message.user_data = elem;
    model_event(BT_MESH_MODEL_LIGHTNESS_RANGE_SET, (void *)&message);

    return MESH_SUCCESS;
}

/*
 * Ethan
 * will range set operation happens dynamically?
 * shall we need to check whether current lightness is in range and adjust if necessary?
 * */
static void _light_lightness_range_set(struct bt_mesh_model *model,
                                       struct bt_mesh_msg_ctx *ctx,
                                       struct net_buf_simple *buf)
{
    LOGD(TAG, "");

    if (_light_lightness_range_analyze(model, ctx->addr, buf) == MESH_SUCCESS) {
        _light_lightness_range_status(model, ctx);
    }
}

static void _light_lightness_range_set_unack(struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf)
{
    LOGD(TAG, "");

    _light_lightness_range_analyze(model, ctx->addr, buf);
}


const struct bt_mesh_model_op g_light_lightness_op[LIGHT_LIGHTNESS_OPC_NUM] = {

    /* Light Lightness */
    { BT_MESH_MODEL_OP_2(0x82, 0x4b), 0, _light_lightness_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x4c), 3, _light_lightness_set },        /* bind operation executed after action done in genie_mesh */
    { BT_MESH_MODEL_OP_2(0x82, 0x4d), 3, _light_lightness_set_unack },

    { BT_MESH_MODEL_OP_2(0x82, 0x4f), 0, _light_lightness_linear_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x50), 3, _light_lightness_linear_set }, /* bind operation executed immediately */
    { BT_MESH_MODEL_OP_2(0x82, 0x51), 3, _light_lightness_linear_set_unack },

    { BT_MESH_MODEL_OP_2(0x82, 0x53), 0, _light_lightness_last_get },

    { BT_MESH_MODEL_OP_2(0x82, 0x55), 0, _light_lightness_default_get },

    { BT_MESH_MODEL_OP_2(0x82, 0x57), 0, _light_lightness_range_get },

    /* Light Lightness Setup */
    { BT_MESH_MODEL_OP_2(0x82, 0x59), 2, _light_lightness_default_set }, /* bind operation executed immediately */
    { BT_MESH_MODEL_OP_2(0x82, 0x5a), 2, _light_lightness_default_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x5b), 4, _light_lightness_range_set },   /* bind operation executed immediately */
    { BT_MESH_MODEL_OP_2(0x82, 0x5c), 4, _light_lightness_range_set_unack },

    BT_MESH_MODEL_OP_END,
};

#endif
