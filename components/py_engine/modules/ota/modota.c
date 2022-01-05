
/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <string.h>
#if MICROPY_PY_OTA
#include "aos_system.h"
#include "board_config.h"
#include "py/builtin.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "py_defines.h"
//#include "be_inl.h"
#include "amp_utils.h"
#include "app_upgrade.h"
#include "module_aiot.h"
#include "ota_agent.h"
#include "ota_import.h"
#include "utility.h"
#include "amp_task.h"
#include "aos/kv.h"
#define MOD_STR "APP_OTA"

static ota_service_t customer_ota_ctx = { 0 };
static ota_store_module_info_t customer_module_info[3];
static aos_task_t user_module_ota_task = { 0 };
static char default_ver[128] = {0};
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

typedef enum {
    ON_TRIGGER = 1,
    ON_DOWNLOAD = 2,
    ON_VERIFY = 3,
    ON_UPGRADE = 4,
} ota_cb_func_t;

static mp_obj_t ota_on_trigger;
static mp_obj_t ota_on_download;
static mp_obj_t ota_on_verify;
static mp_obj_t ota_on_upgrade;

static void ota_install_notify(void *pdata)
{
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;

    if (mp_obj_is_fun(ota_on_upgrade)) {
        callback_to_python(ota_on_upgrade,
                           mp_obj_new_int(ota_package_info->res));
    } else {
        amp_error(MOD_STR, "ota_on_trigger is not function");
    }

    aos_free(ota_package_info);
}

static void ota_install_handler(void *pdata)
{
    int res = -1;
    int js_cb_ref;
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;
    /* clear jsengine timer, distory js app*/
    // amp_module_free();
    // app_js_stop();

    res = ota_install_pyapp(&customer_ota_ctx, ota_package_info->store_path,
                            ota_package_info->length,
                            ota_package_info->install_path);
    if (res < 0) {
        amp_error(MOD_STR, "module install failed!");
    } else {
        /*启动app.js*/
        // res = ota_load_jsapp(&customer_ota_ctx);
        // if(res < 0) {
        //     amp_error(MOD_STR, "module load failed!");
        // }
    }
    ota_package_info->res = 0;

    py_task_schedule_call(ota_install_notify, ota_package_info);
    aos_task_exit(0);
}

static mp_obj_t ota_upgrade(mp_obj_t data)
{
    int res = -1;
    unsigned int length = 0;
    const char *install_path = NULL;
    const char *store_path = NULL;
    aos_task_t ota_install_task;
    ota_package_info_t *ota_package_info = NULL;

    if (!mp_obj_is_dict_or_ordereddict(data)) {
        amp_error(MOD_STR, "ota_report function param must be dict");
        return mp_obj_new_int(-1);
    }

    /* get verify info */
    mp_obj_t index = mp_obj_new_str_via_qstr("length", 6);
    length = mp_obj_get_int(mp_obj_dict_get(data, index));

    /* get hash_type   */
    index = mp_obj_new_str_via_qstr("store_path", 10);
    store_path = mp_obj_str_get_str(mp_obj_dict_get(data, index));

    /* get install_path   */
    index = mp_obj_new_str_via_qstr("install_path", 12);
    install_path = mp_obj_str_get_str(mp_obj_dict_get(data, index));

    ota_package_info = aos_malloc(sizeof(ota_package_info_t));
    if (!ota_package_info) {
        amp_error(MOD_STR, "alloc device notify param fail");
        return mp_obj_new_int(-1);
    }
    memset(ota_package_info, 0x00, sizeof(ota_package_info_t));
    ota_package_info->length = length;
    strncpy(ota_package_info->store_path, store_path,
            sizeof(ota_package_info->store_path));
    strncpy(ota_package_info->install_path, install_path,
            sizeof(ota_package_info->install_path));

    res = aos_task_new_ext(&ota_install_task, "amp ota install task",
                           ota_install_handler, ota_package_info, 1024 * 4,
                           AOS_DEFAULT_APP_PRI);
    if (res != 0) {
        amp_warn(MOD_STR, "iot create task failed");
        aos_free(ota_package_info);
        goto out;
    }
out:
    mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_1(native_ota_upgrade, ota_upgrade);

static void ota_verify_notify(void *pdata)
{
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;

    if (mp_obj_is_fun(ota_on_verify)) {
        callback_to_python(ota_on_verify,
                           mp_obj_new_int(ota_package_info->res));
    } else {
        amp_error(MOD_STR, "ota_on_verify is not function");
    }

    aos_free(ota_package_info);
}

static void ota_verify_handler(void *pdata)
{
    int res = -1;
    int js_cb_ref;
    ota_boot_param_t ota_param = { 0 };
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;

    memset(&ota_param, 0, sizeof(ota_boot_param_t));

    ota_param.len = ota_package_info->length;
    ota_param.hash_type = ota_package_info->hash_type;
    strncpy(ota_param.hash, ota_package_info->hash,
            strlen(ota_package_info->hash));
    res = ota_verify_fsfile(&ota_param, ota_package_info->store_path);
    if (res < 0) {
        amp_error(MOD_STR, "amp jsota verified failed!");
    }
    ota_package_info->res = res;
    py_task_schedule_call(ota_verify_notify, ota_package_info);
    aos_task_exit(0);
}

static mp_obj_t py_ota_verify(mp_obj_t data)
{
    int res = -1;
    // int js_cb_ref;
    aos_task_t ota_verify_task;
    ota_package_info_t *ota_package_info = NULL;

    unsigned int length = 0;
    const char *hash_type = NULL;
    const char *hash = NULL;
    const char *store_path = NULL;

    if (!mp_obj_is_dict_or_ordereddict(data)) {
        amp_error(MOD_STR, "ota_report function param must be dict");
        return mp_obj_new_int(-1);
    }

    /* get verify info */
    // duk_get_prop_string(ctx, 0, "length");
    mp_obj_t index = mp_obj_new_str_via_qstr("length", 6);
    length = mp_obj_get_int(mp_obj_dict_get(data, index));

    /* get hash_type   */
    index = mp_obj_new_str_via_qstr("hash_type", 9);
    hash_type = mp_obj_str_get_str(mp_obj_dict_get(data, index));

    /* get hash   */
    index = mp_obj_new_str_via_qstr("hash", 4);
    hash = mp_obj_str_get_str(mp_obj_dict_get(data, index));

    /* get store_path   */
    index = mp_obj_new_str_via_qstr("store_path", 10);
    store_path = mp_obj_str_get_str(mp_obj_dict_get(data, index));

    ota_package_info = aos_malloc(sizeof(ota_package_info_t));
    if (!ota_package_info) {
        amp_error(MOD_STR, "alloc device notify param fail");
        return mp_obj_new_int(-1);
    }
    memset(ota_package_info, 0x00, sizeof(ota_package_info_t));
    ota_package_info->length = length;
    if (strcmp(hash_type, "null") == 0) {
        ota_package_info->hash_type = 0;
    } else if (strcmp(hash_type, "md5") == 0) {
        ota_package_info->hash_type = 2;
    } else if (strcmp(hash_type, "sha256") == 0) {
        ota_package_info->hash_type = 1;
    } else {
        ota_package_info->hash_type = 3;
    }
    strncpy(ota_package_info->hash, hash, sizeof(ota_package_info->hash));
    strncpy(ota_package_info->store_path, store_path,
            sizeof(ota_package_info->store_path));

    res = aos_task_new_ext(&ota_verify_task, "amp ota verify task",
                           ota_verify_handler, ota_package_info, 1024 * 10,
                           AOS_DEFAULT_APP_PRI);
    if (res != 0) {
        amp_warn(MOD_STR, "iot create task failed");
        aos_free(ota_package_info);
        goto out;
    }
out:
    return mp_obj_new_int(res);
}
MP_DEFINE_CONST_FUN_OBJ_1(native_ota_verify, py_ota_verify);

static void ota_download_notify(void *pdata)
{
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;

    if (mp_obj_is_fun(ota_on_download)) {
        callback_to_python(ota_on_download,
                           mp_obj_new_int(ota_package_info->res));
    } else {
        amp_error(MOD_STR, "ota_on_download is not function");
    }

    aos_free(ota_package_info);
}

static void ota_download_handler(void *pdata)
{
    int res = -1;
    int js_cb_ref;
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;

    res = ota_download_store_fs_start(
        ota_package_info->url, strlen(ota_package_info->url),
        ota_package_info->store_path,
        NULL/*customer_ota_ctx.report_func.report_status_cb*/,
        customer_ota_ctx.report_func.param);
    if (res < 0) {
        amp_error(MOD_STR, "amp jsota download file failed!");
    }

    ota_package_info->res = res;
    py_task_schedule_call(ota_download_notify, ota_package_info);
    aos_task_exit(0);
}

static mp_obj_t ota_download(mp_obj_t data)
{
    int res = -1;
    int js_cb_ref;
    aos_task_t ota_download_task;
    ota_package_info_t *ota_package_info = NULL;
    const char *url = NULL;
    const char *store_path = NULL;

    if (!mp_obj_is_dict_or_ordereddict(data)) {
        amp_error(MOD_STR, "ota_report function param must be dict");
        return mp_obj_new_int(-1);
    }

    /* get store path  */
    mp_obj_t index = mp_obj_new_str_via_qstr("url", 3);
    url = mp_obj_str_get_str(mp_obj_dict_get(data, index));

    /* get url  */
    index = mp_obj_new_str_via_qstr("store_path", 10);
    store_path = mp_obj_str_get_str(mp_obj_dict_get(data, index));

    ota_package_info = aos_malloc(sizeof(ota_package_info_t));
    if (!ota_package_info) {
        amp_error(MOD_STR, "alloc device notify param fail");
        return mp_obj_new_int(-1);
    }
    memset(ota_package_info, 0x00, sizeof(ota_package_info_t));
    strncpy(ota_package_info->url, url, sizeof(ota_package_info->url));
    strncpy(ota_package_info->store_path, store_path,
            sizeof(ota_package_info->store_path));

    res = aos_task_new_ext(&ota_download_task, "amp ota download task",
                           ota_download_handler, ota_package_info, 1024 * 10,
                           AOS_DEFAULT_APP_PRI);
    if (res != 0) {
        amp_warn(MOD_STR, "iot create task failed");
        aos_free(ota_package_info);
        goto out;
    }
out:
    return mp_obj_new_int(res);
}
MP_DEFINE_CONST_FUN_OBJ_1(native_ota_download, ota_download);

static mp_obj_t ota_report(mp_obj_t data)
{
    int res = -1;
    int js_cb_ref;
    iot_device_handle_t *iot_device_handle = NULL;

    if (!mp_obj_is_dict_or_ordereddict(data)) {
        amp_error(MOD_STR, "ota_report function param must be dict");
        return mp_obj_new_int(-1);
    }

    /* get device handle */
    mp_obj_t index = mp_obj_new_str_via_qstr("device_handle", 13);
    iot_device_handle = (iot_device_handle_t *)MP_OBJ_SMALL_INT_VALUE(mp_obj_dict_get(data, index));

    /* get product_key  */
    index = mp_obj_new_str_via_qstr("product_key", 11);
    const char *productkey = mp_obj_str_get_str(mp_obj_dict_get(data, index));

    /* get devicename  */
    index = mp_obj_new_str_via_qstr("device_name", 11);
    const char *devicename = mp_obj_str_get_str(mp_obj_dict_get(data, index));

    /* get module_name  */
    index = mp_obj_new_str_via_qstr("module_name", 11);
    const char *modulename = mp_obj_str_get_str(mp_obj_dict_get(data, index));

    /* get version  */
    index = mp_obj_new_str_via_qstr("version", 7);
    const char *ver = mp_obj_str_get_str(mp_obj_dict_get(data, index));

    amp_warn(MOD_STR, "js report ver!");
    if  (!strncmp(modulename, "default", strlen("default") + 1)) {
        strcpy(default_ver, ver);
    }
    res = ota_transport_inform(iot_device_handle->mqtt_handle, productkey,
                               devicename, modulename, ver);
    if (res < 0) {
        amp_error(MOD_STR, "amp pyota report ver failed!");
    }
out:
    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_1(native_ota_report, ota_report);

static void ota_trigger_notify(void *pdata)
{
    ota_package_info_t *ota_package_info = (ota_package_info_t *)pdata;

    mp_obj_t dict = mp_obj_new_dict(4);
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("length", 6),
                      mp_obj_new_int(ota_package_info->length));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("module_name", 11),
                      mp_obj_new_str(ota_package_info->module_name,
                                     strlen(ota_package_info->module_name)));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("version", 7),
                      mp_obj_new_str(ota_package_info->version,
                                     strlen(ota_package_info->version)));
    mp_obj_dict_store(
        MP_OBJ_FROM_PTR(dict), mp_obj_new_str("url", 3),
        mp_obj_new_str(ota_package_info->url, strlen(ota_package_info->url)));
    mp_obj_dict_store(
        MP_OBJ_FROM_PTR(dict), mp_obj_new_str("hash", 4),
        mp_obj_new_str(ota_package_info->hash, strlen(ota_package_info->hash)));

    const char *hash_type = NULL;
    if (ota_package_info->hash_type == 0) {
        hash_type = "null";
    } else if (ota_package_info->hash_type == 1) {
        hash_type = "sha256";
    } else if (ota_package_info->hash_type == 2) {
        hash_type = "md5";
    } else {
        hash_type = "sha512";
    }
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("hash_type", 9),
                      mp_obj_new_str(hash_type, strlen(hash_type)));

    amp_debug(MOD_STR, "module_name is %s  ; length is %d",
              ota_package_info->module_name,
              strlen(ota_package_info->module_name));
    amp_debug(MOD_STR, "version is %s ;  length is %d",
              ota_package_info->version, strlen(ota_package_info->version));
    if (mp_obj_is_fun(ota_on_trigger)) {
        callback_to_python(ota_on_trigger, dict);
    } else {
        amp_error(MOD_STR, "ota_on_trigger is not function");
    }

    aos_free(ota_package_info);
}

/* system image upgrade */
static int32_t customer_upgrade_cb(void *pctx, char *ver, char *module_name,
                                   void *args)
{
    int32_t ret = OTA_TRANSPORT_PAR_FAIL;
    ota_package_info_t *ota_package_info = NULL;
    ota_boot_param_t ota_param = { 0 };
    aos_task_t customer_ota_task;
    if ((pctx == NULL) || (ver == NULL) || (module_name == NULL) ||
        (args == NULL)) {
        amp_error(MOD_STR, "amp:ota triggered param err!");
        return ret;
    }
    if (strncmp(module_name, "default", strlen(module_name)) == 0) {
        ret = 0;
        printf("ver:%s current_ver:%s\r\n", ver, default_ver);
        if (strncmp(ver, default_ver, strlen(ver)) <= 0) {
            ret = OTA_TRANSPORT_VER_FAIL;
            amp_error(MOD_STR, "amp ota version too old!");
        } else {
            amp_debug(MOD_STR,
                      "ota version:%s is coming, if OTA upgrade or not ?\n",
                      ver);
            /* clear jsengine timer, distory js app*/
            if (aos_task_new_ext(&customer_ota_task, "amp_customer_ota",
                                 internal_sys_upgrade_start, (void *)pctx,
                                 1024 * 4, AOS_DEFAULT_APP_PRI) != 0) {
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
            return -1;
        }

        ota_package_info->js_cb_ref = (int)args;
        ota_package_info->length = ota_param.len;
        ota_package_info->hash_type = ota_param.hash_type;
        strncpy(ota_package_info->url, ota_param.url,
                sizeof(ota_package_info->url));
        strncpy(ota_package_info->version, ver, strlen(ver));
        strncpy(ota_package_info->module_name, module_name,
                strlen(module_name));
        strncpy(ota_package_info->hash, ota_param.hash,
                sizeof(ota_package_info->hash));

        /** todo call ota_trigger_notify */
        py_task_schedule_call(ota_trigger_notify, ota_package_info);
    }
}

static mp_obj_t ota_init(mp_obj_t data)
{
    int res = -1;
    int productkey_len = IOTX_PRODUCT_KEY_LEN;
    int productsecret_len = IOTX_PRODUCT_SECRET_LEN;
    int devicename_len = IOTX_DEVICE_NAME_LEN;
    int devicesecret_len = IOTX_DEVICE_SECRET_LEN;
    iot_device_handle_t *iot_device_handle = NULL;
    ota_package_info_t *ota_package_info = NULL;

    if (!mp_obj_is_dict_or_ordereddict(data)) {
        return mp_obj_new_int(res);
    }

    /* get http request url */
    mp_obj_t index = mp_obj_new_str_via_qstr("device_handle", 13);

    iot_device_handle = (iot_device_handle_t *)MP_OBJ_SMALL_INT_VALUE(mp_obj_dict_get(data, index));
    ota_service_param_reset(&customer_ota_ctx);
    /* get device info
    todo:save kv when do device_connect
    */
    aos_kv_get(AMP_CUSTOMER_PRODUCTKEY, customer_ota_ctx.pk, &productkey_len);
    aos_kv_get(AMP_CUSTOMER_PRODUCTSECRET, customer_ota_ctx.ps,
               &productsecret_len);
    aos_kv_get(AMP_CUSTOMER_DEVICENAME, customer_ota_ctx.dn, &devicename_len);
    aos_kv_get(AMP_CUSTOMER_DEVICESECRET, customer_ota_ctx.ds,
               &devicesecret_len);

    memset(customer_module_info, 0x00, sizeof(customer_module_info));
    customer_ota_ctx.mqtt_client = (void *)iot_device_handle->mqtt_handle;

    ota_package_info = aos_malloc(sizeof(ota_package_info_t));
    if (!ota_package_info) {
        amp_error(MOD_STR, "alloc device notify param fail");
        goto out;
    }
    memset(ota_package_info, 0, sizeof(ota_package_info_t));
    ota_package_info->js_cb_ref = 0;

    ota_register_module_store(&customer_ota_ctx, customer_module_info, 3);
    ota_register_trigger_msg_cb(&customer_ota_ctx, (void *)customer_upgrade_cb,
                                (void *)1);

    /* init ota service */
    res = ota_service_init(&customer_ota_ctx);
    if (res < 0) {
        amp_error(MOD_STR, "customer ota init failed!");
    } else {
        amp_error(MOD_STR, "customer ota init success!");
    }
out:
    if (res < 0) {
        // todo:ota_package_info->js_cb_ref 是否需要free?
        if (ota_package_info != NULL) {
            aos_free(ota_package_info);
        }
        return mp_obj_new_int(-1);
    }
    mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_1(native_ota_init, ota_init);

STATIC mp_obj_t ota_register_cb(mp_obj_t id, mp_obj_t func)
{
    int res = -1;
    int callback_id = mp_obj_get_int(id);
    switch (callback_id) {
        case ON_TRIGGER:
            ota_on_trigger = func;
            break;
        case ON_DOWNLOAD:
            ota_on_download = func;
            break;
        case ON_VERIFY:
            ota_on_verify = func;
            break;
        case ON_UPGRADE:
            ota_on_upgrade = func;
            break;

        default:
            break;
    }
    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_2(native_ota_register_cb, ota_register_cb);

STATIC const mp_rom_map_elem_t ota_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ota) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), MP_ROM_PTR(&native_ota_init) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_on), MP_ROM_PTR(&native_ota_register_cb) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_download), MP_ROM_PTR(&native_ota_download) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_verify), MP_ROM_PTR(&native_ota_verify) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_report), MP_ROM_PTR(&native_ota_report) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_upgrade), MP_ROM_PTR(&native_ota_upgrade) },
};

STATIC MP_DEFINE_CONST_DICT(ota_module_globals, ota_module_globals_table);

const mp_obj_module_t mp_module_ota = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&ota_module_globals,
};
MP_REGISTER_MODULE(MP_QSTR_ota, mp_module_ota, MICROPY_PY_OTA);
#endif
