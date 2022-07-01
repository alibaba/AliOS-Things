/**
 * \file            lwesp_mqtt_client_api.h
 * \brief           MQTT client API
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.2-dev
 */
#ifndef LWESP_HDR_APP_MQTT_CLIENT_API_H
#define LWESP_HDR_APP_MQTT_CLIENT_API_H

#include "lwesp/lwesp.h"
#include "lwesp/apps/lwesp_mqtt_client.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \ingroup         LWESP_APPS
 * \defgroup        LWESP_APP_MQTT_CLIENT_API MQTT client API
 * \brief           Sequential, single thread MQTT client API
 * \{
 */

/**
 * \brief           MQTT API client structure
 */
struct lwesp_mqtt_client_api;

/**
 * \brief           MQTT API RX buffer
 */
typedef struct lwesp_mqtt_client_api_buf {
    char* topic;                                /*!< Topic data */
    size_t topic_len;                           /*!< Topic length */
    uint8_t* payload;                           /*!< Payload data */
    size_t payload_len;                         /*!< Payload length */
    lwesp_mqtt_qos_t qos;                       /*!< Quality of service */
} lwesp_mqtt_client_api_buf_t;

/**
 * \brief           Pointer to \ref lwesp_mqtt_client_api structure
 */
typedef struct lwesp_mqtt_client_api* lwesp_mqtt_client_api_p;

/**
 * \brief           Pointer to \ref lwesp_mqtt_client_api_buf_t structure
 */
typedef struct lwesp_mqtt_client_api_buf* lwesp_mqtt_client_api_buf_p;

lwesp_mqtt_client_api_p lwesp_mqtt_client_api_new(size_t tx_buff_len, size_t rx_buff_len);
void                    lwesp_mqtt_client_api_delete(lwesp_mqtt_client_api_p client);
lwesp_mqtt_conn_status_t    lwesp_mqtt_client_api_connect(lwesp_mqtt_client_api_p client, const char* host, lwesp_port_t port, const lwesp_mqtt_client_info_t* info);
lwespr_t                lwesp_mqtt_client_api_close(lwesp_mqtt_client_api_p client);
lwespr_t                lwesp_mqtt_client_api_subscribe(lwesp_mqtt_client_api_p client, const char* topic, lwesp_mqtt_qos_t qos);
lwespr_t                lwesp_mqtt_client_api_unsubscribe(lwesp_mqtt_client_api_p client, const char* topic);
lwespr_t                lwesp_mqtt_client_api_publish(lwesp_mqtt_client_api_p client, const char* topic, const void* data, size_t btw, lwesp_mqtt_qos_t qos, uint8_t retain);
uint8_t                 lwesp_mqtt_client_api_is_connected(lwesp_mqtt_client_api_p client);
lwespr_t                lwesp_mqtt_client_api_receive(lwesp_mqtt_client_api_p client, lwesp_mqtt_client_api_buf_p* p, uint32_t timeout);
void                    lwesp_mqtt_client_api_buf_free(lwesp_mqtt_client_api_buf_p p);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_APP_MQTT_CLIENT_H */
