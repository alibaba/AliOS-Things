/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "common/log.h"
#include "mesh.h"

#include <atomic.h>
#include "genie_service.h"

static ATOMIC_DEFINE(indicate_flags, SIG_MODEL_INDICATE_FLAGS);

static sig_model_event_e sig_model_event_handle_delay_start(sig_model_element_state_t *p_elem)
{
#ifdef CONFIG_MESH_MODEL_TRANS
    sig_model_transition_timer_stop(p_elem);
#endif

    k_timer_start(&p_elem->state.delay_timer, p_elem->state.delay * 5);

    return SIG_MODEL_EVT_NONE;
}

static sig_model_event_e sig_model_event_handle_delay_end(sig_model_element_state_t *p_elem)
{
#ifdef CONFIG_MESH_MODEL_TRANS
    bt_u32_t cur_time = k_uptime_get();
#endif

    p_elem->state.delay = 0;

#ifdef CONFIG_MESH_MODEL_TRANS
    if (p_elem->state.trans == 0 || cur_time >= p_elem->state.trans_end_time)
    {
        sig_model_transition_state_reset(p_elem);
        return SIG_MODEL_EVT_ACTION_DONE;
    }
    else
    {
        return SIG_MODEL_EVT_TRANS_START;
    }
#else
    return SIG_MODEL_EVT_ACTION_DONE;
#endif
}

#ifdef CONFIG_MESH_MODEL_TRANS
static sig_model_event_e sig_model_event_handle_trans_start(sig_model_element_state_t *p_elem)
{
    bt_u32_t cur_time = k_uptime_get();

    sig_model_transition_timer_stop(p_elem);

    //check time
    if (cur_time >= p_elem->state.trans_end_time - SIG_MODEL_TRANSITION_INTERVAL)
    {
        return SIG_MODEL_EVT_TRANS_END;
    }
    else
    {
        //start cycle
        k_timer_start(&p_elem->state.trans_timer, SIG_MODEL_TRANSITION_INTERVAL);
        BT_DBG("start trans %p", &p_elem->state.trans_timer);

        return SIG_MODEL_EVT_NONE;
    }
}

static sig_model_event_e sig_model_event_handle_trans_cycle(sig_model_element_state_t *p_elem)
{
    if (sig_model_transition_update(p_elem) == 0)
    {
        p_elem->state.trans = 0;
    }

    return SIG_MODEL_EVT_NONE;
}

static sig_model_event_e sig_model_event_handle_trans_end(sig_model_element_state_t *p_elem)
{
    //clear paras
    sig_model_transition_state_reset(p_elem);
    //action done
    return SIG_MODEL_EVT_ACTION_DONE;
}
#endif

static sig_model_event_e sig_model_event_handle_analyze_msg(sig_model_element_state_t *p_elem)
{
#ifdef CONFIG_MESH_MODEL_TRANS
    if (p_elem->state.trans || p_elem->state.delay)
    {
        if (p_elem->state.delay)
        {
            return SIG_MODEL_EVT_DELAY_START;
        }
        else
        {
            return SIG_MODEL_EVT_TRANS_START;
        }
    }
#endif

    return SIG_MODEL_EVT_ACTION_DONE;
}

static sig_model_event_e sig_model_handle_action_done(sig_model_element_state_t *p_elem)
{
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    BT_DBG("onoff cur(%d) tar(%d)", p_elem->state.onoff[TYPE_PRESENT], p_elem->state.onoff[TYPE_TARGET]);

    if (p_elem->state.onoff[TYPE_PRESENT] != p_elem->state.onoff[TYPE_TARGET])
    {
        p_elem->state.onoff[TYPE_PRESENT] = p_elem->state.onoff[TYPE_TARGET];
        atomic_set_bit(indicate_flags, SIG_MODEL_INDICATE_GEN_ONOFF);
    }
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    BT_DBG("lightness cur(%04x) tar(%04x)", p_elem->state.lightness[TYPE_PRESENT], p_elem->state.lightness[TYPE_TARGET]);

    if (p_elem->state.lightness[TYPE_PRESENT] != p_elem->state.lightness[TYPE_TARGET])
    {
        p_elem->state.lightness[TYPE_PRESENT] = p_elem->state.lightness[TYPE_TARGET];
        atomic_set_bit(indicate_flags, SIG_MODEL_INDICATE_GEN_LIGHTNESS);
    }
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
    BT_DBG("color_temperature cur(%04x) tar(%04x)", p_elem->state.color_temperature[TYPE_PRESENT], p_elem->state.color_temperature[TYPE_TARGET]);

    if (p_elem->state.color_temperature[TYPE_PRESENT] != p_elem->state.color_temperature[TYPE_TARGET])
    {
        p_elem->state.color_temperature[TYPE_PRESENT] = p_elem->state.color_temperature[TYPE_TARGET];
        atomic_set_bit(indicate_flags, SIG_MODEL_INDICATE_GEN_CTL);
    }
#endif

#ifdef CONFIG_MESH_MODEL_SCENE_SRV
    BT_DBG("scene cur(%04x) tar(%04x)", p_elem->state.scene[TYPE_PRESENT], p_elem->state.scene[TYPE_TARGET]);

    if (p_elem->state.scene[TYPE_PRESENT] != p_elem->state.scene[TYPE_TARGET])
    {
        p_elem->state.scene[TYPE_PRESENT] = p_elem->state.scene[TYPE_TARGET];
        atomic_set_bit(indicate_flags, SIG_MODEL_INDICATE_GEN_SCENE);
    }
#endif

    if (bt_mesh_is_provisioned())
    {
        return SIG_MODEL_EVT_INDICATE;
    }

    return SIG_MODEL_EVT_NONE;
}

static sig_model_event_e sig_model_handle_indicate(sig_model_element_state_t *p_elem)
{
    uint8_t payload[SIG_MODEL_INDICATE_PAYLOAD_MAX_LEN];
    uint8_t payload_len = 0;
    genie_transport_model_param_t transport_model_param = {0};

    if (p_elem == NULL)
    {
        return SIG_MODEL_EVT_NONE;
    }

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    if (atomic_test_and_clear_bit(indicate_flags, SIG_MODEL_INDICATE_GEN_ONOFF))
    {
        payload[payload_len++] = ATTR_TYPE_GENERIC_ONOFF & 0xff;
        payload[payload_len++] = (ATTR_TYPE_GENERIC_ONOFF >> 8) & 0xff;
        payload[payload_len++] = p_elem->state.onoff[TYPE_PRESENT];
    }
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    if (atomic_test_and_clear_bit(indicate_flags, SIG_MODEL_INDICATE_GEN_LIGHTNESS))
    {
        payload[payload_len++] = ATTR_TYPE_LIGHTNESS & 0xff;
        payload[payload_len++] = (ATTR_TYPE_LIGHTNESS >> 8) & 0xff;
        payload[payload_len++] = p_elem->state.lightness[TYPE_PRESENT] & 0xff;
        payload[payload_len++] = (p_elem->state.lightness[TYPE_PRESENT] >> 8) & 0xff;
    }
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
    if (atomic_test_and_clear_bit(indicate_flags, SIG_MODEL_INDICATE_GEN_CTL))
    {
        payload[payload_len++] = ATTR_TYPE_COLOR_TEMPERATURE & 0xff;
        payload[payload_len++] = (ATTR_TYPE_COLOR_TEMPERATURE >> 8) & 0xff;
        payload[payload_len++] = p_elem->state.color_temperature[TYPE_PRESENT] & 0xff;
        payload[payload_len++] = (p_elem->state.color_temperature[TYPE_PRESENT] >> 8) & 0xff;
    }
#endif
#ifdef CONFIG_MESH_MODEL_SCENE_SRV
    if (atomic_test_and_clear_bit(indicate_flags, SIG_MODEL_INDICATE_GEN_SCENE))
    {
        payload[payload_len++] = ATTR_TYPE_SENCE & 0xff;
        payload[payload_len++] = (ATTR_TYPE_SENCE >> 8) & 0xff;
        payload[payload_len++] = p_elem->state.scene[TYPE_PRESENT] & 0xff;
        payload[payload_len++] = (p_elem->state.scene[TYPE_PRESENT] >> 8) & 0xff;
    }
#endif

    if (payload_len > 0)
    {
        memset(&transport_model_param, 0, sizeof(genie_transport_model_param_t));
        transport_model_param.opid = VENDOR_OP_ATTR_INDICATE;
        transport_model_param.data = payload;
        transport_model_param.len = MIN(SIG_MODEL_INDICATE_PAYLOAD_MAX_LEN, payload_len);
        transport_model_param.p_elem = bt_mesh_elem_find_by_id(p_elem->element_id);
        transport_model_param.retry_period = GENIE_TRANSPORT_EACH_PDU_TIMEOUT * genie_transport_get_seg_count(transport_model_param.len);
        transport_model_param.retry = GENIE_TRANSPORT_DEFAULT_RETRY_COUNT;

        genie_transport_send_model(&transport_model_param);
    }

    return SIG_MODEL_EVT_NONE;
}

void sig_model_event_set_indicate(int indicate)
{
    atomic_set_bit(indicate_flags, indicate);
}

void sig_model_event(sig_model_event_e event, void *p_arg)
{
    sig_model_event_e next_event = SIG_MODEL_EVT_NONE;

    if (event != SIG_MODEL_EVT_NONE)
    {
#ifdef CONFIG_MESH_MODEL_TRANS
        if (event != SIG_MODEL_EVT_TRANS_CYCLE)
        {
            GENIE_LOG_INFO("SigE:%d\r\n", event);
        }
#else
        GENIE_LOG_INFO("SigE:%d\r\n", event);
#endif
    }

    switch (event)
    {
    case SIG_MODEL_EVT_INDICATE:
    {
        next_event = sig_model_handle_indicate((sig_model_element_state_t *)p_arg);
    }
    break;
    case SIG_MODEL_EVT_ACTION_DONE:
    {
        next_event = sig_model_handle_action_done((sig_model_element_state_t *)p_arg);
    }
    break;
    case SIG_MODEL_EVT_ANALYZE_MSG:
    {
        next_event = sig_model_event_handle_analyze_msg((sig_model_element_state_t *)p_arg);
    }
    break;

    case SIG_MODEL_EVT_DELAY_START:
    {
        next_event = sig_model_event_handle_delay_start((sig_model_element_state_t *)p_arg);
    }
    break;
    case SIG_MODEL_EVT_DELAY_END:
    {
        next_event = sig_model_event_handle_delay_end((sig_model_element_state_t *)p_arg);
    }
    break;
#ifdef CONFIG_MESH_MODEL_TRANS
    case SIG_MODEL_EVT_TRANS_START:
    {
        next_event = sig_model_event_handle_trans_start((sig_model_element_state_t *)p_arg);
    }
    break;
    case SIG_MODEL_EVT_TRANS_CYCLE:
    {
        next_event = sig_model_event_handle_trans_cycle((sig_model_element_state_t *)p_arg);
    }
    break;
    case SIG_MODEL_EVT_TRANS_END:
    {
        next_event = sig_model_event_handle_trans_end((sig_model_element_state_t *)p_arg);
    }
    break;
#endif
    case SIG_MODEL_EVT_GENERIC_MESG:
    {
        sig_model_msg *p_msg = (sig_model_msg *)p_arg;
        genie_down_msg(GENIE_DOWN_MESG_SIG_TYPE, p_msg->opcode, p_arg);
    }
    break;
    default:
        break;
    }

    if (next_event != SIG_MODEL_EVT_NONE)
    {
        sig_model_event(next_event, p_arg);
    }

    if (event == SIG_MODEL_EVT_ACTION_DONE)
    {
        genie_event(GENIE_EVT_USER_ACTION_DONE, p_arg);
    }
#ifdef CONFIG_MESH_MODEL_TRANS
    if (event == SIG_MODEL_EVT_TRANS_CYCLE)
    {
        genie_event(GENIE_EVT_USER_TRANS_CYCLE, p_arg);
    }
#endif
}
