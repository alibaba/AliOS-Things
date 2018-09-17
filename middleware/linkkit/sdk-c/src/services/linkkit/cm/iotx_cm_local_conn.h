/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */





#ifndef __CM_VIA_LOCAL_CONN_H__
#define __CM_VIA_LOCAL_CONN_H__

#ifdef CONFIG_CM_SUPPORT_LOCAL_CONN

#include "iot_import.h"

#include "iotx_utils.h"
#include "iotx_system.h"
#include "iotx_cm_common.h"

int iotx_cm_local_conn_disconnect_handler(iotx_cm_conntext_t *cm_ctx);
int iotx_cm_local_conn_reconnect_handler(iotx_cm_conntext_t *cm_ctx);
int iotx_cm_local_conn_found_device_handler(iotx_cm_conntext_t *cm_ctx, iotx_cm_local_device_t *device);
int iotx_cm_local_conn_remove_device_handler(iotx_cm_conntext_t *cm_ctx, iotx_cm_local_device_t *device);
int iotx_cm_local_conn_response_handler(iotx_cm_conntext_t *cm_ctx, iotx_cm_message_info_t *message_info);
int iotx_cm_local_conn_service_handler(iotx_cm_conntext_t *cm_ctx, char *URI, int result, int is_add);

void *iotx_cm_local_conn_init(void *handler, void *param);
int iotx_cm_local_conn_connect(void *handler, void *connectivity);
int iotx_cm_local_conn_init_second(void *handler, void *_connectivity);
int iotx_cm_local_conn_trigger_connected(void *handler, void *connectivity, iotx_cm_event_handle_fp_t event_fp,
        void *user_data);
int iotx_cm_local_conn_check_connected(void *handler, void *_connectivity, iotx_cm_event_handle_fp_t event_fp,
                                       void *user_data);
int iotx_cm_local_conn_register(void *handler, void *connectivity, void *register_param, int count);
int iotx_cm_local_conn_unregister(void *handler, void *connectivity, const char *topic_filter);
int iotx_cm_local_conn_add_service(void *handler, void *connectivity, const char *topic_filter,
                                   iotx_cm_message_auth_types_t auth_type);
int iotx_cm_local_conn_remove_service(void *handler, void *connectivity, const char *topic_filter);
int iotx_cm_local_conn_send(void *handler, void *connectivity, iotx_cm_send_peer_t *target, const char *topic_filter,
                            iotx_cm_message_ack_types_t ack_type, const void *payload, int payload_length, void *context, void *sem);
int iotx_cm_local_conn_add_subdevice(void *handler, void *connectivity, const char *pk, const char *dn);
int iotx_cm_local_conn_remove_subdevice(void *handler, void *connectivity, const char *pk, const char *dn);
int iotx_cm_local_conn_yield(void *handler, void *connectivity, int timeout_ms);
int iotx_cm_local_conn_deinit(void *handler, void *connectivity);
int iotx_cm_local_conn_check_target(iotx_cm_send_peer_t *peer);
iotx_cm_send_peer_t *iotx_cm_local_conn_get_target();
#if (CONFIG_SDK_THREAD_COST == 1)
int iotx_cm_local_conn_add_send(void *handler, iotx_cm_send_peer_t *target, iotx_cm_message_info_t *message_info,
                                void *sem);
void *iotx_cm_local_conn_process(void *pclient);
#endif /* CONFIG_SDK_THREAD_COST */

#endif /* CONFIG_CM_SUPPORT_LOCAL_CONN */

#endif /* __CM_VIA_LOCAL_CONN_H__ */


