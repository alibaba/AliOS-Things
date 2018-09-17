/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "at_util.h"
#include "athost.h"

#define TAG "athost_instance"

static const char *prefix_athost = "AT+";

static bool inited = false;

// default at cmds handled by athost
extern int register_atcmd_coap_opt();
extern int register_atcmd_mqtt_opt();
extern int register_atcmd_cip_opt();
extern int register_atcmd_uart_opt();
extern int register_atcmd_wifi_mgmt_opt();
// extern int register_atcmd_wifi_ywss_opt();

static int athost_atcmds_resgister()
{
    register_atcmd_cip_opt();
#ifdef COAP_AT
    register_atcmd_coap_opt();
#else
    register_atcmd_mqtt_opt();
#endif
    register_atcmd_wifi_mgmt_opt();
    // register_atcmd_wifi_ywss_opt();
    //register_atcmd_uart_opt();

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

    if (athost_handle_register_cb(prefix_athost, atcmd_handle) < 0) {
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
