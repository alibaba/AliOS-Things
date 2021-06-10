/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "aos_network.h"
#include "netmgr_wifi.h"
#include "amp_task.h"
#include "be_inl.h"

#define MOD_STR "WIFI"
#define WIFI_CONNECT_WAIT_TIME_MS (10 * 1000)
#define WIFI_CHECKIP_INTERVAL_MS 200

static void js_cb_wifi_conn_status(void *pdata)
{
    int ret = -1;
    int js_cb_ref = (int)pdata;
    aos_wifi_info_t wifi_info;

    ret = aos_get_wifi_info(&wifi_info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get wifi info failed");
    }

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, js_cb_ref);

    if (strcmp(wifi_info.ip, "0,0,0,0") != 0)
        duk_push_string(ctx, "CONNECTED");
    else
        duk_push_string(ctx, "DISCONNECT");
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    be_unref(ctx, js_cb_ref);
    duk_gc(ctx, 0);
}

static void wifi_check_ip_task(void *arg)
{
    int ret = -1;
    int count = 0;
    aos_wifi_info_t wifi_info;

    ret = aos_get_wifi_info(&wifi_info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get wifi info failed");
    }

    while (1) {
        if ((strcmp(wifi_info.ip, "0,0,0,0") != 0) ||
            (count > WIFI_CONNECT_WAIT_TIME_MS / WIFI_CHECKIP_INTERVAL_MS)) {
            amp_task_schedule_call(js_cb_wifi_conn_status, arg);
            break;
        }
        aos_msleep(WIFI_CHECKIP_INTERVAL_MS);
        count++;
    }

    return;
}

static duk_ret_t native_wifi_connect(duk_context *ctx)
{
    int ret = -1;
    aos_task_t wifi_connect_task;

    if (!duk_is_string(ctx, 0) || !duk_is_string(ctx, 1) ||
        !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be string, string and function\n");
        goto out;
    }
    const char *ssid = duk_get_string(ctx, 0);
    const char *passwd = duk_get_string(ctx, 1);

    ret = aos_wifi_connect(ssid, passwd);
    if (ret != 0) {
        amp_warn(MOD_STR, "wifi connect failed\n");
        goto out;
    }
    duk_dup(ctx, 2);
    int js_cb_ref = be_ref(ctx);

    ret = aos_task_new_ext(&wifi_connect_task, "wifi connect task", wifi_check_ip_task, (void *)js_cb_ref, 1024 * 2, ADDON_TSK_PRIORRITY);

    if (ret != 0) {
        amp_warn(MOD_STR, "jse_osal_create_task failed\n");
        be_unref(ctx, js_cb_ref);
    }
out:
    duk_push_int(ctx, ret);
    return 1;
}

/*****************************************************************************
 * Function:    native_wifi_getip
 * Description: js native addon for WIFI.getip()
 * Called by:   js api
 * Input:       no input
 * Output:      return a string object to js api,if it is NULL means can't get
 *ip
 *****************************************************************************/
static duk_ret_t native_wifi_set_ifconfig(duk_context *ctx)
{
    int ret = -1;
    netmgr_ifconfig_info_t info;

    if (!duk_is_object(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be object\n");
        goto out;
    }

    /* get device certificate */
    duk_get_prop_string(ctx, 0, "dhcp_en");
    duk_get_prop_string(ctx, 0, "ip_addr");
    duk_get_prop_string(ctx, 0, "mask");
    duk_get_prop_string(ctx, 0, "gw");
    duk_get_prop_string(ctx, 0, "dns_server");
    duk_get_prop_string(ctx, 0, "mac");

    memcpy(info.ip_addr, duk_get_string(ctx, -5), strlen(duk_get_string(ctx, -5)));
    memcpy(info.mask, duk_get_string(ctx, -4), strlen(duk_get_string(ctx, -4)));
    memcpy(info.gw, duk_get_string(ctx, -3), strlen(duk_get_string(ctx, -3)));
    memcpy(info.dns_server, duk_get_string(ctx, -2), strlen(duk_get_string(ctx, -2)));
    memcpy(info.mac, duk_get_string(ctx, -1), strlen(duk_get_string(ctx, -1)));


    ret = aos_wifi_set_ifconfig(&info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get wifi info failed");
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

/*****************************************************************************
 * Function:    native_wifi_getip
 * Description: js native addon for WIFI.getip()
 * Called by:   js api
 * Input:       no input
 * Output:      return a string object to js api,if it is NULL means can't get
 *ip
 *****************************************************************************/
static duk_ret_t native_wifi_get_ifconfig(duk_context *ctx)
{
    int ret = -1;
    aos_wifi_info_t wifi_info;

    ret = aos_get_wifi_info(&wifi_info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get wifi info failed");
        goto out;
    }

    const char *ssid = wifi_info.ssid;
    const char *ip = wifi_info.ip;
    const char *mac = wifi_info.mac;
    int rssi = wifi_info.rssi;

    duk_push_object(ctx);

    duk_push_string(ctx, ssid);
    duk_put_prop_string(ctx, -2, "ssid");

    duk_push_string(ctx, ip);
    duk_put_prop_string(ctx, -2, "ip");

    duk_push_string(ctx, mac);
    duk_put_prop_string(ctx, -2, "mac");

    duk_push_int(ctx, rssi);
    duk_put_prop_string(ctx, -2, "rssi");

    return 1;

out:
    duk_push_int(ctx, ret);
    return 1;
}

/*****************************************************************************
 * Function:    native_wifi_getssid
 * Description: js native addon for WIFI.getssid()
 * Called by:   js api
 * Input:       no input
 * Output:      return a string object to js api what the mac is
 *****************************************************************************/
static duk_ret_t native_wifi_disconnect(duk_context *ctx)
{
    int ret = -1;

    ret = aos_wifi_disconnect();
    if (ret != 0) {
        amp_debug(MOD_STR, "wifi disconnect failed");
        goto out;
    }

    return 1;
out:
    duk_push_int(ctx, ret);
    return 1;
}

/*****************************************************************************
 * Function:    native_wifi_getssid
 * Description: js native addon for WIFI.getssid()
 * Called by:   js api
 * Input:       no input
 * Output:      return a string object to js api what the mac is
 *****************************************************************************/
static duk_ret_t native_wifi_get_type(duk_context *ctx)
{
    int ret = -1;

    ret = aos_get_network_type();

out:
    duk_push_int(ctx, ret);
    return 1;
}

/*****************************************************************************
 * Function:    native_wifi_getssid
 * Description: js native addon for WIFI.getssid()
 * Called by:   js api
 * Input:       no input
 * Output:      return a string object to js api what the mac is
 *****************************************************************************/
static duk_ret_t native_wifi_get_state(duk_context *ctx)
{
    int ret = -1;

    ret = aos_wifi_disconnect();
    if (ret != 0) {
        amp_debug(MOD_STR, "wifi disconnect failed");
        goto out;
    }

    return 1;
out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_wifi_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("getType", native_wifi_get_type, 0);
    AMP_ADD_FUNCTION("setIfConfig", native_wifi_set_ifconfig, 1);
    AMP_ADD_FUNCTION("getIfConfig", native_wifi_get_ifconfig, 0);
    AMP_ADD_FUNCTION("connect", native_wifi_connect, 3);
    AMP_ADD_FUNCTION("disconnect", native_wifi_disconnect, 0);
    AMP_ADD_FUNCTION("getState", native_wifi_get_state, 0);

    duk_put_prop_string(ctx, -2, "WIFI");
}
