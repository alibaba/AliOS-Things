/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _IOTX_DM_CM_WRAPPER_H_
#define _IOTX_DM_CM_WRAPPER_H_

#include "iotx_cm.h"

void dm_cmw_event_callback(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data);
void dm_cmw_topic_callback(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
int dm_cmw_init(iotx_dm_device_secret_types_t secret_type, iotx_dm_cloud_domain_types_t domain_type);
int dm_cmw_deinit(void);
int dm_cmw_yield(int timeout_ms);
int dm_cmw_conn_cloud_mqtt_init(void **conn_handle);
int dm_cmw_conn_local_alcs_init(void **conn_handle);
int dm_cmw_conn_get_prototol_handle(void *conn_handle, void **protocol_handle);
int dm_cmw_conn_destroy(void **conn_handle);
int dm_cmw_cloud_register(void *conn_handle, char **uri, int count, void *user_data);
int dm_cmw_cloud_unregister(void *conn_handle, char *uri);
int dm_cmw_local_init_second(void *conn_handle);
int dm_cmw_local_add_service(void *conn_handle, char *uri, iotx_dm_message_auth_types_t auth_type, void *user_data);
int dm_cmw_local_remove_service(void *conn_handle, char *uri);
int dm_cmw_local_add_subdev(void *conn_handle, const char *product_key, const char *device_name);
int dm_cmw_local_remove_subdev(void *conn_handle, char *product_key, char *device_name);
int dm_cmw_send_to_all(char *uri, char *payload, int payload_len, void *user_data);
int dm_cmw_send_to_cloud(char *uri, char *payload, void *user_data);
int dm_cmw_send_to_local(char *uri, int uri_len, char *payload, int payload_len, void *user_data);
int dm_cmw_send_to_device(void *conn_handle, char *product_key, char *device_name, char *uri, char *payload,
                          void *user_data);

#endif
