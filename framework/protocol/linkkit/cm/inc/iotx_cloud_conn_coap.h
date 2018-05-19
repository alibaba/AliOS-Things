
#ifndef SRC_CLOUD_CONN_COAP_H_
#define SRC_CLOUD_CONN_COAP_H_

#include "iotx_cm_connection.h"

void* iotx_cloud_conn_coap_init(void* handle);

int iotx_cloud_conn_coap_subscribe(void *handle, const char *topic_filter, iotx_cm_message_ack_types_t ack_type);

int iotx_cloud_conn_coap_unsubscribe(void *handle, const char *topic_filter);

int iotx_cloud_conn_coap_publish(void *handle, void* _context, iotx_connection_msg_t* message);

int iotx_cloud_conn_coap_deinit(void *handle);

int iotx_cloud_conn_coap_yield(void *handle, int timeout_ms);

#endif /* SRC_CLOUD_CONN_COAP_H_ */
