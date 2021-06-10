/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "be_inl.h"
#include "ota_agent.h"
#include "ota_import.h"
#include "module_aiot.h"
#include "app_upgrade.h"

#define MOD_STR        "APP_OTA"

static ota_service_t customer_ota_ctx = {0};
static ota_store_module_info_t customer_module_info[3];
static aos_task_t user_module_ota_task = NULL;

typedef struct ota_package_info {
    int res;
    int js_cb_ref;
    unsigned int length;
    char version[64];
    char module_name[64];
    int hash_type;
    char hash[64];
    char store_path[64];
    char install_path[64];
    char url[256];
} ota_package_info_t;

static void ota_install_notify(void *pdata)
{
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, ota_package_info->js_cb_ref);

    duk_push_int(ctx, ota_package_info->res);
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);

    duk_gc(ctx, 0);

    aos_free(ota_package_info);
}

static void ota_install_handler(void *pdata)
{
    int res = -1;
    int js_cb_ref;
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

static duk_ret_t native_ota_upgrade(duk_context *ctx)
{
    int res = -1;
    int js_cb_ref;
    aos_task_t ota_install_task;
    ota_package_info_t *ota_package_info = NULL;

    if (!duk_is_object(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be (pointer and function)");
        goto out;
    }

    /* get verify info */
    duk_get_prop_string(ctx, 0, "length");
    duk_get_prop_string(ctx, 0, "store_path");
    duk_get_prop_string(ctx, 0, "install_path");

    unsigned int length = duk_get_int(ctx, -3);
    const char *store_path = duk_get_string(ctx, -2);
    const char *install_path = duk_get_string(ctx, -1);

    duk_dup(ctx, 1);
    js_cb_ref = be_ref(ctx);

    ota_package_info = aos_malloc(sizeof(ota_package_info_t));
    if (!ota_package_info) {
        amp_error(MOD_STR, "alloc device notify param fail");
        return;
    }
    memset(ota_package_info, 0x00, sizeof(ota_package_info_t));
    ota_package_info->length = length;
    ota_package_info->js_cb_ref = js_cb_ref;
    strncpy(ota_package_info->store_path, store_path, sizeof(ota_package_info->store_path));
    strncpy(ota_package_info->install_path, install_path, sizeof(ota_package_info->install_path));

    res = aos_task_new_ext(&ota_install_task, "amp ota install task", ota_install_handler, ota_package_info, 1024 * 10, AOS_DEFAULT_APP_PRI);
    if (res != 0) {
        amp_warn(MOD_STR, "iot create task failed");
        aos_free(ota_package_info);
        goto out;
    }
out:
    duk_push_int(ctx, res);
    return 1;
}

static void ota_verify_notify(void *pdata)
{
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, ota_package_info->js_cb_ref);

    duk_push_int(ctx, ota_package_info->res);
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);

    duk_gc(ctx, 0);

    aos_free(ota_package_info);
}

static void ota_verify_handler(void *pdata)
{
    int res = -1;
    int js_cb_ref;
    ota_boot_param_t ota_param = {0};
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;

    memset(&ota_param, 0, sizeof(ota_boot_param_t));

    ota_param.len = ota_package_info->length;
    ota_param.hash_type = ota_package_info->hash_type;
    strncpy(ota_param.hash, ota_package_info->hash, strlen(ota_package_info->hash));
    res = ota_verify_fsfile(&ota_param, ota_package_info->store_path);
    if (res < 0) {
        amp_error(MOD_STR, "amp jsota verified failed!");
    }
    ota_package_info->res = res;
    amp_task_schedule_call(ota_verify_notify, ota_package_info);
    aos_task_exit(0);
}

static duk_ret_t native_ota_verify(duk_context *ctx)
{
    int res = -1;
    int js_cb_ref;
    aos_task_t ota_verify_task;
    ota_package_info_t *ota_package_info = NULL;

    if (!duk_is_object(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be (pointer and function)");
        goto out;
    }

    /* get verify info */
    duk_get_prop_string(ctx, 0, "length");
    duk_get_prop_string(ctx, 0, "hash_type");
    duk_get_prop_string(ctx, 0, "hash");
    duk_get_prop_string(ctx, 0, "store_path");

    unsigned int length = duk_get_int(ctx, -4);
    const char *hash_type = duk_get_string(ctx, -3);
    const char *hash = duk_get_string(ctx, -2);
    const char *store_path = duk_get_string(ctx, -1);

    duk_dup(ctx, 1);
    js_cb_ref = be_ref(ctx);

    ota_package_info = aos_malloc(sizeof(ota_package_info_t));
    if (!ota_package_info) {
        amp_error(MOD_STR, "alloc device notify param fail");
        return;
    }
    memset(ota_package_info, 0x00, sizeof(ota_package_info_t));
    ota_package_info->length = length;
    ota_package_info->js_cb_ref = js_cb_ref;
    if (strcmp(hash_type, "null") == 0) {
        ota_package_info->hash_type = 0;
    } else if(strcmp(hash_type, "md5") == 0) {
        ota_package_info->hash_type = 2;
    } else if(strcmp(hash_type, "sha256") == 0) {
        ota_package_info->hash_type = 1;
    } else {
        ota_package_info->hash_type = 3;
    }
    strncpy(ota_package_info->hash, hash, sizeof(ota_package_info->hash));
    strncpy(ota_package_info->store_path, store_path, sizeof(ota_package_info->store_path));

    res = aos_task_new_ext(&ota_verify_task, "amp ota verify task", ota_verify_handler, ota_package_info, 1024 * 10, AOS_DEFAULT_APP_PRI);
    if (res != 0) {
        amp_warn(MOD_STR, "iot create task failed");
        aos_free(ota_package_info);
        goto out;
    }
out:
    duk_push_int(ctx, res);
    return 1;
}

static void ota_download_notify(void *pdata)
{
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, ota_package_info->js_cb_ref);
    duk_push_int(ctx, ota_package_info->res);
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);

    duk_gc(ctx, 0);
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
        amp_error(MOD_STR, "amp jsota download file failed!");
    }

    ota_package_info->res = res;
    amp_task_schedule_call(ota_download_notify, ota_package_info);
    aos_task_exit(0);
}

static duk_ret_t native_ota_download(duk_context *ctx)
{
    int res = -1;
    int js_cb_ref;
    aos_task_t ota_download_task;
    ota_package_info_t *ota_package_info = NULL;

    if (!duk_is_object(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be (pointer and function)");
        goto out;
    }

    /* get report info */
    duk_get_prop_string(ctx, 0, "url");
    duk_get_prop_string(ctx, 0, "store_path");

    const char *url = duk_get_string(ctx, -2);
    const char *store_path = duk_get_string(ctx, -1);

    duk_dup(ctx, 1);
    js_cb_ref = be_ref(ctx);

    ota_package_info = aos_malloc(sizeof(ota_package_info_t));
    if (!ota_package_info) {
        amp_error(MOD_STR, "alloc device notify param fail");
        return;
    }
    memset(ota_package_info, 0x00, sizeof(ota_package_info_t));
    ota_package_info->js_cb_ref = js_cb_ref;
    strncpy(ota_package_info->url, url, sizeof(ota_package_info->url));
    strncpy(ota_package_info->store_path, store_path, sizeof(ota_package_info->store_path));

    res = aos_task_new_ext(&ota_download_task, "amp ota download task", ota_download_handler, ota_package_info, 1024 * 10, AOS_DEFAULT_APP_PRI);
    if (res != 0) {
        amp_warn(MOD_STR, "iot create task failed");
        aos_free(ota_package_info);
        goto out;
    }
out:
    duk_push_int(ctx, res);
    return 1;
}

static duk_ret_t native_ota_report(duk_context *ctx)
{
    int res = -1;
    int js_cb_ref;
    iot_device_handle_t *iot_device_handle = NULL;

    if (!duk_is_object(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be (pointer and function)");
        goto out;
    }

    /* get report info */
    duk_get_prop_string(ctx, 0, "device_handle");
    duk_get_prop_string(ctx, 0, "product_key");
    duk_get_prop_string(ctx, 0, "device_name");
    duk_get_prop_string(ctx, 0, "module_name");
    duk_get_prop_string(ctx, 0, "version");

    iot_device_handle = duk_get_pointer(ctx, -5);
    const char *productkey = duk_get_string(ctx, -4);
    const char *devicename = duk_get_string(ctx, -3);
    const char *modulename = duk_get_string(ctx, -2);
    const char *ver = duk_get_string(ctx, -1);

    duk_dup(ctx, 1);
    js_cb_ref = be_ref(ctx);
    amp_warn(MOD_STR, "js report ver!");
    res = ota_transport_inform(iot_device_handle->mqtt_handle, productkey, devicename, modulename, ver);
    if (res < 0) {
        amp_error(MOD_STR, "amp jsota report ver failed!");
    }

out:
    duk_push_int(ctx, res);
    return 1;
}

static void ota_trigger_notify(void *pdata)
{
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, ota_package_info->js_cb_ref);

    duk_push_object(ctx);
    duk_push_int(ctx, (unsigned int)ota_package_info->length);
    duk_put_prop_string(ctx, -2, "length");
    duk_push_string(ctx, ota_package_info->module_name);
    duk_put_prop_string(ctx, -2, "module_name");
    duk_push_string(ctx, ota_package_info->version);
    duk_put_prop_string(ctx, -2, "version");
    duk_push_string(ctx, ota_package_info->url);
    duk_put_prop_string(ctx, -2, "url");
    duk_push_string(ctx, ota_package_info->hash);
    duk_put_prop_string(ctx, -2, "hash");
    if (ota_package_info->hash_type == 0) {
        duk_push_string(ctx, "null");
    } else if (ota_package_info->hash_type == 1) {
        duk_push_string(ctx, "sha256");
    } else if (ota_package_info->hash_type == 2) {
        duk_push_string(ctx, "md5");
    } else {
        duk_push_string(ctx, "sha512");
    }
    duk_put_prop_string(ctx, -2, "hash_type");

    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);

    duk_gc(ctx, 0);

    aos_free(ota_package_info);
}

/* system image upgrade */
static int32_t customer_upgrade_cb(void *pctx, char *ver, char *module_name, void *args)
{
    int32_t ret = OTA_TRANSPORT_PAR_FAIL;
    ota_package_info_t *ota_package_info = NULL;
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
            amp_debug(MOD_STR, "ota version:%s is coming, if OTA upgrade or not ?\n", ver);
            /* clear jsengine timer, distory js app*/
            if (aos_task_new_ext(&customer_ota_task, "amp_customer_ota", internal_sys_upgrade_start, (void *)pctx, 1024 * 8, AOS_DEFAULT_APP_PRI) != 0) {
                amp_debug(MOD_STR, "internal ota task create failed!");
                ret = OTA_TRANSPORT_PAR_FAIL;
            }
            amp_debug(MOD_STR, "app management center start");
        }
    } else {
        /*读取ota 触发时云端下发的文件信息*/
        ret = ota_read_parameter(&ota_param);
        if (ret < 0) {
            amp_error(MOD_STR, "get store ota param info failed\n");
        }

        ota_package_info = aos_malloc(sizeof(ota_package_info_t));
        if (!ota_package_info) {
            amp_error(MOD_STR, "alloc device notify param fail");
            return;
        }

        ota_package_info->js_cb_ref = (int)args;
        ota_package_info->length = ota_param.len;
        ota_package_info->hash_type = ota_param.hash_type;
        strncpy(ota_package_info->url, ota_param.url, sizeof(ota_package_info->url));
        strncpy(ota_package_info->version, ver, strlen(ver));
        strncpy(ota_package_info->module_name, module_name, strlen(module_name));
        strncpy(ota_package_info->hash, ota_param.hash, sizeof(ota_package_info->hash));

        amp_task_schedule_call(ota_trigger_notify, ota_package_info);
    }
}

static duk_ret_t native_ota_init(duk_context *ctx)
{
    int res = -1;
    int js_cb_ref;
    int productkey_len = IOTX_PRODUCT_KEY_LEN;
    int productsecret_len = IOTX_PRODUCT_SECRET_LEN;
    int devicename_len = IOTX_DEVICE_NAME_LEN;
    int devicesecret_len = IOTX_DEVICE_SECRET_LEN;
    iot_device_handle_t *iot_device_handle = NULL;
    if (!duk_is_pointer(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be (pointer and function)");
        goto out;
    }
    ota_service_param_reset(&customer_ota_ctx);
    /* get device info */
    aos_kv_get(AMP_CUSTOMER_PRODUCTKEY, customer_ota_ctx.pk, &productkey_len);
    aos_kv_get(AMP_CUSTOMER_PRODUCTSECRET, customer_ota_ctx.ps, &productsecret_len);
    aos_kv_get(AMP_CUSTOMER_DEVICENAME, customer_ota_ctx.dn, &devicename_len);
    aos_kv_get(AMP_CUSTOMER_DEVICESECRET, customer_ota_ctx.ds, &devicesecret_len);

    memset(customer_module_info, 0x00, sizeof(customer_module_info));
    iot_device_handle = duk_get_pointer(ctx, 0);
    customer_ota_ctx.mqtt_client = (void *)iot_device_handle->mqtt_handle;

    duk_dup(ctx, 1);
    js_cb_ref = be_ref(ctx);
    ota_register_module_store(&customer_ota_ctx, customer_module_info, 3);
    ota_register_trigger_msg_cb(&customer_ota_ctx, (void *)customer_upgrade_cb, js_cb_ref);

    ota_set_module_information(&customer_ota_ctx, "system", OS_APP_PATH, OTA_UPGRADE_ALL);
    /* init ota service */
    res = ota_service_init(&customer_ota_ctx);
    if (res < 0) {
        amp_error(MOD_STR, "customer ota init failed!");
    } else {
        amp_warn(MOD_STR, "customer ota init success!");
    }
    /*custom report version*/
    int current_ver[128] = {0};
    amp_app_version_get(current_ver);
    res = ota_report_module_version(&customer_ota_ctx, "system", current_ver);
    if (res < 0) {
        amp_error(MOD_STR, "amp ota report ver failed!");
    }

out:
    duk_push_int(ctx, res);
    return 1;
}

void module_app_ota_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("otaInit",     native_ota_init, 2);
    AMP_ADD_FUNCTION("otaDownload", native_ota_download, 2);
    AMP_ADD_FUNCTION("otaVerify",   native_ota_verify, 2);
    AMP_ADD_FUNCTION("otaReport",   native_ota_report, 1);
    AMP_ADD_FUNCTION("otaUpgrade",  native_ota_upgrade, 2);

    duk_put_prop_string(ctx, -2, "APPOTA");
}

