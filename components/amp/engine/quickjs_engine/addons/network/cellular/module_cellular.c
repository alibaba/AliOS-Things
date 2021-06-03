/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "aos_network.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR "CELLULAR"

typedef struct {
    int status;
    JSValue js_cb_ref;
} network_status_notify_param_t;

static JSValue g_js_cb_ref;
static JSClassID js_cellular_class_id;

/*************************************************************************************
 * Function:    native_cellular_get_simInfo
 * Description: js native addon for
 *            UDP.close(sock_id)
 * Called by:   js api
 * Input:       sock_id: interger
 *
 * Output:      return 0 when UDP.close call ok
 *            return error number UDP.close call fail
 **************************************************************************************/
static JSValue native_cellular_get_simInfo(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret     = -1;
    amp_sim_info_t sim_info;
    JSValue obj = JS_NewObject(ctx);

    memset(&sim_info, 0, sizeof(sim_info));

    ret = amp_get_sim_info(&sim_info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get sim card info failed");
        goto out;
    }

    JS_SetPropertyStr(ctx, obj, "imsi", JS_NewString(ctx, sim_info.imsi));
    JS_SetPropertyStr(ctx, obj, "imei", JS_NewString(ctx, sim_info.imei));
    JS_SetPropertyStr(ctx, obj, "iccid", JS_NewString(ctx, sim_info.iccid));
out:
    return obj;
}

/*************************************************************************************
 * Function:    native_aiot_close
 * Description: js native addon for
 *            UDP.close(sock_id)
 * Called by:   js api
 * Input:       sock_id: interger
 *
 * Output:      return 0 when UDP.close call ok
 *            return error number UDP.close call fail
 **************************************************************************************/
static JSValue native_cellular_get_locatorInfo(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret     = -1;
    amp_locator_info_t locator_info;
    JSValue obj = JS_NewObject(ctx);

    memset(&locator_info, 0, sizeof(locator_info));

    ret = amp_get_locator_info(&locator_info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get locator card info failed");
        goto out;
    }

    JS_SetPropertyStr(ctx, obj, "cellid", JS_NewInt32(ctx, locator_info.cellid));
    JS_SetPropertyStr(ctx, obj, "lac", JS_NewInt32(ctx, locator_info.lac));
    JS_SetPropertyStr(ctx, obj, "mcc", JS_NewString(ctx, locator_info.mcc));
    JS_SetPropertyStr(ctx, obj, "mnc", JS_NewString(ctx, locator_info.mnc));
    JS_SetPropertyStr(ctx, obj, "signal", JS_NewInt32(ctx, locator_info.signal));
out:
    return obj;
}

/*************************************************************************************
 * Function:    native_aiot_close
 * Description: js native addon for
 *            UDP.close(sock_id)
 * Called by:   js api
 * Input:       sock_id: interger
 *
 * Output:      return 0 when UDP.close call ok
 *            return error number UDP.close call fail
 **************************************************************************************/
void cellInfo_receive_callback(amp_locator_info_t *locator_info, int cell_num)
{
    int ret     = -1;
    int i;
    JSContext *ctx = js_get_context();
    JSValue args = JS_NewArray(ctx);

    for (i = 0; i < cell_num; i++) {
        JSValue obj = JS_NewObject(ctx);

        JS_SetPropertyStr(ctx, obj, "cellid", JS_NewInt32(ctx, locator_info[i].cellid));
        JS_SetPropertyStr(ctx, obj, "lac", JS_NewInt32(ctx, locator_info[i].lac));
        JS_SetPropertyStr(ctx, obj, "mcc", JS_NewString(ctx, locator_info[i].mcc));
        JS_SetPropertyStr(ctx, obj, "mnc", JS_NewString(ctx, locator_info[i].mnc));

        JS_SetPropertyUint32(ctx, args, i, obj);
    }

    JSValue val = JS_Call(ctx, g_js_cb_ref, JS_UNDEFINED, 1, &args);
    JS_FreeValue(ctx, args);
    if (JS_IsException(val)) {
        amp_info(MOD_STR,"cellinfo callback error");
    }
}

static JSValue native_cellular_neighborCellInfo(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret     = -1;

    ret = amp_get_neighbor_locator_info(cellInfo_receive_callback);
    if (ret != 0) {
        amp_debug(MOD_STR, "get locator card info failed");
        goto out;
    }

out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_cellular_getStatus(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret     = -1;

    ret = aos_get_network_status();
    if (ret != 1) {
        amp_debug(MOD_STR, "network status disconnect %d", ret);
        goto out;
    }

out:
    return JS_NewInt32(ctx, ret);
}

static void network_status_notify(void *pdata)
{
    int i                      = 0;
    network_status_notify_param_t *p = (network_status_notify_param_t *)pdata;
    JSContext *ctx  = js_get_context();
    JSValue status =  JS_NewInt32(ctx, p->status);

    JSValue val = JS_Call(ctx, p->js_cb_ref, JS_UNDEFINED, 1, &status);
    JS_FreeValue(ctx, status);
    if (JS_IsException(val)) {
        amp_info(MOD_STR,"cellinfo callback error");
    }
}

static void network_status_callback(int status, void *args)
{
    JSValue js_cb_ref = (JSValue)args;
    network_status_notify_param_t *p = aos_calloc(1, sizeof(network_status_notify_param_t));
    if (!p) {
        amp_warn(MOD_STR, "allocate memory failed");
        JSContext *ctx = be_get_context();
        JS_FreeValue(ctx, js_cb_ref);
        return;
    }

    p->status = status;
    p->js_cb_ref = js_cb_ref;

    amp_task_schedule_call(network_status_notify, p);
}

static JSValue native_cellular_onconnect(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret     = -1;
    JSContext *ctx = js_get_context();
    JSValue js_cb_ref;

    if (!JS_IsFunction(ctx, argv[0])) {
        amp_warn(MOD_STR, "parameter must be function");
        goto out;
    }

    JSValue on_cb = argv[0];
    if (!JS_IsFunction(ctx, on_cb)) {
        return JS_ThrowTypeError(ctx, "not a function");
    }
    js_cb_ref = JS_DupValue(ctx, on_cb);

    ret = aos_network_status_registercb(network_status_callback, js_cb_ref);
    if (ret != 0) {
        JS_FreeValue(ctx, js_cb_ref);
        return JS_NewInt32(ctx, ret);
    }

out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_get_netshare_mode(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;

    ret = amp_get_netsharemode();

out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_set_netshare_mode(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;
    int share_mode = 0;

    if (!JS_IsNumber(argv[0])) {
        amp_warn(MOD_STR, "parameter must be number");
        goto out;
    }

    JS_ToInt32(ctx, &share_mode, argv[0]);
    amp_error(MOD_STR, "native set net share mode = %d", share_mode);
    ret = amp_set_netsharemode(share_mode);
    if (ret != 0) {
        return JS_NewInt32(ctx, ret);
    }
    amp_error(MOD_STR, "native set net share mode success");
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_get_netshare_config(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret     = -1;
    amp_sharemode_info_t *share_mode_info;
    JSValue obj = JS_NewObject(ctx);

    share_mode_info = aos_malloc(sizeof(amp_sharemode_info_t));
    if (share_mode_info == NULL) {
        amp_debug(MOD_STR, "get net share config failed");
        goto out;
    }
    memset(share_mode_info, 0, sizeof(amp_sharemode_info_t));

    ret = amp_get_netshareconfig(share_mode_info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get net share config failed");
        goto out;
    }

    JS_SetPropertyStr(ctx, obj, "action", JS_NewInt32(ctx, share_mode_info->action));
    JS_SetPropertyStr(ctx, obj, "auto_connect", JS_NewInt32(ctx, share_mode_info->auto_connect));
    JS_SetPropertyStr(ctx, obj, "apn", JS_NewString(ctx, share_mode_info->apn));
    JS_SetPropertyStr(ctx, obj, "username", JS_NewString(ctx, share_mode_info->username));
    JS_SetPropertyStr(ctx, obj, "password", JS_NewString(ctx, share_mode_info->password));
    JS_SetPropertyStr(ctx, obj, "ip_type", JS_NewInt32(ctx, share_mode_info->ip_type));
    JS_SetPropertyStr(ctx, obj, "share_mode", JS_NewInt32(ctx, share_mode_info->share_mode));

    aos_free(share_mode_info);
    return obj;

out:
    if (share_mode_info) {
        aos_free(share_mode_info);
    }

    return JS_NewInt32(ctx, 0);
}

static JSValue native_set_netshare_config(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret     = -1;
    amp_sharemode_info_t *share_mode_info;
    JSValue obj = JS_NewObject(ctx);

    /* check paramters */
    if (!JS_IsObject(argv[0]))
    {
        amp_warn(MOD_STR, "parameter must be object\n");
        goto out;
    }

    const uint16_t ipType = QUICKJS_GET_PROPERTY_INT32(ctx, argv[0], "ipType");
    const uint16_t authType = QUICKJS_GET_PROPERTY_INT32(ctx, argv[0], "authType");
    const char *password = QUICKJS_GET_PROPERTY_STR(ctx, argv[0], "password");
    const char *username = QUICKJS_GET_PROPERTY_STR(ctx, argv[0], "username");
    const char *apn = QUICKJS_GET_PROPERTY_STR(ctx, argv[0], "apn");
    const uint16_t autoConnect = QUICKJS_GET_PROPERTY_INT32(ctx, argv[0], "autoConnect");
    const uint16_t action = QUICKJS_GET_PROPERTY_INT32(ctx, argv[0], "action");
    const uint16_t ucid = QUICKJS_GET_PROPERTY_INT32(ctx, argv[0], "ucid");

    share_mode_info = aos_malloc(sizeof(amp_sharemode_info_t));
    if (share_mode_info == NULL) {
        amp_debug(MOD_STR, "set net share config failed");
        goto out;
    }
    memset(share_mode_info, 0, sizeof(amp_sharemode_info_t));

    share_mode_info->action = action;
    share_mode_info->auto_connect = autoConnect;
    memcpy(share_mode_info->apn, apn, strlen(apn));
    memcpy(share_mode_info->username, username, strlen(username));
    memcpy(share_mode_info->password, password, strlen(password));
    share_mode_info->ip_type = ipType;

    JS_FreeCString(ctx, password);
    JS_FreeCString(ctx, username);
    JS_FreeCString(ctx, apn);

    ret = amp_set_netshareconfig(ucid, authType, share_mode_info);
    if (ret != 0) {
        amp_warn(MOD_STR, "amp set net share config failed!");
        aos_free(share_mode_info);
        return JS_NewInt32(ctx, 0);
    }

out:
    if (share_mode_info) {
        aos_free(share_mode_info);
    }

    return JS_NewInt32(ctx, 0);
}

static JSClassDef js_cellular_class = {
    "CELLULAR",
};

static const JSCFunctionListEntry js_cellular_funcs[] = {
    JS_CFUNC_DEF("getSimInfo",          0, native_cellular_get_simInfo),
    JS_CFUNC_DEF("getLocatorInfo",      0, native_cellular_get_locatorInfo),
    JS_CFUNC_DEF("getStatus",           0, native_cellular_getStatus),
    JS_CFUNC_DEF("onConnect",           1, native_cellular_onconnect),
    JS_CFUNC_DEF("getNeighborCellInfo", 0, native_cellular_neighborCellInfo),
    JS_CFUNC_DEF("getNetSharemode",     0, native_get_netshare_mode),
    JS_CFUNC_DEF("setNetSharemode",     1, native_set_netshare_mode),
    JS_CFUNC_DEF("getNetShareconfig",   0, native_get_netshare_config),
    JS_CFUNC_DEF("setNetShareconfig",   1, native_set_netshare_config)
};

static int js_cellular_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_cellular_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_cellular_class_id, &js_cellular_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_cellular_funcs, countof(js_cellular_funcs));
    JS_SetClassProto(ctx, js_cellular_class_id, proto);
    return JS_SetModuleExportList(ctx, m, js_cellular_funcs, countof(js_cellular_funcs));
}

JSModuleDef *js_init_module_cellular(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_cellular_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_cellular_funcs, countof(js_cellular_funcs));
    return m;
}

void module_cellular_register(void)
{
    amp_debug(MOD_STR, "module_cellular_register");
    JSContext *ctx = js_get_context();
    js_init_module_netmgr(ctx, "CELLULAR");
}

