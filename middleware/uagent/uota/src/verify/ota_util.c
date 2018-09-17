/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "ota_service.h"
#include "ota_manifest.h"
#include "ota_transport.h"
#include "ota_hal_os.h"
#include "ota_util.h"
#include "ota_log.h"
#include "ota_version.h"


static ota_info_t g_ota_info_storage = { .update_way    = OTA_SILENT,
                                         .update_type   = OTA_ALL,
                                         .firmware_type = OTA_RAW,
                                         .mutex         = NULL,
                                         .splict_size   = 0,
                                         .diff_version  = 0 };

static ota_info_t *g_ota_info = &g_ota_info_storage;

void ota_status_init()
{
    g_ota_info->status = OTA_INIT;
    g_ota_info->mutex  = ota_mutex_init();
}

void ota_status_deinit()
{
    if (g_ota_info->mutex != NULL) {
        ota_mutex_destroy(g_ota_info->mutex);
        g_ota_info->mutex = NULL;
    }
}

OTA_ENUM_UPDATE_TYPE ota_get_update_type(void)
{
    OTA_ENUM_UPDATE_TYPE type;
    if (g_ota_info->mutex != NULL)
    ota_mutex_lock(g_ota_info_storage.mutex);
    type = g_ota_info->update_type;
    if (g_ota_info->mutex != NULL)
    ota_mutex_unlock(g_ota_info_storage.mutex);
    return type;
}

void ota_set_update_type(OTA_ENUM_UPDATE_TYPE type)
{
    if (g_ota_info->mutex != NULL)
    ota_mutex_lock(g_ota_info_storage.mutex);
    g_ota_info->update_type = type;
    if (g_ota_info->mutex != NULL)
    ota_mutex_unlock(g_ota_info_storage.mutex);
}

OTA_ENUM_FIRMWARE_TYPE ota_get_firmware_type(void)
{
    OTA_ENUM_FIRMWARE_TYPE type;
    if (g_ota_info->mutex != NULL)
    ota_mutex_lock(g_ota_info_storage.mutex);
    type = g_ota_info->firmware_type;
    if (g_ota_info->mutex != NULL)
    ota_mutex_unlock(g_ota_info_storage.mutex);
    return type;
}

void ota_set_firmware_type(OTA_ENUM_FIRMWARE_TYPE type)
{
    if (g_ota_info->mutex != NULL)
    ota_mutex_lock(g_ota_info_storage.mutex);
    g_ota_info->firmware_type = type;
    if (g_ota_info->mutex != NULL)
    ota_mutex_unlock(g_ota_info_storage.mutex);
}


OTA_STATUS_T ota_get_status(void)
{
    OTA_STATUS_T status;
    if (g_ota_info->mutex != NULL)
    ota_mutex_lock(g_ota_info_storage.mutex);
    status = g_ota_info->status;
    if (g_ota_info->mutex != NULL)
    ota_mutex_unlock(g_ota_info_storage.mutex);
    return status;
}

void ota_set_status(OTA_STATUS_T status)
{
    if (g_ota_info->mutex != NULL)
    ota_mutex_lock(g_ota_info_storage.mutex);
    g_ota_info->status = status;
    if (g_ota_info->mutex != NULL)
    ota_mutex_unlock(g_ota_info_storage.mutex);
}

int8_t ota_status_post(int percent)
{
    ota_service_manager *ctx = (ota_service_manager *)get_ota_service_manager();
    ota_transport       *trans =
      (ota_transport *)ota_get_transport(ctx->trans_protcol);
    return trans->status_post(g_ota_info->status, percent);
}

const char *ota_get_version(void)
{
    const char *version = NULL;
    if (g_ota_info->mutex != NULL)
    ota_mutex_lock(g_ota_info_storage.mutex);
    version = ota_get_ota_version();
    if (g_ota_info->mutex != NULL)
    ota_mutex_unlock(g_ota_info_storage.mutex);
    return version;
}

void ota_set_version(const char *ota_version)
{
    if (g_ota_info->mutex != NULL)
    ota_mutex_lock(g_ota_info_storage.mutex);
    ota_set_ota_version((char *)ota_version);
    if (g_ota_info->mutex != NULL)
    ota_mutex_unlock(g_ota_info_storage.mutex);
}

uint8_t ota_get_diff_version(void)
{
    uint8_t ver;
    if (g_ota_info->mutex != NULL)
    ota_mutex_lock(g_ota_info_storage.mutex);
    ver = g_ota_info->diff_version;
    if (g_ota_info->mutex != NULL)
    ota_mutex_unlock(g_ota_info_storage.mutex);
    return ver;
}

void ota_set_diff_version(uint8_t ver)
{
    if (g_ota_info->mutex != NULL)
    ota_mutex_lock(g_ota_info_storage.mutex);
    g_ota_info->diff_version = ver;
    if (g_ota_info->mutex != NULL)
    ota_mutex_unlock(g_ota_info_storage.mutex);
}

uint32_t ota_get_splict_size(void)
{
    uint32_t size;
    if (g_ota_info->mutex != NULL)
    ota_mutex_lock(g_ota_info_storage.mutex);
    size = g_ota_info->splict_size;
    if (g_ota_info->mutex != NULL)
    ota_mutex_unlock(g_ota_info_storage.mutex);
    return size;
}

void ota_set_splict_size(uint32_t size)
{
    if (g_ota_info->mutex != NULL)
    ota_mutex_lock(g_ota_info_storage.mutex);
    g_ota_info->splict_size = size;
    if (g_ota_info->mutex != NULL)
    ota_mutex_unlock(g_ota_info_storage.mutex);
}
