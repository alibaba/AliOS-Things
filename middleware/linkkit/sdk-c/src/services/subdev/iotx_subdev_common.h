/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef SRC_SUBDEVICE_CMP_UTIL_H_
#define SRC_SUBDEVICE_CMP_UTIL_H_

#include "subdev_debug.h"

/* Subdevice    seesion status */
typedef enum IOTX_SUBDEVICE_SESSION_STATUS_CODES {
    /* Initial */
    IOTX_SUBDEVICE_SEESION_STATUS_INIT,

    /* Register */
    IOTX_SUBDEVICE_SEESION_STATUS_REGISTER,

    /* Login */
    IOTX_SUBDEVICE_SEESION_STATUS_LOGIN,

    /* Logout */
    IOTX_SUBDEVICE_SEESION_STATUS_LOGOUT,

    /* Enable */
    IOTX_SUBDEVICE_SEESION_STATUS_ENABLE,

    /* Disalbe */
    IOTX_SUBDEVICE_SEESION_STATUS_DISABLE,

    /* Unregister */
    IOTX_SUBDEVICE_SEESION_STATUS_UNREGISTER,

    /* Maximum number of seesion status type */
    IOTX_SUBDEVICE_SEESION_STATUS_MAX
}iotx_subdevice_session_status_t;

/* The structure of subdevice session */
typedef struct iotx_subdevice_session_st{
    char                                device_cloud_id[DEVICE_ID_LEN];
    char                                device_cloud_secret[DEVICE_SECRET_LEN];
    char                                product_key[PRODUCT_KEY_LEN];
    char                                device_name[DEVICE_NAME_LEN];
    char                                timestamp[20];
    char                                sign[41];
    char                                client_id[IOT_SUBDEVICE_CLIENT_ID_LEN];
    iotx_subdev_sign_method_types_t     sign_method;                     /* HmacSha1, HmacMd5 */
    iotx_subdev_clean_session_types_t   clean_session;                   /* ture, false */
    iotx_subdevice_session_status_t     session_status;
    rrpc_request_callback               rrpc_callback;
#ifdef IOT_GATEWAY_SUPPORT_MULTI_THREAD
    void*                               lock_generic;
    void*                               lock_status;
#endif
    int                                 dynamic_register;
    struct iotx_subdevice_session_st*   next;
} iotx_subdevice_session_t, *iotx_subdevice_session_pt;


/* The structure of common reply data */
typedef struct iotx_common_reply_data_st{
    uint32_t                            id;
    uint32_t                            code;
    iotx_subdevice_session_pt           session;                          /* subdevice session */
}iotx_common_reply_data_t,*iotx_common_reply_data_pt;


/* The structure of gateway data */
typedef struct iotx_gateway_data_st {
    uint32_t                            sync_status;
    iotx_common_reply_data_t            login_reply;
    iotx_common_reply_data_t            logout_reply;
    iotx_common_reply_data_t            topo_add_reply;
    iotx_common_reply_data_t            topo_delete_reply;
    iotx_common_reply_data_t            topo_get_reply;
    iotx_common_reply_data_t            config_get_reply;
    iotx_common_reply_data_t            list_found_reply;
    iotx_common_reply_data_t            register_reply;
    iotx_common_reply_data_t            unregister_reply;
    char                                register_message[REPLY_MESSAGE_LEN_MAX];
    char                                topo_get_message[REPLY_MESSAGE_LEN_MAX];
    char                                config_get_message[REPLY_MESSAGE_LEN_MAX];
    rrpc_request_callback               rrpc_callback;
#ifdef IOT_GATEWAY_SUPPORT_MULTI_THREAD
    void*                               lock_sync;
    void*                               lock_sync_enter;
#endif
} iotx_gateway_data_t, *iotx_gateway_data_pt;


/* The structure of gateway context */
typedef struct iotx_gateway_st {
    void                               *mqtt;
    iotx_subdevice_session_pt           session_list;
    iotx_gateway_data_t                 gateway_data;
    /* If there is another user want to handle the MQTT event,
     * must set the event_handler and event pcontext */
    void*                               event_pcontext;
    iotx_subdev_event_handle_func_fpt   event_handler;
    int                                 is_construct;
} iotx_gateway_t, *iotx_gateway_pt;

extern iotx_gateway_pt g_gateway_subdevice_t;

#define MALLOC_MEMORY(buffer, length) \
    do { \
        if (buffer) \
            LITE_free(buffer); \
        (buffer) = (void*)LITE_malloc(length); \
        if (NULL == (buffer)) { \
            subdev_err("Not enough memory"); \
            return; \
        } \
        memset((buffer), 0x0, (length)); \
    } while(0)

#define MALLOC_MEMORY_WITH_RESULT(buffer, length, result) \
    do { \
        if (buffer) \
            LITE_free(buffer); \
        (buffer) = (void*)LITE_malloc(length); \
        if (NULL == (buffer)) { \
            subdev_err("Not enough memory"); \
            return (result); \
        } \
        memset((buffer), 0x0, (length)); \
    } while(0)

#define MALLOC_MEMORY_WITH_FREE(buffer, length, free_buffer) \
    do { \
        if (buffer) \
            LITE_free(buffer); \
        (buffer) = (void*)LITE_malloc(length); \
        if (NULL == (buffer)) { \
            subdev_err("Not enough memory"); \
            LITE_free(free_buffer); \
            return; \
        } \
        memset((buffer), 0x0, (length)); \
    } while(0)

#define MALLOC_MEMORY_WITH_FREE_AND_RESULT(buffer, length, free_buffer, result) \
    do { \
        if (buffer) \
            LITE_free(buffer); \
        (buffer) = (void*)LITE_malloc(length); \
        if (NULL == (buffer)) { \
            subdev_err("Not enough memory"); \
            LITE_free(free_buffer); \
            return (result); \
        } \
        memset((buffer), 0x0, (length)); \
    } while(0)

#define PARAMETER_NULL_CHECK(param) \
    do { \
        if ((param) == NULL) { \
            subdev_info("param error"); \
            return; \
        } \
    } while(0)

#define PARAMETER_NULL_CHECK_WITH_RESULT(param, result) \
    do { \
        if ((param) == NULL) { \
            subdev_info("param error"); \
            return (result); \
        } \
    } while(0)

#define PARAMETER_STRING_NULL_CHECK(ptr) \
    do { \
        if (NULL == (ptr)) { \
            subdev_err("Invalid argument, %s = %p", #ptr, (ptr)); \
            return; \
        } \
        if (0 == strlen((ptr))) { \
            subdev_err("Invalid argument, %s = '%s'", #ptr, (ptr)); \
            return; \
        } \
    } while(0)

#define PARAMETER_STRING_NULL_CHECK_WITH_RESULT(ptr, result) \
    do { \
        if (NULL == (ptr)) { \
            subdev_err("Invalid argument, %s = %p", #ptr, (ptr)); \
            return (result); \
        } \
        if (0 == strlen((ptr))) { \
            subdev_err("Invalid argument, %s = '%s'", #ptr, (ptr)); \
            return (result); \
        } \
    } while(0)

#define PARAMETER_GATEWAY_CHECK(gateway_t, result) \
    do { \
        if ((gateway_t) == NULL) { \
            subdev_info("param error"); \
            return (result); \
        } \
        if ((gateway_t) != g_gateway_subdevice_t) { \
            subdev_info("param error"); \
            return (result); \
        } \
        if ((gateway_t)->mqtt == NULL) { \
            subdev_info("param error"); \
            return (result); \
        } \
    } while(0)

typedef enum IOTX_GATEWAY_PUBLISH_TYPE {
    IOTX_GATEWAY_PUBLISH_REGISTER,
    IOTX_GATEWAY_PUBLISH_UNREGISTER,
    IOTX_GATEWAY_PUBLISH_TOPO_ADD,
    IOTX_GATEWAY_PUBLISH_TOPO_DELETE,
    IOTX_GATEWAY_PUBLISH_TOPO_GET,
    IOTX_GATEWAY_PUBLISH_CONFIG_GET,
    IOTX_GATEWAY_PUBLISH_LIST_FOUND,
    IOTX_GATEWAY_PUBLISH_LOGIN,
    IOTX_GATEWAY_PUBLISH_LOGOUT,

    IOTX_GATEWAY_PUBLISH_MAX
}iotx_gateway_publish_t;

iotx_subdevice_session_pt iotx_subdevice_find_session(iotx_gateway_pt gateway,
        const char* product_key,
        const char* device_name);

iotx_subdevice_session_pt iotx_subdevice_add_session(iotx_gateway_pt gateway,
        const char * product_key,
        const char* device_name,
        const char* device_cloud_secret,
        const char* sign,
        const char* timestamp,
        const char* client_id,
        iotx_subdev_sign_method_types_t sign_method_type,
        iotx_subdev_clean_session_types_t clean_session_type);

int iotx_subdevice_remove_session(iotx_gateway_pt gateway,
        const char* product_key,
        const char* device_name);

int iotx_gateway_subscribe_unsubscribe_topic(iotx_gateway_pt gateway,
        const char* product_key,
        const char* device_name,
        const char* topic_fmt,
        const char* params,
        int is_subscribe);

int iotx_gateway_subscribe_unsubscribe_default(iotx_gateway_pt gateway,
        int is_subscribe);

/* topo/delete    register   unregister*/
char *iotx_gateway_splice_common_packet(const char *product_key,
        const char* device_name,
        const char* method,
        uint32_t* msg_id);


char *iotx_gateway_splice_topo_get_packet(uint32_t* msg_id);

char *iotx_gateway_splice_config_get_packet(uint32_t* msg_id);


char *iotx_gateway_splice_topo_add_packet(const char *product_key,
        const char* device_name,
        const char* clientId,
        const char* timestamp,
        const char* sign_method,
        const char* sign,
        const char* method,
        uint32_t* msg_id);


char *iotx_gateway_splice_login_packet(const char *product_key,
        const char* device_name,
        const char* clientId,
        const char* timestamp,
        const char* sign_method,
        const char* sign,
        const char* cleanSession,
        uint32_t* msg_id);

char *iotx_gateway_splice_logout_packet(const char *product_key,
        const char* device_name,
        uint32_t* msg_id);


int iotx_gateway_calc_sign(const char* prodect_key,
        const char* device_name,
        const char* device_secret,
        char* hmac_sigbuf,
        const int hmac_buflen,
        iotx_subdev_sign_method_types_t sign_method,
        const char *client_id,
        const char *timestamp_str);

void iotx_subdevice_calc_client_id(char* client_id,
        const char* product_key,
        const char* device_name);

void iotx_gateway_calc_client_id(char* client_id,
        const char* device_cloud_id,
        const char* timestamp_str,
        const char* sign_method);

int iotx_gateway_publish_sync(iotx_gateway_t* gateway,
        iotx_mqtt_qos_t qos,
        const char* topic,
        const char* packet,
        uint32_t message_id,
        iotx_common_reply_data_pt reply_data,
        iotx_gateway_publish_t publish_type);

int iotx_subdevice_set_session_status(iotx_subdevice_session_pt session,
        iotx_subdevice_session_status_t status);

iotx_subdevice_session_status_t iotx_subdevice_get_session_status(iotx_subdevice_session_pt session);


int iotx_subdevice_set_session_dynamic_register(iotx_subdevice_session_pt session);

int iotx_subdevice_get_session_dynamic_register(iotx_subdevice_session_pt session);

#ifdef SUBDEV_VIA_CLOUD_CONN
void _response_handle(void *pcontext, void *pconnection, iotx_cloud_connection_msg_rsp_pt msg);
#endif /* SUBDEV_VIA_CLOUD_CONN */


#endif /* SRC_SUBDEVICE_CMP_UTIL_H_ */
