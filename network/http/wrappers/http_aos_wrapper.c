/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <aos/kernel.h>
#include <network/network.h>
#include "http_wrapper.h"

#if CONFIG_HTTP_SECURE
#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#endif

httpc_wrapper_recv_fn_t g_recv_fn = 0;
httpc_t *g_httpc_sessions = NULL;

struct hostent *httpc_wrapper_gethostbyname(const char *name)
{
    return gethostbyname(name);
}

#if CONFIG_HTTP_SECURE
static int ssl_tx(void *context, const unsigned char *buf, size_t size)
{
    httpc_t *httpc_session = (httpc_t *)context;
    int ret = 0;

    ret = send(httpc_session->socket, buf, size, 0);
    //http_log("%s, size %d, ret %d", __func__, size, ret);
    return ret;
}

static int ssl_rx(void *context, unsigned char *buf, size_t size)
{
    httpc_t *httpc_session = (httpc_t *)context;
    int ret = 0;

    ret = recv(httpc_session->socket, buf, size, 0);
    //http_log("%s, size %d, ret %d", __func__, size, ret);
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

int httpc_wrapper_ssl_connect(int socket, const struct sockaddr *name, socklen_t namelen)
{
    httpc_t *http_session = NULL;
    uint8_t index = 0;
    int ret = 0;

    for (index = 0; index < CONFIG_HTTPC_SESSION_NUM; index++) {
        if (g_httpc_sessions[index].socket == socket) {
             break;
        }
    }
    if (index == CONFIG_HTTPC_SESSION_NUM) {
        return -1;
    }
    http_session = &g_httpc_sessions[index];
    if (http_session->https.is_inited == false) {
        mbedtls_ssl_init(&http_session->https.ssl.context);
        mbedtls_ssl_config_init(&http_session->https.ssl.conf);

        mbedtls_x509_crt_init(&http_session->https.ssl.ca_cert);
        if (http_session->https.ssl.ca_cert_c != NULL) {
            ret = mbedtls_x509_crt_parse(&http_session->https.ssl.ca_cert, http_session->https.ssl.ca_cert_c,
                                         strlen(http_session->https.ssl.ca_cert_c) + 1);
            if (ret != 0) {
                http_log("%s, mbedtls_x509_crt_parse err -0x%x", __func__, -ret);
                goto exit;
            }
        }

        mbedtls_ssl_conf_ca_chain(&http_session->https.ssl.conf,
                                  &http_session->https.ssl.ca_cert, NULL);
        mbedtls_ssl_conf_authmode(&http_session->https.ssl.conf, MBEDTLS_SSL_VERIFY_REQUIRED);

        ret = mbedtls_ssl_config_defaults(&http_session->https.ssl.conf,
                                          MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
                                          MBEDTLS_SSL_PRESET_DEFAULT);
        if (ret != 0) {
            http_log("%s, mbedtls_ssl_config_default err -0x%x", __func__, -ret);
            goto exit;
        }

        mbedtls_debug_set_threshold(3);
        mbedtls_ssl_conf_dbg(&http_session->https.ssl.conf, ssl_debug, NULL);

        mbedtls_ssl_conf_rng(&http_session->https.ssl.conf, ssl_random, NULL);
        ret = mbedtls_ssl_setup(&http_session->https.ssl.context, &http_session->https.ssl.conf);
        if (ret != 0) {
            http_log("%s, mbedtls_ssl_setup err -0x%x", __func__, -ret);
            goto exit;
        }

        http_session->https.is_inited = true;
    }

    ret = connect(socket, name, namelen);
    if (ret != 0) {
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
    return ret;
}

int httpc_wrapper_ssl_destroy(int socket)
{
    httpc_t *http_session = NULL;
    uint8_t index = 0;
    int ret = 0;

    for (index = 0; index < CONFIG_HTTPC_SESSION_NUM; index++) {
        if (g_httpc_sessions[index].socket == socket) {
             break;
        }
    }
    if (index == CONFIG_HTTPC_SESSION_NUM) {
        return -1;
    }
    http_session = &g_httpc_sessions[index];

    http_session->https.is_inited = false;

    mbedtls_ssl_close_notify(&(http_session->https.ssl.context));
    mbedtls_x509_crt_free(&(http_session->https.ssl.ca_cert));
    mbedtls_ssl_free(&(http_session->https.ssl.context));
    mbedtls_ssl_config_free(&(http_session->https.ssl.conf));
    return 0;
}

int httpc_wrapper_ssl_send(int socket, const void *data, uint16_t size, int flags)
{
    int ret = -1;
    uint8_t index = 0;
    httpc_t *httpc_sessions = g_httpc_sessions;

    for (index = 0; index < CONFIG_HTTPC_SESSION_NUM; index++) {
        if (httpc_sessions[index].socket == socket) {
            break;
        }
    }

    if (index == CONFIG_HTTPC_SESSION_NUM) {
        return -1;
    }

    ret = mbedtls_ssl_write(&httpc_sessions[index].https.ssl.context, data, size);
    return ret;
}

#endif

int httpc_wrapper_connect(int socket, const struct sockaddr *name, socklen_t namelen)
{
    return connect(socket, name, namelen);
}

int httpc_wrapper_send(int socket, const void *data, uint16_t size, int flags)
{
    return send(socket, data, size, flags);
}

#define CONFIG_HTTPC_RX_BUF_SIZE 2048
static void httpc_recv_thread(void *arg)
{
    httpc_t *httpc_sessions = (httpc_t *)arg;
    fd_set sets;
    uint8_t index = 0;
    int max_fd = -1;
    int ret;
    uint8_t buf[CONFIG_HTTPC_RX_BUF_SIZE];

    // TODO: support multiple http sessions
    while (1) {
        FD_ZERO(&sets);
        max_fd = -1;
        for (index = 0; index < CONFIG_HTTPC_SESSION_NUM; index++) {
            if (httpc_sessions[index].socket < 0) {
                continue;
            }
            if (max_fd < httpc_sessions[index].socket) {
                max_fd = httpc_sessions[index].socket;
            }
            FD_SET(httpc_sessions[index].socket, &sets);
        }
        if (max_fd == -1) {
            aos_msleep(1);
            continue;
        }
        ret = select(max_fd + 1, &sets, NULL, NULL, NULL);
        if (ret > 0) {
            for (index = 0; index < CONFIG_HTTPC_SESSION_NUM; index++) {
                if (httpc_sessions[index].socket < 0) {
                    continue;
                }
                if (FD_ISSET(httpc_sessions[index].socket, &sets)) {
#if CONFIG_HTTP_SECURE
                    if ((httpc_sessions[index].flags & HTTP_CLIENT_FLAG_SECURE) ==
                        HTTP_CLIENT_FLAG_SECURE) {
                        ret = mbedtls_ssl_read(&httpc_sessions[index].https.ssl.context,
                                               buf, CONFIG_HTTPC_RX_BUF_SIZE);
                    } else
#endif
                    {
                        ret = recv(httpc_sessions[index].socket, buf, CONFIG_HTTPC_RX_BUF_SIZE, 0);
                    }
                    if (ret > 0) {
                        g_recv_fn(&httpc_sessions[index], buf, (int32_t)ret);
                    } else if (ret == 0) {
                        // TODO: add exceptional logics
                    } else {
                        // TODO: add exceptional logics
                    }
                }
            }
        }
    }
}

int httpc_wrapper_register_recv(httpc_t *httpc, httpc_wrapper_recv_fn_t recv_fn)
{
    g_recv_fn = recv_fn;
    g_httpc_sessions = httpc;
    aos_task_new("httpc_recv", httpc_recv_thread, httpc, 4096);
    return HTTPC_SUCCESS;
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
