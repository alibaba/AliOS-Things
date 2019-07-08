/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include <aos/kernel.h>
#include <network/network.h>
#include "http_wrapper.h"

#if CONFIG_HTTP_SECURE
#if CONFIG_HTTP_SECURE_ITLS
#include "itls/ssl.h"
#include "itls/net.h"
#include "itls/debug.h"
#else
#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#endif
#endif

#define HTTP_WRAPPER_SEND_TIMEOUT 10000

httpc_t *g_httpc_sessions = NULL;

struct hostent *httpc_wrapper_gethostbyname(const char *name)
{
    return gethostbyname(name);
}

static int32_t recv_func(int32_t socket, uint8_t *data, uint32_t size, uint32_t timeout, httpc_handle_t httpc)
{
    httpc_t *http_session = (httpc_t *)httpc;
    struct timeval tv;
    fd_set sets;
    int32_t ret;
    int32_t recv_len;
    uint64_t ts_end;
    uint64_t ts_left;

    ts_end = aos_now_ms() + timeout;
    recv_len = 0;

    do {
        ts_left = 0;
        if (ts_end > aos_now_ms()) {
            ts_left = ts_end - (uint64_t)aos_now_ms();
        }

        if (ts_left == 0 && timeout != 0) {
            ret = HTTP_ETIMEOUT;
            break;
        }

        FD_ZERO(&sets);
        FD_SET(socket, &sets);
        tv.tv_sec = ts_left / 1000;
        tv.tv_usec = (ts_left % 1000) * 1000;
        ret = select(socket + 1, &sets, NULL, NULL, (ts_left == 0)? NULL: &tv);

        if (ret > 0) {
            if (FD_ISSET(socket, &sets)) {
                if (http_session == NULL) {
                    ret = recv(socket, data + recv_len, size - recv_len, 0);
                } else {
#if CONFIG_HTTP_SECURE
                    ret = mbedtls_ssl_read(&http_session->https.ssl.context, data + recv_len, size - recv_len);
#else
                    ret = HTTP_ENOTSUPP;
#endif
                }

                if (ret > 0) {
                    recv_len += ret;
                } else if (ret == 0) {
                    http_log("%s, fd %d closed", __func__, socket);
                    ret = HTTP_ECLSD;
                } else {
                    if ((EINTR == errno) || (EAGAIN == errno) || (EWOULDBLOCK == errno) ||
                        (EPROTOTYPE == errno) || (EALREADY == errno) || (EINPROGRESS == errno)) {
                        continue;
                    }
                    ret = HTTP_ERECV;
               }
            }
        } else if (ret == 0) {
            ret = HTTP_ETIMEOUT;
        } else {
            if (EINTR == errno
#if CONFIG_HTTP_SECURE
                || ret == MBEDTLS_ERR_SSL_WANT_READ
#endif
                ) {
                continue;
            }
            ret = HTTP_ERECV;
        }

        if (ret < 0) {
            break;
        }
    } while (recv_len < size);

    return (recv_len > 0? recv_len: ret);
}

static int32_t send_func(int32_t socket, const uint8_t *data, uint32_t size, int flags, httpc_handle_t httpc)
{
    httpc_t *http_session = (httpc_t *)httpc;
    struct timeval tv;
    fd_set sets;
    int32_t ret = HTTP_SUCCESS;
    int32_t sent_len;
    uint64_t ts_end;
    uint64_t ts_left;

    ts_end = aos_now_ms() + HTTP_WRAPPER_SEND_TIMEOUT;
    sent_len = 0;

    do {
        ts_left = 0;
        if (ts_end > aos_now_ms()) {
            ts_left = ts_end - (uint64_t)aos_now_ms();
        }

        if (ts_left == 0) {
            ret = HTTP_ETIMEOUT;
            break;
        }

        FD_ZERO(&sets);
        FD_SET(socket, &sets);
        tv.tv_sec = ts_left / 1000;
        tv.tv_usec = (ts_left % 1000) * 1000;
        ret = select(socket + 1, NULL, &sets, NULL, &tv);

        if (ret > 0) {
            if (FD_ISSET(socket, &sets)) {
                if (http_session == NULL) {
                    ret = send(socket, data + sent_len, size - sent_len, 0);
                } else {
#if CONFIG_HTTP_SECURE
                    ret = mbedtls_ssl_write(&http_session->https.ssl.context, data + sent_len, size - sent_len);
#else
                    ret = HTTP_ENOTSUPP;
#endif
                }
            }

            if (ret > 0) {
                sent_len += ret;
                ret = HTTP_SUCCESS;
            } else if (ret < 0) {
                if (EINTR == errno) {
                    continue;
                }
                ret = HTTP_ESEND;
           }
        } else if (ret == 0) {
            ret = HTTP_ETIMEOUT;
        } else {
            if (EINTR == errno) {
                continue;
            }
            ret = HTTP_ESEND;
        }

        if (ret < 0) {
            break;
        }
    } while (sent_len < size);

    return (ret == HTTP_SUCCESS? sent_len: ret);
}

#if CONFIG_HTTP_SECURE
static int ssl_tx(void *context, const unsigned char *buf, size_t size)
{
    httpc_t *httpc_session = (httpc_t *)context;
    int ret = 0;

    ret = send(httpc_session->socket, buf, size, 0);
    /* http_log("%s, size %d, ret %d", __func__, size, ret); */
    return ret;
}

static int ssl_rx(void *context, unsigned char *buf, size_t size)
{
    httpc_t *httpc_session = (httpc_t *)context;
    int ret = 0;

    ret = recv(httpc_session->socket, buf, size, 0);
    /* http_log("%s, size %d, ret %d", __func__, size, ret); */
    if (ret < 0) {
        ret = MBEDTLS_ERR_NET_RECV_FAILED;
    }

    return ret;
}

static int ssl_random(void *p_rng, unsigned char *output, size_t output_len)
{
    uint32_t rnglen = output_len;
    uint8_t rngoffset = 0;

    while (rnglen > 0) {
        *(output + rngoffset) = (uint8_t)rand();
        rngoffset++;
        rnglen--;
    }

    return 0;
}

static void ssl_debug(void *ctx, int level, const char *file,
                      int line, const char *str)
{
    (void)level;
    http_log("%s:%d, %s", file, line, str);
}

int32_t httpc_wrapper_ssl_connect(httpc_handle_t httpc,
                              const struct sockaddr *name, socklen_t namelen)
{
    httpc_t *http_session = (httpc_t *)httpc;
    int32_t ret = 0;
#if CONFIG_HTTP_SECURE_ITLS
    const char *product_key = NULL;
    const char *product_secret = NULL;
#endif

    if (http_session->https.is_inited == false) {
        mbedtls_ssl_init(&http_session->https.ssl.context);
        mbedtls_ssl_config_init(&http_session->https.ssl.conf);

#if CONFIG_HTTP_SECURE_ITLS
        product_key = (const char *)http_session->https.ssl.ca_cert_c;
        product_secret = (const char *)(http_session->https.ssl.ca_cert_c + strlen(product_key) + 1);
        ret = mbedtls_ssl_conf_auth_extra(&http_session->https.ssl.conf, product_key,
                                          strlen(product_key));
        if (ret != 0) {
            http_log("%s, mbedtls_ssl_conf_auth_extra err -0x%x", __func__, -ret);
            goto exit;
        }
        ret = mbedtls_ssl_conf_auth_token(&http_session->https.ssl.conf, product_secret,
                                          strlen(product_secret));
        if (ret != 0) {
            http_log("%s, mbedtls_ssl_conf_auth_token err -0x%x", __func__, -ret);
            goto exit;
        }
#else
        mbedtls_x509_crt_init(&http_session->https.ssl.ca_cert);
        if (http_session->https.ssl.ca_cert_c != NULL) {
            ret = mbedtls_x509_crt_parse(&http_session->https.ssl.ca_cert, http_session->https.ssl.ca_cert_c,
                                         strlen(http_session->https.ssl.ca_cert_c) + 1);
            if (ret != 0) {
                http_log("%s, mbedtls_x509_crt_parse err -0x%x", __func__, -ret);
                goto exit;
            }
        }

        mbedtls_ssl_conf_ca_chain(&http_session->https.ssl.conf, &http_session->https.ssl.ca_cert, NULL);
        mbedtls_ssl_conf_authmode(&http_session->https.ssl.conf, MBEDTLS_SSL_VERIFY_REQUIRED);
#endif

        ret = mbedtls_ssl_config_defaults(&http_session->https.ssl.conf,
                                          MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
                                          MBEDTLS_SSL_PRESET_DEFAULT);
        if (ret != 0) {
            http_log("%s, mbedtls_ssl_config_default err -0x%x", __func__, -ret);
            goto exit;
        }

        mbedtls_ssl_conf_dbg(&http_session->https.ssl.conf, ssl_debug, NULL);
        mbedtls_ssl_conf_rng(&http_session->https.ssl.conf, ssl_random, NULL);
        ret = mbedtls_ssl_setup(&http_session->https.ssl.context, &http_session->https.ssl.conf);
        if (ret < 0) {
            http_log("%s, mbedtls_ssl_setup err -0x%x", __func__, -ret);
            goto exit;
        }

        http_session->https.is_inited = true;
    }

    ret = connect(http_session->socket, name, namelen);
    if (ret < 0) {
        goto exit;
    }

    mbedtls_ssl_session_reset(&http_session->https.ssl.context);
    mbedtls_ssl_set_bio(&http_session->https.ssl.context, http_session, ssl_tx, ssl_rx, NULL);

    do {
        ret = mbedtls_ssl_handshake(&http_session->https.ssl.context);
        if (ret != MBEDTLS_ERR_SSL_WANT_READ &&
            ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            if (ret == MBEDTLS_ERR_SSL_CONN_EOF) {
                goto exit;
            }
            if (ret < 0) {
                http_log("%s, mbedtls_ssl_handleshake err -0x%x", __func__, -ret);
                goto exit;
            }
        }
    } while (ret != 0);

exit:
    if (ret != 0) {
        ret = HTTP_ECONN;
        http_session->https.is_inited = false;
        mbedtls_ssl_close_notify(&(http_session->https.ssl.context));
#if CONFIG_HTTP_SECURE_ITLS == 0
        mbedtls_x509_crt_free(&(http_session->https.ssl.ca_cert));
#endif
        mbedtls_ssl_free(&(http_session->https.ssl.context));
        mbedtls_ssl_config_free(&(http_session->https.ssl.conf));
    }
    return ret;
}

int32_t httpc_wrapper_ssl_destroy(httpc_handle_t httpc)
{
    httpc_t *http_session = (httpc_t *)httpc;
    int ret = 0;

    http_session->https.is_inited = false;
    mbedtls_ssl_close_notify(&(http_session->https.ssl.context));
#if CONFIG_HTTP_SECURE_ITLS == 0
    mbedtls_x509_crt_free(&(http_session->https.ssl.ca_cert));
#endif
    mbedtls_ssl_free(&(http_session->https.ssl.context));
    mbedtls_ssl_config_free(&(http_session->https.ssl.conf));
    return HTTP_SUCCESS;
}

int32_t httpc_wrapper_ssl_send(httpc_handle_t httpc, const void *data, uint16_t size, int flags)
{
    httpc_t *http_session = (httpc_t *)httpc;
    return send_func(http_session->socket, data, size, flags, httpc);
}

int32_t httpc_wrapper_ssl_recv(httpc_handle_t httpc,
                               uint8_t *data, uint32_t size, uint32_t timeout)
{
    httpc_t *http_session = (httpc_t *)httpc;

    return recv_func(http_session->socket, data, size, timeout, httpc);
}
#endif

int32_t httpc_wrapper_connect(int socket, const struct sockaddr *name, socklen_t namelen)
{
    return connect(socket, name, namelen);
}

int32_t httpc_wrapper_send(int socket, const void *data, uint16_t size, int flags)
{
    return send_func(socket, data, size, flags, (httpc_handle_t)NULL);
}

int32_t httpc_wrapper_recv(int32_t socket, uint8_t *data, uint32_t size, uint32_t timeout)
{
    return recv_func(socket, data, size, timeout, (httpc_handle_t)NULL);
}

#if CONFIG_HTTP_ENABLE_MUTEX
static aos_mutex_t http_mutex;
#endif

void httpc_wrapper_init_mutex(void)
{
#if CONFIG_HTTP_ENABLE_MUTEX
    aos_mutex_new(&http_mutex);
#endif
}

void httpc_wrapper_lock_mutex(void)
{
#if CONFIG_HTTP_ENABLE_MUTEX
    aos_mutex_lock(&http_mutex, AOS_WAIT_FOREVER);
#endif
}

void httpc_wrapper_unlock_mutex(void)
{
#if CONFIG_HTTP_ENABLE_MUTEX
    aos_mutex_unlock(&http_mutex);
#endif
}

void http_log(const char *fmt, ...)
{
    va_list args;

    printf("[httpc][%06d] ", (unsigned)aos_now_ms());
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\r\n");
}
