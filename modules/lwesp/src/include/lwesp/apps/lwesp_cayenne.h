/**
 * \file            lwesp_cayenne.h
 * \brief           MQTT client for Cayenne
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
#ifndef LWESP_HDR_APP_CAYENNE_H
#define LWESP_HDR_APP_CAYENNE_H

#include "lwesp/lwesp.h"
#include "lwesp/apps/lwesp_mqtt_client.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \ingroup         LWESP_APPS
 * \defgroup        LWESP_APP_CAYENNE_API MQTT client Cayenne API
 * \brief           MQTT client API for Cayenne
 * \{
 */

/**
 * \brief           Cayenne API version in string
 */
#ifndef LWESP_CAYENNE_API_VERSION
#define LWESP_CAYENNE_API_VERSION               "v1"
#endif

/**
* \brief           Cayenne host server
*/
#ifndef LWESP_CAYENNE_HOST
#define LWESP_CAYENNE_HOST                      "mqtt.mydevices.com"
#endif

/**
 * \brief           Cayenne port number
 */
#ifndef LWESP_CAYENNE_PORT
#define LWESP_CAYENNE_PORT                      1883
#endif

/**
 * \brief           MQTT default TX buffer len
 */
#ifndef LWESP_CAYENNE_BUFF_TX_LEN
#define LWESP_CAYENNE_BUFF_TX_LEN               256
#endif

/**
 * \brief           MQTT default RX buffer len
 */
#ifndef LWESP_CAYENNE_BUFF_RX_LEN
#define LWESP_CAYENNE_BUFF_RX_LEN               256
#endif

/**
 * \brief           Size of TX messages queue size, waiting to be sent to server
 */
#ifndef LWESP_CAYENNE_BUFF_TX_COUNT
#define LWESP_CAYENNE_BUFF_TX_COUNT             16
#endif

#define LWESP_CAYENNE_NO_CHANNEL                0xFFFE/*!< No channel macro */
#define LWESP_CAYENNE_ALL_CHANNELS              0xFFFF/*!< All channels macro */

/**
 * \brief           List of possible cayenne topics
 */
typedef enum {
    LWESP_CAYENNE_TOPIC_DATA,                   /*!< Data topic */
    LWESP_CAYENNE_TOPIC_COMMAND,                /*!< Command topic */
    LWESP_CAYENNE_TOPIC_CONFIG,
    LWESP_CAYENNE_TOPIC_RESPONSE,
    LWESP_CAYENNE_TOPIC_SYS_MODEL,
    LWESP_CAYENNE_TOPIC_SYS_VERSION,
    LWESP_CAYENNE_TOPIC_SYS_CPU_MODEL,
    LWESP_CAYENNE_TOPIC_SYS_CPU_SPEED,
    LWESP_CAYENNE_TOPIC_DIGITAL,
    LWESP_CAYENNE_TOPIC_DIGITAL_COMMAND,
    LWESP_CAYENNE_TOPIC_DIGITAL_CONFIG,
    LWESP_CAYENNE_TOPIC_ANALOG,
    LWESP_CAYENNE_TOPIC_ANALOG_COMMAND,
    LWESP_CAYENNE_TOPIC_ANALOG_CONFIG,
    LWESP_CAYENNE_TOPIC_END,                    /*!< Last entry */
} lwesp_cayenne_topic_t;

/**
 * \brief           Cayenne response types
 */
typedef enum {
    LWESP_CAYENNE_RESP_OK,                      /*!< Response OK */
    LWESP_CAYENNE_RESP_ERROR,                   /*!< Response error */
} lwesp_cayenne_resp_t;

/**
 * \brief           Cayenne events
 */
typedef enum {
    LWESP_CAYENNE_EVT_CONNECT,                  /*!< Connect to Cayenne event */
    LWESP_CAYENNE_EVT_DISCONNECT,               /*!< Disconnect from Cayenne event */
    LWESP_CAYENNE_EVT_DATA,                     /*!< Data received event */
} lwesp_cayenne_evt_type_t;

/**
 * \brief           Key/Value pair structure
 */
typedef struct {
    const char* key;                            /*!< Key string */
    size_t key_len;                             /*!< Length of key string in units of bytes */
    const char* value;                          /*!< Value string */
    size_t value_len;                           /*!< Length of value string in units of bytes */
} lwesp_cayenne_key_value_t;

typedef enum {
#define LWESP_CAYENNE_DATA_TYPE_DEFINE(tname, tconst, tvalue, uname, uconst, uvalue)    LWESP_CAYENNE_DATA_TYPE_ ## tconst ## _UNIT_ ## uconst,
#include "lwesp/apps/lwesp_cayenne_macros.h"
    LWESP_CAYENNE_DATA_TYPE_END_UNIT_END,       /*!< Final end value */
} lwesp_cayenne_data_type_unit_t;

/**
 * \brief           List of different data types, used for
 *                  library purpose to store data values,
 *                  before these are uploaded to cayenne cloud
 */
typedef enum {
    LWESP_CAYENNE_DATA_FORMAT_UINT32,           /*!< Unsigned integer data type, 32-bit width */
    LWESP_CAYENNE_DATA_FORMAT_INT32,            /*!< Integer data type, 32-bit width */
    LWESP_CAYENNE_DATA_FORMAT_FLOAT,            /*!< Float data type */
    LWESP_CAYENNE_DATA_FORMAT_DOUBLE,           /*!< Double data type */
    LWESP_CAYENNE_DATA_FORMAT_STRING,           /*!< Double data type */
    LWESP_CAYENNE_DATA_FORMAT_END,              /*!< End value indicator */
} lwesp_cayenne_data_format_t;

/**
 * \brief           TX message structure to be sent to cloud
 */
typedef struct {
    lwesp_cayenne_topic_t topic;                /*!< Topic to publish to */
    uint16_t channel;                           /*!< Channel to publish to */
    lwesp_cayenne_data_type_unit_t data_type_unit;  /*!< Data unit and its respective type for Cayenne dashboard.
                                                        It provides necessary value type (light, temperature, sensor, switch, etc)
                                                        and its respective value type (celsius, percentage, ...).
                                                        This is used to automatically detect widget type on Cayenne dashboard.
                                                        Set it to \ref LWESP_CAYENNE_DATA_TYPE_END_UNIT_END to send plain text from `data` field */
    lwesp_cayenne_data_format_t data_format;    /*!< Data format used for union data storage.
                                                    It is used for library internal-purpose only and defines
                                                    used `data` union member to use as valid data unit */
    union {
        uint32_t u32;                           /*!< Unsigned 32-bit integer type, used with \ref LWESP_CAYENNE_DATA_FORMAT_UINT32 format */
        int32_t i32;                            /*!< Signed 32-bit integer type, used with \ref LWESP_CAYENNE_DATA_FORMAT_INT32 format */
        float flt;                              /*!< Single-precision floating point type, used with \ref LWESP_CAYENNE_DATA_FORMAT_FLOAT format */
        double dbl;                             /*!< Double-precision floating point type, used with \ref LWESP_CAYENNE_DATA_FORMAT_DOUBLE format */
        const char* str;                        /*!< String data type. It is only a pointer to string and does not provide any storage,
                                                    used with \ref LWESP_CAYENNE_DATA_FORMAT_STRING format */
    } data;                                     /*!< Data union structure */
} lwesp_cayenne_tx_msg_t;

/**
 * \brief           Cayenne RX message structure
 */
typedef struct {
    lwesp_cayenne_topic_t topic;                /*!< Topic used for the received message */
    uint16_t channel;                           /*!< Message channel, optional, based on topic type */
    const char* seq;                            /*!< Random sequence string when RX message is command from server.
                                                    Used to be able to send effective reply back to server */
    lwesp_cayenne_key_value_t values[2];        /*!< Key/Value pair of values */
    size_t values_count;                        /*!< Count of valid pairs in values member */
} lwesp_cayenne_msg_t;

/**
 * \brief           Cayenne event
 */
typedef struct {
    lwesp_cayenne_evt_type_t type;              /*!< Event type */
    union {
        struct {
            lwesp_cayenne_msg_t* msg;           /*!< Pointer to data message */
        } data;                                 /*!< Data event, used with \ref LWESP_CAYENNE_EVT_DATA event */
    } evt;                                      /*!< Event union */
} lwesp_cayenne_evt_t;

/**
 * \brief           Cayenne handle forward declaration
 */
struct lwesp_cayenne;

/**
 * \brief           Cayenne event callback function
 * \param[in]       c: Cayenne handle
 * \param[in]       evt: Event handle
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
typedef lwespr_t (*lwesp_cayenne_evt_fn)(struct lwesp_cayenne* c, lwesp_cayenne_evt_t* evt);

/**
 * \brief           Cayenne handle
 */
typedef struct lwesp_cayenne {
    struct lwesp_cayenne* next;                 /*!< Next object on a list */
    lwesp_mqtt_client_p mqtt_client;            /*!< MQTT client object */
    const lwesp_mqtt_client_info_t* info_c;     /*!< MQTT client info structure */

    lwesp_cayenne_msg_t msg;                    /*!< Received data message currently being processed */

    /* These fields could be modified prior call to \ref lwesp_cayenne_create function */
    size_t client_buff_tx_len;                  /*!< MQTT client raw buffer length for TX data.
                                                    Set to \ref LWESP_CAYENNE_BUFF_TX_LEN if not defined by user */
    size_t client_buff_rx_len;                  /*!< MQTT client raw buffer length for RX data.
                                                    Set to \ref LWESP_CAYENNE_BUFF_RX_LEN if not defined by user */

    lwesp_buff_t tx_buff;                       /*!< TX data buffer handle */
    size_t tx_buff_count;                       /*!< Size of queue for TX messages.
                                                    Set to \ref LWESP_CAYENNE_BUFF_TX_COUNT if not defined by user */

    lwesp_cayenne_evt_t evt;                    /*!< Event handle object */
    lwesp_cayenne_evt_fn evt_fn;                /*!< Event callback function */
} lwesp_cayenne_t;

lwespr_t    lwesp_cayenne_init(void);
lwespr_t    lwesp_cayenne_create(lwesp_cayenne_t* c, const lwesp_mqtt_client_info_t* client_info, lwesp_cayenne_evt_fn evt_fn);
lwespr_t    lwesp_cayenne_connect(lwesp_cayenne_t* c) ;

lwespr_t    lwesp_cayenne_publish_ex(lwesp_cayenne_t* c, const lwesp_cayenne_tx_msg_t* tx_msg);
lwespr_t    lwesp_cayenne_publish_response(lwesp_cayenne_t* c, lwesp_cayenne_msg_t* msg, lwesp_cayenne_resp_t resp, const char* message);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_APP_CAYENNE_H */
