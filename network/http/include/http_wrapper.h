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
int httpc_wrapper_ssl_connect(int socket, const struct sockaddr *name, socklen_t namelen);
int httpc_wrapper_connect(int socket, const struct sockaddr *name, socklen_t namelen);
int httpc_wrapper_send(int socket, const void *data, uint16_t size, int flags);

typedef int (* httpc_wrapper_recv_fn_t)(httpc_t *http_session, void *data, int32_t size);
int httpc_wrapper_register_recv(httpc_t *httpc, httpc_wrapper_recv_fn_t);

#endif  /* HTTP_CLIENT_WRAPPER_H */

