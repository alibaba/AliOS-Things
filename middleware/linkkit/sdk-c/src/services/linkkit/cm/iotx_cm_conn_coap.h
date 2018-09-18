/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef SRC_CM_CONN_COAP_H_
#define SRC_CM_CONN_COAP_H_

#include "iotx_cm_connection.h"

#ifdef COAP_COMM_ENABLED

    void *iotx_cm_conn_coap_init(void *handle, void *param);

    int iotx_cm_conn_coap_subscribe(void *handle, void *_register_param, int count);

    int iotx_cm_conn_coap_unsubscribe(void *handle, const char *topic_filter);

    int iotx_cm_conn_coap_send(void *handle, void *_context, iotx_connection_msg_t *message);

    int iotx_cm_conn_coap_deinit(void *handle);

    int iotx_cm_conn_coap_yield(void *handle, int timeout_ms);

#endif /* COAP_COMM_ENABLED */

#endif /* SRC_CM_CONN_COAP_H_ */
