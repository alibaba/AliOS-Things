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

/**
 * Recv packets from SSL connection
 *
 * @param[in] httpc   http client context handle
 * @param[in] data    pointer to recv buffer space
 * @param[in] size    expected recv data size
 * @param[in] timeout recv timeout
 *
 * @return (0, len]          recv buffer len
 * @return HTTP_ENOTSUPP(-3) operation not support
 * @return HTTP_ECLSD(-7)    connection is closed
 * @retrun HTTP_ERECV(-8)    err happen when recv, caller is expected to retry later
 * @return HTTP_ETIMEOUT(-9) timeout when recv, caller is expected to retry later
 *
 */
int32_t httpc_wrapper_ssl_recv(httpc_handle_t httpc, uint8_t *data, uint32_t size, uint32_t timeout);
#endif

int32_t httpc_wrapper_connect(int socket, const struct sockaddr *name, socklen_t namelen);
int32_t httpc_wrapper_send(int socket, const void *data, uint16_t size, int flags);

/**
 * Recv packets from network connection
 *
 * @param[in] socket  socket fd
 * @param[in] data    pointer to recv buffer space
 * @param[in] size    expected recv data size
 * @param[in] timeout recv timeout
 *
 * @return (0, len]          recv buffer len
 * @return HTTP_ENOTSUPP(-3) operation not support
 * @return HTTP_ECLSD(-7)    connection is closed
 * @retrun HTTP_ERECV(-8)    err happen when recv, caller is expected to retry later
 * @return HTTP_ETIMEOUT(-9) timeout when recv, caller is expected to retry later
 *
 */
int32_t httpc_wrapper_recv(int32_t socket, uint8_t *data, uint32_t size, uint32_t timeout);

#endif  /* HTTP_CLIENT_WRAPPER_H */

