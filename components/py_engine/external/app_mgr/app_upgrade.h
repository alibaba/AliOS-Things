/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef APP_UPGRADE_H
#define APP_UPGRADE_H

#include <stdint.h>

#define AMP_OTA_MODULE_NAME "pyapp"

#define SUBDEV_FILE_PATH AMP_FS_ROOT_DIR "/pack.bin"
#define JS_JSON_PATH AMP_FS_ROOT_DIR "/py.json"
#define OS_APP_PATH AMP_FS_ROOT_DIR "/app.zip"
#define OS_KERNEL_PATH AMP_FS_ROOT_DIR "/os_kernel.bin"

int ota_load_pyapp(void *ota_ctx);
int32_t amp_app_upgrade_service(void *mqtt_handle);
void internal_sys_upgrade_start(void *ctx);
int ota_install_pyapp(void *ota_ctx, char *store_file, int store_file_len, char *install_path);
int install_pyapp(char *store_file, char *install_path);

#endif /* APP_UPGRADE_H */
