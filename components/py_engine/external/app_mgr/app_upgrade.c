/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "app_upgrade.h"

#include "aiot_state_api.h"
#include "amp_platform.h"
#include "aos/kernel.h"
#include "aos_system.h"
#include "app_mgr.h"
#include "miniunz.h"
#include "ota_agent.h"
#include "py_defines.h"

#define MOD_STR "APP_UPGRADE"

#if defined(__ICCARM__)
#define AMP_WEAK __weak
#else
#define AMP_WEAK __attribute__((weak))
#endif

static ota_store_module_info_t module_info[3];
static ota_service_t internal_ctx = { 0 };

char *py_ota_get_module_ver(void *pctx, char *module_name)
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

int pyamp_ota_file_plus_name(char *file_path1, char *file_path2, char *new_path, int new_path_len)
{
    int ret = -1;
    int file_path1_len = 0;
    int file_path2_len = 0;
    if ((file_path1 == NULL) || (file_path2 == NULL) || (new_path == NULL)) {
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

int ota_install_pyapp(void *ota_ctx, char *store_file, int store_file_len, char *install_path)
{
    int ret = -1;
    ota_service_t *ctx = ota_ctx;
    amp_debug(MOD_STR, "upgrade sub module\n");
    if ((store_file != NULL) && (install_path != NULL)) {
        amp_debug(MOD_STR, "store_file:%s, install_file:%s\n", store_file, install_path);
        // ret = data_file_unpack(store_file, store_file_len, install_path);
        ret = miniUnzip(store_file, install_path);
    }
    if (ret < 0) {
        amp_error(MOD_STR, "py app install failed\n");
        if ((ctx != NULL) && (ctx->report_func.report_status_cb != NULL)) {
            ctx->report_func.report_status_cb(ctx->report_func.param, ret);
        }
    }
    if ((ctx != NULL) && (ctx->feedback_func.on_user_event_cb != NULL)) {
        ctx->feedback_func.on_user_event_cb(OTA_EVENT_INSTALL, ret, ctx->feedback_func.param);
    }
    return ret;
}

int install_pyapp(char *store_file, char *install_path)
{
    int ret = -1;
    amp_debug(MOD_STR, "upgrade sub module\n");
    if ((store_file != NULL) && (install_path != NULL)) {
        amp_debug(MOD_STR, "store_file:%s, install_file:%s\n", store_file, install_path);
        ret = miniUnzip(store_file, install_path);
    }
    if (ret < 0) {
        amp_error(MOD_STR, "py app install failed\n");
    }

    return ret;
}