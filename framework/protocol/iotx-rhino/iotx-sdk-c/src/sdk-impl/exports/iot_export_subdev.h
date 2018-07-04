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

#ifndef SRC_SDK_IMPL_EXPORTS_IOT_EXPORT_SUBDEV_H_
#define SRC_SDK_IMPL_EXPORTS_IOT_EXPORT_SUBDEV_H_

#include "iot_export.h"

#define IOT_GATEWAY_YIELD_MAX_COUNT     100
#define IOT_SUBDEVICE_CLIENT_ID_LEN     64


/* option defined for gateway support multi-thread */
/*#define IOT_GATEWAY_SUPPORT_MULTI_THREAD*/

/* Topic maximum length value */
#define GATEWAY_TOPIC_LEN_MAX                256

/* Reply message maximum length value */
#define REPLY_MESSAGE_LEN_MAX                1024

/* The format of session combine topic */
#define TOPIC_SESSION_COMBINE_FMT            "/ext/session/%s/%s/combine/%s"

/* The format of session sub topic */
#define TOPIC_SESSION_SUB_FMT                "/sys/%s/%s/thing/sub/%s"

/* The format of session topo topic */
#define TOPIC_SESSION_TOPO_FMT               "/sys/%s/%s/thing/topo/%s"

/* The format of session config topic */
#define TOPIC_SESSION_CONFIG_FMT             "/sys/%s/%s/thing/config/%s"

/* The format of session found list topic */
#define TOPIC_SESSION_LIST_FOUND_FMT         "/sys/%s/%s/thing/list/%s"

/* The format of system RRPC topic */
#define TOPIC_SYS_RRPC_FMT                   "/sys/%s/%s/rrpc/%s/+"


/* Subdevice    register/login sign method */
typedef enum IOTX_SUBDEV_SIGN_METHOD_TYPES {
    /* HmacSha1 */
    IOTX_SUBDEV_SIGN_METHOD_TYPE_SHA,
    
    /* HmacMd5 */
    IOTX_SUBDEV_SIGN_METHOD_TYPE_MD5,
    
    /* Undefined */
    IOTX_SUBDEV_SIGN_METHOD_TYPE_UNDEFINED,

    /* Maximum number of login sign method */
    IOTX_SUBDEV_SIGN_METHOD_TYPE_MAX
}iotx_subdev_sign_method_types_t;


/* Subdevice    login clean seesion type */
typedef enum IOTX_SUBDEV_CLEAN_SESSION_TYPES {
    /* True */
    IOTX_SUBDEV_CLEAN_SESSION_TRUE,
    
    /* False */
    IOTX_SUBDEV_CLEAN_SESSION_FALSE,
        
    /* Undefined */
    IOTX_SUBDEV_CLEAN_SESSION_UNDEFINED,
    
    /* Maximum number of login clean seesion type */
    IOTX_SUBDEV_CLEAN_SESSION_MAX
}iotx_subdev_clean_session_types_t;


/* Register type */
typedef enum IOTX_SUBDEV_REGISTER_TYPES {
    /* static, via web get device_secert */
    IOTX_SUBDEV_REGISTER_TYPE_STATIC,
    
    /* dynamic, via register API get device_secert */
    IOTX_SUBDEV_REGISTER_TYPE_DYNAMIC,
    
    IOTX_SUBDEV_REGISTER_TYPE_MAX
}iotx_subdev_register_types_t;


/**
 * @brief It define a datatype of function pointer.
 *        This type of function will be called when a RRPC occur.
 *
 * @param gateway, the gateway context
 * @param product_key, the product key
 * @param deveice_name, the deveice name
 * @param message_id, the RRPC's message id
 * @param payload, the RRPC's payload
 *              payload is consists by message id, method and params
 *                  for example:
 *                      "method"："thing.service.property.set";
 *                      "method"："thing.service.property.get";
 *                      "method"："thing.service.{dsl.service.identifer}";
 *                      "method"："thing.service.disable";
 *                      "method"："thing.service.delete";
 *                      "method"："thing.service.enable";
 *
 * @return none
 */
typedef void (*rrpc_request_callback)(void* gateway, 
        const char* product_key,
        const char* deveice_name,
        const char* message_id, 
        const char* payload);

/**
 * @brief It define a datatype of function pointer.
 *        This type of function will be called when a related event occur.
 *
 * @param pcontext : The program context.
 * @param pclient : The subdevice.
 * @param msg : The event message.
 *       if SUBDEV_VIA_CLOUD_CONN is define, the msg is iotx_cloud_connection_msg_rsp_func_fpt
 *       else the msg is iotx_mqtt_event_msg_pt
 *
 * @return none
 */
typedef void (*iotx_subdev_event_handle_func_fpt)(void *pcontext, void *pclient, void* msg);


/* The structure of gateway param */
typedef struct {
    iotx_mqtt_param_pt                  mqtt;                        /* MQTT params */    
    /* If there is another user want to handle the MQTT event,
     * must set the event_handler and event pcontext */
    void*                               event_pcontext;              /* the user of gateway */
    iotx_subdev_event_handle_func_fpt   event_handler;               /* event handler for gateway user*/
} iotx_gateway_param_t, *iotx_gateway_param_pt;


/**
 * @brief Generate message id.
 *
 * @param NULL.
 *
 * @return Message id.
 */
uint32_t IOT_Gateway_Generate_Message_ID();


/**
 * @brief Construct the Gateway
 *        This function initialize the data structures, establish MQTT connection, 
 *        subscribe some default topic.
 *
 * @param gateway_param, specify the MQTT client parameter and event handler.
 *
 * @return NULL, construct failed; NOT NULL, the handle of Gateway.
 */
void* IOT_Gateway_Construct(iotx_gateway_param_pt gateway_param);


/**
 * @brief Deconstruct the Gateway
 *        This function unsubscribe default topic, disconnect MQTT connection and 
 *        release the related resource.
 *
 * @param pointer of handle, specify the Gateway.
 *
 * @return 0, deconstruct success; -1, deconstruct failed.
 */
int IOT_Gateway_Destroy(void** handle);


/**
 * @brief Device register
 *        This function used to register device and add topo.
 *
 * @param pointer of handle, specify the gateway construction.
 * @param register type.
 *               IOTX_SUBDEV_REGISTER_TYPE_STATIC
 *               IOTX_SUBDEV_REGISTER_TYPE_DYNAMIC
 * @param product key.
 * @param device name.
 * @param timestamp.           [if type = dynamic, must be NULL ]
 * @param client_id.           [if type = dynamic, must be NULL ]
 * @param sign.                [if type = dynamic, must be NULL ]
 * @param sign_method.    
 *               IOTX_SUBDEV_SIGN_METHOD_TYPE_SHA
 *               IOTX_SUBDEV_SIGN_METHOD_TYPE_MD5
 *
 * @return 0, Logout success; -1, Logout fail.
 */
int IOT_Subdevice_Register(void* handle, 
        iotx_subdev_register_types_t type, 
        const char* product_key, 
        const char* device_name,
        char* timestamp, 
        char* client_id, 
        char* sign,
        iotx_subdev_sign_method_types_t sign_type);


/**
 * @brief Device unregister
 *        This function used to delete topo and unregister device.
 *        The device must dynamic register again if it want to use after unregister.
 *
 * @param pointer of handle, specify the gateway construction.
 * @param product key.
 * @param device name.
 *
 * @return 0, Unregister success; -1, Unregister fail.
 */
int IOT_Subdevice_Unregister(void* handle, 
        const char* product_key, 
        const char* device_name);

/**
 * @brief Subdevice login
 *        This function    publish a packet with LOGIN topic and wait for the reply (with
 *        LOGIN_REPLY topic), then subscribe some subdevice topic.
 *
 * @param pointer of handle, specify the Gateway.
 * @param product key.
 * @param device name.
 * @param timestamp.           [if register_type = dynamic, must be NULL ]
 * @param client_id.           [if register_type = dynamic, must be NULL ]
 * @param sign.                [if register_type = dynamic, must be NULL ] 
 * @param sign method,  HmacSha1 or HmacMd5.      
 * @param clean session, ture or false.
 *
 * @return 0, login success; -1, login failed.
 */
int IOT_Subdevice_Login(void* handle,
        const char* product_key, 
        const char* device_name, 
        const char* timestamp, 
        const char* client_id, 
        const char* sign, 
        iotx_subdev_sign_method_types_t sign_method_type,
        iotx_subdev_clean_session_types_t clean_session_type);


/**
 * @brief Subdevice logout
 *        This function unsubscribe some subdevice topic, then publish a packet with
 *        LOGOUT topic and wait for the reply (with LOGOUT_REPLY topic).
 *
 * @param pointer of handle, specify the Gateway.
 * @param product key.
 * @param device name.
 *
 * @return 0, logout success; -1, logout failed.
 */
int IOT_Subdevice_Logout(void* handle, 
        const char* product_key, 
        const char* device_name);


/**
 * @brief Gateway get topo
 *        This function publish a packet with topo/get topic and wait for the reply (with TOPO_GET_REPLY topic).
 *
 * @param pointer of handle, specify the Gateway.
 * @param get_toop_reply.
 * @param length [in/out]. in -- get_topo_reply buffer length, out -- reply length
 *
 * @return 0, logout success; -1, logout failed.
 */
int IOT_Gateway_Get_TOPO(void* handle, 
        char* get_topo_reply, 
        uint32_t* length);

/**
 * @brief Gateway get config
 *        This function publish a packet with config/get topic and wait for the reply (with CONFIG_GET_REPLY topic).
 *
 * @param pointer of handle, specify the Gateway.
 * @param get_config_reply.
 * @param length [in/out]. in -- get_config_reply buffer length, out -- reply length
 *
 * @return 0, logout success; -1, logout failed.
 */
int IOT_Gateway_Get_Config(void* handle, 
        char* get_config_reply, 
        uint32_t* length);

/**
 * @brief Gateway publish found list
 *        This function publish a packet with new subdevice found list.
 *
 * @param pointer of handle, specify the Gateway.
 *
 * @return 0, publish success; -1, publish failed.
 */
int IOT_Gateway_Publish_Found_List(void* handle, const char* product_key, 
    const char* device_name);


/**
 * @brief Gateway Yield
 *        This function    used to received some packets.
 *
 * @param pointer of handle, specify the Gateway.
 * @param timeout interval.
 *
 * @return 0, yield success; -1, parameter error.
 */
int IOT_Gateway_Yield(void* handle, uint32_t timeout);


/**
 * @brief Gateway Subscribe
 *        This function used to subscribe some topic.
 *
 * @param pointer of handle, specify the Gateway.
 * @param topic list.
 * @param QoS.
 * @param receive data function.
 * @param topic_handle_func's userdata.
 *
 * @return 0, Subscribe success; -1, Subscribe fail.
 */
int IOT_Gateway_Subscribe(void* handle, 
        const char *topic_filter, 
        int qos, 
        iotx_subdev_event_handle_func_fpt topic_handle_func, 
        void *pcontext);


/**
 * @brief Gateway Unsubscribe
 *        This function used to unsubscribe some topic.
 *
 * @param pointer of handle, specify the Gateway.
 * @param topic list.
 *
 * @return 0, Unsubscribe success; -1, Unsubscribe fail.
 */
int IOT_Gateway_Unsubscribe(void* handle, const char* topic_filter);


/**
 * @brief Gateway Publish
 *        This function    used to Publish some packet.
 *
 * @param pointer of handle, specify the Gateway.
 * @param topic.
 * @param mqtt packet.
 *
 * @return 0, Publish success; -1, Publish fail.
 */
int IOT_Gateway_Publish(void* handle,
        const char *topic_name, 
        iotx_mqtt_topic_info_pt topic_msg);


/**
 * @brief Register RRPC callback
 *        This function used to register one RRPC callback.
 *
 * @param pointer of handle, specify the Gateway.
 * @param product key.
 * @param device name.
 * @param rrpc callback function.
 *          every device has RRPC callback by itself. 
 *
 * @return 0, Register success; -1, Register fail.
 */
int IOT_Gateway_RRPC_Register(void* handle,
        const char* product_key, 
        const char* device_name,
        rrpc_request_callback rrpc_callback);


/**
 * @brief Response RRPC request
 *        This function used to response the RRPC request.
 *
 * @param pointer of handle, specify the Gateway.
 * @param product key.
 * @param device name.
 * @param The message id of RRPC request.
 * @param Response data.
 *
 * @return 0, Response success; -1, Response fail.
 */
int IOT_Gateway_RRPC_Response(void* handle,
        const char* product_key, 
        const char* device_name,
        const char* message_id, 
        const char* response);


#endif /* SRC_SDK_IMPL_EXPORTS_IOT_EXPORT_SUBDEV_H_ */

