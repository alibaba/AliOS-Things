/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
//#include <aos/aos.h>
#include <aos/kernel.h>

#include <misc/printk.h>
#include <misc/byteorder.h>
//#include <hal/soc/gpio.h>
//#include <hal/soc/pwm.h>

#include <bluetooth/bluetooth.h>
//#include <soc.h>
#include <api/mesh.h>
#include "common/log.h"
#include "genie_service.h"

#ifdef CONFIG_MESH_MODEL_TRANS
void sig_model_transition_timer_stop(sig_model_element_state_t *p_elem)
{
    k_timer_stop(&p_elem->state.delay_timer);
    k_timer_stop(&p_elem->state.trans_timer);
}

void sig_model_transition_state_reset(sig_model_element_state_t *p_elem)
{
    p_elem->state.trans = 0;
    p_elem->state.trans_start_time = 0;
    p_elem->state.trans_end_time = 0;
}

#define DELTA_ACTUAL_MIN 655
#define DELTA_TEMP_MIN 192

uint8_t sig_model_transition_update(sig_model_element_state_t *p_elem)
{
    uint8_t cycle = 0;
    uint16_t delta = 0;
    bt_u32_t cur_time = k_uptime_get();
    sig_model_state_t *p_state = &p_elem->state;

    //stop cycle when timeout
    if (cur_time <= p_state->trans_end_time - SIG_MODEL_TRANSITION_INTERVAL)
    {
#if defined(CONFIG_MESH_MODEL_LIGHTNESS_SRV) || defined(CONFIG_MESH_MODEL_CTL_SRV)
        uint16_t step = (p_state->trans_end_time - cur_time) / SIG_MODEL_TRANSITION_INTERVAL;
#endif

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        if (p_state->onoff[TYPE_PRESENT] != p_state->onoff[TYPE_TARGET])
        {
            p_state->onoff[TYPE_PRESENT] = p_state->onoff[TYPE_TARGET];
            sig_model_event_set_indicate(SIG_MODEL_INDICATE_GEN_ONOFF);
        }
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        if (p_state->lightness[TYPE_PRESENT] != p_state->lightness[TYPE_TARGET])
        {
            if (p_state->lightness[TYPE_TARGET] > p_state->lightness[TYPE_PRESENT])
            {
                delta = (p_state->lightness[TYPE_TARGET] - p_state->lightness[TYPE_PRESENT]) / step;
                delta = delta > DELTA_ACTUAL_MIN ? delta : DELTA_ACTUAL_MIN;
                if (LIGHTNESS_MAX_VALUE - p_state->lightness[TYPE_PRESENT] < delta ||
                    p_state->lightness[TYPE_TARGET] - p_state->lightness[TYPE_PRESENT] < delta)
                {
                    p_state->lightness[TYPE_PRESENT] = p_state->lightness[TYPE_TARGET];
                    sig_model_event_set_indicate(SIG_MODEL_INDICATE_GEN_LIGHTNESS);
                }
                else
                {
                    p_state->lightness[TYPE_PRESENT] += delta;
                    if (p_state->lightness[TYPE_PRESENT] == p_state->lightness[TYPE_TARGET])
                    {
                        sig_model_event_set_indicate(SIG_MODEL_INDICATE_GEN_LIGHTNESS);
                    }
                }
            }
            else
            {
                delta = (p_state->lightness[TYPE_PRESENT] - p_state->lightness[TYPE_TARGET]) / step;
                delta = delta > DELTA_ACTUAL_MIN ? delta : DELTA_ACTUAL_MIN;
                if (p_state->lightness[TYPE_PRESENT] < delta ||
                    p_state->lightness[TYPE_PRESENT] - delta < p_state->lightness[TYPE_TARGET])
                {
                    p_state->lightness[TYPE_PRESENT] = p_state->lightness[TYPE_TARGET];
                    sig_model_event_set_indicate(SIG_MODEL_INDICATE_GEN_LIGHTNESS);
                }
                else
                {
                    p_state->lightness[TYPE_PRESENT] -= delta;
                    if (p_state->lightness[TYPE_PRESENT] == p_state->lightness[TYPE_TARGET])
                    {
                        sig_model_event_set_indicate(SIG_MODEL_INDICATE_GEN_LIGHTNESS);
                    }
                }
            }
            cycle = 1;
        }
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        if (p_state->color_temperature[TYPE_PRESENT] != p_state->color_temperature[TYPE_TARGET])
        {
            if (p_state->color_temperature[TYPE_TARGET] > p_state->color_temperature[TYPE_PRESENT])
            {
                delta = (p_state->color_temperature[TYPE_TARGET] - p_state->color_temperature[TYPE_PRESENT]) / step;
                delta = delta > DELTA_TEMP_MIN ? delta : DELTA_TEMP_MIN;
                if (COLOR_TEMPERATURE_MAX - p_state->color_temperature[TYPE_PRESENT] < delta ||
                    p_state->color_temperature[TYPE_TARGET] - p_state->color_temperature[TYPE_PRESENT] < delta)
                {
                    p_state->color_temperature[TYPE_PRESENT] = p_state->color_temperature[TYPE_TARGET];
                    sig_model_event_set_indicate(SIG_MODEL_INDICATE_GEN_CTL);
                }
                else
                {
                    p_state->color_temperature[TYPE_PRESENT] += delta;
                    if (p_state->color_temperature[TYPE_PRESENT] == p_state->color_temperature[TYPE_TARGET])
                    {
                        sig_model_event_set_indicate(SIG_MODEL_INDICATE_GEN_CTL);
                    }
                }
                delta += p_state->color_temperature[TYPE_PRESENT];
                p_state->color_temperature[TYPE_PRESENT] = delta > p_state->color_temperature[TYPE_TARGET] ? p_state->color_temperature[TYPE_TARGET] : delta;
            }
            else
            {
                delta = (p_state->color_temperature[TYPE_PRESENT] - p_state->color_temperature[TYPE_TARGET]) / step;
                delta = delta > DELTA_TEMP_MIN ? delta : DELTA_TEMP_MIN;
                if (p_state->color_temperature[TYPE_PRESENT] < delta + COLOR_TEMPERATURE_MIN ||
                    p_state->color_temperature[TYPE_PRESENT] - delta < p_state->color_temperature[TYPE_TARGET])
                {
                    p_state->color_temperature[TYPE_PRESENT] = p_state->color_temperature[TYPE_TARGET];
                    sig_model_event_set_indicate(SIG_MODEL_INDICATE_GEN_CTL);
                }
                else
                {
                    p_state->color_temperature[TYPE_PRESENT] -= delta;
                    if (p_state->color_temperature[TYPE_PRESENT] == p_state->color_temperature[TYPE_TARGET])
                    {
                        sig_model_event_set_indicate(SIG_MODEL_INDICATE_GEN_CTL);
                    }
                }
            }
            cycle = 1;
        }
#endif
    }
#if 0
    printf("next: %d->%d|%02x->%02x|%02x->%02x\n", p_state->onoff[TYPE_PRESENT], p_state->onoff[TYPE_TARGET],
           p_state->lightness[TYPE_PRESENT], p_state->lightness[TYPE_TARGET], p_state->color_temperature[TYPE_PRESENT], p_state->color_temperature[TYPE_TARGET]);
#endif
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    if (p_state->onoff[TYPE_PRESENT] == p_state->onoff[TYPE_TARGET])
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        if (p_state->lightness[TYPE_PRESENT] == p_state->lightness[TYPE_TARGET])
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
            if (p_state->color_temperature[TYPE_PRESENT] == p_state->color_temperature[TYPE_TARGET])
#endif
                cycle = 0;

    //BT_DBG("cycle %d", cycle);
    if (cycle == 0)
    {
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        if (p_state->lightness[TYPE_TARGET] == 0)
        {
            p_state->onoff[TYPE_TARGET] = 0;
        }
#endif
        if (p_state->onoff[TYPE_PRESENT] != p_state->onoff[TYPE_TARGET])
        {
            p_state->onoff[TYPE_PRESENT] = p_state->onoff[TYPE_TARGET];
            sig_model_event_set_indicate(SIG_MODEL_INDICATE_GEN_ONOFF);
        }
        //BT_DBG("onoff %d->%d", p_state->onoff[TYPE_PRESENT], p_state->onoff[TYPE_TARGET]);
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        if (p_state->lightness[TYPE_PRESENT] != p_state->lightness[TYPE_TARGET])
        {
            p_state->lightness[TYPE_PRESENT] = p_state->lightness[TYPE_TARGET];
            sig_model_event_set_indicate(SIG_MODEL_INDICATE_GEN_LIGHTNESS);
        }
        //BT_DBG("lightness %02x->%02x", p_state->lightness[TYPE_PRESENT], p_state->lightness[TYPE_TARGET]);
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        if (p_state->color_temperature[TYPE_PRESENT] != p_state->color_temperature[TYPE_TARGET])
        {
            p_state->color_temperature[TYPE_PRESENT] = p_state->color_temperature[TYPE_TARGET];
            sig_model_event_set_indicate(SIG_MODEL_INDICATE_GEN_CTL);
        }
        //BT_DBG("color_temperature %02x->%02x", p_state->color_temperature[TYPE_PRESENT], p_state->color_temperature[TYPE_TARGET]);
#endif
        p_state->trans = 0;
    }

    return cycle;
}
#endif

#ifdef CONFIG_MESH_MODEL_TRANS
bt_u32_t TRANS_TIMES[] = {TRANSITION_TIME_UNIT_1, TRANSITION_TIME_UNIT_2, TRANSITION_TIME_UNIT_3, TRANSITION_TIME_UNIT_4};

static uint8_t _get_transition_byte(bt_u32_t time)
{
    if (time > TRANS_TIMES[3] * TRANSITION_TIME_VALUE_MAX)
    {
        return 0;
    }
    else if (time > TRANS_TIMES[2] * TRANSITION_TIME_VALUE_MAX)
    {
        return (time / TRANS_TIMES[3]) | TRANSITION_TIME_UNIT_BIT(3);
    }
    else if (time > TRANS_TIMES[1] * TRANSITION_TIME_VALUE_MAX)
    {
        return (time / TRANS_TIMES[2]) | TRANSITION_TIME_UNIT_BIT(2);
    }
    else if (time > TRANS_TIMES[0] * TRANSITION_TIME_VALUE_MAX)
    {
        return (time / TRANS_TIMES[1]) | TRANSITION_TIME_UNIT_BIT(1);
    }
    else
    {
        return (time / TRANS_TIMES[0]);
    }
}

//unit is 1ms
bt_u32_t sig_model_transition_get_transition_time(uint8_t byte)
{
    if ((byte & TRANSITION_TIME_VALUE_MASK) == TRANSITION_TIME_VALUE_INVALID)
    {
        MODEL_E("%s ERROR, invalid 0x%02X!!!\n", __func__, byte);
        return TRANSITION_TIME_VALUE_INVALID;
    }

    return (byte & TRANSITION_TIME_VALUE_MASK) * TRANS_TIMES[byte >> 6];
}

uint8_t sig_model_transition_get_remain_time_byte(sig_model_state_t *p_state, bool is_ack)
{
    uint8_t remain_time_byte = p_state->trans;
    int64_t cur_time = k_uptime_get();
    int64_t passed_time = 0;
    bt_u32_t trans_duration = 0;

    if (!is_ack && p_state->trans_start_time < cur_time)
    {
        passed_time -= p_state->trans_start_time;
        trans_duration = sig_model_transition_get_transition_time(p_state->trans);

        if (trans_duration > passed_time)
        {
            remain_time_byte = _get_transition_byte(trans_duration - passed_time);
        }
        else
        {
            remain_time_byte = TRANSITION_TIME_VALUE_MIN;
        }
    }

    return remain_time_byte;
}
#endif
