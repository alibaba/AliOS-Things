/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2021 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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

#ifndef _MRM_CLIENT_H_
#define _MRM_CLIENT_H_

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_MRM_CLIENT_VERSION  "ESP32 MRM/1.0"
#define DEFAULT_MRM_GROUP_ADDR      "239.255.255.252"
#define DEFAULT_MRM_SYNC_SOCK_PORT  (1900)

#define DEFAULT_MRM_BUFFER_SIZE     (2*1024)
#define DEFAULT_MRM_TASK_STACK      (4*1024)
#define DEFAULT_MRM_TASK_PRIO       (5)

typedef struct esp_mrm_client *esp_mrm_client_handle_t;

/**
 * @brief MRM client event
 */
typedef enum {
    MRM_EVENT_NONE = 0,
    MRM_EVENT_SET_URL,
    MRM_EVENT_SET_SYNC,
    MRM_EVENT_GET_PTS,
    MRM_EVENT_GET_TSF,
    MRM_EVENT_SYNC_FAST,
    MRM_EVENT_SYNC_SLOW,
    MRM_EVENT_PLAY_STOP
} mrm_event_t;

/**
 * @brief MRM client event message
 */
typedef struct {
    mrm_event_t type;           /*!< MRM client event type */
    int data_len;               /*!< Data length */
    void *data;                 /*!< TX/RX data */
} mrm_event_msg_t;

typedef int (*mrm_event_handle)(mrm_event_msg_t *event, void *ctx);

/**
 * @brief MRM client configurations
 */
typedef struct
{
    mrm_event_handle        event_handler;      /*!< MRM client event handler */
    char                    *group_addr;        /*!< MRM client multicast group addr */
    int                     sync_sock_port;     /*!< MRM client sync socket port */
    int                     data_sock_port;     /*!< MRM client data socket port */
    void                    *ctx;               /*!< MRM client User context */
} esp_mrm_client_config_t;

/**
 * @brief Creates mrm client handle based on the configuration
 *
 * @param config    mrm configuration structure
 *
 * @return mrm_client_handle if successfully created, NULL on error
 */
esp_mrm_client_handle_t esp_mrm_client_create(esp_mrm_client_config_t *config);

/**
 * @brief Destroys the client handle
 *
 * @param client    mrm client handle
 *
 * @return ESP_OK
 */
esp_err_t esp_mrm_client_destroy(esp_mrm_client_handle_t client);

/**
 * @brief mrm master client start
 *
 * @param client    mrm client handle
 * @param URL       mrm client play url
 *
 * @return ESP_OK
 */
esp_err_t esp_mrm_client_master_start(esp_mrm_client_handle_t client, const char *URL);

/**
 * @brief  mrm master client stop
 *
 * @param client    mrm client handle
 *
 * @return ESP_OK
 */
esp_err_t esp_mrm_client_master_stop(esp_mrm_client_handle_t client);

/**
 * @brief mrm slave client start
 *
 * @param client    mrm client handle
 *
 * @return ESP_OK
 */
esp_err_t esp_mrm_client_slave_start(esp_mrm_client_handle_t client);

/**
 * @brief  mrm slave client stop
 *
 * @param client    mrm client handle
 *
 * @return ESP_OK
 */
esp_err_t esp_mrm_client_slave_stop(esp_mrm_client_handle_t client);

#ifdef __cplusplus
}
#endif

#endif
