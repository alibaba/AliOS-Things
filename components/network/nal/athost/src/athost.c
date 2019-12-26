/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdbool.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"

#include "at_util.h"
#include "athost_export.h"
#include "hal_athost.h"

#define TAG "athost_instance"

static const char *prefix_athost = "AT+";

static bool inited = false;

static int athost_atcmds_resgister()
{
#ifdef ATHOST_CIP
    extern int register_atcmd_cip_opt();
    register_atcmd_cip_opt();
#endif

#ifdef ATHOST_WIFI_MGMT
    extern int register_atcmd_wifi_mgmt_opt();
    register_atcmd_wifi_mgmt_opt();
#endif

#ifdef ATHOST_MQTT
    extern int register_atcmd_mqtt_opt();
    register_atcmd_mqtt_opt();
#endif

#ifdef ATHOST_COAP
    extern int register_atcmd_coap_opt();
    register_atcmd_coap_opt();
#endif

#ifdef ATHOST_WIFI_YWSS
    extern int register_atcmd_wifi_ywss_opt();
    register_atcmd_wifi_ywss_opt();
#endif

#ifdef ATHOST_UART_OPT
    extern int register_atcmd_uart_opt();
    register_atcmd_uart_opt();
#endif

    return 0;
}

int athost_instance_init()
{
    if (inited) {
        LOGW(TAG, "athost instance already initialized");
        return 0;
    }

    if (athost_atcmds_resgister() < 0) {
        LOGE(TAG, "atcmds resigister fail!\n");
        goto err;
    }

    if (HAL_Athost_HandleRegisterCb(prefix_athost, atcmd_handle_entry) < 0) {
        LOGE(TAG, "atcmds handle resigister fail!\n");
        goto err;
    }

    if (atcmd_handle_init() < 0) {
        LOGE(TAG, "atcmds handle init fail!\n");
        goto err;
    }

    inited = true;
    return 0;

err:
    inited = false;

    return -1;
}
