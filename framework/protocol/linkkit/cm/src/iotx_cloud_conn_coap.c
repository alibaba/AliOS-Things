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

#ifdef CM_VIA_CLOUD_CONN_COAP

#include "iot_import.h"
#include "lite-log.h"
#include "lite-utils.h"
#include "utils_timer.h"
#include "lite-system.h"
#include "utils_hmac.h"
#include "iot_export.h"
#include "iot_export_coap.h"
#include "iot_export_cm.h"
#include "iotx_cm_connection.h"

#define IOTX_ONLINE_DTLS_SERVER_URL     "coaps://%s.iot-as-coap.cn-shanghai.aliyuncs.com:5684"

typedef struct iotx_cloud_connection_coap_st {
    iotx_connection_t*                          connection;
    char*                                       URI;
} iotx_cloud_conn_coap_t;


static void iotx_cloud_conn_coap_event_handle(void *context, iotx_coap_event_t event, void *p_data)
{    
    log_info("coap event handle %d", event);
    
    switch(event) {
        case IOTX_COAP_EVENT_AUTH_FAILED:
            break;
        
        case IOTX_COAP_EVENT_RECV_FAILED:{
        }
            break;
        
        case IOTX_COAP_EVENT_SEND_FAILED:{
        }
            break;

        default:
            log_info("event type error");
            break;
    }

}

static void iotx_cloud_connection_coap_response_handler(void *arg, void *p_response)
{
    int len = 0;
    char *payload = NULL;
    iotx_coap_resp_code_t resp_code;
    iotx_cloud_conn_coap_t* coap_pt = (iotx_cloud_conn_coap_t*)arg;

    log_info("coap response");

    if (NULL == coap_pt) {
        log_info("parameter error");
        return;
    }
    
    if (NULL == coap_pt->connection || NULL == coap_pt->URI) {
        log_info("coap_pt error");
        return;
    } 
    
    IOT_CoAP_GetMessageCode(p_response, &resp_code);
    log_info("resp_code: %d", resp_code);
    IOT_CoAP_GetMessagePayload(p_response, (unsigned char **)&payload, &len);
    log_info("payload: %d, %s", len, payload);

    if (IOTX_COAP_RESP_CODE_CONTENT == resp_code) {        
        iotx_connection_msg_rsp_t msg_rsp;
    
        log_info("coap response success");
    
        memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
        msg_rsp.rsp_type = IOTX_CLOUD_CONNECTION_RESPONSE_SEND_SUCCESS;
        log_info("coap response success");
        msg_rsp.URI = coap_pt->URI;
        log_info("URI: %s", coap_pt->URI);
        msg_rsp.URI_length = strlen(msg_rsp.URI);
        log_info("URI_length: %d", msg_rsp.URI_length);
        msg_rsp.payload = payload;
        msg_rsp.payload_length = len;
        
        log_info("coap response success");
        
        if (coap_pt->connection->response_handler)
            coap_pt->connection->response_handler(coap_pt->connection, &msg_rsp);

        LITE_free(coap_pt->URI);        
        LITE_free(coap_pt);
        coap_pt = NULL;
    } else {        
        iotx_connection_msg_rsp_t msg_rsp;
    
        log_info("coap response fail");
    
        memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
        msg_rsp.rsp_type = IOTX_CLOUD_CONNECTION_RESPONSE_SEND_FAIL;
        msg_rsp.URI = coap_pt->URI;
        msg_rsp.URI_length = strlen(coap_pt->URI);
        msg_rsp.payload = payload;
        msg_rsp.payload_length = strlen(payload);
        
        if (coap_pt->connection->response_handler)
            coap_pt->connection->response_handler(coap_pt->connection, &msg_rsp);

        LITE_free(coap_pt->URI);        
        LITE_free(coap_pt);    
        coap_pt = NULL;    
    }
}


void* iotx_cloud_conn_coap_init(void* handle)
{
    iotx_coap_config_t      config;
    char url[256] = {0};
    iotx_coap_context_t *p_ctx = NULL;

    log_info("init");

    /* set config */
    memset(&config, 0x00, sizeof(iotx_coap_config_t));
    HAL_Snprintf(url, sizeof(url), IOTX_ONLINE_DTLS_SERVER_URL, (char *)param->device_info->product_key);
    config.p_url = url;

//    config.p_devinfo = param->device_info;    // HAL_GetProduct
    config.wait_time_ms = 2000;
    config.event_handle = iotx_cloud_conn_coap_event_handle;

    log_info("init");
    /* initial */
    p_ctx = IOT_CoAP_Init(&config);
    if (NULL == p_ctx) {
        log_info("coap init fail");
        return NULL;
    }

    log_info("init");

    /* auth */
    if (IOT_CoAP_DeviceNameAuth(p_ctx)){
        log_info("coap auth fail");
        IOT_CoAP_Deinit(&p_ctx);
        return NULL;
    }

    log_info("init");
    
    return p_ctx;
}

int iotx_cloud_conn_coap_subscribe(void* handle, const char *topic_filter, iotx_cm_message_ack_types_t qos)
{
    log_info("coap not support subscribe");
    return SUCCESS_RETURN;
}
                   
int iotx_cloud_conn_coap_unsubscribe(void* handle, const char *topic_filter)
{
    log_info("coap not support unsubscribe");
    return SUCCESS_RETURN;
}
                        
int iotx_cloud_conn_coap_send(void* handle, void* _context, iotx_connection_msg_t message)
{    
    iotx_message_t coap_message;
    iotx_cloud_conn_coap_t* coap_pt = NULL;
    iotx_connection_t* connection = (iotx_connection_t*)handle;

    if (NULL == message) {
        log_info("parameter error");
        return FAIL_RETURN;
    }
        
    coap_pt = LITE_malloc(sizeof(iotx_cloud_conn_coap_t));
    if (NULL == coap_pt) {
        log_info("memory error");
        return FAIL_RETURN;
    }
    memset(coap_pt, 0x0, sizeof(iotx_cloud_conn_coap_t));
    
    coap_pt->URI = LITE_malloc(message->URI_length + 1);
    if (NULL == coap_pt) {
        log_info("memory error");
        LITE_free(coap_pt);
        return FAIL_RETURN;
    }
    memset(coap_pt->URI, 0x0, message->URI_length + 1);
    strncpy(coap_pt->URI, message->URI, message->URI_length);
    log_info("send message %s", coap_pt->URI);
    coap_pt->connection = connection;
    
    coap_message.p_payload = (unsigned char*)message->payload;
    coap_message.payload_len = message->payload_length;
    coap_message.msg_type = message->message_type;
    coap_message.content_type = message->content_type;
    coap_message.user_data = (void*)coap_pt;
    coap_message.resp_callback = iotx_cloud_connection_coap_response_handler;

    return IOT_CoAP_SendMessage(connection->context, message->URI, &coap_message);
}
                        
int iotx_cloud_conn_coap_deinit(void* handle)
{
    iotx_connection_t* connection = (iotx_connection_t*)handle;

    IOT_CoAP_Deinit(&(connection->context));

    return  SUCCESS_RETURN;
}
                        
int iotx_cloud_conn_coap_yield(void* handle, int timeout_ms)
{
    iotx_connection_t* connection = (iotx_connection_t*)handle;

    return IOT_CoAP_Yield(connection->context);
}
                        
#endif /* CM_VIA_CLOUD_CONN_COAP */
