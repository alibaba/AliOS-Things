/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/yloop.h"
#include "linkkit/infra/infra_compat.h"
#include "netmgr.h"
#include "app_entry.h"
#include "aos/cli.h"

#ifdef CSP_LINUXHOST
    #include <signal.h>
#endif

#ifdef WITH_SAL
#include <atcmd_config_module.h>
#endif

static char linkkit_started = 0;

static app_main_paras_t entry_paras;

typedef void (*task_fun)(void *);

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
        //clear_wifi_ssid();
        return;
    }

    if (!linkkit_started) {
        aos_task_new("iotx_example", (task_fun)linkkit_main, (void *)&entry_paras, 1024 * 6);
        linkkit_started = 1;
    }
}
#ifdef AOS_COMP_CLI

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

static struct cli_command identity_cmd = {
    .name     = "identity",
    .help     = "identity [set pk ps dn ds | set dn ds | get | clean ]",
    .function = handle_identity_cmd
};
#endif

#ifdef TEST_LOOP
const  char *input_data[2] = {"mqttapp", "loop"};
#endif

int application_start(int argc, char **argv)
{
#ifdef CSP_LINUXHOST
    signal(SIGPIPE, SIG_IGN);
#endif

#ifdef TEST_LOOP
    argc = 2;
    argv = (char **)input_data;
#endif
    entry_paras.argc = argc;
    entry_paras.argv = argv;

#ifdef WITH_SAL
    sal_device_config_t data = {0};
    data.uart_dev.port = 1;
    data.uart_dev.config.baud_rate = 115200;
    data.uart_dev.config.data_width = DATA_WIDTH_8BIT;
    data.uart_dev.config.parity = NO_PARITY;
    data.uart_dev.config.stop_bits  = STOP_BITS_1;
    data.uart_dev.config.flow_control = FLOW_CONTROL_DISABLED;
    data.uart_dev.config.mode = MODE_TX_RX;
    sal_add_dev("mk3060", &data);
    sal_init();
#endif

#ifdef MDAL_MAL_ICA_TEST
    HAL_MDAL_MAL_Init();
#endif
#ifdef AOS_COMP_CLI
    aos_cli_register_command(&identity_cmd);
#endif
    aos_set_log_level(AOS_LL_DEBUG);

    netmgr_init();

    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);

    netmgr_start(false);
#if defined (CSP_LINUXHOST) && !defined (WITH_SAL)
    aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0);
#endif
    aos_loop_run();

    return 0;
}
