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


#ifdef CM_VIA_CLOUD_CONN_MQTT

#include "iot_import.h"
#include "lite-log.h"
#include "lite-utils.h"
#include "lite-system.h"
#include "iot_export.h"
#include "iot_export_mqtt.h"
#include "iot_export_cm.h"

#include "mqtt_instance.h"
#include "iotx_cm_common.h"
#include "iotx_cloud_conn_mqtt.h"


#define MQTT_MSGLEN             (1200)

typedef struct iotx_connection_topic_st {
    void*                            next;
    char*                            topic;
    int                              packet_id;
} iotx_connection_topic_t;


typedef struct iotx_cloud_conn_mqtt_st {
    char*                            msg_buf;
    char*                            msg_readbuf;
    int                              list_length;
    iotx_connection_topic_t*         topic_list;
} iotx_cloud_conn_mqtt_t;

static iotx_cloud_conn_mqtt_t* _mqtt_pt = NULL;

static int _add_topic(const char* topic, int packet_id)
{
    iotx_connection_topic_t* new_pt = NULL;

    new_pt = CM_malloc(sizeof(iotx_connection_topic_t));
    if (NULL == new_pt) {
        CM_ERR(cm_log_error_memory);
        return FAIL_RETURN;
    }
    memset(new_pt, 0x0, sizeof(iotx_connection_topic_t));

    new_pt->topic = CM_malloc(strlen(topic) + 1);
    if (NULL == new_pt->topic) {
        CM_ERR(cm_log_error_memory);
        LITE_free(new_pt);
        return FAIL_RETURN;
    }
    memset(new_pt->topic, 0x0, strlen(topic) + 1);
    strncpy(new_pt->topic, topic, strlen(topic));

    new_pt->packet_id = packet_id;
    new_pt->next = _mqtt_pt->topic_list;

    _mqtt_pt->topic_list = new_pt;
    _mqtt_pt->list_length++;

    return SUCCESS_RETURN;
}

static int _delete_topic(unsigned int packet_id)
{
    iotx_connection_topic_t* current = NULL;
    iotx_connection_topic_t* pre = NULL;

    if (NULL == _mqtt_pt->topic_list) {
        CM_ERR(cm_log_warning_no_list);
        return FAIL_RETURN;
    }

    current = pre = _mqtt_pt->topic_list;

    if (current->packet_id == packet_id){
        _mqtt_pt->topic_list = _mqtt_pt->topic_list->next;
        LITE_free(current->topic);
        LITE_free(current);
        _mqtt_pt->list_length--;
        if (_mqtt_pt->list_length == 0)
            _mqtt_pt->topic_list = NULL;
        return SUCCESS_RETURN;
    }

    current = current->next;

    while (current) {
        if (current->packet_id == packet_id) {
            pre->next = current->next;
            LITE_free(current->topic);
            LITE_free(current);

            _mqtt_pt->list_length--;

            if (_mqtt_pt->list_length == 0)
                _mqtt_pt = NULL;
            return SUCCESS_RETURN;
        }
        pre = current;
        current = current->next;
    }

    return FAIL_RETURN;
}

static char* _find_topic(unsigned int packet_id)
{
    iotx_connection_topic_t* current = NULL;
    iotx_connection_topic_t* pre = NULL;

    if (NULL == _mqtt_pt->topic_list) {
        CM_ERR(cm_log_warning_no_list);
        return NULL;
    }

    current = pre = _mqtt_pt->topic_list;

    while (current) {
        if (current->packet_id == packet_id) {
            return current->topic;
        }
        pre = current;
        current = current->next;
    }

    return NULL;
}

static void _delete_all()
{
    iotx_connection_topic_t* current = NULL;
    iotx_connection_topic_t* next = NULL;

    if (NULL == _mqtt_pt->topic_list) {
        CM_ERR(cm_log_warning_no_list);
        return;
    }

    current = next = _mqtt_pt->topic_list;

    while (current) {
        next = current->next;
        LITE_free(current->topic);
        LITE_free(current);
        current = next;
    }

    _mqtt_pt->list_length = 0;
}

static iotx_mqtt_qos_t _QoS (iotx_cm_message_ack_types_t ack_type)
{
    switch(ack_type) {
        case IOTX_CM_MESSAGE_NO_ACK:
            return IOTX_MQTT_QOS0;

        case IOTX_CM_MESSAGE_NEED_ACK:
            return IOTX_MQTT_QOS1;

        default:
            return IOTX_MQTT_QOS0;
    }
}


static void iotx_cloud_conn_mqtt_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    uintptr_t packet_id = (uintptr_t)msg->msg;
    iotx_connection_t* connection = (iotx_connection_t*)pcontext;

    CM_INFO(cm_log_info_event_type, msg->event_type);

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_UNDEF:
            break;

        case IOTX_MQTT_EVENT_DISCONNECT: {
            iotx_connection_event_msg_t event;
            event.event_id = IOTX_CONNECTION_EVENT_DISCONNECT;
            event.msg = NULL;

            CM_INFO(cm_log_info_MQTT_disconnect);

            if (connection->event_handler)
                connection->event_handler(connection->event_pcontext, &event);
        }
            break;

        case IOTX_MQTT_EVENT_RECONNECT: {
            iotx_connection_event_msg_t event;

            event.event_id = IOTX_CONNECTION_EVENT_RECONNECT;
            event.msg = NULL;

            CM_INFO(cm_log_info_MQTT_reconnect);

            if (connection->event_handler)
                connection->event_handler(connection->event_pcontext, &event);
        }
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS: {
            iotx_connection_msg_rsp_t msg_rsp;

            log_info("subscribe success, packet-id=%u", (unsigned int)packet_id);

            memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
            msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_SUBSCRIBE_SUCCESS;
            msg_rsp.URI = _find_topic(packet_id);
            msg_rsp.URI_length = msg_rsp.URI ? strlen(msg_rsp.URI) : 0;

            if (connection->response_handler && msg_rsp.URI)
                connection->response_handler(connection->event_pcontext, &msg_rsp);

            _delete_topic(packet_id);
        }
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:{
            iotx_connection_msg_rsp_t msg_rsp;

            memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
            msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_SUBSCRIBE_FAIL;
            msg_rsp.URI = _find_topic(packet_id);
            msg_rsp.URI_length = msg_rsp.URI ? strlen(msg_rsp.URI) : 0;

            if (msg_rsp.URI && connection->response_handler)
                connection->response_handler(connection->event_pcontext, &msg_rsp);

            _delete_topic(packet_id);
        }
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:{
            iotx_connection_msg_rsp_t msg_rsp;

            memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
            msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_UNSUBSCRIBE_SUCCESS;
            msg_rsp.URI = _find_topic(packet_id);
            msg_rsp.URI_length = msg_rsp.URI ? strlen(msg_rsp.URI) : 0;

            if (msg_rsp.URI && connection->response_handler)
                connection->response_handler(connection->event_pcontext, &msg_rsp);

            _delete_topic(packet_id);
        }
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:{
            iotx_connection_msg_rsp_t msg_rsp;

            memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
            msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_UNSUBSCRIBE_FAIL;
            msg_rsp.URI = _find_topic(packet_id);
            msg_rsp.URI_length = msg_rsp.URI ? strlen(msg_rsp.URI) : 0;

            if (msg_rsp.URI && connection->response_handler)
                connection->response_handler(connection->event_pcontext, &msg_rsp);

            _delete_topic(packet_id);
        }
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:{
            iotx_connection_msg_rsp_t msg_rsp;

            memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
            msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_SEND_SUCCESS;
            msg_rsp.URI = _find_topic(packet_id);
            msg_rsp.URI_length = msg_rsp.URI ? strlen(msg_rsp.URI) : 0;

            if (msg_rsp.URI && connection->response_handler)
                connection->response_handler(connection->event_pcontext, &msg_rsp);

            _delete_topic(packet_id);
        }
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:{
            iotx_connection_msg_rsp_t msg_rsp;

            memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
            msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_SEND_FAIL;
            msg_rsp.URI = _find_topic(packet_id);
            msg_rsp.URI_length = msg_rsp.URI ? strlen(msg_rsp.URI) : 0;

            if (msg_rsp.URI && connection->response_handler)
                connection->response_handler(connection->event_pcontext, &msg_rsp);

            _delete_topic(packet_id);
        }
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECVEIVED:{
            iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;
            iotx_connection_msg_rsp_t msg_rsp;

//            log_info("topic message arrived: topic=%.*s, topic_msg=%.*s",
//                          topic_info->topic_len,
//                          topic_info->ptopic,
//                          topic_info->payload_len,
//                          topic_info->payload);

            memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
            msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_NEW_DATA;
            msg_rsp.URI = (char*)topic_info->ptopic;
            msg_rsp.URI_length = topic_info->topic_len;
            msg_rsp.payload = (void*)topic_info->payload;
            msg_rsp.payload_length = topic_info->payload_len;

            if (connection->response_handler)
                connection->response_handler(connection->event_pcontext, &msg_rsp);
        }
            break;

        case IOTX_MQTT_EVENT_BUFFER_OVERFLOW:
            CM_WARNING(cm_log_warning_buffer_overflow, msg->msg);
            break;

        default:
            CM_WARNING(cm_log_warning_not_arrived);
            break;
    }
}


/*  mqtt */
void* iotx_cloud_conn_mqtt_init(void* handle)
{
    iotx_mqtt_param_t mqtt_param;
    iotx_conn_info_pt pconn_info;
    void *pclient;

    if (NULL == _mqtt_pt) {
        _mqtt_pt = CM_malloc(sizeof(iotx_cloud_conn_mqtt_t));
        if (NULL == _mqtt_pt){
            log_info("not enough memory");
            return NULL;
        }
        memset(_mqtt_pt, 0x0, sizeof(iotx_cloud_conn_mqtt_t));

        if (NULL == (_mqtt_pt->msg_buf = (char *)CM_malloc(MQTT_MSGLEN))) {
            log_info("not enough memory");
            LITE_free(_mqtt_pt);
            return NULL;
        }
        memset(_mqtt_pt->msg_buf, 0x0, MQTT_MSGLEN);

        if (NULL == (_mqtt_pt->msg_readbuf = (char *)CM_malloc(MQTT_MSGLEN))) {
            log_info("not enough memory");
            LITE_free(_mqtt_pt->msg_buf);
            LITE_free(_mqtt_pt);
            return NULL;
        }
        memset(_mqtt_pt->msg_readbuf, 0x0, MQTT_MSGLEN);
    }

    pconn_info = iotx_conn_info_get();

    /* Initialize MQTT parameter */
    memset(&mqtt_param, 0x0, sizeof(mqtt_param));

    mqtt_param.port = pconn_info->port;
    mqtt_param.host = pconn_info->host_name;
    mqtt_param.client_id = pconn_info->client_id;
    mqtt_param.username = pconn_info->username;
    mqtt_param.password = pconn_info->password;
    mqtt_param.pub_key = pconn_info->pub_key;

    mqtt_param.request_timeout_ms = 2000;
    mqtt_param.clean_session = 0;
    mqtt_param.keepalive_interval_ms = 60000;
    mqtt_param.pread_buf = _mqtt_pt->msg_readbuf;
    mqtt_param.read_buf_size = MQTT_MSGLEN;
    mqtt_param.pwrite_buf = _mqtt_pt->msg_buf;
    mqtt_param.write_buf_size = MQTT_MSGLEN;

    mqtt_param.handle_event.h_fp = iotx_cloud_conn_mqtt_event_handle;
    mqtt_param.handle_event.pcontext = (void*)handle;

    /* Construct a MQTT client with specify parameter */
    if (NULL == (pclient = IOT_MQTT_Construct(&mqtt_param))) {
        log_info("MQTT construct failed");
        LITE_free(_mqtt_pt->msg_buf);
        LITE_free(_mqtt_pt->msg_readbuf);
        LITE_free(_mqtt_pt);
        return NULL;
    }

    mqtt_set_instance(pclient);

    return pclient;
}

int iotx_cloud_conn_mqtt_subscribe(void* handle,
                        const char *topic_filter,
                        iotx_cm_message_ack_types_t ack_type)
{
    int rc = 0;
    iotx_connection_t* connection = (iotx_connection_t*)handle;

    rc = IOT_MQTT_Subscribe(connection->context,
                        topic_filter,
                        _QoS(ack_type),
                        iotx_cloud_conn_mqtt_event_handle,
                        (void*)connection);

    if (rc > 0) _add_topic(topic_filter, rc);

    return rc;
}

int iotx_cloud_conn_mqtt_unsubscribe(void* handle,
                        const char *topic_filter)
{
    int rc = 0;
    iotx_connection_t* connection = (iotx_connection_t*)handle;

    rc = IOT_MQTT_Unsubscribe(connection->context, topic_filter);

    if (rc > 0) _add_topic(topic_filter, rc);

    return rc;
}

int iotx_cloud_conn_mqtt_publish(void* handle, void* _context, iotx_connection_msg_t* message)
{
    int rc = 0;
    iotx_mqtt_topic_info_t topic_msg;
    iotx_connection_t* connection = (iotx_connection_t*)handle;

    if (NULL == message) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));

    topic_msg.dup = 0;
    topic_msg.qos = _QoS(message->ack_type);
    topic_msg.retain = 0;
    topic_msg.payload_len = message->payload_length;
    topic_msg.payload = message->payload;
    topic_msg.ptopic = message->URI;
    topic_msg.topic_len = message->URI_length;

    rc = IOT_MQTT_Publish(connection->context, message->URI, &topic_msg);

    if (IOTX_CM_MESSAGE_NO_ACK != topic_msg.qos && rc > 0) {
        _add_topic(topic_msg.ptopic, rc);
    } else if (rc < 0) {
        iotx_connection_msg_rsp_t msg_rsp;

        memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
        msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_SEND_FAIL;
        msg_rsp.URI = message->URI;
        msg_rsp.URI_length = message->URI_length;

        if (connection->response_handler)
            connection->response_handler(connection, &msg_rsp);
    }

    return rc;
}

int iotx_cloud_conn_mqtt_deinit(void* handle)
{
    iotx_connection_t* connection = (iotx_connection_t*)handle;
    int rc = 0;

    mqtt_remove_instance();

    if (_mqtt_pt && _mqtt_pt->list_length != 0) _delete_all();

    rc = IOT_MQTT_Destroy(&(connection->context));

    if (_mqtt_pt && _mqtt_pt->msg_buf) LITE_free(_mqtt_pt->msg_buf);

    if (_mqtt_pt && _mqtt_pt->msg_readbuf) LITE_free(_mqtt_pt->msg_readbuf);

    if (_mqtt_pt) LITE_free(_mqtt_pt);

    return rc;
}

int iotx_cloud_conn_mqtt_yield(void* handle, int timeout_ms)
{
    iotx_connection_t* connection = (iotx_connection_t*)handle;

    return IOT_MQTT_Yield(connection->context, timeout_ms);
}

#endif /* CM_VIA_CLOUD_CONN_MQTT */
