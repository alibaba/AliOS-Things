/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include <be_port_osal.h>
#include <hal/system.h>

#include "be_jse_api.h"
#include "be_jse_module.h"
#include "be_jse_task.h"
#include "module_wifi.h"

#define JS_WIFI_TAG "WIFI"
#define WIFI_CONNECT_WAIT_TIME_MS (10 * 1000)
#define WIFI_CHECKIP_INTERVAL_MS (200)
#define MAX_SSID_SIZE (32 - 1)
#define MAX_PWD_SIZE (64 - 1)

static void js_cb_wifi_conn_status(void *pdata)
{
    be_jse_symbol_t *jscb = (be_jse_symbol_t *)pdata;
    char ip[32]           = {0};

    BE_ASYNC_S *async  = (BE_ASYNC_S *)calloc(1, sizeof(BE_ASYNC_S));
    async->func        = jscb;
    async->param_count = 1;
    async->params      = (be_jse_symbol_t **)calloc(
        1, sizeof(be_jse_symbol_t *) * async->param_count);
    if (hal_system_get_ip(ip) == 0) {
        printf("ip=%s\n\r", ip);
        async->params[0] = new_str_symbol("CONNECTED");
    } else {
        async->params[0] = new_str_symbol("DISCONNECT");
    }
    be_jse_async_event_cb(async);
}

static void *wifi_check_ip_task(void *arg)
{
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
    return NULL;
}

static be_jse_symbol_t *module_wifi_connect()
{
    int ret                      = -1;
    be_jse_symbol_t *arg0        = NULL;
    be_jse_symbol_t *arg1        = NULL;
    be_jse_symbol_t *arg2        = NULL;
    be_jse_symbol_t *func        = NULL;
    uint8_t ssid[MAX_SSID_SIZE]  = {0};
    uint8_t passwd[MAX_PWD_SIZE] = {0};

    be_jse_handle_function(0, &arg0, &arg1, &arg2, NULL);
    if (arg0 == NULL || arg1 == NULL || arg2 == NULL ||
        !symbol_is_string(arg0) || !symbol_is_string(arg1)) {
        be_error(JS_WIFI_TAG, "module_wifi_connect arg invalid\n\r");
        goto Fail;
    }

    if (!symbol_is_function(arg2) || (symbol_str_len(arg0) > MAX_SSID_SIZE) ||
        (symbol_str_len(arg1) > MAX_PWD_SIZE)) {
        be_error(JS_WIFI_TAG, "module_wifi_connect arg lend exceed\n\r");
        goto Fail;
    }

    symbol_to_str(arg0, (char *)ssid, symbol_str_len(arg0));
    symbol_to_str(arg1, (char *)passwd, symbol_str_len(arg1));

    hal_system_wifi_connect(ssid, passwd);
    INC_SYMBL_REF(arg2);
    ret = be_osal_create_task("wifi_task", wifi_check_ip_task, arg2, 2048,
                              WIFI_TSK_PRIORITY, NULL);

Fail:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    symbol_unlock(arg2);
    return new_int_symbol(ret);
}

/*****************************************************************************
 *Function:    module_wifi_getip
 *Description: js native addon for WIFI.getip()
 *Called by:   js api
 *Input:       no input
 *Output:      return a string object to js api,if it is NULL means can't get ip
 *****************************************************************************/
static be_jse_symbol_t *module_wifi_getip()
{
    char ip[32] = {0};

    be_jse_handle_function(0, 0, 0, 0, 0);
    int ret = hal_system_get_ip(ip);
    if (ret == 0) {
        return new_str_symbol(ip);
    } else {
        return new_symbol(BE_SYM_NULL);
    }
}

/*****************************************************************************
 *Function:    module_wifi_getmac
 *Description: js native addon for WIFI.getmac()
 *Called by:   js api
 *Input:       no input
 *Output:      return a string object to js api what the mac is
 *****************************************************************************/
static be_jse_symbol_t *module_wifi_getssid()
{
    char ssid[32 + 1] = {0};

    be_jse_handle_function(0, 0, 0, 0, 0);
    int ret = hal_system_wifi_ssid_get(ssid, sizeof(ssid));

    if (ret == 0) {
        return new_str_symbol(ssid);
    } else {
        return new_symbol(BE_SYM_NULL);
    }
}

be_jse_symbol_t *module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                  be_jse_symbol_t *var, const char *name)
{
    be_debug(JS_WIFI_TAG, "%s,name=%s\n\r", __FUNCTION__, name);

    if (strcmp(name, "connect") == 0) return module_wifi_connect();
    if (strcmp(name, "getip") == 0) return module_wifi_getip();
    if (strcmp(name, "getssid") == 0) return module_wifi_getssid();

    return BE_JSE_FUNC_UNHANDLED;
}

void module_wifi_register(void)
{
    int ret = -1;

    be_jse_module_load(JS_WIFI_TAG, module_handle_cb);
}
