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

static void print_devinfo()
{
    char _product_key[IOTX_PRODUCT_KEY_LEN + 1]       = {0};
    char _device_name[IOTX_DEVICE_NAME_LEN + 1]       = {0};
#ifdef DEMO_DEBUG
    char _product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = {0};
    char _device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = {0};
#endif
    HAL_GetProductKey(_product_key);
    HAL_GetDeviceName(_device_name);
    LOG("pk:%s", _product_key);
    LOG("dn:%s", _device_name);
#ifdef DEMO_DEBUG
    HAL_GetProductSecret(_product_secret);
    HAL_GetDeviceSecret(_device_secret);
    LOG("ps:%s", _product_secret);
    LOG("ds:%s", _device_secret);
#endif
}

static void set_devinfo(char *pk, char *ps, char *dn, char *ds)
{
    if (dn != NULL) {
        HAL_SetDeviceName(dn);
    }
    if (ds != NULL) {
        HAL_SetDeviceSecret(ds);
    }
    if (pk != NULL) {
        HAL_SetProductKey(pk);
    }
    if (ps != NULL) {
        HAL_SetProductSecret(ps);
    }
}

static void handle_devinfo_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    const char *rtype = argc > 1 ? argv[1] : "";
    if (strcmp(rtype, "get") == 0) {
        print_devinfo();
    } else if (strcmp(rtype, "set") == 0) {
        if (argc == 4) {
            set_devinfo(NULL, NULL, argv[2], argv[3]);
        } else if (argc == 5) {
            set_devinfo(argv[2], argv[3], argv[4], "");
        } else if (argc == 6) {
            set_devinfo(argv[2], argv[3], argv[4], argv[5]);
        } else {
            LOG("arg number err! usage:");
            LOG("devinfo set {pk} {ps} {dn} [ds] | devinfo set {dn} {ds}");
        }
    } else if (strcmp(rtype, "clean") == 0) {
        set_devinfo("", "", "", "");
    } else {
        LOG("usage:");
        LOG("devinfo [set pk ps dn ds | set dn ds | get | clean]");
    }
}

static struct cli_command devinfo_cmd = { .name     = "devinfo",
    .help     = "devinfo [set pk ps dn ds | set dn ds | get | clean ]",
     .function = handle_devinfo_cmd
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
    aos_cli_register_command(&devinfo_cmd);
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
