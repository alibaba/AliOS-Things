/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef APP_UPGRADE_H
#define APP_UPGRADE_H

#include <stdint.h>

#define AMP_OTA_MODULE_NAME "jsapp"

#define SUBDEV_FILE_PATH   AMP_FS_ROOT_DIR"/pack.bin"
#define JS_JSON_PATH       AMP_FS_ROOT_DIR"/js.json"
#define OS_APP_PATH        AMP_FS_ROOT_DIR"/os_app.bin"
#define OS_KERNEL_PATH     AMP_FS_ROOT_DIR"/os_kernel.bin"


int ota_load_jsapp(void *ota_ctx);
int32_t amp_app_upgrade_service(void *mqtt_handle);
void internal_sys_upgrade_start(void *ctx);
#endif /* APP_UPGRADE_H */
