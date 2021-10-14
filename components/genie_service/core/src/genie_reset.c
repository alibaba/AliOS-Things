/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
//#include <port/mesh_hal_ble.h>
#include "common/log.h"
#include "mesh/access.h"
#include "mesh/main.h"
#include "prov.h"

#include "genie_service.h"

static struct k_timer repeat_reset_timer;
static struct k_timer delay_reset_timer;
//1:need reset 0:no reset
static uint8_t hw_reset_flag = 0;

uint8_t genie_reset_get_hw_reset_flag(void)
{
    return hw_reset_flag;
}

static genie_storage_status_e _genie_reset_write_count(uint8_t count)
{
    uint8_t data = count;

    return genie_storage_write_userdata(GFI_MESH_RESET_CNT, &data, 1);
}

static void repeat_reset_timer_cb(void *p_timer, void *args)
{
    uint8_t number = 0;

    _genie_reset_write_count(number);
}

void genie_reset_by_repeat_init(void)
{
    uint8_t count = 0;
    genie_storage_status_e flash_err = GENIE_STORAGE_SUCCESS;

    flash_err = genie_storage_read_userdata(GFI_MESH_RESET_CNT, &count, 1);
    if (flash_err != GENIE_STORAGE_SUCCESS)
    {
        count = 0;
    }

    /* update number and check if exceeded */
    if (count > GENIE_RESET_BY_REPEAT_COUNTER)
    {
        count = 0;
    }

    if (count < GENIE_RESET_BY_REPEAT_COUNTER)
    {
        count++;
        _genie_reset_write_count(count);
    }
    else
    {
        hw_reset_flag = 1;
    }

    k_timer_init(&repeat_reset_timer, repeat_reset_timer_cb, NULL);
    k_timer_start(&repeat_reset_timer, GENIE_RESET_BY_REPEAT_TIMEOUT);
}

int genie_reset_provision(void)
{
    bt_mesh_reset();
    if (bt_prov_active())
    {
        extern void reset_link(void);
        reset_link();
    }

    bt_mesh_adv_stop();
#ifndef CONFIG_BT_SETTINGS
    return genie_storage_reset_system();
#else
    return 0;
#endif
}

static void delay_reset_timer_cb(void *p_timer, void *args)
{
    genie_provision_set_state(GENIE_PROVISION_UNPROV);
    genie_reset_provision();

#ifdef CONFIG_GENIE_SW_RESET_PROV
    bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
    genie_event(GENIE_EVT_SDK_MESH_PBADV_START, NULL);
    return;
#else
    aos_reboot();
#endif
}

genie_event_e genie_reset_do_sw_reset(void)
{
    k_timer_start(&delay_reset_timer, GENIE_RESET_WAIT_TIMEOUT); //wait report reset event done
    return GENIE_EVT_NONE;
}

static int reset_report_result_cb(transport_result_e result_e)
{
    if (result_e == SEND_RESULT_SUCCESS)
    {
        hw_reset_flag = 0;
        GENIE_LOG_INFO("report reset ok");
    }
    else
    {
        GENIE_LOG_INFO("report reset fail");
    }

    return 0;
}

static int genie_reset_report_device_reset(void)
{
    uint8_t payload[3];
    genie_transport_payload_param_t transport_payload_param;

    payload[0] = ATTR_TYPE_DEVICE_EVENT & 0xFF;
    payload[1] = (ATTR_TYPE_DEVICE_EVENT >> 8) & 0xFF;
    payload[2] = EL_HW_RESET_T;

    memset(&transport_payload_param, 0, sizeof(genie_transport_payload_param_t));
    transport_payload_param.opid = VENDOR_OP_ATTR_INDICATE;
    transport_payload_param.p_payload = payload;
    transport_payload_param.payload_len = sizeof(payload);
    transport_payload_param.retry_cnt = GENIE_TRANSPORT_DEFAULT_RETRY_COUNT;
    transport_payload_param.result_cb = reset_report_result_cb;

    return genie_transport_send_payload(&transport_payload_param);
}

genie_event_e genie_reset_do_hw_reset(bool is_only_report)
{      extern uint8_t is_proxy_connected();
    if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY) && !is_proxy_connected()) {
       extern int bt_mesh_proxy_gatt_disable();
       bt_mesh_proxy_gatt_disable();
    }
    genie_reset_report_device_reset();

    if (is_only_report)
    {
        return GENIE_EVT_NONE;
    }
    else
    {
        return GENIE_EVT_SW_RESET;
    }
}

int genie_reset_init(void)
{
#ifdef CONFIG_GENIE_RESET_BY_REPEAT
    genie_reset_by_repeat_init();
#endif
    k_timer_init(&delay_reset_timer, delay_reset_timer_cb, NULL);
    return 0;
}
