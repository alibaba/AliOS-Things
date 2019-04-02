/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */


#ifndef HTTP_CLIENT_WRAPPER_H
#define HTTP_CLIENT_WRAPPER_H

#include "http_client.h"

#ifdef BUILD_AOS
#include <network/network.h>
#endif

struct hostent *httpc_wrapper_gethostbyname(const char *name);

#if CONFIG_HTTP_SECURE
int32_t httpc_wrapper_ssl_connect(httpc_handle_t httpc, const struct sockaddr *name, socklen_t namelen);
int32_t httpc_wrapper_ssl_destroy(httpc_handle_t httpc);
int32_t httpc_wrapper_ssl_send(httpc_handle_t httpc, const void *data, uint16_t size, int flags);
int32_t httpc_wrapper_ssl_recv(httpc_handle_t httpc, uint8_t *data, int32_t size, int32_t timeout);
#endif

int32_t httpc_wrapper_connect(int socket, const struct sockaddr *name, socklen_t namelen);
int32_t httpc_wrapper_send(int socket, const void *data, uint16_t size, int flags);
int32_t httpc_wrapper_recv(int32_t socket, uint8_t *data, uint32_t size, uint32_t timeout);

#endif  /* HTTP_CLIENT_WRAPPER_H */

