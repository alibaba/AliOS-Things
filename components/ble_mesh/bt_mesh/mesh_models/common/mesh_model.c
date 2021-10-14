/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <bt_errno.h>
#include "mesh_model/mesh_model.h"

#ifdef CONFIG_BT_MESH_PROVISIONER
#include "provisioner_main.h"
#endif

extern u8_t bt_mesh_default_ttl_get(void);

#define TAG "BT_MESH_MODEL"
#define RECV_MSG_TID_QUEUE_SIZE 20
#define MESH_TRNSATION_CYCLE 100
#define CID_NVAL 0XFFFF
#define TIMEOUT 400
typedef struct _mesh_model {
    uint8_t init_flag;
    const struct bt_mesh_comp *mesh_comp;
    model_event_cb user_model_event_cb;
    S_ELEM_STATE *element_states;
} mesh_model;

mesh_model g_mesh_model;


typedef struct {
    u8_t tid;
    u16_t addr;
    bt_u32_t time;
} _TID_QUEUE_S;



u16_t TRANS_TIMES[] = {1, 10, 100, 6000};

_TID_QUEUE_S tid_queue[RECV_MSG_TID_QUEUE_SIZE];

static void _mesh_timer_stop(S_MESH_STATE *p_state)
{
    k_timer_stop(&p_state->delay_timer);
    k_timer_stop(&p_state->trans_timer);
}

static void _mesh_delay_timer_cb(void *p_timer, void *args)
{
    S_ELEM_STATE *p_elem = (S_ELEM_STATE *)args;

    _mesh_timer_stop(&p_elem->state);
    //model_event(GEN_EVT_SDK_DELAY_END, (void *)p_elem);
}

static void _mesh_trans_timer_cycle(void *p_timer, void *args)
{
    S_ELEM_STATE *p_elem = (S_ELEM_STATE *)args;
    bt_u32_t cur_time = k_uptime_get();

    _mesh_timer_stop(&p_elem->state);

    //LOGD(TAG, ">>>>>%d %d", (bt_u32_t)cur_time, (bt_u32_t)p_elem->state.trans_end_time);
    if (cur_time >= p_elem->state.trans_end_time) {
        //model_event(GEN_EVT_SDK_TRANS_END, (void *)p_elem);
    } else {
        //model_event(GEN_EVT_SDK_TRANS_CYCLE, (void *)p_elem);
        k_timer_start(&p_elem->state.trans_timer, MESH_TRNSATION_CYCLE);
    }
}


uint8_t elem_state_init(struct bt_mesh_elem *elem)
{
    S_ELEM_STATE *elemState = (S_ELEM_STATE *)elem->models[2].user_data;

    if (!elemState) {
        return 0;
    }

    if (!elemState->state.timerInitFlag) {
        k_timer_init(&elemState->state.delay_timer, _mesh_delay_timer_cb, elemState);
        k_timer_init(&elemState->state.trans_timer, _mesh_trans_timer_cycle, elemState);
    }

    elemState->state.timerInitFlag = true;
    return 0;
}


bt_u32_t get_transition_time(u8_t byte)
{
    if ((byte & 0x3F) == 0x3F) {
        //MODEL_E("%s ERROR, invalid 0x%02X!!!\n", __func__, byte);
        return 0xFFFFFFFF;
    }

    return (byte & 0x3F) * TRANS_TIMES[byte >> 6] * 100;
}

static u8_t _get_transition_byte(bt_u32_t time)
{
    //LOGD(TAG, "time(%d)", time);

    time /= 100;

    if (time > TRANS_TIMES[3] * 62) {
        return 0;
    } else if (time > TRANS_TIMES[2] * 62) {
        return (time / TRANS_TIMES[3]) | 0xC0;
    } else if (time > TRANS_TIMES[1] * 62) {
        return (time / TRANS_TIMES[2]) | 0x80;
    } else if (time > TRANS_TIMES[0] * 62) {
        return (time / TRANS_TIMES[1]) | 0x40;
    } else {
        return (time / TRANS_TIMES[0]);
    }
}


u8_t get_remain_byte(S_MESH_STATE *p_state, bool is_ack)
{
    u8_t remain_byte = p_state->trans;
    bt_u32_t cur_time = k_uptime_get();

    if (!is_ack && p_state->trans_start_time < cur_time) {
        cur_time -= p_state->trans_start_time;
        bt_u32_t l_trans = get_transition_time(p_state->trans);

        if (l_trans == 0xFFFFFFFF) {
            remain_byte = 0x3F;
        } else if (l_trans > cur_time) {
            remain_byte = _get_transition_byte(l_trans - cur_time);
        } else {
            remain_byte = 0;
        }

    }

    //LOGD(TAG, "remain_byte(0x%02x)", remain_byte);

    return remain_byte;
}

E_MESH_ERROR_TYPE mesh_check_tid(u16_t src_addr, u8_t tid)
{
    static u8_t cur_index = 0;
    u8_t i = cur_index;
    u8_t ri = 0;
    bt_u32_t cur_time = k_uptime_get();
    bt_u32_t end_time = 0;

    while (i < cur_index + RECV_MSG_TID_QUEUE_SIZE) {
        ri = i % RECV_MSG_TID_QUEUE_SIZE;

        if (tid_queue[ri].tid == tid && tid_queue[ri].addr == src_addr) {
            end_time = tid_queue[ri].time + 6000;

            if (cur_time < end_time) {
                break;
            }
        }

        i++;
    }

    if (i < cur_index + RECV_MSG_TID_QUEUE_SIZE) {
        return MESH_TID_REPEAT;
    } else {
        tid_queue[cur_index].tid = tid;
        tid_queue[cur_index].addr = src_addr;
        tid_queue[cur_index].time = cur_time;
        cur_index++;
        cur_index %= RECV_MSG_TID_QUEUE_SIZE;
        return MESH_SUCCESS;
    }
}


void model_message_process(uint8_t event, void *p_arg)
{
    switch (event) {
        default:
            break;
    }

    return;
}


void model_event(mesh_model_event_en event, void *p_arg)
{

    if (g_mesh_model.user_model_event_cb) {
        g_mesh_model.user_model_event_cb(event, p_arg);
    }

}


static int mesh_model_bind_map_get(const struct bt_mesh_comp *mesh_comp)
{
    if (!mesh_comp) {
        return -1;
    }

    return 0;
}

static int mesh_model_set_user_data(const struct bt_mesh_comp *comp, S_ELEM_STATE *state)
{
    uint8_t ele_num;
    uint8_t model_num;

    for (ele_num = 0 ; ele_num < comp->elem_count; ele_num++) {
        for (model_num = 0; model_num < comp->elem[ele_num].model_count; model_num++) {
            if (comp->elem[ele_num].models[model_num].id != BT_MESH_MODEL_ID_CFG_SRV &&  \
                comp->elem[ele_num].models[model_num].id != BT_MESH_MODEL_ID_CFG_CLI && \
                comp->elem[ele_num].models[model_num].id != BT_MESH_MODEL_ID_HEALTH_SRV && \
                comp->elem[ele_num].models[model_num].id != BT_MESH_MODEL_ID_HEALTH_CLI) {
                comp->elem[ele_num].models[model_num].user_data = &state[ele_num];
            }
        }
    }

    return 0;
}


int ble_mesh_model_init(const struct bt_mesh_comp *comp)
{
    if (!comp) {
        return -1;
    }

    int16_t ret;
    ret = mesh_model_bind_map_get(comp);

    if (ret) {
        return -1;
    }

    if (g_mesh_model.element_states) {
        free(g_mesh_model.element_states);
        g_mesh_model.element_states = NULL;
    }

    g_mesh_model.element_states = (S_ELEM_STATE *)malloc(sizeof(S_ELEM_STATE) * comp->elem_count);

    if (!g_mesh_model.element_states) {
        return -1;
    }

    memset(g_mesh_model.element_states, 0, sizeof(S_ELEM_STATE) * comp->elem_count);

    ret = mesh_model_set_user_data(comp, g_mesh_model.element_states);

    if (ret) {
        return -1;
    }

    g_mesh_model.mesh_comp = comp;
    g_mesh_model.init_flag = 1;

    return 0;
}


const struct bt_mesh_comp *ble_mesh_model_get_comp_data()
{
    return g_mesh_model.mesh_comp;
}

int ble_mesh_model_set_cb(model_event_cb event_cb)
{
    if (!g_mesh_model.init_flag || !event_cb) {
        return -1;
    }

    g_mesh_model.user_model_event_cb = event_cb;
    return 0;
}

struct bt_mesh_model *ble_mesh_model_find(uint16_t elem_idx, uint16_t mod_idx, uint16_t CID)
{
    if (!g_mesh_model.init_flag) {
        return NULL;
    }

    if (elem_idx > g_mesh_model.mesh_comp->elem_count) {
        return NULL;
    }

    if (CID_NVAL != CID) {
        extern struct bt_mesh_model *bt_mesh_model_find_vnd(struct bt_mesh_elem * elem, u16_t company, u16_t id);
        return (struct bt_mesh_model *)bt_mesh_model_find_vnd(&g_mesh_model.mesh_comp->elem[elem_idx], CID, mod_idx);
    } else {
        extern struct bt_mesh_model *bt_mesh_model_find(struct bt_mesh_elem * elem, u16_t id);
        return (struct bt_mesh_model *)bt_mesh_model_find(&g_mesh_model.mesh_comp->elem[elem_idx], mod_idx);
    }

}


int ble_mesh_model_status_get(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr,struct bt_mesh_model *model,uint32_t op_code)
{
    int err;
    struct bt_mesh_msg_ctx ctx = {0};

    if (0x0000 == unicast_addr) {
        return -EADDRNOTAVAIL;
    }

    if (!model) {
        return -EINVAL;
    }

    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 0 + 6);

    bt_mesh_model_msg_init(msg, op_code);

    ctx.addr = unicast_addr;

    ctx.net_idx = netkey_idx;

    ctx.app_idx = appkey_idx;
    ctx.send_ttl = bt_mesh_default_ttl_get();

    err =  bt_mesh_model_send(model, &ctx, msg, NULL, NULL);

    if (err) {
        LOGE(TAG, "mesh model get status send fail %d", err);
        return err;
    }

    return 0;
}

