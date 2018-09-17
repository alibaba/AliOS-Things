/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifdef COAP_COMM_ENABLED

#include "iot_import.h"
#include "iotx_log.h"
#include "iotx_utils.h"
#include "utils_timer.h"
#include "iotx_system.h"
#include "utils_hmac.h"
#include "iot_export.h"
#include "iot_export_coap.h"
#include "iotx_cm.h"
#include "iotx_cm_connection.h"
#include "iotx_cm_conn_coap.h"

#define IOTX_ONLINE_DTLS_SERVER_URL     "coaps://%s.iot-as-coap.cn-shanghai.aliyuncs.com:5684"

typedef struct iotx_cloud_connection_coap_st {
    iotx_cm_connection_t                          *connection;
    char                                          *URI;
    void                                          *sem;
} iotx_cloud_conn_coap_t;


static void iotx_cm_conn_coap_event_handle(void *context, iotx_coap_event_t event, void *p_data)
{
    CM_INFO(cm_log_info_event_type, event);

    /*switch(event) {
        case IOTX_COAP_EVENT_AUTH_FAILED:
            break;

        case IOTX_COAP_EVENT_RECV_FAILED:{
        }
            break;

        case IOTX_COAP_EVENT_SEND_FAILED:{
        }
            break;

        default:
            CM_ERR(cm_log_error_parameter);
            break;
    }*/
}


static iotx_msg_type_t _type(iotx_cm_message_ack_types_t ack_type)
{
    switch (ack_type) {
        case IOTX_CM_MESSAGE_NO_ACK:
            return IOTX_MESSAGE_NON;

        case IOTX_CM_MESSAGE_NEED_ACK:
            return IOTX_MESSAGE_CON;

        default:
            return IOTX_MESSAGE_NON;
    }
}

static void iotx_cm_connection_coap_response_handler(void *arg, void *p_response)
{
    int len = 0;
    char *payload = NULL;
    iotx_coap_resp_code_t resp_code;
    iotx_cloud_conn_coap_t *coap_pt = (iotx_cloud_conn_coap_t *)arg;
    iotx_connection_msg_rsp_t msg_rsp;

    CM_INFO(cm_log_info_response);

    if (NULL == coap_pt) {
        CM_ERR(cm_log_error_parameter);
        return;
    }

    if (NULL == coap_pt->connection || NULL == coap_pt->URI) {
        CM_ERR(cm_log_error_parameter);
        return;
    }

    memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));

    IOT_CoAP_GetMessageCode(p_response, &resp_code);
    CM_INFO(cm_log_info_response_code, resp_code);
    IOT_CoAP_GetMessagePayload(p_response, (unsigned char **)&payload, &len);
    CM_INFO(cm_log_info_payload_length, len);
    CM_INFO(cm_log_info_payload, payload);

    CM_INFO(cm_log_info_URI, coap_pt->URI);
    msg_rsp.URI = coap_pt->URI;
    msg_rsp.URI_length = strlen(msg_rsp.URI);
    msg_rsp.payload = payload;
    msg_rsp.payload_length = len;

    if (IOTX_COAP_RESP_CODE_CONTENT == resp_code) {
        msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_SEND_SUCCESS;
    } else {
        msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_SEND_FAIL;
    }

    if (coap_pt->connection->response_handler) {
        coap_pt->connection->response_handler(coap_pt->connection->event_pcontext, &msg_rsp);
    }

    if (coap_pt->sem) {
        HAL_SemaphorePost(coap_pt->sem);
    }

    LITE_free(coap_pt->URI);
    LITE_free(coap_pt);
    coap_pt = NULL;
}


void *iotx_cm_conn_coap_init(void *handle, void *init_param)
{
    iotx_coap_config_t config = {0};
    char url[256] = {0};
    iotx_coap_context_t *p_ctx = NULL;
    char product_key[PRODUCT_KEY_LEN + 1];

    if (FAIL_RETURN == HAL_GetProductKey(product_key)) {
        CM_ERR(cm_log_error_fail);
        return NULL;
    }

    /* set config */
    memset(&config, 0x00, sizeof(iotx_coap_config_t));
    HAL_Snprintf(url, sizeof(url), IOTX_ONLINE_DTLS_SERVER_URL, (char *)product_key);
    config.p_url = url;

    config.p_devinfo = iotx_device_info_get();
    config.wait_time_ms = 2000;
    config.event_handle = iotx_cm_conn_coap_event_handle;

    /* initial */
    p_ctx = IOT_CoAP_Init(&config);
    if (NULL == p_ctx) {
        CM_ERR(cm_log_error_fail);
        return NULL;
    }

    /* auth */
    if (IOT_CoAP_DeviceNameAuth(p_ctx)) {
        CM_ERR(cm_log_error_fail);
        IOT_CoAP_Deinit(&p_ctx);
        return NULL;
    }

    return p_ctx;
}

int iotx_cm_conn_coap_subscribe(void *handle, void *_register_param, int count)
{
    CM_INFO(cm_log_info_not_support);
    return SUCCESS_RETURN;
}


int iotx_cm_conn_coap_unsubscribe(void *handle, const char *topic_filter)
{
    CM_INFO(cm_log_info_not_support);
    return SUCCESS_RETURN;
}


int iotx_cm_conn_coap_send(void *handle, void *_context, iotx_connection_msg_t *message)
{
    iotx_message_t coap_message;
    iotx_cloud_conn_coap_t *coap_pt = NULL;
    iotx_cm_connection_t *connection = (iotx_cm_connection_t *)handle;

    if (NULL == message) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    coap_pt = LITE_malloc(sizeof(iotx_cloud_conn_coap_t));
    if (NULL == coap_pt) {
        CM_ERR(cm_log_error_memory);
        return FAIL_RETURN;
    }
    memset(coap_pt, 0x0, sizeof(iotx_cloud_conn_coap_t));

    coap_pt->URI = LITE_malloc(message->URI_length + 1);
    if (NULL == coap_pt) {
        CM_ERR(cm_log_error_memory);
        LITE_free(coap_pt);
        return FAIL_RETURN;
    }
    memset(coap_pt->URI, 0x0, message->URI_length + 1);
    strncpy(coap_pt->URI, message->URI, message->URI_length);
    coap_pt->connection = connection;

    coap_message.p_payload = (unsigned char *)message->payload;
    coap_message.payload_len = message->payload_length;
    coap_message.msg_type = _type(message->ack_type);
    /*  sem for send_sync */
    if (IOTX_MESSAGE_CON == coap_message.msg_type) {
        coap_pt->sem = message->sem;
    } else {
        if (coap_pt->sem) {
            HAL_SemaphorePost(coap_pt->sem);
        }
    }
    coap_message.content_type = message->content_type;
    coap_message.user_data = (void *)coap_pt;
    coap_message.resp_callback = iotx_cm_connection_coap_response_handler;

    return IOT_CoAP_SendMessage(connection->context, message->URI, &coap_message);
}

int iotx_cm_conn_coap_deinit(void *handle)
{
    iotx_cm_connection_t *connection = (iotx_cm_connection_t *)handle;

    IOT_CoAP_Deinit(&(connection->context));

    return  SUCCESS_RETURN;
}

int iotx_cm_conn_coap_yield(void *handle, int timeout_ms)
{
    iotx_cm_connection_t *connection = (iotx_cm_connection_t *)handle;

    return IOT_CoAP_Yield(connection->context);
}

#endif /* COAP_COMM_ENABLED */
