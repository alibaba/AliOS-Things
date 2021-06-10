/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "ota_agent.h"
#include "ota_import.h"
#include "module_aiot.h"
#include "app_upgrade.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"
#include "amp_task.h"

#define MOD_STR        "APP_OTA"

static ota_service_t customer_ota_ctx = {0};
static ota_store_module_info_t customer_module_info[3];
static aos_task_t user_module_ota_task = NULL;

typedef struct ota_package_info {
    int res;
    JSValue js_cb_ref;
    unsigned int length;
    char version[64];
    char module_name[64];
    int hash_type;
    char hash[64];
    char store_path[64];
    char install_path[64];
    char url[256];
} ota_package_info_t;

static JSClassID js_appota_class_id;

#ifndef countof
#define countof(x) (sizeof(x) / sizeof((x)[0]))
#endif

static void ota_release_notify(void *pdata)
{
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;
    if (!ota_package_info) {
        return;
    }
    JSContext *ctx = js_get_context();
    if (ctx) {
        JS_FreeValue(ctx, ota_package_info->js_cb_ref);
    }
    aos_free(ota_package_info);
}

static void ota_install_notify(void *pdata)
{
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;
    if (!ota_package_info) {
        amp_error(MOD_STR, "ota_package_info null!");
        return;
    }
    JSContext *ctx = js_get_context();
    if (!ctx) {
        amp_error(MOD_STR, "JSContext null!");
        return;
    }

    JSValue ret = JS_NewInt32(ctx, ota_package_info->res);
    JSValue v = JS_Call(ctx, ota_package_info->js_cb_ref, JS_UNDEFINED, 1, &ret);
    if (JS_IsException(v)) {
        amp_error(MOD_STR, "ota_install_notify callback error!");
    }
    JS_FreeValue(ctx, ret);
    JS_FreeValue(ctx, v);

    JS_FreeValue(ctx, ota_package_info->js_cb_ref);
    aos_free(ota_package_info);
}

static void ota_install_handler(void *pdata)
{
    amp_warn(MOD_STR, "ota_install_handler!");
    int res = -1;
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;
    /* clear jsengine timer, distory js app*/
    amp_module_free();
    app_js_stop();

    res = ota_install_jsapp(&customer_ota_ctx, ota_package_info->store_path, ota_package_info->length, ota_package_info->install_path);
    if (res < 0) {
        amp_error(MOD_STR, "module install failed!");
    } else {
        /*启动app.js*/
        res = ota_load_jsapp(&customer_ota_ctx);
        if(res < 0) {
            amp_error(MOD_STR, "module load failed!");
        }
    }

    ota_package_info->res = res;

    amp_task_schedule_call(ota_install_notify, ota_package_info);
    aos_task_exit(0);
}

static JSValue native_ota_upgrade(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int res = -1;
    aos_task_t ota_install_task;
    ota_package_info_t *ota_package_info = NULL;

    if (argc < 2 || !JS_IsObject(argv[0]) || !JS_IsFunction(ctx, argv[1])) {
        amp_warn(MOD_STR, "parameter must be (object and function)");
        goto out;
    }
    ota_package_info = aos_malloc(sizeof(ota_package_info_t));
    if (!ota_package_info) {
        amp_error(MOD_STR, "alloc device notify param fail");
        goto out;
    }

    JSValue l = JS_GetPropertyStr(ctx, argv[0], "length");
    JSValue sp = JS_GetPropertyStr(ctx, argv[0], "store_path");
    JSValue ip = JS_GetPropertyStr(ctx, argv[0], "install_path");
    unsigned int length = 0;
    JS_ToUint32(ctx, &length, l);
    const char* store_path = JS_ToCString(ctx, sp);
    const char* install_path = JS_ToCString(ctx, ip);

    ota_package_info->length = length;
    ota_package_info->js_cb_ref = JS_DupValue(ctx, argv[1]);
    strncpy(ota_package_info->store_path, store_path, sizeof(ota_package_info->store_path));
    strncpy(ota_package_info->install_path, install_path, sizeof(ota_package_info->install_path));

    res = aos_task_new_ext(&ota_install_task, "amp ota install task", ota_install_handler, ota_package_info, 1024 * 10, AOS_DEFAULT_APP_PRI);
    if (res != 0) {
        amp_warn(MOD_STR, "iot create task failed");
        JS_FreeValue(ctx, ota_package_info->js_cb_ref);
        aos_free(ota_package_info);
    }

    JS_FreeValue(ctx, l);
    JS_FreeValue(ctx, sp);
    JS_FreeValue(ctx, ip);
    JS_FreeCString(ctx, store_path);
    JS_FreeCString(ctx, install_path);
out:
    return JS_NewInt32(ctx, res);
}

static void ota_verify_notify(void *pdata)
{
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;
    if (!ota_package_info) {
        amp_error(MOD_STR, "ota_package_info null!");
        return;
    }
    JSContext *ctx = js_get_context();
    if (!ctx) {
        amp_error(MOD_STR, "JSContext null!");
        return;
    }

    JSValue ret = JS_NewInt32(ctx, ota_package_info->res);
    JSValue v = JS_Call(ctx, ota_package_info->js_cb_ref, JS_UNDEFINED, 1, &ret);
    if (JS_IsException(v)) {
        amp_error(MOD_STR, "ota_verify_notify callback error!");
    }
    JS_FreeValue(ctx, ret);
    JS_FreeValue(ctx, v);

    JS_FreeValue(ctx, ota_package_info->js_cb_ref);
    aos_free(ota_package_info);
}

static void ota_verify_handler(void *pdata)
{
    int res = -1;
    ota_boot_param_t ota_param = {0};
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;

    memset(&ota_param, 0, sizeof(ota_boot_param_t));

    ota_param.len = ota_package_info->length;
    ota_param.hash_type = ota_package_info->hash_type;
    strncpy(ota_param.hash, ota_package_info->hash, strlen(ota_package_info->hash));
    res = ota_verify_fsfile(&ota_param, ota_package_info->store_path);
    if (res < 0) {
        amp_error(MOD_STR, "amp jsota report ver failed!");
    }
    ota_package_info->res = res;
    amp_task_schedule_call(ota_verify_notify, ota_package_info);
    aos_task_exit(0);
}

static JSValue native_ota_verify(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int res = -1;
    aos_task_t ota_verify_task;
    ota_package_info_t *ota_package_info = NULL;

    if (argc < 2 || !JS_IsObject(argv[0]) || !JS_IsFunction(ctx, argv[1])) {
        amp_warn(MOD_STR, "parameter must be (object and function)");
        goto out;
    }
    ota_package_info = aos_malloc(sizeof(ota_package_info_t));
    if (!ota_package_info) {
        amp_error(MOD_STR, "alloc device notify param fail");
        goto out;
    }

    JSValue l = JS_GetPropertyStr(ctx, argv[0], "length");
    JSValue ht = JS_GetPropertyStr(ctx, argv[0], "hash_type");
    JSValue h = JS_GetPropertyStr(ctx, argv[0], "hash");
    JSValue sp = JS_GetPropertyStr(ctx, argv[0], "store_path");
    unsigned int length = 0;
    const char *hash_type_string = JS_ToCString(ctx, ht);
    const char *hash = JS_ToCString(ctx, h);
    const char *store_path = JS_ToCString(ctx, sp);
    JS_ToUint32(ctx, &length, l);
    ota_package_info->length = length;
    if (strcmp(hash_type_string, "null") == 0) {
        ota_package_info->hash_type = 0;
    } else if(strcmp(hash_type_string, "md5") == 0) {
        ota_package_info->hash_type = 2;
    } else if(strcmp(hash_type_string, "sha256") == 0) {
        ota_package_info->hash_type = 1;
    } else {
        ota_package_info->hash_type = 3;
    }
    ota_package_info->js_cb_ref = JS_DupValue(ctx, argv[1]);
    strncpy(ota_package_info->hash, hash, sizeof(ota_package_info->hash));
    strncpy(ota_package_info->store_path, store_path, sizeof(ota_package_info->store_path));

    res = aos_task_new_ext(&ota_verify_task, "amp ota verify task", ota_verify_handler, ota_package_info, 1024 * 10, AOS_DEFAULT_APP_PRI);
    if (res != 0) {
        amp_warn(MOD_STR, "iot create task failed");
        JS_FreeValue(ctx, ota_package_info->js_cb_ref);
        aos_free(ota_package_info);
    }

    JS_FreeValue(ctx, l);
    JS_FreeValue(ctx, ht);
    JS_FreeValue(ctx, h);
    JS_FreeValue(ctx, sp);
    JS_FreeCString(ctx, hash);
    JS_FreeCString(ctx, store_path);
    JS_FreeCString(ctx, hash_type_string);

out:
    return JS_NewInt32(ctx, res);
}

static void ota_download_notify(void *pdata)
{
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;
    if (!ota_package_info) {
        amp_error(MOD_STR, "ota_package_info null!");
        return;
    }
    JSContext *ctx = js_get_context();
    if (!ctx) {
        amp_error(MOD_STR, "JSContext null!");
        return;
    }

    JSValue ret = JS_NewInt32(ctx, ota_package_info->res);
    JSValue v = JS_Call(ctx, ota_package_info->js_cb_ref, JS_UNDEFINED, 1, &ret);
    if (JS_IsException(v)) {
        amp_error(MOD_STR, "ota_download_notify callback error!");
    }
    JS_FreeValue(ctx, ret);
    JS_FreeValue(ctx, v);

    JS_FreeValue(ctx, ota_package_info->js_cb_ref);
    aos_free(ota_package_info);
}

static void ota_download_handler(void *pdata)
{
    int res = -1;
    int js_cb_ref;
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;

    res = ota_download_store_fs_start(ota_package_info->url, strlen(ota_package_info->url), ota_package_info->store_path,
                                      customer_ota_ctx.report_func.report_status_cb, customer_ota_ctx.report_func.param);
    if (res < 0) {
        amp_error(MOD_STR, "amp jsota report ver failed!");
    }

    ota_package_info->res = res;

    amp_task_schedule_call(ota_download_notify, ota_package_info);
    aos_task_exit(0);
}

static JSValue native_ota_download(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int res = -1;
    aos_task_t ota_download_task;
    ota_package_info_t *ota_package_info = NULL;

    if (argc < 2 || !JS_IsObject(argv[0]) || !JS_IsFunction(ctx, argv[1])) {
        amp_warn(MOD_STR, "parameter must be (object and function)");
        goto out;
    }
    ota_package_info = aos_malloc(sizeof(ota_package_info_t));
    if (!ota_package_info) {
        amp_error(MOD_STR, "alloc device notify param fail");
        goto out;
    }

    /* get report info */
    JSValue u = JS_GetPropertyStr(ctx, argv[0], "url");
    JSValue sp = JS_GetPropertyStr(ctx, argv[0], "store_path");
    const char* url = JS_ToCString(ctx, u);
    const char* store_path = JS_ToCString(ctx, sp);
    ota_package_info->js_cb_ref = JS_DupValue(ctx, argv[1]);
    strncpy(ota_package_info->url, url, sizeof(ota_package_info->url));
    strncpy(ota_package_info->store_path, store_path, sizeof(ota_package_info->store_path));

    res = aos_task_new_ext(&ota_download_task, "amp ota download task", ota_download_handler, ota_package_info, 1024 * 10, AOS_DEFAULT_APP_PRI);
    if (res != 0) {
        amp_warn(MOD_STR, "iot create task failed");
        JS_FreeValue(ctx, ota_package_info->js_cb_ref);
        aos_free(ota_package_info);
    }

    JS_FreeValue(ctx, u);
    JS_FreeValue(ctx, sp);
    JS_FreeCString(ctx, url);
    JS_FreeCString(ctx, store_path);
out:
    return JS_NewInt32(ctx, res);
}

static JSValue native_ota_report(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int res = -1;
    iot_device_handle_t* dev_handle = NULL;
    if (argc < 1 || !JS_IsObject(argv[0])) {
        amp_warn(MOD_STR, "parameter must be (pointer and function)");
        goto out;
    }
    /* get report info */

    JSValue jd_handle = JS_GetPropertyStr(ctx, argv[0], "device_handle");
    JSValue jpk = JS_GetPropertyStr(ctx, argv[0], "product_key");
    JSValue jdn = JS_GetPropertyStr(ctx, argv[0], "device_name");
    JSValue jmn = JS_GetPropertyStr(ctx, argv[0], "module_name");
    JSValue jver = JS_GetPropertyStr(ctx, argv[0], "version");

    unsigned int tmp_ptr_addr = 0;
    tmp_ptr_addr = JS_GetOpaque(jd_handle, js_aiot_device_class_id);
    if (!tmp_ptr_addr) {
        amp_error(MOD_STR, "no iot_device_handle");
        goto out;
    }
    dev_handle = (iot_device_handle_t *)tmp_ptr_addr;
    amp_error(MOD_STR, "%s: handle 0x%x", __func__, (unsigned int)dev_handle);
    amp_error(MOD_STR, "%s: mqtthandle 0x%x", __func__, (unsigned int)dev_handle->mqtt_handle);
    const char* productkey = JS_ToCString(ctx, jpk);
    const char* devicename = JS_ToCString(ctx, jdn);
    const char* modulename = JS_ToCString(ctx, jmn);
    const char* version = JS_ToCString(ctx, jver);

    res = ota_transport_inform(dev_handle->mqtt_handle, productkey, devicename, modulename, version);
    if (res < 0) {
        amp_error(MOD_STR, "amp jsota report ver failed!");
    }
    JS_FreeCString(ctx, productkey);
    JS_FreeCString(ctx, devicename);
    JS_FreeCString(ctx, modulename);
    JS_FreeCString(ctx, version);

    JS_FreeValue(ctx, jd_handle);
    JS_FreeValue(ctx, jpk);
    JS_FreeValue(ctx, jdn);
    JS_FreeValue(ctx, jmn);
    JS_FreeValue(ctx, jver);
out:
    return JS_NewInt32(ctx, res);
}

static void ota_trigger_notify(void *pdata)
{
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;
    if (!ota_package_info) {
        amp_error(MOD_STR, "ota_package_info null!");
        return;
    }
    JSContext *ctx = js_get_context();
    if (!ctx) {
        amp_error(MOD_STR, "JSContext null!");
        return;
    }
    JSValue ret = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, ret, "length", JS_NewUint32(ctx, (unsigned int)ota_package_info->length));
    JS_SetPropertyStr(ctx, ret, "module_name", JS_NewString(ctx, ota_package_info->module_name));
    JS_SetPropertyStr(ctx, ret, "version", JS_NewString(ctx, ota_package_info->version));
    JS_SetPropertyStr(ctx, ret, "url", JS_NewString(ctx, ota_package_info->url));
    JS_SetPropertyStr(ctx, ret, "hash", JS_NewString(ctx, ota_package_info->hash));
    if (ota_package_info->hash_type == 0) {
        JS_SetPropertyStr(ctx, ret, "hash_type", JS_NewString(ctx, "null"));
    } else if (ota_package_info->hash_type == 1) {
        JS_SetPropertyStr(ctx, ret, "hash_type", JS_NewString(ctx, "sha256"));
    } else if (ota_package_info->hash_type == 2) {
        JS_SetPropertyStr(ctx, ret, "hash_type", JS_NewString(ctx, "md5"));
    } else {
        JS_SetPropertyStr(ctx, ret, "hash_type", JS_NewString(ctx, "sha512"));
    }
    JSValue v = JS_Call(ctx, ota_package_info->js_cb_ref, JS_UNDEFINED, 1, &ret);
    if (JS_IsException(v)) {
        amp_error(MOD_STR, "ota_trigger_notify callback error!");
    }
    JS_FreeValue(ctx, ret);
    JS_FreeValue(ctx, v);

    JS_FreeValue(ctx, ota_package_info->js_cb_ref);
    aos_free(ota_package_info);
}

/* system image upgrade */
static int32_t customer_upgrade_cb(void *pctx, char *ver, char *module_name, void *args)
{
    int32_t ret = OTA_TRANSPORT_PAR_FAIL;
    ota_package_info_t *ota_package_info = (ota_package_info_t *) args;
    ota_boot_param_t ota_param = {0};
    aos_task_t customer_ota_task;
    if ((pctx == NULL) || (ver == NULL) || (module_name == NULL) || (args == NULL)) {
        amp_error(MOD_STR, "amp:ota triggered param err!");
        return ret;
    }
    if (strncmp(module_name, "system", strlen(module_name)) == 0) {
        ret = 0;
        int current_ver[128] = {0};
        amp_app_version_get(current_ver);
        if (strncmp(ver, current_ver, strlen(ver)) <= 0) {
            ret = OTA_TRANSPORT_VER_FAIL;
            amp_error(MOD_STR, "amp ota version too old!");
        } else {
            amp_debug(MOD_STR, "ota version:%s is coming, begin to upgrade\n", ver);
            /* clear jsengine timer, distory js app*/
            if (aos_task_new_ext(&customer_ota_task, "amp_customer_ota", internal_sys_upgrade_start, (void *)pctx, 1024 * 8, AOS_DEFAULT_APP_PRI) != 0) {
                amp_debug(MOD_STR, "internal ota task create failed!");
                ret = OTA_TRANSPORT_PAR_FAIL;
            }
            amp_debug(MOD_STR, "app management center start");
        }
        amp_task_schedule_call(ota_release_notify, ota_package_info);
    } else {
        /*读取ota 触发时云端下发的文件信息*/
        ret = ota_read_parameter(&ota_param);
        if (ret < 0) {
            amp_error(MOD_STR, "get store ota param info failed\n");
            amp_task_schedule_call(ota_release_notify, ota_package_info);
        } else {
            ret = 0;
            ota_package_info->length = ota_param.len;
            ota_package_info->hash_type = ota_param.hash_type;
            strncpy(ota_package_info->url, ota_param.url, sizeof(ota_package_info->url));
            strncpy(ota_package_info->version, ver, strlen(ver));
            strncpy(ota_package_info->module_name, module_name, strlen(module_name));
            strncpy(ota_package_info->hash, ota_param.hash, sizeof(ota_package_info->hash));
            amp_task_schedule_call(ota_trigger_notify, ota_package_info);
        }
    }
    return ret;
}

static JSValue native_ota_init(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int res = -1;
    int productkey_len = IOTX_PRODUCT_KEY_LEN;
    int productsecret_len = IOTX_PRODUCT_SECRET_LEN;
    int devicename_len = IOTX_DEVICE_NAME_LEN;
    int devicesecret_len = IOTX_DEVICE_SECRET_LEN;
    iot_device_handle_t *iot_device_handle = NULL;
    ota_package_info_t *ota_package_info = NULL;
    int current_ver[128] = {0};
    if (argc < 2 || !JS_IsObject(argv[0]) || !JS_IsFunction(ctx, argv[1])) {
        amp_warn(MOD_STR, "parameter must be (object and function)");
        goto out;
    }
    ota_service_param_reset(&customer_ota_ctx);
    /* get device info */
    aos_kv_get(AMP_CUSTOMER_PRODUCTKEY, customer_ota_ctx.pk, &productkey_len);
    aos_kv_get(AMP_CUSTOMER_PRODUCTSECRET, customer_ota_ctx.ps, &productsecret_len);
    aos_kv_get(AMP_CUSTOMER_DEVICENAME, customer_ota_ctx.dn, &devicename_len);
    aos_kv_get(AMP_CUSTOMER_DEVICESECRET, customer_ota_ctx.ds, &devicesecret_len);

    memset(customer_module_info, 0x00, sizeof(customer_module_info));
    iot_device_handle = JS_GetOpaque(argv[0], js_aiot_device_class_id);
    if (!iot_device_handle) {
        amp_error(MOD_STR, "no iot_device_handle");
        goto out;
    }
    amp_error(MOD_STR, "%s: handle 0x%x", __func__, (unsigned int)iot_device_handle);
    amp_error(MOD_STR, "%s: mqtthandle 0x%x", __func__, (unsigned int)iot_device_handle->mqtt_handle);
    customer_ota_ctx.mqtt_client = (void *)iot_device_handle->mqtt_handle;

    ota_package_info = aos_malloc(sizeof(ota_package_info_t));
    if (!ota_package_info) {
        amp_error(MOD_STR, "alloc device notify param fail");
        goto out;
    }
    memset(ota_package_info, 0, sizeof(ota_package_info_t));
    ota_package_info->js_cb_ref = JS_DupValue(ctx, argv[1]);
    ota_register_module_store(&customer_ota_ctx, customer_module_info, 3);
    ota_register_trigger_msg_cb(&customer_ota_ctx, (void *)customer_upgrade_cb, ota_package_info);

    ota_set_module_information(&customer_ota_ctx, "system", OS_APP_PATH, OTA_UPGRADE_ALL);
    /* init ota service */
    res = ota_service_init(&customer_ota_ctx);
    if (res < 0) {
        amp_error(MOD_STR, "customer ota init failed!");
    } else {
        amp_warn(MOD_STR, "customer ota init success!");
    }
    amp_app_version_get(current_ver);
    res = ota_report_module_version(&customer_ota_ctx, "system", current_ver);
    if (res < 0) {
        amp_error(MOD_STR, "amp ota report ver failed!");
    }

out:
    if (res < 0) {
        // todo:ota_package_info->js_cb_ref 是否需要free?
        if (ota_package_info != NULL) {
            aos_free(ota_package_info);
        }
        return JS_EXCEPTION;
    }
    return JS_NewInt32(ctx, res);
}

static JSClassDef js_appota_class = {
    "APPOTA",
};

static const JSCFunctionListEntry js_appota_funcs[] = {
    JS_CFUNC_DEF("otaInit", 1, native_ota_init ),
    JS_CFUNC_DEF("otaDownload", 1, native_ota_download ),
    JS_CFUNC_DEF("otaVerify", 1, native_ota_verify ),
    JS_CFUNC_DEF("otaReport", 1, native_ota_report ),
    JS_CFUNC_DEF("otaUpgrade", 1, native_ota_upgrade ),
};

static int js_crypto_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_appota_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_appota_class_id, &js_appota_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_appota_funcs,
                               countof(js_appota_funcs));
    JS_SetClassProto(ctx, js_appota_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_appota_funcs,
                                  countof(js_appota_funcs));
}

JSModuleDef *js_init_module_appota(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_crypto_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_appota_funcs, countof(js_appota_funcs));
    return m;
}

void module_app_ota_register(void)
{
    amp_debug(MOD_STR, "module_ota_register");

    JSContext *ctx = js_get_context();

    amp_debug(MOD_STR, "module appota register");
    js_init_module_appota(ctx, "APPOTA");
}

