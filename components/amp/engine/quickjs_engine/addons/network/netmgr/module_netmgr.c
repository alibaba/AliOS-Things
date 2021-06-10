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
#include "aos/list.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR "module_netmgr"
#define CONNECT_WAIT_TIME_MS (10 * 1000)
#define CHECKIP_INTERVAL_MS 200

typedef JSValue js_cb_ref;
typedef struct msg_cb_info {
    slist_t next;
    JSValue cb_ref;
} msg_cb_info_t;

typedef struct wifi_connect_task_params {
    JSValue cb_ref;
    netmgr_hdl_t hdl;
} wifi_connect_task_params_t;

static JSClassID js_netmgr_class_id;
static slist_t g_msg_cb_list_head;
static int g_checkip_task_run_flag = 0;
static char dev_name[256];

static void js_cb_conn_status(void *pdata)
{
    int ret = -1;
    wifi_connect_task_params_t* params;
    netmgr_ifconfig_info_t info;
    netmgr_hdl_t hdl;
    JSValue cb_ref;
    uint32_t value = 0;

    if(pdata == NULL) {
        amp_debug(MOD_STR, "pdata is null");
        return ;
    }

    if(g_checkip_task_run_flag != 1) {
        return;
    }

    params = (wifi_connect_task_params_t*)pdata;
    hdl = params->hdl;
    ret = netmgr_get_ifconfig(hdl, &info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get ifconfig info failed");
    }

    JSContext *ctx = js_get_context();
    JSValue args;
    if (strcmp(info.ip_addr, "0.0.0.0") != 0)
        args = JS_NewString(ctx, "CONNECTED");
    else
        args = JS_NewString(ctx, "DISCONNECT");

    JSValue val = JS_Call(ctx, params->cb_ref, JS_UNDEFINED, 1, &args);
    JS_FreeValue(ctx, args);
    if (JS_IsException(val)){
        amp_info(MOD_STR,"netmgr connect callback error");
    }
    JS_FreeValue(ctx, val);
}

static void check_ip_task(void *arg)
{
    int ret = -1;
    int count = 0;
    netmgr_ifconfig_info_t info;
    wifi_connect_task_params_t* params;
    netmgr_hdl_t hdl;
    JSContext *ctx = js_get_context();

    if(arg == NULL) {
        amp_debug(MOD_STR, "check ip task arg is null");
        return ;
    }

    params = (wifi_connect_task_params_t*) arg;
    hdl = params->hdl;

    while (g_checkip_task_run_flag) {
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

    JS_FreeValue(ctx, params->cb_ref);
    return;
}

static JSValue native_netmgr_service_init(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
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
    return JS_NewInt32(ctx, ret);
}


static JSValue native_netmgr_service_deinit(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    netmgr_hdl_t hdl;
    int ret = -1;

    if (!JS_IsNumber(argv[0])) {
        amp_warn(MOD_STR, "parameter must be number\n");
        goto out;
    }
    JS_ToInt32(ctx, &hdl, argv[0]);

    netmgr_service_deinit(hdl);
    ret = 0;
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_netmgr_add_dev(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    char* name;
    int ret = -1;

    if (!JS_IsString(argv[0])) {
        amp_warn(MOD_STR, "parameter must be string\n");
        goto out;
    }

    name = JS_ToCString(ctx, argv[0]);

    ret = netmgr_add_dev(name);
    if (ret != 0) {
        amp_debug(MOD_STR, "netmgr add dev failed");
    }
    JS_FreeCString(ctx, name);
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_netmgr_get_dev(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    char* name;
    int ret = -1;

    if (!JS_IsString(argv[0])) {
        amp_warn(MOD_STR, "parameter must be string\n");
        goto out;
    }

    name = JS_ToCString(ctx, argv[0]);

    if( name != NULL ) {
        memset(dev_name, 0, sizeof(dev_name));
        strcpy(dev_name, name);
    }

    ret = netmgr_get_dev(name);
    if (ret != 0) {
        amp_debug(MOD_STR, "netmgr get %s dev failed\n", name);
    }
    JS_FreeCString(ctx, name);
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_netmgr_set_auto_reconnect(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;
    netmgr_hdl_t hdl;
    int enable;

    if (!JS_IsNumber(argv[0]) || !JS_IsNumber(argv[1])) {
        amp_warn(MOD_STR, "parameter must be number and number\n");
        goto out;
    }
    JS_ToInt32(ctx, &hdl, argv[0]);
    JS_ToInt32(ctx, &enable, argv[1]);

    if(enable == 1) {
        netmgr_set_auto_reconnect(hdl, true);
        ret = 0;
    } else if(enable == 0) {
        netmgr_set_auto_reconnect(hdl, false);
        ret = 0;
    }
out:
    return JS_NewInt32(ctx, ret);
}

static wifi_connect_task_params_t connect_task_params;
static JSValue native_netmgr_connect(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;
    aos_task_t netmgr_connect_task;
    netmgr_hdl_t hdl;
    netmgr_connect_params_t params;
    char *ssid = NULL;
    char *password = NULL;
    char *bssid = NULL;
    int timeout_ms = 0;

    if (!JS_IsNumber(argv[0]) || !JS_IsObject(argv[1])
            || !JS_IsFunction(ctx, argv[2])) {
        amp_warn(MOD_STR, "parameter must be number, object and function\n");
        goto out;
    }

    JS_ToInt32(ctx, &hdl, argv[0]);

    JSValue js_ssid = JS_GetPropertyStr(ctx, argv[1], "ssid");
    if(!JS_IsString(js_ssid)){
        amp_error(MOD_STR, "request ssid is invalid");
        goto out;
    }
    ssid =  JS_ToCString(ctx, js_ssid);
    JS_FreeValue(ctx, js_ssid);

    JSValue js_password = JS_GetPropertyStr(ctx, argv[1], "password");
    if(!JS_IsString(js_password)){
        amp_error(MOD_STR, "request password is invalid");
        goto out;
    }
    password =  JS_ToCString(ctx, js_password);
    JS_FreeValue(ctx, js_password);

    JSValue js_bssid = JS_GetPropertyStr(ctx, argv[1], "bssid");
    if(!JS_IsString(js_bssid)){
        amp_error(MOD_STR, "request bssid is invalid");
        goto out;
    }
    bssid =  JS_ToCString(ctx, js_bssid);
    JS_FreeValue(ctx, js_bssid);

    JSValue js_timeout_ms = JS_GetPropertyStr(ctx, argv[1], "timeout_ms");
    if(!JS_IsString(js_timeout_ms)){
        JS_ToInt32(ctx, &timeout_ms, js_timeout_ms);
    }
    JS_FreeValue(ctx, js_timeout_ms);

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

    JSValue cb_ref = argv[2];
    if (!JS_IsFunction(ctx, cb_ref)) {
        return JS_ThrowTypeError(ctx, "not a function");
    }

    memset(&connect_task_params, 0, sizeof(connect_task_params));
    connect_task_params.cb_ref = JS_DupValue(ctx, cb_ref);
    connect_task_params.hdl = hdl;

    g_checkip_task_run_flag = 1;

    ret = aos_task_new_ext(&netmgr_connect_task, "netmgr connect task", check_ip_task, (void *)&connect_task_params, 1024 * 2, ADDON_TSK_PRIORRITY);
    if (ret != 0) {
        amp_warn(MOD_STR, "jse_osal_create_task failed\n");
        JS_FreeValue(ctx, cb_ref);
    }
out:
    if (ssid != NULL) {
        JS_FreeCString(ctx, ssid);
    }
    if (password != NULL) {
        JS_FreeCString(ctx, password);
    }
    if (bssid != NULL) {
        JS_FreeCString(ctx, bssid);
    }

    return JS_NewInt32(ctx, ret);
}

/*****************************************************************************
 * Function:    native_netmgr_disconnect
 * Description: js native addon for WIFI.getssid()
 * Called by:   js api
 * Input:       no input
 * Output:      return a string object to js api what the mac is
 *****************************************************************************/
static JSValue native_netmgr_disconnect(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;
    netmgr_hdl_t hdl;

    if (!JS_IsNumber(argv[0])) {
        amp_warn(MOD_STR, "parameter must be number\n");
        goto out;
    }
    JS_ToInt32(ctx, &hdl, argv[0]);

    ret = netmgr_disconnect(hdl);
    if (ret != 0) {
        amp_debug(MOD_STR, "netmgr disconnect failed");
        goto out;
    }
out:
    return JS_NewInt32(ctx, ret);
}

/*****************************************************************************
 * Function:    native_netmgr_get_state
 * Description: js native addon for NETMGR.getState()
 * Called by:   js api
 * Input:       no input
 * Output:      return a string object to js api what the mac is
 *****************************************************************************/
static JSValue native_netmgr_get_state(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;
    netmgr_hdl_t hdl;

    if (!JS_IsNumber(argv[0])) {
        amp_warn(MOD_STR, "parameter must be number\n");
        goto out;
    }
    JS_ToInt32(ctx, &hdl, argv[0]);
    ret = netmgr_get_state(hdl);
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_netmgr_save_config(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;
    netmgr_hdl_t hdl;

    if (!JS_IsNumber(argv[0])) {
        amp_warn(MOD_STR, "parameter must be number\n");
        goto out;
    }
    JS_ToInt32(ctx, &hdl, argv[0]);

    ret = netmgr_save_config(hdl);
    if (ret != 0) {
        amp_debug(MOD_STR, "netmgr get state failed");
        goto out;
    }
out:
    return JS_NewInt32(ctx, ret);
}
static JSValue native_netmgr_get_config(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;
    //TODO
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_netmgr_del_config(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;
    //TODO
out:
    return JS_NewInt32(ctx, ret);
}

/*****************************************************************************
 * Function:    native_netmgr_set_ifconfig
 * Description: js native addon for NETMGR.setIconfig()
 * Called by:   js api
 * Input:       no input
 * Output:      return a string object to js api,if it is NULL means can't get
 *ip
 *****************************************************************************/
static JSValue native_netmgr_set_ifconfig(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;
    netmgr_hdl_t hdl;
    netmgr_ifconfig_info_t info;
    char *ip_addr = NULL;
    char *mask    = NULL;
    char *gw      = NULL;
    char *dns_server = NULL;
    char *mac      = NULL;

    if (!JS_IsNumber(argv[0]) || !JS_IsObject(argv[0])) {
        amp_warn(MOD_STR, "parameter must be number and object\n");
        goto out;
    }
    JS_ToInt32(ctx, &hdl, argv[0]);

    /* get device certificate */
    info.dhcp_en = QUICKJS_GET_PROPERTY_BOOL(ctx, argv[1], "dhcp_en");
    ip_addr    = QUICKJS_GET_PROPERTY_STR(ctx, argv[1], "ip_addr");
    mask       = QUICKJS_GET_PROPERTY_STR(ctx, argv[1], "mask");
    gw         = QUICKJS_GET_PROPERTY_STR(ctx, argv[1], "gw");
    dns_server = QUICKJS_GET_PROPERTY_STR(ctx, argv[1], "dns_server");
    mac        = QUICKJS_GET_PROPERTY_STR(ctx, argv[1], "mac");

    memcpy(info.ip_addr, ip_addr, strlen(ip_addr));
    memcpy(info.mask, mask, strlen(mask));
    memcpy(info.gw, gw, strlen(gw));
    memcpy(info.dns_server, dns_server, strlen(dns_server));
    memcpy(info.mac, mac, strlen(mac));

    ret = netmgr_set_ifconfig(hdl, &info);
    if (ret != 0) {
        amp_debug(MOD_STR, "set ifconfig failed");
    }

out:
    if (ip_addr != NULL) {
        JS_FreeCString(ctx, ip_addr);
    }
    if (mask != NULL) {
        JS_FreeCString(ctx, mask);
    }
    if (gw != NULL) {
        JS_FreeCString(ctx, gw);
    }
    if (dns_server != NULL) {
        JS_FreeCString(ctx, dns_server);
    }
    if (mac != NULL) {
        JS_FreeCString(ctx, mac);
    }
    return JS_NewInt32(ctx, ret);
}

/*****************************************************************************
 * Function:    native_netmgr_get_ifconfig
 * Description: js native addon for NETMGR.getIfconfig()
 * Called by:   js api
 * Input:       no input
 * Output:      return a string object to js api,if it is NULL means can't get
 *ip
 *****************************************************************************/
static JSValue native_netmgr_get_ifconfig(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;
    netmgr_hdl_t hdl;
    netmgr_ifconfig_info_t info;
    JSValue obj = JS_NewObject(ctx);

    if (!JS_IsNumber(argv[0])) {
        amp_warn(MOD_STR, "parameter must be number and object\n");
        goto out;
    }

    JS_ToInt32(ctx, &hdl, argv[0]);

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

    JS_SetPropertyStr(ctx, obj, "dhcp_en", JS_NewInt32(ctx, dhcp_en));
    JS_SetPropertyStr(ctx, obj, "ip_addr", JS_NewString(ctx, ip_addr));
    JS_SetPropertyStr(ctx, obj, "mask", JS_NewString(ctx, mask));
    JS_SetPropertyStr(ctx, obj, "gw", JS_NewString(ctx, gw));
    JS_SetPropertyStr(ctx, obj, "dns_server", JS_NewString(ctx, dns_server));
    JS_SetPropertyStr(ctx, obj, "mac", JS_NewString(ctx, mac));
    JS_SetPropertyStr(ctx, obj, "rssi", JS_NewInt32(ctx, rssi));
out:
    return obj;
}

static int set_msg_cb(netmgr_msg_t* msg)
{
    msg_cb_info_t* info;
    JSContext *ctx = js_get_context();
    JSValue args;

    slist_for_each_entry(&g_msg_cb_list_head, info, msg_cb_info_t, next) {
        switch(msg->id) {
            case NETMGR_MSGID_WIFI_STATUS:
                JS_SetPropertyStr(ctx, args, "msgid", JS_NewInt32(ctx, msg->id));
                JS_SetPropertyStr(ctx, args, "data", JS_NewString(ctx, msg->data.network_status_change));
                break;
            case NETMGR_MSGID_WIFI_STATUS_FROM_IMPL:
                JS_SetPropertyStr(ctx, args, "msgid", JS_NewInt32(ctx, msg->id));
                JS_SetPropertyStr(ctx, args, "data", JS_NewString(ctx, msg->data.status));
                break;
            case NETMGR_MSGID_WIFI_TRACE_FROM_IMPL:
            case NETMGR_MSGID_NETWORK_STATUS:
            case NETMGR_MSGID_ETH_STATUS_FROM_IMPL:
            default:
                return -1;
        }
        JSValue val = JS_Call(ctx, info->cb_ref, JS_UNDEFINED, 1, &args);
        JS_FreeValue(ctx, args);
        if (JS_IsException(val)){
            amp_info(MOD_STR,"netmgr connect callback error");
        }
        JS_FreeValue(ctx, val);
    }
    return 0;
}

static JSValue native_netmgr_set_msg_cb(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    netmgr_hdl_t hdl;
    int ret = -1;
    JSValue msg_cb_ref;
    msg_cb_info_t *info;

    if (!JS_IsNumber(argv[0]) || !JS_IsFunction(ctx, argv[1])) {
        amp_warn(MOD_STR, "parameter must be number and function\n");
        goto out;
    }
    JS_ToInt32(ctx, &hdl, argv[0]);

    JSValue msg_cb = argv[1];
    if (!JS_IsFunction(ctx, msg_cb)) {
        return JS_ThrowTypeError(ctx, "not a function");
    }
    msg_cb_ref = JS_DupValue(ctx, msg_cb);

    info = (msg_cb_info_t*)aos_malloc(sizeof(msg_cb_info_t));
    if(info != NULL) {
        if(slist_empty(&g_msg_cb_list_head)) {
            ret = netmgr_set_msg_cb(hdl, set_msg_cb);
        }
        memset(info, 0, sizeof(msg_cb_info_t));
        info->cb_ref = msg_cb_ref;
        slist_add(&(info->next), &g_msg_cb_list_head);
        ret = 0;
    }
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_netmgr_del_msg_cb(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    netmgr_hdl_t hdl;
    msg_cb_info_t *info;
    JSValue msg_cb_ref;
    int ret = -1;

    if (!JS_IsNumber(argv[0]) || !JS_IsFunction(ctx, argv[1])) {
        amp_warn(MOD_STR, "parameter must be number and object\n");
        goto out;
    }
    JS_ToInt32(ctx, &hdl, argv[0]);

    JSValue msg_cb = argv[1];
    if (!JS_IsFunction(ctx, msg_cb)) {
        return JS_ThrowTypeError(ctx, "not a function");
    }

    slist_for_each_entry(&g_msg_cb_list_head, info, msg_cb_info_t, next) {
        if(JS_VALUE_GET_PTR(msg_cb) == JS_VALUE_GET_PTR(info->cb_ref)) {
            slist_del(&(info->next), &g_msg_cb_list_head);
            ret = 0;
            aos_free(info);
            JS_FreeValue(ctx, msg_cb);
            break;
        }
    }
    if(slist_empty(&g_msg_cb_list_head)) {
        ret = netmgr_del_msg_cb(hdl, set_msg_cb);
    }
out:
    return JS_NewInt32(ctx, ret);
}

static void module_netmgr_source_clean(void)
{
    JSContext *ctx = js_get_context();
    netmgr_hdl_t hdl;

    g_checkip_task_run_flag = 0;
    aos_msleep(CHECKIP_INTERVAL_MS);

    hdl = netmgr_get_dev(dev_name);
    netmgr_disconnect(hdl);
    //netmgr_wifi_deinit(hdl);
    netmgr_service_deinit();
    memset(dev_name, 0, sizeof(dev_name));
    amp_debug(MOD_STR, "module_netmgr_register deinit");
}

static JSClassDef js_netmgr_class = {
    "NETMGR",
};

static const JSCFunctionListEntry js_netmgr_funcs[] = {

    JS_CFUNC_DEF("serviceInit", 0, native_netmgr_service_init),
    JS_CFUNC_DEF("serviceDeinit", 1, native_netmgr_service_deinit),
    JS_CFUNC_DEF("addDev", 1, native_netmgr_add_dev),
    JS_CFUNC_DEF("getDev", 1, native_netmgr_get_dev),
    JS_CFUNC_DEF("setAutoReconnect", 2, native_netmgr_set_auto_reconnect),
    JS_CFUNC_DEF("connect", 3, native_netmgr_connect),
    JS_CFUNC_DEF("disconnect", 1, native_netmgr_disconnect),
    JS_CFUNC_DEF("getState", 1, native_netmgr_get_state),
    JS_CFUNC_DEF("saveConfig", 1, native_netmgr_save_config),
    JS_CFUNC_DEF("getConfig", 2, native_netmgr_get_config),
    JS_CFUNC_DEF("delConfig", 2, native_netmgr_del_config),
    JS_CFUNC_DEF("setIfConfig", 2, native_netmgr_set_ifconfig),
    JS_CFUNC_DEF("getIfConfig", 2, native_netmgr_get_ifconfig),
    JS_CFUNC_DEF("setMsgCb", 2, native_netmgr_set_msg_cb),
    JS_CFUNC_DEF("delMsgCb", 2, native_netmgr_del_msg_cb)

};

static int js_netmgr_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_netmgr_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_netmgr_class_id, &js_netmgr_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_netmgr_funcs, countof(js_netmgr_funcs));
    JS_SetClassProto(ctx, js_netmgr_class_id, proto);
    return JS_SetModuleExportList(ctx, m, js_netmgr_funcs, countof(js_netmgr_funcs));
}

JSModuleDef *js_init_module_netmgr(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_netmgr_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_netmgr_funcs, countof(js_netmgr_funcs));
    return m;
}

void module_netmgr_register(void)
{
    amp_debug(MOD_STR, "module_netmgr_register");
    JSContext *ctx = js_get_context();

    amp_module_free_register(module_netmgr_source_clean);
    js_init_module_netmgr(ctx, "NETMGR");
}
