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

#include <assert.h>

#include "iotx_cm_common.h"
#include "iot_export_event.h"
#ifdef CM_VIA_CLOUD_CONN
#include "iotx_cm_cloud_conn.h"
#endif /* CM_SUPPORT_CLOUD_CONN */
#ifdef CM_SUPPORT_LOCAL_CONN
#include "iotx_cm_local_conn.h"
#endif /* CM_SUPPORT_LOCAL_CONN */
#include "utils_httpc.h"
#include "utils_hmac.h"

#define IOTX_CM_MESSAGE_ID_MAX     (65535)
#define CM_AUTH_TIMEOUT            (10 * 1000)


#ifdef CM_MAPPING_USE_POOL
static iotx_cm_mapping_t g_cm_register_mapping_list[CM_SUPPORT_MAX_MAPPING_SIZE];
static iotx_cm_mapping_t g_cm_service_mapping_list[CM_SUPPORT_MAX_MAPPING_SIZE];
#endif

#ifdef CM_PROCESS_NODE_USE_POOL
static iotx_cm_process_list_node_t g_cm_cloud_process_node_list[CM_SUPPORT_MAX_PROCESS_NODE_SIZE];
#endif

#ifdef CM_SUPPORT_LOCAL_CONN
#ifdef CM_PROCESS_NODE_USE_POOL
static iotx_cm_process_list_node_t g_cm_local_process_node_list[CM_SUPPORT_MAX_PROCESS_NODE_SIZE];
#endif
#endif


static const char string_id[] CM_READ_ONLY = "id";
static const char string_code[] CM_READ_ONLY = "code";
static const char string_data[] CM_READ_ONLY = "data";
static const char string_message[] CM_READ_ONLY = "message";
static const char string_params[] CM_READ_ONLY = "params";
static const char string_param[] CM_READ_ONLY = "param";
static const char string_method[] CM_READ_ONLY = "method";
static const char string_response_format[] CM_READ_ONLY = "{\"id\":\"%u\",\"code\":%d,\"data\":%s}";
static const char string_request_format[] CM_READ_ONLY =
    "{\"id\":\"%u\",\"version\":\"1.0\",\"params\":%s,\"method\":\"%s\"}";


static int iotx_cm_get_next_message_id(iotx_cm_conntext_t *cm_ctx)
{
    if (cm_ctx->cm_message_id == IOTX_CM_MESSAGE_ID_MAX) {
        cm_ctx->cm_message_id = 1;
    }
    cm_ctx->cm_message_id++;
    return cm_ctx->cm_message_id;
}


#ifdef CM_SUPPORT_TOPIC_DISPATCH
int iotx_cm_parse_payload(void *payload,
                          int payload_length,
                          iotx_cm_message_info_t *msg)
{
    char *payload_pt = (char *)payload;
    char *node = NULL;

    if (NULL == payload || NULL == msg || 0 == payload_length) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    switch (msg->message_type) {
        case IOTX_CM_MESSAGE_RAW: {
            CM_INFO(cm_log_info_raw_data);
            msg->code = 0;
            msg->parameter = CM_malloc(payload_length + 1);
            if (NULL == msg->parameter) {
                CM_ERR(cm_log_error_memory);
                LITE_free(node);
                return FAIL_RETURN;
            }
            memset(msg->parameter, 0x0, payload_length);
            memcpy(msg->parameter, payload, payload_length);
            msg->parameter_length = payload_length;
            msg->method = NULL;
            msg->id = 0;
            return SUCCESS_RETURN;
        }

        case IOTX_CM_MESSAGE_RESPONSE: {
            node = LITE_json_value_of((char *)string_id, payload_pt);
            if (node == NULL) {
                CM_ERR(cm_log_error_parse_id);
                return FAIL_RETURN;
            }

            msg->id = atoi(node);
            LITE_free(node);
            node = NULL;

            /* parse code */
            node = LITE_json_value_of((char *)string_code, payload_pt);
            if (node == NULL) {
                CM_ERR(cm_log_error_parse_code);
                return FAIL_RETURN;
            }
            msg->code = atoi(node);
            LITE_free(node);
            node = NULL;

            /* parse data */
            node = LITE_json_value_of((char *)string_data, payload_pt);
            if (node == NULL) {
                CM_ERR(cm_log_error_parse_data);
                return FAIL_RETURN;
            }
            msg->parameter = node;
            msg->parameter_length = strlen(node);
            node = NULL;

            msg->method = NULL;
            return SUCCESS_RETURN;
        }

        case IOTX_CM_MESSAGE_REQUEST: {
            node = LITE_json_value_of((char *)string_id, payload_pt);
            if (node == NULL) {
                CM_ERR(cm_log_error_parse_id);
                return FAIL_RETURN;
            }

            msg->id = atoi(node);
            LITE_free(node);
            node = NULL;

            /* parse params */
            node = LITE_json_value_of((char *)string_params, payload_pt);
            if (node == NULL) {
                node = LITE_json_value_of((char *)string_param, payload_pt);
                if (node == NULL) {
                    CM_ERR(cm_log_error_parse_params);
                    return FAIL_RETURN;
                }
            }

            msg->parameter = node;
            msg->parameter_length = strlen(node);
            node = NULL;

            /* parse method */
            node = LITE_json_value_of((char *)string_method, payload_pt);
            if (node == NULL) {
                CM_ERR(cm_log_error_parse_method);
                return FAIL_RETURN;
            }
            msg->method = node;
            node = NULL;

            msg->code = 0;
            return SUCCESS_RETURN;
        }

        default:
            CM_ERR(cm_log_error_type);
            return FAIL_RETURN;
    }
    return SUCCESS_RETURN;
}
#else
int iotx_cm_parse_payload(void *_payload, int payload_length, iotx_cm_message_info_t *msg)
{
    char *payload = (char *)_payload;
    char *node = NULL;

    if (NULL == _payload || NULL == msg || 0 == payload_length) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    node = LITE_json_value_of((char *)string_id, payload);
    if (node == NULL) {
        msg->message_type = IOTX_CM_MESSAGE_RAW;
        msg->code = 0;
        msg->parameter = CM_malloc(payload_length + 1);
        if (NULL == msg->parameter) {
            CM_ERR(cm_log_error_memory);
            return FAIL_RETURN;
        }
        memset(msg->parameter, 0x0, payload_length);
        memcpy(msg->parameter, _payload, payload_length);
        msg->parameter_length = payload_length;
        msg->method = NULL;
        msg->id = 0;
        return SUCCESS_RETURN;
    }

    msg->id = atoi(node);
    LITE_free(node);
    node = NULL;

    /* parse code */
    node = LITE_json_value_of((char *)string_code, payload);
    if (node == NULL) {
        msg->message_type = IOTX_CM_MESSAGE_REQUEST;

        /* parse params */
        node = LITE_json_value_of((char *)string_params, payload);
        if (node == NULL) {
            node = LITE_json_value_of((char *)string_param, payload);
            if (node == NULL) {
                CM_ERR(cm_log_error_parse_params);
                return FAIL_RETURN;
            }
        }

        msg->parameter = node;
        msg->parameter_length = strlen(node);
        node = NULL;

        /* parse method */
        node = LITE_json_value_of((char *)string_method, payload);
        if (node == NULL) {
            CM_ERR(cm_log_error_parse_method);
            return FAIL_RETURN;
        }
        msg->method  = node;
        node = NULL;

        msg->code = 0;
        return SUCCESS_RETURN;
    }
    msg->code = atoi(node);
    LITE_free(node);
    node = NULL;

    /* parse data */
    node = LITE_json_value_of((char *)string_data, payload);
    if (node == NULL) {
        CM_ERR(cm_log_error_parse_data);
        return FAIL_RETURN;
    }
    msg->parameter = node;
    msg->parameter_length = strlen(node);
    node = NULL;

    msg->method = NULL;
    msg->message_type = IOTX_CM_MESSAGE_RESPONSE;

    /* parse message */
    node = LITE_json_value_of((char *)string_message, payload);
    if (node) {
        msg->message = node;
    }

    return SUCCESS_RETURN;
}
#endif


int iotx_cm_splice_payload(void *payload, int *payload_length, int id, iotx_cm_message_info_t *msg)
{
    if (NULL == payload || NULL == payload_length || NULL == msg) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    switch (msg->message_type) {
        case IOTX_CM_MESSAGE_RESPONSE:
            HAL_Snprintf(payload, *payload_length, string_response_format, id, msg->code, msg->parameter);
            (*payload_length) = strlen(payload);
            break;
        case IOTX_CM_MESSAGE_REQUEST:
            HAL_Snprintf(payload, *payload_length, string_request_format, id, msg->parameter, msg->method);
            (*payload_length) = strlen(payload);
            break;
        case IOTX_CM_MESSAGE_RAW:
            memcpy(payload, msg->parameter, msg->parameter_length);
            (*payload_length) = msg->parameter_length;
            break;
        default:
            break;
    }

    return SUCCESS_RETURN;
}


void iotx_cm_free_message_info(iotx_cm_message_info_t *message_info)
{
    if (message_info->URI) {
        LITE_free(message_info->URI);
    }
    if (message_info->parameter) {
        LITE_free(message_info->parameter);
    }
    if (message_info->method) {
        LITE_free(message_info->method);
    }
    if (message_info->message) {
        LITE_free(message_info->message);
    }
}


void iotx_cm_response_func(void *context, iotx_cm_mapping_type_t mapping_type, iotx_cm_message_info_t *message_info)
{
    iotx_cm_send_peer_t peer;
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)context;
#ifdef CM_SUPPORT_TOPIC_DISPATCH
    iotx_cm_mapping_t *mapping = NULL;
#else
    iotx_cm_event_msg_t msg_event = {0};
    iotx_cm_new_data_t new_data = {0};
#endif
    iotx_device_info_t *device_info = iotx_device_info_get();

    if (NULL == cm_ctx || NULL == message_info) {
        CM_ERR(cm_log_error_parameter);
        return;
    }

    memset(&peer, 0x0, sizeof(iotx_cm_send_peer_t));
    strncpy(peer.product_key, device_info->product_key, strlen(device_info->product_key));
    strncpy(peer.device_name, device_info->device_name, strlen(device_info->device_name));


#ifdef CM_SUPPORT_TOPIC_DISPATCH
    /* find mapping */
    mapping = iotx_cm_find_mapping(cm_ctx, mapping_type, message_info->URI, strlen(message_info->URI));

    if (NULL == mapping) {
        CM_WARNING(cm_log_warning_not_mapping);
        iotx_cm_free_message_info(message_info);
        return;
    }

    if (NULL == mapping->func && NULL == mapping->mail_box) {
        CM_WARNING(cm_log_warning_not_func);
        iotx_cm_free_message_info(message_info);
        return;
    }
    if (mapping->func) {
        mapping->func(&peer, message_info, mapping->user_data);
    }
    iotx_cm_free_message_info(message_info);
#else
    new_data.message_info = message_info;
    new_data.peer = &peer;
    msg_event.event_id = IOTX_CM_EVENT_NEW_DATA_RECEIVED;
    msg_event.msg = &new_data;

    iotx_cm_trigger_event_callback(cm_ctx, &msg_event);

    iotx_cm_free_message_info(message_info);
#endif
}


static void cm_find_connectivity_handler(void *list_node, va_list *params)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)list_node;
    iotx_cm_conntext_t *cm_ctx;
    iotx_cm_send_peer_t *target;
    iotx_cm_send_peer_t *connectivity_target;
    void *conn_ctx;

    cm_ctx = va_arg(*params, iotx_cm_conntext_t *);
    target = va_arg(*params, void *);
    conn_ctx = va_arg(*params, void *);

    assert(cm_ctx);

    /* TODO */
    if (conn_ctx && IOTX_CM_CONNECTIVITY_TYPE_LOCAL == connectivity->type) {
        cm_ctx->target_connectivity = connectivity;
        return;
    }


    if (connectivity && target && connectivity->get_target_func &&
        (connectivity_target = connectivity->get_target_func())) {
        if (0 == strcmp(target->product_key, connectivity_target->product_key) &&
            0 == strcmp(target->device_name, connectivity_target->device_name)) {
            cm_ctx->target_connectivity = connectivity;
        }
    }
}


iotx_cm_connectivity_t *iotx_cm_find_connectivity(iotx_cm_conntext_t *cm_ctx, iotx_cm_send_peer_t *target,
                                                  void *conn_ctx)
{
    linked_list_t *list = NULL;

    assert(cm_ctx);
    list = cm_ctx->list_connectivity;

    assert(list);

    cm_ctx->target_connectivity = NULL;
    linked_list_iterator(list, cm_find_connectivity_handler, cm_ctx, target, conn_ctx);

    return cm_ctx->target_connectivity;
}


int iotx_cm_add_connectivity(iotx_cm_conntext_t *cm_ctx, iotx_cm_connectivity_t *connectivity)
{
    linked_list_t *list = NULL;

    assert(cm_ctx && connectivity);
    list = cm_ctx->list_connectivity;

    assert(list);
    linked_list_insert(list, connectivity);

    return SUCCESS_RETURN;
}


#ifndef CM_SUPPORT_MULTI_THREAD
static void cm_trigger_connected_handler(void *list_node, va_list *params)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)list_node;
    iotx_cm_conntext_t *cm_ctx;

    cm_ctx = va_arg(*params, iotx_cm_conntext_t *);

    assert(cm_ctx);

    connectivity->trigger_connected_func(cm_ctx, connectivity, NULL, NULL);
}
#endif


int iotx_cm_add_connectivity_all(iotx_cm_conntext_t *cm_ctx, iotx_cm_init_param_t *init_param)
{
    iotx_cm_connectivity_t *connectivity = NULL;

    /* cloud connection */
    connectivity = iotx_cm_cloud_conn_init(cm_ctx, init_param);
    if (NULL == connectivity) {
        return FAIL_RETURN;
    }

    if (FAIL_RETURN == iotx_cm_add_connectivity(cm_ctx, connectivity)) {
        return FAIL_RETURN;
    }

#ifndef CM_SUPPORT_MULTI_THREAD
    connectivity->connect_func(cm_ctx, connectivity);
#endif

    /* local connection */
#ifdef CM_SUPPORT_LOCAL_CONN
    connectivity = iotx_cm_local_conn_init(cm_ctx, init_param);
    if (NULL == connectivity) {
        return FAIL_RETURN;
    }

    if (FAIL_RETURN == iotx_cm_add_connectivity(cm_ctx, connectivity)) {
        return FAIL_RETURN;
    }

#ifndef CM_SUPPORT_MULTI_THREAD
    connectivity->connect_func(cm_ctx, connectivity);
#endif
#endif

#ifndef CM_SUPPORT_MULTI_THREAD
    linked_list_iterator(cm_ctx->list_connectivity, cm_trigger_connected_handler, cm_ctx);
#endif

    return SUCCESS_RETURN;
}


int iotx_cm_get_connectivity_id()
{
    static int _connectivity_id = 0;
    _connectivity_id++;
    return _connectivity_id;
}


iotx_cm_connectivity_types_t iotx_cm_get_connectivity_type(iotx_cm_connectivity_t *connectivity)
{
    return connectivity ? connectivity->type : IOTX_CM_CONNECTIVITY_TYPE_MAX;
}


static void cm_remove_connectivity_handler(void *list_node, va_list *params)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)list_node;

    if (connectivity && connectivity->deinit_func) {
        if (FAIL_RETURN == connectivity->deinit_func(connectivity)) {
            CM_ERR(cm_log_error_fail);
        }
        LITE_free(connectivity);
    }
}


int iotx_cm_remove_connectivity_all(iotx_cm_conntext_t *cm_ctx)
{
    linked_list_t *list;

    assert(cm_ctx);
    list = cm_ctx->list_connectivity;

    assert(list);

    linked_list_iterator(list, cm_remove_connectivity_handler, cm_ctx);

    return SUCCESS_RETURN;
}


iotx_cm_mapping_t *iotx_cm_get_mapping_node(iotx_cm_mapping_type_t mapping_type)
{
#ifndef CM_SUPPORT_TOPIC_DISPATCH
    return NULL;

#else /* CM_SUPPORT_TOPIC_DISPATCH */

#ifdef CM_MAPPING_USE_POOL
    int i = 0;
    iotx_cm_mapping_t *mapping_list = NULL;

    if (IOTX_CM_MAPPING_TYPE_REGISTER == mapping_type) {
        mapping_list = g_cm_register_mapping_list;
    } else if (IOTX_CM_MAPPING_TYPE_SERVICE == mapping_type) {
        mapping_list = g_cm_service_mapping_list;
    } else {
        return NULL;
    }

    for (i = 0; i < CM_SUPPORT_MAX_MAPPING_SIZE; i++) {
        if (mapping_list[i].is_used == 0) {
            mapping_list[i].is_used = 1;
            return &mapping_list[i];
        }
    }
    return NULL;
#else
    iotx_cm_mapping_t *node = NULL;

    node = CM_malloc(sizeof(iotx_cm_mapping_t));
    if (NULL == node) {
        return NULL;
    }

    memset(node, 0x0, sizeof(iotx_cm_mapping_t));
    node->mapping_type = mapping_type;
    return node;
#endif

#endif /* CM_SUPPORT_TOPIC_DISPATCH */
}

int iotx_cm_free_mapping_node(iotx_cm_mapping_type_t mapping_type, iotx_cm_mapping_t *node)
{
#ifndef CM_SUPPORT_TOPIC_DISPATCH
    return SUCCESS_RETURN;
#else /* CM_SUPPORT_TOPIC_DISPATCH */
    if (NULL == node) {
        return FAIL_RETURN;
    }

#ifdef CM_MAPPING_USE_POOL
    if (node->is_used == 1) {
        node->is_used = 0;
        memset(node, 0x0, sizeof(iotx_cm_mapping_t));
        return SUCCESS_RETURN;
    }

    return FAIL_RETURN;
#else /* CM_MAPPING_USE_POOL */
    LITE_free(node);
    return SUCCESS_RETURN;
#endif /* CM_MAPPING_USE_POOL */

#endif /* CM_SUPPORT_TOPIC_DISPATCH */
}


int iotx_cm_add_mapping(iotx_cm_conntext_t *cm_ctx, iotx_cm_mapping_type_t mapping_type, char *URI,
                        iotx_cm_message_types_t type,
                        iotx_cm_register_fp_t func, void *user_data, void *mail_box)
{
#ifndef CM_SUPPORT_TOPIC_DISPATCH
    return SUCCESS_RETURN;
#else /* CM_SUPPORT_TOPIC_DISPATCH */
    iotx_cm_mapping_t *mapping = NULL;

    if (NULL == cm_ctx || NULL == URI) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    mapping = iotx_cm_get_mapping_node(mapping_type);
    if (NULL == mapping) {
        return FAIL_RETURN;
    }

    mapping->URI = CM_malloc(strlen(URI) + 1);
    if (NULL == mapping->URI) {
        CM_ERR(cm_log_error_memory);
        iotx_cm_free_mapping_node(mapping_type, mapping);
        return FAIL_RETURN;
    }
    memset(mapping->URI, 0x0, strlen(URI) + 1);
    strncpy(mapping->URI, URI, strlen(URI));

    mapping->type = type;
    mapping->func = func;
    mapping->user_data = user_data;
    mapping->mail_box = mail_box;

    if (IOTX_CM_MAPPING_TYPE_REGISTER == mapping_type) {
        mapping->next = cm_ctx->register_mapping_list;
        cm_ctx->register_mapping_list = mapping;
    } else if (IOTX_CM_MAPPING_TYPE_SERVICE == mapping_type) {
        mapping->next = cm_ctx->service_mapping_list;
        cm_ctx->service_mapping_list = mapping;
    } else {
        LITE_free(mapping->URI);
        iotx_cm_free_mapping_node(mapping_type, mapping);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
#endif /* CM_SUPPORT_TOPIC_DISPATCH */
}


/* remove */
int iotx_cm_remove_mapping(iotx_cm_conntext_t *cm_ctx, iotx_cm_mapping_type_t mapping_type, char *URI)
{
#ifndef CM_SUPPORT_TOPIC_DISPATCH
    return SUCCESS_RETURN;
#else /* CM_SUPPORT_TOPIC_DISPATCH */
    iotx_cm_mapping_t *mapping = NULL;
    iotx_cm_mapping_t *pre_mapping = NULL;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    if (IOTX_CM_MAPPING_TYPE_REGISTER == mapping_type) {
        pre_mapping = mapping = cm_ctx->register_mapping_list;
    } else if (IOTX_CM_MAPPING_TYPE_SERVICE == mapping_type) {
        pre_mapping = mapping = cm_ctx->service_mapping_list;
    } else {
        return FAIL_RETURN;
    }

    if (NULL == mapping) {
        return FAIL_RETURN;
    }

    if (0 == strncmp(URI, mapping->URI, strlen(URI))) {
        if (NULL == mapping->next) {
            if (IOTX_CM_MAPPING_TYPE_REGISTER == mapping_type) {
                cm_ctx->register_mapping_list = NULL;
            } else if (IOTX_CM_MAPPING_TYPE_SERVICE == mapping_type) {
                cm_ctx->service_mapping_list = NULL;
            } else {
                return FAIL_RETURN;
            }
        } else {
            if (IOTX_CM_MAPPING_TYPE_REGISTER == mapping_type) {
                cm_ctx->register_mapping_list = mapping->next;
            } else if (IOTX_CM_MAPPING_TYPE_SERVICE == mapping_type) {
                cm_ctx->service_mapping_list = mapping->next;
            } else {
                return FAIL_RETURN;
            }
        }
    }

    while (mapping) {
        if (0 == strncmp(URI, mapping->URI, strlen(URI))) {
            CM_INFO(cm_log_info_remove_mapping);
            pre_mapping->next = mapping->next;

            LITE_free(mapping->URI);
            iotx_cm_free_mapping_node(mapping_type, mapping);
            return SUCCESS_RETURN;
        }

        pre_mapping = mapping;
        mapping = mapping->next;
    }

    return FAIL_RETURN;
#endif /* CM_SUPPORT_TOPIC_DISPATCH */
}


/* remove all */
int iotx_cm_remove_mapping_all(iotx_cm_conntext_t *cm_ctx)
{
#ifndef CM_SUPPORT_TOPIC_DISPATCH
    return SUCCESS_RETURN;
#else /* CM_SUPPORT_TOPIC_DISPATCH */
    iotx_cm_mapping_t *mapping = NULL;
    iotx_cm_mapping_t *next_mapping = NULL;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    next_mapping = mapping = cm_ctx->register_mapping_list;

    while (mapping) {
        next_mapping = mapping->next;

        LITE_free(mapping->URI);
        iotx_cm_free_mapping_node(IOTX_CM_MAPPING_TYPE_REGISTER, mapping);

        mapping = next_mapping;
    }
    cm_ctx->register_mapping_list = NULL;

    next_mapping = mapping = cm_ctx->service_mapping_list;

    while (mapping) {
        next_mapping = mapping->next;

        LITE_free(mapping->URI);
        iotx_cm_free_mapping_node(IOTX_CM_MAPPING_TYPE_SERVICE, mapping);

        mapping = next_mapping;
    }

    cm_ctx->service_mapping_list = NULL;

    return FAIL_RETURN;
#endif /* CM_SUPPORT_TOPIC_DISPATCH */
}

iotx_cm_mapping_t *iotx_cm_find_mapping(iotx_cm_conntext_t *cm_ctx, iotx_cm_mapping_type_t mapping_type, char *URI,
                                        int URI_length)
{
#ifndef CM_SUPPORT_TOPIC_DISPATCH
    return NULL;
#else /* CM_SUPPORT_TOPIC_DISPATCH */
    iotx_cm_mapping_t *mapping = NULL;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_parameter);
        return NULL;
    }

    if (IOTX_CM_MAPPING_TYPE_REGISTER == mapping_type) {
        mapping = cm_ctx->register_mapping_list;
    } else if (IOTX_CM_MAPPING_TYPE_SERVICE == mapping_type) {
        mapping = cm_ctx->service_mapping_list;
    } else {
        return NULL;
    }


    while (mapping) {
        if ((0 == strncmp(URI, mapping->URI, URI_length)) || (0 != strstr(mapping->URI, URI))) {
            return mapping;
        }

        mapping = mapping->next;
    }

    return NULL;
#endif /* CM_SUPPORT_TOPIC_DISPATCH */
}

static void cm_connectivity_iterator_action_handler(void *list_node, va_list *params)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)list_node;
    iotx_cm_conntext_t *cm_ctx;
    char *uri;
    const char *pk;
    const char *dn;
    iotx_cm_message_auth_types_t auth_type;
    cm_iterator_action_t action;

    cm_ctx = va_arg(*params, iotx_cm_conntext_t *);
    action = va_arg(*params, int);

    assert(cm_ctx && action < cm_iterator_action_max);

    if (connectivity && connectivity->is_connected) {
        switch (action) {
            case cm_iterator_action_register:
                uri = va_arg(*params, char *);
                connectivity->register_func(cm_ctx, connectivity, uri);
                break;

            case cm_iterator_action_unregister:
                uri = va_arg(*params, char *);
                connectivity->unregister_func(cm_ctx, connectivity, uri);
                break;

            case cm_iterator_action_add_service:
                uri = va_arg(*params, char *);
                auth_type = va_arg(*params, int);
                connectivity->add_service_func(cm_ctx, connectivity, uri, auth_type);
                break;

            case cm_iterator_action_remove_service:
                uri = va_arg(*params, char *);
                connectivity->remove_service_func(cm_ctx, connectivity, uri);
                break;

            case cm_iterator_action_add_subdevice:
                pk = va_arg(*params, char *);
                dn = va_arg(*params, char *);
                connectivity->add_subdevice_func(cm_ctx, connectivity, pk, dn);
                break;

            case cm_iterator_action_remove_subdevice:
                pk = va_arg(*params, char *);
                dn = va_arg(*params, char *);
                connectivity->remove_subdevice_func(cm_ctx, connectivity, pk, dn);
                break;

            default:
                break;
        }
    }
}

int iotx_cm_register(iotx_cm_conntext_t *cm_ctx, char *_URI, iotx_cm_message_types_t type,
                     iotx_cm_register_fp_t register_func, void *user_data, void *mail_box)
{
    linked_list_t *list = NULL;

    char *URI = _URI;

#ifdef CM_SUPPORT_TOPIC_DISPATCH
    iotx_cm_mapping_t *mapping = NULL;
#endif

    assert(cm_ctx && URI);

#ifdef CM_SUPPORT_TOPIC_DISPATCH
    if (FAIL_RETURN == iotx_cm_add_mapping(cm_ctx, IOTX_CM_MAPPING_TYPE_REGISTER, URI, type, register_func, user_data,
                                           mail_box)) {
        iotx_cm_event_msg_t event;
        iotx_cm_event_result_t result_pt = {0};

        result_pt.result = -1;
        result_pt.URI = URI;
        event.event_id = IOTX_CM_EVENT_REGISTER_RESULT;
        event.msg = (void *)&result_pt;

        CM_ERR(cm_log_error_fail_1);

        iotx_cm_trigger_event_callback(cm_ctx, &event);

        return FAIL_RETURN;
    }

    mapping = iotx_cm_find_mapping(cm_ctx, IOTX_CM_MAPPING_TYPE_REGISTER, URI, strlen(URI));

    list = cm_ctx->list_connectivity;

    linked_list_iterator(list, cm_connectivity_iterator_action_handler, cm_ctx, cm_iterator_action_register, mapping->URI);
#else /* CM_SUPPORT_TOPIC_DISPATCH */

    list = cm_ctx->list_connectivity;

    linked_list_iterator(list, cm_connectivity_iterator_action_handler, cm_ctx, cm_iterator_action_register, URI);
#endif /* CM_SUPPORT_TOPIC_DISPATCH */

    return SUCCESS_RETURN;
}


int iotx_cm_unregister(iotx_cm_conntext_t *cm_ctx, char *URI)
{
    linked_list_t *list = NULL;

    assert(cm_ctx && URI);

    list = cm_ctx->list_connectivity;

    linked_list_iterator(list, cm_connectivity_iterator_action_handler, cm_ctx, cm_iterator_action_unregister, URI);

    if (FAIL_RETURN == iotx_cm_remove_mapping(cm_ctx, IOTX_CM_MAPPING_TYPE_REGISTER, URI)) {
        CM_ERR(cm_log_error_fail);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}


int iotx_cm_add_service(iotx_cm_conntext_t *cm_ctx, char *_URI, iotx_cm_message_types_t type,
                        iotx_cm_message_auth_types_t auth_type,
                        iotx_cm_register_fp_t register_func, void *user_data, void *mail_box)
{
    linked_list_t *list = NULL;
    char *URI = _URI;
#ifdef CM_SUPPORT_TOPIC_DISPATCH
    iotx_cm_mapping_t *mapping = NULL;
#endif /* CM_SUPPORT_TOPIC_DISPATCH */

    assert(cm_ctx && URI);

#ifdef CM_SUPPORT_TOPIC_DISPATCH
    if (FAIL_RETURN == iotx_cm_add_mapping(cm_ctx, IOTX_CM_MAPPING_TYPE_SERVICE, URI, type, register_func, user_data,
                                           mail_box)) {
        iotx_cm_event_msg_t event;
        iotx_cm_event_result_t result_pt = {0};

        result_pt.result = -1;
        result_pt.URI = URI;
        event.event_id = IOTX_CM_EVENT_ADD_SERVICE_RESULT;
        event.msg = (void *)&result_pt;

        CM_ERR(cm_log_error_fail_1);

        iotx_cm_trigger_event_callback(cm_ctx, &event);

        return FAIL_RETURN;
    }

    mapping = iotx_cm_find_mapping(cm_ctx, IOTX_CM_MAPPING_TYPE_SERVICE, URI, strlen(URI));

    list = cm_ctx->list_connectivity;

    linked_list_iterator(list, cm_connectivity_iterator_action_handler, cm_ctx, cm_iterator_action_add_service,
                         mapping->URI, auth_type);
#else /* CM_SUPPORT_TOPIC_DISPATCH */

    list = cm_ctx->list_connectivity;

    linked_list_iterator(list, cm_connectivity_iterator_action_handler, cm_ctx, cm_iterator_action_add_service, URI,
                         auth_type);
#endif /* CM_SUPPORT_TOPIC_DISPATCH */

    return SUCCESS_RETURN;
}


int iotx_cm_remove_service(iotx_cm_conntext_t *cm_ctx, char *URI)
{
    linked_list_t *list;

    assert(cm_ctx && URI);

    list = cm_ctx->list_connectivity;

    linked_list_iterator(list, cm_connectivity_iterator_action_handler, cm_ctx, cm_iterator_action_remove_service, URI);

    if (FAIL_RETURN == iotx_cm_remove_mapping(cm_ctx, IOTX_CM_MAPPING_TYPE_SERVICE, URI)) {
        CM_ERR(cm_log_error_fail);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}


int iotx_cm_add_subdevice(iotx_cm_conntext_t *cm_ctx, const char *pk, const char *dn)
{
    linked_list_t *list = NULL;

    assert(cm_ctx && pk && dn);

    list = cm_ctx->list_connectivity;

    linked_list_iterator(list, cm_connectivity_iterator_action_handler, cm_ctx, cm_iterator_action_add_subdevice, pk, dn);

    return SUCCESS_RETURN;
}


int iotx_cm_remove_subdevice(iotx_cm_conntext_t *cm_ctx, const char *pk, const char *dn)
{
    linked_list_t *list = NULL;

    assert(cm_ctx && pk && dn);

    list = cm_ctx->list_connectivity;

    linked_list_iterator(list, cm_connectivity_iterator_action_handler, cm_ctx, cm_iterator_action_remove_subdevice, pk,
                         dn);

    return SUCCESS_RETURN;
}


int iotx_cm_parse_message(iotx_cm_conntext_t *cm_ctx, iotx_cm_message_info_t *message_info,
                          char **URI, void **payload, int *payload_length)
{
    (*URI) = CM_malloc(strlen(message_info->URI) + 1);
    memset((*URI), 0X0, strlen(message_info->URI) + 1);
    strcpy((*URI), message_info->URI);

    (*payload_length) = message_info->parameter_length + 10 + strlen(string_request_format);

    if (message_info->method) {
        (*payload_length) += strlen(message_info->method);
    }

    CM_INFO(cm_log_info_payload_length, (*payload_length));

    (*payload) = CM_malloc((*payload_length));
    if (NULL == (*payload)) {
        CM_ERR(cm_log_error_memory);
        return FAIL_RETURN;
    }

    if (0 == message_info->id) {
        message_info->id = iotx_cm_get_next_message_id(cm_ctx);
    }

    if (FAIL_RETURN == iotx_cm_splice_payload((*payload), payload_length, message_info->id, message_info)) {
        CM_ERR(cm_log_error_parse_payload);
        LITE_free((*payload));
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

static void cm_connectivity_iterator_send_data_handler(void *list_node, va_list *params)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)list_node;
    iotx_cm_connectivity_t *target_connectivity;
    iotx_cm_send_peer_t *target;
    iotx_cm_conntext_t *cm_ctx;
    iotx_cm_message_ack_types_t ack_type;
    char *uri;
    void *payload;
    int payload_length;
    void *context;
    int *rc;

    cm_ctx = va_arg(*params, iotx_cm_conntext_t *);
    target_connectivity = va_arg(*params, void *);
    target = va_arg(*params, void *);
    uri = va_arg(*params, char *);
    ack_type = va_arg(*params, int);
    payload = va_arg(*params, void *);
    payload_length = va_arg(*params, int);
    context = va_arg(*params, void *);
    rc = va_arg(*params, int *);

    assert(cm_ctx && uri && payload && payload_length && rc);

    if ((target_connectivity && connectivity == target_connectivity && connectivity->is_connected) ||
        (!target_connectivity && connectivity && connectivity->is_connected)) {
        if (FAIL_RETURN == connectivity->send_func(cm_ctx, connectivity, target, uri, ack_type, payload, payload_length,
                                                   context)) {
            *rc = FAIL_RETURN;
        }
    }
}

int iotx_cm_send_data(iotx_cm_conntext_t *cm_ctx, iotx_cm_send_peer_t *target, void *_connectivity, char *URI,
                      iotx_cm_message_ack_types_t ack_type, void *payload, int payload_length, void *context)
{
    linked_list_t *list;
    int rc = 0;
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;

    assert(cm_ctx && URI && payload && payload_length);

    list = cm_ctx->list_connectivity;

    linked_list_iterator(list, cm_connectivity_iterator_send_data_handler, cm_ctx, connectivity,
                         target, URI, ack_type, payload, payload_length, context, &rc);

    if (SUCCESS_RETURN != rc) {
        iotx_cm_event_msg_t event_msg;
        iotx_cm_event_result_t event_result = {0};

        event_result.result = -1;
        event_result.URI = URI;
        event_msg.event_id = IOTX_CM_EVENT_SEND_RESULT;
        event_msg.msg = &event_result;

        CM_ERR(cm_log_error_fail_rc, rc);

        /* todo: send fail is not a event */
        iotx_cm_trigger_event_callback(cm_ctx, &event_msg);
    }
    LITE_free(URI);
    LITE_free(payload);
    return rc;
}

extern int awss_report_cloud();

static void invoke_event_callback_func(void *_cb_usr_ctx, va_list *params)
{
    static char report_token = 0;
    iotx_cm_event_cb_usr_ctx_t *cb_usr_ctx = _cb_usr_ctx;
    iotx_cm_conntext_t *cm_ctx;
    iotx_cm_event_msg_t *msg;

    cm_ctx = va_arg(*params, void *);
    msg = va_arg(*params, void *);

    assert(cm_ctx && cb_usr_ctx && msg);

    if (report_token == 0) {
        iotx_event_post(IOTX_CONN_CLOUD_SUC);
    }

    if (cb_usr_ctx && cm_ctx &&  cb_usr_ctx->event_func) {
        if (IOTX_CM_EVENT_CLOUD_CONNECTED == msg->event_id && report_token == 0) {
            awss_report_cloud();
            report_token = 1;
        }
        cb_usr_ctx->event_func(cm_ctx, msg, cb_usr_ctx->user_data);
    }
}

static void callback_list_iterator(void *_cm_ctx, linked_list_handle_fp_t handle_fp, void *msg)
{
    iotx_cm_conntext_t *cm_ctx = _cm_ctx;
    linked_list_t *list = cm_ctx->list_event_callback;

    linked_list_iterator(list, handle_fp, cm_ctx, msg);
}

int iotx_cm_trigger_event_callback(iotx_cm_conntext_t *_cm_ctx, iotx_cm_event_msg_t *msg)
{
    iotx_cm_conntext_t *cm_ctx = _cm_ctx;
    linked_list_t *list;

    /* invoke callback funtions. */
    list = cm_ctx->list_event_callback;

    if (list && !linked_list_empty(list)) {
        callback_list_iterator(cm_ctx, invoke_event_callback_func, msg);
    }

    return SUCCESS_RETURN;
}


#ifdef CM_SUPPORT_MULTI_THREAD

iotx_cm_process_list_node_t *iotx_cm_get_list_node(iotx_cm_conntext_t *cm_ctx, iotx_cm_connectivity_types_t type)
{
#ifdef CM_PROCESS_NODE_USE_POOL
    int i = 0;
    iotx_cm_process_list_node_t *list = NULL;

    if (IOTX_CM_CONNECTIVITY_TYPE_CLOUD == type) {
        list = g_cm_cloud_process_node_list;
    }
#ifdef CM_SUPPORT_LOCAL_CONN
    else if (IOTX_CM_CONNECTIVITY_TYPE_LOCAL == type) {
        list = g_cm_local_process_node_list;
    }
#endif
    else {
        return NULL;
    }

    for (i = 0; i < CM_SUPPORT_MAX_PROCESS_NODE_SIZE; i++) {
        if (list[i].is_used == 0) {
            list[i].is_used = 1;
            return &list[i];
        }
    }
    return NULL;
#else
    iotx_cm_process_list_node_t *node = NULL;
    node = CM_malloc(sizeof(iotx_cm_process_list_node_t));
    if (NULL == node) {
        return NULL;
    }

    memset(node, 0x0, sizeof(iotx_cm_process_list_node_t));
    return node;
#endif
}


int iotx_cm_free_list_node(iotx_cm_conntext_t *cm_ctx, iotx_cm_process_list_node_t *node)
{
    if (NULL == node) {
        return FAIL_RETURN;
    }

#ifdef CM_PROCESS_NODE_USE_POOL
    if (node->is_used == 1) {
        node->is_used = 0;
        memset(node, 0x0, sizeof(iotx_cm_process_list_node_t));
        return SUCCESS_RETURN;
    }

    return FAIL_RETURN;
#else
    LITE_free(node);
    return SUCCESS_RETURN;
#endif
}


static void iotx_cm_free_process_list_handler(void *list_node, va_list *params)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)list_node;
    iotx_cm_conntext_t *cm_ctx;
    iotx_cm_process_list_t *node_list = connectivity->process_list;

    if (NULL == node_list) {
        return;
    }

    iotx_cm_process_list_node_t *node = node_list->header;

    cm_ctx = va_arg(*params, iotx_cm_conntext_t *);

    assert(cm_ctx);

    while (node) {
        iotx_cm_process_list_node_t *next_node = node;
        switch (node->type) {
            case IOTX_CM_PROCESS_REGISTER: {
                iotx_cm_process_register_t *_register = (iotx_cm_process_register_t *)node->msg;
                LITE_free(_register->URI);
                LITE_free(_register);
            }
            break;

            case IOTX_CM_PROCESS_UNREGISTER: {
                LITE_free(node->msg);
            }
            break;

            case IOTX_CM_PROCESS_ADD_SERVICE: {
                iotx_cm_process_service_t *_service = (iotx_cm_process_service_t *)node->msg;
                LITE_free(_service->URI);
                LITE_free(_service);
            }
            break;

            case IOTX_CM_PROCESS_REMOVE_SERVICE: {
                LITE_free(node->msg);
            }
            break;

            case IOTX_CM_PROCESS_ADD_SUBDIVCE: {
                LITE_free(node->msg);
            }
            break;

            case IOTX_CM_PROCESS_REMOVE_SUBDIVCE: {
                LITE_free(node->msg);
            }
            break;

            case IOTX_CM_PROCESS_CLOUD_SEND: {
                iotx_cm_process_send_t *send = (iotx_cm_process_send_t *)node->msg;
                if (send->URI) {
                    LITE_free(send->URI);
                }
                if (send->payload) {
                    LITE_free(send->payload);
                }
                if (send->target) {
                    LITE_free(send->target);
                }
                LITE_free(send);
            }
            break;

            case IOTX_CM_PROCESS_REGISTER_RESULT:
            case IOTX_CM_PROCESS_UNREGISTER_RESULT: {
                iotx_cm_process_register_result_t *_register = (iotx_cm_process_register_result_t *)node->msg;
                LITE_free(_register);
            }
            break;

            case IOTX_CM_PROCESS_CLOUD_NEW_DATA: {
                iotx_cm_message_info_t *message_info = (iotx_cm_message_info_t *)node->msg;
                iotx_cm_free_message_info(message_info);
                LITE_free(message_info);
                break;
            }

            case IOTX_CM_PROCESS_ADD_DEVICE: {
            }
            break;

            case IOTX_CM_PROCESS_REMOVE_DEVICE: {
            }
            break;

            case IOTX_CM_PROCESS_ADD_SERVICE_RESULT:
            case IOTX_CM_PROCESS_REMOVE_SERVICE_RESULT: {
            }
            break;

            case IOTX_CM_PROCESS_LOCAL_NEW_DATA: {
            }
            break;

            case IOTX_CM_PROCESS_LOCAL_SEND: {
                break;
            }

            default:
                break;
        }
        next_node = node->next;

#ifdef CM_PROCESS_NODE_USE_POOL
        if (node->is_used == 1) {
            node->is_used = 0;
            memset(node, 0x0, sizeof(iotx_cm_process_list_node_t));
        }
#else
        LITE_free(node);
#endif
        node = next_node;
    }
}


int iotx_cm_free_list_node_all(iotx_cm_conntext_t *cm_ctx)
{
    assert(cm_ctx);

    if (NULL == cm_ctx->list_connectivity) {
        return FAIL_RETURN;
    }

    linked_list_iterator(cm_ctx->list_connectivity, iotx_cm_free_process_list_handler, cm_ctx);
    return SUCCESS_RETURN;
}


/* node is in */
int iotx_cm_process_list_push(iotx_cm_conntext_t *cm_ctx,
                              iotx_cm_connectivity_types_t type,
                              iotx_cm_process_list_node_t *node)
{
    iotx_cm_process_list_t *list = NULL;
    iotx_cm_connectivity_t *connectivity = NULL;

    if (NULL == cm_ctx || NULL == node) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    if (IOTX_CM_CONNECTIVITY_TYPE_CLOUD == type) {
        connectivity = iotx_cm_find_connectivity(cm_ctx, iotx_cm_cloud_conn_get_target(), NULL);
    }
#ifdef CM_SUPPORT_LOCAL_CONN
    else if (IOTX_CM_CONNECTIVITY_TYPE_LOCAL == type) {
        connectivity = iotx_cm_find_connectivity(cm_ctx, iotx_cm_local_conn_get_target(), NULL);
    }
#endif
    else {
        return FAIL_RETURN;
    }

    list = connectivity->process_list;

    HAL_MutexLock(connectivity->process_lock);

    if (NULL == list) {
        list = CM_malloc(sizeof(iotx_cm_process_list_t));

        if (NULL == list) {
            HAL_MutexUnlock(connectivity->process_lock);
            CM_ERR(cm_log_error_memory);
            return FAIL_RETURN;
        }

        list->header = list->tailer = NULL;
        list->size = 0;

        connectivity->process_list = list;
    }

    if (list->size == 0) {
        list->header = list->tailer = node;
        list->size = 1;
    } else if (list->size == CM_SUPPORT_MAX_PROCESS_NODE_SIZE) {
        HAL_MutexUnlock(connectivity->process_lock);
        CM_ERR(cm_log_error_process);
        return FAIL_RETURN;
    } else {
        node->pre = list->tailer;
        list->tailer->next = node;
        list->tailer = node;
        list->size++;
    }

    HAL_MutexUnlock(connectivity->process_lock);

    return SUCCESS_RETURN;
}


iotx_cm_process_list_node_t *iotx_cm_process_list_pop(iotx_cm_conntext_t *cm_ctx,
                                                      iotx_cm_connectivity_types_t type)
{
    iotx_cm_process_list_t *list = NULL;
    iotx_cm_process_list_node_t *node = NULL;
    iotx_cm_connectivity_t *connectivity = NULL;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_parameter);
        return NULL;
    }

    if (IOTX_CM_CONNECTIVITY_TYPE_CLOUD == type) {
        connectivity = iotx_cm_find_connectivity(cm_ctx, iotx_cm_cloud_conn_get_target(), NULL);

    }
#ifdef CM_SUPPORT_LOCAL_CONN
    else if (IOTX_CM_CONNECTIVITY_TYPE_LOCAL == type) {
        connectivity = iotx_cm_find_connectivity(cm_ctx, iotx_cm_local_conn_get_target(), NULL);
    }
#endif

    list = connectivity->process_list;

    if (NULL == list) {
        CM_ERR(cm_log_error_status);
        return NULL;
    }

    HAL_MutexLock(connectivity->process_lock);

    if (list->size == 1) {
        node = list->header;
        list->header = list->tailer = NULL;
        list->size = 0;
        LITE_free(list);
        list = NULL;
        connectivity->process_list = NULL;
    } else {
        node = list->header;
        list->header = list->header->next;
        list->header->pre = NULL;
        list->size--;
    }

    HAL_MutexUnlock(connectivity->process_lock);
    return node;
}


int iotx_cm_process_list_get_size(iotx_cm_conntext_t *cm_ctx,
                                  iotx_cm_connectivity_types_t type)
{
    iotx_cm_connectivity_t *connectivity = NULL;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    if (IOTX_CM_CONNECTIVITY_TYPE_CLOUD == type) {
        connectivity = iotx_cm_find_connectivity(cm_ctx, iotx_cm_cloud_conn_get_target(), NULL);
    }
#ifdef CM_SUPPORT_LOCAL_CONN
    else if (IOTX_CM_CONNECTIVITY_TYPE_LOCAL == type) {
        connectivity = iotx_cm_find_connectivity(cm_ctx, iotx_cm_local_conn_get_target(), NULL);
    }
#endif

    if (NULL == connectivity || NULL == connectivity->process_list) {
        return 0;
    }

    return connectivity->process_list->size;
}
#endif /* CM_SUPPORT_MULTI_THREAD */


