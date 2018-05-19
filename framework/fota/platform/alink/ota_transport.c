/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <cJSON.h>
#include <alink_export.h>
#include <device.h>

#include "ota_log.h"
#include "ota_util.h"
#include "ota_version.h"
#include "ota_transport.h"

#define POST_OTA_STATUS_METHOD "ota/postDeviceUpgradeStatus"
#define POST_OTA_RESULT_METHOD "device.updateVersion"
#define POST_OTA_STATUS_DATA   "{\"version\":\"%s\",\"step\":\"%d\",\"stepPercent\":\"%d\"}"
#define POST_OTA_RESULT_DATA   "{\"uuid\" :\"%s\",\"version\":\"%s;APP2.0;OTA1.0\"}"

typedef struct ota_device_info {
    const char *product_key;
    const char *device_name;
    const char *uuid;
} OTA_device_info;

OTA_device_info g_device_info;

void platform_ota_init( void *info)
{
    if (info == NULL) {
        OTA_LOG_E("ota device info is null");
        return;
    }
    OTA_device_info *device_info = (OTA_device_info *)info;
    g_device_info.uuid = device_info->uuid;    
}

int8_t platform_ota_parse_requset(const char *request, int *buf_len,
                                  ota_request_params *request_parmas)
{
    return 0;
}

int8_t platform_ota_parse_response(const char *response, int buf_len,
                                   ota_response_params *response_parmas)
{
    cJSON *root = cJSON_Parse(response);
    if (!root) {
        OTA_LOG_E("Error before: [%s]\n", cJSON_GetErrorPtr());
        goto parse_failed;
    } else {
        char *info = cJSON_Print(root);
        OTA_LOG_I("root is %s", info);
        cJSON_free(info);

        cJSON *uuid = cJSON_GetObjectItem(root, "uuid");
        if (!uuid) {
            OTA_LOG_E("resourceUrl get error.");
            goto parse_failed;
        }
        strncpy(response_parmas->device_uuid, uuid->valuestring,
                (sizeof response_parmas->device_uuid)-1);

        cJSON *resourceUrl = cJSON_GetObjectItem(root, "resourceUrl");
        if (!resourceUrl) {
            OTA_LOG_E("resourceUrl get error.");
            goto parse_failed;
        }
        strncpy(response_parmas->download_url, resourceUrl->valuestring,
                (sizeof response_parmas->download_url)-1);

        cJSON *md5 = cJSON_GetObjectItem(root, "md5");
        if (!md5) {
            OTA_LOG_E("md5 get error.");
            goto parse_failed;
        }

        memset(response_parmas->md5, 0, sizeof response_parmas->md5);
        strncpy(response_parmas->md5, md5->valuestring, (sizeof response_parmas->md5)-1);

        cJSON *size = cJSON_GetObjectItem(root, "size");
        if (!size) {
            OTA_LOG_E("size get error.");
            goto parse_failed;
        }

        response_parmas->frimware_size = size->valueint;

        cJSON *version = cJSON_GetObjectItem(root, "version");
        if (!version) {
            OTA_LOG_E("version get error.");
            goto parse_failed;
        }

        OTA_LOG_I(" response version %s", version->valuestring);
        char *upgrade_version = strtok(version->valuestring, "_");
        if (!upgrade_version) {
            strncpy(response_parmas->primary_version, version->valuestring,
                    (sizeof response_parmas->primary_version)-1);
        } else {
            strncpy(response_parmas->primary_version, upgrade_version,
                    (sizeof response_parmas->primary_version)-1);

            upgrade_version = strtok(NULL, "_");
            if (upgrade_version) {
                strncpy(response_parmas->secondary_version, upgrade_version,
                        (sizeof response_parmas->secondary_version)-1);
            }
            OTA_LOG_I("response primary_version = %s, secondary_version = %s",
                      response_parmas->primary_version, response_parmas->secondary_version);
        }

    }

    goto parse_success;

parse_failed:
    if (root) {
        cJSON_Delete(root);
    }
    return -1;
parse_success:
    if (root) {
        cJSON_Delete(root);
    }
    return 0;
}


int8_t platform_ota_parse_cancel_responce(const char *response, int buf_len,
                                          ota_response_params *response_parmas)
{
    cJSON *root = cJSON_Parse(response);
    if (!root) {
        OTA_LOG_E("Error before: [%s]\n", cJSON_GetErrorPtr());
        goto parse_failed;
    } else {
        char *info = cJSON_Print(root);
        OTA_LOG_I("root is %s", info);
        cJSON_free(info);

        cJSON *uuid = cJSON_GetObjectItem(root, "uuid");
        if (!uuid) {
            OTA_LOG_E("uuid get error.");
            goto parse_failed;
        }

        strncpy(response_parmas->device_uuid, uuid->valuestring, (sizeof response_parmas->device_uuid)-1);
    }
    goto parse_success;

parse_failed:
    if (root) {
        cJSON_Delete(root);
    }
    return -1;
parse_success:
    if (root) {
        cJSON_Delete(root);
    }
    return 0;
}


int8_t platform_ota_status_post(int status, int percent)
{
    int ret = -1;
    char buff[256];
    memset(&buff, 0x00, sizeof(buff));
    if (percent < 0 || percent > 100) {
        OTA_LOG_E("percent error !(status = %d, percent = %d)\n", status, percent);
        percent = 0;
    }
    const char *ota_version = (const char *)ota_get_ota_version();
    snprintf(buff, sizeof(buff), POST_OTA_STATUS_DATA, ota_version, status, percent);
    ret = aos_cloud_report(POST_OTA_STATUS_METHOD, buff, NULL, NULL);
    OTA_LOG_I("alink_ota_status_post: %s, ret=%d", buff, ret);
    return ret;
}


int8_t platform_ota_result_post(void)
{
    int ret = -1;
    char buff[256] = {0};

    snprintf(buff, sizeof buff, POST_OTA_RESULT_DATA, (char *)platform_ota_get_id(),
             (const char *)ota_get_system_version());
    ret = aos_cloud_report(POST_OTA_RESULT_METHOD, buff, NULL, NULL);

    OTA_LOG_D("alink_ota_status_post: %s, ret=%d\n", buff, ret);
    return ret;
}

int8_t platform_ota_publish_request(ota_request_params *request_parmas)
{
    return 0;
}

int8_t platform_ota_subscribe_upgrade(aos_cloud_cb_t msgCallback)
{
    return aos_cloud_register_callback(ALINK_UPGRADE_DEVICE, msgCallback);
}

int8_t platform_ota_cancel_upgrade(aos_cloud_cb_t msgCallback)
{
    return aos_cloud_register_callback(ALINK_CANCEL_UPGRADE_DEVICE, msgCallback);
}

extern char *config_get_main_uuid(void);

const char *platform_ota_get_id(void)
{
    return g_device_info.uuid;
}

int8_t platform_destroy_connect(void){
    return 0;
}


