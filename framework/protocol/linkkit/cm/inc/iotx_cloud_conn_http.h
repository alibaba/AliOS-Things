
#ifndef SRC_CLOUD_CONN_HTTP_H_
#define SRC_CLOUD_CONN_HTTP_H_

#include "iotx_cm_connection.h"

void* iotx_cloud_conn_http_init(void* handle);

int iotx_cloud_conn_http_subscribe(void *handle, const char *topic_filter, iotx_cm_message_ack_types_t ack_type);

int iotx_cloud_conn_http_unsubscribe(void *handle, const char *topic_filter);

int iotx_cloud_conn_http_publish(void *handle, void* _context, iotx_connection_msg_t* message);

int iotx_cloud_conn_http_deinit(void *handle);

int iotx_cloud_conn_http_yield(void *handle, int timeout_ms);

#endif /* SRC_CLOUD_CONN_HTTP_H_ */
