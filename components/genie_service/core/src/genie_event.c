/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "genie_service.h"
#include "mesh_hal_ble.h"
#include "mesh/cfg_srv.h"
#include "mesh.h"
#include "prov.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_EVENT)
#include "common/log.h"

extern void genie_appkey_register(u16_t net_idx, u16_t app_idx, const u8_t val[16], bool update);

static genie_event_e _genie_event_handle_mesh_init(void)
{
#ifndef CONFIG_BT_SETTINGS
    genie_provision_t genie_provision;

    if (genie_provision_get_saved_data(&genie_provision) == 0)
    {
        bt_mesh_provision(genie_provision.netkey.key, genie_provision.netkey.net_index, genie_provision.netkey.flag, genie_provision.netkey.ivi, genie_provision.addr, genie_provision.devkey);
        genie_appkey_register(genie_provision.appkey.net_index, genie_provision.appkey.key_index, genie_provision.appkey.key, genie_provision.appkey.flag);

        /* check hb */
        mesh_hb_para_t hb_para = {.count = 0};
        if (genie_storage_read_hb(&hb_para) == GENIE_STORAGE_READ_FAIL)
        {
            extern u8_t genie_heartbeat_set(mesh_hb_para_t * p_para);
            genie_heartbeat_set(&hb_para);
        }

        return GENIE_EVT_NONE;
    }
    else
    {
        bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);

        return GENIE_EVT_SDK_MESH_PBADV_START; //prov start
    }
#else
    if (bt_mesh_is_provisioned())
    {
        genie_mesh_load_group_addr();
        genie_mesh_setup();
        return GENIE_EVT_NONE;
    }
    else
    {
        bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);

        return GENIE_EVT_SDK_MESH_PBADV_START; //prov start
    }
#endif
}

static genie_event_e _genie_event_handle_pbadv_start(void)
{
    genie_service_ctx_t *p_context = NULL;

#ifdef CONFIG_PM_SLEEP
    genie_lpm_disable();
#endif

    genie_provision_clear_silent_flag();
    extern void genie_set_silent_unprov_beacon_interval(bool is_silent);
    genie_set_silent_unprov_beacon_interval(false);

    p_context = genie_service_get_context();
    if (p_context)
    {
        genie_provision_pbadv_timer_start(p_context->prov_timeout);
    }

    return GENIE_EVT_NONE;
}

static genie_event_e _genie_event_handle_pbadv_timeout(void)
{
#ifdef CONFIG_PM_SLEEP
    genie_lpm_enable(true);
#endif

    genie_provision_pbadv_timer_stop();
    bt_mesh_prov_disable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);

    return GENIE_EVT_SDK_MESH_SILENT_START; //Enter silent provision mode
}

static genie_event_e _genie_event_handle_silent_start(void)
{
    genie_provision_start_slient_pbadv();

    return GENIE_EVT_NONE;
}

static genie_event_e _genie_event_handle_prov_start(void)
{
    if (genie_provision_get_state() == GENIE_PROVISION_UNPROV)
    {
        genie_provision_set_state(GENIE_PROVISION_START);
        /* disable adv timer */
        genie_provision_pbadv_timer_stop();
        /* enable prov timer */
        genie_provision_prov_timer_start();
    }

    return GENIE_EVT_NONE;
}

static genie_event_e _genie_event_handle_prov_data(uint16_t *p_addr)
{
#ifndef CONFIG_BT_SETTINGS
    genie_storage_write_addr(p_addr);
#endif
    return GENIE_EVT_NONE;
}

static genie_event_e _genie_event_handle_prov_timeout(void)
{
    return GENIE_EVT_SDK_MESH_PROV_FAIL;
}

static genie_event_e _genie_event_handle_prov_success(void)
{
    genie_provision_pbadv_timer_stop();

    return GENIE_EVT_NONE;
}

static genie_event_e _genie_event_handle_prov_fail(void)
{
    /* reset prov */
    genie_provision_set_state(GENIE_PROVISION_UNPROV);
    genie_reset_provision();
    /* restart adv */
    bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);

    return GENIE_EVT_SDK_MESH_PBADV_START;
}

static void _genie_event_save_mesh_data(uint8_t *p_status)
{
    if (*p_status == 0)
    {
#ifndef CONFIG_BT_SETTINGS
        genie_storage_write_para(&bt_mesh);
#endif
    }
}

static genie_event_e _genie_event_handle_appkey_add(uint8_t *p_status)
{
    if (genie_provision_get_state() == GENIE_PROVISION_START)
    {
        /* disable prov timer */
        genie_provision_prov_timer_stop();

        if (*p_status == 0)
        {
#ifndef CONFIG_BT_SETTINGS
            //genie_storage_write_para(&bt_mesh);
            uint8_t devkey[16];
            mesh_netkey_para_t netkey;
            mesh_appkey_para_t appkey;

            memcpy(devkey, bt_mesh.dev_key, 16);
            memset(&netkey, 0, sizeof(netkey));
            memcpy(netkey.key, bt_mesh.sub[0].keys[0].net, 16);
            memset(&appkey, 0, sizeof(appkey));
            memcpy(appkey.key, bt_mesh.app_keys[0].keys[0].val, 16);
            genie_storage_write_devkey(devkey);
            genie_storage_write_netkey(&netkey);
            genie_storage_write_appkey(&appkey);
#endif
            genie_provision_set_state(GENIE_PROVISION_SUCCESS);
            genie_mesh_ready_checktimer_restart(GENIE_PROV_SUCCESS_REPORT_TIMEOUT);
            return GENIE_EVT_SDK_MESH_PROV_SUCCESS;
        }
        else
        {
            return GENIE_EVT_SDK_MESH_PROV_FAIL;
        }
    }
    else
    {
        _genie_event_save_mesh_data(p_status);
        return GENIE_EVT_NONE;
    }
}

static genie_event_e _genie_event_handle_hb_set(mesh_hb_para_t *p_para)
{
#ifndef CONFIG_BT_SETTINGS
    BT_DBG("save");
    genie_storage_write_hb(p_para);
#endif
    return GENIE_EVT_NONE;
}

#ifdef CONFIG_BT_MESH_CTRL_RELAY
static genie_event_e _genie_event_handle_ctrl_relay_set(mesh_ctrl_relay_para_t *p_para)
{
    printf("save ctrl relay param\n");
    genie_storage_write_ctrl_relay(p_para);
    return GENIE_EVT_NONE;
}
#endif

static genie_event_e _genie_event_handle_seq_update(void)
{
#ifndef CONFIG_BT_SETTINGS
    uint32_t seq = bt_mesh.seq;

    genie_storage_write_seq(&seq, false);
#endif
    return GENIE_EVT_NONE;
}

static genie_event_e _genie_event_handle_ais_discon(void)
{
    if (0 == bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV))
    {
        return GENIE_EVT_SDK_MESH_PBADV_START;
    }
    else
    {
        return GENIE_EVT_NONE;
    }
}

static genie_event_e _genie_event_handle_vnd_msg(genie_transport_model_param_t *p_msg)
{
    BT_DBG("vendor message received");
    genie_model_handle_mesg(p_msg);

    return GENIE_EVT_NONE;
}

int genie_down_msg(genie_down_mesg_type msg_type, uint32_t opcode, void *p_msg)
{
    uint8_t *p_data = NULL;
    uint16_t data_len = 0;
    genie_service_ctx_t *p_context = NULL;

#ifdef CONIFG_GENIE_MESH_USER_CMD
    uint8_t element_id = 0;
    genie_down_msg_t down_msg;
#endif

    p_context = genie_service_get_context();

    if (p_msg == NULL || !p_context || !p_context->event_cb)
    {
        GENIE_LOG_ERR("param err");
        return -1;
    }

    if (GENIE_DOWN_MESG_VENDOR_TYPE == msg_type)
    {
        genie_transport_model_param_t *p_vnd_mesg = (genie_transport_model_param_t *)p_msg;
        data_len = 4 + p_vnd_mesg->len;
        p_data = (uint8_t *)aos_malloc(data_len);
        if (p_data == NULL)
        {
            return -1;
        }

        p_data[0] = p_vnd_mesg->opid;
        p_data[1] = opcode;
        p_data[2] = opcode >> 8;
        p_data[3] = p_vnd_mesg->tid;
        if (p_vnd_mesg->len > 0)
        {
            memcpy(&p_data[4], p_vnd_mesg->data, p_vnd_mesg->len);
        }

#ifdef CONIFG_GENIE_MESH_USER_CMD
        sig_model_element_state_t *p_elem_state = (sig_model_element_state_t *)p_vnd_mesg->p_model->user_data;
        if (p_elem_state)
        {
            element_id = p_elem_state->element_id;
        }
#endif
    }
    else
    {
        sig_model_msg *p_net_buf = (sig_model_msg *)p_msg;

        p_context->event_cb(GENIE_EVT_SIG_MODEL_MSG, (void *)p_msg);

        if (opcode < 0x7F) //one byte opcode
        {
            data_len = 1 + p_net_buf->len;
            p_data = (uint8_t *)aos_malloc(data_len);
            if (p_data == NULL)
            {
                return -1;
            }
            p_data[0] = opcode & 0xFF;
            memcpy(&p_data[1], p_net_buf->data, p_net_buf->len);
        }
        else
        {
            data_len = 2 + p_net_buf->len;
            p_data = (uint8_t *)aos_malloc(data_len);
            if (p_data == NULL)
            {
                return -1;
            }
            p_data[0] = (opcode >> 8) & 0xFF;
            p_data[1] = opcode & 0xFF;
            memcpy(&p_data[2], p_net_buf->data, p_net_buf->len);
        }
#ifdef CONIFG_GENIE_MESH_USER_CMD
        element_id = p_net_buf->element_id;
#endif
    }

#ifdef CONFIG_GENIE_MESH_AT_CMD
    genie_at_cmd_send_data_to_mcu(p_data, data_len);
#endif

#ifdef CONIFG_GENIE_MESH_BINARY_CMD
    genie_bin_cmds_send_data_to_mcu(p_data, data_len);
#endif

#ifdef CONIFG_GENIE_MESH_USER_CMD
    down_msg.len = data_len;
    down_msg.data = p_data;
    down_msg.element_id = element_id;

    p_context->event_cb(GENIE_EVT_DOWN_MSG, (void *)&down_msg);
#endif

    aos_free(p_data);

    return 0;
}

#ifdef MESH_MODEL_VENDOR_TIMER
static genie_event_e genie_model_notify_onoff_msg(vendor_attr_data_t *attr_data) //notify MCU
{
    genie_transport_model_param_t msg;
    uint8_t payload[3] = {0};

    BT_DBG("type:%04x data:%04x\r\n", attr_data->type, attr_data->para);

    if (attr_data->type == ATTR_TYPE_GENERIC_ONOFF)
    {
        payload[0] = 0x00;
        payload[1] = 0x01;
        payload[2] = attr_data->para;

        memset(&msg, 0, sizeof(genie_transport_model_param_t));
        msg.opid = VENDOR_OP_ATTR_SET_UNACK;
        msg.tid = genie_transport_gen_tid();
        msg.data = payload;
        msg.len = sizeof(payload);
        genie_down_msg(GENIE_DOWN_MESG_VENDOR_TYPE, CONFIG_MESH_VENDOR_COMPANY_ID, (void *)&msg);
    }

    return GENIE_EVT_NONE;
}
#endif

void genie_event(genie_event_e event, void *p_arg)
{
    genie_event_e next_event = event;
    uint8_t ignore_user_event = 0;
    genie_service_ctx_t *p_context = NULL;

    p_context = genie_service_get_context();
    if (!p_context || !p_context->event_cb)
    {
        return;
    }

    if ((event != GENIE_EVT_SDK_SEQ_UPDATE) && (event != GENIE_EVT_USER_TRANS_CYCLE))
    {
        GENIE_LOG_INFO("GenieE:%d\r\n", event);
    }

    switch (event)
    {
    case GENIE_EVT_SW_RESET:
    {
        //call user_event first
        p_context->event_cb(GENIE_EVT_SW_RESET, p_arg);
        ignore_user_event = 1;
        next_event = genie_reset_do_sw_reset();
    }
    break;
    case GENIE_EVT_HW_RESET_START:
    {
        if (p_arg == NULL)
        {
            next_event = genie_reset_do_hw_reset(false);
        }
        else
        {
            bool is_only_report = *(bool *)p_arg;

            next_event = genie_reset_do_hw_reset(is_only_report);
        }
    }
    break;
    case GENIE_EVT_BT_READY:
    {
        next_event = _genie_event_handle_mesh_init();
    }
    break;
    case GENIE_EVT_SDK_AIS_DISCON:
    {
        next_event = _genie_event_handle_ais_discon();
    }
    break;
    case GENIE_EVT_SDK_MESH_PBADV_START:
    {
        next_event = _genie_event_handle_pbadv_start();
    }
    break;
    case GENIE_EVT_SDK_MESH_PBADV_TIMEOUT:
    {
        next_event = _genie_event_handle_pbadv_timeout();
    }
    break;
    case GENIE_EVT_SDK_MESH_SILENT_START:
    {
        next_event = _genie_event_handle_silent_start();
    }
    break;
    case GENIE_EVT_SDK_MESH_PROV_START:
    {
        next_event = _genie_event_handle_prov_start();
    }
    break;
    case GENIE_EVT_SDK_MESH_PROV_DATA:
    {
        next_event = _genie_event_handle_prov_data((uint16_t *)p_arg);
    }
    break;
    case GENIE_EVT_SDK_MESH_PROV_TIMEOUT:
    {
        next_event = _genie_event_handle_prov_timeout();
    }
    break;
    case GENIE_EVT_SDK_MESH_PROV_FAIL:
    {
        next_event = _genie_event_handle_prov_fail();
    }
    break;
    case GENIE_EVT_SDK_MESH_PROV_SUCCESS:
    {
        next_event = _genie_event_handle_prov_success();
    }
    break;
    case GENIE_EVT_MESH_READY:
    {
        p_context->event_cb(GENIE_EVT_MESH_READY, p_arg); //Report user bootup data at first
        ignore_user_event = 1;
        genie_mesh_init_pharse_ii();
        next_event = GENIE_EVT_NONE;
    }
    break;
    case GENIE_EVT_SDK_APPKEY_ADD:
    {
        next_event = _genie_event_handle_appkey_add((uint8_t *)p_arg);
    }
    break;
    case GENIE_EVT_SDK_APPKEY_DEL:
    case GENIE_EVT_SDK_APPKEY_UPDATE:
    case GENIE_EVT_SDK_NETKEY_ADD:
    case GENIE_EVT_SDK_NETKEY_DEL:
    case GENIE_EVT_SDK_NETKEY_UPDATE:
    {
        //TODO:Support key update
        //_genie_event_save_mesh_data((uint8_t *)p_arg);
        next_event = GENIE_EVT_NONE;
    }
    break;
    case GENIE_EVT_SDK_HB_SET:
    {
        next_event = _genie_event_handle_hb_set((mesh_hb_para_t *)p_arg);
    }
    break;
#ifdef CONFIG_BT_MESH_CTRL_RELAY
    case GENIE_EVT_SDK_CTRL_RELAY_SET:
    {
        next_event = _genie_event_handle_ctrl_relay_set((mesh_ctrl_relay_para_t *)p_arg);
    }
    break;
#endif
    case GENIE_EVT_SDK_SEQ_UPDATE:
    {
        next_event = _genie_event_handle_seq_update();
    }
    break;
    case GENIE_EVT_VENDOR_MODEL_MSG:
    {
        next_event = _genie_event_handle_vnd_msg((genie_transport_model_param_t *)p_arg);
    }
    break;
#ifdef MESH_MODEL_VENDOR_TIMER
    case GENIE_EVT_TIMEOUT:
    {
        next_event = genie_model_notify_onoff_msg((vendor_attr_data_t *)p_arg);
    }
    break;
#endif
    default:
    {
        next_event = GENIE_EVT_NONE;
    }
    break;
    }

    if (!ignore_user_event)
    {
        p_context->event_cb(event, p_arg);
    }

#ifdef CONFIG_GENIE_MESH_AT_CMD
    genie_at_output_event(event, p_arg);
#endif

#ifdef CONIFG_GENIE_MESH_BINARY_CMD
    genie_bin_cmd_handle_event(event, p_arg);
#endif

    if (next_event != GENIE_EVT_NONE)
    {
        genie_event(next_event, p_arg);
    }
}
