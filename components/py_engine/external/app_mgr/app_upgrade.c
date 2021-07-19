/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "amp_platform.h"
#include "aos_system.h"
#include "py_defines.h"
#include "app_mgr.h"
#include "app_upgrade.h"
#include "ota_agent.h"
#include "aiot_state_api.h"
#include "miniunz.h"

#define MOD_STR "APP_UPGRADE"

#if defined(__ICCARM__)
#define AMP_WEAK                __weak
#else
#define AMP_WEAK                __attribute__((weak))
#endif

static ota_store_module_info_t module_info[3];
static ota_service_t internal_ctx = {0};
static module_version[128];

char *py_ota_get_module_ver(void* pctx, char *module_name)
{
   
    return NULL;
}

int ota_get_py_install_path(char *store_file_full_path, char *path_buf, int path_buf_len)
{
    int ret = -1;
    int store_file_path_len = 0;
    int want_copy_len = 0;
    if ((store_file_full_path != NULL) && (path_buf != NULL)) {
        store_file_path_len = strlen(store_file_full_path);
        for (want_copy_len = store_file_path_len - 1; want_copy_len > 0; want_copy_len--) {
            if (store_file_full_path[want_copy_len] == '/') {
                break;
            }
        }
        if (want_copy_len > 0) {
            want_copy_len++;
            if (want_copy_len < path_buf_len) {
                ret = 0;
                memset(path_buf, 0x00, path_buf_len);
                strncpy(path_buf, store_file_full_path, want_copy_len);
                amp_debug(MOD_STR, "get store file path:%s\n", path_buf);
            } else {
                amp_error(MOD_STR, "get store file path failed, path buf too short\n");
            }
        }
    }
    return ret;
}

int ota_file_plus_name(char *file_path1, char *file_path2, char *new_path, int new_path_len)
{
    int ret = -1;
    int file_path1_len = 0;
    int file_path2_len = 0;
    if ((file_path1 == NULL) ||
        (file_path2 == NULL) ||
        (new_path == NULL)) {
        return ret;
    }
    file_path1_len = strlen(file_path1);
    file_path2_len = strlen(file_path2);
    memset(new_path, 0x00, new_path_len);
    if (file_path1_len + file_path2_len < new_path_len) {
        strncpy(new_path, file_path1, file_path1_len);
        if (new_path[file_path1_len - 1] != '/') {
            new_path[file_path1_len++] = '/';
        }
        strncpy(&new_path[file_path1_len], file_path2, file_path2_len);
        ret = 0;
    }
    return ret;
}



AMP_WEAK int fota_image_local_copy(char *image_name, int image_size)
{
    return -1;
}




/* system image upgrade */
static int32_t internal_upgrade_cb(void *pctx, char *ver, char *module_name)
{
    int32_t ret = OTA_TRANSPORT_PAR_FAIL;
    aos_task_t internal_ota_task;
    char *current_ver = NULL;
    if ((pctx == NULL) || (ver == NULL) || (module_name == NULL)) {
        amp_error(MOD_STR, "amp:ota triggered param err!");
        return ret;
    }
    if (strncmp(module_name, "default", strlen(module_name)) == 0) {
        ret = STATE_SUCCESS;
        current_ver = ota_get_module_ver(pctx, module_name);
        if (strncmp(ver, current_ver, strlen(ver)) <= 0) {
            ret = OTA_TRANSPORT_VER_FAIL;
            amp_error(MOD_STR, "amp ota version too old!");
        } else {
            amp_debug(MOD_STR, "ota version:%s is coming, if OTA upgrade or not ?\n", ver);
            /* clear jsengine timer, distory js app*/
            amp_module_free();
            app_js_stop();
            if (aos_task_new_ext(&internal_ota_task, "amp_internal_ota", internal_sys_upgrade_start, (void *)pctx, 1024 * 8, AOS_DEFAULT_APP_PRI) != 0) {
                amp_debug(MOD_STR, "internal ota task create failed!");
                ret = OTA_TRANSPORT_PAR_FAIL;
            }
            amp_debug(MOD_STR, "app management center start");
        }
    } else {
        // current_ver = ota_get_module_ver(pctx, module_name);
        // if (current_ver != NULL) {
        //     ret = STATE_SUCCESS;
        //     if (strncmp(ver, current_ver, strlen(ver)) <= 0) {
        //         ret = OTA_TRANSPORT_VER_FAIL;
        //         amp_error(MOD_STR, "submodule jsapp ota version too old!");
        //     } else {
        //         amp_debug(MOD_STR, "ota module version:%s is coming, if OTA module upgrade or not ?\n", ver);
        //         /* clear jsengine timer, distory js app*/
        //         amp_module_free();
        //         app_js_stop();
        //         if (aos_task_new_ext(&internal_ota_task, "amp_moudle_ota", internal_module_upgrade_start, (void *)pctx, 1024 * 8, AOS_DEFAULT_APP_PRI) != 0) {
        //             amp_debug(MOD_STR, "internal ota task create failed!");
        //             ret = OTA_TRANSPORT_PAR_FAIL;
        //         }
        //         amp_debug(MOD_STR, "app management center start");
        //     }
        // }
    }
    return ret;
}

/* app upgrade service */
int32_t py_app_upgrade_service(void *mqtt_handle)
{
    int res = STATE_SUCCESS;
    int productkey_len = IOTX_PRODUCT_KEY_LEN;
    int productsecret_len = IOTX_PRODUCT_SECRET_LEN;
    int devicename_len = IOTX_DEVICE_NAME_LEN;
    int devicesecret_len = IOTX_DEVICE_SECRET_LEN;
    char *current_ver = NULL;

    ota_service_param_reset(&internal_ctx);
    memset(module_info, 0x00, sizeof(module_info));
    ota_register_module_store(&internal_ctx, module_info, 3);
    /* get device info */
    aos_kv_get(AMP_INTERNAL_PRODUCTKEY, internal_ctx.pk, &productkey_len);
    aos_kv_get(AMP_INTERNAL_PRODUCTSECRET, internal_ctx.ps, &productsecret_len);
    aos_kv_get(AMP_INTERNAL_DEVICENAME, internal_ctx.dn, &devicename_len);
    aos_kv_get(AMP_INTERNAL_DEVICESECRET, internal_ctx.ds, &devicesecret_len);

    ota_register_trigger_msg_cb(&internal_ctx, (void *)internal_upgrade_cb, NULL);
    internal_ctx.mqtt_client = (void *)mqtt_handle;
    ota_set_module_information(&internal_ctx, AMP_OTA_MODULE_NAME, SUBDEV_FILE_PATH, OTA_UPGRADE_CUST);
    ota_set_module_information(&internal_ctx, "default", OS_APP_PATH, OTA_UPGRADE_ALL);
    /* init ota service */
    res = ota_service_init(&internal_ctx);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "internal ota init failed!");
    }
    amp_warn(MOD_STR, "internal ota init success!");

    /*custom report version*/
    current_ver = ota_get_module_ver(&internal_ctx, "default");
    res = ota_report_module_version(&internal_ctx, "default", current_ver);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "amp ota report ver failed!");
    }
    current_ver = ota_get_module_ver(&internal_ctx, AMP_OTA_MODULE_NAME);
    res = ota_report_module_version(&internal_ctx, AMP_OTA_MODULE_NAME, current_ver);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "amp jsota report ver failed!");
        return -1;
    }

    return STATE_SUCCESS;
}


int ota_install_pyapp(void *ota_ctx, char *store_file, int store_file_len, char *install_path)
{
    int ret = -1;
    ota_service_t *ctx = ota_ctx;
    amp_debug(MOD_STR,"upgrade sub module\n");
    if ((store_file != NULL) && (install_path != NULL)) {
        amp_debug(MOD_STR,"store_file:%s, install_file:%s\n", store_file, install_path);
        //ret = data_file_unpack(store_file, store_file_len, install_path);
        ret = miniUnzip(store_file,install_path);
    }
    if (ret < 0) {
        amp_error(MOD_STR,"py app install failed\n");
        if ((ctx != NULL) && (ctx->report_func.report_status_cb !=  NULL)) {
            ctx->report_func.report_status_cb(ctx->report_func.param, ret);
        }
    }
    if ((ctx != NULL) && (ctx->feedback_func.on_user_event_cb != NULL)) {
        ctx->feedback_func.on_user_event_cb(OTA_EVENT_INSTALL, ret, ctx->feedback_func.param);
    }
    return ret;
}


int install_pyapp(char *store_file,  char *install_path)
{
    int ret = -1;
    amp_debug(MOD_STR,"upgrade sub module\n");
    if ((store_file != NULL) && (install_path != NULL)) {
        amp_debug(MOD_STR,"store_file:%s, install_file:%s\n", store_file, install_path);
        ret = miniUnzip(store_file,install_path);
    }
    if (ret < 0) {
        amp_error(MOD_STR,"py app install failed\n");
    }

    return ret;
}