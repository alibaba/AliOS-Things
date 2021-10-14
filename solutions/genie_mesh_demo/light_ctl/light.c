/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "common/log.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
// #include <aos/aos.h>
#include <aos/kernel.h>

// #include <misc/printk.h>
// #include <misc/byteorder.h>
// #include <aos/hal/gpio.h>
// #include <aos/hal/pwm.h>

// #include <bluetooth.h>
// #include <soc.h>

#include <api/mesh.h>
#include "genie_service.h"
#include "light.h"
#include "light_driver.h"

static light_param_t light_param;
static sig_model_element_state_t light_elem_stat[ELEMENT_NUM];
static sig_model_powerup_t light_powerup[ELEMENT_NUM];

#ifdef CONFIG_BT_MESH_CFG_CLI
static struct bt_mesh_cfg_cli cfg_cli = {};
#endif

static struct bt_mesh_model primary_element[] = {
    MESH_MODEL_CFG_SRV_NULL(),
    MESH_MODEL_HEALTH_SRV_NULL(),
#ifdef CONFIG_BT_MESH_CFG_CLI
    BT_MESH_MODEL_CFG_CLI(&cfg_cli),
#endif
    MESH_MODEL_GEN_ONOFF_SRV(&light_elem_stat[0]),
    MESH_MODEL_LIGHTNESS_SRV(&light_elem_stat[0]),
    MESH_MODEL_CTL_SRV(&light_elem_stat[0]),
    MESH_MODEL_SCENE_SRV(&light_elem_stat[0]),
};

static struct bt_mesh_model primary_vendor_element[] = {
    MESH_MODEL_VENDOR_SRV(&light_elem_stat[0]),
};

struct bt_mesh_elem light_elements[] = {
    BT_MESH_ELEM(0, primary_element, primary_vendor_element, GENIE_ADDR_LIGHT),
};

#ifdef CONFIG_GENIE_OTA
bool genie_sal_ota_is_allow_reboot(void)
{
    // the device will reboot when it is off
    if (light_elem_stat[0].state.onoff[TYPE_PRESENT] == 0) {
        // save light para, always off
        light_powerup[0].last_onoff = 0;
        genie_storage_write_userdata(GFI_MESH_POWERUP, (uint8_t *)light_powerup, sizeof(light_powerup));
        LIGHT_DBG("Allow to reboot!");

        return true;
    }

    LIGHT_DBG("light is no so no reboot!");

    return false;
}
#endif

static void _mesh_delay_timer_cb(void *p_timer, void *p_arg)
{
    sig_model_element_state_t *p_elem = (sig_model_element_state_t *)p_arg;

#ifdef CONFIG_MESH_MODEL_TRANS
    sig_model_transition_timer_stop(p_elem);
#endif
    sig_model_event(SIG_MODEL_EVT_DELAY_END, p_arg);
}

#ifdef CONFIG_MESH_MODEL_TRANS
static void _mesh_trans_timer_cycle(void *p_timer, void *p_arg)
{
    sig_model_element_state_t *p_elem = (sig_model_element_state_t *)p_arg;
    sig_model_state_t *p_state = &p_elem->state;

    sig_model_transition_timer_stop(p_elem);

    // do cycle
    sig_model_event(SIG_MODEL_EVT_TRANS_CYCLE, p_arg);
    // BT_DBG(">>>>>%d %d", (u32_t)cur_time, (u32_t)p_elem->state.trans_end_time);

    if (p_state->trans == 0) {
        sig_model_event(SIG_MODEL_EVT_TRANS_END, p_arg);
    } else {
        k_timer_start(&p_state->trans_timer, SIG_MODEL_TRANSITION_INTERVAL);
    }
}
#endif

void light_elem_state_init(void)
{
    uint8_t index = 0;
    genie_storage_status_e ret;

    memset(light_elem_stat, 0, sizeof(light_elem_stat));

    // load light param
    ret = genie_storage_read_userdata(GFI_MESH_POWERUP, (uint8_t *)light_powerup, sizeof(light_powerup));
    for (index = 0; index < ELEMENT_NUM; index++) {
        light_elem_stat[index].element_id = index;

        if (ret == GENIE_STORAGE_SUCCESS) {
            // Use saved data
            memcpy(&light_elem_stat[index].powerup, &light_powerup[index], sizeof(sig_model_powerup_t));
        } else {
            // Use default value
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
            light_elem_stat[index].powerup.last_onoff = GEN_ONOFF_DEFAULT;
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
            light_elem_stat[index].powerup.last_lightness = LIGHTNESS_DEFAULT;
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
            light_elem_stat[index].powerup.last_color_temperature = COLOR_TEMPERATURE_DEFAULT;
#endif
#ifdef CONFIG_MESH_MODEL_SCENE_SRV
            light_elem_stat[index].powerup.last_scene = SCENE_DEFAULT;
#endif
        }

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        light_elem_stat[index].state.onoff[TYPE_PRESENT] = 0;
        light_elem_stat[index].state.onoff[TYPE_TARGET] = light_elem_stat[index].powerup.last_onoff;
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        light_elem_stat[index].state.lightness[TYPE_PRESENT] = 0;
        light_elem_stat[index].state.lightness[TYPE_TARGET] = light_elem_stat[index].powerup.last_lightness;
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        light_elem_stat[index].state.color_temperature[TYPE_PRESENT] = COLOR_TEMPERATURE_MIN;
        light_elem_stat[index].state.color_temperature[TYPE_TARGET] =
            light_elem_stat[index].powerup.last_color_temperature;
#endif
#ifdef CONFIG_MESH_MODEL_SCENE_SRV
        light_elem_stat[index].state.scene[TYPE_PRESENT] = GENIE_SCENE_NORMAL;
        light_elem_stat[index].state.scene[TYPE_TARGET] = light_elem_stat[index].powerup.last_scene;
#endif

#ifdef CONFIG_MESH_MODEL_TRANS
        k_timer_init(&light_elem_stat[index].state.delay_timer, _mesh_delay_timer_cb, &light_elem_stat[index]);
        k_timer_init(&light_elem_stat[index].state.trans_timer, _mesh_trans_timer_cycle, &light_elem_stat[index]);

        light_elem_stat[index].state.trans = TRANSITION_DEFAULT_VALUE;
        light_elem_stat[index].state.delay = DELAY_DEFAULT_VAULE;
        if (light_elem_stat[index].state.trans) {
            light_elem_stat[index].state.trans_start_time =
                k_uptime_get() + light_elem_stat[index].state.delay * DELAY_TIME_UNIT;
            light_elem_stat[index].state.trans_end_time =
                light_elem_stat[index].state.trans_start_time +
                sig_model_transition_get_transition_time(light_elem_stat[index].state.trans);
        }

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        if (light_elem_stat[index].state.onoff[TYPE_TARGET] == 1) {
            // light on
            sig_model_event(SIG_MODEL_EVT_DELAY_START, &light_elem_stat[index]);
        }
#endif
#else // No CONFIG_MESH_MODEL_TRANS
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        light_elem_stat[index].state.onoff[TYPE_PRESENT] = light_elem_stat[index].state.onoff[TYPE_TARGET];
        if (light_elem_stat[index].state.onoff[TYPE_TARGET] == 1) {
            // light on
            // light_driver_update(1, light_elem_stat[index].state.lightness[TYPE_TARGET], light_elem_stat[index].state.color_temperature[TYPE_TARGET]);
        }
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        light_elem_stat[index].state.lightness[TYPE_PRESENT] = light_elem_stat[index].state.lightness[TYPE_TARGET];
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        light_elem_stat[index].state.color_temperature[TYPE_PRESENT] =
            light_elem_stat[index].state.color_temperature[TYPE_TARGET];
#endif
#ifdef CONFIG_MESH_MODEL_SCENE_SRV
        light_elem_stat[index].state.scene[TYPE_PRESENT] = light_elem_stat[index].state.scene[TYPE_TARGET];
#endif
#endif
    }
}

#ifdef MESH_MODEL_VENDOR_TIMER
static void light_ctl_handle_order_msg(vendor_attr_data_t *attr_data)
{
    GENIE_LOG_INFO("type:%04x data:%04x\r\n", attr_data->type, attr_data->para);

    if (attr_data->type == ATTR_TYPE_GENERIC_ONOFF) {
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        light_elem_stat[0].state.onoff[TYPE_TARGET] = attr_data->para;

        if (light_elem_stat[0].state.onoff[TYPE_PRESENT] != light_elem_stat[0].state.onoff[TYPE_TARGET]) {
            sig_model_event(SIG_MODEL_EVT_DELAY_START, &light_elem_stat[0]);
        }
#endif
    }
}
#endif

static void light_param_reset(void)
{
    genie_storage_delete_userdata(GFI_MESH_POWERUP);
}

static void light_save_state(sig_model_element_state_t *p_elem)
{
    uint8_t *p_read = NULL;

    if (p_elem->state.lightness[TYPE_PRESENT] != 0) {
        p_elem->powerup.last_lightness = p_elem->state.lightness[TYPE_PRESENT];
        light_powerup[p_elem->element_id].last_lightness = p_elem->state.lightness[TYPE_PRESENT];
    }

    p_elem->powerup.last_color_temperature = p_elem->state.color_temperature[TYPE_PRESENT];
    light_powerup[p_elem->element_id].last_color_temperature = p_elem->state.color_temperature[TYPE_PRESENT];
    // always on
    p_elem->powerup.last_onoff = p_elem->state.onoff[TYPE_PRESENT];
    light_powerup[p_elem->element_id].last_onoff = p_elem->state.onoff[TYPE_PRESENT];

    p_read = aos_malloc(sizeof(light_powerup));
    if (!p_read) {
        GENIE_LOG_WARN("no mem");
        return;
    }

    genie_storage_read_userdata(GFI_MESH_POWERUP, p_read, sizeof(light_powerup));

    if (memcmp(light_powerup, p_read, sizeof(light_powerup))) {
        LIGHT_DBG("save %d %d %d", light_powerup[0].last_onoff, light_powerup[0].last_lightness,
                  light_powerup[0].last_color_temperature);
        genie_storage_write_userdata(GFI_MESH_POWERUP, (uint8_t *)light_powerup, sizeof(light_powerup));
    }

    aos_free(p_read);
#ifdef CONFIG_GENIE_OTA
    if (light_powerup[0].last_onoff == 0 && genie_ota_is_ready() == 1) {
        // Means have ota, wait for reboot while light off
        GENIE_LOG_INFO("reboot by ota");
        aos_reboot();
    }
#endif
}

static void light_update(sig_model_element_state_t *p_elem)
{
    static uint8_t last_onoff = 0;
    static uint16_t last_lightness = 0;
    static uint16_t last_temperature = 0;

    uint8_t onoff = p_elem->state.onoff[TYPE_PRESENT];
    uint16_t lightness = p_elem->state.lightness[TYPE_PRESENT];
    uint16_t temperature = p_elem->state.color_temperature[TYPE_PRESENT];

    if (last_onoff != onoff || last_lightness != lightness || last_temperature != temperature) {
        last_onoff = onoff;
        last_lightness = lightness;
        last_temperature = temperature;
        // LIGHT_DBG("%d,%d,%d", onoff, lightness, temperature);
        // light_driver_update(onoff, lightness, temperature);
    }
}

void _cal_flash_next_step(uint32_t delta_time)
{
    uint16_t lightness_end;

    if (delta_time < 1000) {
        lightness_end = light_param.target_lightness;
        light_param.present_color_temperature = light_param.target_color_temperature;
    } else {
        lightness_end = light_param.lightness_start;
        delta_time %= 1000;
    }

    if (delta_time > LED_BLINK_ON_TIME) {
        delta_time -= LED_BLINK_ON_TIME;
        light_param.present_lightness = light_param.lightness_start * delta_time / LED_BLINK_OFF_TIME;
    } else {
        light_param.present_lightness = lightness_end * (LED_BLINK_ON_TIME - delta_time) / LED_BLINK_ON_TIME;
    }
    // LIGHT_DBG("delta %d, lightness %04x", delta_time, light_param.present_lightness);
}

static void _led_blink_timer_cb(void *p_timer, void *p_arg)
{
    uint32_t cur_time = k_uptime_get();

    if (cur_time >= light_param.color_temperature_end) {
        // light_driver_update(1, light_param.target_lightness, light_param.target_color_temperature);
    } else {
        _cal_flash_next_step(light_param.color_temperature_end - cur_time);
        // light_driver_update(1, light_param.present_lightness, light_param.present_color_temperature);
        k_timer_start(&light_param.led_blink_timer, SIG_MODEL_TRANSITION_INTERVAL);
    }
}

static void light_led_blink(uint8_t times, uint8_t reset)
{
    if (light_elem_stat[0].state.onoff[TYPE_PRESENT] == 1) {
        if (light_elem_stat[0].state.lightness[TYPE_PRESENT]) {
            light_param.lightness_start = light_param.present_lightness =
                light_elem_stat[0].state.lightness[TYPE_PRESENT];
        } else {
            light_param.lightness_start = light_param.present_lightness = LIGHTNESS_DEFAULT;
        }

        if (light_elem_stat[0].state.color_temperature[TYPE_PRESENT]) {
            light_param.present_color_temperature = light_elem_stat[0].state.color_temperature[TYPE_PRESENT];
        } else {
            light_param.present_color_temperature = COLOR_TEMPERATURE_DEFAULT;
        }

        if (reset) {
            light_param.target_lightness = LIGHTNESS_DEFAULT;
            light_param.target_color_temperature = COLOR_TEMPERATURE_DEFAULT;
        } else {
            light_param.target_lightness = light_param.present_lightness;
            light_param.target_color_temperature = light_param.present_color_temperature;
        }

        light_param.color_temperature_end = k_uptime_get() + times * LED_BLINK_PERIOD;
    } else {
        if (light_elem_stat[0].powerup.last_lightness && !reset) {
            light_param.lightness_start = light_param.target_lightness = light_elem_stat[0].powerup.last_lightness;
        } else {
            light_param.lightness_start = light_param.target_lightness = LIGHTNESS_DEFAULT;
        }

        light_param.present_lightness = 0;
        if (light_elem_stat[0].powerup.last_color_temperature) {
            light_param.present_color_temperature = light_elem_stat[0].powerup.last_color_temperature;
        } else {
            light_param.present_color_temperature = COLOR_TEMPERATURE_DEFAULT;
        }

        if (reset) {
            light_param.target_color_temperature = COLOR_TEMPERATURE_DEFAULT;
        } else {
            light_param.target_color_temperature = light_param.present_color_temperature;
        }

        light_param.color_temperature_end = k_uptime_get() + times * LED_BLINK_PERIOD - LED_BLINK_OFF_TIME;
    }
    // LIGHT_DBG("%d (%d-%d) tar %04x", times, k_uptime_get(), light_param.color_temperature_end, light_param.target_lightness);

    k_timer_start(&light_param.led_blink_timer, SIG_MODEL_TRANSITION_INTERVAL);
}

static void light_report_poweron_state(sig_model_element_state_t *p_elem)
{
    uint16_t index = 0;
    uint8_t payload[20];
    genie_transport_payload_param_t transport_payload_param;

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    payload[index++] = ATTR_TYPE_GENERIC_ONOFF & 0xff;
    payload[index++] = (ATTR_TYPE_GENERIC_ONOFF >> 8) & 0xff;
    payload[index++] = p_elem->state.onoff[TYPE_TARGET];
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    payload[index++] = ATTR_TYPE_LIGHTNESS & 0xff;
    payload[index++] = (ATTR_TYPE_LIGHTNESS >> 8) & 0xff;
    payload[index++] = p_elem->state.lightness[TYPE_TARGET] & 0xff;
    payload[index++] = (p_elem->state.lightness[TYPE_TARGET] >> 8) & 0xff;
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
    payload[index++] = ATTR_TYPE_COLOR_TEMPERATURE & 0xff;
    payload[index++] = (ATTR_TYPE_COLOR_TEMPERATURE >> 8) & 0xff;
    payload[index++] = p_elem->state.color_temperature[TYPE_TARGET] & 0xff;
    payload[index++] = (p_elem->state.color_temperature[TYPE_TARGET] >> 8) & 0xff;
#endif
#ifdef CONFIG_MESH_MODEL_SCENE_SRV
    payload[index++] = ATTR_TYPE_SENCE & 0xff;
    payload[index++] = (ATTR_TYPE_SENCE >> 8) & 0xff;
    payload[index++] = p_elem->state.scene[TYPE_TARGET] & 0xff;
    payload[index++] = (p_elem->state.scene[TYPE_TARGET] >> 8) & 0xff;
#endif

    memset(&transport_payload_param, 0, sizeof(genie_transport_payload_param_t));
    transport_payload_param.opid = VENDOR_OP_ATTR_INDICATE;
    transport_payload_param.p_payload = payload;
    transport_payload_param.payload_len = index;
    transport_payload_param.retry_cnt = GENIE_TRANSPORT_DEFAULT_RETRY_COUNT;

    genie_transport_send_payload(&transport_payload_param);
}

static void light_ctl_event_handler(genie_event_e event, void *p_arg)
{
    switch (event) {
    case GENIE_EVT_SW_RESET: {
        light_param_reset();
        // light_led_blink(3, 1);
    } break;
    case GENIE_EVT_MESH_READY: {
        // User can report data to cloud at here
        GENIE_LOG_INFO("User report data\n");
        light_report_poweron_state(&light_elem_stat[0]);
    } break;
    case GENIE_EVT_SDK_MESH_PROV_SUCCESS: {
        GENIE_LOG_INFO("Mesh Prov success\n");
        light_blink();
        // light_led_blink(3, 0);
    } break;
#ifdef CONFIG_MESH_MODEL_TRANS
    case GENIE_EVT_USER_TRANS_CYCLE:
#endif
    case GENIE_EVT_USER_ACTION_DONE: {
        sig_model_element_state_t *p_elem = (sig_model_element_state_t *)p_arg;
        light_update(p_elem);
        if (event == GENIE_EVT_USER_ACTION_DONE) {
            light_save_state(p_elem);
        }
    } break;
    case GENIE_EVT_SIG_MODEL_MSG: {
        sig_model_msg *p_msg = (sig_model_msg *)p_arg;

        if (p_msg) {
            GENIE_LOG_INFO("SIG mesg ElemID(%d) in %s\n", p_msg->element_id, __func__);
            if (p_msg->opcode == 0x8202) {
                if (p_msg->data[0] == 0x00) {
                    light_set(0);
                    GENIE_LOG_INFO("rec SIG mesg: turn off cmd in ElemID(%d)\n", p_msg->element_id);
                } else if (p_msg->data[0] == 0x01) {
                    light_set(1);
                    GENIE_LOG_INFO("rec SIG mesg: turn on cmd in ElemID(%d)\n", p_msg->element_id);
                }
            }
        }
    } break;
    case GENIE_EVT_VENDOR_MODEL_MSG: {
        genie_transport_model_param_t *p_msg = (genie_transport_model_param_t *)p_arg;

        if (p_msg && p_msg->p_model && p_msg->p_model->user_data) {
            sig_model_element_state_t *p_elem_state = (sig_model_element_state_t *)p_msg->p_model->user_data;
            GENIE_LOG_INFO("ElemID(%d) TID(%02X)\n", p_elem_state->element_id, p_msg->tid);
            (void)p_elem_state;
        }
    } break;
#ifdef CONIFG_GENIE_MESH_USER_CMD
    case GENIE_EVT_DOWN_MSG: {
        genie_down_msg_t *p_msg = (genie_down_msg_t *)p_arg;
        // User handle this msg,such as send to MCU
        // if (p_msg) {
        // }
    } break;
#endif
#ifdef MESH_MODEL_VENDOR_TIMER
    case GENIE_EVT_TIMEOUT: {
        vendor_attr_data_t *pdata = (vendor_attr_data_t *)p_arg;
        // User handle vendor timeout event at here
        if (pdata) {
            light_ctl_handle_order_msg(pdata);
        }
    } break;
#endif
    default:
        break;
    }
}

#ifdef CONFIG_PM_SLEEP
static void light_ctl_lpm_cb(genie_lpm_wakeup_reason_e reason, genie_lpm_status_e status, void *args)
{
    if (status == STATUS_WAKEUP) {
        GENIE_LOG_INFO("wakeup by %s", (reason == WAKEUP_BY_IO) ? "io" : "timer");
    } else {
        GENIE_LOG_INFO("sleep");
    }
}
#endif

static void light_init(void)
{
    light_driver_init();
    light_set(0);
    light_elem_state_init();
    k_timer_init(&light_param.led_blink_timer, _led_blink_timer_cb, NULL);
}

int application_start(int argc, char **argv)
{
    genie_service_ctx_t context;

    aos_set_log_level(LOG_INFO);

    GENIE_LOG_INFO("BTIME:%s\n", __DATE__ "," __TIME__);
    printf("enter %s:%d\n", __func__, __LINE__);

    light_init();

    memset(&context, 0, sizeof(genie_service_ctx_t));
    context.prov_timeout = MESH_PBADV_TIME;
    context.event_cb = light_ctl_event_handler;
    context.p_mesh_elem = light_elements;
    context.mesh_elem_counts = sizeof(light_elements) / sizeof(struct bt_mesh_elem);

#ifdef CONFIG_PM_SLEEP
    context.lpm_conf.is_auto_enable = 1;
    context.lpm_conf.lpm_wakeup_io = 14;

    context.lpm_conf.genie_lpm_cb = light_ctl_lpm_cb;

    // User can config sleep time and wakeup time when not config GLP
    context.lpm_conf.sleep_ms = 1000;
    context.lpm_conf.wakeup_ms = 30;
#endif

    genie_service_init(&context);

#ifdef CONFIG_BT_MESH_SHELL
    extern void cli_reg_cmd_blemesh(void);
    cli_reg_cmd_blemesh();
#endif

    // aos_loop_run();

    return 0;
}
