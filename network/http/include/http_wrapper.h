/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */


#ifndef HTTP_CLIENT_WRAPPER_H
#define HTTP_CLIENT_WRAPPER_H

#include "http_client.h"

#ifdef BUILD_AOS
#include <network/network.h>
#endif

/**
 * execute DNS to get host's IP address
 *
 * @param[in] name host URL
 *
 * @return IP   host's IP address
 * @return NULL DNS failed to get IP address
 *
 */
struct hostent *httpc_wrapper_gethostbyname(const char *name);

#if CONFIG_HTTP_SECURE

/**
 * establish SSL connection
 *
 * @param[in] httpc   http client context handle
 * @param[in] name    pointer to the address
 * @param[in] namelen address name length
 *
 * @return HTTP_SUCCESS   establish SSL connection success
 * @return HTTP_ECONN(-5) establish SSL connection fail
 *
 */
int32_t httpc_wrapper_ssl_connect(httpc_handle_t httpc, const struct sockaddr *name, socklen_t namelen);

/**
 * destroy SSL connection
 *
 * @param[in] httpc http client context handle
 *
 * @return HTTP_SUCCESS destroy SSL connection success
 *
 */
int32_t httpc_wrapper_ssl_destroy(httpc_handle_t httpc);

/**
 * send data packets over SSL connection
 *
 * @param[in] httpc http client context handle
 * @param[in] data  pointer to data to send
 * @param[in] size  size of data to send
 * @param[in] flags flags for this transmission, usually set to 0
 *
 * @return (0, size] send data length
 * @return < 0       send fail
 *
 */
int32_t httpc_wrapper_ssl_send(httpc_handle_t httpc, const void *data, uint16_t size, int flags);

/**
 * recv packets from SSL connection
 *
 * @param[in] httpc   http client context handle
 * @param[in] data    pointer to recv buffer space
 * @param[in] size    expected recv data size
 * @param[in] timeout recv timeout
 *
 * @return (0, size]         recv buffer length
 * @return HTTP_ENOTSUPP(-3) operation not support
 * @return HTTP_ECLSD(-7)    connection is closed
 * @retrun HTTP_ERECV(-8)    err happen when recv, caller is expected to retry later
 * @return HTTP_ETIMEOUT(-9) timeout when recv, caller is expected to retry later
 *
 */
int32_t httpc_wrapper_ssl_recv(httpc_handle_t httpc, uint8_t *data, uint32_t size, uint32_t timeout);
#endif

/**
 * establish network connection
 *
 * @param[in] socket  socket fd
 * @param[in] name    pointer to the address
 * @param[in] namelen address name length
 *
 * @return HTTP_SUCCESS establish connection success
 * @return < 0          establish connection fail
 *
 */
int32_t httpc_wrapper_connect(int socket, const struct sockaddr *name, socklen_t namelen);

/**
 * send data packets over network connection
 *
 * @param[in] socket socket fd
 * @param[in] data   pointer to data to send
 * @param[in] size   size of data to send
 * @param[in] flags  flags for this transmission, usually set to 0
 *
 * @return (0, size] send data length
 * @return < 0       send fail
 *
 */
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

