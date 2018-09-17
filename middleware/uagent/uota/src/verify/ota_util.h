/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _OTA_UTIL_H_
#define _OTA_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "ota_hal_plat.h"

#undef MAX_VERSION_LEN
#define MAX_VERSION_LEN 64

typedef enum {
    OTA_DIFF_FAILED = -9,
    OTA_CHECK_IMAGE_FAILED = -8,
    OTA_RSA_VERIFY_FAILED = -7,
    OTA_REBOOT_FAILED = -6,
    OTA_UPGRADE_FAILED = -5,
    OTA_CHECK_FAILED = -4,
    OTA_DECOMPRESS_FAILED = -3,
    OTA_DOWNLOAD_FAILED = -2,
    OTA_INIT_FAILED = -1,
    OTA_INIT = 0,
    OTA_DOWNLOAD = 1,
    OTA_DECOMPRESS = 2,
    OTA_CHECK = 3,
    OTA_UPGRADE = 4,
    OTA_REBOOT = 5,
    OTA_REBOOT_SUCCESS = 6,
    OTA_CANCEL = 7,
    OTA_MAX
} OTA_STATUS_T;

typedef enum {
    OTA_DOWNLOAD_SIGN_INIT_FAIL = -7,
    OTA_DOWNLOAD_RECV_FAIL = -6,
    OTA_DOWNLOAD_SEND_FAIL = -5,
    OTA_DOWNLOAD_SOCKET_FAIL = -4,
    OTA_DOWNLOAD_IP_FAIL = -3,
    OTA_DOWNLOAD_URL_FAIL = -2,
    OTA_DOWNLOAD_FAIL = -1,
    OTA_DOWNLOAD_CONTINUE = 0,
    OTA_DOWNLOAD_CANCEL = 1,
    OTA_DOWNLOAD_FINISH = 2
} OTA_DOWNLOAD_T;

typedef enum {
    OTA_UPDATE_WAY_BEGIN,
    OTA_INTERACTION,
    OTA_SILENT,
    OTA_FORCE,
    OTA_UPDATE_WAY_END
} OTA_ENUM_UPDATE_WAY;

typedef struct {
    OTA_STATUS_T status;
    OTA_ENUM_UPDATE_WAY  update_way;
    OTA_ENUM_UPDATE_TYPE update_type;
    OTA_ENUM_FIRMWARE_TYPE firmware_type;
    void *mutex;
    uint32_t splict_size;
    uint8_t diff_version;

} ota_info_t;

void ota_status_init(void);

void ota_status_deinit(void);

void ota_set_status(OTA_STATUS_T status);

OTA_STATUS_T ota_get_status(void);

int8_t ota_status_post(int percent);

void ota_set_update_type(OTA_ENUM_UPDATE_TYPE type);

OTA_ENUM_UPDATE_TYPE ota_get_update_type(void);

const char *ota_get_version(void);

void ota_set_version(const char *ota_version);

OTA_ENUM_FIRMWARE_TYPE ota_get_firmware_type(void);

void ota_set_firmware_type(OTA_ENUM_FIRMWARE_TYPE type);

uint8_t ota_get_diff_version(void);

void ota_set_diff_version(uint8_t ver);

uint32_t ota_get_splict_size(void);

void ota_set_splict_size(uint32_t size);

#ifdef __cplusplus
}
#endif

#endif
