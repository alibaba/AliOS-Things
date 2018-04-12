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


#ifndef SRC_SDK_IMPL_EXPORTS_IOT_EXPORT_CM_H_
#define SRC_SDK_IMPL_EXPORTS_IOT_EXPORT_CM_H_

#include <stdint.h>

#include "iot_export.h"

/* support mutli thread
#define CM_SUPPORT_MULTI_THREAD */

/*
* CM: connection manager platform
*/


#define CM_TOPIC_LEN_MAX  256


/* domain type */
typedef enum IOTX_CM_CLOUD_DOMAIN_TYPES {
    /* "iot-as-mqtt.cn-shanghai.aliyuncs.com" */
    IOTX_CM_CLOUD_DOMAIN_SH,

    /* USA */
    IOTX_CM_CLOUD_DOMAIN_USA,

    /* Maximum number of domain */
    IOTX_CM_CLOUD_DOMAIN_MAX
}iotx_cm_cloud_domain_types_t;


/* message confirmation type */
typedef enum IOTX_CM_MESSAGE_ACK_TYPES {
    /* non ACK */
    /* MQTT: QoS is 0 */
    /* CoAP: NON */
    /* default */
    IOTX_CM_MESSAGE_NO_ACK,

    /* need ACK */
    /* MQTT: QoS is 1 */
    /* CoAP: CON */
    IOTX_CM_MESSAGE_NEED_ACK,

    /* Maximum number of domain */
    IOTX_CM_MESSAGE_ACK_MAX
}iotx_cm_message_ack_types_t;

/* message auth type */
typedef enum IOTX_CM_MESSAGE_AUTH_TYPES {
    /* no auth */
    IOTX_CM_MESSAGE_NO_AUTH,

    /* need auth */
    IOTX_CM_MESSAGE_NEED_AUTH,

    /* Maximum number of domain */
    IOTX_CM_MESSAGE_AUTH_MAX
}iotx_cm_message_auth_types_t;


/* The message payload encode format */
typedef enum {
    /* json */
    IOTX_CM_CONTENT_TYPE_JSON,

    /* cbor */
    IOTX_CM_CONTENT_TYPE_CBOR,

    /* Maximun number of type */
    IOTX_CM_CONTENT_TYPE_MAX
} iotx_cm_content_type_t;


/* event type */
typedef enum IOTX_CM_EVENT_TYPES {
    /* cloud connected */
    /* event_msg is null */
    IOTX_CM_EVENT_CLOUD_CONNECTED  = 0,

    /* cloud: disconnect */
    /* event_msg is null */
    IOTX_CM_EVENT_CLOUD_DISCONNECT,

    /* cloud: reconnect */
    /* event_msg is null */
    IOTX_CM_EVENT_CLOUD_RECONNECT,

    /* local connected */
    /* event_msg is null */
    IOTX_CM_EVENT_LOCAL_CONNECTED,

    /* local: disconnect */
    /* event_msg is null */
    IOTX_CM_EVENT_LOCAL_DISCONNECT,

    /* local: reconnect */
    /* event_msg is null */
    IOTX_CM_EVENT_LOCAL_RECONNECT,

    /* local: found device */
    /* event_msg is iotx_cm_send_peer_t* */
    IOTX_CM_EVENT_FOUND_DEVICE = 10,

    /* local: remove device */
    /* event_msg is iotx_cm_send_peer_t* */
    IOTX_CM_EVENT_REMOVE_DEVICE,

    /* register */
    /* event_msg is iotx_cm_event_result_pt */
    IOTX_CM_EVENT_REGISTER_RESULT = 20,

    /* unregister */
    /* event_msg is iotx_cm_event_result_pt */
    IOTX_CM_EVENT_UNREGISTER_RESULT,

    /* unregister */
    /* event_msg is iotx_cm_event_result_pt */
    IOTX_CM_EVENT_SEND_RESULT,

    /* add service */
    /* event_msg is iotx_cm_event_result_pt */
    IOTX_CM_EVENT_ADD_SERVICE_RESULT,

    /* remoce service */
    /* event_msg is iotx_cm_event_result_pt */
    IOTX_CM_EVENT_REMOVE_SERVICE_RESULT,

    /* new version detected, please ota */
    /* event_msg is iotx_cm_ota_parameter_t */
    IOTX_CM_EVENT_NEW_VERSION_DETECTED = 30,

        /* event_msg is iotx_cm_ota_parameter_t */
    IOTX_CM_EVENT_NEW_DATA_RECEIVED = 40,

    /* Maximum number of protocol */
    IOTX_CM_EVENT_MAX
}iotx_cm_event_types_t;


typedef enum IOTX_CM_MESSAGE_TYPE {
    /* RAW */
    IOTX_CM_MESSAGE_RAW = 1,

    /* request */
    /* method and parameter is non-value, code is 0 */
    IOTX_CM_MESSAGE_REQUEST = 2,

    /* response */
    /* code and parameter is non-value, method is NULL */
    /* parameter is for data node */
    IOTX_CM_MESSAGE_RESPONSE = 3,

    IOTX_CM_MESSAGE_MAX
}iotx_cm_message_types_t;


/* message confirmation type */
typedef enum IOTX_CM_DEVICE_SECRET_TYPES {
    /* product secret */
    IOTX_CM_DEVICE_SECRET_PRODUCT,

    /* device secret */
    IOTX_CM_DEVICE_SECRET_DEVICE,

    /* Maximum number of domain */
    IOTX_CM_DEVICE_SECRET_TYPES_MAX
}iotx_cm_device_secret_types_t;


#ifdef SERVICE_OTA_ENABLED
/* URI type */
typedef enum IOTX_CM_OTA_TYPE {
    /* FOTA */
    IOTX_CM_OTA_TYPE_FOTA = 1,

    /* COTA */
    IOTX_CM_OTA_TYPE_COTA = 2,

    /* Maximum */
    IOTX_CM_OTA_TYPE_MAX
}iotx_cm_ota_types_t;
#endif /* SERVICE_OTA_ENABLED */


/* The structure of event for cloud found new device */
typedef struct {
    char                                     product_key[PRODUCT_KEY_LEN + 1];
    char                                     device_name[DEVICE_NAME_LEN + 1];
} iotx_cm_event_device_t, *iotx_cm_event_device_pt;


/* The structure of event for register/unregister result */
typedef struct {
    /* 0: success, -1:nack */
    int                                      result;
    char*                                    URI;
} iotx_cm_event_result_t, *iotx_cm_event_result_pt;



#ifdef SERVICE_OTA_ENABLED
/* The structure of fota result */
typedef struct {
    uint32_t                                  size_file;            /* size of file */
    char*                                     purl;                 /* point to URL */
    char*                                     version;              /* point to string */
} iotx_cm_fota_parameter_t;


/* The structure of cota result */
typedef struct {
    char*                                     configId;             /* config ID */
    uint32_t                                  configSize;           /* config size */
    char*                                     sign;                 /* sign */
    char*                                     signMethod;           /* sign method */
    char*                                     url;                  /* point to URL */
    char*                                     getType;              /* getType */
} iotx_cm_cota_parameter_t;
#endif /* SERVICE_OTA_ENABLED */


/* The structure of cm event msg */
typedef struct {
    uint8_t                                   event_id;
    void*                                     msg;
} iotx_cm_event_msg_t;


/* The structure of cm event msg */
typedef struct {
    /* If it is the IOTX_CM_MESSAGE_REQUEST in IOT_CM_Send, this id is no mean.
     * If it is the IOTX_CM_MESSAGE_RESPONSE in IOT_CM_Send,
     * this id is must have value, read in register_callback's IOTX_CM_MESSAGE_REQUEST.
     * If is is the IOTX_CM_MESSAGE_RESPONSE in register_callback, this id is no mean.
     * If is is the IOTX_CM_MESSAGE_REQUEST in register_callback, this id must be non-null. */
    int                                       id;
    iotx_cm_message_ack_types_t               ack_type;
    char*                                     URI;
    unsigned int                              code;   /* [in/out] */
    char*                                     method;
    void*                                     parameter;
    unsigned int                              parameter_length;
    char*                                     message;
    iotx_cm_message_types_t                   message_type;   /* response, request or raw */
    void*                                     conn_ctx;      /* connection context */
} iotx_cm_message_info_t;


/* The structure of event for cloud found new device */
typedef struct {
    char                                      product_key[PRODUCT_KEY_LEN + 1];
    char                                      device_name[DEVICE_NAME_LEN + 1];
} iotx_cm_send_peer_t;


/* The structure of new data */
typedef struct {
    iotx_cm_send_peer_t*                      peer;
    iotx_cm_message_info_t*                   message_info;
} iotx_cm_new_data_t;


/**
 * @brief It define a datatype of function pointer.
 *        This type of function will be called when a related event occur.
 *
 * @param pcontext : The program context.
 * @param user_data : The user_data set by user.
 * @param msg : The event message.
 *
 * @return none
 */
typedef void (*iotx_cm_event_handle_fp_t)(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data);


#ifdef SERVICE_OTA_ENABLED
/**
 * @brief It define a datatype of function pointer.
 *        This type of function will be called when a related event occur.
 *
 * @param pcontext : The program context.
 * @param ota_parameter : The fota parameter.
 * @param user_data : The user_data set by user.
 *
 * @return none
 */
typedef void (*iotx_cm_fota_handle_fp_t)(void* pcontext, iotx_cm_fota_parameter_t* ota_parameter, void* user_data);

/**
 * @brief It define a datatype of function pointer.
 *        This type of function will be called when a related event occur.
 *
 * @param pcontext : The program context.
 * @param ota_parameter : The cota parameter.
 * @param user_data : The user_data set by user.
 *
 * @return none
 */
typedef void (*iotx_cm_cota_handle_fp_t)(void* pcontext, iotx_cm_cota_parameter_t* ota_parameter, void* user_data);
#endif /* SERVICE_OTA_ENABLED */


/**
 * @brief It define a datatype of function pointer.
 *        This type of function will be called when received a related resqust.
 *
 * @param target : The register source.
 * @param user_data : The user_data set by user.
 * @param msg : The message info.
 *
 * @return none
 */
typedef void (*iotx_cm_register_fp_t)(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);


/* The structure of CM param */
typedef struct {
    iotx_cm_device_secret_types_t               secret_type;
    iotx_cm_cloud_domain_types_t                domain_type;
    iotx_cm_event_handle_fp_t                   event_func;
    void*                                       user_data;
} iotx_cm_init_param_t;


/* The structure of Register param */
typedef struct {
    char*                                       URI;
    iotx_cm_message_types_t                     message_type;
    iotx_cm_register_fp_t                       register_func;
    void*                                       user_data;
    void*                                       mail_box;
} iotx_cm_register_param_t;


/* The structure of Register param */
typedef struct {
    char*                                      URI;
} iotx_cm_unregister_param_t;


/* The structure of Register param */
typedef struct {
    char*                                      URI;
    iotx_cm_message_types_t                    message_type;
    iotx_cm_message_auth_types_t               auth_type;
    iotx_cm_register_fp_t                      register_func;
    void*                                      user_data;
    void*                                      mail_box;
} iotx_cm_add_service_param_t;


/* The structure of Register param */
typedef struct {
    char*                                      URI;
} iotx_cm_remove_service_param_t;


#ifdef SERVICE_OTA_ENABLED
typedef struct {
    iotx_cm_ota_types_t                       ota_type;
    /* if there is more data to download, is_more is 1, else is_more is 0 */
    uint8_t                                   is_more;       /* [out] */
    /* is_more is 0, result: */
    /*
     * Burn firmware file failed  -4
     * Check firmware file failed -3
     * Fetch firmware file failed -2
     * Initialized failed -1
     * success 0
     */
    int8_t                                    result;         /* [out] */
    int                                       progress;       /* [out] */
    void*                                     buffer;         /* [in/out] */
    int                                       buffer_length;  /* [in/out] */
} iotx_cm_ota_t, *iotx_cm_ota_pt;
#endif /* SERVICE_OTA_ENABLED */


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
int IOT_CM_Init(iotx_cm_init_param_t* init_param, void* option);


#ifdef SERVICE_OTA_ENABLED
/**
 * @brief Start OTA
 *        This function use to start OTA, set cur_version and ota_func.
 *
 * @param cur_version, current version.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int IOT_CM_OTA_Start(char* cur_version, void* option);


/**
* @brief Set OTA callback
*        This function use to set OTA callback.
*
* @param type, fota or cota.
* @param ota_func, ota callback function.
* @param user_context, ota callback user context.
* @param option, reserve.
*
* @return success or fail.
*/
int IOT_CM_OTA_Set_Callback(iotx_cm_ota_types_t type, void* ota_func, void* user_context, void* option);


/**
* @brief Get Config
*
*
* @param configScope, device or product.
* @param getType, file or other type.
* @param attributeKeys.
* @param option, reserve.
*
* @return success or fail.
*/
int IOT_CM_OTA_Get_Config(const char* configScope, const char* getType, const char* attributeKeys, void* option);


/**
* @brief Get firmware image
*
*
* @param version, firmware image version.
* @param option, reserve.
*
* @return success or fail.
*/
int IOT_CM_OTA_Request_Image(const char* version, void* option);
#endif /* SERVICE_OTA_ENABLED */


/**
 * @brief Register service.
 *        This function used to register some service by different URI, set the URI's callback.
 *        If it is received a request, will callback the register_cb.
 *        If there is no match register_cb (user have not register the service set callback), the request will be discard.
 *
 * @param pparam, register parameter, include URI and register callback.
 * @param option, reserve.
 *          This API not support one URI register twice, if the URI have been register, it will return fail.
 *
 * @return success or fail.
 */
int IOT_CM_Register(iotx_cm_register_param_t* pparam, void* option);


/**
 * @brief Unregister service.
 *        This function used to unregister some service by different URI
 *
 * @param pparam, unregister parameter, include URI.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int IOT_CM_Unregister(iotx_cm_unregister_param_t* unregister_param, void* option);


/**
 * @brief Register service.
 *        This function used to register some service by different URI, set the URI's callback.
 *        If it is received a request, will callback the register_cb.
 *        If there is no match register_cb (user have not register the service set callback), the request will be discard.
 *
 * @param pparam, register parameter, include URI and register callback.
 * @param option, reserve.
 *          This API not support one URI register twice, if the URI have been register, it will return fail.
 *
 * @return success or fail.
 */
int IOT_CM_Add_Service(iotx_cm_add_service_param_t* service_param, void* option);


/**
 * @brief Unregister service.
 *        This function used to unregister some service by different URI
 *
 * @param pparam, unregister parameter, include URI.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int IOT_CM_Remove_Service(iotx_cm_remove_service_param_t* service_param, void* option);


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
 *
 */
int IOT_CM_Send(iotx_cm_send_peer_t* target, iotx_cm_message_info_t* message_info, void* option);


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
 *
 */
int IOT_CM_Send_Sync(iotx_cm_send_peer_t* send_peer, iotx_cm_message_info_t* message_info, void* option);


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
int IOT_CM_Yield(int timeout_ms, void* option);
#endif /* CM_SUPPORT_MULTI_THREAD */


/**
 * @brief deinit
 *
 * @param option, reserve.
 *
 * @return success or fail.
 */
int IOT_CM_Deinit(void* option);

#ifdef SERVICE_OTA_ENABLED
/**
 * @brief OTA yield function.
 *        Set buffer and length yield the ota data.
 *
 * @param ota_pt, ota yield params.
 *
 * @return success or fail.
 */
int IOT_CM_OTA_Yield(iotx_cm_ota_t* cm_ota);
#endif /* SERVICE_OTA_ENABLED */


#ifdef UT_TEST
int request_inject(int id, char* uri, char* method, void* parameter, int parameter_len);

int response_inject(int id, char* uri, int code, void* data, int data_length);

#endif /* UT_TEST */



#endif /* SRC_SDK_IMPL_EXPORTS_IOT_EXPORT_CM_H_ */


