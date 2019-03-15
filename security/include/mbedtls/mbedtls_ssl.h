/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_MBEDTLS_SSL_H
#define AOS_MBEDTLS_SSL_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <stdlib.h>
#include "aos/kernel.h"


/**
 * Create a ssl connect.
 *
 * @param[in]  tcp_fd       handle of the tcp socket.
 * @param[in]  ca_cert      CA.
 * @param[in]  ca_cert_len  length of the CA.
 *
 * @return  NULL: error; ssl_param: success.
 */
void *mbedtls_ssl_connect(void *tcp_fd, const char *ca_cert, int ca_cert_len);

/**
 * Send data through ssl.
 *
 * @param[in]  ssl     handle of the ssl.
 * @param[in]  buffer  data to send.
 * @param[in]  length  length of the data.
 *
 * @return  -1: error; others: length of the data be sended.
 */
int mbedtls_ssl_send(void *ssl, const char *buffer, int length);

/**
 * Recv data through ssl.
 *
 * @param[in]  ssl     handle of the ssl.
 * @param[in]  buffer  buffer to recv data.
 * @param[in]  length  the max size of the buffer.
 *
 * @return  -1: error; 0: EOF; others: length of the data be sended.
 */
int mbedtls_ssl_recv(void *ssl, char *buffer, int length);

/**
 * Close the ssl.
 *
 * @param[in]  ssl  handle to be closed.
 *
 * @return  0: success.
 */
int mbedtls_ssl_close(void *ssl);

#endif /* AOS_MBEDTLS_SSL_H */

