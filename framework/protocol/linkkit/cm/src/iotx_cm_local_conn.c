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

#ifdef CM_SUPPORT_LOCAL_CONN

#include "iot_import.h"

#include "lite-utils.h"
#include "lite-system.h"
#include "iot_export.h"
#include "iotx_cm_common.h"
#include "iotx_cm_local_conn.h"
#include "iotx_cm_connection.h"
#include "iotx_local_conn_alcs.h"

typedef struct iotx_cm_local_device_list_st {
    iotx_cm_local_device_t*                        node;
    void*                                          next;
    int                                            is_used;
} iotx_cm_local_device_list_t;

static iotx_cm_local_device_list_t* g_local_device = NULL;

#if 0
static iotx_cm_local_device_list_t iotx_cm_local_device_list[CM_SUPPORT_MAX_MAPPING_SIZE];

static int iotx_cm_local_conn_add_device(iotx_cm_local_device_t* device)
{
    iotx_cm_local_device_list_t* device_list = NULL;
    iotx_cm_local_device_list_t* temp = NULL;
    int i = 0;

    if (NULL == device) {
        log_info("parameter error");
        return FAIL_RETURN;
    }

    for (i = 0; i < CM_SUPPORT_MAX_MAPPING_SIZE; i++) {
        if (iotx_cm_local_device_list[i].is_used == 0) {
            iotx_cm_local_device_list[i].is_used = 1;
            device_list = &iotx_cm_local_device_list[i];
        }
    }

    if (NULL == device_list) {
        return FAIL_RETURN;
    }

    device_list->node = device;

    if (NULL == g_local_device) {
        g_local_device = device_list;
        return SUCCESS_RETURN;
    }

    temp = g_local_device;
    while (temp->next) {
        temp = temp->next;
    }

    temp->next = device_list;

    return SUCCESS_RETURN;
}

/* remove */
static int iotx_cm_local_conn_remove_device(iotx_cm_local_device_t* device)
{
    iotx_cm_local_device_list_t* temp = NULL;
    iotx_cm_local_device_list_t* pre_temp = NULL;

    if (NULL == g_local_device) {
        log_info("parameter error");
        return FAIL_RETURN;
    }

    if (g_local_device->node == device) {
        temp = g_local_device;
        g_local_device = g_local_device->next;

        /* free */
        LITE_free(temp->node->device_info);
        LITE_free(temp->node);
        temp->is_used = 0;
        memset(temp, 0x0, sizeof(iotx_cm_local_device_list_t));
        return SUCCESS_RETURN;
    }

    pre_temp = g_local_device;
    temp = g_local_device->next;
    while (temp) {
        if (device == temp->node) {
            pre_temp->next = temp->next;

            /* free */
            LITE_free(temp->node->device_info);
            LITE_free(temp->node);
            temp->is_used = 0;
            memset(temp, 0x0, sizeof(iotx_cm_local_device_list_t));
            return SUCCESS_RETURN;
        }

        pre_temp = temp;
        temp = temp->next;
    }

    return FAIL_RETURN;
}


/* remove all */
static int iotx_cm_local_conn_remove_device_all()
{
    int i = 0;
    for (i = 0; i < CM_SUPPORT_MAX_MAPPING_SIZE; i++) {
        if (iotx_cm_local_device_list[i].is_used == 1) {
            LITE_free(iotx_cm_local_device_list[i].node->device_info);
            LITE_free(iotx_cm_local_device_list[i].node);
        }
    }
    return SUCCESS_RETURN;
}


static int iotx_cm_local_compare_device(iotx_cm_send_peer_t* device_info_1, iotx_cm_send_peer_t* device_info_2)
{
    if (NULL == device_info_1 || NULL == device_info_2)
        return -1;

    if (0 == strncmp(device_info_1->product_key, device_info_2->product_key, strlen(device_info_1->product_key)) &&
            0 == strncmp(device_info_1->device_name, device_info_2->device_name, strlen(device_info_1->device_name))) {
        return 0;
    }

    return -1;
}

/* find */
static iotx_cm_local_device_t* iotx_cm_local_conn_find_device(iotx_cm_send_peer_t* device_info)
{
    iotx_cm_local_device_list_t* temp = NULL;

    if (NULL == g_local_device) {
        log_info("parameter error");
        return NULL;
    }

    if (0 == iotx_cm_local_compare_device(g_local_device->node->device_info, device_info)) {
        return g_local_device->node;
    }

    temp = g_local_device->next;
    while (temp) {
        if (0 == iotx_cm_local_compare_device(temp->node->device_info, device_info)) {
            return temp->node;
        }

        temp = temp->next;
    }

    return NULL;
}
#endif


static void iotx_local_conn_event_callback(void *pcontext, iotx_connection_event_msg_t* event_msg)
{
    iotx_cm_conntext_t* cm_ctx = (iotx_cm_conntext_t*)pcontext;

    if (NULL == cm_ctx || NULL == event_msg) {
        CM_ERR(cm_log_error_parameter);
        return;
    }

    switch (event_msg->event_id) {
    case IOTX_CONNECTION_EVENT_DISCONNECT:
#ifdef CM_SUPPORT_MULTI_THREAD
    {
        /* send message to itself thread */
        int ret = 0;
        iotx_cm_process_list_node_t* node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL);
        if (NULL == node) return;

        node->type = IOTX_CM_PROCESS_LOCAL_DISCONNECT;
        node->msg = NULL;

        if (FAIL_RETURN == (ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node)))
            iotx_cm_free_list_node(cm_ctx, node);
    }
#else
        iotx_cm_local_conn_disconnect_handler(cm_ctx);
#endif /* CM_SUPPORT_MULTI_THREAD */
        break;

    case IOTX_CONNECTION_EVENT_RECONNECT:{
#ifdef CM_SUPPORT_MULTI_THREAD
        {
            /* send message to itself thread */
            int ret = 0;
            iotx_cm_process_list_node_t* node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL);
            if (NULL == node) return;
            node->type = IOTX_CM_PROCESS_LOCAL_RECONNECT;
            node->msg = NULL;

            if (FAIL_RETURN == (ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node)))
                iotx_cm_free_list_node(cm_ctx, node);
        }
#else
        iotx_cm_local_conn_reconnect_handler(cm_ctx);
#endif /* CM_SUPPORT_MULTI_THREAD */
    }
        break;

#if 0
    case IOTX_LOCAL_CONN_EVENT_ADD_DEVICE: {
        iotx_local_conn_device_event_t* msg = (iotx_local_conn_device_event_t*)event_msg->msg;
        iotx_cm_send_peer_t* device_info = NULL;
        iotx_cm_local_device_t* device = NULL;

        if (NULL == msg) return;

        CM_INFO(cm_log_info_add_device);

        device_info = LITE_malloc(sizeof(iotx_cm_event_device_t));
        if (NULL == device_info) return;

        device = LITE_malloc(sizeof(iotx_cm_local_device_t));
        if (NULL == device) {
            LITE_free(device_info);
            return;
        }

        memset(device_info, 0x0, sizeof(iotx_cm_event_device_t));
        stretpy(device_info->product_key, msg->product_key);
        stretpy(device_info->device_name, msg->device_name);
        device->device_info = device_info;
        device->addr = msg->addr;

#ifdef CM_SUPPORT_MULTI_THREAD
        {
            /* send message to itself thread */
            int ret = 0;
            iotx_cm_process_list_node_t* node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL);
            if (NULL == node) return;
            node->type = IOTX_CM_PROCESS_LOCAL_ADD_DEVICE;
            node->msg = device;

            ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);

            if (FAIL_RETURN == ret) iotx_cm_free_list_node(cm_ctx, node);
        }
#else
        iotx_cm_local_conn_found_device_handler(cm_ctx, device);
#endif /* CM_SUPPORT_MULTI_THREAD */
    }
        break;

    case IOTX_LOCAL_CONN_EVENT_REMOVE_DEVICE: {
        iotx_local_conn_device_event_t* msg = (iotx_local_conn_device_event_t*)event_msg->msg;
        iotx_cm_send_peer_t* device_info = NULL;
        iotx_cm_local_device_t* device = NULL;

        if (NULL == msg) return;

        CM_INFO(cm_log_info_remove_device);

        device = LITE_malloc(sizeof(iotx_cm_local_device_t));
        if (NULL == device) {
            LITE_free(device_info);
            return;
        }

        device->device_info = device_info;
        device->addr = msg->addr;

#ifdef CM_SUPPORT_MULTI_THREAD
        {
            /* send message to itself thread */
            int ret = 0;
            iotx_cm_process_list_node_t* node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL);
            if (NULL == node) return;
            node->type = IOTX_CM_PROCESS_LOCAL_REMOVE_DEVICE;
            node->msg = device;

            ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);

            if (FAIL_RETURN == ret) iotx_cm_free_list_node(cm_ctx, node);
        }
#else
        iotx_cm_local_conn_remove_device_handler(cm_ctx, device);
#endif /* CM_SUPPORT_MULTI_THREAD */
    }
        break;
#endif

    default:
        break;
    }
}

static int iotx_cm_local_conn_response_callback(void* pcontext, iotx_connection_msg_rsp_t* msg)
{
    iotx_cm_conntext_t* cm_ctx = (iotx_cm_conntext_t*)pcontext;

    if (NULL == cm_ctx || NULL == msg) {
        CM_ERR(cm_log_error_fail);
        return FAIL_RETURN;
    }

    CM_INFO(cm_log_info_rsp_type, msg->rsp_type);

    switch (msg->rsp_type) {
    case IOTX_CONNECTION_RESPONSE_ADD_SERVICE_SUCCESS: {
#ifdef CM_SUPPORT_MULTI_THREAD
        {
            /* send message to itself thread */
            int ret = 0;
            iotx_cm_process_list_node_t* node = NULL;
            iotx_cm_process_service_result_pt result_msg = NULL;

            if (NULL == (node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD))) return FAIL_RETURN;

            node->type = IOTX_CM_PROCESS_LOCAL_ADD_SERVICE_RESULT;
            node->msg = LITE_malloc(sizeof(iotx_cm_process_service_result_t));
            if (NULL == node) {
                CM_ERR(cm_log_error_memory);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }

            result_msg = node->msg;

            result_msg->URI = LITE_malloc(strlen(msg->URI) + 1);
            if (NULL == result_msg->URI) {
                LITE_free(node->msg);
                LITE_free(node);
                return FAIL_RETURN;
            }
            memset(result_msg->URI, 0x0, strlen(msg->URI) + 1);
            strncpy(result_msg->URI, (msg->URI), strlen(msg->URI) + 1);
            result_msg->result = 0;
            result_msg->is_add = 1;

            ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);

            if (FAIL_RETURN == ret) {
                LITE_free(result_msg->URI);
                LITE_free(node->msg);
                LITE_free(node);
                return FAIL_RETURN;
            }
        }
#else
        return iotx_cm_local_conn_service_handler(cm_ctx, msg->URI, 0, 1);
#endif /* CM_SUPPORT_MULTI_THREAD */
    }
        break;

    case IOTX_CONNECTION_RESPONSE_ADD_SERVICE_FAIL: {
#ifdef CM_SUPPORT_MULTI_THREAD
        {
            /* send message to itself thread */
            int ret = 0;
            iotx_cm_process_list_node_t* node = NULL;
            iotx_cm_process_service_result_pt result_msg = NULL;

            if (NULL == (node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL))) return FAIL_RETURN;

            node->type = IOTX_CM_PROCESS_LOCAL_ADD_SERVICE_RESULT;
            node->msg = LITE_malloc(sizeof(iotx_cm_process_service_result_t));
            if (NULL == node) {
                CM_ERR(cm_log_error_memory);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }

            result_msg = node->msg;

            result_msg->URI = LITE_malloc(strlen(msg->URI) + 1);
            if (NULL == result_msg->URI) {
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }
            memset(result_msg->URI, 0x0, strlen(msg->URI) + 1);
            strncpy(result_msg->URI, (msg->URI), strlen(msg->URI) + 1);

            result_msg->result = -1;
            result_msg->is_add = 1;

            ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);

            if (FAIL_RETURN == ret) {
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }
        }
#else
        return iotx_cm_local_conn_service_handler(cm_ctx, msg->URI, -1, 1);
#endif /* CM_SUPPORT_MULTI_THREAD */
    }
        break;

    case IOTX_CONNECTION_RESPONSE_REMOVE_SERVICE_SUCCESS: {
#ifdef CM_SUPPORT_MULTI_THREAD
        {
            /* send message to itself thread */
            int ret = 0;
            iotx_cm_process_list_node_t* node = NULL;
            iotx_cm_process_service_result_pt result_msg = NULL;

            if (NULL == (node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL))) return FAIL_RETURN;

            node->type = IOTX_CM_PROCESS_LOCAL_REMOVE_SERVICE_RESULT;
            node->msg = LITE_malloc(sizeof(iotx_cm_process_service_result_t));
            if (NULL == node) {
                CM_ERR(cm_log_error_memory);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }

            result_msg = node->msg;

            result_msg->URI = LITE_malloc(strlen(msg->URI) + 1);
            if (NULL == result_msg->URI) {
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }
            memset(result_msg->URI, 0x0, strlen(msg->URI) + 1);
            strncpy(result_msg->URI, (msg->URI), strlen(msg->URI) + 1);
            result_msg->result = 0;
            result_msg->is_add = 0;

            ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);

            if (FAIL_RETURN == ret) {
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }
        }
#else
        return iotx_cm_local_conn_service_handler(cm_ctx, msg->URI, 0, 0);
#endif /* CM_SUPPORT_MULTI_THREAD */
    }
        break;

    case IOTX_CONNECTION_RESPONSE_REMOVE_SERVICE_FAIL: {
#ifdef CM_SUPPORT_MULTI_THREAD
        {
            /* send message to itself thread */
            int ret = 0;
            iotx_cm_process_list_node_t* node = NULL;
            iotx_cm_process_service_result_pt result_msg = NULL;

            if (NULL == (node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL))) return FAIL_RETURN;

            node->type = IOTX_CM_PROCESS_LOCAL_REMOVE_SERVICE_RESULT;
            node->msg = LITE_malloc(sizeof(iotx_cm_process_service_result_t));
            if (NULL == node) {
                CM_ERR(cm_log_error_memory);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }

            result_msg = node->msg;

            result_msg->URI = LITE_malloc(strlen(msg->URI) + 1);
            if (NULL == result_msg->URI) {
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }
            memset(result_msg->URI, 0x0, strlen(msg->URI) + 1);
            strncpy(result_msg->URI, (msg->URI), strlen(msg->URI) + 1);
            result_msg->result = -1;
            result_msg->is_add = 0;

            ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);

            if (FAIL_RETURN == ret) {
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
            }
        }
#else
        return iotx_cm_local_conn_service_handler(cm_ctx, msg->URI, -1, 0);
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

            if (NULL == (node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL))) return FAIL_RETURN;

            node->type = IOTX_CM_PROCESS_LOCAL_NEW_DATA;
            node->msg = LITE_malloc(sizeof(iotx_cm_message_info_t));
            if (NULL == node->msg) {
                CM_ERR(cm_log_error_memory);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }

            msg_info = node->msg;

            msg_info->URI = LITE_malloc(msg->URI_length + 1);
#if 0
            if (NULL == msg_info->URI) {
                CM_ERR(cm_log_error_memory);
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }

            if (FAIL_RETURN == iotx_cm_parse_URI((char*)msg->URI, msg->URI_length, msg_info->URI, &msg_info->URI_type)) {
                CM_ERR(cm_log_error_parse_URI);
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }
#endif
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
            mapping = iotx_cm_find_mapping(cm_ctx, (char*)msg->URI, msg->URI_length);
            if (NULL == mapping) {
                CM_WARNING(cm_log_warning_not_mapping);
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }
            msg_info->message_type = mapping->type;
#endif /* CM_SUPPORT_TOPIC_DISPATCH */

            if (FAIL_RETURN == iotx_cm_parse_payload((char*)msg->payload, msg->payload_length, msg_info)) {
                CM_ERR(cm_log_error_parse_URI);
                iotx_cm_free_message_info(msg_info);
                LITE_free(node->msg);
                iotx_cm_free_list_node(cm_ctx, node);
                return FAIL_RETURN;
            }

            ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);

            if (FAIL_RETURN == ret) {
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
#endif /* CM_SUPPORT_TOPIC_DISPATCH */

        memset(&message_info, 0x0, sizeof(iotx_cm_message_info_t));

        message_info.URI = LITE_malloc(msg->URI_length + 1);
#if 0
        if (NULL == message_info.URI) {
            CM_ERR(cm_log_error_memory);
            return FAIL_RETURN;
        }
        memset(message_info.URI, 0x0, msg->URI_length + 1);
        if (FAIL_RETURN == iotx_cm_parse_URI((char*)msg->URI, msg->URI_length, message_info.URI, &message_info.URI_type)) {
            CM_ERR(cm_log_error_parse_URI);
            iotx_cm_free_message_info(&message_info);
            return FAIL_RETURN;
        }
#endif
        if (message_info.URI) {
            strncpy(message_info.URI, msg->URI, msg->URI_length);
        } else {
            iotx_cm_free_message_info(&message_info);
            return FAIL_RETURN;
        }

#ifdef CM_SUPPORT_TOPIC_DISPATCH
        /* find mapping */
        mapping = iotx_cm_find_mapping(cm_ctx, (char*)msg->URI, msg->URI_length);
        if (NULL == mapping) {
            CM_WARNING(cm_log_warning_not_mapping);
            iotx_cm_free_message_info(&message_info);
            return FAIL_RETURN;
        }
        message_info.message_type = mapping->type;
#endif /* CM_SUPPORT_TOPIC_DISPATCH */

        if (FAIL_RETURN == iotx_cm_parse_payload((char*)msg->payload, msg->payload_length, &message_info)) {
            CM_ERR(cm_log_error_parse_payload);
            iotx_cm_free_message_info(&message_info);
            return FAIL_RETURN;
        }

        message_info.conn_ctx = msg->conn_ctx;

        return iotx_cm_local_conn_response_handler(cm_ctx, &message_info);
#endif /* CM_SUPPORT_MULTI_THREAD */
    }
        break;

    default:
        CM_INFO(cm_log_warning_not_arrived);
        break;
    }

    return FAIL_RETURN;
}


int iotx_cm_local_conn_service_handler(iotx_cm_conntext_t* cm_ctx, char* URI, int result, int is_add)
{
    iotx_cm_event_msg_t event;
    iotx_cm_event_result_t event_result = {0};

    if (NULL == cm_ctx || NULL == URI) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    event_result.result = result;
    event_result.URI = LITE_malloc(CM_TOPIC_LEN_MAX);
#if 0
    if (NULL == event_result.URI) {
        CM_ERR(cm_log_error_memory);
        return FAIL_RETURN;
    }
    memset(event_result.URI, 0x0, CM_TOPIC_LEN_MAX);

    if (FAIL_RETURN == iotx_cm_parse_URI((char*)URI, strlen(event_result.URI), event_result.URI, &event_result.URI_type)) {
        CM_ERR(cm_log_error_parse_URI);
        LITE_free(event_result.URI);
        return FAIL_RETURN;
    }
#endif
    if (event_result.URI) {
        strncpy(event_result.URI, URI, CM_TOPIC_LEN_MAX);
    } else {
        return FAIL_RETURN;
    }

    event.event_id = is_add ? IOTX_CM_EVENT_ADD_SERVICE_RESULT : IOTX_CM_EVENT_REMOVE_SERVICE_RESULT;

    event.msg = (void*)&event_result;

    iotx_cm_trigger_event_callback(cm_ctx, &event);

    LITE_free(event_result.URI);

    return SUCCESS_RETURN;
}


int iotx_cm_local_conn_disconnect_handler(iotx_cm_conntext_t* cm_ctx)
{
    iotx_cm_event_msg_t event;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    event.event_id = IOTX_CM_EVENT_LOCAL_DISCONNECT;
    event.msg = NULL;

    CM_INFO(cm_log_info_local_disconnect);

    iotx_cm_trigger_event_callback(cm_ctx, &event);

    return SUCCESS_RETURN;
}

int iotx_cm_local_conn_reconnect_handler(iotx_cm_conntext_t* cm_ctx)
{
    iotx_cm_event_msg_t event;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    event.event_id = IOTX_CM_EVENT_LOCAL_RECONNECT;
    event.msg = NULL;

    CM_INFO(cm_log_info_local_reconnect);

    iotx_cm_trigger_event_callback(cm_ctx, &event);

    return SUCCESS_RETURN;
}

int iotx_cm_local_conn_found_device_handler(iotx_cm_conntext_t* cm_ctx, iotx_cm_local_device_t* device)
{
#if 0
    iotx_cm_event_msg_t event;

    if (FAIL_RETURN == iotx_cm_local_conn_add_device(device)) {
        return FAIL_RETURN;
    }

    event.event_id = IOTX_CM_EVENT_FOUND_DEVICE;
    event.msg = device->device_info;

    CM_INFO(cm_log_info_found_device);
    iotx_cm_trigger_event_callback(cm_ctx, &event);
#endif
    return SUCCESS_RETURN;
}

int iotx_cm_local_conn_remove_device_handler(iotx_cm_conntext_t* cm_ctx, iotx_cm_local_device_t* device)
{
#if 0
    iotx_cm_event_msg_t event;
    if (FAIL_RETURN == iotx_cm_local_conn_remove_device(device)) {
        return FAIL_RETURN;
    }

    event.event_id = IOTX_CM_EVENT_REMOVE_DEVICE;
    event.msg = device->device_info;

    CM_INFO(cm_log_info_remove_device);
    iotx_cm_trigger_event_callback(cm_ctx, &event);

#endif
    return SUCCESS_RETURN;
}


int iotx_cm_local_conn_response_handler(iotx_cm_conntext_t* cm_ctx,
                                        iotx_cm_message_info_t* message_info)
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


int iotx_cm_local_conn_connect(void* handler, void* _connectivity)
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


static void* _alcs_init(void* handler)
{
    iotx_connection_t* connection = NULL;

    connection = CM_malloc(sizeof(iotx_connection_t));

    if (NULL == connection) return NULL;

    connection->init_func = iotx_local_conn_alcs_init;
    connection->deinit_func = iotx_local_conn_alcs_deinit;
    connection->send_func = iotx_local_conn_alcs_send;
    connection->sub_func = NULL;
    connection->unsub_func = NULL;
    connection->add_service_func = iotx_local_conn_alcs_add_service;
    connection->remove_service_func = iotx_local_conn_alcs_remove_service;
    connection->yield_func = iotx_local_conn_alcs_yield;
    connection->protocol_type = IOTX_CONNECTION_PROTOCOL_TYPE_ALCS;
    connection->event_handler = iotx_local_conn_event_callback;
    connection->event_pcontext = handler;
    connection->response_handler = iotx_cm_local_conn_response_callback;

    g_local_device = CM_malloc(sizeof(iotx_cm_local_device_list_t));
    memset(g_local_device, 0x0, sizeof(iotx_cm_local_device_list_t));

    g_local_device->node = CM_malloc(sizeof(iotx_cm_local_device_t));
    memset(g_local_device->node, 0x0, sizeof(iotx_cm_local_device_t));

    g_local_device->node->device_info = CM_malloc(sizeof(iotx_cm_send_peer_t));
    memset(g_local_device->node->device_info, 0x0, sizeof(iotx_cm_send_peer_t));

    strcpy(g_local_device->node->device_info->device_name, "device_name");
    strcpy(g_local_device->node->device_info->product_key, "product_key");

    return connection;
}

void* iotx_cm_local_conn_init(void* handler, iotx_cm_init_param_t* pparam)
{
    iotx_cm_conntext_t* cm_ctx = (iotx_cm_conntext_t*)handler;
    iotx_cm_connectivity_t* connectivity = NULL;
    iotx_connection_t* connection = NULL;

    if (NULL == cm_ctx || NULL == pparam) {
        CM_ERR(cm_log_error_parameter);
        return NULL;
    }
    connection = _alcs_init(cm_ctx);

    if (connection == NULL) return NULL;

    connectivity = LITE_malloc(sizeof(iotx_cm_connectivity_t));
    if (NULL == connectivity) {
        CM_ERR(cm_log_error_memory);
        iotx_cm_local_conn_deinit(connectivity);
        return NULL;
    }

    connectivity->context = connection;

#ifdef CM_SUPPORT_MULTI_THREAD
#ifdef CM_SUPPORT_MULTI_THREAD_VIA_HAL
    HAL_ThreadCreate(&connectivity->pthread_process, iotx_cm_local_conn_process, cm_ctx, NULL, NULL);
#else
    pthread_create(&connectivity->pthread_process, NULL, iotx_cm_local_conn_process, (void*)cm_ctx);
#endif
    connectivity->process_lock = HAL_MutexCreate();
#endif /* CM_SUPPORT_MULTI_THREAD */
    connectivity->init_func = iotx_cm_local_conn_init;
    connectivity->connect_func = iotx_cm_local_conn_connect;
    connectivity->register_func = iotx_cm_local_conn_register;
    connectivity->trigger_connected_func = iotx_cm_local_conn_trigger_connected;
    connectivity->unregister_func = iotx_cm_local_conn_unregister;
    connectivity->add_service_func = iotx_cm_local_conn_add_service;
    connectivity->remove_service_func = iotx_cm_local_conn_remove_service;
    connectivity->send_func = iotx_cm_local_conn_send;
    connectivity->send_sync_func = iotx_cm_local_conn_send_sync;
    connectivity->yield_func = iotx_cm_local_conn_yield;
    connectivity->deinit_func = iotx_cm_local_conn_deinit;
    connectivity->get_target_func = iotx_cm_local_conn_get_target;
#ifdef CM_SUPPORT_MULTI_THREAD
    connectivity->add_send_func = iotx_cm_local_conn_add_send;
    connectivity->thread_process_func = iotx_cm_local_conn_process;
#endif /* CM_SUPPORT_MULTI_THREAD */

    connectivity->id = iotx_cm_get_connectivity_id();
    connectivity->type = IOTX_CM_CONNECTIVITY_TYPE_LOCAL;

    return connectivity;
}


int iotx_cm_local_conn_trigger_connected(void* handler, void* _connectivity, iotx_cm_event_handle_fp_t event_fp, void* user_data)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_cm_conntext_t* cm_ctx = (iotx_cm_conntext_t*)handler;

    if (NULL == connectivity || NULL == cm_ctx) return FAIL_RETURN;

    if (0 == connectivity->is_connected && connectivity->connect_func)
        connectivity->connect_func(cm_ctx, connectivity);

    if (1 == connectivity->is_connected) {
        iotx_cm_event_msg_t event_msg = {0};
        event_msg.event_id = IOTX_CM_EVENT_LOCAL_CONNECTED;
        event_msg.msg = (void*)connectivity->id;
        event_fp ? event_fp(cm_ctx, &event_msg, user_data) : iotx_cm_trigger_event_callback(cm_ctx, &event_msg);
        return SUCCESS_RETURN;
    }

    return FAIL_RETURN;
}


int iotx_cm_local_conn_register(void* handler, void* _connectivity, const char* topic_filter)
{
    return SUCCESS_RETURN;
}


int iotx_cm_local_conn_unregister(void* handler, void* _connectivity, const char* topic_filter)
{
    return SUCCESS_RETURN;
}


int iotx_cm_local_conn_add_service(void* handler, void* _connectivity, const char* topic_filter, iotx_cm_message_auth_types_t auth_type)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_connection_t* connection = NULL;

    connection = (iotx_connection_t*)connectivity->context;

    return connection->add_service_func(connection, topic_filter, IOTX_CM_MESSAGE_NEED_ACK, auth_type);
}


int iotx_cm_local_conn_remove_service(void* handler, void* _connectivity, const char* topic_filter)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_connection_t* connection = NULL;

    connection = (iotx_connection_t*)connectivity->context;

    return connection->remove_service_func(connection, topic_filter);
}


int iotx_cm_local_conn_send(void* handler,
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
    msg.response_handler = iotx_cm_local_conn_response_callback;
    msg.response_pcontext = handler;

    return connection->send_func(connection, context, &msg);
}


int iotx_cm_local_conn_send_sync(void* handler,
                                 void* _connectivity,
                                 iotx_cm_send_peer_t* target,
                                 const char* topic_filter,
                                 iotx_cm_message_ack_types_t ack_type,
                                 const void* payload,
                                 int payload_length,
                                 void* context)
{
    return SUCCESS_RETURN;
}


int iotx_cm_local_conn_yield(void* _connectivity, int timeout_ms)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_connection_t* connection = NULL;

    connection = (iotx_connection_t*)connectivity->context;

    return connection->yield_func(connection, timeout_ms);
}


int iotx_cm_local_conn_deinit(void* _connectivity)
{
    int ret;
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_connection_t* connection = NULL;

    connection = (iotx_connection_t*)connectivity->context;

    if(g_local_device->node->device_info) LITE_free(g_local_device->node->device_info);
    if(g_local_device->node) LITE_free(g_local_device->node);
    if(g_local_device) LITE_free(g_local_device);

    ret = connection->deinit_func(connection);

    LITE_free(connection);

    connectivity->context = NULL;

    return ret;
}


iotx_cm_send_peer_t* iotx_cm_local_conn_get_target()
{
    if (g_local_device == NULL)
        return NULL;

    return g_local_device->node->device_info;
}


#ifdef CM_SUPPORT_MULTI_THREAD
int iotx_cm_local_conn_add_send(void* handler,
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

    node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL);
    if (NULL == node) return FAIL_RETURN;

    node->type = IOTX_CM_PROCESS_LOCAL_SEND;

    node->msg = LITE_malloc(sizeof(iotx_cm_process_send_t));
    if (NULL == node->msg) {
        CM_INFO(cm_log_error_parameter);
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
        CM_INFO(cm_log_error_parameter);
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

    ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);

    if (FAIL_RETURN == ret) {
        if (msg->target) LITE_free(msg->target);
        LITE_free(msg->URI);
        LITE_free(msg->payload);
        LITE_free(node->msg);
        iotx_cm_free_list_node(cm_ctx, node);
    }

    return ret;
}

void* iotx_cm_local_conn_process(void *pclient)
{
    iotx_cm_conntext_t* cm_ctx = (iotx_cm_conntext_t*)pclient;
    iotx_cm_connectivity_t* connectivity = iotx_cm_find_connectivity(cm_ctx, iotx_cm_local_conn_get_target(), NULL);

    if (NULL == cm_ctx) {
        CM_INFO(cm_log_error_parameter);
        return NULL;
    }

    log_info("enter cloud process");

    while (!cm_ctx->thread_stop) {
        if (NULL == connectivity) {
            HAL_SleepMs(1000);
            connectivity = iotx_cm_find_connectivity(cm_ctx, iotx_cm_local_conn_get_target(), NULL);
            continue;
        }

        if (0 == connectivity->is_connected) {
            connectivity->trigger_connected_func(cm_ctx, connectivity, NULL, NULL);
            continue;
        }

        if (iotx_cm_process_list_get_size(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL) > 0) {
            iotx_cm_process_list_node_t* node = NULL;
            if (NULL != (node = iotx_cm_process_list_pop(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL))) {
                log_info("process %d", node->type);

                switch(node->type) {
                case IOTX_CM_PROCESS_LOCAL_ADD_DEVICE: {
                    iotx_cm_local_device_t* device = (iotx_cm_local_device_t*)node->msg;
                    iotx_cm_local_conn_found_device_handler(cm_ctx, device);
                }
                    break;

                case IOTX_CM_PROCESS_LOCAL_REMOVE_DEVICE: {
                    iotx_cm_local_device_t* device = (iotx_cm_local_device_t*)node->msg;
                    iotx_cm_local_conn_remove_device_handler(cm_ctx, device);
                }
                    break;

                case IOTX_CM_PROCESS_LOCAL_SEND: {
                    iotx_cm_process_send_pt send_pt = (iotx_cm_process_send_pt)node->msg;
                    if (FAIL_RETURN == iotx_cm_send_data(cm_ctx,
                                                         send_pt->target,
                                                         iotx_cm_find_connectivity(cm_ctx, iotx_cm_local_conn_get_target(), send_pt->conn_ctx),
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

                case IOTX_CM_PROCESS_LOCAL_DISCONNECT: {
                    iotx_cm_local_conn_disconnect_handler(cm_ctx);
                }
                    break;

                case IOTX_CM_PROCESS_LOCAL_RECONNECT: {
                    iotx_cm_local_conn_reconnect_handler(cm_ctx);
                }
                    break;

                case IOTX_CM_PROCESS_LOCAL_ADD_SERVICE: {
                    iotx_cm_process_service_t* service_pt = (iotx_cm_process_service_t*)node->msg;
                    if (FAIL_RETURN == iotx_cm_add_service(cm_ctx,
                                                           service_pt->URI,
                                                           service_pt->type,
                                                           service_pt->auth_type,
                                                           service_pt->register_func,
                                                           service_pt->user_data,
                                                           service_pt->mail_box)) {
                        log_info("add service fail");
                    }

                    LITE_free(service_pt->URI);
                    LITE_free(service_pt);
                }
                    break;

                case IOTX_CM_PROCESS_LOCAL_REMOVE_SERVICE: {
                    char* URI = (char*)node->msg;
                    if (FAIL_RETURN == iotx_cm_remove_service(cm_ctx, URI)) {
                        log_info("unregister fail");
                    }

                    LITE_free(URI);
                }
                    break;

                case IOTX_CM_PROCESS_LOCAL_ADD_SERVICE_RESULT:
                case IOTX_CM_PROCESS_LOCAL_REMOVE_SERVICE_RESULT: {
                    iotx_cm_process_service_result_pt service_pt = (iotx_cm_process_service_result_pt)node->msg;
                    iotx_cm_local_conn_service_handler(cm_ctx, service_pt->URI, service_pt->result, service_pt->is_add);
                    LITE_free(service_pt->URI);
                    LITE_free(service_pt);
                }
                    break;

                case IOTX_CM_PROCESS_LOCAL_NEW_DATA: {
                    iotx_cm_message_info_t* message_info = (iotx_cm_message_info_t*)node->msg;
                    iotx_cm_local_conn_response_handler(cm_ctx, message_info);
                    LITE_free(node->msg);
                    break;
                }

                default:
                    break;
                }

                iotx_cm_free_list_node(cm_ctx, node);
            }
            /* local yield */
            connectivity->yield_func(connectivity, 200);
        } else {

            /* local yield */
            connectivity->yield_func(connectivity, 200);
        }
    }

    return NULL;
}
#endif /* CM_SUPPORT_MULTI_THREAD */

#endif /* CM_SUPPORT_LOCAL_CONN */

