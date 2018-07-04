#ifndef _IOTX_DM_CM_WRAPPER_H_
#define _IOTX_DM_CM_WRAPPER_H_

#include "iot_export_cm.h"
#include "iot_export_dm.h"

void iotx_dcw_event_callback(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data);
void iotx_dcw_topic_callback(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
int iotx_dcw_init(iotx_dm_device_secret_types_t secret_type, iotx_dm_cloud_domain_types_t domain_type);
int iotx_dcw_deinit(void);
int iotx_dcw_yield(int timeout_ms);
int iotx_dcw_conn_cloud_mqtt_init(void **conn_handle);
int iotx_dcw_conn_local_alcs_init(void **conn_handle);
int iotx_dcw_conn_destroy(void **conn_handle);
int iotx_dcw_cloud_register(void *conn_handle, char *uri, void *user_data);
int iotx_dcw_cloud_unregister(void *conn_handle, char *uri);
int iotx_dcw_local_add_service(void *conn_handle, char *uri, iotx_dm_message_auth_types_t auth_type, void *user_data);
int iotx_dcw_local_remove_service(void *conn_handle, char *uri);
int iotx_dcw_local_add_subdev(void *conn_handle, const char *product_key, const char *device_name);
int iotx_dcw_local_remove_subdev(void *conn_handle, char *product_key, char *device_name);
int iotx_dcw_send_to_cloud(char *uri, char *payload, void *user_data);
int iotx_dcw_send_to_device(void *conn_handle, char *product_key, char *device_name, char *uri, char *payload, void *user_data);

#endif
