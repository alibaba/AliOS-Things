/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#include "uvoice_os.h"

#ifndef LINKKIT_SSL_ENABLE

#include "mbedtls/config.h"
#include "mbedtls/error.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/pk.h"
#include "mbedtls/debug.h"
#include "mbedtls/platform.h"

#define SEND_TIMEOUT_SECONDS (10)

typedef struct _TLSDataParams
{
    mbedtls_ssl_context ssl;     /**< mbed TLS control context. */
    mbedtls_net_context fd;      /**< mbed TLS network context. */
    mbedtls_ssl_config  conf;    /**< mbed TLS configuration context. */
    mbedtls_x509_crt    cacertl; /**< mbed TLS CA certification. */
    mbedtls_x509_crt    clicert; /**< mbed TLS Client certification. */
    mbedtls_pk_context  pkey;    /**< mbed TLS Client key. */
} TLSDataParams_t, *TLSDataParams_pt;


#define DEBUG_LEVEL 1

static unsigned int _avRandom()
{
    uint64_t time_ms;
    
    time_ms = aos_now_ms();
    srandom((unsigned int)time_ms);

    return (((unsigned int)rand() << 16) + rand());
}

static int _ssl_random(void *p_rng, unsigned char *output, size_t output_len)
{
    uint32_t rnglen    = output_len;
    uint8_t  rngoffset = 0;

    while (rnglen > 0) {
        *(output + rngoffset) = (unsigned char)_avRandom();
        rngoffset++;
        rnglen--;
    }
    return 0;
}

static void _ssl_debug(void *ctx, int level, const char *file, int line,
                       const char *str)
{
    ((void)level);
    if (NULL != ctx) {
#if 0
        fprintf((FILE *) ctx, "%s:%04d: %s", file, line, str);
        fflush((FILE *) ctx);
#endif
        M_LOGI("%s\n", str);
    }
}

static int _real_confirm(int verify_result)
{
    M_LOGI("certificate verification result: 0x%02x\n", verify_result);

#if defined(FORCE_SSL_VERIFY)
    if ((verify_result & MBEDTLS_X509_BADCERT_EXPIRED) != 0) {
        M_LOGE("! fail ! ERROR_CERTIFICATE_EXPIRED\n");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_REVOKED) != 0) {
        M_LOGE("! fail ! server certificate has been revoked\n");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_CN_MISMATCH) != 0) {
        M_LOGE("! fail ! CN mismatch\n");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_NOT_TRUSTED) != 0) {
        M_LOGE("! fail ! self-signed or not signed by a trusted CA\n");
        return -1;
    }
#endif

    return 0;
}

static int _ssl_client_init(mbedtls_ssl_context *ssl,
                            mbedtls_net_context *tcp_fd,
                            mbedtls_ssl_config * conf,
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
#if !defined(VOICELOUDER_APP)
    //mbedtls_debug_set_threshold((int)DEBUG_LEVEL);
#endif
#endif
    mbedtls_net_init(tcp_fd);
    mbedtls_ssl_init(ssl);
    mbedtls_ssl_config_init(conf);
    mbedtls_x509_crt_init(crt509_ca);

    /*verify_source->trusted_ca_crt==NULL
     * 0. Initialize certificates
     */

    M_LOGI("Loading the CA root certificate ...\n");
    if (NULL != ca_crt) {
        if (0 != (ret = mbedtls_x509_crt_parse(
                    crt509_ca, (const unsigned char *)ca_crt, ca_len))) {
            M_LOGE(" failed ! x509parse_crt returned -0x%04x\n", -ret);
            return ret;
        }
    }
    M_LOGI(" ok (%d skipped)\n", ret);


    /* Setup Client Cert/Key */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_CERTS_C)
    mbedtls_x509_crt_init(crt509_cli);
    mbedtls_pk_init(pk_cli);
#endif
    if (cli_crt != NULL && cli_key != NULL) {
#if defined(MBEDTLS_CERTS_C)
        M_LOGI("start prepare client cert .\n");
        ret = mbedtls_x509_crt_parse(crt509_cli, (const unsigned char *)cli_crt,
                                     cli_len);
#else
        {
            ret = 1;
            M_LOGE("MBEDTLS_CERTS_C not defined.\n");
        }
#endif
        if (ret != 0) {
            M_LOGE("failed ! mbedtls_x509_crt_parse returned -0x%x\n",
                         -ret);
            return ret;
        }

#if defined(MBEDTLS_CERTS_C)
        M_LOGI("start mbedtls_pk_parse_key[%s]\n", cli_pwd);
        ret =
          mbedtls_pk_parse_key(pk_cli, (const unsigned char *)cli_key, key_len,
                               (const unsigned char *)cli_pwd, pwd_len);
#else
        {
            ret = 1;
            M_LOGE("MBEDTLS_CERTS_C not defined.\n");
        }
#endif

        if (ret != 0) {
            M_LOGE("failed ! mbedtls_pk_parse_key returned -0x%x\n",
                         -ret);
            return ret;
        }
    }
#endif /* MBEDTLS_X509_CRT_PARSE_C */

    return 0;
}

#if defined(_PLATFORM_IS_LINUX_)
static int net_prepare(void)
{
#if (defined(_WIN32) || defined(_WIN32_WCE)) && !defined(EFIX64) && \
  !defined(EFI32)
    WSADATA    wsaData;
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


static int mbedtls_net_connect_timeout(mbedtls_net_context *ctx,
                                       const char *host, const char *port,
                                       int proto, unsigned int timeout)
{
    int             ret;
    struct addrinfo hints, *addr_list, *cur;
    struct timeval  sendtimeout;

    if ((ret = net_prepare()) != 0) {
        return (ret);
    }

    /* Do name resolution with both IPv6 and IPv4 */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype =
      proto == MBEDTLS_NET_PROTO_UDP ? SOCK_DGRAM : SOCK_STREAM;
    hints.ai_protocol =
      proto == MBEDTLS_NET_PROTO_UDP ? IPPROTO_UDP : IPPROTO_TCP;
    hints.ai_socktype &= ~SOCK_NONBLOCK;
    if (getaddrinfo(host, port, &hints, &addr_list) != 0) {
        return (MBEDTLS_ERR_NET_UNKNOWN_HOST);
    }

    /* Try the sockaddrs until a connection succeeds */
    ret = MBEDTLS_ERR_NET_UNKNOWN_HOST;
    for (cur = addr_list; cur != NULL; cur = cur->ai_next) {
        ctx->fd =
          (int)socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (ctx->fd < 0) {
            ret = MBEDTLS_ERR_NET_SOCKET_FAILED;
            continue;
        }

        sendtimeout.tv_sec  = timeout;
        sendtimeout.tv_usec = 0;

        if (0 != setsockopt(ctx->fd, SOL_SOCKET, SO_SNDTIMEO, &sendtimeout,
                            sizeof(sendtimeout))) {
            M_LOGE("setsockopt SO_SNDTIMEO error\n");
        }

        if (0 != setsockopt(ctx->fd, SOL_SOCKET, SO_RECVTIMEO, &sendtimeout,
                            sizeof(sendtimeout))) {
            M_LOGE("setsockopt SO_RECVTIMEO error\n");
        }

        if (0 != setsockopt(ctx->fd, SOL_SOCKET, SO_SNDTIMEO, &sendtimeout,
                            sizeof(sendtimeout))) {
            M_LOGE("setsockopt SO_SNDTIMEO error\n");
        }

        M_LOGI("setsockopt SO_SNDTIMEO  SO_RECVTIMEO  SO_SNDTIMEO timeout: %ds\n",
                      sendtimeout.tv_sec);

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
    if (0 != (ret = _ssl_client_init(
                &(pTlsData->ssl), &(pTlsData->fd), &(pTlsData->conf),
                &(pTlsData->cacertl), ca_crt, ca_crt_len, &(pTlsData->clicert),
                client_crt, client_crt_len, &(pTlsData->pkey), client_key,
                client_key_len, client_pwd, client_pwd_len))) {
        M_LOGE(" failed ! ssl_client_init returned -0x%04x\n", -ret);
        return ret;
    }

    /*
     * 1. Start the connection
     */
    M_LOGI("Connecting to /%s/%s...\n", addr, port);
#if defined(_PLATFORM_IS_LINUX_)
    if (0 != (ret = mbedtls_net_connect_timeout(&(pTlsData->fd), addr, port,
                                                MBEDTLS_NET_PROTO_TCP,
                                                SEND_TIMEOUT_SECONDS))) {
        M_LOGE(" failed ! net_connect returned -0x%04x\n", -ret);
        return ret;
    }
#else
    if (0 != (ret = mbedtls_net_connect(&(pTlsData->fd), addr, port,
                                        MBEDTLS_NET_PROTO_TCP))) {
        M_LOGE(" failed ! net_connect returned -0x%04x\n", -ret);
        return ret;
    }
#endif
    M_LOGI(" ok\n");

    /*
     * 2. Setup stuff
     */
    M_LOGI("  . Setting up the SSL/TLS structure...\n");
    if ((ret = mbedtls_ssl_config_defaults(
           &(pTlsData->conf), MBEDTLS_SSL_IS_CLIENT,
           MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        M_LOGE(" failed! mbedtls_ssl_config_defaults returned %d\n", ret);
        return ret;
    }

    mbedtls_ssl_conf_max_version(&pTlsData->conf, MBEDTLS_SSL_MAJOR_VERSION_3,
                                 MBEDTLS_SSL_MINOR_VERSION_3);
    mbedtls_ssl_conf_min_version(&pTlsData->conf, MBEDTLS_SSL_MAJOR_VERSION_3,
                                 MBEDTLS_SSL_MINOR_VERSION_3);

    M_LOGI(" ok\n");

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

    if ((ret = mbedtls_ssl_conf_own_cert(
           &(pTlsData->conf), &(pTlsData->clicert), &(pTlsData->pkey))) != 0) {
        M_LOGE(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n",
                     ret);
        return ret;
    }
#endif
    mbedtls_ssl_conf_rng(&(pTlsData->conf), _ssl_random, NULL);
    mbedtls_ssl_conf_dbg(&(pTlsData->conf), _ssl_debug, NULL);
    /* mbedtls_ssl_conf_dbg( &(pTlsData->conf), _ssl_debug, stdout ); */

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    if ((ret = mbedtls_ssl_conf_max_frag_len( &(pTlsData->conf), MBEDTLS_SSL_MAX_FRAG_LEN_NONE)) != 0) {
        M_LOGE(" failed\n  ! mbedtls_ssl_conf_max_frag_len returned %d\n\n", ret);
        return ret;
    }
#endif

    if ((ret = mbedtls_ssl_setup(&(pTlsData->ssl), &(pTlsData->conf))) != 0) {
        M_LOGE("failed! mbedtls_ssl_setup returned %d\n", ret);
        return ret;
    }
    mbedtls_ssl_set_hostname(&(pTlsData->ssl), addr);
    mbedtls_ssl_set_bio(&(pTlsData->ssl), &(pTlsData->fd), mbedtls_net_send,
                        mbedtls_net_recv, mbedtls_net_recv_timeout);

    /*
     * 4. Handshake
     */
    M_LOGI("Performing the SSL/TLS handshake...\n");

    while ((ret = mbedtls_ssl_handshake(&(pTlsData->ssl))) != 0) {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) &&
            (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            M_LOGE("failed  ! mbedtls_ssl_handshake returned -0x%04x\n",
                         -ret);
            return ret;
        }
    }
    M_LOGI(" ok\n");
    /*
     * 5. Verify the server certificate
     */
    M_LOGI("  . Verifying peer X.509 certificate..\n");
    if (0 != (ret = _real_confirm(
                mbedtls_ssl_get_verify_result(&(pTlsData->ssl))))) {
        M_LOGE(" failed  ! verify result not confirmed.\n");
        return ret;
    }
    /* n->my_socket = (int)((n->tlsdataparams.fd).fd); */
    /* WRITE_IOT_DEBUG_LOG("my_socket=%d", n->my_socket); */

    return 0;
}

static int _network_ssl_read(TLSDataParams_t *pTlsData, char *buffer, int len,
                             int timeout_ms)
{
    uint32_t   readLen    = 0;
    static int net_status = 0;
    int        ret        = -1;
    // char            err_str[33] = {0};

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
                // mbedtls_strerror(ret, err_str, sizeof(err_str));
                M_LOGE("ssl recv error: code = %d\n", ret);
                net_status = -2; /* connection is closed */
                break;
            } else if ((MBEDTLS_ERR_SSL_TIMEOUT == ret) ||
                       (MBEDTLS_ERR_SSL_CONN_EOF == ret) ||
                       (MBEDTLS_ERR_SSL_SESSION_TICKET_EXPIRED == ret) ||
                       (MBEDTLS_ERR_SSL_WANT_READ == ret) ||
                       (MBEDTLS_ERR_SSL_NON_FATAL == ret)) {
                /* read already complete */
                /* if call mbedtls_ssl_read again, it will return 0 (means EOF)
                 */
                return readLen;
            }

            else {
                // mbedtls_strerror(ret, err_str, sizeof(err_str));
#ifdef CSP_LINUXHOST
                if (MBEDTLS_ERR_SSL_WANT_READ == ret && errno == EINTR) {
                    continue;
                }
#endif
                M_LOGE("ssl recv error: code = %d\n", ret);
                net_status = -1;
                return -1; /* Connection error */
            }
        }
    }

    return (readLen > 0) ? readLen : net_status;
}

static int _network_ssl_write(TLSDataParams_t *pTlsData, const char *buffer,
                              int len, int timeout_ms)
{
    uint32_t writtenLen = 0;
    int      ret        = -1;

    while (writtenLen < len) {
        ret = mbedtls_ssl_write(&(pTlsData->ssl),
                                (unsigned char *)(buffer + writtenLen),
                                (len - writtenLen));
        if (ret > 0) {
            writtenLen += ret;
            continue;
        } else if (ret == 0) {
            M_LOGE("ssl write timeout\n");
            return 0;
        } else {
            // mbedtls_strerror(ret, err_str, sizeof(err_str));
            M_LOGE("ssl write fail, code=%d\n", ret);
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
        M_LOGI("need release client crt&key\n");
#if defined(MBEDTLS_CERTS_C)
        mbedtls_x509_crt_free(&(pTlsData->clicert));
        mbedtls_pk_free(&(pTlsData->pkey));
#endif
    }
#endif
    mbedtls_ssl_free(&(pTlsData->ssl));
    mbedtls_ssl_config_free(&(pTlsData->conf));
    M_LOGI("ssl_disconnect\n");
}

#endif /* LINKKIT_SSL_ENABLE */

int32_t net_ssl_read(uintptr_t handle, char *buf, int len, int timeout_ms)
{
#ifdef LINKKIT_SSL_ENABLE
    return HAL_SSL_Read(handle, buf, len, timeout_ms);
#else
    return _network_ssl_read((TLSDataParams_t *)handle, buf, len, timeout_ms);
#endif
}

int32_t net_ssl_write(uintptr_t handle, const char *buf, int len,
                      int timeout_ms)
{
#ifdef LINKKIT_SSL_ENABLE
    return HAL_SSL_Write(handle, buf, len, timeout_ms);
#else
    return _network_ssl_write((TLSDataParams_t *)handle, buf, len, timeout_ms);
#endif
}

int32_t net_ssl_disconnect(uintptr_t handle)
{
#ifdef LINKKIT_SSL_ENABLE
	return HAL_SSL_Destroy(handle);
#else
    if ((uintptr_t)NULL == handle) {
        M_LOGE("handle is NULL\n");
        return 0;
    }

    _network_ssl_disconnect((TLSDataParams_t *)handle);
    snd_free((void *)handle);
    return 0;
#endif
}

uintptr_t net_ssl_connect(const char *host, uint16_t port, const char *ca_crt,
                            size_t ca_crt_len)
{
#ifdef LINKKIT_SSL_ENABLE
	return HAL_SSL_Establish(host, port, ca_crt, ca_crt_len);
#else
    char             port_str[6];
    TLSDataParams_pt pTlsData;

    pTlsData = snd_zalloc(sizeof(TLSDataParams_t), AFM_MAIN);
    if (NULL == pTlsData) {
        return (uintptr_t)NULL;
    }
    memset(pTlsData, 0x0, sizeof(TLSDataParams_t));

    sprintf(port_str, "%u", port);

    if (0 != _TLSConnectNetwork(pTlsData, host, port_str, ca_crt, ca_crt_len,
                                NULL, 0, NULL, 0, NULL, 0)) {
        M_LOGE("tls connect failed !\n");
        _network_ssl_disconnect(pTlsData);
        snd_free(pTlsData);
        return (uintptr_t)NULL;
    }

    return (uintptr_t)pTlsData;
#endif
}
