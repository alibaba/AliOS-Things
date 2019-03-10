/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>

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

/* http client connection settings */
typedef struct httpc_connection_s {
    bool keep_alive;  // keep alive http connection or not
    // boot use_proxy;  // not support in this version
    httpc_recv_fun recv_fn;
    int timeout;
    char *server_name;
    int port;
} httpc_connection_t;

typedef int32_t httpc_handle_t;

httpc_handle_t httpc_init(httpc_connection_t *settings);

typedef int (* httpc_recv_fn)(httpc_handle_t httpc, uint8_t *body_buf, uint16_t body_len, int err);

int httpc_send_request(int socket, httpc_handle_t httpc, HTTPC_METHOD_TYPE_T method,
                       const char *uri, char *hdr_name, char *hdr_data, char *param);

int httpc_deinit(httpc_handle_t httpc);

#endif
