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


#include "iot_import.h"
#include "lite-log.h"
#include "lite-utils.h"
#include "lite-system.h"
#include "iot_export.h"
#include "iot_export_mqtt.h"
#include "iot_export_cm.h"

#include "mqtt_instance.h"
#include "iotx_cm_common.h"
#include "iotx_cm_conn_mqtt.h"


typedef struct iotx_connection_topic_st {
    void*                            next;
    char*                            topic;
    int                              packet_id;
    void*                            sem;
} iotx_connection_topic_t;


typedef struct iotx_cloud_conn_mqtt_st {
    char*                            msg_buf;
    char*                            msg_readbuf;
    void*                            mqtt_handler;
    int                              list_length;
    iotx_connection_topic_t*         topic_list;
} iotx_cloud_conn_mqtt_t;


static int _add_topic(iotx_cloud_conn_mqtt_t* mqtt_ctx, const char* topic, int packet_id, void* sem)
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
    new_pt->sem = sem;
    new_pt->next = mqtt_ctx->topic_list;

    mqtt_ctx->topic_list = new_pt;
    mqtt_ctx->list_length++;

    return SUCCESS_RETURN;
}

static int _delete_topic(iotx_cloud_conn_mqtt_t* mqtt_ctx, unsigned int packet_id)
{
    iotx_connection_topic_t* current = NULL;
    iotx_connection_topic_t* pre = NULL;

    if (NULL == mqtt_ctx->topic_list) {
        CM_ERR(cm_log_warning_no_list);
        return FAIL_RETURN;
    }

    current = pre = mqtt_ctx->topic_list;

    if (current->packet_id == packet_id){
        mqtt_ctx->topic_list = mqtt_ctx->topic_list->next;
        if (current->sem) {
            HAL_SemaphorePost(current->sem);
        }
        LITE_free(current->topic);
        LITE_free(current);
        mqtt_ctx->list_length--;
        if (mqtt_ctx->list_length == 0)
            mqtt_ctx->topic_list = NULL;
        return SUCCESS_RETURN;
    }

    current = current->next;

    while (current) {
        if (current->packet_id == packet_id) {
            pre->next = current->next;
            LITE_free(current->topic);
            LITE_free(current);

            mqtt_ctx->list_length--;

            if (mqtt_ctx->list_length == 0)
                mqtt_ctx = NULL;
            return SUCCESS_RETURN;
        }
        pre = current;
        current = current->next;
    }

    return FAIL_RETURN;
}

static char* _find_topic(iotx_cloud_conn_mqtt_t* mqtt_ctx, unsigned int packet_id)
{
    iotx_connection_topic_t* current = NULL;
    iotx_connection_topic_t* pre = NULL;

    if (NULL == mqtt_ctx->topic_list) {
        CM_ERR(cm_log_warning_no_list);
        return NULL;
    }

    current = pre = mqtt_ctx->topic_list;

    while (current) {
        if (current->packet_id == packet_id) {
            return current->topic;
        }
        pre = current;
        current = current->next;
    }

    return NULL;
}

static void _delete_all(iotx_cloud_conn_mqtt_t* mqtt_ctx)
{
    iotx_connection_topic_t* current = NULL;
    iotx_connection_topic_t* next = NULL;

    if (NULL == mqtt_ctx->topic_list) {
        CM_ERR(cm_log_warning_no_list);
        return;
    }

    current = next = mqtt_ctx->topic_list;

    while (current) {
        next = current->next;
        if (current->sem) {
            HAL_SemaphorePost(current->sem);
        }
        LITE_free(current->topic);
        LITE_free(current);
        current = next;
    }

    mqtt_ctx->list_length = 0;
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
    iotx_cm_connection_t* connection = (iotx_cm_connection_t*)pcontext;

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
        iotx_cloud_conn_mqtt_t* mqtt_ctx = (iotx_cloud_conn_mqtt_t*)connection->context;

        CM_INFO(cm_log_info_suback, (unsigned int)packet_id);

        memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
        msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_SUBSCRIBE_SUCCESS;
        msg_rsp.URI = _find_topic(mqtt_ctx, packet_id);
        msg_rsp.URI_length = msg_rsp.URI ? strlen(msg_rsp.URI) : 0;

        if (connection->response_handler && msg_rsp.URI)
            connection->response_handler(connection->event_pcontext, &msg_rsp);

        _delete_topic(mqtt_ctx, packet_id);
    }
        break;

    case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
    case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:{
        iotx_connection_msg_rsp_t msg_rsp;
        iotx_cloud_conn_mqtt_t* mqtt_ctx = (iotx_cloud_conn_mqtt_t*)connection->context;

        memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
        msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_SUBSCRIBE_FAIL;
        msg_rsp.URI = _find_topic(mqtt_ctx, packet_id);
        msg_rsp.URI_length = msg_rsp.URI ? strlen(msg_rsp.URI) : 0;

        if (msg_rsp.URI && connection->response_handler)
            connection->response_handler(connection->event_pcontext, &msg_rsp);

        _delete_topic(mqtt_ctx, packet_id);
    }
        break;

    case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:{
        iotx_connection_msg_rsp_t msg_rsp;
        iotx_cloud_conn_mqtt_t* mqtt_ctx = (iotx_cloud_conn_mqtt_t*)connection->context;

        memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
        msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_UNSUBSCRIBE_SUCCESS;
        msg_rsp.URI = _find_topic(mqtt_ctx, packet_id);
        msg_rsp.URI_length = msg_rsp.URI ? strlen(msg_rsp.URI) : 0;

        if (msg_rsp.URI && connection->response_handler)
            connection->response_handler(connection->event_pcontext, &msg_rsp);

        _delete_topic(mqtt_ctx, packet_id);
    }
        break;

    case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
    case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:{
        iotx_connection_msg_rsp_t msg_rsp;
        iotx_cloud_conn_mqtt_t* mqtt_ctx = (iotx_cloud_conn_mqtt_t*)connection->context;

        memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
        msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_UNSUBSCRIBE_FAIL;
        msg_rsp.URI = _find_topic(mqtt_ctx, packet_id);
        msg_rsp.URI_length = msg_rsp.URI ? strlen(msg_rsp.URI) : 0;

        if (msg_rsp.URI && connection->response_handler)
            connection->response_handler(connection->event_pcontext, &msg_rsp);

        _delete_topic(mqtt_ctx, packet_id);
    }
        break;

    case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:{
        iotx_connection_msg_rsp_t msg_rsp;
        iotx_cloud_conn_mqtt_t* mqtt_ctx = (iotx_cloud_conn_mqtt_t*)connection->context;

        memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
        msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_SEND_SUCCESS;
        msg_rsp.URI = _find_topic(mqtt_ctx, packet_id);
        msg_rsp.URI_length = msg_rsp.URI ? strlen(msg_rsp.URI) : 0;

        if (msg_rsp.URI && connection->response_handler)
            connection->response_handler(connection->event_pcontext, &msg_rsp);

        _delete_topic(mqtt_ctx, packet_id);
    }
        break;

    case IOTX_MQTT_EVENT_PUBLISH_NACK:
    case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:{
        iotx_connection_msg_rsp_t msg_rsp;
        iotx_cloud_conn_mqtt_t* mqtt_ctx = (iotx_cloud_conn_mqtt_t*)connection->context;

        memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
        msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_SEND_FAIL;
        msg_rsp.URI = _find_topic(mqtt_ctx, packet_id);
        msg_rsp.URI_length = msg_rsp.URI ? strlen(msg_rsp.URI) : 0;

        if (msg_rsp.URI && connection->response_handler)
            connection->response_handler(connection->event_pcontext, &msg_rsp);

        _delete_topic(mqtt_ctx, packet_id);
    }
        break;

    case IOTX_MQTT_EVENT_PUBLISH_RECVEIVED:{
        iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;
        iotx_connection_msg_rsp_t msg_rsp;

        CM_INFO(cm_log_info_mqtt_received, topic_info->topic_len, topic_info->ptopic, topic_info->payload_len, topic_info->payload);

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
void* iotx_cm_conn_mqtt_init(void* handle, void* init_param)
{
    iotx_mqtt_param_t mqtt_param;
    iotx_conn_info_pt pconn_info;
    void *pclient;
    iotx_cloud_conn_mqtt_t* mqtt_ctx = NULL;

    mqtt_ctx = CM_malloc(sizeof(iotx_cloud_conn_mqtt_t));
    if (NULL == mqtt_ctx){
        CM_ERR(cm_log_error_memory);
        return NULL;
    }
    memset(mqtt_ctx, 0x0, sizeof(iotx_cloud_conn_mqtt_t));

    if (NULL == (mqtt_ctx->msg_buf = (char *)CM_malloc(CM_MQTT_SEND_LEN))) {
        CM_ERR(cm_log_error_memory);
        LITE_free(mqtt_ctx);
        return NULL;
    }
    memset(mqtt_ctx->msg_buf, 0x0, CM_MQTT_SEND_LEN);

    if (NULL == (mqtt_ctx->msg_readbuf = (char *)CM_malloc(CM_MQTT_RECV_LEN))) {
        CM_ERR(cm_log_error_memory);
        LITE_free(mqtt_ctx->msg_buf);
        LITE_free(mqtt_ctx);
        return NULL;
    }
    memset(mqtt_ctx->msg_readbuf, 0x0, CM_MQTT_RECV_LEN);

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
    mqtt_param.pread_buf = mqtt_ctx->msg_readbuf;
    mqtt_param.read_buf_size = CM_MQTT_RECV_LEN;
    mqtt_param.pwrite_buf = mqtt_ctx->msg_buf;
    mqtt_param.write_buf_size = CM_MQTT_SEND_LEN;

    mqtt_param.handle_event.h_fp = iotx_cloud_conn_mqtt_event_handle;
    mqtt_param.handle_event.pcontext = (void*)handle;

    /* Construct a MQTT client with specify parameter */
    if (NULL == (pclient = IOT_MQTT_Construct(&mqtt_param))) {
        CM_ERR(cm_log_error_fail);
        LITE_free(mqtt_ctx->msg_buf);
        LITE_free(mqtt_ctx->msg_readbuf);
        LITE_free(mqtt_ctx);
        return NULL;
    }

    mqtt_set_instance(pclient);

    mqtt_ctx->mqtt_handler = pclient;

    return mqtt_ctx;
}

int iotx_cm_conn_mqtt_subscribe(void* handle, void *_register_param, int count)
{
    int ret = 0, i = 0;
    iotx_cm_connection_t* connection = (iotx_cm_connection_t*)handle;
    iotx_cloud_conn_mqtt_t* mqtt_ctx = (iotx_cloud_conn_mqtt_t*)connection->context;
#ifndef CM_SUPPORT_MULTI_THREAD
    iotx_cm_register_param_t* register_param = (iotx_cm_register_param_t*)_register_param;
    void* topicFilter = register_param[i].URI;
#else
    iotx_cm_register_param_t** register_param = (iotx_cm_register_param_t**)_register_param;
    void* topicFilter = register_param[i]->URI;
#endif 
    iotx_mutli_sub_info_t* mutli_sub[count];

    for (; i < count; i++) {
        mutli_sub[i] = CM_malloc(sizeof(iotx_mutli_sub_info_t));
        mutli_sub[i]->messageHandler = iotx_cloud_conn_mqtt_event_handle;
        mutli_sub[i]->qos = IOTX_CM_MESSAGE_NO_ACK;
        mutli_sub[i]->topicFilter = topicFilter;
    }
    ret = IOT_MQTT_Subscribe_Multi(mqtt_ctx->mqtt_handler,
                                   mutli_sub,
                                   count,
                                   (void*)connection);
    i--;
    for (; i >= 0; i--) {
        LITE_free(mutli_sub[i]);
    }

    if (ret > 0) _add_topic(mqtt_ctx, topicFilter, ret, NULL);

    return ret;
}

int iotx_cm_conn_mqtt_unsubscribe(void* handle,
                                  const char *topic_filter)
{
    int ret = 0;
    iotx_cm_connection_t* connection = (iotx_cm_connection_t*)handle;
    iotx_cloud_conn_mqtt_t* mqtt_ctx = (iotx_cloud_conn_mqtt_t*)connection->context;

    ret = IOT_MQTT_Unsubscribe(mqtt_ctx->mqtt_handler, topic_filter);

    if (ret > 0) _add_topic(mqtt_ctx, topic_filter, ret, NULL);

    return ret;
}

int iotx_cm_conn_mqtt_publish(void* handle, void* _context, iotx_connection_msg_t* message)
{
    int ret = 0;
    iotx_mqtt_topic_info_t topic_msg;
    iotx_cm_connection_t* connection = (iotx_cm_connection_t*)handle;
    iotx_cloud_conn_mqtt_t* mqtt_ctx = (iotx_cloud_conn_mqtt_t*)connection->context;

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

    CM_INFO(cm_log_info_mqtt_send, topic_msg.topic_len, topic_msg.ptopic, topic_msg.payload_len, topic_msg.payload);

    ret = IOT_MQTT_Publish(mqtt_ctx->mqtt_handler, message->URI, &topic_msg);

    if (IOTX_CM_MESSAGE_NO_ACK != topic_msg.qos && ret > 0) {
        _add_topic(mqtt_ctx, topic_msg.ptopic, ret, message->sem);
    } else if (ret > 0) {
        if (message->sem) HAL_SemaphorePost(message->sem);
    } else if (ret < 0) {
        iotx_connection_msg_rsp_t msg_rsp;

        memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
        msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_SEND_FAIL;
        msg_rsp.URI = message->URI;
        msg_rsp.URI_length = message->URI_length;

        if (connection->response_handler)
            connection->response_handler(connection->event_pcontext, &msg_rsp);
    }

    return ret;
}


int iotx_cm_conn_mqtt_deinit(void* handle)
{
    iotx_cm_connection_t* connection = (iotx_cm_connection_t*)handle;
    iotx_cloud_conn_mqtt_t* mqtt_ctx = (iotx_cloud_conn_mqtt_t*)connection->context;
    int ret = 0;

    mqtt_remove_instance();

    if (mqtt_ctx && mqtt_ctx->list_length != 0) _delete_all(mqtt_ctx);

    ret = IOT_MQTT_Destroy(&(mqtt_ctx->mqtt_handler));

    if (mqtt_ctx && mqtt_ctx->msg_buf) LITE_free(mqtt_ctx->msg_buf);

    if (mqtt_ctx && mqtt_ctx->msg_readbuf) LITE_free(mqtt_ctx->msg_readbuf);

    if (mqtt_ctx) LITE_free(mqtt_ctx);

    return ret;
}

int iotx_cm_conn_mqtt_yield(void* handle, int timeout_ms)
{
    iotx_cm_connection_t* connection = (iotx_cm_connection_t*)handle;
    iotx_cloud_conn_mqtt_t* mqtt_ctx = (iotx_cloud_conn_mqtt_t*)connection->context;

    return IOT_MQTT_Yield(mqtt_ctx->mqtt_handler, timeout_ms);
}

