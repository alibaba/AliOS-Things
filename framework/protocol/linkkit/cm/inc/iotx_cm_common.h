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


#ifndef __SRC_CM_COMMON_H__
#define __SRC_CM_COMMON_H__

#include "iot_export_cm.h"
#include "linked_list.h"

#ifdef CM_SUPPORT_MULTI_THREAD
#ifdef CM_SUPPORT_MULTI_THREAD_VIA_HAL
#include "iot_import.h"
#else
/* multi-thread */
#include <pthread.h>
#endif
#endif /* CM_SUPPORT_MULTI_THREAD */


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

#define CM_INFO(...)         LITE_syslog(__FUNCTION__, __LINE__, LOG_INFO_LEVEL, __VA_ARGS__)
#define CM_WARNING(...)      LITE_syslog(__FUNCTION__, __LINE__, LOG_WARNING_LEVEL, __VA_ARGS__)
#define CM_ERR(...)          LITE_syslog(__FUNCTION__, __LINE__, LOG_ERR_LEVEL, __VA_ARGS__)


#define CM_SUPPORT_MAX_MAPPING_SIZE            (20)

#ifdef CM_SUPPORT_TOPIC_DISPATCH
#define CM_MAPPING_USE_POOL
#endif

#ifdef CM_SUPPORT_MULTI_THREAD

#define CM_PROCESS_NODE_USE_POOL
#define CM_SUPPORT_MAX_PROCESS_NODE_SIZE       (30)
#endif /* CM_SUPPORT_MULTI_THREAD */


/* text for log */
/* error */
extern const char cm_log_error_parameter[] CM_READ_ONLY;
extern const char cm_log_error_param_1[] CM_READ_ONLY;
extern const char cm_log_error_auth[] CM_READ_ONLY;
extern const char cm_log_error_memory[] CM_READ_ONLY;
extern const char cm_log_error_fail[] CM_READ_ONLY;
extern const char cm_log_error_fail_ota[] CM_READ_ONLY;
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

/* warning */
extern const char cm_log_warning_ota_started[] CM_READ_ONLY;
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

typedef enum {
    /* cloud */
    IOTX_CM_CONNECTIVITY_TYPE_CLOUD = 1,

    /* Local */
    IOTX_CM_CONNECTIVITY_TYPE_LOCAL  = 2,

    IOTX_CM_CONNECTIVITY_TYPE_MAX
}iotx_cm_connectivity_types_t;


/* The structure of cm event msg */
typedef struct {
    char                                       *URI;
    void                                       *payload;
    unsigned int                                payload_length;
} iotx_cm_response_info_t;

#ifdef CM_SUPPORT_LOCAL_CONN
typedef struct iotx_cm_local_device_st {
    iotx_cm_send_peer_t*                        device_info;
    void*                                       addr;
} iotx_cm_local_device_t;
#endif


typedef void (*iotx_cm_response_fp_t)(void* pcontext, iotx_cm_message_info_t* msg_info);


/* The structure of cloud connection context */
typedef struct iotx_cm_mapping_st {
    void*                                      next;
    char*                                      URI;
#ifdef CM_MAPPING_USE_POOL
    int                                        is_used;
#endif
    iotx_cm_message_types_t                    type;
    iotx_cm_register_fp_t                      func;
    void*                                      user_data;
    void*                                      mail_box;
} iotx_cm_mapping_t;


#ifdef CM_SUPPORT_MULTI_THREAD

typedef enum {
    /* register */
    /* msg is iotx_cm_process_register_pt */
    IOTX_CM_PROCESS_CLOUD_REGISTER = 0,

    /* register result */
    /* msg is iotx_cm_process_register_result_pt */
    IOTX_CM_PROCESS_CLOUD_REGISTER_RESULT,

    /* unregister */
    /* msg is URI */
    IOTX_CM_PROCESS_CLOUD_UNREGISTER,

    /* unregister result */
    /* msg is iotx_cm_process_register_result_pt */
    IOTX_CM_PROCESS_CLOUD_UNREGISTER_RESULT,

    /* send */
    /* msg is iotx_cm_process_send_pt */
    IOTX_CM_PROCESS_CLOUD_SEND,

    /* new data */
    /* msg is iotx_mqtt_topic_info_pt or iotx_cloud_connection_msg_rsp_pt */
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
    IOTX_CM_PROCESS_LOCAL_ADD_DEVICE = IOTX_CM_PROCESS_CLOUD_MAX + 1,

    /* local remoce device */
    /* todo */
    IOTX_CM_PROCESS_LOCAL_REMOVE_DEVICE,

    /* register */
    /* msg is iotx_cm_process_register_pt */
    IOTX_CM_PROCESS_LOCAL_ADD_SERVICE,

    /* register result */
    /* msg is iotx_cm_process_register_result_pt */
    IOTX_CM_PROCESS_LOCAL_ADD_SERVICE_RESULT,

    /* unregister */
    /* msg is URI */
    IOTX_CM_PROCESS_LOCAL_REMOVE_SERVICE,

    /* unregister result */
    /* msg is iotx_cm_process_register_result_pt */
    IOTX_CM_PROCESS_LOCAL_REMOVE_SERVICE_RESULT,

    /* send */
    /* msg is iotx_cm_process_send_pt */
    IOTX_CM_PROCESS_LOCAL_SEND,

    /* new data */
    /* msg is iotx_mqtt_topic_info_pt or iotx_cloud_connection_msg_rsp_pt */
    IOTX_CM_PROCESS_LOCAL_NEW_DATA,

    /* local disconnect */
    /* msg is null*/
    IOTX_CM_PROCESS_LOCAL_DISCONNECT,

    /* local reconnect */
    /* msg is null*/
    IOTX_CM_PROCESS_LOCAL_RECONNECT,

    IOTX_CM_PROCESS_LOCAL_MAX,

    IOTX_CM_PROCESS_MAX = 100
}iotx_cm_process_node_types_t;


typedef struct iotx_cm_process_register_st {
    char*                                       URI;
    iotx_cm_message_types_t                     type;
    iotx_cm_register_fp_t                       register_func;
    void*                                       user_data;
    void*                                       mail_box;
} iotx_cm_process_register_t, *iotx_cm_process_register_pt;


typedef struct iotx_cm_process_service_st {
    char*                                       URI;
    iotx_cm_message_types_t                     type;
    iotx_cm_message_auth_types_t                auth_type;
    iotx_cm_register_fp_t                       register_func;
    void*                                       user_data;
    void*                                       mail_box;
} iotx_cm_process_service_t, *iotx_cm_process_service_pt;


typedef struct iotx_cm_process_send_st {
    iotx_cm_send_peer_t*                        target;
    iotx_cm_message_ack_types_t                 ack_type;
    char*                                       URI;
    void*                                       payload;
    int                                         payload_length;
    void*                                       conn_ctx;
} iotx_cm_process_send_t, *iotx_cm_process_send_pt;

typedef struct iotx_cm_process_register_result_st {
    char*                                       URI;
    /* 0: success, -1:fail */
    int                                         result;
    int                                         is_register;
} iotx_cm_process_register_result_t, *iotx_cm_process_register_result_pt;

typedef struct iotx_cm_process_service_result_st {
    char*                                       URI;
    /* 0: success, -1:fail */
    int                                         result;
    int                                         is_add;
} iotx_cm_process_service_result_t, *iotx_cm_process_service_result_pt;


typedef struct iotx_cm_process_list_node_st {
    void*                                       next;
    void*                                       pre;
#ifdef CM_PROCESS_NODE_USE_POOL
    int                                         is_used;
#endif
    iotx_cm_process_node_types_t                type;
    void*                                       msg;
} iotx_cm_process_list_node_t;


typedef struct iotx_cm_process_list_st {
    iotx_cm_process_list_node_t*                header;
    iotx_cm_process_list_node_t*                tailer;
    int                                         size;
} iotx_cm_process_list_t;

#endif /* CM_SUPPORT_MULTI_THREAD */

typedef void* (*init_fp_t)(void* handler, iotx_cm_init_param_t* pparam);
typedef int   (*connect_fp_t)(void* handler, void* connectivity);
typedef int   (*trigger_connected_fp_t)(void* handler, void* connectivity, iotx_cm_event_handle_fp_t event_fp, void* user_data);
typedef int   (*register_fp_t)(void* handler, void* connectivity, const char* topic_filter);
typedef int   (*unregister_fp_t)(void* handler, void* connectivity, const char* topic_filter);
typedef int   (*add_service_fp_t)(void* handler, void* connectivity, const char* topic_filter, iotx_cm_message_auth_types_t auth_type);
typedef int   (*delete_service_fp_t)(void* handler, void* connectivity, const char* topic_filter);
typedef int   (*send_fp_t)(void* handler, void* connectivity, iotx_cm_send_peer_t* target,
                            const char* topic_filter, iotx_cm_message_ack_types_t ack_type, const void* payload, int payload_length, void* context);
typedef int   (*send_sync_fp_t)(void* handler, void* connectivity, iotx_cm_send_peer_t* target,
                                 const char* topic_filter,  iotx_cm_message_ack_types_t ack_type, const void* payload, int payload_length, void* context);
typedef int   (*yield_fp_t)(void* connectivity, int timeout_ms);
typedef int   (*deinit_fp_t)(void* connectivity);
typedef iotx_cm_send_peer_t* (*get_target_fp_t)(void);
#ifdef CM_SUPPORT_MULTI_THREAD
typedef int   (*add_send_fp_t)(void* cm_ctx, iotx_cm_send_peer_t* target, iotx_cm_message_info_t* message_info);
typedef void* (*thread_process_fp_t)(void *pclient);
#endif /* CM_SUPPORT_MULTI_THREAD */

typedef struct iotx_cm_connectivity_st {
    int                                           id;
    int                                           is_connected;
    iotx_cm_connectivity_types_t                  type;
    void*                                         context;
#ifdef CM_SUPPORT_MULTI_THREAD
#ifdef CM_SUPPORT_MULTI_THREAD_VIA_HAL
    void*                                         pthread_process;
#else
    pthread_t                                     pthread_process;
#endif
    iotx_cm_process_list_t*                       process_list;
    void*                                         process_lock;
#endif /* CM_SUPPORT_MULTI_THREAD */
    init_fp_t                                     init_func;
    connect_fp_t                                  connect_func;
    trigger_connected_fp_t                        trigger_connected_func;
    register_fp_t                                 register_func;
    unregister_fp_t                               unregister_func;
    add_service_fp_t                              add_service_func;
    delete_service_fp_t                           remove_service_func;
    send_fp_t                                     send_func;
    send_sync_fp_t                                send_sync_func;
    yield_fp_t                                    yield_func;
    deinit_fp_t                                   deinit_func;
    get_target_fp_t                               get_target_func;
#ifdef CM_SUPPORT_MULTI_THREAD
    add_send_fp_t                                 add_send_func;
    thread_process_fp_t                           thread_process_func;
#endif /* CM_SUPPORT_MULTI_THREAD */
} iotx_cm_connectivity_t;

typedef struct iotx_cm_connectivity_list_st {
    iotx_cm_connectivity_t*                       node;
    void*                                         next;
} iotx_cm_connectivity_list_t;

typedef struct _iotx_cm_event_cb_usr_ctx {
    iotx_cm_event_handle_fp_t                     event_func;
    void*                                         user_data;
}iotx_cm_event_cb_usr_ctx_t;


typedef struct iotx_cm_conntext_st{
#ifdef CM_SUPPORT_MULTI_THREAD
    uint8_t                                      thread_stop;
#endif /* CM_SUPPORT_MULTI_THREAD */
    uint64_t                                     cm_message_id;
    linked_list_t*                               list_connectivity;
    iotx_cm_mapping_t*                           mapping_list;
    iotx_cm_response_fp_t                        response_func;
    linked_list_t*                               list_event_callback;
#ifdef SERVICE_OTA_ENABLED
    void*                                        ota_handler;
    iotx_cm_fota_handle_fp_t                     fota_func;
    void*                                        fota_user_context;
    iotx_cm_cota_handle_fp_t                     cota_func;
    void*                                        cota_user_context;
#endif /* SERVICE_OTA_ENABLED */
    int                                          inited;
    iotx_cm_connectivity_t*                      target_connectivity;
} iotx_cm_conntext_t;

int iotx_cm_auth(const char *product_key, const char *device_name, const char *client_id);

void iotx_cm_free_message_info(iotx_cm_message_info_t* message_info);

void iotx_cm_response_func(void* context, iotx_cm_message_info_t* message_info);

int iotx_cm_parse_payload(void* _payload, int payload_length, iotx_cm_message_info_t* msg);

int iotx_cm_splice_payload(void* payload, int* payload_length, int id, iotx_cm_message_info_t* msg);

iotx_cm_connectivity_t* iotx_cm_find_connectivity(iotx_cm_conntext_t* cm_ctx, iotx_cm_send_peer_t* target, void* conn_ctx);

int iotx_cm_get_connectivity_id();

int iotx_cm_add_connectivity(iotx_cm_conntext_t* cm_ctx, iotx_cm_connectivity_t* connectivity);

int iotx_cm_add_connectivity_all(iotx_cm_conntext_t* cm_ctx, iotx_cm_init_param_t* init_param);

int iotx_cm_remove_connectivity_all(iotx_cm_conntext_t* cm_ctx);

iotx_cm_mapping_t* iotx_cm_get_mapping_node();

int iotx_cm_free_mapping_node(iotx_cm_mapping_t* node);

int iotx_cm_add_mapping(iotx_cm_conntext_t* cm_ctx, char* URI, iotx_cm_message_types_t type, iotx_cm_register_fp_t func, void* user_data, void* mail_box);

int iotx_cm_remove_mapping(iotx_cm_conntext_t* cm_ctx, char* URI);

int iotx_cm_remove_mapping_all(iotx_cm_conntext_t* cm_ctx);

iotx_cm_mapping_t* iotx_cm_find_mapping(iotx_cm_conntext_t* cm_ctx, char* URI, int URI_length);

int iotx_cm_register_service(iotx_cm_conntext_t* cm_ctx, char* _URI, iotx_cm_message_types_t type,
                             iotx_cm_register_fp_t register_func, void* user_data, void* mail_box);

int iotx_cm_unregister_service(iotx_cm_conntext_t* cm_ctx, char* URI);

int iotx_cm_add_service(iotx_cm_conntext_t* cm_ctx, char* URI, iotx_cm_message_types_t type, iotx_cm_message_auth_types_t auth_type,
                        iotx_cm_register_fp_t register_func, void* user_data, void* mail_box);

int iotx_cm_remove_service(iotx_cm_conntext_t* cm_ctx, char* URI);

int iotx_cm_parse_message(iotx_cm_conntext_t* cm_ctx, iotx_cm_message_info_t* message_info, char** URI, void** payload, int* payload_length);

int iotx_cm_send_data(iotx_cm_conntext_t* cm_ctx, iotx_cm_send_peer_t* target, void* _connectivity,
                      char* URI, iotx_cm_message_ack_types_t ack_type, void* payload, int payload_length, void* context);

int iotx_cm_trigger_event_callback(iotx_cm_conntext_t* cm_ctx, iotx_cm_event_msg_t* msg);

#ifdef CM_SUPPORT_MULTI_THREAD
iotx_cm_process_list_node_t* iotx_cm_get_list_node(iotx_cm_conntext_t* cm_ctx, iotx_cm_connectivity_types_t type);

int iotx_cm_free_list_node(iotx_cm_conntext_t* cm_ctx, iotx_cm_process_list_node_t* node);

int iotx_cm_free_list_node_all(iotx_cm_conntext_t* cm_ctx);

/* node is in */
int iotx_cm_process_list_push(iotx_cm_conntext_t* cm_ctx, iotx_cm_connectivity_types_t type, iotx_cm_process_list_node_t* node);

/* node is out */
iotx_cm_process_list_node_t* iotx_cm_process_list_pop(iotx_cm_conntext_t* cm_ctx, iotx_cm_connectivity_types_t type);

int iotx_cm_process_list_get_size(iotx_cm_conntext_t* cm_ctx, iotx_cm_connectivity_types_t type);

#endif /* CM_SUPPORT_MULTI_THREAD */

#endif /* __SRC_CM_COMMON_H__ */


