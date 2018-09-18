/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifdef CONFIG_CM_SUPPORT_LOCAL_CONN

#include "iot_import.h"

#include "iotx_utils.h"
#include "iotx_system.h"
#include "iot_export.h"
#include "iotx_cm_common.h"
#include "iotx_cm_local_conn.h"
#include "iotx_cm_connection.h"
#include "iotx_cm_connectivity.h"
#include "iotx_local_conn_alcs.h"

#define CM_SUPPORT_MULTI_LOCAL_DEVICE

#define CM_SUPPORT_MAX_LOCAL_DEVICE     (1)

typedef struct iotx_cm_local_device_list_st {
    iotx_cm_local_device_t                        *node;
    void                                          *next;
    int                                            is_used;
} iotx_cm_local_device_list_t;

static iotx_cm_local_device_list_t *g_local_device = NULL;

static iotx_cm_local_device_list_t iotx_cm_local_device_list[CM_SUPPORT_MAX_LOCAL_DEVICE];

static int iotx_cm_local_conn_add_device(iotx_cm_local_device_t *device)
{
    iotx_cm_local_device_list_t *temp = NULL;
    int i = 0;

    if (NULL == device) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    for (i = 0; i < CM_SUPPORT_MAX_LOCAL_DEVICE; i++) {
        if (iotx_cm_local_device_list[i].is_used == 0) {
            iotx_cm_local_device_list[i].is_used = 1;
            break;
        }
    }

    if (i == CM_SUPPORT_MAX_LOCAL_DEVICE) {
        return FAIL_RETURN;
    }
    iotx_cm_local_device_list[i].node = device;

    if (NULL == g_local_device) {
        g_local_device = &iotx_cm_local_device_list[i];
        return SUCCESS_RETURN;
    }

    temp = g_local_device;
    while (temp->next) {
        temp = temp->next;
    }

    temp->next = &iotx_cm_local_device_list[i];

    return SUCCESS_RETURN;
}

/* remove */
static int iotx_cm_local_conn_remove_device(iotx_cm_local_device_t *device)
{
    iotx_cm_local_device_list_t *temp = NULL;
    iotx_cm_local_device_list_t *pre_temp = NULL;

    if (NULL == g_local_device) {
        CM_ERR(cm_log_error_parameter);
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
    for (i = 0; i < CM_SUPPORT_MAX_LOCAL_DEVICE; i++) {
        if (iotx_cm_local_device_list[i].is_used == 1) {
            LITE_free(iotx_cm_local_device_list[i].node->device_info);
            LITE_free(iotx_cm_local_device_list[i].node);
            iotx_cm_local_device_list[i].is_used = 0;
        }
    }
    return SUCCESS_RETURN;
}


static int iotx_cm_local_compare_device(iotx_cm_send_peer_t *device_info_1, iotx_cm_send_peer_t *device_info_2)
{
    if (NULL == device_info_1 || NULL == device_info_2) {
        return -1;
    }

    if (0 == strncmp(device_info_1->product_key, device_info_2->product_key, strlen(device_info_1->product_key)) &&
        0 == strncmp(device_info_1->device_name, device_info_2->device_name, strlen(device_info_1->device_name))) {
        return 0;
    }

    return -1;
}


/* find */
static iotx_cm_local_device_t *iotx_cm_local_conn_find_device(iotx_cm_send_peer_t *device_info)
{
    iotx_cm_local_device_list_t *temp = NULL;

    if (NULL == g_local_device) {
        CM_ERR(cm_log_error_parameter);
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


static void iotx_local_conn_event_callback(void *pcontext, iotx_connection_event_msg_t *event_msg)
{
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)pcontext;

    if (NULL == cm_ctx || NULL == event_msg) {
        CM_ERR(cm_log_error_parameter);
        return;
    }

    switch (event_msg->event_id) {
        case IOTX_CONNECTION_EVENT_DISCONNECT:
#if (CONFIG_SDK_THREAD_COST == 1)
            {
                /* send message to itself thread */
                int ret = 0;
                iotx_cm_process_list_node_t *node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL);
                if (NULL == node) {
                    return;
                }

                node->type = IOTX_CM_PROCESS_LOCAL_DISCONNECT;
                node->msg = NULL;

                if (FAIL_RETURN == (ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node))) {
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                }
            }
#else
            iotx_cm_local_conn_disconnect_handler(cm_ctx);
#endif /* CONFIG_SDK_THREAD_COST */
            break;

        case IOTX_CONNECTION_EVENT_RECONNECT: {
#if (CONFIG_SDK_THREAD_COST == 1)
            {
                /* send message to itself thread */
                int ret = 0;
                iotx_cm_process_list_node_t *node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL);
                if (NULL == node)
                {
                    return;
                }
                node->type = IOTX_CM_PROCESS_LOCAL_RECONNECT;
                node->msg = NULL;

                if (FAIL_RETURN == (ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node)))
                {
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                }
            }
#else
            iotx_cm_local_conn_reconnect_handler(cm_ctx);
#endif /* CONFIG_SDK_THREAD_COST */
        }
        break;

#ifdef CM_SUPPORT_MULTI_LOCAL_DEVICE
        case IOTX_CONNECTION_EVENT_ADD_DEVICE: {
            iotx_cm_local_device_t *_device = (iotx_cm_local_device_t *)event_msg->msg;
            iotx_cm_local_device_t *device = NULL;

            if (NULL == _device) {
                return;
            }

            CM_INFO(cm_log_info_add_device);

            device = LITE_malloc(sizeof(iotx_cm_local_device_t));
            if (NULL == device) {
                return;
            }
            device->device_info = LITE_malloc(sizeof(iotx_cm_send_peer_t));
            if (NULL == device->device_info) {
                LITE_free(device);
                return;
            }
            device->addr = LITE_malloc(strlen(_device->addr) + 1);
            if (NULL == device->addr) {
                LITE_free(device->device_info);
                LITE_free(device);
                return;
            }

            strcpy(device->device_info->product_key, _device->device_info->product_key);
            strcpy(device->device_info->device_name, _device->device_info->device_name);
            strcpy(device->addr, _device->addr);

#if (CONFIG_SDK_THREAD_COST == 1)
            {
                /* send message to itself thread */
                int ret = 0;
                iotx_cm_process_list_node_t *node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL);
                if (NULL == node) {
                    return;
                }
                node->type = IOTX_CM_PROCESS_ADD_DEVICE;
                node->msg = device;

                ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);

                if (FAIL_RETURN == ret) {
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                }
            }
#else
            iotx_cm_local_conn_found_device_handler(cm_ctx, device);
#endif /* CONFIG_SDK_THREAD_COST */
        }
        break;

        case IOTX_CONNECTION_EVENT_REMOVE_DEVICE: {
            iotx_cm_local_device_t *_device = (iotx_cm_local_device_t *)event_msg->msg;
            iotx_cm_local_device_t *device = NULL;

            if (NULL == _device) {
                return;
            }

            CM_INFO(cm_log_info_remove_device);

            device = LITE_malloc(sizeof(iotx_cm_local_device_t));
            if (NULL == device) {
                return;
            }
            device->device_info = LITE_malloc(sizeof(iotx_cm_send_peer_t));
            if (NULL == device->device_info) {
                LITE_free(device);
                return;
            }
            device->addr = LITE_malloc(strlen(_device->addr) + 1);
            if (NULL == device->addr) {
                LITE_free(device->device_info);
                LITE_free(device);
                return;
            }

            strcpy(device->device_info->product_key, _device->device_info->product_key);
            strcpy(device->device_info->device_name, _device->device_info->device_name);
            strcpy(device->addr, _device->addr);

#if (CONFIG_SDK_THREAD_COST == 1)
            {
                /* send message to itself thread */
                int ret = 0;
                iotx_cm_process_list_node_t *node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL);
                if (NULL == node) {
                    return;
                }
                node->type = IOTX_CM_PROCESS_REMOVE_DEVICE;
                node->msg = device;

                ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);

                if (FAIL_RETURN == ret) {
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                }
            }
#else
            iotx_cm_local_conn_remove_device_handler(cm_ctx, device);
#endif /* CONFIG_SDK_THREAD_COST */
        }
        break;
#endif /* CM_SUPPORT_MULTI_LOCAL_DEVICE */

        default:
            break;
    }
}

static int iotx_cm_local_conn_response_callback(void *pcontext, iotx_connection_msg_rsp_t *msg)
{
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)pcontext;

    if (NULL == cm_ctx || NULL == msg) {
        CM_ERR(cm_log_error_fail);
        return FAIL_RETURN;
    }

    CM_INFO(cm_log_info_rsp_type, msg->rsp_type);

    switch (msg->rsp_type) {
        case IOTX_CONNECTION_RESPONSE_ADD_SERVICE_SUCCESS: {
#if (CONFIG_SDK_THREAD_COST == 1)
            {
                /* send message to itself thread */
                int ret = 0;
                iotx_cm_process_list_node_t *node = NULL;
                iotx_cm_process_service_result_t *result_msg = NULL;

                if (NULL == (node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL)))
                {
                    return FAIL_RETURN;
                }

                node->type = IOTX_CM_PROCESS_ADD_SERVICE_RESULT;
                node->msg = LITE_malloc(sizeof(iotx_cm_process_service_result_t));
                if (NULL == node)
                {
                    CM_ERR(cm_log_error_memory);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                    return FAIL_RETURN;
                }

                result_msg = node->msg;

                result_msg->URI = LITE_malloc(strlen(msg->URI) + 1);
                if (NULL == result_msg->URI)
                {
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                    return FAIL_RETURN;
                }
                memset(result_msg->URI, 0x0, strlen(msg->URI) + 1);
                strncpy(result_msg->URI, (msg->URI), strlen(msg->URI) + 1);
                result_msg->result = 0;
                result_msg->is_add = 1;

                ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);

                if (FAIL_RETURN == ret)
                {
                    LITE_free(result_msg->URI);
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                    return FAIL_RETURN;
                }
            }
#else
            return iotx_cm_local_conn_service_handler(cm_ctx, msg->URI, 0, 1);
#endif /* CONFIG_SDK_THREAD_COST */
        }
        break;

        case IOTX_CONNECTION_RESPONSE_ADD_SERVICE_FAIL: {
#if (CONFIG_SDK_THREAD_COST == 1)
            {
                /* send message to itself thread */
                int ret = 0;
                iotx_cm_process_list_node_t *node = NULL;
                iotx_cm_process_service_result_t *result_msg = NULL;

                if (NULL == (node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL)))
                {
                    return FAIL_RETURN;
                }

                node->type = IOTX_CM_PROCESS_ADD_SERVICE_RESULT;
                node->msg = LITE_malloc(sizeof(iotx_cm_process_service_result_t));
                if (NULL == node)
                {
                    CM_ERR(cm_log_error_memory);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                    return FAIL_RETURN;
                }

                result_msg = node->msg;

                result_msg->URI = LITE_malloc(strlen(msg->URI) + 1);
                if (NULL == result_msg->URI)
                {
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                    return FAIL_RETURN;
                }
                memset(result_msg->URI, 0x0, strlen(msg->URI) + 1);
                strncpy(result_msg->URI, (msg->URI), strlen(msg->URI) + 1);

                result_msg->result = -1;
                result_msg->is_add = 1;

                ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);

                if (FAIL_RETURN == ret)
                {
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                    return FAIL_RETURN;
                }
            }
#else
            return iotx_cm_local_conn_service_handler(cm_ctx, msg->URI, -1, 1);
#endif /* CONFIG_SDK_THREAD_COST */
        }
        break;

        case IOTX_CONNECTION_RESPONSE_REMOVE_SERVICE_SUCCESS: {
#if (CONFIG_SDK_THREAD_COST == 1)
            {
                /* send message to itself thread */
                int ret = 0;
                iotx_cm_process_list_node_t *node = NULL;
                iotx_cm_process_service_result_t *result_msg = NULL;

                if (NULL == (node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL)))
                {
                    return FAIL_RETURN;
                }

                node->type = IOTX_CM_PROCESS_REMOVE_SERVICE_RESULT;
                node->msg = LITE_malloc(sizeof(iotx_cm_process_service_result_t));
                if (NULL == node)
                {
                    CM_ERR(cm_log_error_memory);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                    return FAIL_RETURN;
                }

                result_msg = node->msg;

                result_msg->URI = LITE_malloc(strlen(msg->URI) + 1);
                if (NULL == result_msg->URI)
                {
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                    return FAIL_RETURN;
                }
                memset(result_msg->URI, 0x0, strlen(msg->URI) + 1);
                strncpy(result_msg->URI, (msg->URI), strlen(msg->URI) + 1);
                result_msg->result = 0;
                result_msg->is_add = 0;

                ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);

                if (FAIL_RETURN == ret)
                {
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                    return FAIL_RETURN;
                }
            }
#else
            return iotx_cm_local_conn_service_handler(cm_ctx, msg->URI, 0, 0);
#endif /* CONFIG_SDK_THREAD_COST */
        }
        break;

        case IOTX_CONNECTION_RESPONSE_REMOVE_SERVICE_FAIL: {
#if (CONFIG_SDK_THREAD_COST == 1)
            {
                /* send message to itself thread */
                int ret = 0;
                iotx_cm_process_list_node_t *node = NULL;
                iotx_cm_process_service_result_t *result_msg = NULL;

                if (NULL == (node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL)))
                {
                    return FAIL_RETURN;
                }

                node->type = IOTX_CM_PROCESS_REMOVE_SERVICE_RESULT;
                node->msg = LITE_malloc(sizeof(iotx_cm_process_service_result_t));
                if (NULL == node)
                {
                    CM_ERR(cm_log_error_memory);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                    return FAIL_RETURN;
                }

                result_msg = node->msg;

                result_msg->URI = LITE_malloc(strlen(msg->URI) + 1);
                if (NULL == result_msg->URI)
                {
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                    return FAIL_RETURN;
                }
                memset(result_msg->URI, 0x0, strlen(msg->URI) + 1);
                strncpy(result_msg->URI, (msg->URI), strlen(msg->URI) + 1);
                result_msg->result = -1;
                result_msg->is_add = 0;

                ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);

                if (FAIL_RETURN == ret)
                {
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                }
            }
#else
            return iotx_cm_local_conn_service_handler(cm_ctx, msg->URI, -1, 0);
#endif /* CONFIG_SDK_THREAD_COST */
        }
        break;

        case IOTX_CONNECTION_RESPONSE_SEND_SUCCESS:
        case IOTX_CONNECTION_RESPONSE_SEND_FAIL:
        case IOTX_CONNECTION_RESPONSE_NEW_DATA: {
#if (CONFIG_SDK_THREAD_COST == 1)
            {
                /* send message to itself thread */
                int ret = 0;
                iotx_cm_process_list_node_t *node = NULL;
                iotx_cm_message_info_t *msg_info = NULL;
#ifdef CM_SUPPORT_TOPIC_DISPATCH
                iotx_cm_mapping_t *mapping = NULL;
#endif /* CM_SUPPORT_TOPIC_DISPATCH */

                if (NULL == (node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL)))
                {
                    return FAIL_RETURN;
                }

                node->type = IOTX_CM_PROCESS_LOCAL_NEW_DATA;
                node->msg = LITE_malloc(sizeof(iotx_cm_message_info_t));
                if (NULL == node->msg)
                {
                    CM_ERR(cm_log_error_memory);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                    return FAIL_RETURN;
                }

                msg_info = node->msg;

                msg_info->URI = LITE_malloc(msg->URI_length + 1);
                if (msg_info->URI)
                {
                    strncpy(msg_info->URI, msg->URI, msg->URI_length);
                } else
                {
                    CM_ERR(cm_log_error_memory);
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                    return FAIL_RETURN;
                }
                msg_info->URI_length = msg->URI_length;

#ifdef CM_SUPPORT_TOPIC_DISPATCH
                /* find mapping */
                mapping = iotx_cm_find_mapping(cm_ctx, IOTX_CM_MAPPING_TYPE_SERVICE, (char *)msg->URI, msg->URI_length);
                if (NULL == mapping)
                {
                    CM_WARNING(cm_log_warning_not_mapping);
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                    return FAIL_RETURN;
                }
#endif /* CM_SUPPORT_TOPIC_DISPATCH */

                msg_info->payload = CM_malloc(msg->payload_length + 1);
                msg_info->payload_length = msg->payload_length;
                if (msg_info->payload)
                {
                    strncpy(msg_info->payload, msg->payload, msg->payload_length);
                } else
                {
                    CM_ERR(cm_log_error_memory);
                    LITE_free(msg_info->URI);
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                    return FAIL_RETURN;
                }
                msg_info->conn_ctx = msg->conn_ctx;

                //            if (FAIL_RETURN == iotx_cm_parse_payload((char*)msg->payload, msg->payload_length, msg_info)) {
                //                CM_ERR(cm_log_error_memory);
                //                LITE_free(msg_info->URI);
                //                LITE_free(node->msg);
                //                iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                //                return FAIL_RETURN;
                //            }

                ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);

                if (FAIL_RETURN == ret)
                {
                    CM_ERR(cm_log_error_memory);
                    LITE_free(msg_info->URI);
                    LITE_free(msg_info->payload);
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                    break;
                }
            }
#else
            iotx_cm_message_info_t message_info;
#ifdef CM_SUPPORT_TOPIC_DISPATCH
            iotx_cm_mapping_t *mapping = NULL;
#endif /* CM_SUPPORT_TOPIC_DISPATCH */

            memset(&message_info, 0x0, sizeof(iotx_cm_message_info_t));
            message_info.URI = msg->URI;
            message_info.URI_length = msg->URI_length;
            message_info.payload = msg->payload;
            message_info.payload_length = msg->payload_length;
            message_info.conn_ctx = msg->conn_ctx;

            //        message_info.URI = LITE_malloc(msg->URI_length + 1);
            //        if (message_info.URI) {
            //            strncpy(message_info.URI, msg->URI, msg->URI_length);
            //        } else {
            //            iotx_cm_free_message_info(&message_info);
            //            return FAIL_RETURN;
            //        }

#ifdef CM_SUPPORT_TOPIC_DISPATCH
            /* find mapping */
            mapping = iotx_cm_find_mapping(cm_ctx, IOTX_CM_MAPPING_TYPE_SERVICE, (char *)msg->URI, msg->URI_length);
            if (NULL == mapping) {
                CM_WARNING(cm_log_warning_not_mapping);
                //            iotx_cm_free_message_info(&message_info);
                return FAIL_RETURN;
            }
            //        message_info.message_type = mapping->type;
#endif /* CM_SUPPORT_TOPIC_DISPATCH */

            //        if (FAIL_RETURN == iotx_cm_parse_payload((char*)msg->payload, msg->payload_length, &message_info)) {
            //            CM_ERR(cm_log_error_parse_payload);
            //            iotx_cm_free_message_info(&message_info);
            //            return FAIL_RETURN;
            //        }

            return iotx_cm_local_conn_response_handler(cm_ctx, &message_info);
#endif /* CONFIG_SDK_THREAD_COST */
        }
        break;

        default:
            CM_INFO(cm_log_warning_not_arrived);
            break;
    }

    return FAIL_RETURN;
}


int iotx_cm_local_conn_service_handler(iotx_cm_conntext_t *cm_ctx, char *URI, int result, int is_add)
{
    iotx_cm_event_msg_t event;
    iotx_cm_event_result_t event_result = {0};

    if (NULL == cm_ctx || NULL == URI) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    event_result.result = result;
    event_result.URI = LITE_malloc(CM_TOPIC_LEN_MAX);
    if (event_result.URI) {
        strncpy(event_result.URI, URI, CM_TOPIC_LEN_MAX);
    } else {
        return FAIL_RETURN;
    }

    event.event_id = is_add ? IOTX_CM_EVENT_ADD_SERVICE_RESULT : IOTX_CM_EVENT_REMOVE_SERVICE_RESULT;

    event.msg = (void *)&event_result;

    iotx_cm_trigger_event_callback(cm_ctx, &event);

    LITE_free(event_result.URI);

    return SUCCESS_RETURN;
}


int iotx_cm_local_conn_disconnect_handler(iotx_cm_conntext_t *cm_ctx)
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

int iotx_cm_local_conn_reconnect_handler(iotx_cm_conntext_t *cm_ctx)
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

int iotx_cm_local_conn_found_device_handler(iotx_cm_conntext_t *cm_ctx, iotx_cm_local_device_t *device)
{
    iotx_cm_event_msg_t event;

    if (FAIL_RETURN == iotx_cm_local_conn_add_device(device)) {
        return FAIL_RETURN;
    }

    event.event_id = IOTX_CM_EVENT_FOUND_DEVICE;
    event.msg = device->device_info;

    CM_INFO(cm_log_info_found_device);
    iotx_cm_trigger_event_callback(cm_ctx, &event);
    return SUCCESS_RETURN;
}


int iotx_cm_local_conn_remove_device_handler(iotx_cm_conntext_t *cm_ctx, iotx_cm_local_device_t *device)
{
    iotx_cm_event_msg_t event;
    if (FAIL_RETURN == iotx_cm_local_conn_remove_device(device)) {
        return FAIL_RETURN;
    }

    event.event_id = IOTX_CM_EVENT_REMOVE_DEVICE;
    event.msg = device->device_info;

    CM_INFO(cm_log_info_remove_device);
    iotx_cm_trigger_event_callback(cm_ctx, &event);
    return SUCCESS_RETURN;
}


int iotx_cm_local_conn_response_handler(iotx_cm_conntext_t *cm_ctx,
                                        iotx_cm_message_info_t *message_info)
{
    if (NULL == cm_ctx || NULL == message_info) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    CM_INFO(cm_log_info_URI, (char *)message_info->URI);

    if (cm_ctx->response_func) {
        cm_ctx->response_func(cm_ctx, IOTX_CM_MAPPING_TYPE_SERVICE, message_info);
    }

    return SUCCESS_RETURN;
}


int iotx_cm_local_conn_connect(void *handler, void *_connectivity)
{
    iotx_cm_connectivity_t *connectivity = NULL;
    iotx_cm_connection_t *connection = NULL;

    connectivity = (iotx_cm_connectivity_t *)_connectivity;
    connection = (iotx_cm_connection_t *)connectivity->context;

    connection->context = connection->init_func(connection, NULL);

    if (connection->context) {
        connectivity->status = IOTX_CM_CONNECTIVITY_STATUS_CONNECTED;
    }

    return connection->context ? SUCCESS_RETURN : FAIL_RETURN;
}


static void *_alcs_init(void *handler)
{
    iotx_cm_connection_t *connection = NULL;
    iotx_cm_local_device_t *local_device = NULL;

    connection = CM_malloc(sizeof(iotx_cm_connection_t));

    if (NULL == connection) {
        return NULL;
    }

    connection->init_func = iotx_local_conn_alcs_init;
    connection->init_second_func = iotx_local_conn_alcs_cloud_init;
    connection->deinit_func = iotx_local_conn_alcs_deinit;
    connection->send_func = iotx_local_conn_alcs_send;
    connection->sub_func = NULL;
    connection->unsub_func = NULL;
    connection->add_service_func = iotx_local_conn_alcs_add_service;
    connection->remove_service_func = iotx_local_conn_alcs_remove_service;
    connection->add_subdevice_func = iotx_local_conn_alcs_add_subdevice;
    connection->remove_subdevice_func = iotx_local_conn_alcs_remove_subdevice;
    connection->yield_func = iotx_local_conn_alcs_yield;
    connection->protocol_type = IOTX_CM_CONNECTION_PROTOCOL_TYPE_ALCS;
    connection->event_handler = iotx_local_conn_event_callback;
    connection->event_pcontext = handler;
    connection->response_handler = iotx_cm_local_conn_response_callback;

    local_device = CM_malloc(sizeof(iotx_cm_local_device_t));
    memset(local_device, 0x0, sizeof(iotx_cm_local_device_t));

    local_device->device_info = CM_malloc(sizeof(iotx_cm_send_peer_t));
    memset(local_device->device_info, 0x0, sizeof(iotx_cm_send_peer_t));

    strcpy(local_device->device_info->device_name, "device_name");
    strcpy(local_device->device_info->product_key, "product_key");

    iotx_cm_local_conn_add_device(local_device);
    return connection;
}

void *iotx_cm_local_conn_init(void *handler, void *param)
{
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)handler;
    iotx_cm_connectivity_t *connectivity = NULL;
    iotx_cm_connection_t *connection = NULL;
    iotx_cm_connectivity_alcs_param_t *alcs_param = (iotx_cm_connectivity_alcs_param_t *)param;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_parameter);
        return NULL;
    }

    switch (alcs_param->protocol_type) {
        case IOTX_CM_CONNECTION_PROTOCOL_TYPE_ALCS:
            connection = _alcs_init(cm_ctx);
            break;
        default:
            CM_ERR(cm_log_error_protocol, alcs_param->protocol_type);
            break;
    }

    if (connection == NULL) {
        return NULL;
    }

    connectivity = LITE_malloc(sizeof(iotx_cm_connectivity_t));
    if (NULL == connectivity) {
        CM_ERR(cm_log_error_memory);
        connection->deinit_func(connection);
        LITE_free(connection);
        return NULL;
    }

    connectivity->context = connection;
    connectivity->init_func = iotx_cm_local_conn_init;
    connectivity->init_second_func = iotx_cm_local_conn_init_second;
    connectivity->connect_func = iotx_cm_local_conn_connect;
    connectivity->register_func = iotx_cm_local_conn_register;
    connectivity->check_connected_func = iotx_cm_local_conn_check_connected;
    connectivity->trigger_connected_func = iotx_cm_local_conn_trigger_connected;
    connectivity->unregister_func = iotx_cm_local_conn_unregister;
    connectivity->add_service_func = iotx_cm_local_conn_add_service;
    connectivity->remove_service_func = iotx_cm_local_conn_remove_service;
    connectivity->send_func = iotx_cm_local_conn_send;
    connectivity->add_subdevice_func = iotx_cm_local_conn_add_subdevice;
    connectivity->remove_subdevice_func = iotx_cm_local_conn_remove_subdevice;
    connectivity->yield_func = iotx_cm_local_conn_yield;
    connectivity->deinit_func = iotx_cm_local_conn_deinit;
    connectivity->get_target_func = iotx_cm_local_conn_get_target;
    connectivity->check_target_func = iotx_cm_local_conn_check_target;
#if (CONFIG_SDK_THREAD_COST == 1)
    connectivity->add_send_func = iotx_cm_local_conn_add_send;
    connectivity->thread_process_func = iotx_cm_local_conn_process;
#endif /* CONFIG_SDK_THREAD_COST */

    connectivity->id = iotx_cm_get_connectivity_id();
    connectivity->type = IOTX_CM_CONNECTIVITY_TYPE_LOCAL;
    connectivity->is_try_connect = 0;
#if (CONFIG_SDK_THREAD_COST == 1)
    connectivity->process_lock = HAL_MutexCreate();
    HAL_ThreadCreate(&connectivity->pthread_process, iotx_cm_local_conn_process, cm_ctx, NULL, NULL);
#endif /* CONFIG_SDK_THREAD_COST */

    return connectivity;
}

int iotx_cm_local_conn_init_second(void *handler, void *_connectivity)
{
    iotx_cm_connectivity_t *connectivity = NULL;
    iotx_cm_connection_t *connection = NULL;

    connectivity = (iotx_cm_connectivity_t *)_connectivity;
    connection = (iotx_cm_connection_t *)connectivity->context;

    return connection->init_second_func(connection);
}

int iotx_cm_local_conn_trigger_connected(void *handler, void *_connectivity, iotx_cm_event_handle_fp_t event_fp,
        void *user_data)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)handler;

    if (NULL == connectivity || NULL == cm_ctx) {
        return FAIL_RETURN;
    }

    connectivity->is_try_connect = 1;

    if ((IOTX_CM_CONNECTIVITY_STATUS_CONNECTED != iotx_cm_get_connectivity_status(connectivity))
        && connectivity->connect_func) {
        connectivity->connect_func(cm_ctx, connectivity);
    }

    if (IOTX_CM_CONNECTIVITY_STATUS_CONNECTED == iotx_cm_get_connectivity_status(connectivity)) {
        iotx_cm_event_msg_t event_msg = {0};
        event_msg.event_id = IOTX_CM_EVENT_LOCAL_CONNECTED;
        event_msg.msg = (void *)connectivity;
        if(event_fp != NULL)
        {
            event_fp(cm_ctx, &event_msg, user_data);
        }
        else
        {
            iotx_cm_trigger_event_callback(cm_ctx, &event_msg);
        }
        return SUCCESS_RETURN;
    }

    return FAIL_RETURN;
}


int iotx_cm_local_conn_check_connected(void *handler, void *_connectivity, iotx_cm_event_handle_fp_t event_fp,
                                       void *user_data)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)handler;

    if (NULL == connectivity || NULL == cm_ctx) {
        return FAIL_RETURN;
    }

    if (IOTX_CM_CONNECTIVITY_STATUS_CONNECTED == iotx_cm_get_connectivity_status(connectivity)) {
        iotx_cm_event_msg_t event_msg = {0};
        event_msg.event_id = IOTX_CM_EVENT_LOCAL_CONNECTED;
        event_msg.msg = (void *)connectivity;
        if(event_fp != NULL)
        {
            event_fp(cm_ctx, &event_msg, user_data);
        }
        else
        {
            iotx_cm_trigger_event_callback(cm_ctx, &event_msg);
        }
        return SUCCESS_RETURN;
    }

    return FAIL_RETURN;
}


int iotx_cm_local_conn_register(void *handler, void *_connectivity, void *register_param, int count)
{
    return SUCCESS_RETURN;
}


int iotx_cm_local_conn_unregister(void *handler, void *_connectivity, const char *topic_filter)
{
    return SUCCESS_RETURN;
}


int iotx_cm_local_conn_add_service(void *handler, void *_connectivity, const char *topic_filter,
                                   iotx_cm_message_auth_types_t auth_type)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_cm_connection_t *connection = NULL;

    connection = (iotx_cm_connection_t *)connectivity->context;

    if (IOTX_CM_CONNECTIVITY_STATUS_CONNECTED != connectivity->status) {
        return FAIL_RETURN;
    }

    return connection->add_service_func(connection, topic_filter, IOTX_CM_MESSAGE_NEED_ACK, auth_type);
}


int iotx_cm_local_conn_remove_service(void *handler, void *_connectivity, const char *topic_filter)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_cm_connection_t *connection = NULL;

    connection = (iotx_cm_connection_t *)connectivity->context;

    if (IOTX_CM_CONNECTIVITY_STATUS_CONNECTED != connectivity->status) {
        return FAIL_RETURN;
    }

    return connection->remove_service_func(connection, topic_filter);
}


int iotx_cm_local_conn_send(void *handler,
                            void *_connectivity,
                            iotx_cm_send_peer_t *target,
                            const char *topic_filter,
                            iotx_cm_message_ack_types_t ack_type,
                            const void *payload,
                            int payload_length,
                            void *context,
                            void *sem)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_cm_connection_t *connection = NULL;
    iotx_connection_msg_t msg = {0};

    connection = (iotx_cm_connection_t *)connectivity->context;

    if (IOTX_CM_CONNECTIVITY_STATUS_CONNECTED != connectivity->status) {
        return FAIL_RETURN;
    }

    msg.type = IOTX_CONNECTION_MESSAGE_TYPE_SEND;
    msg.ack_type = ack_type;
    msg.URI = (char *)topic_filter;
    msg.URI_length = strlen(topic_filter);
    msg.payload = (char *)payload;
    msg.payload_length = payload_length;
    msg.response_handler = iotx_cm_local_conn_response_callback;
    msg.response_pcontext = handler;
    msg.sem = sem;

    return connection->send_func(connection, context, &msg);
}


int iotx_cm_local_conn_add_subdevice(void *handler, void *_connectivity, const char *pk, const char *dn)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_cm_connection_t *connection = NULL;

    connection = (iotx_cm_connection_t *)connectivity->context;

    if (IOTX_CM_CONNECTIVITY_STATUS_CONNECTED != connectivity->status) {
        return FAIL_RETURN;
    }

    return connection->add_subdevice_func(connection, pk, dn);
}


int iotx_cm_local_conn_remove_subdevice(void *handler, void *_connectivity, const char *pk, const char *dn)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_cm_connection_t *connection = NULL;

    connection = (iotx_cm_connection_t *)connectivity->context;

    if (IOTX_CM_CONNECTIVITY_STATUS_CONNECTED != connectivity->status) {
        return FAIL_RETURN;
    }

    return connection->remove_subdevice_func(connection, pk, dn);
}


int iotx_cm_local_conn_yield(void *handler, void *_connectivity, int timeout_ms)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_cm_connection_t *connection = NULL;

    connection = (iotx_cm_connection_t *)connectivity->context;

    if (IOTX_CM_CONNECTIVITY_STATUS_CONNECTED != connectivity->status) {
        return FAIL_RETURN;
    }

    return connection->yield_func(connection, timeout_ms);
}


int iotx_cm_local_conn_deinit(void *handler, void *_connectivity)
{
    int ret = -1;
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_cm_connection_t *connection = NULL;

#if (CONFIG_SDK_THREAD_COST == 1)
    HAL_ThreadDelete(connectivity->pthread_process);
    HAL_SleepMs(100);
    HAL_MutexDestroy(connectivity->process_lock);
    iotx_cm_free_process_list_handler(connectivity, handler);
#endif /* CONFIG_SDK_THREAD_COST */

    connection = (iotx_cm_connection_t *)connectivity->context;

    iotx_cm_local_conn_remove_device_all();
    g_local_device = NULL;

    if (IOTX_CM_CONNECTIVITY_STATUS_CONNECTED == connectivity->status) {
        ret = connection->deinit_func(connection);
    }

    //    connectivity->status = IOTX_CM_CONNECTIVITY_STATUS_DISCONNECTED;

    LITE_free(connection);

    connectivity->context = NULL;

    return ret;
}


iotx_cm_send_peer_t *iotx_cm_local_conn_get_target()
{
    if (g_local_device == NULL) {
        return NULL;
    }

    return g_local_device->node->device_info;
}


int iotx_cm_local_conn_check_target(iotx_cm_send_peer_t *peer)
{
    if (peer == NULL) {
        return FAIL_RETURN;
    }

    if (NULL != iotx_cm_local_conn_find_device(peer)) {
        return SUCCESS_RETURN;
    }

    return FAIL_RETURN;
}


#if (CONFIG_SDK_THREAD_COST == 1)
int iotx_cm_local_conn_add_send(void *handler, iotx_cm_send_peer_t *target, iotx_cm_message_info_t *message_info,
                                void *sem)
{
    /* send message to itself thread */
    int ret = 0;
    iotx_cm_process_list_node_t *node = NULL;
    iotx_cm_process_send_t *msg = NULL;
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)handler;

    node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL);
    if (NULL == node) {
        return FAIL_RETURN;
    }

    node->type = IOTX_CM_PROCESS_LOCAL_SEND;
    node->msg = CM_malloc(sizeof(iotx_cm_process_send_t));
    if (NULL == node->msg) {
        CM_INFO(cm_log_error_parameter);
        iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
        return FAIL_RETURN;
    }

    msg = node->msg;
    if (target) {
        msg->target = CM_malloc(sizeof(iotx_cm_send_peer_t));
        memset(msg->target, 0x0, sizeof(iotx_cm_send_peer_t));
        strncpy(msg->target->device_name, target->device_name, strlen(target->device_name));
        strncpy(msg->target->product_key, target->product_key, strlen(target->product_key));
    } else {
        msg->target = NULL;
    }

    msg->URI = CM_malloc(strlen(message_info->URI) + 1);
    if (NULL == msg->URI) {
        CM_ERR(cm_log_error_memory);
        LITE_free(msg->target);
        LITE_free(msg);
        iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
        return FAIL_RETURN;
    }
    strncpy(msg->URI, message_info->URI, strlen(message_info->URI));

    msg->payload = CM_malloc(message_info->payload_length);
    if (NULL == msg->payload) {
        CM_ERR(cm_log_error_memory);
        LITE_free(msg->URI);
        LITE_free(msg->target);
        LITE_free(msg);
        iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
        return FAIL_RETURN;
    }

    memcpy(msg->payload, message_info->payload, message_info->payload_length);
    msg->payload_length = message_info->payload_length;
    msg->ack_type = message_info->ack_type;
    msg->conn_ctx = message_info->conn_ctx;
    msg->sem = sem;

    ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);

    if (FAIL_RETURN == ret) {
        if (msg->target) {
            LITE_free(msg->target);
        }
        if (msg->URI) {
            LITE_free(msg->URI);
        }
        if (msg->payload) {
            LITE_free(msg->payload);
        }
        LITE_free(node->msg);
        iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
    }

    return ret;
}

void *iotx_cm_local_conn_process(void *pclient)
{
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)pclient;
    iotx_cm_connectivity_t *connectivity = iotx_cm_find_connectivity_by_type(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL);

    if (NULL == cm_ctx) {
        CM_INFO(cm_log_error_parameter);
        return NULL;
    }

    CM_INFO(cm_log_info_enter_process_2);

    while (!connectivity) {
        connectivity = iotx_cm_find_connectivity_by_type(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL);
        HAL_SleepMs(1000);
        continue;
    }

    while (connectivity) {
        if (connectivity->is_try_connect
            && IOTX_CM_CONNECTIVITY_STATUS_CONNECTED != iotx_cm_get_connectivity_status(connectivity)) {
            if (SUCCESS_RETURN == iotx_cm_local_conn_trigger_connected(cm_ctx, connectivity, NULL, NULL)) {
                iotx_cm_set_connectivity_status(connectivity, IOTX_CM_CONNECTIVITY_STATUS_CONNECTED);
            }
            HAL_SleepMs(1000);
            continue;
        }

        if (iotx_cm_process_list_get_size(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL) > 0) {
            iotx_cm_process_list_node_t *node = NULL;
            if (NULL != (node = iotx_cm_process_list_pop(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL))) {
                CM_INFO(cm_log_info_process_type, node->type);

                switch (node->type) {
                    case IOTX_CM_PROCESS_CONNECT: {
                        if (SUCCESS_RETURN == iotx_cm_local_conn_trigger_connected(cm_ctx, connectivity, NULL, NULL)) {
                            iotx_cm_set_connectivity_status(connectivity, IOTX_CM_CONNECTIVITY_STATUS_CONNECTED);
                        }
                    }
                    break;

                    case IOTX_CM_PROCESS_DISCONNECT: {
                        /* todo */
                        /* connectivity must reset to NULL */
                        /* iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
                        if (FAIL_RETURN == iotx_cm_local_conn_deinit(cm_ctx, connectivity)) {
                            CM_ERR(cm_log_error_fail);
                        } */
                        /*  connectivity->status = IOTX_CM_CONNECTIVITY_STATUS_UNCREATED;
                         linked_list_remove(cm_ctx->list_connectivity, connectivity);
                         LITE_free(connectivity); */
                        /* return NULL; */
                    }
                    break;

                    case IOTX_CM_PROCESS_ADD_DEVICE: {
                        iotx_cm_local_device_t *device = (iotx_cm_local_device_t *)node->msg;
                        iotx_cm_local_conn_found_device_handler(cm_ctx, device);
                    }
                    break;

                    case IOTX_CM_PROCESS_REMOVE_DEVICE: {
                        iotx_cm_local_device_t *device = (iotx_cm_local_device_t *)node->msg;
                        iotx_cm_local_conn_remove_device_handler(cm_ctx, device);
                    }
                    break;

                    case IOTX_CM_PROCESS_LOCAL_SEND: {
                        iotx_cm_process_send_t *_send = (iotx_cm_process_send_t *)node->msg;
                        if (FAIL_RETURN == iotx_cm_local_conn_send(cm_ctx,
                                connectivity,
                                _send->target,
                                _send->URI,
                                _send->ack_type,
                                _send->payload,
                                _send->payload_length,
                                _send->conn_ctx,
                                _send->sem)) {
                            CM_ERR(cm_log_error_fail);
                        }

                        if (_send->target) {
                            LITE_free(_send->target);
                        }
                        if (_send->URI) {
                            LITE_free(_send->URI);
                        }
                        if (_send->payload) {
                            LITE_free(_send->payload);
                        }
                        LITE_free(_send);
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

                    case IOTX_CM_PROCESS_REGISTER: {
                        int i = 0;
                        iotx_cm_process_register_t *_register = (iotx_cm_process_register_t *)node->msg;
                        iotx_cm_register_param_t **register_param = (iotx_cm_register_param_t **)_register->register_param;
                        iotx_cm_local_conn_register(cm_ctx, connectivity, _register->register_param, _register->count);
                        for (; i < _register->count; i++) {
                            LITE_free(register_param[i]->URI);
                            LITE_free(register_param[i]);
                        }
                        LITE_free(register_param);
                        LITE_free(_register);
                    }
                    break;

                    case IOTX_CM_PROCESS_UNREGISTER: {
                        char *URI = (char *)node->msg;
                        iotx_cm_local_conn_unregister(cm_ctx, connectivity, URI);
                        LITE_free(URI);
                    }
                    break;

                    case IOTX_CM_PROCESS_ADD_SERVICE: {
                        iotx_cm_process_service_t *_service = (iotx_cm_process_service_t *)node->msg;
#ifdef CM_SUPPORT_TOPIC_DISPATCH
                        iotx_cm_mapping_t *mapping = NULL;
                        if (FAIL_RETURN == iotx_cm_add_mapping(cm_ctx,
                                                               IOTX_CM_MAPPING_TYPE_SERVICE,
                                                               _service->URI,
                                                               _service->register_func,
                                                               _service->user_data,
                                                               _service->mail_box)) {
                            iotx_cm_event_msg_t event;
                            iotx_cm_event_result_t result_pt = {0};

                            result_pt.result = -1;
                            result_pt.URI = _service->URI;
                            event.event_id = IOTX_CM_EVENT_ADD_SERVICE_RESULT;
                            event.msg = (void *)&result_pt;
                            CM_ERR(cm_log_error_fail_1);

                            iotx_cm_trigger_event_callback(cm_ctx, &event);
                            LITE_free(_service->URI);
                            LITE_free(_service);
                            continue;
                        }

                        mapping = iotx_cm_find_mapping(cm_ctx, IOTX_CM_MAPPING_TYPE_SERVICE, _service->URI, strlen(_service->URI));

                        iotx_cm_local_conn_add_service(cm_ctx, connectivity, mapping->URI, _service->auth_type);
#else /* CM_SUPPORT_TOPIC_DISPATCH */
                        iotx_cm_local_conn_add_service(cm_ctx, connectivity, _service->URI, _service->auth_type);
#endif
                        LITE_free(_service->URI);
                        LITE_free(_service);
                    }
                    break;

                    case IOTX_CM_PROCESS_REMOVE_SERVICE: {
                        char *URI = (char *)node->msg;
                        iotx_cm_local_conn_remove_service(cm_ctx, connectivity, URI);
                        iotx_cm_remove_mapping(cm_ctx, IOTX_CM_MAPPING_TYPE_SERVICE, URI);
                        LITE_free(URI);
                    }
                    break;

                    case IOTX_CM_PROCESS_ADD_SERVICE_RESULT:
                    case IOTX_CM_PROCESS_REMOVE_SERVICE_RESULT: {
                        iotx_cm_process_service_result_t *_service = (iotx_cm_process_service_result_t *)node->msg;
                        iotx_cm_local_conn_service_handler(cm_ctx, _service->URI, _service->result, _service->is_add);
                        LITE_free(_service->URI);
                        LITE_free(_service);
                    }
                    break;

                    case IOTX_CM_PROCESS_LOCAL_NEW_DATA: {
                        iotx_cm_message_info_t *message_info = (iotx_cm_message_info_t *)node->msg;
                        iotx_cm_local_conn_response_handler(cm_ctx, message_info);
                        LITE_free(message_info->URI);
                        LITE_free(message_info->payload);
                        LITE_free(node->msg);
                    }
                    break;

                    case IOTX_CM_PROCESS_ADD_SUBDIVCE: {
                        iotx_cm_process_subdevice_t *subdevice = (iotx_cm_process_subdevice_t *)node->msg;
                        iotx_cm_local_conn_add_subdevice(cm_ctx, connectivity, subdevice->pk, subdevice->dn);
                        LITE_free(node->msg);
                    }
                    break;

                    case IOTX_CM_PROCESS_REMOVE_SUBDIVCE: {
                        iotx_cm_process_subdevice_t *subdevice = (iotx_cm_process_subdevice_t *)node->msg;
                        iotx_cm_local_conn_remove_subdevice(cm_ctx, connectivity, subdevice->pk, subdevice->dn);
                        LITE_free(node->msg);
                    }
                    break;
                    case IOTX_CM_PROCESS_LOCAL_CLOUD_INIT: {
                        iotx_cm_local_conn_init_second(cm_ctx, connectivity);
                    }
                    break;
                    default:
                        break;
                }

                iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);
            }
        }
        /* local yield */
        if (connectivity && IOTX_CM_CONNECTIVITY_STATUS_CONNECTED == iotx_cm_get_connectivity_status(connectivity)) {
            connectivity->yield_func(cm_ctx, connectivity, 50);
        } else {
            HAL_SleepMs(50);
        }
    }

    return NULL;
}
#endif /* CONFIG_SDK_THREAD_COST */

#endif /* CONFIG_CM_SUPPORT_LOCAL_CONN */

