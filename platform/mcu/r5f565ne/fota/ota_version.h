/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _OTA_VERSION_H_
#define _OTA_VERSION_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#undef MAX_VERSION_LEN
#define MAX_VERSION_LEN   (64)

typedef struct version_config {
    char dev_version[MAX_VERSION_LEN];
    char ota_version[MAX_VERSION_LEN];
    char app_version[MAX_VERSION_LEN];
    char system_version[MAX_VERSION_LEN];
} version_config_t;


const char *ota_get_system_version(void);
const char *ota_get_dev_version(void);


void ota_set_ota_version(const char *version);
void ota_set_dev_version(const char *dev_version);

#ifdef __cplusplus
}
#endif

#endif
