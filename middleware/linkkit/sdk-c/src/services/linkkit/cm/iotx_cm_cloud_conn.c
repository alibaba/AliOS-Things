/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifdef CONFIG_CM_VIA_CLOUD_CONN

#include "iot_import.h"

#include "iotx_utils.h"
#include "iotx_system.h"
#include "iot_export.h"
#include "iotx_cm_common.h"
#include "iotx_cm_connection.h"
#include "iotx_cm_connectivity.h"
#include "iotx_cm_cloud_conn.h"
#include "iotx_cm_conn_mqtt.h"
#ifdef COAP_COMM_ENABLED
    #include "iotx_cm_conn_coap.h"
#endif
#include "iotx_cm_conn_http.h"

static iotx_cm_send_peer_t g_cloud_target;

static void iotx_cm_cloud_conn_event_callback(void *_cm_ctx, iotx_connection_event_msg_t *msg)
{
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)_cm_ctx;

    if (NULL == cm_ctx || NULL == msg) {
        CM_ERR(cm_log_error_parameter);
        return;
    }

    CM_INFO(cm_log_info_event_id, msg->event_id);

    switch (msg->event_id) {
        case IOTX_CONNECTION_EVENT_DISCONNECT: {
#if (CONFIG_SDK_THREAD_COST == 1)
            {
                /* send message to itself thread */
                int ret = 0;
                iotx_cm_process_list_node_t *node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD);
                if (NULL == node)
                {
                    CM_ERR(cm_log_error_get_node);
                    return;
                }

                node->type = IOTX_CM_PROCESS_CLOUD_DISCONNECT;
                node->msg = NULL;

                ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                if (FAIL_RETURN == ret)
                {
                    CM_ERR(cm_log_error_push_node);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                }
            }
#else
            iotx_cm_cloud_conn_disconnect_handler(cm_ctx);
#endif /* CONFIG_SDK_THREAD_COST */
        }
        break;

        case IOTX_CONNECTION_EVENT_RECONNECT: {
#if (CONFIG_SDK_THREAD_COST == 1)
            {
                /* send message to itself thread */
                int ret = 0;
                iotx_cm_process_list_node_t *node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD);
                if (NULL == node)
                {
                    CM_ERR(cm_log_error_get_node);
                    return;
                }
                node->type = IOTX_CM_PROCESS_CLOUD_RECONNECT;
                node->msg = NULL;

                ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                if (FAIL_RETURN == ret)
                {
                    CM_ERR(cm_log_error_push_node);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                }
            }
#else
            iotx_cm_cloud_conn_reconnect_handler(cm_ctx);
#endif /* CONFIG_SDK_THREAD_COST */
        }
        break;

        default:
            CM_WARNING(cm_log_warning_not_arrived);
            break;
    }
}


static int iotx_cm_cloud_conn_response_callback(void *_cm_ctx, iotx_connection_msg_rsp_t *msg)
{
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)_cm_ctx;

    if (NULL == cm_ctx || NULL == msg) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    CM_INFO(cm_log_info_rsp_type, msg->rsp_type);

    switch (msg->rsp_type) {
        case IOTX_CONNECTION_RESPONSE_SUBSCRIBE_SUCCESS: {
#if (CONFIG_SDK_THREAD_COST == 1)
            {
                /* send message to itself thread */
                int ret = 0;
                iotx_cm_process_list_node_t *node = NULL;
                iotx_cm_process_register_result_t *result_msg = NULL;

                node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD);
                if (NULL == node)
                {
                    return FAIL_RETURN;
                }

                node->type = IOTX_CM_PROCESS_REGISTER_RESULT;
                if (NULL == (node->msg = CM_malloc(sizeof(iotx_cm_process_register_result_t))))
                {
                    CM_ERR(cm_log_error_memory);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                    return FAIL_RETURN;
                }

                result_msg = node->msg;
                if (NULL == (result_msg->URI = CM_malloc(strlen(msg->URI) + 1)))
                {
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                    return FAIL_RETURN;
                }
                memset(result_msg->URI, 0x0, strlen(msg->URI) + 1);
                strncpy(result_msg->URI, (msg->URI), strlen(msg->URI) + 1);
                result_msg->result = 0;
                result_msg->is_register = 1;

                if (FAIL_RETURN == (ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node)))
                {
                    LITE_free(result_msg->URI);
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                }
            }
#else
            if (msg->URI) {
                iotx_cm_cloud_conn_register_handler(cm_ctx, msg->URI, 0, 1);
            }
#endif /* CONFIG_SDK_THREAD_COST */
        }
        break;

        case IOTX_CONNECTION_RESPONSE_SUBSCRIBE_FAIL: {
#if (CONFIG_SDK_THREAD_COST == 1)
            {
                /* send message to itself thread */
                int ret = 0;
                iotx_cm_process_list_node_t *node = NULL;
                iotx_cm_process_register_result_t *result_msg = NULL;

                node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD);
                if (NULL == node)
                {
                    return FAIL_RETURN;
                }

                node->type = IOTX_CM_PROCESS_REGISTER_RESULT;
                if (NULL == (node->msg = CM_malloc(sizeof(iotx_cm_process_register_result_t))))
                {
                    CM_ERR(cm_log_error_memory);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                    return FAIL_RETURN;
                }

                result_msg = node->msg;

                if (NULL == (result_msg->URI = CM_malloc(strlen(msg->URI) + 1)))
                {
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                    return FAIL_RETURN;
                }
                memset(result_msg->URI, 0x0, strlen(msg->URI) + 1);
                strncpy(result_msg->URI, (msg->URI), strlen(msg->URI) + 1);

                result_msg->result = -1;
                result_msg->is_register = 1;

                if (FAIL_RETURN == (ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node)))
                {
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                }
            }
#else
            if (msg->URI) {
                iotx_cm_cloud_conn_register_handler(cm_ctx, msg->URI, -1, 1);
            }
#endif /* CONFIG_SDK_THREAD_COST */
        }
        break;

        case IOTX_CONNECTION_RESPONSE_UNSUBSCRIBE_SUCCESS: {
#if (CONFIG_SDK_THREAD_COST == 1)
            {
                /* send message to itself thread */
                int ret = 0;
                iotx_cm_process_list_node_t *node = NULL;
                iotx_cm_process_register_result_t *result_msg = NULL;

                if (NULL == (node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD)))
                {
                    return FAIL_RETURN;
                }

                node->type = IOTX_CM_PROCESS_UNREGISTER_RESULT;
                if (NULL == (node->msg = CM_malloc(sizeof(iotx_cm_process_register_result_t))))
                {
                    CM_ERR(cm_log_error_memory);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                    return FAIL_RETURN;
                }

                result_msg = node->msg;

                if (NULL == (result_msg->URI = CM_malloc(strlen(msg->URI) + 1)))
                {
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                    return FAIL_RETURN;
                }
                memset(result_msg->URI, 0x0, strlen(msg->URI) + 1);
                strncpy(result_msg->URI, (msg->URI), strlen(msg->URI) + 1);
                result_msg->result = 1;
                result_msg->is_register = 0;

                if (FAIL_RETURN == (ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node)))
                {
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                }
            }
#else
            if (msg->URI) {
                iotx_cm_cloud_conn_register_handler(cm_ctx, msg->URI, 0, 0);
            }
#endif /* CONFIG_SDK_THREAD_COST */
        }
        break;

        case IOTX_CONNECTION_RESPONSE_UNSUBSCRIBE_FAIL: {
#if (CONFIG_SDK_THREAD_COST == 1)
            {
                /* send message to itself thread */
                int ret = 0;
                iotx_cm_process_list_node_t *node = NULL;
                iotx_cm_process_register_result_t *result_msg = NULL;

                if (NULL == (node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD)))
                {
                    return FAIL_RETURN;
                }

                node->type = IOTX_CM_PROCESS_UNREGISTER_RESULT;
                if (NULL == (node->msg = CM_malloc(sizeof(iotx_cm_process_register_result_t))))
                {
                    CM_ERR(cm_log_error_memory);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                    return FAIL_RETURN;
                }

                result_msg = node->msg;

                if (NULL == (result_msg->URI = CM_malloc(strlen(msg->URI) + 1)))
                {
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                    return FAIL_RETURN;
                }
                memset(result_msg->URI, 0x0, strlen(msg->URI) + 1);
                strncpy(result_msg->URI, (msg->URI), strlen(msg->URI) + 1);
                result_msg->result = 0;
                result_msg->is_register = 0;

                if (FAIL_RETURN == (ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node)))
                {
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                }
            }
#else
            if (msg->URI) {
                iotx_cm_cloud_conn_register_handler(cm_ctx, msg->URI, -1, 0);
            }
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

                if (NULL == (node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD)))
                {
                    return FAIL_RETURN;
                }

                node->type = IOTX_CM_PROCESS_CLOUD_NEW_DATA;
                if (NULL == (node->msg = CM_malloc(sizeof(iotx_cm_message_info_t))))
                {
                    CM_ERR(cm_log_error_memory);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                    return FAIL_RETURN;
                }

                msg_info = node->msg;

                msg_info->URI = CM_malloc(msg->URI_length + 1);
                if (msg_info->URI)
                {
                    strncpy(msg_info->URI, msg->URI, msg->URI_length);
                } else
                {
                    CM_ERR(cm_log_error_memory);
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                    return FAIL_RETURN;
                }
                msg_info->URI_length = msg->URI_length;

#ifdef CM_SUPPORT_TOPIC_DISPATCH
                /* find mapping */
                if (NULL == (mapping = iotx_cm_find_mapping(cm_ctx, IOTX_CM_MAPPING_TYPE_REGISTER, (char *)msg->URI, msg->URI_length)))
                {
                    CM_WARNING(cm_log_warning_not_mapping);
                    LITE_free(msg_info->URI);
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                    return FAIL_RETURN;
                }
#endif

                //            if (FAIL_RETURN == iotx_cm_parse_payload((char*)msg->payload, msg->payload_length, msg_info)) {
                //                CM_ERR(cm_log_error_parse_payload);
                //                iotx_cm_free_message_info(msg_info);
                //                LITE_free(node->msg);
                //                iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                //                return FAIL_RETURN;
                //            }

                msg_info->payload = CM_malloc(msg->payload_length + 1);
                if (msg_info->payload)
                {
                    strncpy(msg_info->payload, msg->payload, msg->payload_length);
                } else
                {
                    CM_ERR(cm_log_error_memory);
                    LITE_free(msg_info->URI);
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                    return FAIL_RETURN;
                }
                msg_info->payload_length = msg->payload_length;
                msg_info->conn_ctx = msg->conn_ctx;

                if (FAIL_RETURN == (ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node)))
                {
                    LITE_free(msg_info->URI);
                    LITE_free(msg_info->payload);
                    LITE_free(node->msg);
                    iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
                    break;
                }
            }
#else
            iotx_cm_message_info_t message_info;
#ifdef CM_SUPPORT_TOPIC_DISPATCH
            iotx_cm_mapping_t *mapping = NULL;
#endif

            memset(&message_info, 0x0, sizeof(iotx_cm_message_info_t));
            message_info.URI = msg->URI;
            message_info.URI_length = msg->URI_length;
            message_info.payload = msg->payload;
            message_info.payload_length = msg->payload_length;
            message_info.conn_ctx = msg->conn_ctx;

            //        message_info.URI = CM_malloc(msg->URI_length + 1);
            //        if (message_info.URI) {
            //            strncpy(message_info.URI, msg->URI, msg->URI_length);
            //        } else {
            //            iotx_cm_free_message_info(&message_info);
            //            return FAIL_RETURN;
            //        }

#ifdef CM_SUPPORT_TOPIC_DISPATCH
            /* find mapping */
            if (NULL == (mapping = iotx_cm_find_mapping(cm_ctx, IOTX_CM_MAPPING_TYPE_REGISTER, (char *)msg->URI,
                                   msg->URI_length))) {
                CM_WARNING(cm_log_warning_not_mapping);
                //            iotx_cm_free_message_info(&message_info);
                return FAIL_RETURN;
            }
            //message_info.message_type = mapping->type;
#endif

            //        if (FAIL_RETURN == iotx_cm_parse_payload((char*)msg->payload, msg->payload_length, &message_info)) {
            //            CM_ERR(cm_log_error_parse_payload);
            //            iotx_cm_free_message_info(&message_info);
            //            return FAIL_RETURN;
            //        }

            iotx_cm_cloud_conn_response_handler(cm_ctx, &message_info);
#endif /* CONFIG_SDK_THREAD_COST */
        }
        break;

        default:
            CM_WARNING(cm_log_warning_not_arrived);
            break;
    }

    return SUCCESS_RETURN;
}


int iotx_cm_cloud_conn_disconnect_handler(iotx_cm_conntext_t *cm_ctx)
{
    iotx_cm_event_msg_t event;

    event.event_id = IOTX_CM_EVENT_CLOUD_DISCONNECT;
    event.msg = NULL;

    CM_INFO(cm_log_info_cloud_disconnect);

    iotx_cm_trigger_event_callback(cm_ctx, &event);

    return SUCCESS_RETURN;
}

int iotx_cm_cloud_conn_reconnect_handler(iotx_cm_conntext_t *cm_ctx)
{
    iotx_cm_event_msg_t event;

    event.event_id = IOTX_CM_EVENT_CLOUD_RECONNECT;
    event.msg = NULL;

    CM_INFO(cm_log_info_cloud_reconnect);

    iotx_cm_trigger_event_callback(cm_ctx, &event);

    return SUCCESS_RETURN;
}

int iotx_cm_cloud_conn_register_handler(iotx_cm_conntext_t *cm_ctx, char *URI, int result, int is_register)
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

    if (is_register) {
        event.event_id = IOTX_CM_EVENT_REGISTER_RESULT;
    } else {
        event.event_id = IOTX_CM_EVENT_UNREGISTER_RESULT;
    }

    event.msg = (void *)&event_result;

    iotx_cm_trigger_event_callback(cm_ctx, &event);

    LITE_free(event_result.URI);

    return SUCCESS_RETURN;
}


int iotx_cm_cloud_conn_response_handler(iotx_cm_conntext_t *cm_ctx, iotx_cm_message_info_t *message_info)
{
    if (NULL == cm_ctx || NULL == message_info) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    CM_INFO(cm_log_info_URI, (char *)message_info->URI);

    if (cm_ctx->response_func) {
        cm_ctx->response_func(cm_ctx, IOTX_CM_MAPPING_TYPE_REGISTER, message_info);
    }

    return SUCCESS_RETURN;
}


int iotx_cm_cloud_conn_connect(void *handler, void *_connectivity)
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


/* can not call twice */
static void *_mqtt_init(void *handler)
{
    iotx_cm_connection_t *connection = NULL;

    connection = CM_malloc(sizeof(iotx_cm_connection_t));

    if (NULL == connection) {
        return NULL;
    }

    connection->init_func = iotx_cm_conn_mqtt_init;
    connection->deinit_func = iotx_cm_conn_mqtt_deinit;
    connection->send_func = iotx_cm_conn_mqtt_publish;
    connection->sub_func = iotx_cm_conn_mqtt_subscribe;
    connection->unsub_func = iotx_cm_conn_mqtt_unsubscribe;
    connection->add_service_func = NULL;
    connection->remove_service_func = NULL;
    connection->add_subdevice_func = NULL;
    connection->remove_subdevice_func = NULL;
    connection->yield_func = iotx_cm_conn_mqtt_yield;
    connection->protocol_type = IOTX_CM_CONNECTION_PROTOCOL_TYPE_MQTT;
    connection->event_handler = iotx_cm_cloud_conn_event_callback;
    connection->event_pcontext = handler;
    connection->response_handler = iotx_cm_cloud_conn_response_callback;
    return connection;
}

/* can not call twice */
static void *_coap_init(void *handler)
{
#ifdef COAP_COMM_ENABLED
    iotx_cm_connection_t *connection = NULL;

    connection = CM_malloc(sizeof(iotx_cm_connection_t));

    if (NULL == connection) {
        return NULL;
    }

    connection->init_func = iotx_cm_conn_coap_init;
    connection->deinit_func = iotx_cm_conn_coap_deinit;
    connection->send_func = iotx_cm_conn_coap_send;
    connection->sub_func = iotx_cm_conn_coap_subscribe;
    connection->unsub_func = iotx_cm_conn_coap_unsubscribe;
    connection->add_service_func = NULL;
    connection->remove_service_func = NULL;
    connection->add_subdevice_func = NULL;
    connection->remove_subdevice_func = NULL;
    connection->yield_func = iotx_cm_conn_coap_yield;
    connection->protocol_type = IOTX_CM_CONNECTION_PROTOCOL_TYPE_COAP;
    connection->event_handler = iotx_cm_cloud_conn_event_callback;
    connection->event_pcontext = handler;
    connection->response_handler = iotx_cm_cloud_conn_response_callback;

    return connection;
#else
    CM_INFO("!!!!!!!!!!!!!!!please set FEATURE_COAP_COMM_ENABLED = y!!!!!!!!!!!!!!!");
    CM_INFO(cm_log_info_not_support);
    return NULL;
#endif
}

#ifdef HTTP_COMM_ENABLED
/* can not call twice */
static void *_http_init(void *handler)
{
    iotx_cm_connection_t *connection = NULL;

    connection = CM_malloc(sizeof(iotx_cm_connection_t));

    if (NULL == connection) {
        return NULL;
    }

    connection->init_func = iotx_cm_conn_http_init;
    connection->deinit_func = iotx_cm_conn_http_deinit;
    connection->send_func = iotx_cm_conn_http_send;
    connection->sub_func = iotx_cm_conn_http_subscribe;
    connection->unsub_func = iotx_cm_conn_http_unsubscribe;
    connection->add_service_func = NULL;
    connection->remove_service_func = NULL;
    connection->add_subdevice_func = NULL;
    connection->remove_subdevice_func = NULL;
    connection->yield_func = iotx_cm_conn_http_yield;
    connection->protocol_type = IOTX_CM_CONNECTION_PROTOCOL_TYPE_HTTP;
    connection->event_handler = iotx_cm_cloud_conn_event_callback;
    connection->event_pcontext = handler;
    connection->response_handler = iotx_cm_cloud_conn_response_callback;

    return connection;
}
#endif

void *iotx_cm_cloud_conn_init(void *handler, void *param)
{
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)handler;
    iotx_cm_connectivity_t *connectivity = NULL;
    iotx_device_info_t *device_info = NULL;
    iotx_cm_connection_t *connection = NULL;
    iotx_cm_connectivity_cloud_param_t *cloud_param = (iotx_cm_connectivity_cloud_param_t *)param;

    switch (cloud_param->protocol_type) {
        case IOTX_CM_CONNECTION_PROTOCOL_TYPE_MQTT:
            connection = _mqtt_init(cm_ctx);
            break;

        case IOTX_CM_CONNECTION_PROTOCOL_TYPE_COAP:
            connection = _coap_init(cm_ctx);
            break;

#ifdef HTTP_COMM_ENABLED
        case IOTX_CM_CONNECTION_PROTOCOL_TYPE_HTTP:
            connection = _http_init(cm_ctx);
            break;
#endif

        default:
            CM_ERR(cm_log_error_protocol, cloud_param->protocol_type);
            break;
    }

    if (connection == NULL) {
        return NULL;
    }

    connectivity = CM_malloc(sizeof(iotx_cm_connectivity_t));
    if (NULL == connectivity) {
        CM_ERR(cm_log_error_memory);
        connection->deinit_func(connection);
        LITE_free(connection);
        return NULL;
    }

    iotx_event_post(IOTX_CONN_CLOUD);

    device_info = iotx_device_info_get();
    memset(&g_cloud_target, 0x0, sizeof(iotx_cm_send_peer_t));
    strncpy(g_cloud_target.product_key, device_info->product_key, strlen(device_info->product_key));
    strncpy(g_cloud_target.device_name, device_info->device_name, strlen(device_info->device_name));

    connectivity->context = connection;
    connectivity->init_func = iotx_cm_cloud_conn_init;
    connectivity->init_second_func = iotx_cm_cloud_conn_init_second;
    connectivity->connect_func = iotx_cm_cloud_conn_connect;
    connectivity->trigger_connected_func = iotx_cm_cloud_conn_trigger_connected;
    connectivity->check_connected_func = iotx_cm_cloud_conn_check_connected;
    connectivity->register_func = iotx_cm_cloud_conn_register;
    connectivity->unregister_func = iotx_cm_cloud_conn_unregister;
    connectivity->add_service_func = iotx_cm_cloud_conn_add_service;
    connectivity->remove_service_func = iotx_cm_cloud_conn_remove_service;
    connectivity->send_func = iotx_cm_cloud_conn_send;
    connectivity->add_subdevice_func = iotx_cm_cloud_conn_add_subdevice;
    connectivity->remove_subdevice_func = iotx_cm_cloud_conn_remove_subdevice;
    connectivity->yield_func = iotx_cm_cloud_conn_yield;
    connectivity->deinit_func = iotx_cm_cloud_conn_deinit;
    connectivity->get_target_func = iotx_cm_cloud_conn_get_target;
    connectivity->check_target_func = iotx_cm_cloud_conn_check_target;

    connectivity->id = iotx_cm_get_connectivity_id();
    connectivity->type = IOTX_CM_CONNECTIVITY_TYPE_CLOUD;
    connectivity->is_try_connect = 0;
#if (CONFIG_SDK_THREAD_COST == 1)
    connectivity->add_send_func = iotx_cm_cloud_conn_add_send;
    connectivity->thread_process_func = iotx_cm_cloud_conn_process;
    connectivity->process_lock = HAL_MutexCreate();

    hal_os_thread_param_t task_parms = {0};
    task_parms.stack_size = 6144;
    task_parms.name = "cm_cloud_conn_process";
    HAL_ThreadCreate(&connectivity->pthread_process, iotx_cm_cloud_conn_process, handler, &task_parms, 0);
#endif

    return connectivity;
}


int iotx_cm_cloud_conn_trigger_connected(void *handler, void *_connectivity, iotx_cm_event_handle_fp_t event_fp,
        void *user_data)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)handler;

    if (NULL == connectivity || NULL == cm_ctx) {
        return FAIL_RETURN;
    }

    connectivity->is_try_connect = 1;

    if ((IOTX_CM_CONNECTIVITY_STATUS_INITED == iotx_cm_get_connectivity_status(connectivity))
        && connectivity->connect_func) {
        connectivity->connect_func(cm_ctx, connectivity);
    }

    if (IOTX_CM_CONNECTIVITY_STATUS_CONNECTED == iotx_cm_get_connectivity_status(connectivity)) {
        iotx_cm_event_msg_t event_msg = {0};
        event_msg.event_id = IOTX_CM_EVENT_CLOUD_CONNECTED;
        event_msg.msg = (void *)connectivity;
        if( event_fp != NULL )
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

int iotx_cm_cloud_conn_check_connected(void *handler, void *_connectivity, iotx_cm_event_handle_fp_t event_fp,
                                       void *user_data)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)handler;

    if (NULL == connectivity || NULL == cm_ctx) {
        return FAIL_RETURN;
    }

    if (IOTX_CM_CONNECTIVITY_STATUS_CONNECTED == iotx_cm_get_connectivity_status(connectivity)) {
        iotx_cm_event_msg_t event_msg = {0};
        event_msg.event_id = IOTX_CM_EVENT_CLOUD_CONNECTED;
        event_msg.msg = (void *)connectivity;
        if( event_fp != NULL )
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


int iotx_cm_cloud_conn_register(void *handler, void *_connectivity, void *register_param, int count)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_cm_connection_t *connection = NULL;

    connection = (iotx_cm_connection_t *)connectivity->context;

    if (IOTX_CM_CONNECTIVITY_STATUS_CONNECTED != connectivity->status) {
        return FAIL_RETURN;
    }

    return connection->sub_func(connection, register_param, count);
}


int iotx_cm_cloud_conn_unregister(void *handler, void *_connectivity, const char *topic_filter)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_cm_connection_t *connection = NULL;

    connection = (iotx_cm_connection_t *)connectivity->context;

    if (IOTX_CM_CONNECTIVITY_STATUS_CONNECTED != connectivity->status) {
        return FAIL_RETURN;
    }

    return connection->unsub_func(connection, topic_filter);
}


int iotx_cm_cloud_conn_add_service(void *handler, void *_connectivity, const char *topic_filter,
                                   iotx_cm_message_auth_types_t auth_type)
{
    return SUCCESS_RETURN;
}


int iotx_cm_cloud_conn_remove_service(void *handler, void *_connectivity, const char *topic_filter)
{
    return SUCCESS_RETURN;
}


int iotx_cm_cloud_conn_send(void *handler,
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
    msg.response_handler = iotx_cm_cloud_conn_response_callback;
    msg.response_pcontext = handler;
    msg.sem = sem;

    return connection->send_func(connection, context, &msg);
}


int iotx_cm_cloud_conn_add_subdevice(void *handler, void *connectivity, const char *pk, const char *dn)
{
    return SUCCESS_RETURN;
}


int iotx_cm_cloud_conn_remove_subdevice(void *handler, void *connectivity, const char *pk, const char *dn)
{
    return SUCCESS_RETURN;
}

int iotx_cm_cloud_conn_init_second(void *handler, void *connectivity)
{
    return SUCCESS_RETURN;
}

int iotx_cm_cloud_conn_yield(void *handler, void *_connectivity, int timeout_ms)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_cm_connection_t *connection = NULL;

    connection = (iotx_cm_connection_t *)connectivity->context;

    if (IOTX_CM_CONNECTIVITY_STATUS_CONNECTED != connectivity->status) {
        return FAIL_RETURN;
    }

    return connection->yield_func(connection, timeout_ms);
}


int iotx_cm_cloud_conn_deinit(void *handler, void *_connectivity)
{
    int ret = FAIL_RETURN;
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    iotx_cm_connection_t *connection = NULL;

#if (CONFIG_SDK_THREAD_COST == 1)
    HAL_ThreadDelete(connectivity->pthread_process);
    HAL_SleepMs(100);
    HAL_MutexDestroy(connectivity->process_lock);
    iotx_cm_free_process_list_handler(connectivity, handler);
#endif /* CONFIG_SDK_THREAD_COST */

    connection = (iotx_cm_connection_t *)connectivity->context;

    if (NULL == connection) {
        return ret;
    }

    if (IOTX_CM_CONNECTIVITY_STATUS_CONNECTED == connectivity->status) {
        ret = connection->deinit_func(connection);
    }

    LITE_free(connection);

    connectivity->context = NULL;

    return ret;
}


iotx_cm_send_peer_t *iotx_cm_cloud_conn_get_target()
{
    return &g_cloud_target;
}


int iotx_cm_cloud_conn_check_target(iotx_cm_send_peer_t *peer)
{
    if ((strlen(g_cloud_target.product_key) == strlen(peer->product_key)) &&
        (strlen(g_cloud_target.device_name) == strlen(peer->device_name)) &&
        0 == strncmp(peer->product_key, g_cloud_target.product_key, strlen(peer->product_key)) &&
        0 == strncmp(peer->device_name, g_cloud_target.device_name, strlen(peer->device_name))) {
        return SUCCESS_RETURN;
    }
    return FAIL_RETURN;
}


#if (CONFIG_SDK_THREAD_COST == 1)
int iotx_cm_cloud_conn_add_send(void *handler, iotx_cm_send_peer_t *target, iotx_cm_message_info_t *message_info,
                                void *sem)
{
    /* send message to itself thread */
    int ret = 0;
    iotx_cm_process_list_node_t *node = NULL;
    iotx_cm_process_send_t *msg = NULL;
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)handler;

    node = iotx_cm_get_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD);
    if (NULL == node) {
        return FAIL_RETURN;
    }

    node->type = IOTX_CM_PROCESS_CLOUD_SEND;

    node->msg = CM_malloc(sizeof(iotx_cm_process_send_t));
    if (NULL == node->msg) {
        CM_ERR(cm_log_error_memory);
        iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
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
        iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
        return FAIL_RETURN;
    }
    strncpy(msg->URI, message_info->URI, strlen(message_info->URI));

    msg->payload = CM_malloc(message_info->payload_length);
    if (NULL == msg->payload) {
        CM_ERR(cm_log_error_memory);
        LITE_free(msg->URI);
        LITE_free(msg->target);
        LITE_free(msg);
        iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
        return FAIL_RETURN;
    }

    memcpy(msg->payload, message_info->payload, message_info->payload_length);
    msg->payload_length = message_info->payload_length;
    msg->ack_type = message_info->ack_type;
    msg->conn_ctx = message_info->conn_ctx;
    msg->sem = sem;

    ret = iotx_cm_process_list_push(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);

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
        iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
    }

    return SUCCESS_RETURN;
}


void *iotx_cm_cloud_conn_process(void *pclient)
{
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)pclient;
    iotx_cm_connectivity_t *connectivity = iotx_cm_find_connectivity_by_type(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD);

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_parameter);
        return NULL;
    }

    CM_INFO(cm_log_info_enter_process_1);

    while (!connectivity) {
        connectivity = iotx_cm_find_connectivity_by_type(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD);
        HAL_SleepMs(1000);
        continue;
    }

    while (connectivity) {
        if (connectivity->is_try_connect
            && IOTX_CM_CONNECTIVITY_STATUS_CONNECTED != iotx_cm_get_connectivity_status(connectivity)) {
            if (SUCCESS_RETURN == iotx_cm_cloud_conn_trigger_connected(cm_ctx, connectivity, NULL, NULL)) {
                iotx_cm_set_connectivity_status(connectivity, IOTX_CM_CONNECTIVITY_STATUS_CONNECTED);
            }
            continue;
        }

        if (iotx_cm_process_list_get_size(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD) > 0) {
            iotx_cm_process_list_node_t *node = NULL;
            if (NULL != (node = iotx_cm_process_list_pop(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD))) {
                CM_INFO(cm_log_info_process_type, node->type);

                switch (node->type) {
                    case IOTX_CM_PROCESS_CONNECT: {
                        if (SUCCESS_RETURN == iotx_cm_cloud_conn_trigger_connected(cm_ctx, connectivity, NULL, NULL)) {
                            iotx_cm_set_connectivity_status(connectivity, IOTX_CM_CONNECTIVITY_STATUS_CONNECTED);
                        }
                    }
                    break;

                    case IOTX_CM_PROCESS_DISCONNECT: {
                        /* todo */
                        /* connectivity must reset to NULL */
                        /* iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node); */
                        /* if (FAIL_RETURN == iotx_cm_cloud_conn_deinit(cm_ctx, connectivity)) {
                            CM_ERR(cm_log_error_fail);
                        } */
                        /* connectivity->status = IOTX_CM_CONNECTIVITY_STATUS_UNCREATED;
                        linked_list_remove(cm_ctx->list_connectivity, connectivity);
                        LITE_free(connectivity); */
                    }
                    break;

                    case IOTX_CM_PROCESS_REGISTER: {
                        int i = 0;
                        iotx_cm_process_register_t *_register = (iotx_cm_process_register_t *)node->msg;
                        iotx_cm_register_param_t **register_param = (iotx_cm_register_param_t **)_register->register_param;
#ifdef CM_SUPPORT_TOPIC_DISPATCH
                        iotx_cm_mapping_t *mapping = NULL;
                        if (FAIL_RETURN == iotx_cm_add_mapping(cm_ctx,
                                                               IOTX_CM_MAPPING_TYPE_REGISTER,
                                                               register_param->URI,
                                                               register_param->register_func,
                                                               register_param->user_data,
                                                               register_param->mail_box)) {
                            iotx_cm_event_msg_t event;
                            iotx_cm_event_result_t result_pt = {0};

                            result_pt.result = -1;
                            result_pt.URI = _register->URI;
                            event.event_id = IOTX_CM_EVENT_REGISTER_RESULT;
                            event.msg = (void *)&result_pt;

                            CM_ERR(cm_log_error_fail_1);

                            iotx_cm_trigger_event_callback(cm_ctx, &event);
                            LITE_free(register_param->URI);
                            LITE_free(register_param);
                            continue;
                        }

                        mapping = iotx_cm_find_mapping(cm_ctx, IOTX_CM_MAPPING_TYPE_REGISTER, register_param->URI, strlen(register_param->URI));
                        iotx_cm_cloud_conn_register(cm_ctx, connectivity, register_param, _register->count);
#else /* CM_SUPPORT_TOPIC_DISPATCH */
                        iotx_cm_cloud_conn_register(cm_ctx, connectivity, _register->register_param, _register->count);
#endif
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
                        iotx_cm_cloud_conn_unregister(cm_ctx, connectivity, URI);
                        iotx_cm_remove_mapping(cm_ctx, IOTX_CM_MAPPING_TYPE_REGISTER, URI);
                        LITE_free(URI);
                    }
                    break;

                    case IOTX_CM_PROCESS_ADD_SERVICE: {
                        iotx_cm_process_service_t *_service = (iotx_cm_process_service_t *)node->msg;
                        iotx_cm_cloud_conn_add_service(cm_ctx, connectivity, _service->URI, _service->auth_type);
                        LITE_free(_service->URI);
                        LITE_free(_service);
                    }
                    break;

                    case IOTX_CM_PROCESS_REMOVE_SERVICE: {
                        char *URI = (char *)node->msg;
                        iotx_cm_cloud_conn_remove_service(cm_ctx, connectivity, URI);
                        LITE_free(URI);
                    }
                    break;

                    case IOTX_CM_PROCESS_CLOUD_SEND: {
                        iotx_cm_process_send_t *_send = (iotx_cm_process_send_t *)node->msg;
                        if (FAIL_RETURN == iotx_cm_cloud_conn_send(cm_ctx,
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

                        if (_send->URI) {
                            LITE_free(_send->URI);
                        }
                        if (_send->payload) {
                            LITE_free(_send->payload);
                        }
                        if (_send->target) {
                            LITE_free(_send->target);
                        }
                        LITE_free(_send);
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

                    case IOTX_CM_PROCESS_REGISTER_RESULT:
                    case IOTX_CM_PROCESS_UNREGISTER_RESULT: {
                        iotx_cm_process_register_result_t *_register = (iotx_cm_process_register_result_t *)node->msg;
                        if (_register->URI) {
                            iotx_cm_cloud_conn_register_handler(cm_ctx, _register->URI, _register->result, _register->is_register);
                        }
                        LITE_free(_register->URI);
                        LITE_free(_register);
                    }
                    break;

                    case IOTX_CM_PROCESS_ADD_SUBDIVCE: {
                        iotx_cm_process_subdevice_t *subdevice = (iotx_cm_process_subdevice_t *)node->msg;
                        iotx_cm_cloud_conn_add_subdevice(cm_ctx, connectivity, subdevice->pk, subdevice->dn);
                        LITE_free(node->msg);
                    }
                    break;

                    case IOTX_CM_PROCESS_REMOVE_SUBDIVCE: {
                        iotx_cm_process_subdevice_t *subdevice = (iotx_cm_process_subdevice_t *)node->msg;
                        iotx_cm_cloud_conn_remove_subdevice(cm_ctx, connectivity, subdevice->pk, subdevice->dn);
                        LITE_free(node->msg);
                    }
                    break;

                    case IOTX_CM_PROCESS_CLOUD_NEW_DATA: {
                        iotx_cm_message_info_t *message_info = (iotx_cm_message_info_t *)node->msg;
                        iotx_cm_cloud_conn_response_handler(cm_ctx, message_info);
                        LITE_free(message_info->URI);
                        LITE_free(message_info->payload);
                        LITE_free(node->msg);
                        break;
                    }
                    case IOTX_CM_PROCESS_LOCAL_CLOUD_INIT: {
                        iotx_cm_cloud_conn_init_second(cm_ctx, connectivity);
                    }
                    break;
                    default:
                        break;
                }
                CM_ERR("3. Close Connectivity Node: %p",node);
                iotx_cm_free_list_node(cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
            }
        }
        /* cloud yield */
        if (connectivity && IOTX_CM_CONNECTIVITY_STATUS_CONNECTED == iotx_cm_get_connectivity_status(connectivity)) {
            connectivity->yield_func(cm_ctx, connectivity, 50);
        } else {
            HAL_SleepMs(50);
        }
    }

    return NULL;
}
#endif /* CONFIG_SDK_THREAD_COST */

#endif /* CONFIG_CM_VIA_CLOUD_CONN */

