/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_platform.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "be_inl.h"
#include "aiot_state_api.h"
#include "bt_host_adapter.h"
#include "cJSON.h"

#define MOD_STR "BT_HOST"

typedef struct {
    int scan_js_cb_ref;
    int conn_js_cb_ref;
    int gatts_js_cb_ref;
}module_bt_host_info;

module_bt_host_info g_module_bt_host_info;

static module_bt_host_info * module_bt_host_get_info(void)
{
    return &g_module_bt_host_info;
}

static void module_bt_host_clean(void)
{

}

static duk_ret_t native_bt_host_init(duk_context *ctx)
{
    int ret = -1;
    amp_bt_host_adapter_init_t *adapter_init;
    char *deviceName;
    int32_t conn_num_max;

    duk_get_prop_string(ctx, 0, "deviceName");
    duk_get_prop_string(ctx, 0, "conn_num_max");

    deviceName = duk_get_string(ctx, -2);
    conn_num_max = duk_get_number(ctx, -1);

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
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_bt_host_start_adv(duk_context *ctx)
{
    int ret = -1;
    int32_t type, interval_min, interval_max, channel_map;
    char *adv_data, *scan_rsp_data;
    amp_bt_host_adapter_adv_start_t *adapter_start_adv;
    module_bt_host_info * info = module_bt_host_get_info();

    amp_debug(MOD_STR, "%s enter", __func__);

    duk_get_prop_string(ctx, 0, "type");
    duk_get_prop_string(ctx, 0, "adv_data");
    duk_get_prop_string(ctx, 0, "scan_rsp_data");
    duk_get_prop_string(ctx, 0, "interval_min");
    duk_get_prop_string(ctx, 0, "interval_max");
    duk_get_prop_string(ctx, 0, "channel_map");

    type = duk_get_number(ctx, -6);
    adv_data = duk_get_string(ctx, -5);
    scan_rsp_data = duk_get_string(ctx, -4);
    interval_min = duk_get_number(ctx, -3);
    interval_max = duk_get_number(ctx, -2);
    channel_map = duk_get_number(ctx, -1);
    amp_debug(MOD_STR, "%s: type = %d, ad = %s, sd = %s, interval_min = %d, interval_max = %d, channel_map = %d", __func__, type, adv_data, scan_rsp_data, interval_min,
            interval_max, channel_map);

    duk_dup(ctx, 1);
    info->conn_js_cb_ref = be_ref(ctx);

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
    duk_push_int(ctx, ret);
    return 1;

}

static duk_ret_t native_bt_host_stop_adv(duk_context *ctx)
{
    int ret = -1;

    ret = bt_host_adapter_stop_adv();
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_bt_host_start_scan(duk_context *ctx)
{
    int ret = -1;
    int32_t type, interval, window;
    amp_bt_host_adapter_scan_start_t *adapter_start_scan;
    module_bt_host_info * info = module_bt_host_get_info();

    amp_debug(MOD_STR, "%s enter", __func__);

    duk_get_prop_string(ctx, 0, "type");
    duk_get_prop_string(ctx, 0, "interval");
    duk_get_prop_string(ctx, 0, "window");

    type = duk_get_number(ctx, -3);
    interval = duk_get_number(ctx, -2);
    window = duk_get_number(ctx, -1);

    duk_dup(ctx, 1);
    info->scan_js_cb_ref = be_ref(ctx);
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
    duk_push_int(ctx, ret);
    return 1;

}

static duk_ret_t native_bt_host_stop_scan(duk_context *ctx)
{
    int ret = -1;

    ret = bt_host_adapter_stop_scan();
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_bt_host_add_service(duk_context *ctx)
{
    int ret = -1, chars_num = 0;
    char *attr_uuid = NULL, *attr_permit = NULL, *service_cfg = NULL;
    cJSON* service_obj = NULL, *chars_list_obj = NULL, *descr_obj = NULL, *elemet = NULL;

    amp_bt_host_adapter_gatts_srvc_t service = {0};
    amp_bt_host_adapter_gatt_chars_t *chars_list;
    module_bt_host_info * info = module_bt_host_get_info();

    amp_debug(MOD_STR, "%s enter", __func__);

    service_cfg = duk_to_string(ctx, 0);
#if 1
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
        duk_dup(ctx, 1);
        info->gatts_js_cb_ref = be_ref(ctx);
    }
#endif
    duk_push_int(ctx, ret?0:-1);
    return 1;
}

static duk_ret_t native_bt_host_update_chars(duk_context *ctx)
{
    size_t vals_len = 0;
    int ret = -1, index = 0;
    uint8_t *vals_data = NULL;
    char *arg_str = NULL, *uuid = NULL;
    cJSON *root = NULL, *arry = NULL, *elemet = NULL;
    module_bt_host_info * info = module_bt_host_get_info();

    amp_debug(MOD_STR, "%s enter with top %d", __func__, duk_get_top(ctx));
    arg_str = duk_get_string(ctx, 0);

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

    duk_push_int(ctx, ret); // 0: success, -1: fail

    return 1;
}

void native_bt_host_conn_handle(int32_t conn_handle, int32_t connect)
{
    module_bt_host_info * info = module_bt_host_get_info();

    if (info->conn_js_cb_ref) {
        duk_context *ctx = be_get_context();
        be_push_ref(ctx, info->conn_js_cb_ref);
        duk_push_int(ctx, conn_handle);
        duk_push_int(ctx, connect);
        if (duk_pcall(ctx, 2) != DUK_EXEC_SUCCESS) {
            amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
        }
        duk_pop(ctx);
        duk_gc(ctx, 0);
    }
}

void native_bt_host_scan_handle(char *addr, int32_t addr_type, int32_t adv_type, char *adv_data, int32_t rssi)
{
    module_bt_host_info * info = module_bt_host_get_info();

    if (info->scan_js_cb_ref) {
        amp_error(MOD_STR, "scan cb %x\n", info->scan_js_cb_ref);
        duk_context *ctx = be_get_context();
        be_push_ref(ctx, info->scan_js_cb_ref);
        duk_push_string(ctx, addr);
        duk_push_int(ctx, addr_type);
        duk_push_int(ctx, adv_type);
        duk_push_string(ctx, adv_data);
        duk_push_int(ctx, rssi);
        if (duk_pcall(ctx, 5) != DUK_EXEC_SUCCESS) {
            amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
        }
        duk_pop(ctx);
        duk_gc(ctx, 0);
    }
}

void native_bt_host_gatts_handle_write(uint8_t data[], size_t len)
{
    module_bt_host_info * info = module_bt_host_get_info();
    amp_debug(MOD_STR, "+++[%s][%d]+++", __func__, __LINE__);
    if (info->gatts_js_cb_ref) {
        duk_context *ctx = be_get_context();
        amp_debug(MOD_STR, "+++[%s][%d]+++", __func__, __LINE__);
        be_push_ref(ctx, info->gatts_js_cb_ref);
        amp_debug(MOD_STR, "+++[%s][%d]+++", __func__, __LINE__);
        duk_idx_t arr_idx = duk_push_array(ctx);
        for (int i = 0; i < len; i++) {
            duk_push_int(ctx, data[i]);
            duk_put_prop_index(ctx, arr_idx, i);
        }
        amp_debug(MOD_STR, "+++[%s][%d]+++", __func__, __LINE__);
        if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
            amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
        }
        amp_debug(MOD_STR, "+++[%s][%d]+++", __func__, __LINE__);
        duk_pop(ctx);
        duk_gc(ctx, 0);
    }
}

void module_bt_host_register(void)
{
    duk_context *ctx = be_get_context();

    amp_module_free_register(module_bt_host_clean);

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("init",        native_bt_host_init, 1);
    AMP_ADD_FUNCTION("start_adv",   native_bt_host_start_adv, 2);
    AMP_ADD_FUNCTION("stop_adv",    native_bt_host_stop_adv, 0);
    AMP_ADD_FUNCTION("start_scan",  native_bt_host_start_scan, 2);
    AMP_ADD_FUNCTION("stop_scan",   native_bt_host_stop_scan, 0);
    AMP_ADD_FUNCTION("add_service", native_bt_host_add_service, 2);
    AMP_ADD_FUNCTION("update_char", native_bt_host_update_chars, 1);

    amp_debug(MOD_STR, "bt_host registered");
    duk_put_prop_string(ctx, -2, "BT_HOST");
}

