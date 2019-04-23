/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdbool.h>
#include <stdint.h>
#include "http_def_config.h"
#include "http_parser.h"

#ifndef HTTP_API_H
#define HTTP_API_H

enum {
    HTTP_SUCCESS = 0,
    HTTP_ENOBUFS = -1,  // buffer error
    HTTP_EARG = -2,  // illegal argument
    HTTP_ENOTSUPP = -3,  // not support
    HTTP_EDNS = -4,  // DNS fail
    HTTP_ECONN = -5,  // connect fail
    HTTP_ESEND = -6,  // send packet fail
    HTTP_ECLSD = -7,  // connect closed
    HTTP_ERECV = -8,  // recv packet fail
    HTTP_ETIMEOUT = -9,  // timeout
};

typedef int32_t httpc_handle_t;

/* http client connection settings */
typedef struct httpc_connection_s {
    int socket;  // one socket per http session, not support multiple http sessions on one socket
    bool keep_alive;  // keep alive http connection or not
    // boot use_proxy;  // not support in this version
    char *server_name;  // pointer to server name
#if CONFIG_HTTP_SECURE
    const char *ca_cert;  // pointer to ca certificate
#endif
    int port;

    uint8_t *req_buf;  // pointer to request buffer
    int32_t req_buf_size; // request buffer size
} httpc_connection_t;

typedef struct http_rsp_info_s {
    uint32_t rsp_len;
    uint8_t content_len_present:1;
    uint8_t body_present:1;
    uint8_t *body_start;
    uint8_t message_complete:1;
    uint8_t headers_complete:1;
} http_rsp_info_t;

int8_t http_client_initialize(void);

httpc_handle_t httpc_init(httpc_connection_t *settings);
int8_t httpc_deinit(httpc_handle_t httpc);

int32_t httpc_send_request(httpc_handle_t httpc, int32_t method, const char *uri,
                           const char *hdr, const char *content_type, const char *param, uint16_t param_len);
int32_t httpc_recv_response(httpc_handle_t httpc, uint8_t *rsp, uint32_t rsp_size,
                            http_rsp_info_t *info, uint32_t timeout);

int32_t httpc_construct_header(char *buf, uint16_t buf_size, const char *name, const char *data);

#endif
