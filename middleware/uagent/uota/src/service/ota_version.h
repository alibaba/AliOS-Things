/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _OTA_VERSION_H_
#define _OTA_VERSION_H_

#ifdef __AOS__
#include "aos/aos.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#undef MAX_VERSION_LEN
#define MAX_VERSION_LEN   (64)

#define KEY_OTA_VER  "ota_ota_version"
#define KEY_DEV_VER  "ota_dev_version"
#define KEY_APP_VER  "app_version"

#ifndef SYSINFO_APP_VERSION
#define SYSINFO_APP_VERSION "app-1.0.0-20180101.1000"
#endif

#ifndef SYSINFO_KERNEL_VERSION
#define SYSINFO_KERNEL_VERSION "AOS-R-2.0.0"
#endif

typedef struct version_config {
    char dev_version[MAX_VERSION_LEN];
    char ota_version[MAX_VERSION_LEN];
    char app_version[MAX_VERSION_LEN];
    char system_version[MAX_VERSION_LEN];
} version_config_t;


const char *ota_get_system_version(void);
const char *ota_get_dev_version(void);
const char *ota_get_ota_version(void);
const char *ota_get_os_version(void);
const char *ota_get_app_version(void);

void ota_set_ota_version(const char *version);
void ota_set_dev_version(const char *dev_version);
#ifdef __cplusplus
}
#endif
#endif
