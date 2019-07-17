/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* #define LOG_NDEBUG 0 */
#include <stdio.h>
#include <string.h>
#include "be_jse_task.h"
#include "be_log.h"
#include "be_port_osal.h"
#include "bone_engine_inl.h"
#include "hal/system.h"

#define WIFI_CONNECT_WAIT_TIME_MS (10 * 1000)
#define WIFI_CHECKIP_INTERVAL_MS 200

static void js_cb_wifi_conn_status(void *pdata) {
    char ip[32]      = {0};
    int js_cb_ref    = (int)pdata;
    duk_context *ctx = bone_engine_get_context();
    bone_engine_push_ref(ctx, js_cb_ref);
    if (hal_system_get_ip(ip) == 0)
        duk_push_string(ctx, "CONNECTED");
    else
        duk_push_string(ctx, "DISCONNECT");
    duk_pcall(ctx, 1);
    duk_pop(ctx);
    bone_engine_unref(ctx, js_cb_ref);
}

static void wifi_check_ip_task(void *arg) {
    char ip[32] = {0};
    int count   = 0;

    while (1) {
        if ((hal_system_get_ip(ip) == 0) ||
            (count > WIFI_CONNECT_WAIT_TIME_MS / WIFI_CHECKIP_INTERVAL_MS)) {
            be_jse_task_schedule_call(js_cb_wifi_conn_status, arg);
            break;
        }
        be_osal_delay(WIFI_CHECKIP_INTERVAL_MS);
        count++;
    }

    be_osal_delete_task(NULL);
}

static duk_ret_t native_wifi_connect(duk_context *ctx) {
    int ret = -1;

    debug("in\n");
    if (!duk_is_string(ctx, 0) || !duk_is_string(ctx, 1) ||
        !duk_is_function(ctx, 2)) {
        warn("parameter must be string, string and function\n");
        goto out;
    }
    const char *ssid   = duk_get_string(ctx, 0);
    const char *passwd = duk_get_string(ctx, 1);
    ret = hal_system_wifi_connect((uint8_t *)ssid, (uint8_t *)passwd);
    if (ret) {
        warn("hal_system_wifi_connect failed\n");
        goto out;
    }
    duk_dup(ctx, 2);
    int js_cb_ref = bone_engine_ref(ctx);
    ret           = be_osal_create_task("wifi_task", wifi_check_ip_task,
                              (void *)js_cb_ref, 4096, WIFI_TSK_PRIORITY, NULL);
    if (ret) {
        warn("be_osal_create_task failed\n");
        bone_engine_unref(ctx, js_cb_ref);
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
static duk_ret_t native_wifi_getip(duk_context *ctx) {
    char ip[32] = {0};
    int ret     = hal_system_get_ip(ip);
    if (ret == 0) {
        duk_push_string(ctx, ip);
    } else {
        duk_push_null(ctx);
    }
    return 1;
}

/*****************************************************************************
 * Function:    native_wifi_getssid
 * Description: js native addon for WIFI.getssid()
 * Called by:   js api
 * Input:       no input
 * Output:      return a string object to js api what the mac is
 *****************************************************************************/
static duk_ret_t native_wifi_getssid(duk_context *ctx) {
    char ssid[32 + 1] = {0};
    int ret           = hal_system_wifi_ssid_get(ssid, sizeof(ssid));
    if (ret == 0) {
        duk_push_string(ctx, ssid);
    } else {
        duk_push_null(ctx);
    }
    return 1;
}

void module_wifi_register(void) {
    duk_context *ctx = bone_engine_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_wifi_connect, 3);
    duk_put_prop_string(ctx, -2, "connect");

    duk_push_c_function(ctx, native_wifi_getip, 0);
    duk_put_prop_string(ctx, -2, "getip");

    duk_push_c_function(ctx, native_wifi_getssid, 0);
    duk_put_prop_string(ctx, -2, "getssid");

    duk_put_global_string(ctx, "WIFI");
}
