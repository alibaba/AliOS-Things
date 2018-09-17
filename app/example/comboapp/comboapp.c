/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <aos/aos.h>
#include <aos/yloop.h>
#include "netmgr.h"
#include "iot_export.h"

#ifdef AOS_ATCMD
#include <atparser.h>
#endif
#ifdef CSP_LINUXHOST
#include <signal.h>
#endif

#include <k_api.h>

#include <breeze_export.h>
#include "dev_info.h"

static char linkkit_started = 0;
static char awss_running    = 0;
static breeze_apinfo_t apinfo;
static bool is_ble_connected = false;

void linkkit_main(void *p);
void set_iotx_info();
void awss_success_notify();

static void apinfo_ready_handler(breeze_apinfo_t *ap)
{
    if (!ap)
        return;

    memcpy(&apinfo, ap, sizeof(apinfo));
    aos_post_event(EV_BZ_COMBO, CODE_COMBO_AP_INFO_READY, (unsigned long)&apinfo);
}

static void notify_wifi_status(void *arg)
{
    /* tlv response */
    uint8_t rsp[] = { 0x01, 0x01, 0x01 };

    /* tx_cmd is defaulted to ALI_CMD_STATUS so we don't worry here. */
    breeze_post(rsp, sizeof(rsp));
}

#ifdef CONFIG_PRINT_HEAP
void print_heap()
{
    extern k_mm_head *g_kmm_head;
    int               free = g_kmm_head->free_size;
    LOG("============free heap size =%d==========", free);
}
#endif

static void reboot_system(void *parms)
{
    LOG("reboot system");
    aos_reboot();
    while (1);
}

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    /* Notify app the AP connect status */
    if (is_ble_connected) {
        //aos_post_delayed_action(100, notify_wifi_status, NULL);
        notify_wifi_status(NULL);
    }

    if (awss_running) {
        awss_success_notify();
#ifdef AWSS_NEED_REBOOT
        aos_post_delayed_action(1000, reboot_system, NULL);
        return;
#endif
    }

    if (!linkkit_started) {
#ifdef CONFIG_PRINT_HEAP
        print_heap();
#endif
        aos_task_new("linkkit", linkkit_main, NULL, 1024 * 6);
        linkkit_started = 1;
    }
}

static void linkkit_work(breeze_apinfo_t *info);
static void combo_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_BZ_COMBO) {
        return;
    }

    LOG("%s %d", __func__, event->code);

    awss_running = 1;

    if (event->code == CODE_COMBO_AP_INFO_READY) {
        linkkit_work((breeze_apinfo_t *)event->value);
    }
}

/*
 * Note:
 * the linkkit_event_monitor must not block and should run to complete fast
 * if user wants to do complex operation with much time,
 * user should post one task to do this, not implement complex operation in
 * linkkit_event_monitor
 */

static void linkkit_event_monitor(int event)
{
    switch (event) {
        case IOTX_AWSS_START: // AWSS start without enbale, just supports device
                              // discover
            // operate led to indicate user
            LOG("IOTX_AWSS_START");
            break;
        case IOTX_AWSS_ENABLE: // AWSS enable
            LOG("IOTX_AWSS_ENABLE");
            // operate led to indicate user
            break;
        case IOTX_AWSS_LOCK_CHAN: // AWSS lock channel(Got AWSS sync packet)
            LOG("IOTX_AWSS_LOCK_CHAN");
            // operate led to indicate user
            break;
        case IOTX_AWSS_PASSWD_ERR: // AWSS decrypt passwd error
            LOG("IOTX_AWSS_PASSWD_ERR");
            // operate led to indicate user
            break;
        case IOTX_AWSS_GOT_SSID_PASSWD:
            LOG("IOTX_AWSS_GOT_SSID_PASSWD");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ADHA: // AWSS try to connnect adha (device
                                     // discover, router solution)
            LOG("IOTX_AWSS_CONNECT_ADHA");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ADHA_FAIL: // AWSS fails to connect adha
            LOG("IOTX_AWSS_CONNECT_ADHA_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_AHA: // AWSS try to connect aha (AP solution)
            LOG("IOTX_AWSS_CONNECT_AHA");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_AHA_FAIL: // AWSS fails to connect aha
            LOG("IOTX_AWSS_CONNECT_AHA_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_SETUP_NOTIFY: // AWSS sends out device setup information
                                     // (AP and router solution)
            LOG("IOTX_AWSS_SETUP_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ROUTER: // AWSS try to connect destination router
            LOG("IOTX_AWSS_CONNECT_ROUTER");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ROUTER_FAIL: // AWSS fails to connect destination
                                            // router.
            LOG("IOTX_AWSS_CONNECT_ROUTER_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_GOT_IP: // AWSS connects destination successfully and got
                               // ip address
            LOG("IOTX_AWSS_GOT_IP");
            // operate led to indicate user
            break;
        case IOTX_AWSS_SUC_NOTIFY: // AWSS sends out success notify (AWSS
                                   // sucess)
            LOG("IOTX_AWSS_SUC_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_AWSS_BIND_NOTIFY: // AWSS sends out bind notify information to
                                    // support bind between user and device
            LOG("IOTX_AWSS_BIND_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD: // Device try to connect cloud
            LOG("IOTX_CONN_CLOUD");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD_FAIL: // Device fails to connect cloud, refer to
                                   // net_sockets.h for error code
            LOG("IOTX_CONN_CLOUD_FAIL");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD_SUC: // Device connects cloud successfully
            LOG("IOTX_CONN_CLOUD_SUC");
            //do_report();
            // operate led to indicate user
            break;
        case IOTX_RESET: // Linkkit reset success (just got reset response from
                         // cloud without any other operation)
            LOG("IOTX_RESET");
            // operate led to indicate user
            break;
        default:
            break;
    }
}

static void linkkit_reset(void *p)
{
    netmgr_clear_ap_config();
    HAL_Sys_reboot();
}

extern int  awss_report_reset();
static void do_awss_reset()
{
    aos_task_new("reset", (void (*)(void *))awss_report_reset, NULL, 2560);
    aos_post_delayed_action(2000, linkkit_reset, NULL);
}

void linkkit_key_process(input_event_t *eventinfo, void *priv_data)
{
    if (eventinfo->type != EV_KEY) {
        return;
    }
    LOG("awss config press %d\n", eventinfo->value);

    if (eventinfo->code == CODE_BOOT) {
        if (eventinfo->value == VALUE_KEY_CLICK) {
            //Do something here.
        } else if (eventinfo->value == VALUE_KEY_LTCLICK) {
            do_awss_reset();
        }
    }
}

#ifdef CONFIG_AOS_CLI
static void handle_reset_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(do_awss_reset, NULL);
}

static struct cli_command resetcmd = { .name     = "reset",
                                       .help     = "factory reset",
                                       .function = handle_reset_cmd };
#endif

#ifdef CONFIG_PRINT_HEAP
static void duration_work(void *p)
{
    print_heap();
    aos_post_delayed_action(5000, duration_work, NULL);
}
#endif

static void linkkit_init()
{
    netmgr_init();
    set_iotx_info();
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    aos_register_event_filter(EV_BZ_COMBO, combo_service_event, NULL);
    aos_register_event_filter(EV_KEY, linkkit_key_process, NULL);
    aos_cli_register_command(&resetcmd);
}

static void linkkit_work(breeze_apinfo_t *info)
{
    netmgr_ap_config_t config;

    if (!info)
        return;

    printf("%s %d, ssid: %s, pw: %s\r\n", __func__, __LINE__, info->ssid,
           info->pw);

    strncpy(config.ssid, info->ssid, sizeof(config.ssid) - 1);
    strncpy(config.pwd, info->pw, sizeof(config.pwd) - 1);
    memcpy(config.bssid, info->bssid, ETH_ALEN);
    netmgr_set_ap_config(&config);
    hal_wifi_suspend_station(NULL);
    LOGI("aos_awss", "Will reconnect wifi: %s %s", config.ssid, config.pwd);
    netmgr_reconnect_wifi();
}

static int combo_init()
{
    linkkit_init();

    iotx_event_regist_cb(linkkit_event_monitor);

    /* Do not start BLE if wifi AP available */
    if (netmgr_start(false) == 1) {
        return 1;
    }

    return 0;
}

int application_start(int argc, char **argv)
{
    breeze_dev_info_t dinfo = {
        .product_id = PRODUCT_ID,
        .product_key = PRODUCT_KEY,
        .product_secret = PRODUCT_SECRET,
        .device_name = DEVICE_NAME,
        .device_secret = DEVICE_SECRET
    };

    if (combo_init() == 0) {
        breeze_awss_start(apinfo_ready_handler, &dinfo);
    }

    breeze_event_dispatcher();

    return 0;
}
