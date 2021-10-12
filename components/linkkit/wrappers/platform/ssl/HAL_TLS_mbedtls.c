/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "sl_config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <lwip/netdb.h>
#include "mbedtls/error.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/pk.h"
#include "mbedtls/debug.h"
#include "mbedtls/platform.h"
#include "mbedtls/x509_crt.h"
#include "iot_import.h"
#include "sal_sockets.h"
#include <netdb.h>

typedef struct _TLSDataParams {
    mbedtls_ssl_context ssl;  /**< mbed TLS control context. */
    mbedtls_net_context fd;   /**< mbed TLS network context. */
    mbedtls_ssl_config conf;  /**< mbed TLS configuration context. */
    mbedtls_x509_crt cacertl; /**< mbed TLS CA certification. */
    mbedtls_x509_crt clicert; /**< mbed TLS Client certification. */
    mbedtls_pk_context pkey;  /**< mbed TLS Client key. */
} TLSDataParams_t, *TLSDataParams_pt;

#define SSL_LOGD(format, ...) LOGD("tls", format, ##__VA_ARGS__)
#define SSL_LOG(format, ...)  LOGI("tls", format, ##__VA_ARGS__)
#define SSL_LOGE(format, ...) LOGE("tls", format, ##__VA_ARGS__)

#define DEBUG_LEVEL           2

static void _net_init(mbedtls_net_context *ctx)
{
    ctx->fd = -1;
}

static int _net_connect(mbedtls_net_context *ctx, const char *host,
                        const char *port, int proto)
{
    int ret;
    struct addrinfo hints, *addr_list, *cur;

    /* Do name resolution with both IPv6 and IPv4 */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype =
        proto == MBEDTLS_NET_PROTO_UDP ? SOCK_DGRAM : SOCK_STREAM;
    hints.ai_protocol =
        proto == MBEDTLS_NET_PROTO_UDP ? IPPROTO_UDP : IPPROTO_TCP;

    if (getaddrinfo(host, port, &hints, &addr_list) != 0)
        return MBEDTLS_ERR_NET_UNKNOWN_HOST;

    /* Try the sockaddrs until a connection succeeds */
    ret = MBEDTLS_ERR_NET_UNKNOWN_HOST;
    for (cur = addr_list; cur != NULL; cur = cur->ai_next) {
        ctx->fd =
            (int)socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (ctx->fd < 0) {
            ret = MBEDTLS_ERR_NET_SOCKET_FAILED;
            continue;
        }

        if (connect(ctx->fd, cur->ai_addr, (socklen_t)cur->ai_addrlen) == 0) {
            ret = 0;
            break;
        }

        close(ctx->fd);
        ret = MBEDTLS_ERR_NET_CONNECT_FAILED;
    }

    freeaddrinfo(addr_list);

    return ret;
}

static void _net_free(mbedtls_net_context *ctx)
{
    if (ctx->fd == -1)
        return;

    shutdown(ctx->fd, 2);
    close(ctx->fd);

    ctx->fd = -1;
}

static int g_send_timeout_ms = 1000;
static int _net_send(void *ctx, const unsigned char *buf, size_t len)
{
    int ret;

    int fd = ((mbedtls_net_context *)ctx)->fd;
    struct timeval interval = { g_send_timeout_ms / 1000,
                                (g_send_timeout_ms % 1000) * 1000 };

    if (fd < 0)
        return MBEDTLS_ERR_NET_INVALID_CONTEXT;

    if (interval.tv_sec < 0 ||
        (interval.tv_sec == 0 && interval.tv_usec <= 100)) {
        interval.tv_sec = 0;
        interval.tv_usec = 10000;
    }

    /*set send timeout to avoid send block*/
    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&interval,
                   sizeof(struct timeval))) {
        return -1;
    }

    ret = (int)send(fd, buf, len, 0);

    if (ret < 0) {
        if (errno == EPIPE || errno == ECONNRESET)
            return MBEDTLS_ERR_NET_CONN_RESET;

        if (errno == EINTR)
            return MBEDTLS_ERR_SSL_WANT_WRITE;

        return MBEDTLS_ERR_NET_SEND_FAILED;
    }

    return ret;
}

static int _net_recv(void *ctx, unsigned char *buf, size_t len)
{
    int ret;
    int fd = ((mbedtls_net_context *)ctx)->fd;

    if (fd < 0)
        return MBEDTLS_ERR_NET_INVALID_CONTEXT;

    ret = (int)recv(fd, buf, len, 0);

    if (ret < 0) {
        if (errno == EPIPE || errno == ECONNRESET)
            return MBEDTLS_ERR_NET_CONN_RESET;

        if (errno == EINTR)
            return MBEDTLS_ERR_SSL_WANT_READ;

        return MBEDTLS_ERR_NET_RECV_FAILED;
    }

    return ret;
}

static int _net_recv_timeout(void *ctx, unsigned char *buf, size_t len,
                             uint32_t timeout)
{
    int ret;
    struct timeval tv;
    fd_set read_fds;
    int fd = ((mbedtls_net_context *)ctx)->fd;

    if (fd < 0)
        return MBEDTLS_ERR_NET_INVALID_CONTEXT;

    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    /* no wait if timeout == 0 */
    ret = select(fd + 1, &read_fds, NULL, NULL, &tv);

    /* Zero fds ready means we timed out */
    if (ret == 0)
        return MBEDTLS_ERR_SSL_TIMEOUT;

    if (ret < 0) {
        if (errno == EINTR)
            return MBEDTLS_ERR_SSL_WANT_READ;

        return MBEDTLS_ERR_NET_RECV_FAILED;
    }

    /* This call will not block */
    return _net_recv(ctx, buf, len);
}

// #include "drv_tee.h"
static int _ssl_random(void *p_rng, unsigned char *output, size_t output_len)
{
#ifdef CONFIG_TEE
    (void)p_rng;

    csi_tee_rand_generate(output, output_len);
#else
    while (output_len--) {
        *(output++) = (uint8_t)(rand() % 255);
    }
#endif
    return 0;
}

static void _ssl_debug(void *ctx, int level, const char *file, int line,
                       const char *str)
{
    ((void)level);
    if (NULL != ctx) {
        fprintf((FILE *)ctx, "%s:%04d: %s", file, line, str);
        fflush((FILE *)ctx);
    }
}

static int _real_confirm(int verify_result)
{
    SSL_LOG("certverify ret 0x%02x", verify_result);

#if defined(FORCE_SSL_VERIFY)
    if ((verify_result & MBEDTLS_X509_BADCERT_EXPIRED) != 0) {
        SSL_LOGE("!f ERROR_CERTIFICATE_EXPIRED");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_REVOKED) != 0) {
        SSL_LOGE("!f srv cert revoked");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_CN_MISMATCH) != 0) {
        SSL_LOGE("!f CN mismatch");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_NOT_TRUSTED) != 0) {
        SSL_LOGE("!f not trusted");
        return -1;
    }
#endif

    return 0;
}

#if defined(MBEDTLS_DEBUG_C)
static int _ssl_parse_crt(mbedtls_x509_crt *crt)
{
    char buf[1024];
    mbedtls_x509_crt *local_crt = crt;
    int i = 0;
    while (local_crt) {
        mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", local_crt);
        {
            char str[512];
            const char *start, *cur;
            start = buf;
            for (cur = buf; *cur != '\0'; cur++) {
                if (*cur == '\n') {
                    size_t len = cur - start + 1;
                    if (len > 511) {
                        len = 511;
                    }
                    memcpy(str, start, len);
                    str[len] = '\0';
                    start = cur + 1;
                    HAL_Printf("%s", str);
                }
            }
        }
        SSL_LOGD("crt content:%u", (uint32_t)strlen(buf));
        local_crt = local_crt->next;
        i++;
    }
    return i;
}
#endif

static int _ssl_client_init(mbedtls_ssl_context *ssl,
                            mbedtls_net_context *tcp_fd,
                            mbedtls_ssl_config *conf,
                            mbedtls_x509_crt *crt509_ca, const char *ca_crt,
                            size_t ca_len, mbedtls_x509_crt *crt509_cli,
                            const char *cli_crt, size_t cli_len,
                            mbedtls_pk_context *pk_cli, const char *cli_key,
                            size_t key_len, const char *cli_pwd, size_t pwd_len)
{
    int ret = -1;

    /*
     * 0. Initialize the RNG and the session data
     */
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold((int)DEBUG_LEVEL);
#endif
    _net_init(tcp_fd);
    mbedtls_ssl_init(ssl);
    mbedtls_ssl_config_init(conf);
    mbedtls_x509_crt_init(crt509_ca);

/* c-sky add: should init the crt before error return */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_CERTS_C)
    mbedtls_x509_crt_init(crt509_cli);
    mbedtls_pk_init(pk_cli);
#endif
#endif /* MBEDTLS_X509_CRT_PARSE_C */

    /*verify_source->trusted_ca_crt==NULL
     * 0. Initialize certificates
     */

    SSL_LOGD("LD CA root Cert");
    if (NULL != ca_crt) {
        ret = mbedtls_x509_crt_parse(
                      crt509_ca, (const unsigned char *)ca_crt, ca_len);
        if (ret != 0) {
            SSL_LOGE("x509 -0x%04x", -ret);
            return ret;
        }
    }
#if defined(MBEDTLS_DEBUG_C)
    _ssl_parse_crt(crt509_ca);
#endif
    // SSL_LOG("ok(%d skip)", ret);

    /* Setup Client Cert/Key */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if (cli_crt != NULL && cli_key != NULL) {
#if defined(MBEDTLS_CERTS_C)
        SSL_LOGD("start crt_parse");
        ret = mbedtls_x509_crt_parse(crt509_cli, (const unsigned char *)cli_crt,
                                     cli_len);
#else
        {
            ret = 1;
            SSL_LOG("MBEDTLS_CERTS_C not def");
        }
#endif
        if (ret != 0) {
            SSL_LOGE("crt parse -0x%x\n", -ret);
            return ret;
        }

#if defined(MBEDTLS_CERTS_C)
        SSL_LOGD("Start pk_parse_key[%s]", cli_pwd);
        ret = mbedtls_pk_parse_key(pk_cli, (const unsigned char *)cli_key,
                                   key_len, (const unsigned char *)cli_pwd,
                                   pwd_len);
#else
        {
            ret = 1;
            SSL_LOG("MBEDTLS_CERTS_C not def");
        }
#endif

        if (ret != 0) {
            SSL_LOGE("pk parse -0x%x\n", -ret);
            return ret;
        }
    }
#endif /* MBEDTLS_X509_CRT_PARSE_C */

    return 0;
}

/**
 * @brief This function connects to the specific SSL server with TLS, and
 * returns a value that indicates whether the connection is create successfully
 * or not. Call #NewNetwork() to initialize network structure before calling
 * this function.
 * @param[in] n is the the network structure pointer.
 * @param[in] addr is the Server Host name or IP address.
 * @param[in] port is the Server Port.
 * @param[in] ca_crt is the Server's CA certification.
 * @param[in] ca_crt_len is the length of Server's CA certification.
 * @param[in] client_crt is the client certification.
 * @param[in] client_crt_len is the length of client certification.
 * @param[in] client_key is the client key.
 * @param[in] client_key_len is the length of client key.
 * @param[in] client_pwd is the password of client key.
 * @param[in] client_pwd_len is the length of client key's password.
 * @sa #NewNetwork();
 * @return If the return value is 0, the connection is created successfully. If
 * the return value is -1, then calling lwIP #socket() has failed. If the return
 * value is -2, then calling lwIP #connect() has failed. Any other value
 * indicates that calling lwIP #getaddrinfo() has failed.
 */
static int _TLSConnectNetwork(TLSDataParams_t *pTlsData, const char *addr,
                              const char *port, const char *ca_crt,
                              size_t ca_crt_len, const char *client_crt,
                              size_t client_crt_len, const char *client_key,
                              size_t client_key_len, const char *client_pwd,
                              size_t client_pwd_len)
{
    int ret = -1;
    /*
     * 0. Init
     */
    ret = _ssl_client_init(&(pTlsData->ssl), &(pTlsData->fd),
                           &(pTlsData->conf), &(pTlsData->cacertl), ca_crt,
                           ca_crt_len, &(pTlsData->clicert), client_crt,
                           client_crt_len, &(pTlsData->pkey), client_key,
                           client_key_len, client_pwd, client_pwd_len)
    if (ret != 0) {
        SSL_LOGE("cli init -0x%04x", -ret);
        return ret;
    }

    /*
     * 2. Setup stuff
     */
    SSL_LOGD("SSL/TLS struct");
    ret = mbedtls_ssl_config_defaults(
             &(pTlsData->conf), MBEDTLS_SSL_IS_CLIENT,
             MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret != 0) {
        SSL_LOGE("sslcfg %d", ret);
        return ret;
    }

    mbedtls_ssl_conf_read_timeout(&(pTlsData->conf), 5000);

    mbedtls_ssl_conf_max_version(&pTlsData->conf, MBEDTLS_SSL_MAJOR_VERSION_3,
                                 MBEDTLS_SSL_MINOR_VERSION_3);
    mbedtls_ssl_conf_min_version(&pTlsData->conf, MBEDTLS_SSL_MAJOR_VERSION_3,
                                 MBEDTLS_SSL_MINOR_VERSION_3);

    // SSL_LOG("ok");

    /* OPTIONAL is not optimal for security, but makes interop easier in this
     * simplified example */
    if (ca_crt != NULL) {
#if defined(FORCE_SSL_VERIFY)
        mbedtls_ssl_conf_authmode(&(pTlsData->conf),
                                  MBEDTLS_SSL_VERIFY_REQUIRED);
#else
        mbedtls_ssl_conf_authmode(&(pTlsData->conf),
                                  MBEDTLS_SSL_VERIFY_OPTIONAL);
#endif
    } else {
        mbedtls_ssl_conf_authmode(&(pTlsData->conf), MBEDTLS_SSL_VERIFY_NONE);
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_ssl_conf_ca_chain(&(pTlsData->conf), &(pTlsData->cacertl), NULL);

    ret = mbedtls_ssl_conf_own_cert(
             &(pTlsData->conf), &(pTlsData->clicert), &(pTlsData->pkey));
    if (ret != 0) {
        SSL_LOG("!f conf_own_cert ret%d\n", ret);
        return ret;
    }
#endif
    mbedtls_ssl_conf_rng(&(pTlsData->conf), _ssl_random, NULL);
    mbedtls_ssl_conf_dbg(&(pTlsData->conf), _ssl_debug, NULL);
    /* mbedtls_ssl_conf_dbg( &(pTlsData->conf), _ssl_debug, stdout ); */

    ret = mbedtls_ssl_setup(&(pTlsData->ssl), &(pTlsData->conf));
    if (ret != 0) {
        SSL_LOGE("ssl_setup %d", ret);
        return ret;
    }
    mbedtls_ssl_set_hostname(&(pTlsData->ssl), addr);
    mbedtls_ssl_set_bio(&(pTlsData->ssl), &(pTlsData->fd), _net_send, _net_recv,
                        _net_recv_timeout);

    /*
     * 1. Start the connection
     */
    SSL_LOG("Conn /%s/%s", addr, port);
    ret = _net_connect(&(pTlsData->fd), addr, port,
                                 MBEDTLS_NET_PROTO_TCP);
    if (ret != 0) {
        SSL_LOGE("conn -0x%04x", -ret);
        return ret;
    }
    // SSL_LOG(" ok");

    /*
     * 4. Handshake
     */
    SSL_LOGD("Handshake");

    ret = mbedtls_ssl_handshake(&(pTlsData->ssl));
    while (ret != 0) {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) &&
            (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            SSL_LOGE("hs -0x%04x", -ret);
            return ret;
        }
    }
    // SSL_LOG(" ok");
    /*
     * 5. Verify the server certificate
     */
    SSL_LOGD("Verify X.509");
    ret = _real_confirm(
                  mbedtls_ssl_get_verify_result(&(pTlsData->ssl)));
    if (ret != 0) {
        SSL_LOGE("cfm -0x%04x", -ret);
        return ret;
    }
    /* n->my_socket = (int)((n->tlsdataparams.fd).fd); */
    /* WRITE_IOT_DEBUG_LOG("my_socket=%d", n->my_socket); */

    return 0;
}

static int _network_ssl_read(TLSDataParams_t *pTlsData, char *buffer, int len,
                             uint32_t timeout_ms)
{
    uint32_t readLen = 0;
    static int net_status = 0;
    int ret = -1;

    mbedtls_ssl_conf_read_timeout(&(pTlsData->conf), timeout_ms);
    while (readLen < len) {
        ret = mbedtls_ssl_read(&(pTlsData->ssl),
                               (unsigned char *)(buffer + readLen),
                               (len - readLen));
        if (ret > 0) {
            readLen += ret;
            net_status = 0;
        } else if (ret == 0) {
            /* if ret is 0 and net_status is -2, indicate the connection is
             * closed during last call */
            return (net_status == -2) ? net_status : readLen;
        } else {
            if (MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY == ret) {
                SSL_LOGE("peer close -0x%04x", -ret);
                net_status = -2; /* connection is closed */
                break;
            } else if ((MBEDTLS_ERR_SSL_TIMEOUT == ret) ||
                       (MBEDTLS_ERR_SSL_CONN_EOF == ret) ||
                       (MBEDTLS_ERR_SSL_SESSION_TICKET_EXPIRED == ret) ||
                       (MBEDTLS_ERR_SSL_NON_FATAL == ret)) {
                /* read already complete */
                /* if call mbedtls_ssl_read again, it will return 0 (means EOF)
                 */

                return readLen;
            } else {
                SSL_LOGE("recv -0x%04x", -ret);
                net_status = -1;
                return -1; /* Connection error */
            }
        }
    }

    return (readLen > 0) ? readLen : net_status;
}

static int _network_ssl_write(TLSDataParams_t *pTlsData, const char *buffer,
                              int len, uint32_t timeout_ms)
{
    uint32_t writtenLen = 0;
    int ret = -1;

    while (writtenLen < len) {
        ret = mbedtls_ssl_write(&(pTlsData->ssl),
                                (unsigned char *)(buffer + writtenLen),
                                (len - writtenLen));
        if (ret > 0) {
            writtenLen += ret;
            continue;
        } else if (ret == 0) {
            SSL_LOGE("write timeout");
            return 0;
        } else {
            SSL_LOGE("write -0x%04x", -ret);
            return -1; /* Connnection error */
        }
    }

    return writtenLen;
}

static void _network_ssl_disconnect(TLSDataParams_t *pTlsData)
{
    mbedtls_ssl_close_notify(&(pTlsData->ssl));
    _net_free(&(pTlsData->fd));
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_free(&(pTlsData->cacertl));
    if ((pTlsData->pkey).pk_info != NULL) {
        SSL_LOG("need release crt&key");
#if defined(MBEDTLS_CERTS_C)
        mbedtls_x509_crt_free(&(pTlsData->clicert));
        mbedtls_pk_free(&(pTlsData->pkey));
#endif
    }
#endif
    mbedtls_ssl_free(&(pTlsData->ssl));
    mbedtls_ssl_config_free(&(pTlsData->conf));
    SSL_LOG("discon");
}

int HAL_SSL_Read(_IN_ uintptr_t handle, _OU_ char *buf, _OU_ int len,
                 _IN_ int timeout_ms)
{
    return _network_ssl_read((TLSDataParams_t *)handle, buf, len, timeout_ms);
}

int HAL_SSL_Write(_IN_ uintptr_t handle, _IN_ const char *buf, _IN_ int len,
                  _IN_ int timeout_ms)
{
    return _network_ssl_write((TLSDataParams_t *)handle, buf, len, timeout_ms);
}

int32_t HAL_SSL_Destroy(uintptr_t handle)
{
    if ((uintptr_t)NULL == handle) {
        SSL_LOGD("handle is NULL");
        return 0;
    }

    _network_ssl_disconnect((TLSDataParams_t *)handle);
    HAL_Free((void *)handle);
    return 0;
}

static ssl_hooks_t g_ssl_hooks = { HAL_Malloc, HAL_Free };
int HAL_SSLHooks_set(ssl_hooks_t *hooks)
{
    if (hooks == NULL || hooks->malloc == NULL || hooks->free == NULL) {
        return DTLS_INVALID_PARAM;
    }

    g_ssl_hooks.malloc = hooks->malloc;
    g_ssl_hooks.free = hooks->free;

    return DTLS_SUCCESS;
}

uintptr_t HAL_SSL_Establish(const char *host, uint16_t port, const char *ca_crt,
                            size_t ca_crt_len)
{
    char port_str[6];
    TLSDataParams_pt pTlsData;

    pTlsData = HAL_Malloc(sizeof(TLSDataParams_t));
    if (NULL == pTlsData) {
        return (uintptr_t)NULL;
    }

    sprintf(port_str, "%u", port);

    if (0 != _TLSConnectNetwork(pTlsData, host, port_str, ca_crt, ca_crt_len,
                                NULL, 0, NULL, 0, NULL, 0)) {
        mbedtls_x509_crt_free(&(pTlsData->cacertl));
#if defined(MBEDTLS_CERTS_C)
        mbedtls_x509_crt_free(&(pTlsData->clicert));
#endif
        if (pTlsData->ssl.hostname) {
            mbedtls_free(pTlsData->ssl.hostname);
            pTlsData->ssl.hostname = NULL;
        }
        _network_ssl_disconnect(pTlsData);
        HAL_Free(pTlsData);
        return 0;
    }

    return (uintptr_t)pTlsData;
}

void HAL_SSL_SetSendTimeout(int ms)
{
    g_send_timeout_ms = ms;
}
