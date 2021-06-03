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
#include "aos/list.h"

#define MOD_STR "module_netmgr"
#define CONNECT_WAIT_TIME_MS (10 * 1000)
#define CHECKIP_INTERVAL_MS 200

typedef int js_cb_ref;
typedef struct msg_cb_info {
    slist_t next;
    js_cb_ref cb_ref;
} msg_cb_info_t;

typedef struct wifi_connect_task_params {
    js_cb_ref cb_ref;
    netmgr_hdl_t hdl;
} wifi_connect_task_params_t;

static slist_t g_msg_cb_list_head;

static void js_cb_conn_status(void *pdata)
{
    int ret = -1;
    wifi_connect_task_params_t* params;
    netmgr_ifconfig_info_t info;
    netmgr_hdl_t hdl;
    int cb_ref;

    if(pdata == NULL) {
        amp_debug(MOD_STR, "pdata is null");
        return ;
    }

    params = (wifi_connect_task_params_t*) pdata;
    hdl = params->hdl;
    ret = netmgr_get_ifconfig(hdl, &info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get ifconfig info failed");
    }

    duk_context *ctx = be_get_context();
    cb_ref = params->cb_ref;
    be_push_ref(ctx, cb_ref);

    if (strcmp(info.ip_addr, "0.0.0.0") != 0)
        duk_push_string(ctx, "CONNECTED");
    else
        duk_push_string(ctx, "DISCONNECT");
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    be_unref(ctx, cb_ref);
    duk_gc(ctx, 0);
}

static void check_ip_task(void *arg)
{
    int ret = -1;
    int count = 0;
    netmgr_ifconfig_info_t info;
    wifi_connect_task_params_t* params;
    netmgr_hdl_t hdl;

    if(arg == NULL) {
        amp_debug(MOD_STR, "check ip task arg is null");
        return ;
    }

    params = (wifi_connect_task_params_t*) arg;
    hdl = params->hdl;

    while (1) {
        ret = netmgr_get_ifconfig(hdl, &info);
        if (ret != 0) {
            amp_debug(MOD_STR, "get ifconfig info failed");
        }

        if ((strcmp(info.ip_addr, "0.0.0.0") != 0) ||
            (count > CONNECT_WAIT_TIME_MS / CHECKIP_INTERVAL_MS)) {
            amp_task_schedule_call(js_cb_conn_status, arg);
            break;
        }
        aos_msleep(CHECKIP_INTERVAL_MS);
        count++;
    }

    return;
}

static duk_ret_t native_netmgr_service_init(duk_context *ctx)
{
    int ret;

    ret = event_service_init(NULL);
    if (ret != 0) {
        amp_debug(MOD_STR, "netmgr service init failed");
        goto out;
    }

    ret = netmgr_service_init(NULL);
    if (ret != 0) {
        amp_debug(MOD_STR, "netmgr service init failed");
        goto out;
    }
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_netmgr_service_deinit(duk_context *ctx)
{
    netmgr_hdl_t hdl;
    int ret = -1;

    if (!duk_is_number(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be number\n");
        goto out;
    }
    hdl = duk_get_number(ctx, 0);

    netmgr_service_deinit(hdl);
    ret = 0;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_netmgr_add_dev(duk_context *ctx)
{
    char* name;
    int ret = -1;

    if (!duk_is_string(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be string\n");
        goto out;
    }

    name = duk_get_string(ctx, 0);

    ret = netmgr_add_dev(name);
    if (ret != 0) {
        amp_debug(MOD_STR, "netmgr add dev failed");
        goto out;
    }
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_netmgr_get_dev(duk_context *ctx)
{
    char* name;
    int ret = -1;

    if (!duk_is_string(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be string\n");
        goto out;
    }

    name = duk_get_string(ctx, 0);

    ret = netmgr_get_dev(name);
    if (ret != 0) {
        amp_debug(MOD_STR, "netmgr get dev failed");
        goto out;
    }
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_netmgr_set_auto_reconnect(duk_context *ctx)
{
    int ret = -1;
    netmgr_hdl_t hdl;
    int enable;

    if (!duk_is_number(ctx, 0) || !duk_is_number(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be number and number\n");
        goto out;
    }
    hdl = duk_get_int(ctx, 0);

    enable = duk_get_int(ctx, 1);
    if(enable == 1) {
        netmgr_set_auto_reconnect(hdl, true);
        ret = 0;
    } else if(enable == 0) {
        netmgr_set_auto_reconnect(hdl, false);
        ret = 0;
    }
out:
    duk_push_int(ctx, ret);
    return 1;
}

static wifi_connect_task_params_t connect_task_params;
static duk_ret_t native_netmgr_connect(duk_context *ctx)
{
    int ret = -1;
    aos_task_t netmgr_connect_task;
    netmgr_hdl_t hdl;
    netmgr_connect_params_t params;
    char* ssid;
    char* password;
    char* bssid;
    int timeout_ms;

    if (!duk_is_number(ctx, 0) || !duk_is_object(ctx, 1)
            || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be number, object and function\n");
        goto out;
    }
    hdl = duk_get_int(ctx, 0);

    duk_get_prop_string(ctx, 1, "ssid");
    duk_get_prop_string(ctx, 1, "password");
    duk_get_prop_string(ctx, 1, "bssid");
    duk_get_prop_string(ctx, 1, "timeout_ms");

    if(!duk_is_string(ctx, -4) || !duk_is_string(ctx, -3)
           || !duk_is_string(ctx, -2) || !duk_is_number(ctx, -1)) {
        amp_warn(MOD_STR, "invalid params\n");
        ret = -2;
        goto out;
    }

    ssid = duk_get_string(ctx, -4);
    password = duk_get_string(ctx, -3);
    bssid = duk_get_string(ctx, -2);
    timeout_ms = duk_get_number(ctx, -1);

    memset(&params, 0, sizeof(netmgr_connect_params_t));
    params.type = NETMGR_TYPE_WIFI;
    strncpy(params.params.wifi_params.ssid, ssid, sizeof(params.params.wifi_params.ssid) - 1);
	strncpy(params.params.wifi_params.pwd, password, sizeof(params.params.wifi_params.pwd) - 1);
    params.params.wifi_params.timeout = timeout_ms;
    ret = netmgr_connect(hdl, &params);
    if (ret != 0) {
        amp_warn(MOD_STR, "netmgr connect failed\n");
        goto out;
    }

    duk_dup(ctx, 2);
    int cb_ref = be_ref(ctx);

    memset(&connect_task_params, 0, sizeof(connect_task_params));
    connect_task_params.cb_ref = cb_ref;
    connect_task_params.hdl = hdl;

    ret = aos_task_new_ext(&netmgr_connect_task, "netmgr connect task", check_ip_task, (void *)&connect_task_params, 1024 * 2, ADDON_TSK_PRIORRITY);

    if (ret != 0) {
        amp_warn(MOD_STR, "jse_osal_create_task failed\n");
        be_unref(ctx, cb_ref);
    }
out:
    duk_push_int(ctx, ret);
    return 1;
}

/*****************************************************************************
 * Function:    native_netmgr_disconnect
 * Description: js native addon for WIFI.getssid()
 * Called by:   js api
 * Input:       no input
 * Output:      return a string object to js api what the mac is
 *****************************************************************************/
static duk_ret_t native_netmgr_disconnect(duk_context *ctx)
{
    int ret = -1;
    netmgr_hdl_t hdl;

    if (!duk_is_number(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be number\n");
        goto out;
    }
    hdl = duk_get_number(ctx, 0);

    ret = netmgr_disconnect(hdl);
    if (ret != 0) {
        amp_debug(MOD_STR, "netmgr disconnect failed");
        goto out;
    }
out:
    duk_push_int(ctx, ret);
    return 1;
}

/*****************************************************************************
 * Function:    native_netmgr_get_state
 * Description: js native addon for NETMGR.getState()
 * Called by:   js api
 * Input:       no input
 * Output:      return a string object to js api what the mac is
 *****************************************************************************/
static duk_ret_t native_netmgr_get_state(duk_context *ctx)
{
    int ret = -1;
    netmgr_hdl_t hdl;

    if (!duk_is_number(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be number\n");
        goto out;
    }
    hdl = duk_get_number(ctx, 0);

    ret = netmgr_get_state(hdl);
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_netmgr_save_config(duk_context *ctx)
{
    int ret = -1;
    netmgr_hdl_t hdl;

    if (!duk_is_number(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be number\n");
        goto out;
    }
    hdl = duk_get_number(ctx, 0);

    ret = netmgr_save_config(hdl);
    if (ret != 0) {
        amp_debug(MOD_STR, "netmgr get state failed");
        goto out;
    }
out:
    duk_push_int(ctx, ret);
    return 1;
}
static duk_ret_t native_netmgr_get_config(duk_context *ctx)
{
    int ret = -1;
    //TODO
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_netmgr_del_config(duk_context *ctx)
{
    int ret = -1;
    //TODO
out:
    duk_push_int(ctx, ret);
    return 1;
}
/*****************************************************************************
 * Function:    native_netmgr_set_ifconfig
 * Description: js native addon for NETMGR.setIconfig()
 * Called by:   js api
 * Input:       no input
 * Output:      return a string object to js api,if it is NULL means can't get
 *ip
 *****************************************************************************/
static duk_ret_t native_netmgr_set_ifconfig(duk_context *ctx)
{
    int ret = -1;
    netmgr_hdl_t hdl;
    netmgr_ifconfig_info_t info;

    if (!duk_is_number(ctx, 0) || !duk_is_object(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be number and object\n");
        goto out;
    }

    hdl = duk_get_number(ctx, 0);

    /* get device certificate */
    duk_get_prop_string(ctx, 1, "dhcp_en");
    duk_get_prop_string(ctx, 1, "ip_addr");
    duk_get_prop_string(ctx, 1, "mask");
    duk_get_prop_string(ctx, 1, "gw");
    duk_get_prop_string(ctx, 1, "dns_server");
    duk_get_prop_string(ctx, 1, "mac");

    info.dhcp_en = duk_get_boolean(ctx, -6);
    memcpy(info.ip_addr, duk_get_string(ctx, -5), strlen(duk_get_string(ctx, -5)));
    memcpy(info.mask, duk_get_string(ctx, -4), strlen(duk_get_string(ctx, -4)));
    memcpy(info.gw, duk_get_string(ctx, -3), strlen(duk_get_string(ctx, -3)));
    memcpy(info.dns_server, duk_get_string(ctx, -2), strlen(duk_get_string(ctx, -2)));
    memcpy(info.mac, duk_get_string(ctx, -1), strlen(duk_get_string(ctx, -1)));

    ret = netmgr_set_ifconfig(hdl, &info);
    if (ret != 0) {
        amp_debug(MOD_STR, "set ifconfig failed");
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

/*****************************************************************************
 * Function:    native_netmgr_get_ifconfig
 * Description: js native addon for NETMGR.getIfconfig()
 * Called by:   js api
 * Input:       no input
 * Output:      return a string object to js api,if it is NULL means can't get
 *ip
 *****************************************************************************/
static duk_ret_t native_netmgr_get_ifconfig(duk_context *ctx)
{
    int ret = -1;
    netmgr_hdl_t hdl;
    netmgr_ifconfig_info_t info;

    if (!duk_is_number(ctx, 0) || !duk_is_object(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be number and object\n");
        goto out;
    }
    hdl = duk_get_number(ctx, 0);

    ret = netmgr_get_ifconfig(hdl, &info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get ifconfig failed");
        goto out;
    }

    const bool dhcp_en = info.dhcp_en;
    const char *ip_addr = info.ip_addr;
    const char *mask = info.mask;
    const char *gw = info.gw;
    const char *dns_server = info.dns_server;
    const char *mac = info.mac;
    const int rssi = info.rssi;

    duk_push_object(ctx);

    duk_push_boolean(ctx, dhcp_en);
    duk_put_prop_string(ctx, 1, "dhcp_en");

    duk_push_string(ctx, ip_addr);
    duk_put_prop_string(ctx, 1, "ip_addr");

    duk_push_string(ctx, mask);
    duk_put_prop_string(ctx, 1, "mask");

    duk_push_string(ctx, gw);
    duk_put_prop_string(ctx, 1, "gw");

    duk_push_string(ctx, dns_server);
    duk_put_prop_string(ctx, 1, "dns_server");

    duk_push_string(ctx, mac);
    duk_put_prop_string(ctx, 1, "mac");

    duk_push_int(ctx, rssi);
    duk_put_prop_string(ctx, 1, "rssi");
out:
    duk_push_int(ctx, ret);
    return 1;
}

static int set_msg_cb(netmgr_msg_t* msg)
{
    msg_cb_info_t* info;
    duk_context *ctx = be_get_context();

    slist_for_each_entry(&g_msg_cb_list_head, info, msg_cb_info_t, next) {
        be_push_ref(ctx, info->cb_ref);
            switch(msg->id) {
            case NETMGR_MSGID_WIFI_STATUS:
                duk_push_object(ctx);
                duk_push_int(ctx, msg->id);
                duk_put_prop_string(ctx, 0, "msgid");
                duk_push_string(ctx, msg->data.network_status_change);
                duk_put_prop_string(ctx, 0, "data");
                break;
            case NETMGR_MSGID_WIFI_STATUS_FROM_IMPL:
                duk_push_object(ctx);
                duk_push_int(ctx, msg->id);
                duk_put_prop_string(ctx, 0, "msgid");
                duk_push_string(ctx, msg->data.status);
                duk_put_prop_string(ctx, 0, "data");
                break;
            case NETMGR_MSGID_WIFI_TRACE_FROM_IMPL:
            case NETMGR_MSGID_NETWORK_STATUS:
            case NETMGR_MSGID_ETH_STATUS_FROM_IMPL:
            default:
                return -1;
        }

        if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
            amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
        }
    }
    return 0;
}

static duk_ret_t native_netmgr_set_msg_cb(duk_context *ctx)
{
    netmgr_hdl_t hdl;
    int ret = -1;
    int msg_cb_ref;
    msg_cb_info_t *info;

    if (!duk_is_number(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be number and function\n");
        goto out;
    }
    hdl = duk_get_number(ctx, 0);

    duk_dup(ctx, 1);
    msg_cb_ref = be_ref(ctx);

    info = (msg_cb_info_t*)malloc(sizeof(msg_cb_info_t));
    if(info != NULL) {
        if(slist_empty(&g_msg_cb_list_head)) {
            ret = netmgr_set_msg_cb(hdl, set_msg_cb);
        }
        memset(info, 0, sizeof(msg_cb_info_t));
        info->cb_ref = msg_cb_ref;
        slist_add(info, &g_msg_cb_list_head);
        ret = 0;
    }
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_netmgr_del_msg_cb(duk_context *ctx)
{
    netmgr_hdl_t hdl;
    msg_cb_info_t *info;
    int msg_cb_ref;
    int ret = -1;

    if (!duk_is_number(ctx, 0) || !duk_is_object(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be number and object\n");
        goto out;
    }
    hdl = duk_get_number(ctx, 0);

    duk_dup(ctx, 1);
    msg_cb_ref = be_ref(ctx);

    slist_for_each_entry(&g_msg_cb_list_head, info, msg_cb_info_t, next) {
        if(msg_cb_ref == info->cb_ref) {
            slist_del(&(info->next), &g_msg_cb_list_head);
            ret = 0;
            break;
        }
    }
    if(slist_empty(&g_msg_cb_list_head)) {
        ret = netmgr_del_msg_cb(hdl, set_msg_cb);
    }
out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_netmgr_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);
    AMP_ADD_FUNCTION("serviceInit", native_netmgr_service_init, 0);
    AMP_ADD_FUNCTION("serviceDeinit", native_netmgr_service_deinit, 1);
    AMP_ADD_FUNCTION("addDev", native_netmgr_add_dev, 1);
    AMP_ADD_FUNCTION("getDev", native_netmgr_get_dev, 1);
    AMP_ADD_FUNCTION("setAutoReconnect", native_netmgr_set_auto_reconnect, 2);
    AMP_ADD_FUNCTION("connect", native_netmgr_connect, 3);
    AMP_ADD_FUNCTION("disconnect", native_netmgr_disconnect, 1);
    AMP_ADD_FUNCTION("getState", native_netmgr_get_state, 1);
    AMP_ADD_FUNCTION("saveConfig", native_netmgr_save_config, 1);
    AMP_ADD_FUNCTION("getConfig", native_netmgr_get_config, 2);
    AMP_ADD_FUNCTION("delConfig", native_netmgr_del_config, 2);
    AMP_ADD_FUNCTION("setIfConfig", native_netmgr_set_ifconfig, 2);
    AMP_ADD_FUNCTION("getIfConfig", native_netmgr_get_ifconfig, 2);
    AMP_ADD_FUNCTION("setMsgCb", native_netmgr_set_msg_cb, 2);
    AMP_ADD_FUNCTION("delMsgCb", native_netmgr_del_msg_cb, 2);

    duk_put_prop_string(ctx, -2, "NETMGR");
}
