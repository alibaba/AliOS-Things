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

#include "iot_import.h"

#include "lite-utils.h"
#include "lite-system.h"
#include "iot_export.h"
#include "iotx_cm_common.h"
#include "iotx_cm_ota.h"

static iotx_cm_conntext_t* g_cm_ctx = NULL;

extern void guider_set_domain_type(int domain_type);
#ifdef CM_VIA_CLOUD_CONN
extern iotx_cm_send_peer_t* iotx_cm_cloud_conn_get_target();
#endif /* CM_VIA_CLOUD_CONN */


static void cm_trigger_connected_event_handler(void* list_node, va_list* params)
{
    iotx_cm_connectivity_t* connectivity = (iotx_cm_connectivity_t*)list_node;
    iotx_cm_conntext_t* cm_ctx;
    iotx_cm_init_param_t* init_param;

    cm_ctx = va_arg(*params, iotx_cm_conntext_t*);
    init_param = va_arg(*params, iotx_cm_init_param_t*);

    assert(cm_ctx && init_param);

    if (connectivity->is_connected) {
        iotx_cm_event_msg_t event_msg = {0};
        event_msg.event_id = IOTX_CM_EVENT_CLOUD_CONNECTED;
        event_msg.msg = NULL;
        init_param->event_func(g_cm_ctx, &event_msg, init_param->user_data);
    }
}

/**
 * @brief CM init
 *        This function initialize the CM structures, establish network connection
 *        If CM has been initialized, this function will return success directly.
 *
 * @param pparam, specify the cm and event handler.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int IOT_CM_Init(iotx_cm_init_param_t* init_param, void* option)
{
    iotx_conn_info_t* pconn_info;
    iotx_cm_event_cb_usr_ctx_t* cb_usr_ctx = NULL;

    char product_key[PRODUCT_KEY_LEN + 1] = {0};
#ifdef SUPPORT_PRODUCT_SECRET
    char product_secret[DEVICE_SECRET_LEN + 1] = {0};
#endif
    char device_name[DEVICE_NAME_LEN + 1] = {0};
    char device_secret[DEVICE_SECRET_LEN + 1] = {0};
    char device_id[DEVICE_ID_LEN + 1] = {0};

    if (NULL == init_param || init_param->user_data == NULL || init_param->event_func == NULL) return FAIL_RETURN;

    if (NULL == g_cm_ctx) {
        HAL_GetProductKey(product_key);
        HAL_GetDeviceName(device_name);
        HAL_GetDeviceID(device_id);

#ifdef SUPPORT_PRODUCT_SECRET
        /* product secret */
        if (IOTX_CM_DEVICE_SECRET_PRODUCT == pparam->secret_type) {
            HAL_GetDeviceSecret(device_secret);
            if (strlen(device_secret) != 0) {
                return SUCCESS_RETURN;
            }

            HAL_GetProductSecret(product_secret);
            if (strlen(product_secret) == 0) {
                CM_ERR(cm_log_error_secret_1);
                return FAIL_RETURN;
            }

            /* auth */
            if (FAIL_RETURN == iotx_cm_auth(product_key, device_name, device_id)) {
                CM_ERR(cm_log_error_auth);
                return FAIL_RETURN;
            }
        }
#endif /* SUPPORT_PRODUCT_SECRET*/

        HAL_GetDeviceSecret(device_secret);
        assert(strlen(device_name) && strlen(product_key) && strlen(device_id) && strlen(device_secret));

        /* Device AUTH */
        if (0 != IOT_SetupConnInfo(product_key, device_name, device_secret, (void **)&pconn_info)) {
            CM_ERR(cm_log_error_auth);
            return FAIL_RETURN;
        }

        CM_INFO(cm_log_info_init);
        g_cm_ctx = CM_malloc(sizeof(iotx_cm_conntext_t));
        if (NULL == g_cm_ctx) {
            CM_ERR(cm_log_error_memory);
            return FAIL_RETURN;
        }
        memset(g_cm_ctx, 0x0, sizeof(iotx_cm_conntext_t));

        g_cm_ctx->response_func = iotx_cm_response_func;
        guider_set_domain_type(init_param->domain_type);
    }

    if (g_cm_ctx->list_event_callback == NULL) {
        g_cm_ctx->list_event_callback = linked_list_create("cm event cb list", 0);
    }

    if (g_cm_ctx->list_connectivity == NULL) {
        g_cm_ctx->list_connectivity = linked_list_create("cm connectivity list", 0);
    }

    cb_usr_ctx = CM_malloc(sizeof(iotx_cm_event_cb_usr_ctx_t));
    if (NULL == cb_usr_ctx) {
        CM_ERR(cm_log_error_memory);
        goto exit;
    }

    cb_usr_ctx->event_func = init_param->event_func;
    cb_usr_ctx->user_data = init_param->user_data;

    if (g_cm_ctx->list_event_callback) {
        linked_list_insert(g_cm_ctx->list_event_callback, cb_usr_ctx);
    }

    if (g_cm_ctx->inited == 0) {
         if (FAIL_RETURN == iotx_cm_add_connectivity_all(g_cm_ctx, init_param)) goto exit;
         g_cm_ctx->inited = 1;
    } else {
         linked_list_iterator(g_cm_ctx->list_connectivity, cm_trigger_connected_event_handler, g_cm_ctx, init_param);
    }

    return SUCCESS_RETURN;

exit:
    CM_ERR(cm_log_error_fail);
    IOT_CM_Deinit(g_cm_ctx);

    return FAIL_RETURN;
}

#ifdef SERVICE_OTA_ENABLED
int IOT_CM_OTA_Start(char* cur_version, void* option)
{
    iotx_cm_connectivity_t* connectivity = NULL;
    iotx_cm_conntext_t* cm_ctx = g_cm_ctx;

    assert(cm_ctx && cur_version);

    /* Do not bind with cloud conn */
    connectivity = iotx_cm_find_connectivity(cm_ctx, iotx_cm_cloud_conn_get_target(), NULL);

    if (connectivity == NULL || connectivity->is_connected == 0) {
        CM_WARNING(cm_log_warning_cloud_disconnected);
        return FAIL_RETURN;
    }

    /* ota init */
    cm_ctx->ota_handler = iotx_cm_ota_init(cm_ctx, cur_version);
    if (NULL == cm_ctx->ota_handler) return FAIL_RETURN;

    return SUCCESS_RETURN;
}


int IOT_CM_OTA_Set_Callback(iotx_cm_ota_types_t type, void* ota_func, void* user_context, void* option)
{
    void** _ota_func = NULL;
    void** _user_context = NULL;
    iotx_cm_conntext_t* cm_ctx = g_cm_ctx;

    if (NULL == cm_ctx || NULL == cm_ctx->ota_handler) {
        CM_ERR(cm_log_error_status);
        return FAIL_RETURN;
    }

    if (NULL == ota_func) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    switch(type) {
    case IOTX_CM_OTA_TYPE_FOTA:
        _ota_func = (void**)&cm_ctx->fota_func;
        _user_context = (void**)&cm_ctx->fota_user_context;
        break;

    case IOTX_CM_OTA_TYPE_COTA:
        _ota_func = (void**)&cm_ctx->cota_func;
        _user_context = (void**)&cm_ctx->cota_user_context;
        break;

    default:
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
        break;
    }

    if (_ota_func) {
        CM_WARNING(cm_log_warning_ota_started);
        return FAIL_RETURN;
    }

    *_ota_func = ota_func;
    *_user_context = user_context;

    return SUCCESS_RETURN;
}

int IOT_CM_OTA_Get_Config(const char* configScope, const char* getType, const char* attributeKeys, void* option)
{
    iotx_cm_conntext_t* cm_ctx = g_cm_ctx;

    if (NULL == cm_ctx || cm_ctx->ota_handler) {
        CM_ERR(cm_log_error_status);
        return FAIL_RETURN;
    }

    return iotx_cm_ota_get_config(cm_ctx, configScope, getType, attributeKeys);
}


int IOT_CM_OTA_Request_Image(const char* version, void* option)
{
    iotx_cm_conntext_t* cm_ctx = g_cm_ctx;

    if (NULL == cm_ctx || cm_ctx->ota_handler) {
        CM_ERR(cm_log_error_status);
        return FAIL_RETURN;
    }

    return iotx_cm_ota_request_image(cm_ctx, version);
}
#endif /* SERVICE_OTA_ENABLED */


/**
 * @brief Register service.
 *        This function used to register some service by different URI, set the URI's callback.
 *        If it is received a request, will callback the register_cb.
 *        If there is no match register_cb (user have not register the service set callback), the request will be discard.
 *
 * @param register_param, register parameter, include URI and register callback.
 * @param option, reserve.
 *
 * @return success or fail.
 *          This API not support one URI register twice, if the URI have been register, it will return fail.
 */
int IOT_CM_Register(iotx_cm_register_param_t* register_param, void* option)
{
    int rc = 0;
    iotx_cm_conntext_t* cm_ctx = g_cm_ctx;

    assert(cm_ctx && register_param && register_param->URI);

    if (NULL != iotx_cm_find_mapping(cm_ctx, register_param->URI, strlen(register_param->URI))) {
        CM_INFO(cm_log_info_registered);
        return FAIL_RETURN;
    }

#ifdef CM_SUPPORT_MULTI_THREAD
    {
        /* send message to itself thread */
        iotx_cm_process_list_node_t* node = NULL;
        iotx_cm_process_register_t* msg = NULL;
        char* URI = NULL;
        int length = strlen(register_param->URI);

        URI = CM_malloc(length + 1);
        strcpy(URI, register_param->URI);

        node = iotx_cm_get_list_node(g_cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD);
        if (NULL == node) {
            CM_ERR(cm_log_error_get_node);
            LITE_free(URI);
            return FAIL_RETURN;
        }

        node->type = IOTX_CM_PROCESS_CLOUD_REGISTER;
        node->msg = CM_malloc(sizeof(iotx_cm_process_register_t));
        if (NULL == node->msg) {
            CM_ERR(cm_log_error_memory);
            LITE_free(URI);
            iotx_cm_free_list_node(g_cm_ctx, node);
            return FAIL_RETURN;
        }

        msg = node->msg;
        msg->URI = URI;
        msg->type = register_param->message_type;
        msg->register_func = register_param->register_func;
        msg->user_data = register_param->user_data;
        msg->mail_box = register_param->mail_box;

        rc = iotx_cm_process_list_push(g_cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
        if (FAIL_RETURN == rc) {
            CM_ERR(cm_log_error_push_node);
            LITE_free(URI);
            LITE_free(node->msg);
            iotx_cm_free_list_node(g_cm_ctx, node);
        }
    }
#else /* CM_SUPPORT_MULTI_THREAD */
    rc = iotx_cm_register_service(cm_ctx, register_param->URI, register_param->message_type, register_param->register_func, register_param->user_data, register_param->mail_box);
#endif /* CM_SUPPORT_MULTI_THREAD */

    return rc;
}


/**
 * @brief Unregister service.
 *        This function used to unregister some service by different URI
 *
 * @param unregister_param, unregister parameter, include URI.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int IOT_CM_Unregister(iotx_cm_unregister_param_t* unregister_param, void* option)
{
    int ret = 0;

    iotx_cm_conntext_t* cm_ctx = g_cm_ctx;

    assert(cm_ctx && unregister_param && unregister_param->URI);

#ifdef CM_SUPPORT_MULTI_THREAD
    {
        /* send message to itself thread */
        iotx_cm_process_list_node_t* node = NULL;
        char* URI = NULL;
        int length = strlen(unregister_param->URI);

        URI = CM_malloc(length + 1);
        strcpy(URI, unregister_param->URI);

        node = iotx_cm_get_list_node(g_cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD);
        if (NULL == node) {
            CM_ERR(cm_log_error_get_node);
            LITE_free(URI);
            return FAIL_RETURN;
        }

        node->type = IOTX_CM_PROCESS_CLOUD_UNREGISTER;
        node->msg = (void*)URI;

        ret = iotx_cm_process_list_push(g_cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_CLOUD, node);
        if (FAIL_RETURN == ret) {
            CM_ERR(cm_log_error_push_node);
            LITE_free(URI);
            iotx_cm_free_list_node(g_cm_ctx, node);
        }
    }

#else /* CM_SUPPORT_MULTI_THREAD */
    ret = iotx_cm_unregister_service(cm_ctx, unregister_param->URI);
#endif /* CM_SUPPORT_MULTI_THREAD */

    return ret;
}


/**
 * @brief Register service.
 *        This function used to register some service by different URI, set the URI's callback.
 *        If it is received a request, will callback the register_cb.
 *        If there is no match register_cb (user have not register the service set callback), the request will be discard.
 *
 * @param service_param, register parameter, include URI and register callback.
 * @param option, reserve.
 *          This API not support one URI register twice, if the URI have been register, it will return fail.
 *
 * @return success or fail.
 */
int IOT_CM_Add_Service(iotx_cm_add_service_param_t* service_param, void* option)
{
    int ret = 0;
    iotx_cm_conntext_t* cm_ctx = g_cm_ctx;

    assert(cm_ctx && service_param && service_param->URI);

    if (NULL != iotx_cm_find_mapping(cm_ctx, service_param->URI, strlen(service_param->URI))) {
        CM_INFO(cm_log_info_registered_1);
        return FAIL_RETURN;
    }

#ifdef CM_SUPPORT_MULTI_THREAD
    {
        /* send message to itself thread */
        iotx_cm_process_list_node_t* node = NULL;
        iotx_cm_process_service_t* msg = NULL;
        char* URI = NULL;
        int length = strlen(service_param->URI);

        URI = CM_malloc(length + 1);
        strcpy(URI, service_param->URI);

        if (NULL == (node = iotx_cm_get_list_node(g_cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL))) {
            LITE_free(URI);
            return FAIL_RETURN;
        }

        node->type = IOTX_CM_PROCESS_LOCAL_ADD_SERVICE;
        if(NULL == (node->msg = CM_malloc(sizeof(iotx_cm_process_service_t)))) {
            LITE_free(URI);
            iotx_cm_free_list_node(g_cm_ctx, node);
            return FAIL_RETURN;
        }

        msg = node->msg;
        msg->URI = URI;
        msg->type = service_param->message_type;
        msg->auth_type = service_param->auth_type;
        msg->register_func = service_param->register_func;
        msg->user_data = service_param->user_data;
        msg->mail_box = service_param->mail_box;

        if (FAIL_RETURN == iotx_cm_process_list_push(g_cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node)) {
            LITE_free(URI);
            LITE_free(node->msg);
            iotx_cm_free_list_node(g_cm_ctx, node);
        }
    }
#else /* CM_SUPPORT_MULTI_THREAD */
    ret = iotx_cm_add_service(cm_ctx, service_param->URI, service_param->message_type, service_param->auth_type,
                              service_param->register_func, service_param->user_data, service_param->mail_box);
#endif /* CM_SUPPORT_MULTI_THREAD */

    return ret;
}


/**
 * @brief Remove service.
 *        This function used to remove some service by different URI
 *
 * @param pparam, remove parameter, include URI.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int IOT_CM_Remove_Service(iotx_cm_remove_service_param_t* service_param, void* option)
{
    int ret = 0;
    iotx_cm_conntext_t* cm_ctx = g_cm_ctx;

    assert(cm_ctx && service_param && service_param->URI);

#ifdef CM_SUPPORT_MULTI_THREAD
    {
        char* URI = NULL;
        /* send message to itself thread */
        iotx_cm_process_list_node_t* node = NULL;

        int length = strlen(service_param->URI);
        URI = CM_malloc(length + 1);
        strcpy(URI, service_param->URI);

        node = iotx_cm_get_list_node(g_cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL);

        if (NULL == node) {
            LITE_free(URI);
            return FAIL_RETURN;
        }

        node->type = IOTX_CM_PROCESS_LOCAL_REMOVE_SERVICE;
        node->msg = (void*)URI;

        ret = iotx_cm_process_list_push(g_cm_ctx, IOTX_CM_CONNECTIVITY_TYPE_LOCAL, node);

        if (FAIL_RETURN == ret) {
            LITE_free(URI);
            iotx_cm_free_list_node(g_cm_ctx, node);
        }
    }

#else /* CM_SUPPORT_MULTI_THREAD */
    ret = iotx_cm_remove_service(cm_ctx, service_param->URI);
#endif /* CM_SUPPORT_MULTI_THREAD */

    return ret;
}

#ifdef CM_SUPPORT_MULTI_THREAD
static void cm_connectivity_add_send_handler(void* list_node, va_list* params)
{
    iotx_cm_connectivity_t* connectivity = (iotx_cm_connectivity_t*)list_node;
    iotx_cm_conntext_t* cm_ctx;
    iotx_cm_send_peer_t* send_peer;
    iotx_cm_message_info_t* message_info;

    cm_ctx = va_arg(*params, iotx_cm_conntext_t*);
    send_peer = va_arg(*params, iotx_cm_send_peer_t*);
    message_info = va_arg(*params, iotx_cm_message_info_t*);

    assert(cm_ctx);

    if (connectivity && connectivity->add_send_func) {
        connectivity->add_send_func(cm_ctx, send_peer, message_info);
    }
}
#endif


/**
 * @brief Send data.
 *        This function used to send data to target.
 *        If the target is NULL, the data will broadcast to all the reachable places.
 *        If the target is not NULL, the data will send to target only.
 *        If the target's product_key and device_name is itself, the data will send to cloud.
 *
 * @param target.
 * @param message_info.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int IOT_CM_Send(iotx_cm_send_peer_t* send_peer, iotx_cm_message_info_t* message_info, void* option)
{
    int rc = 0;
    iotx_cm_connectivity_t* connectivity = NULL;
    iotx_cm_conntext_t* cm_ctx = g_cm_ctx;
#ifndef CM_SUPPORT_MULTI_THREAD
    char* URI;
    void* payload;
    int payload_length = 0;
#endif /* CM_SUPPORT_MULTI_THREAD */

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_status);
        return FAIL_RETURN;
    }

    if (NULL == message_info || NULL == message_info->URI) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    connectivity = iotx_cm_find_connectivity(cm_ctx, send_peer, message_info->conn_ctx);
#ifdef CM_SUPPORT_MULTI_THREAD
    {
        if (connectivity) {
            connectivity->add_send_func(g_cm_ctx, send_peer, message_info);
        } else {
            linked_list_iterator(g_cm_ctx->list_connectivity, cm_connectivity_add_send_handler, g_cm_ctx, send_peer, message_info);
        }
    }
#else /* CM_SUPPORT_MULTI_THREAD */
    if (FAIL_RETURN == iotx_cm_parse_message(cm_ctx, message_info, &URI, &payload, &payload_length)) {
        CM_ERR(cm_log_error_memory);
        return FAIL_RETURN;
    }

    rc = iotx_cm_send_data(cm_ctx, send_peer, connectivity, URI, message_info->ack_type, payload, payload_length, message_info->conn_ctx);
#endif /* CM_SUPPORT_MULTI_THREAD */

    return rc;
}


#ifndef CM_SUPPORT_MULTI_THREAD
/**
 * @brief Send data.
 *        This function used to send data to target and wait for response.
 *        If the target is NULL, the data will broadcast to all the reachable places.
 *        If the target is not NULL, the data will send to target only.
 *        If the target's product_key and device_name is itself, the data will send to cloud.
 *
 * @param target.
 * @param message_info [in/out].
 * @param option, reserve.
 *
 * @return success or fail.
 */
int IOT_CM_Send_Sync(iotx_cm_send_peer_t* send_peer, iotx_cm_message_info_t* message_info, void* option)
{
    iotx_cm_conntext_t* cm_ctx = g_cm_ctx;
    if (NULL == cm_ctx){
        CM_ERR(cm_log_error_status);
        return FAIL_RETURN;
    }

    CM_WARNING(cm_log_warning_not_support);

    return SUCCESS_RETURN;
}


static void cm_connectivity_yield_handler(void* list_node, va_list* params)
{
    iotx_cm_connectivity_t* connectivity = (iotx_cm_connectivity_t*)list_node;
    iotx_cm_conntext_t* cm_ctx;
    int timeout_ms;

    cm_ctx = va_arg(*params, iotx_cm_conntext_t*);
    timeout_ms = va_arg(*params, int);

    assert(cm_ctx);

    if (connectivity && connectivity->yield_func) {
        if (connectivity->is_connected == 0) {
            connectivity->connect_func(cm_ctx, connectivity);
            connectivity->trigger_connected_func(cm_ctx, connectivity, NULL, NULL);
        } else {
            connectivity->yield_func(connectivity, timeout_ms);
        }
    }
}

/**
 * @brief Yield.
 *        This function used to yield when want to received data.
 *        This function just need used in CM_SUPPORT_MULTI_THREAD = n.
 *        If the CM_SUPPORT_MULTI_THREAD = y, this function is no need.
 *
 * @param target.
 * @param message_info.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int IOT_CM_Yield(int timeout_ms, void* option)
{
    linked_list_t* list = NULL;
    iotx_cm_conntext_t* cm_ctx = g_cm_ctx;

    assert(cm_ctx);

    list = cm_ctx->list_connectivity;
    linked_list_iterator(list, cm_connectivity_yield_handler, cm_ctx, timeout_ms / list->_size);

    return SUCCESS_RETURN;
}
#endif /* CM_SUPPORT_MULTI_THREAD */

static void free_list_event_callback(void* _cb_usr_ctx, va_list* params)
{
    void* cb_usr_ctx = _cb_usr_ctx;

    assert(cb_usr_ctx);

    LITE_free(cb_usr_ctx);
}


/**
 * @brief deinit
 *
 * @param option, reserve.
 *
 * @return success or fail.
 */
int IOT_CM_Deinit(void* option)
{
    iotx_cm_conntext_t* cm_ctx = g_cm_ctx;
    linked_list_t* list;

    if (NULL == cm_ctx){
        CM_ERR(cm_log_error_status);
        return FAIL_RETURN;
    }

    /* stop thread */
#ifdef CM_SUPPORT_MULTI_THREAD
    g_cm_ctx->thread_stop = 1;
    HAL_SleepMs(1000);
#endif /* CM_SUPPORT_MULTI_THREAD */

    iotx_cm_remove_mapping_all(cm_ctx);

#ifdef CM_SUPPORT_MULTI_THREAD
    iotx_cm_free_list_node_all(g_cm_ctx);
#endif /* CM_SUPPORT_MULTI_THREAD */

    iotx_cm_remove_connectivity_all(cm_ctx);

#ifdef SERVICE_OTA_ENABLED
    if (cm_ctx->ota_handler) {
        iotx_cm_ota_deinit(cm_ctx);
    }
#endif /* SERVICE_OTA_ENABLED */

    list = cm_ctx->list_event_callback;

    linked_list_iterator(list, free_list_event_callback);

    linked_list_clear(list);
    linked_list_destroy(list);

    list = cm_ctx->list_connectivity;

    linked_list_clear(list);
    linked_list_destroy(list);

    LITE_free(cm_ctx);
    g_cm_ctx = 0;

    return SUCCESS_RETURN;
}


#ifdef SERVICE_OTA_ENABLED
int IOT_CM_OTA_Yield(iotx_cm_ota_t* cm_ota)
{
    iotx_cm_conntext_t* cm_ctx = g_cm_ctx;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_status);
        return FAIL_RETURN;
    }

    if (NULL == cm_ota || NULL == cm_ota->buffer) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    return iotx_cm_ota_yield(cm_ctx, cm_ota);
}
#endif /* SERVICE_OTA_ENABLED */
