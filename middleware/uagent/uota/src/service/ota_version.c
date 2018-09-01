/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include "ota_transport.h"
#include "ota_manifest.h"
#include "ota_util.h"
#include "ota_version.h"
#include "ota_log.h"
#include "ota_hal_os.h"

#define KEY_OTA_VER  "ota_ota_version"
#define KEY_DEV_VER  "ota_dev_version"
#define KEY_APP_VER  "app_version"

version_config_t version_config;
const char *ota_get_ota_version(void)
{
    int len = MAX_VERSION_LEN;
    memset(version_config.ota_version, 0, MAX_VERSION_LEN);
    ota_kv_get(KEY_OTA_VER, version_config.ota_version, &len);
    return version_config.ota_version;
}

void ota_set_ota_version(const char *str)
{
    if (str == NULL) {
        return ;
    }
    ota_kv_set(KEY_OTA_VER, str, MAX_VERSION_LEN, 1);
}

const char *ota_get_dev_version(void)
{
    int len = MAX_VERSION_LEN;
    memset(version_config.dev_version, 0, MAX_VERSION_LEN);
    ota_kv_get(KEY_DEV_VER, version_config.dev_version, &len);
    return version_config.dev_version;
}

void  ota_set_dev_version(const char *str)
{
    if (str == NULL) {
        return ;
    }
    ota_kv_set(KEY_DEV_VER, str, MAX_VERSION_LEN, 1);
}

const char   *ota_get_kernel_version(void)
{
    return (const char *)SYSINFO_KERNEL_VERSION;
}

const char   *ota_get_app_version(void)
{
    int len = MAX_VERSION_LEN;
    memset(version_config.app_version, 0, MAX_VERSION_LEN);
  
    ota_kv_get(KEY_APP_VER, version_config.app_version, &len); 
    return version_config.app_version;
}

const char   *ota_get_system_version(void)
{
#ifdef OTA_MULTI_BINS
    int len = MAX_VERSION_LEN;
    memset(version_config.system_version, 0, MAX_VERSION_LEN);
    ota_snprintf(version_config.system_version, MAX_VERSION_LEN, "%s_%s", aos_get_kernel_version(), aos_get_app_version());
    return version_config.system_version;
#else
    return (const char *)SYSINFO_APP_VERSION;
#endif
}
