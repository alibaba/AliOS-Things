/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef OTA_BREEZE_H
#define OTA_BREEZE_H

#include <breeze_export.h>

#define OTA_BREEZE_FW_VER_LEN (32)

typedef void (*ota_breeze_get_data_t)(breeze_otainfo_t *breeze_info);

typedef struct {
    unsigned char fw_ver[OTA_BREEZE_FW_VER_LEN];
    unsigned int  fw_ver_len;
} ota_breeze_version_t;

typedef struct {
    unsigned char         is_ota_enable;
    ota_breeze_version_t  verison;
    ota_breeze_get_data_t get_dat_cb;
} ota_breeze_service_manage_t;

int ota_breeze_service_init(ota_breeze_service_manage_t *ota_manage);

#endif /*OTA_BREEZE_H*/

