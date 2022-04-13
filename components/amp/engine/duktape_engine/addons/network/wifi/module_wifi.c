/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "amp_system.h"
#include "amp_defines.h"
#include "amp_network.h"
#include "amp_task.h"
#include "be_inl.h"

#define MOD_STR "WIFI"
#define WIFI_CONNECT_WAIT_TIME_MS (10 * 1000)
#define WIFI_CHECKIP_INTERVAL_MS 200

static void js_cb_wifi_conn_status(void *pdata)
{
    int ret      = -1;
    int js_cb_ref    = (int)pdata;
    wifi_info_t wifi_info;

    ret = HAL_WiFi_Get_Info(&wifi_info);
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

static void *wifi_check_ip_task(void *arg)
{
    int ret = -1;
    int count   = 0;
    wifi_info_t wifi_info;

    ret = HAL_WiFi_Get_Info(&wifi_info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get wifi info failed");
    }

    while (1) {
        if ((strcmp(wifi_info.ip, "0,0,0,0") != 0) ||
            (count > WIFI_CONNECT_WAIT_TIME_MS / WIFI_CHECKIP_INTERVAL_MS)) {
            amp_task_schedule_call(js_cb_wifi_conn_status, arg);
            break;
        }
        HAL_SleepMs(WIFI_CHECKIP_INTERVAL_MS);
        count++;
    }

    return NULL;
}

static duk_ret_t native_wifi_connect(duk_context *ctx)
{
    int ret = -1;
    void *wifi_connect_task;
    int wifi_connect_task_stack_used;
    amp_os_thread_param_t wifi_connect_task_params = {0};

    if (!duk_is_string(ctx, 0) || !duk_is_string(ctx, 1) ||
        !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be string, string and function\n");
        goto out;
    }
    const char *ssid   = duk_get_string(ctx, 0);
    const char *passwd = duk_get_string(ctx, 1);

    ret = HAL_WiFi_Connect(ssid, passwd);
    if (ret != 0) {
        amp_warn(MOD_STR, "wifi connect failed\n");
        goto out;
    }
    duk_dup(ctx, 2);
    int js_cb_ref = be_ref(ctx);

    wifi_connect_task_params.name = "wifi connect task";
    wifi_connect_task_params.priority = ADDON_TSK_PRIORRITY;
    wifi_connect_task_params.stack_size = 1024 * 2;

    ret = HAL_ThreadCreate(&wifi_connect_task, wifi_check_ip_task, (void *)js_cb_ref, &wifi_connect_task_params, &wifi_connect_task_stack_used);

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
static duk_ret_t native_wifi_getInfo(duk_context *ctx)
{
    int ret = -1;
    wifi_info_t wifi_info;

    ret = HAL_WiFi_Get_Info(&wifi_info);
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

    ret = HAL_WiFi_Disconnect();
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

    duk_push_c_function(ctx, native_wifi_connect, 3);
    duk_put_prop_string(ctx, -2, "connect");

    duk_push_c_function(ctx, native_wifi_getInfo, 0);
    duk_put_prop_string(ctx, -2, "getInfo");

    duk_push_c_function(ctx, native_wifi_disconnect, 0);
    duk_put_prop_string(ctx, -2, "disconnect");

    duk_put_prop_string(ctx, -2, "WIFI");
}
