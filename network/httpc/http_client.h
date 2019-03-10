/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "http_client_api.h"

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

/* http client connection handle */
typedef struct httpc_s {
    int socket;
    httpc_recv_fun recv_fn;
    uint16_t rx_state;
} httpc_t;

#endif
