/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "aos/cli.h"
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/yloop.h"

#include "netmgr.h"
#include "linkkit/wifi_provision_api.h"
#include "linkkit/infra/infra_compat.h"
#include "linkkit/infra/infra_defs.h"
#include "linkkit/wrappers/wrappers.h"

#ifdef CSP_LINUXHOST
    #include <signal.h>
#endif

#ifdef WITH_SAL
#include <atcmd_config_module.h>
#endif

#include <k_api.h>

#ifdef LINKKIT_GATEWAY_TEST_CMD
    #include "simulate_subdev/testcmd.h"
#endif

#if defined(ENABLE_AOS_OTA)
#include "ota/ota_agent.h"
static ota_service_t ctx = {0};
#endif

static char linkkit_started = 0;
static char awss_running    = 0;

void linkkit_main(void *p);
void set_iotx_info();
void do_awss_active();

#ifdef CONFIG_PRINT_HEAP
void print_heap()
{
    extern k_mm_head *g_kmm_head;
    int    free = g_kmm_head->free_size;
    LOG("============free heap size =%d==========", free);
}
#endif

#if defined(ENABLE_AOS_OTA)
static int ota_upgrade_cb(ota_service_t* pctx, char *ver, char *url)
{
    int ret = -1;
    LOG("ota version:%s is coming, if OTA upgrade or not ?\n", ver);
    void *thread = NULL;
    if(pctx != NULL) {
        ret = ota_thread_create(&thread, (void *)ota_service_start, (void *)pctx, NULL, 1024 * 6);
    }
    if (ret < 0) {
        LOG("ota thread err;%d ", ret);
    }
    return ret;
}

static ota_service_t *ota_get_ctx(void)
{
    char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
    char device_secret[IOTX_DEVICE_SECRET_LEN + 1] = {0};
    char product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = {0};
    HAL_GetProductSecret(product_secret);
    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);
    HAL_GetDeviceSecret(device_secret);
    strncpy(ctx.pk, product_key, sizeof(ctx.pk) - 1);
    strncpy(ctx.dn, device_name, sizeof(ctx.dn) - 1);
    strncpy(ctx.ds, device_secret, sizeof(ctx.ds) - 1);
    strncpy(ctx.ps, device_secret, sizeof(ctx.ps) - 1);
    return &ctx;
}
#endif

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }
    netmgr_ap_config_t config;
    memset(&config, 0, sizeof(netmgr_ap_config_t));
    netmgr_get_ap_config(&config);
    LOG("wifi_service_event config.ssid %s", config.ssid);
    if (strcmp(config.ssid, "adha") == 0 || strcmp(config.ssid, "aha") == 0) {
        // clear_wifi_ssid();
        return;
    }

    if (!linkkit_started) {
#ifdef CONFIG_PRINT_HEAP
        print_heap();
#endif
        aos_task_new("linkkit", (void (*)(void *))linkkit_main, NULL, 1024 * 8);
        linkkit_started = 1;
    }
}


static void cloud_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_YUNIO) {
        return;
    }

    LOG("cloud_service_event %d", event->code);

    if (event->code == CODE_YUNIO_ON_CONNECTED) {
        LOG("user sub and pub here");
        return;
    }

    if (event->code == CODE_YUNIO_ON_DISCONNECTED) {
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
        case IOTX_AWSS_START: // AWSS start without enbale, just supports device discover
            // operate led to indicate user
            LOG("IOTX_AWSS_START");
            break;
        case IOTX_AWSS_ENABLE: // AWSS enable, AWSS doesn't parse awss packet until AWSS is enabled.
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
        case IOTX_AWSS_ENABLE_TIMEOUT: // AWSS enable timeout
            // user needs to enable awss again to support get ssid & passwd of router
            LOG("IOTX_AWSS_ENALBE_TIMEOUT");
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

static void start_netmgr(void *p)
{
    iotx_event_regist_cb(linkkit_event_monitor);
    netmgr_start(true);
    aos_task_exit(0);
}

void do_awss_active()
{
    LOG("do_awss_active %d\n", awss_running);
    awss_running = 1;
#ifdef WIFI_PROVISION_ENABLED
    extern int awss_config_press();
    awss_config_press();
#endif
}

static void linkkit_reset(void *p)
{
    netmgr_clear_ap_config();
    HAL_Reboot();
}

extern int  awss_report_reset();
static void do_awss_reset()
{
#ifdef WIFI_PROVISION_ENABLED
    aos_task_new("reset", (void (*)(void *))awss_report_reset, NULL, 2048);
#endif
    aos_post_delayed_action(2000, linkkit_reset, NULL);
}

void linkkit_key_process(input_event_t *eventinfo, void *priv_data)
{
    if (eventinfo->type != EV_KEY) {
        return;
    }
    LOG("awss config press %u\n", eventinfo->value);

    if (eventinfo->code == CODE_BOOT) {
        if (eventinfo->value == VALUE_KEY_CLICK) {
            do_awss_active();
        } else if (eventinfo->value == VALUE_KEY_LTCLICK) {
            do_awss_reset();
        }
    }
}
#ifdef AOS_COMP_CLI
static void handle_gw_mm_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    IOT_SetLogLevel(IOT_LOG_DEBUG);
    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
}
static struct cli_command gw_mm = {
    .name = "gw_mm",
    .help = "gw_mm",
    .function = handle_gw_mm_cmd
};

static void print_identity()
{
    char _product_key[IOTX_PRODUCT_KEY_LEN + 1]       = {0};
    char _device_name[IOTX_DEVICE_NAME_LEN + 1]       = {0};
#ifdef DEMO_DEBUG
    char _product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = {0};
    char _device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = {0};
#endif

    HAL_GetProductKey(_product_key);
    aos_cli_printf("ProductKey: %s\n", _product_key);
#ifdef DEMO_DEBUG
    HAL_GetProductSecret(_product_secret);
    aos_cli_printf("ProductSecret: %s\n", _product_secret);
#endif
    HAL_GetDeviceName(_device_name);
    aos_cli_printf("DeviceName: %s\n", _device_name);
#ifdef DEMO_DEBUG
    HAL_GetDeviceSecret(_device_secret);
    aos_cli_printf("DeviceSecret: %s\n", _device_secret);
#endif
}

static void handle_identity_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    const char *rtype = argc > 1 ? argv[1] : "";
    if (strcmp(rtype, "get") == 0) {
        print_identity();
    } else if (strcmp(rtype, "set") == 0) {
        if (argc == 4) {
            HAL_SaveDeviceIdentity(NULL, NULL, argv[2], argv[3]);
        } else if (argc == 5) {
            HAL_SaveDeviceIdentity(argv[2], argv[3], argv[4], "");
        } else if (argc == 6) {
            HAL_SaveDeviceIdentity(argv[2], argv[3], argv[4], argv[5]);
        } else {
            aos_cli_printf("arg number err! usage:\n");
            aos_cli_printf("identity set {pk} {ps} {dn} [ds] | identity set {dn} {ds}\n");
        }
    } else if (strcmp(rtype, "clear") == 0) {
        HAL_ClearDeviceIdentity();
    } else {
        aos_cli_printf("usage:\n");
        aos_cli_printf("identity [set pk ps dn ds | set dn ds | get | clear]\n");
    }
}

static void handle_reset_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(do_awss_reset, NULL);
}

static void handle_active_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(do_awss_active, NULL);
    aos_cli_printf("awss activated\n");
}

static struct cli_command resetcmd = {
    .name     = "reset",
    .help     = "factory reset",
    .function = handle_reset_cmd
};

static struct cli_command ncmd = {
    .name     = "active_awss",
    .help     = "active_awss [start]",
    .function = handle_active_cmd
};

static struct cli_command identity_cmd = {
    .name     = "identity",
    .help     = "identity [set pk ps dn ds | set dn ds | get | clear]",
    .function = handle_identity_cmd
};
#endif

#ifdef CONFIG_PRINT_HEAP
static void duration_work(void *p)
{
    print_heap();
    aos_post_delayed_action(5000, duration_work, NULL);
}
#endif

static int mqtt_connected_event_handler(void)
{
#if defined(ENABLE_AOS_OTA)
    LOG("OTA service init ...\n");
    ota_register_cb(&ctx, OTA_CB_ID_UPGRADE, (void*)ota_upgrade_cb);
    ota_service_init(ota_get_ctx());
#endif
    return 0;
}

int application_start(int argc, char **argv)
{
#ifdef CONFIG_PRINT_HEAP
    print_heap();
    aos_post_delayed_action(5000, duration_work, NULL);
#endif

#ifdef CSP_LINUXHOST
    signal(SIGPIPE, SIG_IGN);
#endif

#ifdef WITH_SAL
    sal_device_config_t data = {0};

    data.uart_dev.port = 1;
    data.uart_dev.config.baud_rate = 115200;
    data.uart_dev.config.data_width = DATA_WIDTH_8BIT;
    data.uart_dev.config.parity = NO_PARITY;
    data.uart_dev.config.stop_bits  = STOP_BITS_1;
    data.uart_dev.config.flow_control = FLOW_CONTROL_DISABLED;
    data.uart_dev.config.mode = MODE_TX_RX;

    sal_add_dev(NULL, &data);
    sal_init();
#endif

    aos_set_log_level(AOS_LL_DEBUG);

    netmgr_init();
    aos_register_event_filter(EV_KEY, linkkit_key_process, NULL);
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    aos_register_event_filter(EV_YUNIO, cloud_service_event, NULL);
    IOT_RegisterCallback(ITE_MQTT_CONNECT_SUCC, mqtt_connected_event_handler);

#ifdef AOS_COMP_CLI
    aos_cli_register_command(&identity_cmd);
    aos_cli_register_command(&resetcmd);
    aos_cli_register_command(&ncmd);
    aos_cli_register_command(&gw_mm);

#ifdef LINKKIT_GATEWAY_TEST_CMD
    aos_cli_register_command(&gw_test_cmd);
#endif /* LINKKIT_GATEWAY_TEST_CMD */
#endif
    set_iotx_info();
    IOT_SetLogLevel(IOT_LOG_DEBUG);
    aos_task_new("netmgr", start_netmgr, NULL, 4096);

    aos_loop_run();

    return 0;
}
