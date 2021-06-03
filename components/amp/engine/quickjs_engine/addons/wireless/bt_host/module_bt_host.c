/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "amp_platform.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "aiot_state_api.h"
#include "bt_host_adapter.h"
#include "cJSON.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"
#include "quickjs-libc.h"

#define MOD_STR "BT_HOST"

typedef struct {
    JSValue scan_js_cb_ref;
    JSValue conn_js_cb_ref;
    JSValue gatts_js_cb_ref;
}module_bt_host_info;

typedef struct {
    char addr[19];
    int32_t addr_type;
    int32_t adv_type;
    char adv_data[63];
    int32_t rssi;
}scan_js_cb_param;

typedef struct {
    int32_t conn_handle;
    int32_t connect;
}conn_js_cb_param;

#define GATTS_MAX_DATA_LEN  128
typedef struct {
    int32_t len;
    uint8_t data[GATTS_MAX_DATA_LEN*2+1];
}gatts_js_cb_param;

static JSClassID js_bt_host_class_id;

module_bt_host_info g_module_bt_host_info;

extern JSContext *js_get_context(void);
extern void js_std_dump_error(JSContext *ctx);
int bt_host_adapter_bin2hex(char *hex, uint8_t *buf, uint8_t buf_len);

static module_bt_host_info * module_bt_host_get_info(void)
{
    return &g_module_bt_host_info;
}

static void module_bt_host_clean(void)
{

}

static JSValue native_bt_host_init(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int ret = -1;
    amp_bt_host_adapter_init_t *adapter_init;
    char *deviceName;
    int32_t conn_num_max;
    JSValue JS_deviceName, JS_conn_num_max;

    JS_deviceName = JS_GetPropertyStr(ctx, argv[0], "deviceName");
    JS_conn_num_max = JS_GetPropertyStr(ctx, argv[0],"conn_num_max");

    deviceName = (char *)JS_ToCString(ctx, JS_deviceName);
    JS_ToInt32(ctx, &conn_num_max, JS_conn_num_max);

    amp_debug(MOD_STR, "%s: deviceName = %s, conn_num_max = %d", __func__, deviceName, conn_num_max);

    adapter_init = (amp_bt_host_adapter_init_t *)aos_malloc(sizeof(amp_bt_host_adapter_init_t));
    if (!adapter_init) {
        amp_error(MOD_STR, "allocate memory failed\n");
        goto out;
    }

    adapter_init->dev_name = deviceName;
    adapter_init->conn_num_max = conn_num_max;
    ret = bt_host_adapter_init(adapter_init);
    amp_debug(MOD_STR, "%s: init ret = %d", __func__, ret);
    aos_free(adapter_init);

out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_bt_host_start_adv(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int ret = -1;
    int32_t type, interval_min, interval_max, channel_map;
    char *adv_data, *scan_rsp_data;
    amp_bt_host_adapter_adv_start_t *adapter_start_adv;
    module_bt_host_info * info = module_bt_host_get_info();
    JSValue JStype, JSadv_data, JSscan_rsp_data, JSinterval_min, JSinterval_max, JSchannel_map, JS_cb;

    amp_debug(MOD_STR, "%s enter", __func__);

    JStype = JS_GetPropertyStr(ctx, argv[0], "type");
    JSadv_data = JS_GetPropertyStr(ctx, argv[0], "adv_data");
    JSscan_rsp_data = JS_GetPropertyStr(ctx, argv[0], "scan_rsp_data");
    JSinterval_min = JS_GetPropertyStr(ctx, argv[0], "interval_min");
    JSinterval_max = JS_GetPropertyStr(ctx, argv[0], "interval_max");
    JSchannel_map = JS_GetPropertyStr(ctx, argv[0], "channel_map");

    JS_ToInt32(ctx, &type, JStype);
    adv_data = (char *)JS_ToCString(ctx, JSadv_data);
    scan_rsp_data = (char *)JS_ToCString(ctx, JSscan_rsp_data);
    JS_ToInt32(ctx, &interval_min, JSinterval_min);
    JS_ToInt32(ctx, &interval_max, JSinterval_max);
    JS_ToInt32(ctx, &channel_map, JSchannel_map);
    amp_debug(MOD_STR, "%s: type = %d, ad = %s, sd = %s, interval_min = %d, interval_max = %d, channel_map = %d", __func__, type, adv_data, scan_rsp_data, interval_min,
            interval_max, channel_map);

    JS_cb = argv[1];
    if (!JS_IsFunction(ctx, JS_cb)) {
        amp_warn(MOD_STR, "native_bt_host_start_adv, not a fun");
        return JS_ThrowTypeError(ctx, "not a function");
    }
    info->conn_js_cb_ref = JS_DupValue(ctx, JS_cb);

    adapter_start_adv = (amp_bt_host_adapter_adv_start_t *)aos_malloc(sizeof(amp_bt_host_adapter_adv_start_t));
    if (!adapter_start_adv) {
        amp_error(MOD_STR, "allocate memory failed\n");
        goto out;
    }

    adapter_start_adv->type = type;
    adapter_start_adv->adv_data = adv_data;
    adapter_start_adv->scan_rsp_data = scan_rsp_data;
    adapter_start_adv->interval_min = interval_min;
    adapter_start_adv->interval_max = interval_max;
    adapter_start_adv->channel_map = channel_map;

    ret = bt_host_adapter_start_adv(adapter_start_adv);
    aos_free(adapter_start_adv);
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_bt_host_stop_adv(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int ret = -1;

    ret = bt_host_adapter_stop_adv();
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_bt_host_start_scan(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int ret = -1;
    int32_t type, interval, window;
    amp_bt_host_adapter_scan_start_t *adapter_start_scan;
    module_bt_host_info * info = module_bt_host_get_info();
    JSValue JStype, JSinterval, JSwindow, JS_cb;

    amp_debug(MOD_STR, "%s enter", __func__);

    JStype = JS_GetPropertyStr(ctx, argv[0], "type");
    JSinterval = JS_GetPropertyStr(ctx, argv[0], "interval");
    JSwindow = JS_GetPropertyStr(ctx, argv[0], "window");

    JS_ToInt32(ctx, &type, JStype);
    JS_ToInt32(ctx, &interval, JSinterval);
    JS_ToInt32(ctx, &window, JSwindow);

    JS_cb = argv[1];
    if (!JS_IsFunction(ctx, JS_cb)) {
        amp_warn(MOD_STR, "native_bt_host_start_scan, not a fun");
        return JS_ThrowTypeError(ctx, "not a function");
    }
    info->scan_js_cb_ref = JS_DupValue(ctx, JS_cb);

    amp_debug(MOD_STR, "%s: type = %d, interval = %d, window = %d, channel_map = %d", __func__, type, interval, window);

    adapter_start_scan = (amp_bt_host_adapter_scan_start_t *)aos_malloc(sizeof(amp_bt_host_adapter_scan_start_t));
    if (!adapter_start_scan) {
        amp_error(MOD_STR, "allocate memory failed\n");
        goto out;
    }

    adapter_start_scan->type = type;
    adapter_start_scan->interval = interval;
    adapter_start_scan->window = window;

    ret = bt_host_adapter_start_scan(adapter_start_scan);
    aos_free(adapter_start_scan);
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_bt_host_stop_scan(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int ret = -1;

    ret = bt_host_adapter_stop_scan();
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_bt_host_add_service(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int ret = -1, chars_num = 0;
    char *attr_uuid = NULL, *attr_permit = NULL, *service_cfg = NULL;
    cJSON* service_obj = NULL, *chars_list_obj = NULL, *descr_obj = NULL, *elemet = NULL;
    amp_bt_host_adapter_gatts_srvc_t service = {0};
    amp_bt_host_adapter_gatt_chars_t *chars_list;
    module_bt_host_info * info = module_bt_host_get_info();
    JSValue JS_cb;

    amp_debug(MOD_STR, "%s enter", __func__);

    service_cfg = (char *)JS_ToCString(ctx, argv[0]);
    JS_cb = argv[1];
    if (!JS_IsFunction(ctx, JS_cb)) {
        amp_warn(MOD_STR, "native_bt_host_add_service, not a fun");
        return JS_ThrowTypeError(ctx, "not a function");
    }
    info->gatts_js_cb_ref = JS_DupValue(ctx, JS_cb);

    /* eg.
    {
        "s_uuid":"0x1A1A",
        "chars_list":[
            {
                "char_uuid":"0x1B1B",
                "char_permit":"RW",
                "char_descr":{
                    "descr_type":"CCC",
                    "descr_uuid":"0x1C1C"
                }
            },
            {
                "char_uuid":"0x1D1D",
                "char_permit":"R"
            }
        ]
    }
    */
    if (service_cfg) {
        amp_debug(MOD_STR, "[%s] service_cfg: %s", __func__, service_cfg);
        service_obj = cJSON_Parse(service_cfg);
        if (service_obj != NULL && cJSON_IsObject(service_obj)) {
            service.s_uuid = cJSON_GetStringValue(cJSON_GetObjectItem(service_obj, "s_uuid"));
            service.attr_cnt ++; // for service declaration
            if (service.s_uuid) {
                chars_list_obj = cJSON_GetObjectItem(service_obj, "chars_list");
                if (chars_list_obj && cJSON_IsArray(chars_list_obj)) {
                    chars_num = cJSON_GetArraySize(chars_list_obj);
                    if (chars_num) {
                        chars_list = service.chars = (amp_bt_host_adapter_gatt_chars_t *)aos_malloc(
                                                      chars_num * sizeof(amp_bt_host_adapter_gatt_chars_t));
                        if (chars_list) {
                            memset(chars_list, 0, chars_num * sizeof(amp_bt_host_adapter_gatt_chars_t));
                            cJSON_ArrayForEach(elemet, chars_list_obj) {
                                chars_list->char_uuid  = cJSON_GetStringValue(cJSON_GetObjectItem(elemet, "char_uuid"));
                                chars_list->permission = cJSON_GetStringValue(cJSON_GetObjectItem(elemet, "char_permit"));
                                descr_obj = cJSON_GetObjectItem(elemet, "char_descr");
                                if (descr_obj) {
                                    chars_list->descr_uuid = cJSON_GetStringValue(cJSON_GetObjectItem(descr_obj, "descr_uuid"));
                                    chars_list->descr_type = cJSON_GetStringValue(cJSON_GetObjectItem(descr_obj, "descr_type"));
                                    service.attr_cnt += 3;
                                } else {
                                    service.attr_cnt += 2;
                                }
                                chars_list ++;
                            }
                            ret = bt_gatts_adapter_add_service(&service);
                            aos_free(service.chars);
                        } else {
                            amp_error(MOD_STR, "[%s] memory not enough for chars_list", __func__);
                        }
                    } else {
                        amp_error(MOD_STR, "[%s] the number of characteristic is invalid", __func__);
                    }
                }
            }
            cJSON_Delete(service_obj);
        } else {
            amp_error(MOD_STR, "[%s] failed to parse service_cfg to json object", __func__);
        }
    } else {
        amp_error(MOD_STR, "[%s] service_cfg is null", __func__);
    }

    if (ret) { // success
        amp_debug(MOD_STR, "[%s] add service success", __func__);
    }

    return JS_NewInt32(ctx, ret?0:-1);
}

static JSValue native_bt_host_update_chars(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    size_t vals_len = 0;
    int ret = -1, index = 0;
    uint8_t *vals_data = NULL;
    char *arg_str = NULL, *uuid = NULL;
    cJSON *root = NULL, *arry = NULL, *elemet = NULL;
    module_bt_host_info * info = module_bt_host_get_info();

    amp_debug(MOD_STR, "%s enter", __func__);
    arg_str = (char *)JS_ToCString(ctx, argv[0]);

    amp_debug(MOD_STR, "arg str = %s", arg_str);
    //{"uuid":0x1000, "value":[1, 2, 3, 4]}
    if (arg_str) {
        root = cJSON_Parse(arg_str);
        if (root && cJSON_IsObject(root)) {
            uuid = cJSON_GetStringValue(cJSON_GetObjectItem(root, "uuid"));
            arry = cJSON_GetObjectItem(root, "value");
            if (arry && cJSON_IsArray(arry)) {
                vals_len = cJSON_GetArraySize(arry);
                vals_data = (uint8_t *)malloc(vals_len * sizeof(uint8_t));
                if (vals_data) {
                    memset(vals_data, 0, vals_len * sizeof(uint8_t));
                    cJSON_ArrayForEach(elemet, arry) {
                        vals_data[index++] = elemet->valueint;
                    }
                    ret = bt_gatts_adapter_update_chars(uuid, vals_data, vals_len);
                    free(vals_data);
                }
            }
            cJSON_Delete(root);
        }
    }

    return JS_NewInt32(ctx, ret); // 0: success, -1: fail
}

void native_bt_host_conn_handle_js(void *param)
{
    module_bt_host_info * info = module_bt_host_get_info();
    JSContext *ctx = js_get_context();
    conn_js_cb_param *cb_param;

    cb_param = (conn_js_cb_param *)param;
    if (info->conn_js_cb_ref) {
        JSValue val;
        JSValue args[2];

        if (!JS_IsFunction(ctx, info->conn_js_cb_ref)) {
            amp_warn(MOD_STR, "native_bt_host_conn_handle, not a fun");
        }

        amp_debug(MOD_STR, "native_bt_host_conn_handle");
        args[0] = JS_NewInt32(ctx, cb_param->conn_handle);
        args[1] = JS_NewInt32(ctx, cb_param->connect);
        val = JS_Call(ctx, info->conn_js_cb_ref, JS_UNDEFINED, 2, args);
        JS_FreeValue(ctx, args[0]);
        JS_FreeValue(ctx, args[1]);
        aos_free(cb_param);
        if (JS_IsException(val)) {
            js_std_dump_error(ctx);
        }
        JS_FreeValue(ctx, val);
    }
}

void native_bt_host_conn_handle(int32_t conn_handle, int32_t connect)
{
    module_bt_host_info * info = module_bt_host_get_info();
    JSContext *ctx = js_get_context();
    conn_js_cb_param *cb_param;

    if (info->conn_js_cb_ref) {
        conn_js_cb_param *cb_param;
        cb_param = aos_malloc(sizeof(conn_js_cb_param));
        if (!cb_param)
            return;

        cb_param->conn_handle = conn_handle;
        cb_param->connect = connect;

        amp_task_schedule_call(native_bt_host_conn_handle_js, cb_param);
    }
}

void native_bt_host_scan_handle_js(void *param)
{
    module_bt_host_info * info = module_bt_host_get_info();
    JSContext *ctx = js_get_context();
    scan_js_cb_param *cb_param;

    cb_param = (scan_js_cb_param *)param;
    if (info->scan_js_cb_ref) {
        JSValue val;
        JSValue args[5];

        amp_debug(MOD_STR, "native_bt_host_scan_handle");

        if (!JS_IsFunction(ctx, info->scan_js_cb_ref)) {
            amp_warn(MOD_STR, "native_bt_host_scan_handle, not a fun");
        }

        args[0] = JS_NewString(ctx, cb_param->addr);
        args[1] = JS_NewInt32(ctx, cb_param->addr_type);
        args[2] = JS_NewInt32(ctx, cb_param->adv_type);
        args[3] = JS_NewString(ctx, cb_param->adv_data);
        args[4] = JS_NewInt32(ctx, cb_param->rssi);
        val = JS_Call(ctx, info->scan_js_cb_ref, JS_UNDEFINED, 5, args);
        JS_FreeValue(ctx, args[0]);
        JS_FreeValue(ctx, args[1]);
        JS_FreeValue(ctx, args[2]);
        JS_FreeValue(ctx, args[3]);
        JS_FreeValue(ctx, args[4]);
        aos_free(cb_param);
        if (JS_IsException(val)) {
            js_std_dump_error(ctx);
        }
        JS_FreeValue(ctx, val);
    }
}

void native_bt_host_scan_handle(char *addr, int32_t addr_type, int32_t adv_type, char *adv_data, int32_t rssi)
{
    module_bt_host_info * info = module_bt_host_get_info();

    if (info->scan_js_cb_ref) {
        scan_js_cb_param *cb_param;
        cb_param = aos_malloc(sizeof(scan_js_cb_param));
        if (!cb_param)
            return;

        strcpy(cb_param->addr, addr);
        cb_param->addr_type = addr_type;
        cb_param->adv_type = adv_type;
        strcpy(cb_param->adv_data, adv_data);
        cb_param->rssi = rssi;

        amp_task_schedule_call(native_bt_host_scan_handle_js, cb_param);
    }
}

void native_bt_host_gatts_handle_write_js(void *param)
{
    module_bt_host_info * info = module_bt_host_get_info();
    JSContext *ctx = js_get_context();
    gatts_js_cb_param *cb_param;

    cb_param = (gatts_js_cb_param *)param;
    if (info->gatts_js_cb_ref) {
        JSValue val;
        JSValue args[2];

        amp_debug(MOD_STR, "native_bt_host_gatts_handle_write_js");

        if (!JS_IsFunction(ctx, info->gatts_js_cb_ref)) {
            amp_warn(MOD_STR, "native_bt_host_gatts_handle_write, not a fun");
        }

        args[0] = JS_NewInt32(ctx, cb_param->len);
        args[1] = JS_NewString(ctx, cb_param->data);
        val = JS_Call(ctx, info->gatts_js_cb_ref, JS_UNDEFINED, 2, args);
        JS_FreeValue(ctx, args[0]);
        JS_FreeValue(ctx, args[1]);

        aos_free(cb_param);
        if (JS_IsException(val)) {
            js_std_dump_error(ctx);
        }
        JS_FreeValue(ctx, val);
    }
}

void native_bt_host_gatts_handle_write(uint8_t data[], size_t len)
{
    module_bt_host_info * info = module_bt_host_get_info();

    if (len > GATTS_MAX_DATA_LEN) {
        amp_error(MOD_STR, "data too long");
        return;
    }

    if (info->gatts_js_cb_ref) {
        gatts_js_cb_param *cb_param;
        cb_param = aos_malloc(sizeof(gatts_js_cb_param));
        if (!cb_param)
            return;

        cb_param->len = len;
        bt_host_adapter_bin2hex(cb_param->data, data, len);
        amp_task_schedule_call(native_bt_host_gatts_handle_write_js, cb_param);
    }
}

static JSClassDef js_bt_host_class = {
    "BT_HOST",
};

static const JSCFunctionListEntry js_bt_host_funcs[] = {
    JS_CFUNC_DEF("init", 1, native_bt_host_init ),
    JS_CFUNC_DEF("start_adv", 2, native_bt_host_start_adv ),
    JS_CFUNC_DEF("stop_adv", 0, native_bt_host_stop_adv ),
    JS_CFUNC_DEF("start_scan", 2, native_bt_host_start_scan),
    JS_CFUNC_DEF("stop_scan", 0, native_bt_host_stop_scan),
    JS_CFUNC_DEF("add_service", 2, native_bt_host_add_service),
    JS_CFUNC_DEF("update_chars", 1, native_bt_host_update_chars),
};

static int js_bt_host_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_bt_host_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_bt_host_class_id, &js_bt_host_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_bt_host_funcs,
                               countof(js_bt_host_funcs));
    JS_SetClassProto(ctx, js_bt_host_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_bt_host_funcs,
                                  countof(js_bt_host_funcs));
}

JSModuleDef *js_init_module_bt_host(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_bt_host_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_bt_host_funcs, countof(js_bt_host_funcs));
    return m;
}

void module_bt_host_register(void)
{
    amp_debug(MOD_STR, "module_bt_host_register");
    JSContext *ctx = js_get_context();

    amp_module_free_register(module_bt_host_clean);

    js_init_module_bt_host(ctx, "BT_HOST");
}

