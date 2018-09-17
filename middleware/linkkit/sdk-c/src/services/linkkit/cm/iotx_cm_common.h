/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef __SRC_CM_COMMON_H__
#define __SRC_CM_COMMON_H__

#include "iotx_utils.h"
#include "iotx_system.h"
#include "iotx_cm.h"
#include "linked_list.h"
#include "iotx_log.h"

#include "iot_import.h"

#ifdef CM_SUPPORT_MEMORY_MAGIC
    #define CM_malloc(size) LITE_malloc(size, MEM_MAGIC, "CM")
#else
    #define CM_malloc(size) LITE_malloc(size)
#endif


#ifdef ESP8266
    #include "esp_common.h"
    #define CM_READ_ONLY ICACHE_RODATA_ATTR STORE_ATTR
#else
    #define CM_READ_ONLY
#endif

#define CM_INFO(...)         log_info("CM", __VA_ARGS__)
#define CM_WARNING(...)      log_warning("CM", __VA_ARGS__)
#define CM_ERR(...)          log_err("CM", __VA_ARGS__)


#define CM_SUPPORT_MAX_MAPPING_SIZE            (20)

#ifdef CM_SUPPORT_TOPIC_DISPATCH
    /* #define CM_MAPPING_USE_POOL */
#endif

#if (CONFIG_SDK_THREAD_COST == 1)

    /*  #define CM_PROCESS_NODE_USE_POOL */
    #define CM_SUPPORT_MAX_PROCESS_NODE_SIZE       (30)
#endif /* CONFIG_SDK_THREAD_COST */


/* text for log */
/* error */
extern const char cm_log_error_parameter[] CM_READ_ONLY;
extern const char cm_log_error_param_1[] CM_READ_ONLY;
extern const char cm_log_error_param_2[] CM_READ_ONLY;
extern const char cm_log_error_param_3[] CM_READ_ONLY;
extern const char cm_log_error_auth[] CM_READ_ONLY;
extern const char cm_log_error_memory[] CM_READ_ONLY;
extern const char cm_log_error_fail[] CM_READ_ONLY;
extern const char cm_log_error_fail_1[] CM_READ_ONLY;
extern const char cm_log_error_fail_rc[] CM_READ_ONLY;
extern const char cm_log_error_status[] CM_READ_ONLY;
extern const char cm_log_error_splice_URI[] CM_READ_ONLY;
extern const char cm_log_error_parse_URI[] CM_READ_ONLY;
extern const char cm_log_error_splice_payload[] CM_READ_ONLY;
extern const char cm_log_error_parse_payload[] CM_READ_ONLY;
extern const char cm_log_error_get_node[] CM_READ_ONLY;
extern const char cm_log_error_push_node[] CM_READ_ONLY;
extern const char cm_log_error_process[] CM_READ_ONLY;
extern const char cm_log_error_parse_id[] CM_READ_ONLY;
extern const char cm_log_error_parse_code[] CM_READ_ONLY;
extern const char cm_log_error_parse_data[] CM_READ_ONLY;
extern const char cm_log_error_parse_params[] CM_READ_ONLY;
extern const char cm_log_error_parse_method[] CM_READ_ONLY;
extern const char cm_log_error_type[] CM_READ_ONLY;
extern const char cm_log_error_pk[] CM_READ_ONLY;
extern const char cm_log_error_dn[] CM_READ_ONLY;
extern const char cm_log_error_di[] CM_READ_ONLY;
extern const char cm_log_error_secret_1[] CM_READ_ONLY;
extern const char cm_log_error_secret_2[] CM_READ_ONLY;
extern const char cm_log_error_ret_code[] CM_READ_ONLY;
extern const char cm_log_error_protocol[] CM_READ_ONLY;

/* warning */
extern const char cm_log_warning_cloud_disconnected[] CM_READ_ONLY;
extern const char cm_log_warning_not_support[] CM_READ_ONLY;
extern const char cm_log_warning_not_arrived[] CM_READ_ONLY;
extern const char cm_log_warning_not_mapping[] CM_READ_ONLY;
extern const char cm_log_warning_not_func[] CM_READ_ONLY;
extern const char cm_log_warning_no_list[] CM_READ_ONLY;
extern const char cm_log_warning_buffer_overflow[] CM_READ_ONLY;

/* info */
extern const char cm_log_info_init[] CM_READ_ONLY;
extern const char cm_log_info_registered[] CM_READ_ONLY;
extern const char cm_log_info_registered_1[] CM_READ_ONLY;
extern const char cm_log_info_event_id[] CM_READ_ONLY;
extern const char cm_log_info_event_type[] CM_READ_ONLY;
extern const char cm_log_info_rsp_type[] CM_READ_ONLY;
extern const char cm_log_info_cloud_disconnect[] CM_READ_ONLY;
extern const char cm_log_info_cloud_reconnect[] CM_READ_ONLY;
extern const char cm_log_info_result[] CM_READ_ONLY;
extern const char cm_log_info_URI[] CM_READ_ONLY;
extern const char cm_log_info_URI_1[] CM_READ_ONLY;
extern const char cm_log_info_URI_sys[] CM_READ_ONLY;
extern const char cm_log_info_URI_ext[] CM_READ_ONLY;
extern const char cm_log_info_URI_undefined[] CM_READ_ONLY;
extern const char cm_log_info_URI_length[] CM_READ_ONLY;
extern const char cm_log_info_payload_length[] CM_READ_ONLY;
extern const char cm_log_info_payload[] CM_READ_ONLY;
extern const char cm_log_info_firmware[] CM_READ_ONLY;
extern const char cm_log_info_MQTT_disconnect[] CM_READ_ONLY;
extern const char cm_log_info_MQTT_reconnect[] CM_READ_ONLY;
extern const char cm_log_info_remove_mapping[] CM_READ_ONLY;
extern const char cm_log_info_enter_process_1[] CM_READ_ONLY;
extern const char cm_log_info_enter_process_2[] CM_READ_ONLY;
extern const char cm_log_info_process_type[] CM_READ_ONLY;
extern const char cm_log_info_raw_data[] CM_READ_ONLY;
extern const char cm_log_info_auth_req[] CM_READ_ONLY;
extern const char cm_log_info_auth_rsp[] CM_READ_ONLY;
extern const char cm_log_info_auth_payload_req[] CM_READ_ONLY;
extern const char cm_log_info_auth_payload_rsp[] CM_READ_ONLY;
extern const char cm_log_info_add_device[] CM_READ_ONLY;
extern const char cm_log_info_found_device[] CM_READ_ONLY;
extern const char cm_log_info_remove_device[] CM_READ_ONLY;
extern const char cm_log_info_local_disconnect[] CM_READ_ONLY;
extern const char cm_log_info_local_reconnect[] CM_READ_ONLY;
extern const char cm_log_info_version[] CM_READ_ONLY;
extern const char cm_log_info_not_support[] CM_READ_ONLY;
extern const char cm_log_info_response_code[] CM_READ_ONLY;
extern const char cm_log_info_response[] CM_READ_ONLY;
extern const char cm_log_info_mqtt_send[] CM_READ_ONLY;
extern const char cm_log_info_mqtt_received[] CM_READ_ONLY;
extern const char cm_log_info_suback[] CM_READ_ONLY;

/* The structure of cm event msg */
typedef struct {
    char                                       *URI;
    void                                       *payload;
    unsigned int                                payload_length;
} iotx_cm_response_info_t;

#ifdef CONFIG_CM_SUPPORT_LOCAL_CONN
typedef struct iotx_cm_local_device_st {
    iotx_cm_send_peer_t                        *device_info;
    void                                       *addr;
} iotx_cm_local_device_t;
#endif

typedef enum {
    IOTX_CM_MAPPING_TYPE_REGISTER = 0,
    IOTX_CM_MAPPING_TYPE_SERVICE,
    IOTX_CM_MAPPING_TYPE_MAX,
} iotx_cm_mapping_type_t;


typedef void (*iotx_cm_response_fp_t)(void *pcontext, iotx_cm_mapping_type_t mapping_type,
                                      iotx_cm_message_info_t *msg_info);


/* The structure of cloud connection context */
typedef struct iotx_cm_mapping_st {
    void                                      *next;
    char                                      *URI;
#ifdef CM_MAPPING_USE_POOL
    int                                        is_used;
#endif
    iotx_cm_mapping_type_t                     mapping_type;
    iotx_cm_register_fp_t                      func;
    void                                      *user_data;
    void                                      *mail_box;
} iotx_cm_mapping_t;


typedef enum {
    cm_iterator_action_register = 0,
    cm_iterator_action_unregister,
    cm_iterator_action_add_service,
    cm_iterator_action_remove_service,
    cm_iterator_action_add_subdevice,
    cm_iterator_action_remove_subdevice,
    cm_iterator_action_init_second,

    cm_iterator_action_max
} cm_iterator_action_t;



#if (CONFIG_SDK_THREAD_COST == 1)

typedef enum {
    /* connect */
    /* msg is NULL */
    IOTX_CM_PROCESS_CONNECT = 1,

    /* connect */
    /* msg is NULL */
    IOTX_CM_PROCESS_DISCONNECT,

    /* register */
    /* msg is iotx_cm_process_register_t */
    IOTX_CM_PROCESS_REGISTER,

    /* register result */
    /* msg is iotx_cm_process_register_result_t */
    IOTX_CM_PROCESS_REGISTER_RESULT,

    /* unregister */
    /* msg is URI */
    IOTX_CM_PROCESS_UNREGISTER,

    /* unregister result */
    /* msg is iotx_cm_process_register_result_t */
    IOTX_CM_PROCESS_UNREGISTER_RESULT,

    /* send */
    /* msg is iotx_cm_process_send_t */
    IOTX_CM_PROCESS_CLOUD_SEND,

    /* new data */
    /* msg is iotx_mqtt_topic_info_t or iotx_cloud_connection_msg_rsp_t */
    IOTX_CM_PROCESS_CLOUD_NEW_DATA,

    /* cloud disconnect */
    /* msg is null*/
    IOTX_CM_PROCESS_CLOUD_DISCONNECT,

    /* cloud reconnect */
    /* msg is null*/
    IOTX_CM_PROCESS_CLOUD_RECONNECT,

    IOTX_CM_PROCESS_CLOUD_MAX,

    /* local add device */
    /* todo */
    IOTX_CM_PROCESS_ADD_DEVICE = IOTX_CM_PROCESS_CLOUD_MAX + 1,

    /* local remoce device */
    /* todo */
    IOTX_CM_PROCESS_REMOVE_DEVICE,

    /* register */
    /* msg is iotx_cm_process_register_t */
    IOTX_CM_PROCESS_ADD_SERVICE,

    /* register result */
    /* msg is iotx_cm_process_register_result_t */
    IOTX_CM_PROCESS_ADD_SERVICE_RESULT,

    /* unregister */
    /* msg is URI */
    IOTX_CM_PROCESS_REMOVE_SERVICE,

    /* unregister result */
    /* msg is iotx_cm_process_register_result_t */
    IOTX_CM_PROCESS_REMOVE_SERVICE_RESULT,

    /* send */
    /* msg is iotx_cm_process_send_t */
    IOTX_CM_PROCESS_LOCAL_SEND,

    /* new data */
    /* msg is iotx_mqtt_topic_info_t or iotx_cloud_connection_msg_rsp_t */
    IOTX_CM_PROCESS_LOCAL_NEW_DATA,

    /* local disconnect */
    /* msg is null*/
    IOTX_CM_PROCESS_LOCAL_DISCONNECT,

    /* local reconnect */
    /* msg is null*/
    IOTX_CM_PROCESS_LOCAL_RECONNECT,

    IOTX_CM_PROCESS_LOCAL_CLOUD_INIT,

    /* add subdevice */
    IOTX_CM_PROCESS_ADD_SUBDIVCE,

    /* remove subdevice */
    IOTX_CM_PROCESS_REMOVE_SUBDIVCE,

    IOTX_CM_PROCESS_LOCAL_MAX,

    IOTX_CM_PROCESS_MAX = 100
} iotx_cm_process_node_types_t;


typedef struct iotx_cm_process_register_st {
    /*char*                                       URI;
    iotx_cm_register_fp_t                       register_func;
    void*                                       user_data;
    void*                                       mail_box;*/
    iotx_cm_register_param_t                  **register_param;
    int                                         count;
} iotx_cm_process_register_t;


typedef struct iotx_cm_process_service_st {
    char                                       *URI;
    iotx_cm_message_auth_types_t                auth_type;
    iotx_cm_register_fp_t                       register_func;
    void                                       *user_data;
    void                                       *mail_box;
} iotx_cm_process_service_t;


typedef struct iotx_cm_process_subdevice_st {
    char                                        pk[PRODUCT_KEY_LEN + 1];
    char                                        dn[DEVICE_NAME_LEN + 1];
} iotx_cm_process_subdevice_t;


typedef struct iotx_cm_process_send_st {
    iotx_cm_send_peer_t                        *target;
    iotx_cm_message_ack_types_t                 ack_type;
    char                                       *URI;
    void                                       *payload;
    int                                         payload_length;
    void                                       *sem;
    void                                       *conn_ctx;
} iotx_cm_process_send_t;

typedef struct iotx_cm_process_register_result_st {
    char                                       *URI;
    /* 0: success, -1:fail */
    int                                         result;
    int                                         is_register;
} iotx_cm_process_register_result_t;

typedef struct iotx_cm_process_service_result_st {
    char                                       *URI;
    /* 0: success, -1:fail */
    int                                         result;
    int                                         is_add;
} iotx_cm_process_service_result_t;


typedef struct iotx_cm_process_list_node_st {
    void                                       *next;
    void                                       *pre;
#ifdef CM_PROCESS_NODE_USE_POOL
    int                                         is_used;
#endif
    iotx_cm_process_node_types_t                type;
    void                                       *msg;
} iotx_cm_process_list_node_t;


typedef struct iotx_cm_process_list_st {
    iotx_cm_process_list_node_t                *header;
    iotx_cm_process_list_node_t                *tailer;
    int                                         size;
} iotx_cm_process_list_t;

#endif /* CONFIG_SDK_THREAD_COST */


typedef struct iotx_cm_conntext_st {
#if (CONFIG_SDK_THREAD_COST == 1)
    void                                        *action_lock;
#endif /* CONFIG_SDK_THREAD_COST */
    uint64_t                                     cm_message_id;
    linked_list_t                               *list_connectivity;
    iotx_cm_mapping_t                           *register_mapping_list;
    iotx_cm_mapping_t                           *service_mapping_list;
    iotx_cm_response_fp_t                        response_func;
    linked_list_t                               *list_event_callback;
    int                                          inited;
    void                                        *target_connectivity;
    //    int                                          send_sync_s
} iotx_cm_conntext_t;

int iotx_cm_auth(const char *product_key, const char *device_name, const char *client_id,
                 iotx_cm_cloud_domain_types_t domain_type);

//void iotx_cm_free_message_info(iotx_cm_message_info_t* message_info);

void iotx_cm_response_func(void *context, iotx_cm_mapping_type_t mapping_type, iotx_cm_message_info_t *message_info);

//int iotx_cm_parse_payload(void* _payload, int payload_length, iotx_cm_message_info_t* msg);

//int iotx_cm_splice_payload(void* payload, int* payload_length, int id, iotx_cm_message_info_t* msg);

iotx_cm_mapping_t *iotx_cm_get_mapping_node(iotx_cm_mapping_type_t mapping_type);

int iotx_cm_free_mapping_node(iotx_cm_mapping_type_t mapping_type, iotx_cm_mapping_t *node);

int iotx_cm_add_mapping(iotx_cm_conntext_t *cm_ctx, iotx_cm_mapping_type_t mapping_type, char *URI,
                        iotx_cm_register_fp_t func, void *user_data, void *mail_box);

int iotx_cm_remove_mapping(iotx_cm_conntext_t *cm_ctx, iotx_cm_mapping_type_t mapping_type, char *URI);

int iotx_cm_remove_mapping_all(iotx_cm_conntext_t *cm_ctx);

iotx_cm_mapping_t *iotx_cm_find_mapping(iotx_cm_conntext_t *cm_ctx, iotx_cm_mapping_type_t mapping_type, char *URI,
                                        int URI_length);

int iotx_cm_register(iotx_cm_conntext_t *cm_ctx, void *_connectivity, iotx_cm_register_param_t *register_param,
                     int count);

int iotx_cm_unregister(iotx_cm_conntext_t *cm_ctx, void *_connectivity, char *URI);

int iotx_cm_add_service(iotx_cm_conntext_t *cm_ctx, void *_connectivity, char *URI,
                        iotx_cm_message_auth_types_t auth_type,
                        iotx_cm_register_fp_t register_func, void *user_data, void *mail_box);

int iotx_cm_remove_service(iotx_cm_conntext_t *cm_ctx, void *_connectivity, char *URI);

int iotx_cm_add_subdevice(iotx_cm_conntext_t *cm_ctx, void *_connectivity, const char *pk, const char *dn);

int iotx_cm_remove_subdevice(iotx_cm_conntext_t *cm_ctx, void *_connectivity, const char *pk, const char *dn);

int iotx_cm_init_second(iotx_cm_conntext_t *cm_ctx, void *_connectivity);

//int iotx_cm_parse_message(iotx_cm_conntext_t* cm_ctx, iotx_cm_message_info_t* message_info, char** URI, void** payload, int* payload_length);

int iotx_cm_send_data(iotx_cm_conntext_t *cm_ctx, void *_connectivity, iotx_cm_send_peer_t *target,
                      char *URI, iotx_cm_message_ack_types_t ack_type, void *payload, int payload_length, void *context, void *sem);

int iotx_cm_trigger_event_callback(iotx_cm_conntext_t *cm_ctx, iotx_cm_event_msg_t *msg);


#endif /* __SRC_CM_COMMON_H__ */


