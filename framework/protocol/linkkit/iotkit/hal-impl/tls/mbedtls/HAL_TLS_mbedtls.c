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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#include "mbedtls/error.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/pk.h"
#include "mbedtls/debug.h"
#include "mbedtls/platform.h"
#include "lite-utils.h"

#include "iot_import.h"

#define SEND_TIMEOUT_SECONDS (10)

typedef struct _TLSDataParams {
    mbedtls_ssl_context ssl;          /**< mbed TLS control context. */
    mbedtls_net_context fd;           /**< mbed TLS network context. */
    mbedtls_ssl_config conf;          /**< mbed TLS configuration context. */
    mbedtls_x509_crt cacertl;         /**< mbed TLS CA certification. */
    mbedtls_x509_crt clicert;         /**< mbed TLS Client certification. */
    mbedtls_pk_context pkey;          /**< mbed TLS Client key. */
} TLSDataParams_t, *TLSDataParams_pt;

#define SSL_LOG(format, ...) \
    do { \
        HAL_Printf("[inf] %s(%d): "format"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);\
        fflush(stdout);\
    }while(0);


#define DEBUG_LEVEL 10

static int ssl_fd = -1;

int get_ssl_fd()
{
   return ssl_fd;
}

static unsigned int _avRandom()
{
    return (((unsigned int)rand() << 16) + rand());
}

static int _ssl_random(void *p_rng, unsigned char *output, size_t output_len)
{
    uint32_t rnglen = output_len;
    uint8_t   rngoffset = 0;

    while (rnglen > 0) {
        *(output + rngoffset) = (unsigned char)_avRandom() ;
        rngoffset++;
        rnglen--;
    }
    return 0;
}



static void _ssl_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    ((void) level);
    if (NULL != ctx) {
        fprintf((FILE *) ctx, "%s:%04d: %s", file, line, str);
        fflush((FILE *) ctx);
    }
}

static int _real_confirm(int verify_result)
{
    SSL_LOG("certificate verification result: 0x%02x", verify_result);

#if defined(FORCE_SSL_VERIFY)
    if ((verify_result & MBEDTLS_X509_BADCERT_EXPIRED) != 0) {
        SSL_LOG("! fail ! ERROR_CERTIFICATE_EXPIRED");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_REVOKED) != 0) {
        SSL_LOG("! fail ! server certificate has been revoked");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_CN_MISMATCH) != 0) {
        SSL_LOG("! fail ! CN mismatch");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_NOT_TRUSTED) != 0) {
        SSL_LOG("! fail ! self-signed or not signed by a trusted CA");
        return -1;
    }
#endif

    return 0;
}

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
        SSL_LOG("crt content:%u", (uint32_t)strlen(buf));
        local_crt = local_crt->next;
        i++;
    }
    return i;
}

static int _ssl_client_init(mbedtls_ssl_context *ssl,
                            mbedtls_net_context *tcp_fd,
                            mbedtls_ssl_config *conf,
                            mbedtls_x509_crt *crt509_ca, const char *ca_crt, size_t ca_len,
                            mbedtls_x509_crt *crt509_cli, const char *cli_crt, size_t cli_len,
                            mbedtls_pk_context *pk_cli, const char *cli_key, size_t key_len,  const char *cli_pwd, size_t pwd_len
                           )
{
    int ret = -1;

    /*
     * 0. Initialize the RNG and the session data
     */
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold((int)DEBUG_LEVEL);
#endif
    mbedtls_net_init(tcp_fd);
    mbedtls_ssl_init(ssl);
    mbedtls_ssl_config_init(conf);
    mbedtls_x509_crt_init(crt509_ca);

    /*verify_source->trusted_ca_crt==NULL
     * 0. Initialize certificates
     */

    SSL_LOG("Loading the CA root certificate ...");
    if (NULL != ca_crt) {
        if (0 != (ret = mbedtls_x509_crt_parse(crt509_ca, (const unsigned char *)ca_crt, ca_len))) {
            SSL_LOG(" failed ! x509parse_crt returned -0x%04x", -ret);
            return ret;
        }
    }
    _ssl_parse_crt(crt509_ca);
    SSL_LOG(" ok (%d skipped)", ret);


    /* Setup Client Cert/Key */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_CERTS_C)
    mbedtls_x509_crt_init(crt509_cli);
    mbedtls_pk_init(pk_cli);
#endif
    if (cli_crt != NULL && cli_key != NULL) {
#if defined(MBEDTLS_CERTS_C)
        SSL_LOG("start prepare client cert .");
        ret = mbedtls_x509_crt_parse(crt509_cli, (const unsigned char *) cli_crt, cli_len);
#else
        {
            ret = 1;
            SSL_LOG("MBEDTLS_CERTS_C not defined.");
        }
#endif
        if (ret != 0) {
            SSL_LOG(" failed!  mbedtls_x509_crt_parse returned -0x%x\n", -ret);
            return ret;
        }

#if defined(MBEDTLS_CERTS_C)
        SSL_LOG("start mbedtls_pk_parse_key[%s]", cli_pwd);
        ret = mbedtls_pk_parse_key(pk_cli, (const unsigned char *) cli_key, key_len, (const unsigned char *) cli_pwd, pwd_len);
#else
        {
            ret = 1;
            SSL_LOG("MBEDTLS_CERTS_C not defined.");
        }
#endif

        if (ret != 0) {
            SSL_LOG(" failed\n  !  mbedtls_pk_parse_key returned -0x%x\n", -ret);
            return ret;
        }
    }
#endif /* MBEDTLS_X509_CRT_PARSE_C */

    return 0;
}

#if defined(_PLATFORM_IS_LINUX_)
static int net_prepare(void)
{
#if ( defined(_WIN32) || defined(_WIN32_WCE) ) && !defined(EFIX64) && \
   !defined(EFI32)
    WSADATA wsaData;
    static int wsa_init_done = 0;

    if (wsa_init_done == 0) {
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
            return (MBEDTLS_ERR_NET_SOCKET_FAILED);
        }

        wsa_init_done = 1;
    }
#else
#if !defined(EFIX64) && !defined(EFI32)
    signal(SIGPIPE, SIG_IGN);
#endif
#endif
    return (0);
}


static int mbedtls_net_connect_timeout(mbedtls_net_context *ctx, const char *host,
                                       const char *port, int proto, unsigned int timeout)
{
    int ret;
    struct addrinfo hints, *addr_list, *cur;
    struct timeval sendtimeout;

    if ((ret = net_prepare()) != 0) {
        return (ret);
    }

    /* Do name resolution with both IPv6 and IPv4 */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = proto == MBEDTLS_NET_PROTO_UDP ? SOCK_DGRAM : SOCK_STREAM;
    hints.ai_protocol = proto == MBEDTLS_NET_PROTO_UDP ? IPPROTO_UDP : IPPROTO_TCP;

    if (getaddrinfo(host, port, &hints, &addr_list) != 0) {
        return (MBEDTLS_ERR_NET_UNKNOWN_HOST);
    }

    /* Try the sockaddrs until a connection succeeds */
    ret = MBEDTLS_ERR_NET_UNKNOWN_HOST;
    for (cur = addr_list; cur != NULL; cur = cur->ai_next) {
        ctx->fd = (int) socket(cur->ai_family, cur->ai_socktype,
                               cur->ai_protocol);
        if (ctx->fd < 0) {
            ret = MBEDTLS_ERR_NET_SOCKET_FAILED;
            continue;
        }

        sendtimeout.tv_sec = timeout;
        sendtimeout.tv_usec = 0;

        if (0 != setsockopt(ctx->fd, SOL_SOCKET, SO_SNDTIMEO, &sendtimeout, sizeof(sendtimeout))) {
            perror("setsockopt");
            SSL_LOG("setsockopt error");
        }

        SSL_LOG("setsockopt SO_SNDTIMEO timeout: %ds", sendtimeout.tv_sec);

        if (connect(ctx->fd, cur->ai_addr, cur->ai_addrlen) == 0) {
            ret = 0;
            break;
        }

        close(ctx->fd);
        ret = MBEDTLS_ERR_NET_CONNECT_FAILED;
    }

    freeaddrinfo(addr_list);

    return (ret);
}
#endif

/**
 * @brief This function connects to the specific SSL server with TLS, and returns a value that indicates whether the connection is create successfully or not. Call #NewNetwork() to initialize network structure before calling this function.
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
 * @return If the return value is 0, the connection is created successfully. If the return value is -1, then calling lwIP #socket() has failed. If the return value is -2, then calling lwIP #connect() has failed. Any other value indicates that calling lwIP #getaddrinfo() has failed.
 */
static int _TLSConnectNetwork(TLSDataParams_t *pTlsData, const char *addr, const char *port,
                              const char *ca_crt, size_t ca_crt_len,
                              const char *client_crt,   size_t client_crt_len,
                              const char *client_key,   size_t client_key_len,
                              const char *client_pwd, size_t client_pwd_len)
{
    int ret = -1;
    /*
     * 0. Init
     */
    if (0 != (ret = _ssl_client_init(&(pTlsData->ssl), &(pTlsData->fd), &(pTlsData->conf),
                                     &(pTlsData->cacertl), ca_crt, ca_crt_len,
                                     &(pTlsData->clicert), client_crt, client_crt_len,
                                     &(pTlsData->pkey), client_key, client_key_len, client_pwd, client_pwd_len))) {
        SSL_LOG(" failed ! ssl_client_init returned -0x%04x", -ret);
        return ret;
    }

    /*
     * 1. Start the connection
     */
    SSL_LOG("Connecting to /%s/%s...", addr, port);
#if defined(_PLATFORM_IS_LINUX_)
    if (0 != (ret = mbedtls_net_connect_timeout(&(pTlsData->fd), addr, port, MBEDTLS_NET_PROTO_TCP,
                    SEND_TIMEOUT_SECONDS))) {
        SSL_LOG(" failed ! net_connect returned -0x%04x", -ret);
        return ret;
    }
#else
    if (0 != (ret = mbedtls_net_connect(&(pTlsData->fd), addr, port, MBEDTLS_NET_PROTO_TCP))) {
        SSL_LOG(" failed ! net_connect returned -0x%04x", -ret);
        return ret;
    }
#endif

    /*
     * 2. Setup stuff
     */
    SSL_LOG("  . Setting up the SSL/TLS structure...");
    if ((ret = mbedtls_ssl_config_defaults(&(pTlsData->conf), MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        SSL_LOG(" failed! mbedtls_ssl_config_defaults returned %d", ret);
        return ret;
    }

    mbedtls_ssl_conf_max_version(&pTlsData->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
    mbedtls_ssl_conf_min_version(&pTlsData->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);

    SSL_LOG(" ok");

    /* OPTIONAL is not optimal for security, but makes interop easier in this simplified example */
    if (ca_crt != NULL) {
#if defined(FORCE_SSL_VERIFY)
        mbedtls_ssl_conf_authmode(&(pTlsData->conf), MBEDTLS_SSL_VERIFY_REQUIRED);
#else
        mbedtls_ssl_conf_authmode(&(pTlsData->conf), MBEDTLS_SSL_VERIFY_OPTIONAL);
#endif
    } else {
        mbedtls_ssl_conf_authmode(&(pTlsData->conf), MBEDTLS_SSL_VERIFY_NONE);
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_ssl_conf_ca_chain(&(pTlsData->conf), &(pTlsData->cacertl), NULL);

    if ((ret = mbedtls_ssl_conf_own_cert(&(pTlsData->conf), &(pTlsData->clicert), &(pTlsData->pkey))) != 0) {
        SSL_LOG(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n", ret);
        return ret;
    }
#endif
    mbedtls_ssl_conf_rng(&(pTlsData->conf), _ssl_random, NULL);
    mbedtls_ssl_conf_dbg(&(pTlsData->conf), _ssl_debug, NULL);
    /* mbedtls_ssl_conf_dbg( &(pTlsData->conf), _ssl_debug, stdout ); */

    if ((ret = mbedtls_ssl_setup(&(pTlsData->ssl), &(pTlsData->conf))) != 0) {
        SSL_LOG("failed! mbedtls_ssl_setup returned %d", ret);
        return ret;
    }
    mbedtls_ssl_set_hostname(&(pTlsData->ssl), addr);
    mbedtls_ssl_set_bio(&(pTlsData->ssl), &(pTlsData->fd), mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    /*
      * 4. Handshake
      */
    SSL_LOG("Performing the SSL/TLS handshake...");

    while ((ret = mbedtls_ssl_handshake(&(pTlsData->ssl))) != 0) {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            SSL_LOG("failed  ! mbedtls_ssl_handshake returned -0x%04x", -ret);
            return ret;
        }
    }
    SSL_LOG(" ok");
    /*
     * 5. Verify the server certificate
     */
    SSL_LOG("  . Verifying peer X.509 certificate..");
    if (0 != (ret = _real_confirm(mbedtls_ssl_get_verify_result(&(pTlsData->ssl))))) {
        SSL_LOG(" failed  ! verify result not confirmed.");
        return ret;
    }
    /* n->my_socket = (int)((n->tlsdataparams.fd).fd); */
    /* WRITE_IOT_DEBUG_LOG("my_socket=%d", n->my_socket); */

    return 0;

}

#include "errno.h"
static int _network_ssl_read(TLSDataParams_t *pTlsData, char *buffer, int len, int timeout_ms)
{
    uint32_t        readLen = 0;
    static int      net_status = 0;
    int             ret = -1;
    char            err_str[33];

    mbedtls_ssl_conf_read_timeout(&(pTlsData->conf), timeout_ms);
    while (readLen < len) {
        ret = mbedtls_ssl_read(&(pTlsData->ssl), (unsigned char *)(buffer + readLen), (len - readLen));
        if (ret > 0) {
            readLen += ret;
            net_status = 0;
        } else if (ret == 0) {
            /* if ret is 0 and net_status is -2, indicate the connection is closed during last call */
            return (net_status == -2) ? net_status : readLen;
        } else {
            if (MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY == ret) {
                //mbedtls_strerror(ret, err_str, sizeof(err_str));
                SSL_LOG("ssl recv error: code = %d, err_str = '%s'", ret, err_str);
                net_status = -2; /* connection is closed */
                break;
            } else if ((MBEDTLS_ERR_SSL_TIMEOUT == ret)
                       || (MBEDTLS_ERR_SSL_CONN_EOF == ret)
                       || (MBEDTLS_ERR_SSL_SESSION_TICKET_EXPIRED == ret)
                       || (MBEDTLS_ERR_SSL_NON_FATAL == ret)) {
                /* read already complete */
                /* if call mbedtls_ssl_read again, it will return 0 (means EOF) */

                return readLen;
            } else {
                //mbedtls_strerror(ret, err_str, sizeof(err_str));
                SSL_LOG("ssl recv error: code = %d, err_str = '%s'", ret, err_str);
       #ifdef CSP_LINUXHOST
                if (MBEDTLS_ERR_SSL_WANT_READ == ret && errno == EINTR) {
                    aos_msleep(200);
                    continue;
                }
       #endif
                net_status = -1;
                return -1; /* Connection error */
            }
        }
    }

    return (readLen > 0) ? readLen : net_status;
}

static int _network_ssl_write(TLSDataParams_t *pTlsData, const char *buffer, int len, int timeout_ms)
{
    uint32_t writtenLen = 0;
    int ret = -1;

    while (writtenLen < len) {
        ret = mbedtls_ssl_write(&(pTlsData->ssl), (unsigned char *)(buffer + writtenLen), (len - writtenLen));
        if (ret > 0) {
            writtenLen += ret;
            continue;
        } else if (ret == 0) {
            SSL_LOG("ssl write timeout");
            return 0;
        } else {
            char err_str[33];
            //mbedtls_strerror(ret, err_str, sizeof(err_str));
            SSL_LOG("ssl write fail, code=%d, str=%s", ret, err_str);
            return -1; /* Connnection error */
        }
    }

    return writtenLen;
}

static void _network_ssl_disconnect(TLSDataParams_t *pTlsData)
{
    mbedtls_ssl_close_notify(&(pTlsData->ssl));
    mbedtls_net_free(&(pTlsData->fd));
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_free(&(pTlsData->cacertl));
    if ((pTlsData->pkey).pk_info != NULL) {
        SSL_LOG("need release client crt&key");
#if defined(MBEDTLS_CERTS_C)
        mbedtls_x509_crt_free(&(pTlsData->clicert));
        mbedtls_pk_free(&(pTlsData->pkey));
#endif
    }
#endif
    mbedtls_ssl_free(&(pTlsData->ssl));
    mbedtls_ssl_config_free(&(pTlsData->conf));
    SSL_LOG("ssl_disconnect");
}

int32_t HAL_SSL_GetFd(uintptr_t handle)
{
    int32_t fd = -1;
    if ((uintptr_t)NULL == handle) {
           SSL_LOG("handle is NULL");
           return fd;
    }
    fd = ((TLSDataParams_t *)handle)->fd.fd;
    return fd;
}

int32_t HAL_SSL_Read(_IN_ uintptr_t handle, _OU_ char *buf, _OU_ int len, _IN_ int timeout_ms)
{
    return _network_ssl_read((TLSDataParams_t *)handle, buf, len, timeout_ms);
}


int32_t HAL_SSL_Write(_IN_ uintptr_t handle, _IN_ const char *buf, _IN_ int len, _IN_ int timeout_ms)
{
    return _network_ssl_write((TLSDataParams_t *)handle, buf, len, timeout_ms);
}

int32_t HAL_SSL_Destroy(uintptr_t handle)
{
    void *              ptr;

    if ((uintptr_t)NULL == handle) {
        SSL_LOG("handle is NULL");
        return 0;
    }

    _network_ssl_disconnect((TLSDataParams_t *)handle);
    ptr = (void *)handle;
    LITE_free(ptr);

    return 0;
}

uintptr_t HAL_SSL_Establish(const char *host,
                            uint16_t port,
                            const char *ca_crt,
                            size_t ca_crt_len)
{
    char                port_str[6];
    TLSDataParams_pt    pTlsData;

    //mbedtls_platform_set_calloc_free(LITE_calloc_routine, LITE_free_routine);

    pTlsData = LITE_malloc(sizeof(TLSDataParams_t), MEM_MAGIC, "HAL");
    if (NULL == pTlsData) {
        return (uintptr_t)NULL;
    }

    sprintf(port_str, "%u", port);

    if (0 != _TLSConnectNetwork(pTlsData, host, port_str, ca_crt, ca_crt_len, NULL, 0, NULL, 0, NULL, 0)) {
        mbedtls_x509_crt_free(&(pTlsData->cacertl));
        mbedtls_x509_crt_free(&(pTlsData->clicert));
        if (pTlsData->ssl.hostname) {
            mbedtls_free(pTlsData->ssl.hostname);
            pTlsData->ssl.hostname = NULL;
        }
        LITE_free(pTlsData);
        return 0;
    }
    ssl_fd = pTlsData->fd.fd;
    return (uintptr_t)pTlsData;
}
