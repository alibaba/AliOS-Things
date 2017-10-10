/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "platform.h"
#include <aos/mbedtls_ssl.h>

void *platform_ssl_connect(_IN_ void *tcp_fd, _IN_ const char *server_cert,
                           _IN_ int server_cert_len)
{
    return mbedtls_ssl_connect(tcp_fd, server_cert, server_cert_len);;
}

int platform_ssl_send(_IN_ void *ssl, _IN_ const char *buffer, _IN_ int length)
{
    return mbedtls_ssl_send(ssl, buffer, length);
}

int platform_ssl_recv(_IN_ void *ssl, _IN_ char *buffer, _IN_ int length)
{
    return mbedtls_ssl_recv(ssl, buffer, length);
}

int platform_ssl_close(_IN_ void *ssl)
{
    return mbedtls_ssl_close(ssl);
}
