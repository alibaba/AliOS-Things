/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "amp_platform.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "app_mgr.h"
#include "app_upgrade.h"
#include "ota_agent.h"
#include "aiot_state_api.h"

#define MOD_STR "APP_UPGRADE"

#if defined(__ICCARM__)
#define AMP_WEAK                __weak
#else
#define AMP_WEAK                __attribute__((weak))
#endif

static ota_store_module_info_t module_info[3];
static ota_service_t internal_ctx = {0};
static module_version[128];

char *ota_get_module_ver(void* pctx, char *module_name)
{
    int ret = 0;
    ota_store_module_info_t module_info;
    ota_service_t *ctx = (ota_service_t *)pctx;
    if ((pctx != NULL) && (module_name != NULL)) {
        if (strncmp(module_name, AMP_OTA_MODULE_NAME, strlen(module_name)) == 0) {
            char app_js_full_path[128] = {0};
            char js_install_path[64] = {0};
            memset(module_version, 0x00, sizeof(module_version));

            ret = ota_get_module_information(ctx, module_name, &module_info);
            if (ret < 0) {
                amp_error(MOD_STR, "get store module info failed\n");
                goto INTERNAL_M_VER_OVER;
            }

            ret = ota_get_js_install_path(module_info.store_path, js_install_path, sizeof(js_install_path));
            if (ret < 0) {
                amp_error(MOD_STR, "get js install path failed!");
                goto INTERNAL_M_VER_OVER;
            }

            ret = ota_file_plus_name(js_install_path, "app.json", app_js_full_path, sizeof(app_js_full_path));
            if ((ret < 0)) {
                amp_error(MOD_STR, "module get js version failed!");
                goto INTERNAL_M_VER_OVER;
            }

            ret = ota_jsapp_version_get(module_version, app_js_full_path);
            if ((ret < 0)) {
                amp_error(MOD_STR, "app.json path get failed!");
            }
        } else if (strncmp(module_name, "default", strlen(module_name)) == 0) {
            amp_app_version_get(module_version);
        }
    } else {
        amp_error(MOD_STR, "get store file path failed, path buf too short\n");
    }
INTERNAL_M_VER_OVER:
    if (ret < 0) {
        amp_error(MOD_STR, "app.json ver get failed! set default ver");
        strncpy(module_version, "0.0.0", 5);
    }
    return module_version;
}

int ota_get_js_install_path(char *store_file_full_path, char *path_buf, int path_buf_len)
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

int ota_load_jsapp(void *ota_ctx)
{
    int ret = -1;
    ota_service_t *ctx = ota_ctx;
    ret = app_js_restart();
    if (ret < 0) {
        amp_error(MOD_STR, "module load failed\n");
        if ((ctx != NULL) && (ctx->report_func.report_status_cb !=  NULL)) {
            ctx->report_func.report_status_cb(ctx->report_func.param, ret);
        }
    }
    if ((ctx != NULL) && (ctx->feedback_func.on_user_event_cb != NULL)) {
        ctx->feedback_func.on_user_event_cb(OTA_EVENT_LOAD, ret, ctx->feedback_func.param);
    }
    return ret;
}

AMP_WEAK int fota_image_local_copy(char *image_name, int image_size)
{
    return -1;
}

AMP_WEAK void internal_sys_upgrade_start(void *ctx)
{
    ota_service_maindev_fs_start((ota_service_t *)ctx);
    return;
}

int ota_service_maindev_fs_start(ota_service_t *ctx)
{
    int ret = 0;
    ota_boot_param_t ota_param = {0};
    ota_store_module_info_t module_info;
    if (ctx == NULL) {
        return OTA_DOWNLOAD_INIT_FAIL;
    }
    ret = ota_read_parameter(&ota_param);
    if (ret < 0) {
        goto MAINDEV_FS_EXIT;
    }
    amp_debug(MOD_STR, "download start upg_flag:0x%x\n", ota_param.upg_flag);
    ret = ota_get_module_information(ctx, ctx->module_name, &module_info);
    if (ret < 0) {
        amp_error(MOD_STR, "get store module info failed\n");
        goto MAINDEV_FS_EXIT;
    }
    amp_debug(MOD_STR, "file_path = %s\rn", module_info.store_path);
    ret = ota_download_to_fs_service(ctx, module_info.store_path);
    if (ret < 0) {
        amp_error(MOD_STR, "module download failed!");
        goto MAINDEV_FS_EXIT;
    }

MAINDEV_FS_EXIT:
    amp_debug(MOD_STR, "Download complete, rebooting ret:%d.\n", ret);
    if (ret < 0) {
        if (ctx->report_func.report_status_cb !=  NULL) {
            ctx->report_func.report_status_cb(ctx->report_func.param, ret);
        }
        ret = ota_clear();
        if (ret < 0) {
            amp_error(MOD_STR, "clear ota failed\n");
        }
        /* give same time to sync err msg to cloud*/
        ota_msleep(3000);
    } else {
        ret = fota_image_local_copy(module_info.store_path, ota_param.len);
        if (ret < 0) {
            amp_error(MOD_STR, "ota local copy failed\n");
            if (ctx->report_func.report_status_cb !=  NULL) {
                ctx->report_func.report_status_cb(ctx->report_func.param, ret);
            }
            ota_msleep(1500);
        }
    }
    ota_reboot();
    aos_task_exit(0);
    return ret;
}

AMP_WEAK void internal_module_upgrade_start(void *ctx)
{
    int ret = -1;
    ota_service_t *tmp_ctx = (ota_service_t *)ctx;
    char version[64] = {0};
    char js_install_path[64] = {0};
    char app_js_full_path[128] = {0};
    ota_store_module_info_t module_info;
    ota_boot_param_t ota_param = {0};
    if (tmp_ctx == NULL) {
        amp_error(MOD_STR, "internal module ota input ctx is null");
        return ret;
    }

    ret = ota_read_parameter(&ota_param);
    if (ret < 0) {
        amp_error(MOD_STR, "get store ota param info failed\n");
        goto INTERNAL_M_OTA_OVER;
    }

    ret = ota_get_module_information(tmp_ctx, tmp_ctx->module_name, &module_info);
    if (ret < 0) {
        amp_error(MOD_STR, "get store module info failed\n");
        goto INTERNAL_M_OTA_OVER;
    }
    amp_debug(MOD_STR, "file_path = %s\rn", module_info.store_path);

    ret = ota_download_to_fs_service(ctx, module_info.store_path);
    if (ret < 0) {
        amp_error(MOD_STR, "module download failed!");
        goto INTERNAL_M_OTA_OVER;
    }

    ret = ota_get_js_install_path(module_info.store_path, js_install_path, sizeof(js_install_path));
    if (ret < 0) {
        amp_error(MOD_STR, "get js install path failed!");
        goto INTERNAL_M_OTA_OVER;
    }

    ret = ota_install_jsapp(ctx, module_info.store_path, ota_param.len, js_install_path);
    if (ret < 0) {
        amp_error(MOD_STR, "module install failed!");
        goto INTERNAL_M_OTA_OVER;
    }

    ret = ota_load_jsapp(ctx);
    if (ret < 0) {
        amp_error(MOD_STR, "module load failed!");
        goto INTERNAL_M_OTA_OVER;
    }

    ret = ota_file_plus_name(js_install_path, "app.json", app_js_full_path, sizeof(app_js_full_path));
    if ((ret < 0)) {
        amp_error(MOD_STR, "module get js version failed!");
        goto INTERNAL_M_OTA_OVER;
    }

    ret = ota_jsapp_version_get(version, app_js_full_path);
    if ((ret < 0)) {
        amp_error(MOD_STR, "app.json path get failed!");
        goto INTERNAL_M_OTA_OVER;
    }

    ret = ota_report_module_version(ctx, tmp_ctx->module_name, version);
    if (ret < 0) {
        amp_error(MOD_STR, "module report ver failed!");
        goto INTERNAL_M_OTA_OVER;
    }

INTERNAL_M_OTA_OVER:
    if (ret < 0) {
        amp_error(MOD_STR, "ota module upgrade failed\n");
        if ((tmp_ctx->report_func.report_status_cb !=  NULL)) {
            tmp_ctx->report_func.report_status_cb(tmp_ctx->report_func.param, ret);
        }
        ota_msleep(3000);
    } else {
        amp_debug(MOD_STR, "module download success!");
    }
    ret = ota_clear();
    if (ret < 0) {
        amp_error(MOD_STR, "clear ota failed\n");
    }
    aos_task_exit(0);
    return;
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
        current_ver = ota_get_module_ver(pctx, module_name);
        if (current_ver != NULL) {
            ret = STATE_SUCCESS;
            if (strncmp(ver, current_ver, strlen(ver)) <= 0) {
                ret = OTA_TRANSPORT_VER_FAIL;
                amp_error(MOD_STR, "submodule jsapp ota version too old!");
            } else {
                amp_debug(MOD_STR, "ota module version:%s is coming, if OTA module upgrade or not ?\n", ver);
                /* clear jsengine timer, distory js app*/
                amp_module_free();
                app_js_stop();
                if (aos_task_new_ext(&internal_ota_task, "amp_moudle_ota", internal_module_upgrade_start, (void *)pctx, 1024 * 8, AOS_DEFAULT_APP_PRI) != 0) {
                    amp_debug(MOD_STR, "internal ota task create failed!");
                    ret = OTA_TRANSPORT_PAR_FAIL;
                }
                amp_debug(MOD_STR, "app management center start");
            }
        }
    }
    return ret;
}

/* app upgrade service */
int32_t amp_app_upgrade_service(void *mqtt_handle)
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
