/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef __CM_CONNECTIVITY_H__
#define __CM_CONNECTIVITY_H__

#include "iotx_cm.h"
#include "iotx_cm_common.h"

typedef enum {
    /* before create */
    IOTX_CM_CONNECTIVITY_STATUS_UNCREATED,

    /* created */
    IOTX_CM_CONNECTIVITY_STATUS_CREATED,

    /* inited */
    IOTX_CM_CONNECTIVITY_STATUS_INITED,

    /* connected */
    IOTX_CM_CONNECTIVITY_STATUS_CONNECTED,

    /* reconnected */
    IOTX_CM_CONNECTIVITY_STATUS_RECONNECTED,

    /* disconnected */
    IOTX_CM_CONNECTIVITY_STATUS_DISCONNECTED,

    IOTX_CM_CONNECTIVITY_STATUS_MAX
} iotx_cm_connectivity_status_t;


typedef void *(*init_fp_t)(void *handler, void *pparam);
typedef int (*init_sencond_fp_t)(void *handler, void *connectivity);
typedef int (*connect_fp_t)(void *handler, void *connectivity);
typedef int (*trigger_connected_fp_t)(void *handler, void *connectivity, iotx_cm_event_handle_fp_t event_fp,
                                      void *user_data);
typedef int (*check_connected_fp_t)(void *handler, void *connectivity, iotx_cm_event_handle_fp_t event_fp,
                                    void *user_data);
typedef int (*register_fp_t)(void *handler, void *connectivity, void *register_param, int count);
typedef int (*unregister_fp_t)(void *handler, void *connectivity, const char *topic_filter);
typedef int (*add_service_fp_t)(void *handler, void *connectivity, const char *topic_filter,
                                iotx_cm_message_auth_types_t auth_type);
typedef int (*delete_service_fp_t)(void *handler, void *connectivity, const char *topic_filter);
typedef int (*send_fp_t)(void *handler, void *connectivity, iotx_cm_send_peer_t *target,
                         const char *topic_filter, iotx_cm_message_ack_types_t ack_type,
                         const void *payload, int payload_length, void *context, void *sem);
typedef int (*send_sync_fp_t)(void *handler, void *connectivity, iotx_cm_send_peer_t *target,
                              const char *topic_filter,  iotx_cm_message_ack_types_t ack_type, const void *payload, int payload_length,
                              void *context);
typedef int (*add_subdevice_fp_t)(void *handler, void *connectivity, const char *pk, const char *dn);
typedef int (*remove_subdevice_fp_t)(void *handler, void *connectivity, const char *pk, const char *dn);
typedef int (*yield_fp_t)(void *handler, void *connectivity, int timeout_ms);
typedef int (*deinit_fp_t)(void *handler, void *connectivity);
typedef iotx_cm_send_peer_t *(*get_target_fp_t)(void);
typedef int (*check_target_fp_t)(iotx_cm_send_peer_t *);
#if (CONFIG_SDK_THREAD_COST == 1)
typedef int (*add_send_fp_t)(void *cm_ctx, iotx_cm_send_peer_t *target, iotx_cm_message_info_t *message_info,
                             void *sem);
typedef void *(*thread_process_fp_t)(void *pclient);
#endif /* CONFIG_SDK_THREAD_COST */


typedef struct iotx_cm_connectivity_st {
    int                                           id;
    iotx_cm_connectivity_types_t                  type;
    iotx_cm_connectivity_status_t                 status;
    void                                         *context;
#if (CONFIG_SDK_THREAD_COST == 1)
    void                                         *pthread_process;
    iotx_cm_process_list_t                       *process_list;
    void                                         *process_lock;
#endif /* CONFIG_SDK_THREAD_COST */
    uint8_t                                       is_try_connect;
    init_fp_t                                     init_func;
    init_sencond_fp_t                             init_second_func;
    connect_fp_t                                  connect_func;
    trigger_connected_fp_t                        trigger_connected_func;
    check_connected_fp_t                          check_connected_func;
    register_fp_t                                 register_func;
    unregister_fp_t                               unregister_func;
    add_service_fp_t                              add_service_func;
    delete_service_fp_t                           remove_service_func;
    send_fp_t                                     send_func;
    send_sync_fp_t                                send_sync_func;
    add_subdevice_fp_t                            add_subdevice_func;
    remove_subdevice_fp_t                         remove_subdevice_func;
    yield_fp_t                                    yield_func;
    deinit_fp_t                                   deinit_func;
    get_target_fp_t                               get_target_func;
    check_target_fp_t                             check_target_func;
#if (CONFIG_SDK_THREAD_COST == 1)
    add_send_fp_t                                 add_send_func;
    thread_process_fp_t                           thread_process_func;
#endif /* CONFIG_SDK_THREAD_COST */
} iotx_cm_connectivity_t;


typedef struct _iotx_cm_event_cb_usr_ctx {
    iotx_cm_event_handle_fp_t                     event_func;
    void                                         *user_data;
} iotx_cm_event_cb_usr_ctx_t;


int iotx_cm_get_connectivity_id();

iotx_cm_connectivity_status_t iotx_cm_get_connectivity_status(iotx_cm_connectivity_t *connectivity);

iotx_cm_connectivity_types_t iotx_cm_get_connectivity_type(iotx_cm_connectivity_t *connectivity);

int iotx_cm_set_connectivity_status(iotx_cm_connectivity_t *connectivity, iotx_cm_connectivity_status_t status);

iotx_cm_connectivity_t  *iotx_cm_create_connectivity(iotx_cm_conntext_t *cm_ctx, iotx_cm_connectivity_param_t *param);

int iotx_cm_connect_connectivity(iotx_cm_conntext_t *cm_ctx, iotx_cm_connectivity_t *connectivity);

iotx_cm_connectivity_t *iotx_cm_find_connectivity(iotx_cm_conntext_t *cm_ctx, iotx_cm_send_peer_t *target,
        void *conn_ctx);

iotx_cm_connectivity_t *iotx_cm_find_connectivity_by_type(iotx_cm_conntext_t *cm_ctx,
        iotx_cm_connectivity_types_t type);

int iotx_cm_add_connectivity(iotx_cm_conntext_t *cm_ctx, iotx_cm_connectivity_t *connectivity);

int iotx_cm_destory_connectivity(iotx_cm_conntext_t *cm_ctx, iotx_cm_connectivity_t **connectivity);

int iotx_cm_connect_connectivity_all(iotx_cm_conntext_t *cm_ctx);

#if (CONFIG_SDK_THREAD_COST == 1)
iotx_cm_process_list_node_t *iotx_cm_get_list_node(iotx_cm_conntext_t *cm_ctx, iotx_cm_connectivity_types_t type);

int iotx_cm_free_list_node(iotx_cm_conntext_t *cm_ctx, iotx_cm_connectivity_types_t type,
                           iotx_cm_process_list_node_t *node);

void iotx_cm_free_process_list_handler(void *list_node, va_list *params);

int iotx_cm_free_list_node_all(iotx_cm_conntext_t *cm_ctx);

/* node is in */
int iotx_cm_process_list_push(iotx_cm_conntext_t *cm_ctx, iotx_cm_connectivity_types_t type,
                              iotx_cm_process_list_node_t *node);

/* node is out */
iotx_cm_process_list_node_t *iotx_cm_process_list_pop(iotx_cm_conntext_t *cm_ctx, iotx_cm_connectivity_types_t type);

int iotx_cm_process_list_get_size(iotx_cm_conntext_t *cm_ctx, iotx_cm_connectivity_types_t type);

#endif /* CONFIG_SDK_THREAD_COST */

#endif /* __CM_CONNECTIVITY_H__ */
