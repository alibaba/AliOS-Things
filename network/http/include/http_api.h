/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdbool.h>
#include <stdint.h>
#include "http_config.h"
#include "http_parser.h"

#ifndef HTTP_API_H
#define HTTP_API_H

enum {
    HTTPC_SUCCESS = 0,
    HTTPC_FAIL = -1,
};

enum {
    RX_CONTINUE = 1,
    RX_FINAL = 2,
};

typedef int32_t httpc_handle_t;

typedef int (* httpc_recv_fn)(httpc_handle_t httpc, uint8_t *buf, int32_t buf_size,
                              int32_t data_len, int32_t ret);

/* http client connection settings */
typedef struct httpc_connection_s {
    int socket;  // one socket per http session, not support multiple http sessions on one socket
    bool keep_alive;  // keep alive http connection or not
    // boot use_proxy;  // not support in this version
    int timeout;  // rx timeout in ms
    char *server_name;  // pointer to server name
#if CONFIG_HTTP_SECURE
    const char *ca_cert;  // pointer to ca certificate
#endif
    int port;

    httpc_recv_fn recv_fn;
    uint8_t *rsp_buf;  // pointer to response buffer
    int32_t rsp_buf_size;  // response buffer size
} httpc_connection_t;

int8_t http_client_intialize(void);

httpc_handle_t httpc_init(httpc_connection_t *settings);

int32_t httpc_send_request(httpc_handle_t httpc, int method, const char *uri,
                          const char *hdr, const char *content_type, const char *param, uint16_t param_len);
int32_t httpc_construct_header(char *buf, uint16_t buf_size, const char *name, const char *data);

int8_t httpc_deinit(httpc_handle_t httpc);

#endif
