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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "iot_import.h"
#include "iot_export.h"

#define IOTX_PRODUCT_KEY        "yfTuLfBJTiL"
#define IOTX_DEVICE_NAME        "TestDeviceForDemo"
#define IOTX_DEVICE_SECRET      "fSCl9Ns5YPnYN8Ocg0VEel1kXFnRlV6c"
#define IOTX_DEVICE_ID          "IoTxHttpTestDev_001"

    
/* These are pre-defined topics */
#define TOPIC_UPDATE            "/"IOTX_PRODUCT_KEY"/"IOTX_DEVICE_NAME"/update"
#define TOPIC_ERROR             "/"IOTX_PRODUCT_KEY"/"IOTX_DEVICE_NAME"/update/error"
#define TOPIC_GET               "/"IOTX_PRODUCT_KEY"/"IOTX_DEVICE_NAME"/get"
#define TOPIC_DATA              "/"IOTX_PRODUCT_KEY"/"IOTX_DEVICE_NAME"/data"

#define COAP_TOPIC_DATA         "/topic/"IOTX_PRODUCT_KEY"/"IOTX_DEVICE_NAME"/data"

#define MQTT_MSGLEN             (1024)

#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)
        
        
static int _iotx_set_devinfo(iotx_device_info_t *p_devinfo)
{
    if (NULL == p_devinfo) {
        return IOTX_ERR_INVALID_PARAM;
    }

    memset(p_devinfo, 0x00, sizeof(iotx_device_info_t));
    HAL_GetDeviceID(p_devinfo->device_id);
    EXAMPLE_TRACE("device_id %s",p_devinfo->device_id);
    HAL_GetProductKey(p_devinfo->product_key);
    EXAMPLE_TRACE("product_key %s",p_devinfo->product_key);
    HAL_GetDeviceSecret(p_devinfo->device_secret);
    EXAMPLE_TRACE("device_secret %s",p_devinfo->device_secret);
    HAL_GetDeviceName(p_devinfo->device_name);
    EXAMPLE_TRACE("device_name %s",p_devinfo->device_name);
    return IOTX_SUCCESS;
}


static void _event_handle(void *pconnection, iotx_cloud_connection_event_msg_pt msg)
{
    EXAMPLE_TRACE("_event_handle type %d", msg->event_id);
}

static void _response_handle(void *pcontext, void *pconnection, iotx_cloud_connection_msg_rsp_pt msg)
{
    EXAMPLE_TRACE("_response_handle type %d", msg->rsp_type);

    switch(msg->rsp_type) {
        case IOTX_CLOUD_CONNECTION_RESPONSE_SUBSCRIBE_SUCCESS: {
            EXAMPLE_TRACE("subscribe success %s", msg->URI);
        }
            break;
        case IOTX_CLOUD_CONNECTION_RESPONSE_SUBSCRIBE_FAIL:
            EXAMPLE_TRACE("subscribe fail %s", msg->URI);
            break;
        case IOTX_CLOUD_CONNECTION_RESPONSE_UNSUBSCRIBE_SUCCESS:
            EXAMPLE_TRACE("unsubscribe success %s", msg->URI);
            break;
        case IOTX_CLOUD_CONNECTION_RESPONSE_UNSUBSCRIBE_FAIL:
            EXAMPLE_TRACE("unsubscribe fail %s", msg->URI);
            break;
        case IOTX_CLOUD_CONNECTION_RESPONSE_SEND_SUCCESS: 
            EXAMPLE_TRACE("send success topic %s", msg->URI);
            EXAMPLE_TRACE("send success reponse %s", msg->payload);
            break;
        case IOTX_CLOUD_CONNECTION_RESPONSE_SEND_FAIL:
            EXAMPLE_TRACE("send success topic %s", msg->URI);
            break;
        case IOTX_CLOUD_CONNECTION_RESPONSE_NEW_DATA:
            EXAMPLE_TRACE("new data QoS %d", msg->QoS);
            EXAMPLE_TRACE("new data topic %s", msg->URI);
            EXAMPLE_TRACE("new data reponse %s", msg->payload);
            break;
        default:
            break;
    }
}


int http_client(void)
{
    void* handle = NULL;
    int rc = 0;
    iotx_cloud_connection_param_t param = {0};
    iotx_cloud_connection_msg_t msg = {0};
    char msg_pub[128];  
    
    param.device_info = HAL_Malloc(sizeof(iotx_device_info_t));
    if (NULL == param.device_info) {
        EXAMPLE_TRACE("memory error!");         
        return FAIL_RETURN;
    }
    _iotx_set_devinfo(param.device_info); 
    
    param.clean_session = 0;    
    param.keepalive_interval_ms = 60000;
    param.request_timeout_ms = 2000;
    param.protocol_type = IOTX_CLOUD_CONNECTION_PROTOCOL_TYPE_HTTP;
    param.event_handler = _event_handle;
    param.event_pcontext = NULL;
    
    EXAMPLE_TRACE("HTTP init!");   

    handle = IOT_Cloud_Connection_Init(&param);

    if (handle == NULL) {
        EXAMPLE_TRACE("HTTP init fail!");        
        return FAIL_RETURN;
    }

    strcpy(msg_pub, "{\"name\":\"hello world\"}");
    msg.type = IOTX_CLOUD_CONNECTION_MESSAGE_TYPE_PUBLISH;
    msg.QoS = IOTX_MESSAGE_QOS1;
    msg.URI = COAP_TOPIC_DATA;
    msg.URI_length = strlen(msg.URI);
    msg.payload = msg_pub;
    msg.payload_length = strlen(msg_pub);
    msg.response_handler = _response_handle;
    msg.response_pcontext = NULL;
    
    EXAMPLE_TRACE("HTTP send!");       
    rc = IOT_Cloud_Connection_Send_Message(handle, &msg);
    if (FAIL_RETURN == rc) {
        EXAMPLE_TRACE("HTTP send message fail!");  
        IOT_Cloud_Connection_Deinit(&handle);
        return FAIL_RETURN;
    }

    EXAMPLE_TRACE("HTTP deinit!");    
    rc = IOT_Cloud_Connection_Deinit(&handle);
    if (FAIL_RETURN == rc) {
        EXAMPLE_TRACE("HTTP deinit fail!");        
        return FAIL_RETURN;
    }
    return SUCCESS_RETURN;
}


int coap_client(void)
{
    void* handle = NULL;
    int rc = 0;
    iotx_cloud_connection_param_t param = {0};
    iotx_cloud_connection_msg_t msg = {0};
    char msg_pub[128];  
    
    param.device_info = HAL_Malloc(sizeof(iotx_device_info_t));
    if (NULL == param.device_info) {
        EXAMPLE_TRACE("memory error!");         
        return FAIL_RETURN;
    }
    _iotx_set_devinfo(param.device_info); 
    
    param.clean_session = 0;    
    param.keepalive_interval_ms = 60000;
    param.request_timeout_ms = 2000;
    param.protocol_type = IOTX_CLOUD_CONNECTION_PROTOCOL_TYPE_COAP;
    param.event_handler = _event_handle;
    param.event_pcontext = NULL;
    
    EXAMPLE_TRACE("CoAP init!");   

    handle = IOT_Cloud_Connection_Init(&param);

    if (handle == NULL) {
        EXAMPLE_TRACE("CoAP init fail!");        
        return FAIL_RETURN;
    }

    strcpy(msg_pub, "{\"name\":\"hello world\"}");
    msg.type = IOTX_CLOUD_CONNECTION_MESSAGE_TYPE_PUBLISH;
    msg.QoS = IOTX_MESSAGE_QOS1;
    msg.URI = COAP_TOPIC_DATA;
    msg.URI_length = strlen(msg.URI);
    msg.payload = msg_pub;
    msg.payload_length = strlen(msg_pub);
    msg.response_handler = _response_handle;
    msg.response_pcontext = NULL;
    msg.message_type = IOTX_MESSAGE_CON;
    msg.content_type = IOTX_CONTENT_TYPE_JSON;
    
    EXAMPLE_TRACE("CoAP send!");       
    rc = IOT_Cloud_Connection_Send_Message(handle, &msg);
    if (FAIL_RETURN == rc) {
        EXAMPLE_TRACE("CoAP send message fail!");   
        IOT_Cloud_Connection_Deinit(&handle);
        return FAIL_RETURN;
    }

    EXAMPLE_TRACE("CoAP yield!");    

    rc = IOT_Cloud_Connection_Yield(handle, 200);
    if (FAIL_RETURN == rc) {
        EXAMPLE_TRACE("CoAP yield fail!");    
        IOT_Cloud_Connection_Deinit(&handle);
        return FAIL_RETURN;
    }

    EXAMPLE_TRACE("CoAP deinit!");    

    rc = IOT_Cloud_Connection_Deinit(&handle);
    if (FAIL_RETURN == rc) {
        EXAMPLE_TRACE("CoAP deinit fail!");        
        return FAIL_RETURN;
    }
    return SUCCESS_RETURN;
}


int mqtt_client(void)
{
    void* handle = NULL;
    int rc = 0;
    iotx_cloud_connection_param_t param = {0};
    iotx_cloud_connection_msg_t msg = {0};
    char msg_pub[128];    
    
    param.device_info = HAL_Malloc(sizeof(iotx_device_info_t));
    if (NULL == param.device_info) {
        EXAMPLE_TRACE("memory error!");         
        return FAIL_RETURN;
    }
    _iotx_set_devinfo(param.device_info);
    
    param.clean_session = 0;    
    param.keepalive_interval_ms = 60000;
    param.request_timeout_ms = 2000;
    param.protocol_type = IOTX_CLOUD_CONNECTION_PROTOCOL_TYPE_MQTT;
    param.event_handler = _event_handle;
    param.event_pcontext = NULL;

    EXAMPLE_TRACE("MQTT init!");   
    handle = IOT_Cloud_Connection_Init(&param);

    if (handle == NULL) {
        EXAMPLE_TRACE("MQTT init fail!");        
        return FAIL_RETURN;
    }

    msg.type = IOTX_CLOUD_CONNECTION_MESSAGE_TYPE_SUBSCRIBE;
    msg.QoS = IOTX_MESSAGE_QOS1;
    msg.URI = TOPIC_DATA;
    msg.URI_length = strlen(msg.URI);
    msg.payload = NULL;
    msg.payload_length = 0;
    msg.response_handler = _response_handle;
    msg.response_pcontext = NULL;
    EXAMPLE_TRACE("MQTT subscribe!");    
    rc = IOT_Cloud_Connection_Send_Message(handle, &msg);
    if (FAIL_RETURN == rc) {
        EXAMPLE_TRACE("MQTT subscribe fail!"); 
        IOT_Cloud_Connection_Deinit(&handle);
        return FAIL_RETURN;
    }

    EXAMPLE_TRACE("MQTT yield!");    
    rc = IOT_Cloud_Connection_Yield(handle, 200);
    if (FAIL_RETURN == rc) {
        EXAMPLE_TRACE("MQTT yield fail!");       
        IOT_Cloud_Connection_Deinit(&handle);
        return FAIL_RETURN;
    }

    memset(&msg, 0x0, sizeof(iotx_cloud_connection_msg_t));
    strcpy(msg_pub, "message: hello! start!");
    msg.type = IOTX_CLOUD_CONNECTION_MESSAGE_TYPE_PUBLISH;
    msg.QoS = IOTX_MESSAGE_QOS1;
    msg.URI = TOPIC_DATA;
    msg.URI_length = strlen(msg.URI);
    msg.payload = msg_pub;
    msg.payload_length = strlen(msg_pub);
    msg.response_handler = _response_handle;
    msg.response_pcontext = NULL;
    EXAMPLE_TRACE("MQTT publish!");
    rc = IOT_Cloud_Connection_Send_Message(handle, &msg);
    if (FAIL_RETURN == rc) {
        EXAMPLE_TRACE("MQTT subscribe fail!");   
        IOT_Cloud_Connection_Deinit(&handle);       
        return FAIL_RETURN;
    }

    EXAMPLE_TRACE("MQTT yield!");
    rc = IOT_Cloud_Connection_Yield(handle, 200);
    if (FAIL_RETURN == rc) {
        EXAMPLE_TRACE("MQTT yield fail!");   
        IOT_Cloud_Connection_Deinit(&handle);       
        return FAIL_RETURN;
    }

    HAL_SleepMs(2000);

    memset(&msg, 0x0, sizeof(iotx_cloud_connection_msg_t));
    msg.type = IOTX_CLOUD_CONNECTION_MESSAGE_TYPE_UNSUBSCRIBE;
    msg.QoS = IOTX_MESSAGE_QOS1;
    msg.URI = TOPIC_DATA;
    msg.URI_length = strlen(msg.URI);
    msg.payload = NULL;
    msg.payload_length = 0;
    msg.response_handler = _response_handle;
    msg.response_pcontext = NULL;
    EXAMPLE_TRACE("MQTT unsubscribe!");
    rc = IOT_Cloud_Connection_Send_Message(handle, &msg);
    if (FAIL_RETURN == rc) {
        EXAMPLE_TRACE("MQTT subscribe fail!");   
        IOT_Cloud_Connection_Deinit(&handle);       
        return FAIL_RETURN;
    }

    EXAMPLE_TRACE("MQTT yield!");
    rc = IOT_Cloud_Connection_Yield(handle, 200);
    if (FAIL_RETURN == rc) {
        EXAMPLE_TRACE("MQTT yield fail!");     
        IOT_Cloud_Connection_Deinit(&handle);     
        return FAIL_RETURN;
    }

    EXAMPLE_TRACE("MQTT deinit!");
    rc = IOT_Cloud_Connection_Deinit(&handle);
    if (FAIL_RETURN == rc) {
        EXAMPLE_TRACE("MQTT deinit fail!");        
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}


int main(int argc, char **argv)
{
    IOT_OpenLog("cloud-conn");
    IOT_SetLogLevel(IOT_LOG_DEBUG);
    /**< set device info*/
    HAL_SetProductKey(IOTX_PRODUCT_KEY);
    HAL_SetDeviceName(IOTX_DEVICE_NAME);
    HAL_SetDeviceSecret(IOTX_DEVICE_SECRET);
    /**< end*/
    EXAMPLE_TRACE("MQTT!");
    if (SUCCESS_RETURN == mqtt_client())        
        EXAMPLE_TRACE("MQTT success!");
    else 
        EXAMPLE_TRACE("MQTT fail!");
    
    HAL_SleepMs(200);
    
   /* EXAMPLE_TRACE("--------");
    EXAMPLE_TRACE("CoAP!");
    if (SUCCESS_RETURN == coap_client())        
        EXAMPLE_TRACE("CoAP success!");
    else 
        EXAMPLE_TRACE("CoAP fail!");
    
    HAL_SleepMs(200);
    
    EXAMPLE_TRACE("--------");
    EXAMPLE_TRACE("HTTP!");
    if (SUCCESS_RETURN == http_client())        
        EXAMPLE_TRACE("HTTP success!");
    else 
        EXAMPLE_TRACE("HTTP fail!")*/;
    
    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_CloseLog();

    EXAMPLE_TRACE("out of sample!");

    return 0;
}

