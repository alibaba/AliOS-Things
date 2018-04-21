/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifdef SERVICE_OTA_ENABLED

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

#include "lite-utils.h"
#include "lite-system.h"

#include "iotx_cm_ota.h"
#include "ota_internal.h"
#include "iotx_cm_common.h"
#include "iotx_cm_connection.h"
#include "iot_export_cm.h"
#include "iot_import.h"
#include "iot_export_ota.h"

static int g_cm_ota_status = 0;

static void _fota_fetch(void* user_data, int is_fetch, uint32_t size_file, char *purl, char *version)
{
    iotx_cm_conntext_t* cm_ctx = (iotx_cm_conntext_t*)user_data;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_parameter);
        return;
    }

    /* start */
    if (0 == is_fetch) {
        iotx_cm_fota_parameter_t ota_parameter = {0};

        ota_parameter.purl = purl;
        ota_parameter.size_file = size_file;
        ota_parameter.version = version;

        if (cm_ctx->fota_func) cm_ctx->fota_func(cm_ctx, &ota_parameter, cm_ctx->fota_user_context);
    } else {
        g_cm_ota_status = 1;
    }
}

static void _cota_fetch(void* user_data, int is_fetch, char* configId, uint32_t configSize,
                        char *sign, char *signMethod, char* url, char* getType)
{
    iotx_cm_conntext_t* cm_ctx = (iotx_cm_conntext_t*)user_data;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_parameter);
        return;
    }

    /* start */
    if (0 == is_fetch) {
        iotx_cm_cota_parameter_t ota_parameter = {0};

        ota_parameter.configId = configId;
        ota_parameter.configSize = configSize;
        ota_parameter.sign = sign;
        ota_parameter.signMethod = signMethod;
        ota_parameter.url = url;
        ota_parameter.getType = getType;

        if (cm_ctx->cota_func) cm_ctx->cota_func(cm_ctx, &ota_parameter, cm_ctx->cota_user_context);
    } else {
        g_cm_ota_status = 1;
    }
}

static void cm_find_ota_connectivity_handler(void* list_node, va_list* params)
{
    iotx_cm_connectivity_t* connectivity = (iotx_cm_connectivity_t*)list_node;
    iotx_cm_conntext_t* cm_ctx;
    iotx_cm_connectivity_types_t connectivity_type;

    cm_ctx = va_arg(*params, iotx_cm_conntext_t*);
    connectivity_type = va_arg(*params, iotx_cm_connectivity_types_t);

    assert(cm_ctx && connectivity_type < IOTX_CM_CONNECTIVITY_TYPE_MAX);

    if (connectivity && connectivity->is_connected && connectivity->type == connectivity_type) {
        cm_ctx->target_connectivity = connectivity;
    }
}

void* iotx_cm_ota_init(iotx_cm_conntext_t* cm_ctx, const char* version)
{
    linked_list_t* list;
    void* h_ota = NULL;
    iotx_device_info_t* device_info = iotx_device_info_get();
    iotx_connection_t* connection = NULL;

    connection = (iotx_connection_t*)cm_ctx->target_connectivity->context;

    if (NULL == cm_ctx || NULL == version) {
        CM_ERR(cm_log_error_parameter);
        return NULL;
    }

    list = cm_ctx->list_connectivity;

    cm_ctx->target_connectivity = NULL;
    linked_list_iterator(list, cm_find_ota_connectivity_handler, cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD);

    if (cm_ctx->target_connectivity) {
        h_ota = IOT_OTA_Init(device_info->product_key, device_info->device_name, connection->context);
        if (NULL == h_ota) {
            CM_ERR(cm_log_error_fail);
            return NULL;
        }
    } else {
        CM_WARNING(cm_log_warning_cloud_disconnected);
        return NULL;
    }

    cm_ctx->ota_handler = h_ota;

    if (0 != IOT_OTA_ReportVersion(h_ota, version)) {
        CM_ERR(cm_log_error_fail);
        IOT_OTA_Deinit(h_ota);
        return NULL;
    }

    iotx_ota_set_fetch_callback(h_ota, _fota_fetch, cm_ctx);
    iotx_ota_set_cota_fetch_callback(h_ota, _cota_fetch, cm_ctx);

    return h_ota;
}

int iotx_cm_ota_yield(iotx_cm_conntext_t* cm_ctx, iotx_cm_ota_t* cm_ota)
{
    uint32_t len, size_downloaded, size_file;
    uint32_t percent = 0;
    static uint32_t pre_percent = 0;
    static unsigned long long pre_report_time = 0;
    unsigned long long report_time = 0;

    if (NULL == cm_ctx || NULL == cm_ctx->ota_handler || NULL == cm_ota || NULL == cm_ota->buffer) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    len = IOT_OTA_FetchYield(cm_ctx->ota_handler, cm_ota->buffer, cm_ota->buffer_length, 1);
    if (len <= 0) {
        IOT_OTA_ReportProgress(cm_ctx->ota_handler, IOT_OTAP_FETCH_FAILED, NULL);
        cm_ota->is_more = 0;
        cm_ota->result = IOT_OTAP_FETCH_FAILED;
        cm_ota->progress = IOT_OTAP_FETCH_FAILED;
        CM_ERR(cm_log_error_fail);
        return FAIL_RETURN;
    }

    cm_ota->buffer_length = len;

    /* get OTA information */
    IOT_OTA_Ioctl(cm_ctx->ota_handler, IOT_OTAG_FETCHED_SIZE, &size_downloaded, 4);
    IOT_OTA_Ioctl(cm_ctx->ota_handler, IOT_OTAG_FILE_SIZE, &size_file, 4);
    if (IOTX_CM_OTA_TYPE_FOTA == cm_ota->ota_type) {
        char version[128], md5sum[33];
        IOT_OTA_Ioctl(cm_ctx->ota_handler, IOT_OTAG_MD5SUM, md5sum, 33);
        IOT_OTA_Ioctl(cm_ctx->ota_handler, IOT_OTAG_VERSION, version, 128);
    }

    percent = (size_downloaded * 100) / size_file;
    report_time = HAL_UptimeMs();
    if ((((percent - pre_percent) > 5) && ((report_time - pre_report_time) > 50)) || (percent >= IOT_OTAP_FETCH_PERCENTAGE_MAX) ){
        IOT_OTA_ReportProgress(cm_ctx->ota_handler, percent, NULL);
        pre_percent = percent;
        pre_report_time = report_time;
    }

    if (percent < IOT_OTAP_FETCH_PERCENTAGE_MAX) {
        cm_ota->is_more = 1;
        cm_ota->result = 0;
        cm_ota->progress = percent;
    } else {
        uint32_t firmware_valid;

        cm_ota->is_more = 0;
        cm_ota->progress = percent;
        /* finished */
        if (1 == g_cm_ota_status) {
            IOT_OTA_Ioctl(cm_ctx->ota_handler, IOT_OTAG_CHECK_FIRMWARE, &firmware_valid, 4);
            if (0 == firmware_valid) {
                CM_ERR(cm_log_error_fail);
                cm_ota->result = IOT_OTAP_CHECK_FALIED;
            } else {
                CM_INFO(cm_log_info_firmware);
                cm_ota->result = 0;
            }
        } else {
            CM_ERR(cm_log_error_fail_ota);
            cm_ota->result = IOT_OTAP_GENERAL_FAILED;
        }
    }

    return SUCCESS_RETURN;
}

int iotx_cm_ota_deinit(iotx_cm_conntext_t* cm_ctx)
{
    if (NULL == cm_ctx || NULL == cm_ctx->ota_handler) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    return IOT_OTA_Deinit(cm_ctx->ota_handler);
}


int iotx_cm_ota_request_image(iotx_cm_conntext_t* cm_ctx, const char* version)
{
    if (NULL == cm_ctx || NULL == cm_ctx->ota_handler) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    return IOT_OTA_RequestImage(cm_ctx->ota_handler, version);
}


int iotx_cm_ota_get_config(iotx_cm_conntext_t* cm_ctx, const char* configScope, const char* getType, const char* attributeKeys)
{
    if (NULL == cm_ctx || NULL == cm_ctx->ota_handler) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    return IOT_OTA_GetConfig(cm_ctx->ota_handler, configScope, getType, attributeKeys);
}

#endif /* SERVICE_OTA_ENABLED */

