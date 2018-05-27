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


#ifndef __CM_CONNECTION_H__
#define __CM_CONNECTION_H__

#include "iot_export_cm.h"
#include "iotx_cm_common.h"


/* response type */
typedef enum IOTX_CONNECTION_RESPONSE_TYPES {
    /* subcribe success */
    IOTX_CONNECTION_RESPONSE_SUBSCRIBE_SUCCESS = 1,

    /* subcribe fail */
    IOTX_CONNECTION_RESPONSE_SUBSCRIBE_FAIL,

    /* subcribe success */
    IOTX_CONNECTION_RESPONSE_UNSUBSCRIBE_SUCCESS,

    /* subcribe fail */
    IOTX_CONNECTION_RESPONSE_UNSUBSCRIBE_FAIL,

    /* add service success */
    IOTX_CONNECTION_RESPONSE_ADD_SERVICE_SUCCESS,

    /* add service fail */
    IOTX_CONNECTION_RESPONSE_ADD_SERVICE_FAIL,

    /* remoce service success */
    IOTX_CONNECTION_RESPONSE_REMOVE_SERVICE_SUCCESS,

    /* remoce service fail */
    IOTX_CONNECTION_RESPONSE_REMOVE_SERVICE_FAIL,

    /* send success */
    IOTX_CONNECTION_RESPONSE_SEND_SUCCESS,

    /* send fail */
    IOTX_CONNECTION_RESPONSE_SEND_FAIL,

    /* receive new data */
    IOTX_CONNECTION_RESPONSE_NEW_DATA,

    /* Maximum number of response type */
    IOTX_CONNECTION_RESPONSE_TYPE_MAX
} iotx_connection_response_types_t;


/* message type */
typedef enum IOTX_CONNECTION_MESSAGE_TYPES {
    /* MQTT: subcribe */
    IOTX_CONNECTION_MESSAGE_TYPE_SUBSCRIBE = 1,

    /* MQTT: unsubcribe */
    IOTX_CONNECTION_MESSAGE_TYPE_UNSUBSCRIBE = 2,

    /* COAP: register */
    IOTX_CONNECTION_MESSAGE_TYPE_ADD_SERVICE = 3,

    /* COAP: unregister */
    IOTX_CONNECTION_MESSAGE_TYPE_DELETE_SERVICE = 4,

    /* MQTT: publish; CoAP & HTTP: send message */
    IOTX_CONNECTION_MESSAGE_TYPE_SEND = 5,

    /* Maximum number of message type */
    IOTX_CONNECTION_MESSAGE_TYPE_MAX
}iotx_connection_message_types_t;


/* message type */
typedef enum IOTX_CONNECTION_PROTOCOL_TYPES {
    /* MQTT */
    IOTX_CONNECTION_PROTOCOL_TYPE_MQTT = 1,

    /* COAP */
    IOTX_CONNECTION_PROTOCOL_TYPE_COAP = 2,

    /* HTTP */
    IOTX_CONNECTION_PROTOCOL_TYPE_HTTP = 3,

    /* ALCS */
    IOTX_CONNECTION_PROTOCOL_TYPE_ALCS = 4,

    /* Maximum number of protocol type */
    IOTX_CONNECTION_PROTOCOL_TYPE_MAX
}iotx_connection_protocol_types_t;

typedef enum IOTX_CONNECTION_EVENT_TYPES {
    IOTX_CONNECTION_EVENT_DISCONNECT,

    IOTX_CONNECTION_EVENT_RECONNECT,

    IOTX_CONNECTION_EVENT_MAX
}iotx_connection_event_types_t;


/* The structure of cloud Connection event struct */
typedef struct {
    uint8_t                                   event_id;
    void*                                     msg;
} iotx_connection_event_msg_t;


/**
 * @brief It define a datatype of function pointer.
 *        This type of function will be called when a related event occur.
 *
 * @param pcontext : The program context.
 * @param pConnection : The cloud Connection.
 * @param msg : The event message.
 *
 * @return none
 */
typedef void (*iotx_connection_event_handle_fp_t)(void* pconnection, iotx_connection_event_msg_t* msg);

typedef struct {
    iotx_connection_response_types_t            rsp_type;
    char*                                       URI;
    uint32_t                                    URI_length;
    void*                                       payload;
    uint32_t                                    payload_length;
    iotx_cm_message_ack_types_t                 ack_type;
    void*                                       conn_ctx;
} iotx_connection_msg_rsp_t;


typedef int (*iotx_connection_msg_rsp_fp_t)(void* pcontext, iotx_connection_msg_rsp_t* msg);


typedef struct {
    iotx_connection_message_types_t             type;
    char*                                       URI;
    uint32_t                                    URI_length;
    void*                                       payload;
    uint32_t                                    payload_length;
    iotx_cm_message_ack_types_t                 ack_type;
    void*                                       response_pcontext;
    iotx_connection_msg_rsp_fp_t                response_handler;
    iotx_cm_content_type_t                      content_type;
} iotx_connection_msg_t;


typedef void* (*iotx_connection_init_fp_t)(void* connection);

typedef int (*iotx_connection_subscribe_fp_t)(void* connection, const char *topic_filter, iotx_cm_message_ack_types_t ack_type);

typedef int (*iotx_connection_unsubscribe_fp_t)(void* connection, const char *topic_filter);

typedef int (*iotx_connection_add_service_fp_t)(void* connection, const char *topic_filter, iotx_cm_message_ack_types_t ack_type, iotx_cm_message_auth_types_t auth_type);

typedef int (*iotx_connection_remove_service_fp_t)(void* connection, const char *topic_filter);

typedef int (*iotx_connection_send_fp_t)(void* connection, void* _context, iotx_connection_msg_t* message);

typedef int (*iotx_connection_add_subdevice_fp_t)(void* connection, const char* pk, const char* dn);

typedef int (*iotx_connection_remove_subdevice_fp_t)(void* connection, const char* pk, const char* dn);

typedef int (*iotx_connection_deinit_fp_t)(void* connection);

typedef int (*iotx_connection_yield_fp_t)(void* connection, int timeout_ms);

typedef int (*iotx_connection_resposne_fp_t)(void* connection, iotx_connection_msg_rsp_t* msg);


typedef struct iotx_connection_param_st {
    uint8_t                                     clean_session;               /* MQTT: clean_session,  HTTP: keep_alive*/
    uint32_t                                    request_timeout_ms;
    uint32_t                                    keepalive_interval_ms;
} iotx_connection_param_t;

/* The structure of cloud connection context */
typedef struct iotx_connection_st {
    void*                                       context;
    iotx_connection_init_fp_t                   init_func;
    iotx_connection_subscribe_fp_t              sub_func;
    iotx_connection_unsubscribe_fp_t            unsub_func;
    iotx_connection_add_service_fp_t            add_service_func;
    iotx_connection_remove_service_fp_t         remove_service_func;
    iotx_connection_send_fp_t                   send_func;
    iotx_connection_add_subdevice_fp_t          add_subdevice_func;
    iotx_connection_remove_subdevice_fp_t       remove_subdevice_func;
    iotx_connection_yield_fp_t                  yield_func;
    iotx_connection_deinit_fp_t                 deinit_func;
    iotx_connection_protocol_types_t            protocol_type;
    void*                                       event_pcontext;
    iotx_connection_event_handle_fp_t           event_handler;
    iotx_connection_resposne_fp_t               response_handler;
} iotx_connection_t;

#endif /* __CM_CONNECTION_H__ */
