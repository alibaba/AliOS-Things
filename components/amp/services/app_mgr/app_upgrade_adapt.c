/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "amp_platform.h"
#include "aos_system.h"
#include "aos_ota.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "app_mgr.h"
#include "app_upgrade.h"
#include "ota_agent.h"
#include "ota_import.h"
// #include "ota_hal_os.h"
#include "aiot_state_api.h"
#include "upack_data_file.h"
#define MOD_STR "APP_UPGRADE"

#if defined(__ICCARM__)
#define AMP_WEAK                __weak
#else
#define AMP_WEAK                __attribute__((weak))
#endif

int ota_get_js_install_path(char *store_file_full_path, char *path_buf, int path_buf_len);
int ota_file_plus_name(char *file_path1, char *file_path2, char *new_path, int new_path_len);
int ota_service_maindev_fs_start(ota_service_t *ctx);

OTA_WEAK int ota_jsapp_version_get(char *version, char *file_path)
{
    int ret = -1;
    int i = 0;
    int fd = -1;
    char read_buf[65];
    int read_len = 0;
    char *pos = 0;
    if ((version != NULL) && (file_path != NULL)) {
        ret = 0;
        fd = ota_fopen(file_path, O_RDONLY);
        if (fd < 0) {
            OTA_LOG_I("can't find app.json file\r\n");
            strcpy(version, "0.0.0");
        } else {
            memset(read_buf, 0x00, sizeof(read_buf));
            read_len = ota_fread(fd, read_buf, sizeof(read_buf) - 1);
            if (read_len > 13) {
                pos = strstr(read_buf, "\"version\":");
                if (pos != NULL) {
                    pos += 10;
                    while (pos[0] != '"') {
                        pos++;
                    }
                    pos++;
                    while (pos[0] != '"') {
                        version[i++] = pos[0];
                        pos++;
                    }
                    version[i] = 0;
                }
             }
        }
        if (fd >= 0) {
            ota_fclose(fd);
        }
    }
    return ret;
}

int ota_install_jsapp(void *ota_ctx, char *store_file, int store_file_len, char *install_path)
{
    int ret = -1;
    ota_service_t *ctx = ota_ctx;
    if ((store_file != NULL) && (install_path != NULL)) {
        ret = data_file_unpack(store_file, store_file_len, install_path);
    }
    if (ret < 0) {
        if ((ctx != NULL) && (ctx->report_func.report_status_cb !=  NULL)) {
            ctx->report_func.report_status_cb(ctx->report_func.param, ret);
        }
    }
    if ((ctx != NULL) && (ctx->feedback_func.on_user_event_cb != NULL)) {
        ctx->feedback_func.on_user_event_cb(OTA_EVENT_INSTALL, ret, ctx->feedback_func.param);
    }
    return ret;
}


AMP_WEAK int aos_fota_image_local_copy(char *image_name, int image_size)
{
    amp_error(MOD_STR, "local copy code is invalid, weak function failed!");
    return -1;
}

AMP_WEAK void internal_sys_upgrade_start(void *ctx)
{
    amp_debug(MOD_STR, "internal sys upgrade start weak function\n");
    ota_service_maindev_fs_start((ota_service_t *)ctx);
    return;
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
        amp_error(MOD_STR, "internal module ota input ctx is null\n");
        return;
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
        aos_msleep(3000);
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
