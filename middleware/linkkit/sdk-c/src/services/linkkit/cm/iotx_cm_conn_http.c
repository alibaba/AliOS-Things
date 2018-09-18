/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#include "iot_import.h"
#include "iotx_log.h"
#include "iotx_utils.h"
#include "utils_timer.h"
#include "iotx_system.h"
#include "utils_hmac.h"
#include "iot_export.h"
#include "iot_export_http.h"
#include "iotx_cm.h"
#include "iotx_cm_connection.h"
#include "iotx_cm_conn_http.h"

#define IOTX_CLOUD_CLIENT_HTTP_RSP_LEN (1024)


void *iotx_cm_conn_http_init(void *handle, void *_param)
{
    void *http_pt = NULL;
    iotx_http_param_t http_param;

    memset(&http_param, 0, sizeof(http_param));

    http_param.device_info = iotx_device_info_get();
    http_param.timeout_ms = 2000;
    http_param.keep_alive = 1;

    http_pt = IOT_HTTP_Init(&http_param);
    if (NULL == http_pt) {
        CM_ERR(cm_log_error_fail);
        return NULL;
    }

    if (IOT_HTTP_DeviceNameAuth(http_pt)) {
        CM_ERR(cm_log_error_fail);
        return NULL;
    }

    return http_pt;
}

int iotx_cm_conn_http_send(void *handle, void *_context, iotx_connection_msg_t *message)
{
    iotx_http_message_param_t msg_param;
    iotx_connection_msg_rsp_t msg_rsp;
    int rc = 0;
    iotx_cm_connection_t *connection = (iotx_cm_connection_t *)handle;

    if (NULL == message) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    memset(&msg_param, 0x0, sizeof(iotx_http_message_param_t));
    msg_param.timeout_ms = 2000;
    msg_param.request_payload = (char *)message->payload;
    msg_param.request_payload_len = message->payload_length;
    msg_param.response_payload = LITE_malloc(IOTX_CLOUD_CLIENT_HTTP_RSP_LEN);
    if (NULL == msg_param.response_payload) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }
    msg_param.response_payload_len = IOTX_CLOUD_CLIENT_HTTP_RSP_LEN;
    msg_param.topic_path = message->URI;

    rc = IOT_HTTP_SendMessage(connection->context, &msg_param);

    memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
    msg_rsp.URI = message->URI;
    msg_rsp.URI_length = message->URI_length;

    if (SUCCESS_RETURN == rc) {
        CM_INFO(cm_log_info_response);

        msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_SEND_SUCCESS;
        msg_rsp.payload = msg_param.response_payload;
        msg_rsp.payload_length = msg_param.response_payload_len;

        //        if (message->response_handler) {
        //            message->response_handler(message->response_pcontext, &msg_rsp);
        //        }
        if (connection->response_handler) {
            connection->response_handler(connection->event_pcontext, &msg_rsp);
        }
    } else {
        CM_INFO(cm_log_error_fail);

        msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_SEND_FAIL;
        msg_rsp.payload = NULL;
        msg_rsp.payload_length = 0;

        //        if (message->response_handler)
        //            message->response_handler(message->response_pcontext, &msg_rsp);
        if (connection->response_handler) {
            connection->response_handler(connection->event_pcontext, &msg_rsp);
        }
    }

    LITE_free(msg_param.response_payload);
    return SUCCESS_RETURN;
}


int iotx_cm_conn_http_subscribe(void *handle, void *_register_param, int count)
{
    CM_INFO(cm_log_info_not_support);
    return SUCCESS_RETURN;
}


int iotx_cm_conn_http_unsubscribe(void *handle, const char *topic_filter)
{
    CM_INFO(cm_log_info_not_support);
    return SUCCESS_RETURN;
}


int iotx_cm_conn_http_deinit(void *handle)
{
    iotx_cm_connection_t *connection = (iotx_cm_connection_t *)handle;

    IOT_HTTP_DeInit(&(connection->context));

    return SUCCESS_RETURN;
}

int iotx_cm_conn_http_yield(void *handle, int timeout_ms)
{
    CM_INFO(cm_log_info_not_support);
    return SUCCESS_RETURN;
}

