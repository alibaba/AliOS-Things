/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <hal/hal.h>
#include <aos/aos.h>
#include "ota_transport.h"
#include "ota_update_manifest.h"
#include "ota_util.h"
#include "ota_version.h"

#define TAG "ota"
#define KEY_OTA_VER  "ota_ota_version"
#define KEY_DEV_VER  "ota_dev_version"

version_config_t version_config;

const char *ota_get_ota_version(void)
{
    int len = MAX_VERSION_LEN;
    memset(version_config.ota_version, 0, MAX_VERSION_LEN);
    aos_kv_get(KEY_OTA_VER, version_config.ota_version, &len);
    //LOGI(TAG,"ota_get_ota_version=%s",version_config.ota_version);
    return version_config.ota_version;
}

void ota_set_ota_version(const char *str)
{
    if (str == NULL) {
        return ;
    }
    aos_kv_set(KEY_OTA_VER, str, MAX_VERSION_LEN, 1);
}

const char *ota_get_dev_version(void)
{
    int len = MAX_VERSION_LEN;
    memset(version_config.dev_version, 0, MAX_VERSION_LEN);
    aos_kv_get(KEY_DEV_VER, version_config.dev_version, &len);
    return version_config.dev_version;
}

void  ota_set_dev_version(const char *str)
{
    if (str == NULL) {
        return ;
    }
    aos_kv_set(KEY_DEV_VER, str, MAX_VERSION_LEN, 1);
}

const char *ota_get_system_version()
{
    return aos_get_os_version();
}

#ifdef SYSINFO_OS_BINS
const char   *aos_get_kernel_version(void)
{
    return (const char *)aos_version_get();
}

#define KEY_APP_VER  "app_version"

const char   *aos_get_app_version(void)
{
    int len = MAX_VERSION_LEN;
    memset(version_config.app_version, 0, MAX_VERSION_LEN);
    aos_kv_get(KEY_APP_VER, version_config.app_version, &len); 
    return version_config.app_version;
}
#endif 



