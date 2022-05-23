/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __OTA_SERVICE_H__
#define __OTA_SERVICE_H__

#include "esp_partition.h"
#include "periph_service.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OTA_SERVICE_ERR_REASON_BASE  (0x90000)

typedef struct {
    int task_stack;           /*!< >0 Service task stack; =0 with out task created */
    int task_prio;            /*!< Service task priority (based on freeRTOS priority) */
    int task_core;            /*!< Service task running in core (0 or 1) */
    periph_service_cb evt_cb; /*!< Service callback function */
    void *cb_ctx;             /*!< Callback context */
} ota_service_config_t;

#define OTA_SERVICE_DEFAULT_CONFIG() \
    {                                \
        .task_stack = 4096,          \
        .task_prio = 5,              \
        .task_core = 1,              \
        .evt_cb = NULL,              \
        .cb_ctx = NULL,              \
    }

typedef struct {
    esp_partition_type_t type;
    char *label;
    char *uri;
    char *cert_pem;
} ota_node_attr_t;

typedef enum {
    OTA_SERV_EVENT_TYPE_RESULT,
    OTA_SERV_EVENT_TYPE_FINISH
} ota_service_event_type_t;

typedef enum {
    OTA_SERV_ERR_REASON_UNKNOWN               = ESP_FAIL,
    OTA_SERV_ERR_REASON_SUCCESS               = ESP_OK,
    OTA_SERV_ERR_REASON_NULL_POINTER          = OTA_SERVICE_ERR_REASON_BASE + 1,
    OTA_SERV_ERR_REASON_URL_PARSE_FAIL        = OTA_SERVICE_ERR_REASON_BASE + 2,
    OTA_SERV_ERR_REASON_ERROR_VERSION         = OTA_SERVICE_ERR_REASON_BASE + 3,
    OTA_SERV_ERR_REASON_NO_HIGHER_VERSION     = OTA_SERVICE_ERR_REASON_BASE + 4,
    OTA_SERV_ERR_REASON_ERROR_MAGIC_WORD      = OTA_SERVICE_ERR_REASON_BASE + 5,
    OTA_SERV_ERR_REASON_ERROR_PROJECT_NAME    = OTA_SERVICE_ERR_REASON_BASE + 6,
    OTA_SERV_ERR_REASON_FILE_NOT_FOUND        = OTA_SERVICE_ERR_REASON_BASE + 7,
    OTA_SERV_ERR_REASON_PARTITION_NOT_FOUND   = OTA_SERVICE_ERR_REASON_BASE + 8,
    OTA_SERV_ERR_REASON_PARTITION_WT_FAIL     = OTA_SERVICE_ERR_REASON_BASE + 9,
    OTA_SERV_ERR_REASON_PARTITION_RD_FAIL     = OTA_SERVICE_ERR_REASON_BASE + 10,
    OTA_SERV_ERR_REASON_STREAM_INIT_FAIL      = OTA_SERVICE_ERR_REASON_BASE + 11,
    OTA_SERV_ERR_REASON_STREAM_RD_FAIL        = OTA_SERVICE_ERR_REASON_BASE + 12,
    OTA_SERV_ERR_REASON_GET_NEW_APP_DESC_FAIL = OTA_SERVICE_ERR_REASON_BASE + 13,
} ota_service_err_reason_t;

typedef struct {
    ota_node_attr_t node;
    ota_service_err_reason_t (*prepare)(void **handle, ota_node_attr_t *node);
    ota_service_err_reason_t (*need_upgrade)(void *handle, ota_node_attr_t *node);
    ota_service_err_reason_t (*execute_upgrade)(void *handle, ota_node_attr_t *node);
    ota_service_err_reason_t (*finished_check)(void *handle, ota_node_attr_t *node, ota_service_err_reason_t result);
    bool reboot_flag;
    bool break_after_fail;
} ota_upgrade_ops_t;

typedef struct {
    uint8_t   id;
    ota_service_err_reason_t result;
} ota_result_t;

/**
  * @brief     Create the OTA service instance
  *
  * @param     config  configuration of the OTA service
  *
  * @return
  *    - NULL:  Failed
  *    - Others: Success
  */
periph_service_handle_t ota_service_create(ota_service_config_t *config);

/**
  * @brief     Configure the upgrade parameter
  * @Note      This function is not thread safe
  *
  * This function will set the parameter table to ota service,
  * and the ota service will upgrade the partitions defined in the table one by one,
  *
  * @param[in]  handle      pointer to 'periph_service_handle_t' structure
  * @param[in]  list        pointer to 'ota_upgrade_ops_t' structure
  * @param[in]  list_len    length of the 'list'
  *
  * @return
  *    - ESP_OK:  Success
  *    - Others:  Failed
  */
esp_err_t ota_service_set_upgrade_param(periph_service_handle_t handle, ota_upgrade_ops_t *list, int list_len);

#ifdef __cplusplus
}
#endif

#endif /* __OTA_SERVICE_H__ */
