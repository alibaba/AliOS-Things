/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef SRC_SDK_IMPL_EXPORTS_IOT_EXPORT_CM_H_
#define SRC_SDK_IMPL_EXPORTS_IOT_EXPORT_CM_H_

#include <stdint.h>

#include "iot_export.h"

/* support mutli thread
#define CONFIG_SDK_THREAD_COST */

/*
* CM: connection manager platform
*/


#define CM_TOPIC_LEN_MAX  256


/* domain type */
typedef enum IOTX_CM_CLOUD_DOMAIN_TYPES {
    /* Shanghai */
    IOTX_CM_CLOUD_DOMAIN_ShangHai,

    /* Singapore */
    IOTX_CM_CLOUD_DOMAIN_Singapore,

    /* Japan */
    IOTX_CM_CLOUD_DOMAIN_Japan,

    /* America */
    IOTX_CM_CLOUD_DOMAIN_America,

    /* Germany */
    IOTX_CM_CLOUD_DOMAIN_Germany,

    /* Maximum number of domain */
    IOTX_CM_CLOUD_DOMAIN_MAX
} iotx_cm_cloud_domain_types_t;


/* protocol type */
typedef enum IOTX_CM_CONNECTION_PROTOCOL_TYPES {
    /* MQTT */
    IOTX_CM_CONNECTION_PROTOCOL_TYPE_MQTT = 1,

    /* COAP */
    IOTX_CM_CONNECTION_PROTOCOL_TYPE_COAP = 2,

    /* HTTP */
    IOTX_CM_CONNECTION_PROTOCOL_TYPE_HTTP = 3,

    /* HTTP2 */
    IOTX_CM_CONNECTION_PROTOCOL_TYPE_HTTP2 = 4,

    /* ALCS */
    IOTX_CM_CONNECTION_PROTOCOL_TYPE_ALCS = 5,

    /* Maximum number of protocol type */
    IOTX_CM_CONNECTION_PROTOCOL_TYPE_MAX
} iotx_cm_connection_protocol_types_t;


/* connectivity type */
typedef enum IOTX_CM_CONNECTIVITY_TYPES {
    /* cloud */
    IOTX_CM_CONNECTIVITY_TYPE_CLOUD = 1,

    /* alcs */
    IOTX_CM_CONNECTIVITY_TYPE_LOCAL = 2,

    /* undef */
    IOTX_CM_CONNECTIVITY_TYPE_UNDEF = 3,

    /* Maximum number of connectivity type */
    IOTX_CM_CONNECTIVITY_TYPE_MAX
} iotx_cm_connectivity_types_t;


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

    /* Maximum number of ack type */
    IOTX_CM_MESSAGE_ACK_MAX
} iotx_cm_message_ack_types_t;

/* message auth type */
typedef enum IOTX_CM_MESSAGE_AUTH_TYPES {
    /* no auth */
    IOTX_CM_MESSAGE_NO_AUTH,

    /* need auth */
    IOTX_CM_MESSAGE_NEED_AUTH,

    /* Maximum number of domain */
    IOTX_CM_MESSAGE_AUTH_MAX
} iotx_cm_message_auth_types_t;


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
    IOTX_CM_EVENT_FOUND_DEVICE,

    /* local: remove device */
    /* event_msg is iotx_cm_send_peer_t* */
    IOTX_CM_EVENT_REMOVE_DEVICE,

    /* register */
    /* event_msg is iotx_cm_event_result_pt */
    IOTX_CM_EVENT_REGISTER_RESULT,

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

    /* event_msg is iotx_cm_message_info_t */
    IOTX_CM_EVENT_NEW_DATA_RECEIVED,

    /* Maximum number of event */
    IOTX_CM_EVENT_MAX
} iotx_cm_event_types_t;


/* message confirmation type */
typedef enum IOTX_CM_DEVICE_SECRET_TYPES {
    /* product secret */
    IOTX_CM_DEVICE_SECRET_PRODUCT,

    /* device secret */
    IOTX_CM_DEVICE_SECRET_DEVICE,

    /* Maximum number of domain */
    IOTX_CM_DEVICE_SECRET_TYPES_MAX
} iotx_cm_device_secret_types_t;


/* The structure of event for cloud found new device */
typedef struct {
    char                                     product_key[PRODUCT_KEY_LEN + 1];
    char                                     device_name[DEVICE_NAME_LEN + 1];
} iotx_cm_event_device_t, *iotx_cm_event_device_pt;


/* The structure of event for register/unregister result */
typedef struct {
    /* 0: success, -1:nack */
    int                                      result;
    char                                    *URI;
} iotx_cm_event_result_t, *iotx_cm_event_result_pt;


/* The structure of cm event msg */
typedef struct {
    uint8_t                                   event_id;
    void                                     *msg;
} iotx_cm_event_msg_t;


/* The structure of cm event msg */
typedef struct {
    /* If it is the IOTX_CM_MESSAGE_REQUEST in iotx_cm_send, this id is no mean.
     * If it is the IOTX_CM_MESSAGE_RESPONSE in iotx_cm_send,
     * this id is must have value, read in register_callback's IOTX_CM_MESSAGE_REQUEST.
     * If is is the IOTX_CM_MESSAGE_RESPONSE in register_callback, this id is no mean.
     * If is is the IOTX_CM_MESSAGE_REQUEST in register_callback, this id must be non-null. */
    //    int                                       id;
    iotx_cm_message_ack_types_t               ack_type;
    //    iotx_cm_message_method_types_t            method_type;
    char                                     *URI;
    unsigned int                              URI_length;
    //    unsigned int                              code;   /* [in/out] */
    //    char*                                     method;
    //    void*                                     parameter;
    //    unsigned int                              parameter_length;
    void                                     *payload;
    unsigned int                              payload_length;
    //    char*                                     message;
    //    iotx_cm_message_types_t                   message_type;   /* response, request or raw */
    void                                     *conn_ctx;      /* connection context */
} iotx_cm_message_info_t;


/* The structure of event for cloud found new device */
typedef struct {
    char                                           product_key[PRODUCT_KEY_LEN + 1];
    char                                           device_name[DEVICE_NAME_LEN + 1];
} iotx_cm_send_peer_t;


/* The structure of new data */
typedef struct {
    iotx_cm_send_peer_t                           *peer;
    iotx_cm_message_info_t                        *message_info;
} iotx_cm_new_data_t;


/* The struct of cloud connectivity parameter */
typedef struct cm_connectivity_cloud_param_st {
    //    iotx_cm_cloud_domain_types_t              domain_type;
    iotx_cm_connection_protocol_types_t            protocol_type;
} iotx_cm_connectivity_cloud_param_t;


/* The struct of alcs connectivity parameter */
typedef struct cm_connectivity_alcs_param_st {
    iotx_cm_connection_protocol_types_t            protocol_type;
} iotx_cm_connectivity_alcs_param_t;


/* The struct of undefined connectivity parameter */
typedef struct cm_connectivity_undefined_param_st {
    iotx_cm_connection_protocol_types_t            protocol_type;
    char                                          *url;
    int                                            port;
    char                                          *pkey;
} iotx_cm_connectivity_undefined_param_t;


/* The struct of connectivity parameter */
typedef struct cm_connectivity_param_st {
    iotx_cm_connectivity_types_t                   connectivity_type;
    union {
        iotx_cm_connectivity_cloud_param_t        *cloud_param;
        iotx_cm_connectivity_alcs_param_t         *alcs_param;
        iotx_cm_connectivity_undefined_param_t    *undefined_param;
    };
} iotx_cm_connectivity_param_t;

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
typedef void (*iotx_cm_event_handle_fp_t)(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data);


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
typedef void (*iotx_cm_register_fp_t)(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);


/* The structure of CM param */
typedef struct {
    iotx_cm_device_secret_types_t               secret_type;
    iotx_cm_cloud_domain_types_t                domain_type;
    iotx_cm_event_handle_fp_t                   event_func;
    void                                       *user_data;
} iotx_cm_init_param_t;


/* The structure of Register param */
typedef struct {
    char                                       *URI;
    iotx_cm_register_fp_t                       register_func;
    void                                       *user_data;
    void                                       *mail_box;
} iotx_cm_register_param_t;


/* The structure of Register param */
typedef struct {
    char                                      *URI;
} iotx_cm_unregister_param_t;


/* The structure of Register param */
typedef struct {
    char                                      *URI;
    iotx_cm_message_auth_types_t               auth_type;
    iotx_cm_register_fp_t                      service_func;
    void                                      *user_data;
    void                                      *mail_box;
} iotx_cm_add_service_param_t;


/* The structure of Register param */
typedef struct {
    char                                      *URI;
} iotx_cm_remove_service_param_t;


/**
 * @brief CM init
 *        This function initialize the CM structures
 *        If CM has been initialized, this function will return success directly.
 *
 * @param pparam, specify the cm and event handler.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int iotx_cm_init(iotx_cm_init_param_t *init_param, void *option);


/**
 * @brief Create connectivity
 *        This function use to create one connectivity with protocol and url.
 *
 * @param connectivity_param, connectivity type and protocol, url, port, psk.
 * @param option, reserve.
 *
 * @return Connectivity handler.
 */
void *iotx_cm_open(iotx_cm_connectivity_param_t *connectivity_param, void *option);

/**
 * @brief Get protocol connection handle from connectivity
 *        This function use to get protocol connection handler from connectivity.
 *        ex: mqtt handler returned by IOT_MQTT_Construct
 *            alcs handler returned by iotx_alcs_construct
 * @param connectivity, the return by iotx_cm_open.
 *
 * @return Connectivity handle.
 */
void *iotx_cm_get_protocol_handle(void *connectivity);

/**
 * @brief Connect connectivity
 *        This function use to connect one connectivity with create by iotx_cm_open.
 *
 * @param connectivity, the return by iotx_cm_open.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int iotx_cm_connect(void *connectivity, void *option);


/**
 * @brief Destroy connectivity
 *        This function use to free one connectivity with create by iotx_cm_open.
 *
 * @param connectivity, the return by iotx_cm_open.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int iotx_cm_close(void **connectivity, void *option);


/**
 * @brief Register service.
 *        This function used to register some service by different URI, set the URI's callback.
 *        If it is received a request, will callback the register_cb.
 *        If there is no match register_cb (user have not register the service set callback), the request will be discard.
 *
 * @param connectivity
 *        return by iotx_cm_open
 *        If it is NULL, call all connectivity, alse, via connectivity to register.
 * @param pparam, register parameter, include URI and register callback.
 * @param option, reserve.
 *          This API not support one URI register twice, if the URI have been register, it will return fail.
 *
 * @return success or fail.
 */
int iotx_cm_serv_reg(void *_connectivity, iotx_cm_register_param_t *pparam, int count, void *option);


/**
 * @brief Unregister service.
 *        This function used to unregister some service by different URI
 *
 * @param connectivity
 *        return by iotx_cm_open
 *        If it is NULL, call all connectivity, alse, via connectivity to unregister.
 * @param pparam, unregister parameter, include URI.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int iotx_cm_serv_unreg(void *_connectivity, iotx_cm_unregister_param_t *unregister_param, void *option);


/**
 * @brief Register service.
 *        This function used to register some service by different URI, set the URI's callback.
 *        If it is received a request, will callback the register_cb.
 *        If there is no match register_cb (user have not register the service set callback), the request will be discard.
 *
 * @param connectivity
 *        return by iotx_cm_open
 *        If it is NULL, call all connectivity, alse, via connectivity to add service.
 * @param pparam, register parameter, include URI and register callback.
 * @param option, reserve.
 *          This API not support one URI register twice, if the URI have been register, it will return fail.
 *
 * @return success or fail.
 */
int iotx_cm_serv_add(void *_connectivity, iotx_cm_add_service_param_t *service_param, void *option);


/**
 * @brief Unregister service.
 *        This function used to unregister some service by different URI
 *
 * @param connectivity
 *        return by iotx_cm_open
 *        If it is NULL, call all connectivity, alse, via connectivity to remove service.
 * @param pparam, unregister parameter, include URI.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int iotx_cm_serv_del(void *_connectivity, iotx_cm_remove_service_param_t *service_param, void *option);


/**
 * @brief add sub-device.
 *        This function used to add sub-device with sub-device's pk and dn.
 *
 * @param connectivity
 *        return by iotx_cm_open
 *        If it is NULL, call all connectivity, alse, via connectivity to add sub-device.
 * @param PK, product_key.
 * @param DN, device_name.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int iotx_cm_subdev_add(void *_connectivity, const char *PK, const char *DN, void *option);


/**
 * @brief remove sub-device.
 *        This function used to remove sub-device with sub-device's pk and dn.
 *
 * @param connectivity
 *        return by iotx_cm_open
 *        If it is NULL, call all connectivity, alse, via connectivity to remove sub-device.
 * @param PK, product_key.
 * @param DN, device_name.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int iotx_cm_subdev_del(void *_connectivity, const char *PK, const char *DN, void *option);

int iotx_cm_postinit(void *_connectivity);


/**
 * @brief Send data.
 *        This function used to send data to target.
 *        If the connectivity and target is NULL, the data will broadcast to all the reachable places.
 *        If the connectivity is not NULL, target is NULL, the data will broadcast to the connectivity's all the reachable places.
 *        If the connectivity is NULL, target's product_key and device_name is itself, the data will send to cloud.
 *        If the connectivity and target is not NULL, the data will send to target only.
 *
 * @param connectivity
 *        return by iotx_cm_open
 *        If it is NULL, call all connectivity, alse, via connectivity to send.
 * @param target. the send peer.
 *        if NULL, will send to all target. [alcs will have more targets in one connectivity]
 * @param message_info.
 * @param option, reserve.
 *
 * @return success or fail.
 *
 */
int iotx_cm_send(void *connectivity, iotx_cm_send_peer_t *target, iotx_cm_message_info_t *message_info, void *option);


/**
 * @brief Send data.
 *        This function used to send data to target.
 *        If the connectivity and target is NULL, the data will broadcast to all the reachable places.
 *        If the connectivity is not NULL, target is NULL, the data will broadcast to the connectivity's all the reachable places.
 *        If the connectivity is NULL, target's product_key and device_name is itself, the data will send to cloud.
 *        If the connectivity and target is not NULL, the data will send to target only.
 *
 * @note This function only working in CONFIG_SDK_THREAD_COST.
 *       This function only working in _connectivity is not NULL.
 *
 * @param connectivity. the handler of iotx_cm_open.
 *        if NULL, will send to all connectivity.
 * @param target. the send peer.
 *        if NULL, will send to all target. [alcs will have more targets in one connectivity]
 * @param message_info [in/out].
 * @param option, reserve.
 *
 * @return success or fail.
 *
 */
int iotx_cm_send_sync(void *_connectivity, iotx_cm_send_peer_t *send_peer, iotx_cm_message_info_t *message_info,
                     void *option);


#if (CONFIG_SDK_THREAD_COST == 0)
    /**
    * @brief Yield.
    *        This function used to yield when want to received data.
    *        This function just need used in CONFIG_SDK_THREAD_COST = n.
    *        If the CONFIG_SDK_THREAD_COST = y, this function is no need.
    *
    * @param target.
    * @param message_info.
    * @param option, reserve.
    *
    * @return success or fail.
    */
    int iotx_cm_yield(int timeout_ms, void *option);
#endif /* CONFIG_SDK_THREAD_COST */


/**
 * @brief deinit
 *
 * @param option, reserve.
 *
 * @return success or fail.
 */
int iotx_cm_deinit(void *option);


#endif /* SRC_SDK_IMPL_EXPORTS_IOT_EXPORT_CM_H_ */
