/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef SRC_CM_CONN_HTTP_H_
#define SRC_CM_CONN_HTTP_H_

#include "iotx_cm_connection.h"

void *iotx_cm_conn_http_init(void *handle, void *param);

int iotx_cm_conn_http_subscribe(void *handle, void *_register_param, int count);

int iotx_cm_conn_http_unsubscribe(void *handle, const char *topic_filter);

int iotx_cm_conn_http_send(void *handle, void *_context, iotx_connection_msg_t *message);

int iotx_cm_conn_http_deinit(void *handle);

int iotx_cm_conn_http_yield(void *handle, int timeout_ms);

#endif /* SRC_CM_CONN_HTTP_H_ */
