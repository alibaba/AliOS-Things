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

#ifdef CM_VIA_CLOUD_CONN

#include "iot_import.h"

#include "lite-utils.h"
#include "lite-system.h"
#include "iot_export.h"
#include "iotx_cm_common.h"
#include "iotx_cm_connection.h"
#include "iotx_cm_cloud_conn.h"
#include "iotx_cloud_conn_mqtt.h"

static iotx_cm_send_peer_t g_cloud_target;

static void iotx_cm_cloud_conn_event_callback(void *_cm_ctx, iotx_connection_event_msg_t* msg)
{
    iotx_cm_conntext_t* cm_ctx = (iotx_cm_conntext_t*)_cm_ctx;

    if (NULL == cm_ctx || NULL == msg) {
        CM_ERR(cm_log_error_parameter);
        return;
    }

    CM_INFO(cm_log_info_event_id, msg->event_id);

    switch (msg->event_id) {
    case IOTX_CONNECTION_EVENT_DISCONNECT: {
#ifdef CM_SUPPORT_MULTI_THREAD
        {
            /* send message to itself thread */
            int ret = 0;
            iotx_cm_process_list_node_t* node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD);
            if (NULL == node) {
                CM_ERR(cm_log_error_get_node);
                return;
            }

            node->type = IOTX_CM_PROCESS_CLOUD_DISCONNECT;
            node->msg = NULL;

            ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
            if (FAIL_RETURN == ret) {
                CM_ERR(cm_log_error_push_node);
                iotx_cm_free_list_node(cm_ctx, node);
            }
        }
#else
        iotx_cm_cloud_conn_disconnect_handler(cm_ctx);
#endif /* CM_SUPPORT_MULTI_THREAD */
    }
        break;

    case IOTX_CONNECTION_EVENT_RECONNECT: {
#ifdef CM_SUPPORT_MULTI_THREAD
        {
            /* send message to itself thread */
            int ret = 0;
            iotx_cm_process_list_node_t* node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD);
            if (NULL == node) {
                CM_ERR(cm_log_error_get_node);
                return;
            }
            node->type = IOTX_CM_PROCESS_CLOUD_RECONNECT;
            node->msg = NULL;

            ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
            if (FAIL_RETURN == ret) {
                CM_ERR(cm_log_error_push_node);
                iotx_cm_free_list_node(cm_ctx, node);
            }
        }
#else
        iotx_cm_cloud_conn_reconnect_handler(cm_ctx);
#endif /* CM_SUPPORT_MULTI_THREAD */
    }
        break;

    default:
        CM_WARNING(cm_log_warning_not_arrived);
        break;
    }
}


static int iotx_cm_cloud_conn_response_callback(void *_cm_ctx, iotx_connection_msg_rsp_t* msg)
{
    iotx_cm_conntext_t* cm_ctx = (iotx_cm_conntext_t*)_cm_ctx;

    if (NULL == cm_ctx || NULL == msg) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    CM_INFO(cm_log_info_rsp_type, msg->rsp_type);

    switch (msg->rsp_type) {
    case IOTX_CONNECTION_RESPONSE_SUBSCRIBE_SUCCESS: {
#ifdef CM_SUPPORT_MULTI_THREAD
        {
            /* send message to itself thread */
            int ret = 0;
            iotx_cm_process_list_node_t* node = NULL;
            iotx_cm_process_register_result_pt result_msg = NULL;

            node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD);
            if (NULL == node) return FAIL_RETURN;

            node->type = IOTX_CM_PROCESS_CLOUD_REGISTER_RESULT;
            if (NULL == (node->msg = CM_malloc(sizeof(iotx_cm_process_register_result_t)))) {
                CM_ERR(cm_log_error_memory);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }

            result_msg = node->msg;
            if (NULL == (result_msg->URI = CM_malloc(strlen(msg->URI) + 1))) {
                LITE_free(node->msg);
                LITE_free(node);
                return FAIL_RETURN;
            }
            memset(result_msg->URI, 0x0, strlen(msg->URI) + 1);
            strncpy(result_msg->URI, (msg->URI), strlen(msg->URI) + 1);
            result_msg->result = 0;
            result_msg->is_register = 1;

            if (FAIL_RETURN == (ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node))) {
                LITE_free(result_msg->URI);
                LITE_free(node->msg);
                LITE_free(node);
            }
        }
#else
        if (msg->URI) iotx_cm_cloud_conn_register_handler(cm_ctx, msg->URI, 0, 1);
#endif /* CM_SUPPORT_MULTI_THREAD */
    }
        break;

    case IOTX_CONNECTION_RESPONSE_SUBSCRIBE_FAIL: {
#ifdef CM_SUPPORT_MULTI_THREAD
        {
            /* send message to itself thread */
            int ret = 0;
            iotx_cm_process_list_node_t* node = NULL;
            iotx_cm_process_register_result_pt result_msg = NULL;

            node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD);
            if (NULL == node) return FAIL_RETURN;

            node->type = IOTX_CM_PROCESS_CLOUD_REGISTER_RESULT;
            if (NULL == (node->msg = CM_malloc(sizeof(iotx_cm_process_register_result_t)))) {
                CM_ERR(cm_log_error_memory);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }

            result_msg = node->msg;

            if (NULL == (result_msg->URI = CM_malloc(strlen(msg->URI) + 1))) {
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }
            memset(result_msg->URI, 0x0, strlen(msg->URI) + 1);
            strncpy(result_msg->URI, (msg->URI), strlen(msg->URI) + 1);

            result_msg->result = -1;
            result_msg->is_register = 1;

            if (FAIL_RETURN == (ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node))) {
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
            }
        }
#else
        if (msg->URI) iotx_cm_cloud_conn_register_handler(cm_ctx, msg->URI, -1, 1);
#endif /* CM_SUPPORT_MULTI_THREAD */
    }
        break;

    case IOTX_CONNECTION_RESPONSE_UNSUBSCRIBE_SUCCESS: {
#ifdef CM_SUPPORT_MULTI_THREAD
        {
            /* send message to itself thread */
            int ret = 0;
            iotx_cm_process_list_node_t* node = NULL;
            iotx_cm_process_register_result_pt result_msg = NULL;

            if (NULL == (node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD))) return FAIL_RETURN;

            node->type = IOTX_CM_PROCESS_CLOUD_UNREGISTER_RESULT;
            if (NULL == (node->msg = CM_malloc(sizeof(iotx_cm_process_register_result_t)))) {
                CM_ERR(cm_log_error_memory);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }

            result_msg = node->msg;

            if (NULL == (result_msg->URI = CM_malloc(strlen(msg->URI) + 1))) {
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }
            memset(result_msg->URI, 0x0, strlen(msg->URI) + 1);
            strncpy(result_msg->URI, (msg->URI), strlen(msg->URI) + 1);
            result_msg->result = 1;
            result_msg->is_register = 0;

            if (FAIL_RETURN == (ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node))) {
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
            }
        }
#else
        if (msg->URI) iotx_cm_cloud_conn_register_handler(cm_ctx, msg->URI, 0, 0);
#endif /* CM_SUPPORT_MULTI_THREAD */
    }
        break;

    case IOTX_CONNECTION_RESPONSE_UNSUBSCRIBE_FAIL: {
#ifdef CM_SUPPORT_MULTI_THREAD
        {
            /* send message to itself thread */
            int ret = 0;
            iotx_cm_process_list_node_t* node = NULL;
            iotx_cm_process_register_result_pt result_msg = NULL;

            if (NULL == (node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD))) return FAIL_RETURN;

            node->type = IOTX_CM_PROCESS_CLOUD_UNREGISTER_RESULT;
            if (NULL == (node->msg = CM_malloc(sizeof(iotx_cm_process_register_result_t)))) {
                CM_ERR(cm_log_error_memory);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }

            result_msg = node->msg;

            if (NULL == (result_msg->URI = CM_malloc(strlen(msg->URI) + 1))) {
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }
            memset(result_msg->URI, 0x0, strlen(msg->URI) + 1);
            strncpy(result_msg->URI, (msg->URI), strlen(msg->URI) + 1);
            result_msg->result = 0;
            result_msg->is_register = 0;

            if (FAIL_RETURN == (ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node))) {
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
            }
        }
#else
        if (msg->URI) iotx_cm_cloud_conn_register_handler(cm_ctx, msg->URI, -1, 0);
#endif /* CM_SUPPORT_MULTI_THREAD */
    }
        break;

    case IOTX_CONNECTION_RESPONSE_SEND_SUCCESS:
    case IOTX_CONNECTION_RESPONSE_SEND_FAIL:
    case IOTX_CONNECTION_RESPONSE_NEW_DATA: {
#ifdef CM_SUPPORT_MULTI_THREAD
        {
            /* send message to itself thread */
            int ret = 0;
            iotx_cm_process_list_node_t* node = NULL;
            iotx_cm_message_info_t* msg_info = NULL;
#ifdef CM_SUPPORT_TOPIC_DISPATCH
            iotx_cm_mapping_t* mapping = NULL;
#endif /* CM_SUPPORT_TOPIC_DISPATCH */

            if (NULL == (node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD))) return FAIL_RETURN;

            node->type = IOTX_CM_PROCESS_CLOUD_NEW_DATA;
            if (NULL == (node->msg = CM_malloc(sizeof(iotx_cm_message_info_t)))) {
                CM_ERR(cm_log_error_memory);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }

            msg_info = node->msg;

            msg_info->URI = CM_malloc(msg->URI_length + 1);
            if (msg_info->URI) {
                strncpy(msg_info->URI, msg->URI, msg->URI_length);
            } else {
                CM_ERR(cm_log_error_parse_URI);
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }

#ifdef CM_SUPPORT_TOPIC_DISPATCH
            /* find mapping */
            if (NULL == (mapping = iotx_cm_find_mapping(cm_ctx, (char*)msg->URI, msg->URI_length))) {
                CM_WARNING(cm_log_warning_not_mapping);
                LITE_free(msg_info->URI);
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }
            msg_info->message_type = mapping->type;
#endif

            if (FAIL_RETURN == iotx_cm_parse_payload((char*)msg->payload, msg->payload_length, msg_info)) {
                CM_ERR(cm_log_error_parse_payload);
                iotx_cm_free_message_info(msg_info);
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }

            if (FAIL_RETURN == (ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node))) {
                iotx_cm_free_message_info(msg_info);
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
                break;
            }
        }
#else
        iotx_cm_message_info_t message_info;
#ifdef CM_SUPPORT_TOPIC_DISPATCH
        iotx_cm_mapping_t* mapping = NULL;
#endif

        memset(&message_info, 0x0, sizeof(iotx_cm_message_info_t));

        message_info.URI = CM_malloc(msg->URI_length + 1);
        if (message_info.URI) {
            strncpy(message_info.URI, msg->URI, msg->URI_length);
        } else {
            iotx_cm_free_message_info(&message_info);
            return FAIL_RETURN;
        }

#ifdef CM_SUPPORT_TOPIC_DISPATCH
        /* find mapping */
        if (NULL == (mapping = iotx_cm_find_mapping(cm_ctx, (char*)msg->URI, msg->URI_length))) {
            CM_WARNING(cm_log_warning_not_mapping);
            iotx_cm_free_message_info(&message_info);
            return FAIL_RETURN;
        }
        message_info.message_type = mapping->type;
#endif

        if (FAIL_RETURN == iotx_cm_parse_payload((char*)msg->payload, msg->payload_length, &message_info)) {
            CM_ERR(cm_log_error_parse_payload);
            iotx_cm_free_message_info(&message_info);
            return FAIL_RETURN;
        }

        message_info.conn_ctx = msg->conn_ctx;

        iotx_cm_cloud_conn_response_handler(cm_ctx, &message_info);
#endif /* CM_SUPPORT_MULTI_THREAD */
    }
        break;

    default:
        CM_WARNING(cm_log_warning_not_arrived);
        break;
    }

    return SUCCESS_RETURN;
}


int iotx_cm_cloud_conn_disconnect_handler(iotx_cm_conntext_t* cm_ctx)
{
    iotx_cm_event_msg_t event;

    event.event_id = IOTX_CM_EVENT_CLOUD_DISCONNECT;
    event.msg = NULL;

    CM_INFO(cm_log_info_cloud_disconnect);

    iotx_cm_trigger_event_callback(cm_ctx, &event);

    return SUCCESS_RETURN;
}

int iotx_cm_cloud_conn_reconnect_handler(iotx_cm_conntext_t* cm_ctx)
{
    iotx_cm_event_msg_t event;

    event.event_id = IOTX_CM_EVENT_CLOUD_RECONNECT;
    event.msg = NULL;

    CM_INFO(cm_log_info_cloud_reconnect);

    iotx_cm_trigger_event_callback(cm_ctx, &event);

    return SUCCESS_RETURN;
}

int iotx_cm_cloud_conn_register_handler(iotx_cm_conntext_t* cm_ctx, char* URI, int result, int is_register)
{
    iotx_cm_event_msg_t event;
    iotx_cm_event_result_t event_result = {0};

    if (NULL == cm_ctx || NULL == URI) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    event_result.result = result;
    event_result.URI = CM_malloc(CM_TOPIC_LEN_MAX);
    if (event_result.URI) {
        strncpy(event_result.URI, URI, CM_TOPIC_LEN_MAX);
    } else {
        return FAIL_RETURN;
    }

    if (is_register)
        event.event_id = IOTX_CM_EVENT_REGISTER_RESULT;
    else
        event.event_id = IOTX_CM_EVENT_UNREGISTER_RESULT;

    event.msg = (void*)&event_result;

    iotx_cm_trigger_event_callback(cm_ctx, &event);

    LITE_free(event_result.URI);

    return SUCCESS_RETURN;
}


int iotx_cm_cloud_conn_response_handler(iotx_cm_conntext_t* cm_ctx, iotx_cm_message_info_t* message_info)
{
    if (NULL == cm_ctx || NULL == message_info) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    CM_INFO(cm_log_info_URI, (char*)message_info->URI);

    if (cm_ctx->response_func)
        cm_ctx->response_func(cm_ctx, message_info);

    return SUCCESS_RETURN;
}


int iotx_cm_cloud_conn_connect(void* handler, void* _connectivity)
{
    iotx_cm_connectivity_t *connectivity = NULL;
    iotx_connection_t* connection = NULL;

    connectivity = (iotx_cm_connectivity_t *)_connectivity;
    connection = (iotx_connection_t*)connectivity->context;

    connection->context = connection->init_func(connection);
    if (connection->context) {
        connectivity->is_connected = 1;

        return SUCCESS_RETURN;
    }

    return FAIL_RETURN;
}


#ifdef CM_VIA_CLOUD_CONN_MQTT
static void* _mqtt_init(void* handler)
{
    iotx_connection_t* connection = NULL;

    connection = CM_malloc(sizeof(iotx_connection_t));

    if (NULL == connection) return NULL;

    connection->init_func = iotx_cloud_conn_mqtt_init;
    connection->deinit_func = iotx_cloud_conn_mqtt_deinit;
    connection->send_func = iotx_cloud_conn_mqtt_publish;
    connection->sub_func = iotx_cloud_conn_mqtt_subscribe;
    connection->unsub_func = iotx_cloud_conn_mqtt_unsubscribe;
    connection->add_service_func = NULL;
    connection->remove_service_func = NULL;
    connection->yield_func = iotx_cloud_conn_mqtt_yield;
    connection->protocol_type = IOTX_CONNECTION_PROTOCOL_TYPE_MQTT;
    connection->event_handler = iotx_cm_cloud_conn_event_callback;
    connection->event_pcontext = handler;
    connection->response_handler = iotx_cm_cloud_conn_response_callback;
    return connection;
}
#endif /* CM_VIA_CLOUD_CONN_MQTT */


#ifdef CM_VIA_CLOUD_CONN_COAP
static void* _coap_init(void* handler)
{
    iotx_connection_t* connection = NULL;
    iotx_connection_param_t param = {0};

    connection = CM_malloc(sizeof(iotx_connection_t));

    if (NULL == connection) return NULL;

    connection->init_func = iotx_cloud_conn_coap_init;
    connection->deinit_func = iotx_cloud_conn_coap_deinit;
    connection->send_func = iotx_cloud_conn_coap_send;
    connection->sub_func = iotx_cloud_conn_coap_subscribe;
    connection->sub_func = iotx_cloud_conn_coap_unsubscribe;
    connection->add_service_func = NULL;
    connection->remove_service_func = NULL;
    connection->protocol_type = IOTX_CONNECTION_PROTOCOL_TYPE_COAP;
    connection->event_handler = iotx_cm_cloud_conn_event_callback;
    connection->event_pcontext = handler;
    connection->response_handler = iotx_cm_cloud_conn_response_callback;

    return connection;
}
#endif /* CM_VIA_CLOUD_CONN_COAP */


#ifdef CM_VIA_CLOUD_CONN_HTTP
static void* _http_init(void* handler)
{
    iotx_connection_t* connection = NULL;

    connection = CM_malloc(sizeof(iotx_connection_t));

    if (NULL == connection) return NULL;

    connection->init_func = iotx_cloud_conn_http_init;
    connection->deinit_func = iotx_cloud_conn_http_deinit;
    connection->send_func = iotx_cloud_conn_http_send;
    connection->sub_func = iotx_cloud_conn_http_subscribe;
    connection->sub_func = iotx_cloud_conn_http_unsubscribe;
    connection->add_service_func = NULL;
    connection->remove_service_func = NULL;
    connection->protocol_type = IOTX_CONNECTION_PROTOCOL_TYPE_HTTP;
    connection->event_handler = iotx_cm_cloud_conn_event_callback;
    connection->event_pcontext = handler;
    connection->response_handler = iotx_cm_cloud_conn_response_callback;

    return connection;
}
#endif /* CM_VIA_CLOUD_CONN_HTTP */


void* iotx_cm_cloud_conn_init(void* handler, iotx_cm_init_param_t *pparam)
{
    iotx_cm_conntext_t* cm_ctx = (iotx_cm_conntext_t*)handler;
    iotx_cm_connectivity_t* connectivity = NULL;
    iotx_device_info_t* device_info = NULL;
    iotx_connection_t* connection = NULL;

#ifdef CM_VIA_CLOUD_CONN_MQTT
    connection = _mqtt_init(cm_ctx);
#else
#ifdef CM_VIA_CLOUD_CONN_COAP
    connection = _coap_init(cm_ctx);
#else
    connection = _http_init(cm_ctx);
#endif
#endif

    if (connection == NULL) return NULL;

    connectivity = CM_malloc(sizeof(iotx_cm_connectivity_t));
    if (NULL == connectivity) {
        CM_ERR(cm_log_error_memory);
        connection->deinit_func(connection);
        iotx_cm_cloud_conn_deinit(connectivity);
        return NULL;
    }

    device_info = iotx_device_info_get();
    memset(&g_cloud_target, 0x0, sizeof(iotx_cm_send_peer_t));
    strncpy(g_cloud_target.product_key, device_info->product_key, strlen(device_info->product_key));
    strncpy(g_cloud_target.device_name, device_info->device_name, strlen(device_info->device_name));

    connectivity->context = connection;
#ifdef CM_SUPPORT_MULTI_THREAD
#ifdef CM_SUPPORT_MULTI_THREAD_VIA_HAL
    HAL_ThreadCreate(&connectivity->pthread_process, iotx_cm_cloud_conn_process, handler, NULL, NULL);
#else
    pthread_create(&connectivity->pthread_process, NULL, iotx_cm_cloud_conn_process, (void*)handler);
#endif
    connectivity->process_lock = HAL_MutexCreate();
#endif
    connectivity->init_func = iotx_cm_cloud_conn_init;
    connectivity->connect_func = iotx_cm_cloud_conn_connect;
    connectivity->trigger_connected_func = iotx_cm_cloud_conn_trigger_connected;
    connectivity->register_func = iotx_cm_cloud_conn_register;
    connectivity->unregister_func = iotx_cm_cloud_conn_unregister;
    connectivity->add_service_func = iotx_cm_cloud_conn_add_service;
    connectivity->remove_service_func = iotx_cm_cloud_conn_remove_service;
    connectivity->send_func = iotx_cm_cloud_conn_send;
    connectivity->send_sync_func = iotx_cm_cloud_conn_send_sync;
    connectivity->yield_func = iotx_cm_cloud_conn_yield;
    connectivity->deinit_func = iotx_cm_cloud_conn_deinit;
    connectivity->get_target_func = iotx_cm_cloud_conn_get_target;
#ifdef CM_SUPPORT_MULTI_THREAD
    connectivity->add_send_func = iotx_cm_cloud_conn_add_send;
    connectivity->thread_process_func = iotx_cm_cloud_conn_process;
#endif /* CM_SUPPORT_MULTI_THREAD */

    connectivity->id = iotx_cm_get_connectivity_id();
    connectivity->type = IOTX_CM_CONNECTIVITY_TYPE_CLOUD;

    return connectivity;
}


int iotx_cm_cloud_conn_trigger_connected(void* handler, void* _connectivity, iotx_cm_event_handle_fp_t event_fp, void* user_data)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_cm_conntext_t* cm_ctx = (iotx_cm_conntext_t*)handler;

    if (NULL == connectivity || NULL == cm_ctx) return FAIL_RETURN;

    if (0 == connectivity->is_connected && connectivity->connect_func)
        connectivity->connect_func(cm_ctx, connectivity);

    if (1 == connectivity->is_connected) {
        iotx_cm_event_msg_t event_msg = {0};
        event_msg.event_id = IOTX_CM_EVENT_CLOUD_CONNECTED;
        event_msg.msg = (void*)connectivity->id;
        event_fp ? event_fp(cm_ctx, &event_msg, user_data) : iotx_cm_trigger_event_callback(cm_ctx, &event_msg);
        return SUCCESS_RETURN;
    }

    return FAIL_RETURN;
}


int iotx_cm_cloud_conn_register(void* handler, void* _connectivity, const char* topic_filter)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_connection_t* connection = NULL;

    connection = (iotx_connection_t*)connectivity->context;

    return connection->sub_func(connection, topic_filter, IOTX_CM_MESSAGE_NEED_ACK);
}


int iotx_cm_cloud_conn_unregister(void* handler, void* _connectivity, const char* topic_filter)
{
    iotx_cm_connectivity_t* connectivity = (iotx_cm_connectivity_t*)_connectivity;
    iotx_connection_t* connection = NULL;

    connection = (iotx_connection_t*)connectivity->context;

    return connection->unsub_func(connection, topic_filter);
}


int iotx_cm_cloud_conn_add_service(void* handler, void* _connectivity, const char* topic_filter, iotx_cm_message_auth_types_t auth_type)
{
    return SUCCESS_RETURN;
}

int iotx_cm_cloud_conn_remove_service(void* handler, void* _connectivity, const char* topic_filter)
{
    return SUCCESS_RETURN;
}


int iotx_cm_cloud_conn_send(void* handler,
                            void* _connectivity,
                            iotx_cm_send_peer_t* target,
                            const char* topic_filter,
                            iotx_cm_message_ack_types_t ack_type,
                            const void* payload,
                            int payload_length,
                            void* context)
{
    iotx_cm_connectivity_t* connectivity = (iotx_cm_connectivity_t*)_connectivity;
    iotx_connection_t* connection = NULL;
    iotx_connection_msg_t msg = {0};

    connection = (iotx_connection_t*)connectivity->context;

    msg.type = IOTX_CONNECTION_MESSAGE_TYPE_SEND;
    msg.ack_type = ack_type;
    msg.URI = (char*)topic_filter;
    msg.URI_length = strlen(topic_filter);
    msg.payload = (char*)payload;
    msg.payload_length = payload_length;
    msg.response_handler = iotx_cm_cloud_conn_response_callback;
    msg.response_pcontext = handler;

    return connection->send_func(connection, context, &msg);
}


int iotx_cm_cloud_conn_send_sync(void* handler,
                                 void* _connectivity,
                                 iotx_cm_send_peer_t* target,
                                 const char* topic_filter,
                                 iotx_cm_message_ack_types_t ack_type,
                                 const void* payload,
                                 int payload_length,
                                 void* context)
{
    return FAIL_RETURN;
}


int iotx_cm_cloud_conn_yield(void* _connectivity, int timeout_ms)
{
    iotx_cm_connectivity_t* connectivity = (iotx_cm_connectivity_t*)_connectivity;
    iotx_connection_t* connection = NULL;

    connection = (iotx_connection_t*)connectivity->context;

    return connection->yield_func(connection, timeout_ms);
}


int iotx_cm_cloud_conn_deinit(void* _connectivity)
{
    int ret = FAIL_RETURN;
    iotx_cm_connectivity_t* connectivity = (iotx_cm_connectivity_t*)_connectivity;
    iotx_connection_t* connection = NULL;

    connection = (iotx_connection_t*)connectivity->context;

    ret = connection->deinit_func(connection);

    LITE_free(connection);

    connectivity->context = NULL;

    return ret;
}


iotx_cm_send_peer_t* iotx_cm_cloud_conn_get_target()
{
    return &g_cloud_target;
}


#ifdef CM_SUPPORT_MULTI_THREAD
int iotx_cm_cloud_conn_add_send(void* handler,
                                iotx_cm_send_peer_t* target,
                                iotx_cm_message_info_t* message_info)
{
    /* send message to itself thread */
    int ret = 0;
    iotx_cm_process_list_node_t* node = NULL;
    iotx_cm_process_send_pt msg = NULL;
    char* URI;
    void* payload;
    int payload_length = 0;
    iotx_cm_conntext_t* cm_ctx = (iotx_cm_conntext_t*)handler;

    node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD);
    if (NULL == node)
        return FAIL_RETURN;

    node->type = IOTX_CM_PROCESS_CLOUD_SEND;

    node->msg = LITE_malloc(sizeof(iotx_cm_process_send_t));
    if (NULL == node->msg) {
        CM_ERR(cm_log_error_memory);
        iotx_cm_free_list_node(cm_ctx, node);
        return FAIL_RETURN;
    }

    msg = node->msg;
    if (target) {
        msg->target = LITE_malloc(sizeof(iotx_cm_send_peer_t));
        memset(msg->target, 0x0, sizeof(iotx_cm_send_peer_t));
        strncpy(msg->target->device_name, target->device_name, strlen(target->device_name));
        strncpy(msg->target->product_key, target->product_key, strlen(target->product_key));
    } else {
        msg->target = NULL;
    }

    if (FAIL_RETURN == iotx_cm_parse_message(cm_ctx, message_info, &URI, &payload, &payload_length)) {
        CM_ERR(cm_log_error_memory);
        iotx_cm_free_list_node(cm_ctx, node);
        if (msg->target)
            LITE_free(msg->target);
        return FAIL_RETURN;
    }

    msg->URI = URI;
    msg->ack_type = message_info->ack_type;
    msg->payload = payload;
    msg->payload_length = payload_length;
    msg->conn_ctx = message_info->conn_ctx;

    ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);

    if (FAIL_RETURN == ret) {
        if (msg->target)
            LITE_free(msg->target);
        LITE_free(msg->URI);
        LITE_free(msg->payload);
        LITE_free(node->msg);
        iotx_cm_free_list_node(cm_ctx, node);
    }

    return SUCCESS_RETURN;
}


void* iotx_cm_cloud_conn_process(void *pclient)
{
    iotx_cm_conntext_t* cm_ctx = (iotx_cm_conntext_t*)pclient;
    iotx_cm_connectivity_t* connectivity = iotx_cm_find_connectivity(cm_ctx, iotx_cm_cloud_conn_get_target(), NULL);

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_parameter);
        return NULL;
    }

    log_info("enter cloud process");

    while (!cm_ctx->thread_stop) {
        if (NULL == connectivity) {
            HAL_SleepMs(1000);
            connectivity = iotx_cm_find_connectivity(cm_ctx, iotx_cm_cloud_conn_get_target(), NULL);
            continue;
        }

        if (0 == connectivity->is_connected) {
            connectivity->trigger_connected_func(cm_ctx, connectivity, NULL, NULL);
            continue;
        }

        if (iotx_cm_process_list_get_size(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD) > 0) {
            iotx_cm_process_list_node_t* node = NULL;
            if (NULL != (node = iotx_cm_process_list_pop(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD))) {
                log_info("process %d", node->type);

                switch(node->type) {
                case IOTX_CM_PROCESS_CLOUD_REGISTER: {
                    iotx_cm_process_register_t* register_pt = (iotx_cm_process_register_t*)node->msg;
                    if (FAIL_RETURN == iotx_cm_register_service(cm_ctx, register_pt->URI, register_pt->type, register_pt->register_func,
                                                                register_pt->user_data, register_pt->mail_box)) {
                        log_info("register fail");
                    }

                    LITE_free(register_pt->URI);
                    LITE_free(register_pt);
                }
                    break;

                case IOTX_CM_PROCESS_CLOUD_UNREGISTER: {
                    char* URI = (char*)node->msg;
                    if (FAIL_RETURN == iotx_cm_unregister_service(cm_ctx, URI)) {
                        log_info("unregister fail");
                    }

                    LITE_free(URI);
                }
                    break;

                case IOTX_CM_PROCESS_CLOUD_SEND: {
                    iotx_cm_process_send_pt send_pt = (iotx_cm_process_send_pt)node->msg;
                    if (FAIL_RETURN == iotx_cm_send_data(cm_ctx,
                                                         send_pt->target,
                                                         iotx_cm_find_connectivity(cm_ctx, &g_cloud_target, send_pt->conn_ctx),
                                                         send_pt->URI,
                                                         send_pt->ack_type,
                                                         send_pt->payload,
                                                         send_pt->payload_length,
                                                         send_pt->conn_ctx)) {
                        log_info("send fail");
                    }

                    if (send_pt->target)
                        LITE_free(send_pt->target);
                    LITE_free(send_pt);
                }
                    break;

                case IOTX_CM_PROCESS_CLOUD_DISCONNECT: {
                    iotx_cm_cloud_conn_disconnect_handler(cm_ctx);
                }
                    break;

                case IOTX_CM_PROCESS_CLOUD_RECONNECT: {
                    iotx_cm_cloud_conn_reconnect_handler(cm_ctx);
                }
                    break;

                case IOTX_CM_PROCESS_CLOUD_REGISTER_RESULT:
                case IOTX_CM_PROCESS_CLOUD_UNREGISTER_RESULT: {
                    iotx_cm_process_register_result_pt register_pt = (iotx_cm_process_register_result_pt)node->msg;
                    if (register_pt->URI) iotx_cm_cloud_conn_register_handler(cm_ctx, register_pt->URI, register_pt->result, register_pt->is_register);
                    LITE_free(register_pt->URI);
                    LITE_free(register_pt);
                }
                    break;

                case IOTX_CM_PROCESS_CLOUD_NEW_DATA: {
                    iotx_cm_message_info_t* message_info = (iotx_cm_message_info_t*)node->msg;
                    iotx_cm_cloud_conn_response_handler(cm_ctx, message_info);
                    LITE_free(node->msg);
                    break;
                }

                default:
                    break;
                }

                iotx_cm_free_list_node(cm_ctx, node);
            }
            /* cloud yield */
            connectivity->yield_func(connectivity, 200);
        } else {
            /* cloud yield */
            connectivity->yield_func(connectivity, 200);
        }
    }

    return NULL;
}
#endif /* CM_SUPPORT_MULTI_THREAD */

#endif /* CM_VIA_CLOUD_CONN */

