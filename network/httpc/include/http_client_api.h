/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdbool.h>
#include <stdint.h>
#include "http_client_config.h"

#ifndef HTTP_CLIENT_API_H
#define HTTP_CLIENT_API_H

typedef enum {
    GET,
    POST,
    PUT,
    DELETE,  // not support
    HEAD,  // not support
    OPTIONS,  // not support
} HTTPC_METHOD_TYPE_T;

enum {
    HTTPC_SUCCESS = 0,
    HTTPC_FAIL = -1,
};

typedef int32_t httpc_handle_t;

typedef int (* httpc_recv_fn)(httpc_handle_t httpc, uint8_t *body_buf, uint16_t body_len, int8_t err);

/* http client connection settings */
typedef struct httpc_connection_s {
    int socket;  // one socket per http session, not support multiple http sessions on one socket
    bool keep_alive;  // keep alive http connection or not
    // boot use_proxy;  // not support in this version
    httpc_recv_fn recv_fn;
    int timeout;
    char *server_name;
    int port;
} httpc_connection_t;

int8_t http_client_intialize(void);

httpc_handle_t httpc_init(httpc_connection_t *settings);

int8_t httpc_send_request(httpc_handle_t httpc, HTTPC_METHOD_TYPE_T method,
                          const char *uri, char *param, uint16_t param_len);

int8_t httpc_deinit(httpc_handle_t httpc);

#endif
