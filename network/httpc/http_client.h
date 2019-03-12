/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "http_client_api.h"

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#define HTTP_CLIENT_FLAG_KEEP_ALIVE 0x00000001
#define HTTP_CLIENT_FLAG_NO_CACHE 0x00000004

#define HTTP_CLIENT_FLAG_SECURE 0x00000010
#define HTTP_CLIENT_FLAG_PORT 0x00000020

/* http client connection handle */
typedef struct httpc_s {
    int socket;
    uint8_t index;
    uint32_t flags;
    char server_name[CONFIG_HTTPC_SERVER_NAME_SIZE + 2];
    httpc_recv_fun recv_fn;
    uint16_t rx_state;
    uint16_t port;

    char header[CONFIG_HTTPC_HEADER_SIZE + 1];
    uint16_t header_len;
} httpc_t;

#define HTTPC_VERSION "HTTP/1.1"

#define HTTPC_CRLF "\r\n"

typedef struct httpc_param_s {
    char *param;
    uint16_t len;
} httpc_param_t;

#endif
