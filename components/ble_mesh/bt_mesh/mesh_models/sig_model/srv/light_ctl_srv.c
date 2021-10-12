/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <api/mesh.h>
#include <mesh.h>
#include <aos/kernel.h>
#include "mesh_model/mesh_model.h"
#define TAG "LIGHT_CTL_SRV"
#if defined(CONFIG_BT_MESH_MODEL_LIGHT_CTL_SRV)

extern u8_t get_remain_byte(S_MESH_STATE *p_state, bool is_ack);
extern uint8_t mesh_check_tid(u16_t src_addr, u8_t tid);
extern bt_u32_t get_transition_time(u8_t byte);
extern long long aos_now_ms(void);
static int _light_ctl_prepare_publication(struct bt_mesh_model *model);

struct bt_mesh_model_pub g_ctl_srv_pub = {
    .msg = NET_BUF_SIMPLE(2 + 9 + 4),
    .update = _light_ctl_prepare_publication,
};


//LIGHT CTL SRV

static void _ctl_prepare_buf(struct bt_mesh_model *model, struct net_buf_simple *p_msg, bool is_ack)
{

    S_ELEM_STATE *elem = model->user_data;
    u8_t remain_byte = get_remain_byte(&elem->state, is_ack);

    LOGD(TAG, "cur_actual(0x%04x) tar_actual(0x%04x) cur_temp(0x%04x) tar_temp(0x%04x) remain(0x%02x)",
         elem->state.lightness[T_CUR], elem->state.lightness[T_TAR],
         elem->state.temp[T_CUR], elem->state.temp[T_TAR], remain_byte);

    //prepear buff
    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_OP_2(0x82, 0x60));

    net_buf_simple_add_le16(p_msg, elem->state.lightness[T_CUR]);
    net_buf_simple_add_le16(p_msg, elem->state.temp[T_CUR]);

    if (remain_byte) {
        net_buf_simple_add_le16(p_msg, elem->state.lightness[T_TAR]);
        net_buf_simple_add_le16(p_msg, elem->state.temp[T_TAR]);
        net_buf_simple_add_u8(p_msg, remain_byte);
    }
}

static int _light_ctl_prepare_publication(struct bt_mesh_model *model)
{

    struct net_buf_simple *msg;
    if (!model) {
        return -1;
    }

    msg = model->pub->msg;

    LOGD(TAG, "addr(0x%04x)", model->pub->addr);

    if (model->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        _ctl_prepare_buf(model, msg, 0);
    }

    return 0;

}

int ble_mesh_light_ctl_publication(struct bt_mesh_model *model)
{

    struct net_buf_simple *msg;
    int16_t ret;

    if (!model) {
        return -1;
    }

    msg = model->pub->msg;

    LOGD(TAG, "addr(0x%04x)", model->pub->addr);

    if (model->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        _ctl_prepare_buf(model, msg, 0);

        ret = bt_mesh_model_publish(model);

        if (ret) {
            LOGE(TAG, "bt_mesh_model_publish err %d\n", ret);
            return ret;
        }

        LOGD(TAG, "Success!!!");
    }

    return 0;

}

static void _ctl_status(struct bt_mesh_model *model,
                        struct bt_mesh_msg_ctx *p_ctx, bool is_ack)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 9 + 4);

    LOGD(TAG, "addr(0x%04x)", p_ctx->addr);

    _ctl_prepare_buf(model, p_msg, is_ack);

    if (bt_mesh_model_send(model, p_ctx, p_msg, NULL, NULL)) {
        LOGE(TAG, "Unable to send ctl Status");
    }

    LOGD(TAG, "Success!!!");
}

static u8_t _ctl_analyze(struct bt_mesh_model *model, u16_t src_addr, struct net_buf_simple *p_buf)
{
    u16_t lightness = 0;
    u16_t temp = 0;
    u16_t uv = 0;
    u8_t tid = 0;
    u8_t trans = 0;
    u8_t delay = 0;
    S_ELEM_STATE *elem = model->user_data;

    if (p_buf->len != 7 && p_buf->len != 9) {
        LOGE(TAG, "MESH_ANALYZE_SIZE_ERROR p_buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    //get message info
    lightness = net_buf_simple_pull_le16(p_buf);
    temp = net_buf_simple_pull_le16(p_buf);
    uv = net_buf_simple_pull_le16(p_buf);
    tid = net_buf_simple_pull_u8(p_buf);

    if (p_buf->len) {
        trans = net_buf_simple_pull_u8(p_buf);
        delay = net_buf_simple_pull_u8(p_buf);
    } else {
        //trans = elem->powerup.def_trans;
        delay = 0;
    }

    //check
    //1 lightness
    //2 temp
    if (temp < CTL_TEMP_MIN || temp > CTL_TEMP_MAX) {
        LOGE(TAG, "MESH_ANALYZE_ARGS_ERROR temp(0x%04x)", temp);
        return MESH_ANALYZE_ARGS_ERROR;
    }

    //3 uv
    //4 tid
    if (mesh_check_tid(src_addr, tid) != MESH_SUCCESS) {
        LOGE(TAG, "MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", src_addr, tid);
        return MESH_TID_REPEAT;
    }

    //5 trans
    if ((trans & 0x3F) == 0x3F) {
        LOGE(TAG, "MESH_SET_TRANSTION_ERROR");
        return MESH_SET_TRANSTION_ERROR;
    }

    LOGD(TAG, "temp(0x%04x), min(0x%04x), max(0x%04x)", temp, elem->powerup.ctl_temp_range.range_min, elem->powerup.ctl_temp_range.range_max);

    if (temp < elem->powerup.ctl_temp_range.range_min) {
        temp = elem->powerup.ctl_temp_range.range_min;
    }

    if (temp > elem->powerup.ctl_temp_range.range_max) {
        temp = elem->powerup.ctl_temp_range.range_max;
    }

    //
    elem->state.lightness[T_TAR] = lightness;
    elem->state.temp[T_TAR] = temp;
    elem->state.UV[T_TAR] = uv;

    //TODO bound :    mesh_state_bound(LIGHT_CTL, T_TAR);
#if 0
    mesh_state_bound(LIGHT_CTL, T_TAR);
#endif

    elem->state.trans = trans;
    elem->state.delay = delay;

    if (elem->state.trans) {
        elem->state.trans_start_time = aos_now_ms() + elem->state.delay * 5;
    }

    LOGD(TAG, "actual(0x%04x) temp(0x%04x) uv(0x%04x) trans(0x%02x) delay(0x%02x)",
         elem->state.lightness[T_TAR], elem->state.temp[T_TAR],
         elem->state.UV[T_TAR], elem->state.trans, elem->state.delay);

    LOGD(TAG, "start(%d) end(%d)", (bt_u32_t)elem->state.trans_start_time, (bt_u32_t)elem->state.trans_end_time);

    //model_event(GEN_EVT_SDK_ANALYZE_MSG, elem);

    if (elem->state.trans || elem->state.delay) {
        if (elem->state.delay) {
            //model_event(GEN_EVT_SDK_DELAY_START, elem);
        } else {
            //model_event(GEN_EVT_SDK_TRANS_START, elem);
        }
    } else {
        elem->state.lightness[T_CUR] = lightness;
        elem->state.temp[T_CUR] = elem->state.temp[T_TAR];
        elem->state.UV[T_CUR] = elem->state.UV[T_TAR];
        model_message message;
        message.source_addr = src_addr;
        message.status_data = NULL;
        message.user_data = elem;
        model_event(BT_MESH_MODEL_LIGHT_CTL_SET, &message);
    }

    return MESH_SUCCESS;
}


static void _ctl_get(struct bt_mesh_model *model,
                     struct bt_mesh_msg_ctx *p_ctx,
                     struct net_buf_simple *p_buf)
{
    LOGD(TAG, "");

    _ctl_status(model, p_ctx, 0);
}

static void _ctl_set(struct bt_mesh_model *model,
                     struct bt_mesh_msg_ctx *p_ctx,
                     struct net_buf_simple *p_buf)
{
    E_MESH_ERROR_TYPE ret = _ctl_analyze(model, p_ctx->addr, p_buf);

    LOGD(TAG, "ret %d", ret);

    if (ret == MESH_SUCCESS || ret == MESH_TID_REPEAT) {
        _ctl_status(model, p_ctx, 1);
    }
}

static void _ctl_set_unack(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *p_ctx,
                           struct net_buf_simple *p_buf)
{
    LOGD(TAG, "");
    _ctl_analyze(model, p_ctx->addr, p_buf);
}

static void _ctl_temp_range_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *p_ctx)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 5 + 4);
    S_ELEM_STATE *elem = model->user_data;


    LOGD(TAG, "range_status(0x%02x) min_temp(0x%04x) max_temp(0x%04x)",
         elem->powerup.ctl_temp_range.code, elem->powerup.ctl_temp_range.range_min,
         elem->powerup.ctl_temp_range.range_max);

    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_OP_2(0x82, 0x63));
    net_buf_simple_add_u8(p_msg, elem->powerup.ctl_temp_range.code);
    net_buf_simple_add_le16(p_msg, elem->powerup.ctl_temp_range.range_min);
    net_buf_simple_add_le16(p_msg, elem->powerup.ctl_temp_range.range_max);

    if (bt_mesh_model_send(model, p_ctx, p_msg, NULL, NULL)) {
        LOGE(TAG, "Unable to send temp range Status");
    }

    LOGD(TAG, "Success!!!");
}

static void _ctl_temp_range_get(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *p_ctx,
                                struct net_buf_simple *p_buf)
{
    LOGD(TAG, "");

    _ctl_temp_range_status(model, p_ctx);
}

static void _ctl_defatult_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *p_ctx)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 6 + 4);
    S_ELEM_STATE *elem = model->user_data;


    LOGD(TAG, "actual(0x%04x) temp(0x%04x) uv(0x%04x)",
         elem->powerup.lightness_default, elem->powerup.temp_default,
         elem->powerup.UV_default);

    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_OP_2(0x82, 0x68));
    net_buf_simple_add_le16(p_msg, elem->powerup.lightness_default);
    net_buf_simple_add_le16(p_msg, elem->powerup.temp_default);
    net_buf_simple_add_le16(p_msg, elem->powerup.UV_default);

    if (bt_mesh_model_send(model, p_ctx, p_msg, NULL, NULL)) {
        LOGE(TAG, "Unable to send default Status");
    }

    LOGD(TAG, "Success!!!");
}

static void _ctl_default_get(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *p_ctx,
                             struct net_buf_simple *p_buf)
{
    LOGD(TAG, "");

    _ctl_defatult_status(model, p_ctx);
}

const struct bt_mesh_model_op g_ctl_srv_op[CTL_OPC_NUM] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x5d), 0, _ctl_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x5e), 7, _ctl_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x5f), 7, _ctl_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x62), 0, _ctl_temp_range_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x67), 0, _ctl_default_get },
    BT_MESH_MODEL_OP_END,
};


//light ctl setup server
static E_MESH_ERROR_TYPE _ctl_default_analyze(struct bt_mesh_model *model, u16_t src_addr, struct net_buf_simple *p_buf)
{
    u16_t actual = 0;
    u16_t temp = 0;
    u16_t uv = 0;
    S_ELEM_STATE *elem = model->user_data;

    if (p_buf->len != 6) {
        LOGE(TAG, "MESH_ANALYZE_SIZE_ERROR p_buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    actual = net_buf_simple_pull_le16(p_buf);
    temp = net_buf_simple_pull_le16(p_buf);
    uv = net_buf_simple_pull_le16(p_buf);

    if (temp < elem->powerup.ctl_temp_range.range_min || temp > elem->powerup.ctl_temp_range.range_max) {
        LOGE(TAG, "MESH_ANALYZE_ARGS_ERROR tar(0x%04x) min(0x%04x) max(0x%04x)",
             temp, elem->powerup.ctl_temp_range.range_min, elem->powerup.ctl_temp_range.range_max);
        return MESH_ANALYZE_ARGS_ERROR;
    }

    elem->powerup.lightness_default = actual;
    elem->powerup.temp_default = temp;
    elem->powerup.UV_default = uv;

    LOGD(TAG, "actual(0x%04x) temp(0x%04x) uv(0x%04x)", elem->powerup.lightness_default,
         elem->powerup.temp_default, elem->powerup.UV_default);

    //model_event(GEN_EVT_SDK_ANALYZE_MSG, elem);

    if (elem->state.trans || elem->state.delay) {
        if (elem->state.delay) {
            //model_event(GEN_EVT_SDK_DELAY_START, elem);
        } else {
            //model_event(GEN_EVT_SDK_TRANS_START, elem);
        }
    } else {
        model_message message;
        message.source_addr = src_addr;
        message.user_data = elem;
        model_event(BT_MESH_MODEL_LIGHT_CTL_DEF_SET, (void *)&message);
    }

    return MESH_SUCCESS;
}

static void _ctl_default_set(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *p_ctx,
                             struct net_buf_simple *p_buf)
{
    LOGD(TAG, "");

    if (_ctl_default_analyze(model, p_ctx->addr, p_buf) == MESH_SUCCESS) {
        _ctl_defatult_status(model, p_ctx);
    }
}

static void _ctl_default_set_unack(struct bt_mesh_model *model,
                                   struct bt_mesh_msg_ctx *p_ctx,
                                   struct net_buf_simple *p_buf)
{
    LOGD(TAG, "");

    _ctl_default_analyze(model, p_ctx->addr, p_buf);
}

static E_MESH_ERROR_TYPE _ctl_temp_range_analyze(struct bt_mesh_model *model, u16_t src_addr, struct net_buf_simple *p_buf)
{
    u16_t min = 0;
    u16_t max = 0;
    S_ELEM_STATE *elem = model->user_data;

    if (p_buf->len != 4) {
        LOGE(TAG, "MESH_ANALYZE_SIZE_ERROR p_buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    min = net_buf_simple_pull_le16(p_buf);
    max = net_buf_simple_pull_le16(p_buf);

    if (!min || !max || min > max || min < CTL_TEMP_MIN || max > CTL_TEMP_MAX) {
        LOGE(TAG, "MESH_ANALYZE_ARGS_ERROR min(0x%04x) max(0x%04x)", min, max);
        return MESH_ANALYZE_ARGS_ERROR;
    }

    elem->powerup.ctl_temp_range.range_min = min;
    elem->powerup.ctl_temp_range.range_max = max;
    elem->powerup.ctl_temp_range.code = 0;
    LOGD(TAG, "min_temp(0x%04x) max_temp(0x%04x)",
         elem->powerup.ctl_temp_range.range_min, elem->powerup.ctl_temp_range.range_max);

    //model_event(GEN_EVT_SDK_ANALYZE_MSG, elem);

    if (elem->state.trans || elem->state.delay) {
        if (elem->state.delay) {
            //model_event(GEN_EVT_SDK_DELAY_START, elem);
        } else {
            //model_event(GEN_EVT_SDK_TRANS_START, elem);
        }
    } else {
        model_message message;
        message.source_addr = src_addr;
        message.user_data = elem;
        model_event(BT_MESH_MODEL_LIGHT_CTL_RANGE_SET, (void *)&message);
    }

    return MESH_SUCCESS;
}

static void _ctl_temp_range_set(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *p_ctx,
                                struct net_buf_simple *p_buf)
{
    LOGD(TAG, "");

    if (_ctl_temp_range_analyze(model, p_ctx->addr, p_buf) == MESH_SUCCESS) {
        _ctl_temp_range_status(model, p_ctx);
    }
}

static void _ctl_temp_range_set_unack(struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *p_ctx,
                                      struct net_buf_simple *p_buf)
{
    LOGD(TAG, "");

    _ctl_temp_range_analyze(model, p_ctx->addr, p_buf);
}

const struct bt_mesh_model_op g_ctl_setup_srv_op[CTL_SETUP_OPC_NUM] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x69), 6, _ctl_default_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x6A), 6, _ctl_default_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x6B), 4, _ctl_temp_range_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x6C), 4, _ctl_temp_range_set_unack },
    BT_MESH_MODEL_OP_END,
};


//ctl temperatur server
static void _ctl_temp_prepear_buf(struct bt_mesh_model *model, struct net_buf_simple *p_msg, bool is_ack)
{
    S_ELEM_STATE *elem = model->user_data;
    u8_t remain_byte = get_remain_byte(&elem->state, is_ack);

    LOGD(TAG, "cur_temp(0x%04x) tar_temp(0x%04x) uv(0x%04x) uv(0x%04x) remain(0x%02x)",
         elem->state.temp[T_CUR], elem->state.temp[T_TAR],
         elem->state.UV[T_CUR], elem->state.UV[T_TAR], remain_byte);

    //prepear buff
    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_OP_2(0x82, 0x66));

    net_buf_simple_add_le16(p_msg, elem->state.temp[T_CUR]);
    net_buf_simple_add_le16(p_msg, elem->state.UV[T_CUR]);

    if (remain_byte) {
        net_buf_simple_add_le16(p_msg, elem->state.temp[T_TAR]);
        net_buf_simple_add_le16(p_msg, elem->state.UV[T_TAR]);
        net_buf_simple_add_u8(p_msg, remain_byte);
    }
}

static void _ctl_temp_status(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *p_ctx, bool is_ack)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 9 + 4);

    LOGD(TAG, "addr(0x%04x)", p_ctx->addr);

    _ctl_temp_prepear_buf(model, p_msg, is_ack);

    if (bt_mesh_model_send(model, p_ctx, p_msg, NULL, NULL)) {
        LOGE(TAG, "Unable to send ctl Status");
    }

    LOGD(TAG, "Success!!!");
}

static u8_t _ctl_temp_analyze(struct bt_mesh_model *model, u16_t src_addr, struct net_buf_simple *p_buf)
{
    u16_t temp = 0;
    u16_t uv = 0;
    u8_t tid = 0;
    u8_t trans = 0;
    u8_t delay = 0;

    S_ELEM_STATE *elem = model->user_data;

    if (p_buf->len != 5 && p_buf->len != 7) {
        LOGE(TAG, "MESH_ANALYZE_SIZE_ERROR p_buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    temp = net_buf_simple_pull_le16(p_buf);

    if (temp < CTL_TEMP_MIN || temp > CTL_TEMP_MAX) {
        LOGE(TAG, "MESH_ANALYZE_ARGS_ERROR temp(0x%04x)", temp);
        return MESH_ANALYZE_ARGS_ERROR;
    }

    if (temp < elem->powerup.ctl_temp_range.range_min) {
        temp = elem->powerup.ctl_temp_range.range_min;
    }

    if (temp > elem->powerup.ctl_temp_range.range_max) {
        temp = elem->powerup.ctl_temp_range.range_max;
    }

    uv = net_buf_simple_pull_le16(p_buf);

    tid = net_buf_simple_pull_u8(p_buf);

    if (mesh_check_tid(src_addr, tid) != MESH_SUCCESS) {
        LOGE(TAG, "MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", src_addr, tid);
        return MESH_TID_REPEAT;
    }

    if (p_buf->len) {
        trans = net_buf_simple_pull_u8(p_buf);
        delay = net_buf_simple_pull_u8(p_buf);
    }

    if ((trans & 0x3F) == 0x3F) {
        LOGE(TAG, "MESH_SET_TRANSTION_ERROR");
        return MESH_SET_TRANSTION_ERROR;
    }

    elem->state.temp[T_TAR] = temp;
    elem->state.UV[T_TAR] = uv;
    //mesh_state_bound(LIGHT_CTL_TEMP, T_TAR);

    //elem->state.trans = trans?trans:elem->powerup.def_trans;
    elem->state.delay = delay;

    if (elem->state.trans) {
        elem->state.trans_start_time = aos_now_ms() + elem->state.delay * 5;
    }

    LOGD(TAG, "temp(0x%04x) uv(0x%04x) trans(0x%02x) delay(0x%02x)",
         elem->state.temp[T_TAR], elem->state.UV[T_TAR],
         elem->state.trans, elem->state.delay);

    //model_event(GEN_EVT_SDK_ANALYZE_MSG, (void *)elem);

    if (elem->state.trans || elem->state.delay) {
        if (elem->state.delay) {
            //model_event(GEN_EVT_SDK_DELAY_START, (void *)elem);
        } else {
            //model_event(GEN_EVT_SDK_TRANS_START, (void *)elem);
        }
    } else {
        elem->state.temp[T_CUR] = elem->state.temp[T_TAR] ;
        elem->state.UV[T_CUR] = elem->state.UV[T_TAR];
        model_message message;
        message.source_addr = src_addr;
        message.user_data = elem;
        model_event(BT_MESH_MODEL_LIGHT_CTL_TEMP_SET, (void *)&message);
    }

    return MESH_SUCCESS;
}

/*
static void _ctl_temp_done(void)
{
    LOGD(TAG, "");
    //elem->state.temp[T_CUR] = elem->state.temp[T_TAR];
    //elem->state.UV[T_CUR] = elem->state.UV[T_TAR];
    //mesh_state_bound(LIGHT_CTL_TEMP, T_CUR);
    //ctl_gpio(elem->state.temp[T_CUR]);
}


static void _ctl_temp_transition(struct bt_mesh_model *model)
{
    S_ELEM_STATE *elem = model->user_data;
    LOGD(TAG, "trans %d", get_transition_time(elem->state.trans));
    aos_msleep(get_transition_time(elem->state.trans));
    LOGD(TAG, "trans end");

    elem->state.trans = 0;
    elem->state.trans_start_time = 0;

    _ctl_temp_done();
    //mesh_publication(model->elem, MESH_PUB_CTL_TEMP);
}*/

/*
static void _ctl_temp_delay(struct bt_mesh_model *model)
{

    S_ELEM_STATE *elem = model->user_data;
    LOGD(TAG, "delay %d", elem->state.delay*5);
    aos_msleep(elem->state.delay*5);
    LOGD(TAG, "delay end");

    elem->state.delay = 0;

    if(elem->state.trans == 0) {
        _ctl_temp_done();
        //mesh_publication(model->elem, MESH_PUB_CTL_TEMP);
    }
    else {
        aos_schedule_call(_ctl_temp_transition, model);
    }
}*/

#if 0
static bool _ctl_temp_action(struct bt_mesh_model *model)
{
    S_ELEM_STATE *elem = model->user_data;

    if (elem->state.trans || elem->state.delay) {
        if (elem->state.delay) {
            aos_schedule_call(_ctl_temp_delay, model);
        } else {
            aos_schedule_call(_ctl_temp_transition, model);
        }

        return 0;
    } else {
        _ctl_temp_done();
        return 1;
    }
}
#endif

void ble_mesh_light_ctl_temp_publication(struct bt_mesh_model *model)
{
    struct net_buf_simple *p_msg = model->pub->msg;
    int err;

    LOGD(TAG, "addr(0x%04x)", model->pub->addr);

    /*
     * If a server has a publish address, it is required to
     * publish status on a state change
     *
     * See Mesh Profile Specification 3.7.6.1.2
     *
     * Only publish if there is an assigned address
     */
    if (model->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        _ctl_temp_prepear_buf(model, p_msg, 0);

        err = bt_mesh_model_publish(model);

        if (err) {
            LOGE(TAG, "bt_mesh_model_publish err %d\n", err);
        }

        LOGD(TAG, "Success!!!");
    }
}

static void _ctl_temp_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *p_ctx,
                          struct net_buf_simple *p_buf)
{
    LOGD(TAG, "");

    _ctl_temp_status(model, p_ctx, 0);
}

static void _ctl_temp_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *p_ctx,
                          struct net_buf_simple *p_buf)
{
    LOGD(TAG, "");

    if (_ctl_temp_analyze(model, p_ctx->addr, p_buf) == MESH_SUCCESS) {
#if 0
        pub_need = _ctl_temp_action(model);
        _ctl_temp_status(model, p_ctx, 1);

        if (pub_need && model->pub->addr != p_ctx->addr) {
            mesh_publication(model->elem, MESH_PUB_CTL_TEMP);
        }

#else
        _ctl_temp_status(model, p_ctx, 1);
#endif
    }
}

static void _ctl_temp_set_unack(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *p_ctx,
                                struct net_buf_simple *p_buf)
{
    LOGD(TAG, "");

    if (_ctl_temp_analyze(model, p_ctx->addr, p_buf) == MESH_SUCCESS) {
#if 0
        pub_need = _ctl_temp_action(model);

        if (pub_need && model->pub->addr != p_ctx->addr) {
            mesh_publication(model->elem, MESH_PUB_CTL_TEMP);
        }

#endif
    }
}

const struct bt_mesh_model_op g_ctl_temp_srv_op[CTL_TEMP_OPC_NUM] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x61), 0, _ctl_temp_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x64), 5, _ctl_temp_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x65), 0, _ctl_temp_set_unack },
    BT_MESH_MODEL_OP_END,
};

#endif
