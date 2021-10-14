/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
//#include <aos/aos.h>
#include <aos/kernel.h>

#include <misc/printk.h>
//#include <hal/hal.h>
#include <ble_os.h>
#include <bluetooth/bluetooth.h>
#include <api/mesh.h>

#include "net.h"
//#include "transport.h"
#include "genie_service.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"

static struct bt_mesh_prov genie_mesh_provision;
static struct bt_mesh_comp genie_composition;
static aos_timer_t do_mesh_ready_timer;

uint8_t g_mesh_log_mode = 1;

struct bt_mesh_elem *genie_mesh_get_primary_element(void)
{
    return genie_composition.elem;
}

void genie_mesh_load_group_addr(void)
{
    if (genie_storage_read_sub(g_sub_list) == GENIE_STORAGE_SUCCESS)
    {
        BT_INFO("g_sub_list %s", bt_hex(g_sub_list, sizeof(g_sub_list)));
    }
}

static int report_poweron_callback(transport_result_e result)
{
#ifdef CONFIG_PM_SLEEP
    genie_lpm_start();
#endif
    return 0;
}

static int genie_mesh_report_poweron(void)
{
    uint8_t payload[3];
    genie_transport_payload_param_t transport_payload_param;

    payload[0] = ATTR_TYPE_DEVICE_EVENT & 0xFF;
    payload[1] = (ATTR_TYPE_DEVICE_EVENT >> 8) & 0xFF;
    payload[2] = EL_DEV_UP_T;

    memset(&transport_payload_param, 0, sizeof(genie_transport_payload_param_t));
    transport_payload_param.opid = VENDOR_OP_ATTR_INDICATE;
    transport_payload_param.p_payload = payload;
    transport_payload_param.payload_len = sizeof(payload);
    transport_payload_param.retry_cnt = GENIE_TRANSPORT_DEFAULT_RETRY_COUNT;
    transport_payload_param.result_cb = report_poweron_callback;

    return genie_transport_send_payload(&transport_payload_param);
}

int genie_mesh_init_pharse_ii(void)
{
#ifdef CONFIG_GENIE_OTA
    genie_ota_init();
#endif

#ifdef MESH_MODEL_VENDOR_TIMER
    genie_time_init();
#endif

    genie_mesh_report_poweron();

#if defined(CONFIG_BT_MESH_CTRL_RELAY) && !defined(CONFIG_PM_SLEEP)
    /* check ctrl relay */
    extern uint8_t genie_ctrl_relay_conf_set(mesh_ctrl_relay_para_t * p_para);
    mesh_ctrl_relay_para_t cr_para;
    genie_storage_status_e cr_ret = GENIE_STORAGE_SUCCESS;
    memset(&cr_para, 0, sizeof(mesh_ctrl_relay_para_t));
    cr_ret = genie_storage_read_ctrl_relay(&cr_para);
    if (cr_ret == GENIE_STORAGE_SUCCESS)
    {
        GENIE_LOG_INFO("proved, use save ctrl relay config");
        genie_ctrl_relay_conf_set(&cr_para);
    }
    else
    {
        GENIE_LOG_INFO("proved, use default ctrl relay config");
        genie_ctrl_relay_conf_set(NULL);
    }
#endif
    return 0;
}

static void do_mesh_ready_timer_cb(void *p_timer, void *args)
{
    char mesh_init_state = *(char *)args;

    GENIE_LOG_INFO("mesh init state:%d\n", mesh_init_state);
    if (mesh_init_state == GENIE_MESH_INIT_STATE_PROVISION)
    {
        if (genie_provision_get_state() != GENIE_PROVISION_SUCCESS)
        {
            GENIE_LOG_INFO("wait app key timeout");
            genie_event(GENIE_EVT_SDK_MESH_PROV_TIMEOUT, NULL);

            return;
        }
    }
    else if (mesh_init_state == GENIE_MESH_INIT_STATE_HW_RESET)
    {
        return genie_event(GENIE_EVT_HW_RESET_START, NULL);
    }

    genie_event(GENIE_EVT_MESH_READY, NULL);
}

void genie_mesh_ready_checktimer_restart(uint32_t timeout)
{
    aos_timer_stop(&do_mesh_ready_timer);
    aos_timer_change_once(&do_mesh_ready_timer, timeout);
    aos_timer_start(&do_mesh_ready_timer);
}

static void mesh_provision_complete(u16_t net_idx, u16_t addr)
{
    uint8_t rand;
    uint16_t random_time;
    static char mesh_init_state = 0; //1:now is provision 2:hw reset 3:normal boot

    //This time is in provision
    if (genie_provision_get_state() != GENIE_PROVISION_UNPROV)
    {
        GENIE_LOG_INFO("is in prov");
        mesh_init_state = GENIE_MESH_INIT_STATE_PROVISION;
        aos_timer_new(&do_mesh_ready_timer, do_mesh_ready_timer_cb, &mesh_init_state, GENIE_MESH_INIT_PHARSE_II_CHECK_APPKEY_TIMEOUT, 0);
    }
    else
    {
        if (genie_reset_get_hw_reset_flag())
        {
            GENIE_LOG_INFO("hw reset");
            mesh_init_state = GENIE_MESH_INIT_STATE_HW_RESET;
            aos_timer_new(&do_mesh_ready_timer, do_mesh_ready_timer_cb, &mesh_init_state, GENIE_MESH_INIT_PHARSE_II_HW_RESET_DELAY, 0);
        }
        else
        {
            bt_rand(&rand, 1);

            //Random range[GENIE_MESH_INIT_PHARSE_II_DELAY_START_MIN-GENIE_MESH_INIT_PHARSE_II_DELAY_START_MAX]
            random_time = GENIE_MESH_INIT_PHARSE_II_DELAY_START_MIN + (GENIE_MESH_INIT_PHARSE_II_DELAY_START_MAX - GENIE_MESH_INIT_PHARSE_II_DELAY_START_MIN) * rand / 255;

            GENIE_LOG_INFO("Rand delay:%dms", random_time);
            aos_timer_new(&do_mesh_ready_timer, do_mesh_ready_timer_cb, &mesh_init_state, random_time, 0);
        }
    }

#ifdef CONFIG_BT_MESH_SHELL
    extern void genie_prov_complete_notify(u16_t net_idx, u16_t addr);
    genie_prov_complete_notify(net_idx, addr);
#endif
}

static void mesh_provision_reset(void)
{
    BT_INFO("reset genie_mesh_provision");

#ifdef MESH_MODEL_VENDOR_TIMER
    genie_time_finalize();
#endif
}

static void bt_ready_cb(int err)
{
    if (err)
    {
        BT_ERR("BT init err %d", err);
        return;
    }
#if 0
    k_work_init(&genie_mesh_work, do_genie_mesh_work);
#endif

    BT_INFO(">>>Mesh initialized<<<");
    err = bt_mesh_init(&genie_mesh_provision, &genie_composition, NULL);
    if (err)
    {
        BT_ERR("mesh init err %d", err);
        return;
    }
#ifdef CONFIG_GENIE_MESH_PORT
    extern int genie_mesh_port_init(void);
    genie_mesh_port_init();
#endif
    //send event
    genie_event(GENIE_EVT_BT_READY, NULL);
}

int genie_mesh_init(struct bt_mesh_elem *p_mesh_elem, unsigned int mesh_elem_counts)
{
    int ret;
    uint8_t random[16] = {0};

    BT_INFO(">>>init genie<<<");

    genie_mesh_provision.uuid = genie_provision_get_uuid();

    genie_mesh_provision.static_val = genie_crypto_get_auth(random);
    genie_mesh_provision.static_val_len = STATIC_OOB_LENGTH;

    genie_mesh_provision.complete = mesh_provision_complete;
    genie_mesh_provision.reset = mesh_provision_reset;

    genie_composition.cid = CONFIG_MESH_VENDOR_COMPANY_ID;
    genie_composition.pid = 0;
    genie_composition.vid = 1; // firmware version for ota
    genie_composition.elem = p_mesh_elem;
    genie_composition.elem_count = mesh_elem_counts;
    genie_sal_ble_init();
    ret = bt_enable(NULL);
    //BT_INFO("the return of bt_enable is %d\n", ret);
    bt_ready_cb(ret);
    return ret;
}

int genie_mesh_suspend(bool force)
{
    BT_INFO("[%u]%s, force:%d", k_uptime_get_32(), __func__, force);
    return bt_mesh_suspend(force);
}

int genie_mesh_resume(void)
{
    BT_INFO("[%u]%s", k_uptime_get_32(), __func__);
    return bt_mesh_resume();
}
