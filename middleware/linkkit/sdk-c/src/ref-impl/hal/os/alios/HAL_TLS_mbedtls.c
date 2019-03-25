/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#include "aos/errno.h"
#include "aos/kernel.h"
#include "ulog/ulog.h"

#include "mbedtls/error.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/pk.h"
#include "mbedtls/debug.h"
#include "mbedtls/platform.h"
#include "iot_import.h"

#define LOG_TAG "HAL_TLS"



#define platform_info(format, ...) LOGI(LOG_TAG, format, ##__VA_ARGS__)
#define platform_err(format, ...) LOGE(LOG_TAG, format, ##__VA_ARGS__)

#define SEND_TIMEOUT_SECONDS (10)

#define DEBUG_LEVEL 10

static ssl_hooks_t g_ssl_hooks = {HAL_Malloc, HAL_Free};

typedef struct _TLSDataParams {
    mbedtls_ssl_context ssl;     /**< mbed TLS control context. */
    mbedtls_net_context fd;      /**< mbed TLS network context. */
    mbedtls_ssl_config  conf;    /**< mbed TLS configuration context. */
    mbedtls_x509_crt    cacertl; /**< mbed TLS CA certification. */
    mbedtls_x509_crt    clicert; /**< mbed TLS Client certification. */
    mbedtls_pk_context  pkey;    /**< mbed TLS Client key. */
} TLSDataParams_t, *TLSDataParams_pt;

#ifdef TLS_SAVE_TICKET

#define TLS_MAX_SESSION_BUF 384
#define KV_SESSION_KEY  "TLS_%s_%s"
static mbedtls_ssl_session *saved_session = NULL;

static int ssl_serialize_session(const mbedtls_ssl_session *session,
                                 unsigned char *buf, size_t buf_len,
                                 size_t *olen)
{
    unsigned char *p = buf;
    size_t left = buf_len;

    if (left < sizeof(mbedtls_ssl_session)) {
        return (MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL);
    }

    memcpy(p, session, sizeof(mbedtls_ssl_session));
    p += sizeof(mbedtls_ssl_session);
    left -= sizeof(mbedtls_ssl_session);
#if defined(MBEDTLS_SSL_SESSION_TICKETS) && defined(MBEDTLS_SSL_CLI_C)
    if (left < sizeof(mbedtls_ssl_session)) {
        return (MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL);
    }
    memcpy(p, session->ticket, session->ticket_len);
    p += session->ticket_len;
    left -= session->ticket_len;
#endif

    *olen = p - buf;

    return (0);
}

static int ssl_deserialize_session(mbedtls_ssl_session *session,
                                   const unsigned char *buf, size_t len)
{
    const unsigned char *p = buf;
    const unsigned char *const end = buf + len;

    if (sizeof(mbedtls_ssl_session) > (size_t)(end - p)) {
        return (MBEDTLS_ERR_SSL_BAD_INPUT_DATA);
    }

    memcpy(session, p, sizeof(mbedtls_ssl_session));
    p += sizeof(mbedtls_ssl_session);
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    session->peer_cert = NULL;
#endif

#if defined(MBEDTLS_SSL_SESSION_TICKETS) && defined(MBEDTLS_SSL_CLI_C)
    if(session->ticket_len > 0) {
        if (session->ticket_len > (size_t)(end - p)) {
            return (MBEDTLS_ERR_SSL_BAD_INPUT_DATA);
        }
        session->ticket = aos_malloc(session->ticket_len);
        if (session->ticket == NULL) {
            return (MBEDTLS_ERR_SSL_ALLOC_FAILED);
        }
        memcpy(session->ticket, p, session->ticket_len);
        p += session->ticket_len;
        platform_info("saved ticket len = %d...", session->ticket_len);
    }
#endif

    if (p != end) {
        return (MBEDTLS_ERR_SSL_BAD_INPUT_DATA);
    }

    return (0);
}
#endif

static unsigned int _avRandom()
{
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

static void _aos_srand(unsigned int seed)
{
#define SEED_MAGIC 0x123
    int           ret        = 0;
    int           seed_len   = 0;
    unsigned int  seed_val   = 0;
    static char  *g_seed_key = "seed_key";

    seed_len = sizeof(seed_val);
    ret = aos_kv_get(g_seed_key, &seed_val, &seed_len);
    if (ret) {
        seed_val = SEED_MAGIC;
    }
    seed_val += seed;
    srand(seed_val);

    seed_val = rand();
    aos_kv_set(g_seed_key, &seed_val, sizeof(seed_val), 1);
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
        platform_info("%s", str);
    }
}

static int _real_confirm(int verify_result)
{
    platform_info("certificate verification result: 0x%02x", verify_result);

#if defined(FORCE_SSL_VERIFY)
    if ((verify_result & MBEDTLS_X509_BADCERT_EXPIRED) != 0) {
        platform_err("! fail ! ERROR_CERTIFICATE_EXPIRED");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_REVOKED) != 0) {
        platform_err("! fail ! server certificate has been revoked");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_CN_MISMATCH) != 0) {
        platform_err("! fail ! CN mismatch");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_NOT_TRUSTED) != 0) {
        platform_err("! fail ! self-signed or not signed by a trusted CA");
        return -1;
    }
#endif

    return 0;
}

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
    mbedtls_net_init(tcp_fd);
    mbedtls_ssl_init(ssl);
    mbedtls_ssl_config_init(conf);
    mbedtls_x509_crt_init(crt509_ca);
    _aos_srand(aos_now_ms());
    /*verify_source->trusted_ca_crt==NULL
     * 0. Initialize certificates
     */

    platform_info("Loading the CA root certificate ...");
    if (NULL != ca_crt) {
        if (0 != (ret = mbedtls_x509_crt_parse(
                                    crt509_ca, (const unsigned char *)ca_crt, ca_len))) {
            platform_err(" failed ! x509parse_crt returned -0x%04x", -ret);
            return ret;
        }
    }
    platform_info(" ok (%d skipped)", ret);


    /* Setup Client Cert/Key */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_CERTS_C)
    mbedtls_x509_crt_init(crt509_cli);
    mbedtls_pk_init(pk_cli);
#endif
    if (cli_crt != NULL && cli_key != NULL) {
#if defined(MBEDTLS_CERTS_C)
        platform_info("start prepare client cert .");
        ret = mbedtls_x509_crt_parse(crt509_cli, (const unsigned char *)cli_crt,
                                     cli_len);
#else
        {
            ret = 1;
            platform_err("MBEDTLS_CERTS_C not defined.");
        }
#endif
        if (ret != 0) {
            platform_err(" failed!  mbedtls_x509_crt_parse returned -0x%x\n",
                         -ret);
            return ret;
        }

#if defined(MBEDTLS_CERTS_C)
        platform_info("start mbedtls_pk_parse_key[%s]", cli_pwd);
        ret =
                    mbedtls_pk_parse_key(pk_cli, (const unsigned char *)cli_key, key_len,
                                         (const unsigned char *)cli_pwd, pwd_len);
#else
        {
            ret = 1;
            platform_err("MBEDTLS_CERTS_C not defined.");
        }
#endif

        if (ret != 0) {
            platform_err(" failed\n  !  mbedtls_pk_parse_key returned -0x%x\n",
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
            platform_err("setsockopt SO_SNDTIMEO error");
        }

        if (0 != setsockopt(ctx->fd, SOL_SOCKET, SO_RECVTIMEO, &sendtimeout,
                            sizeof(sendtimeout))) {
            platform_err("setsockopt SO_RECVTIMEO error");
        }

        if (0 != setsockopt(ctx->fd, SOL_SOCKET, SO_SNDTIMEO, &sendtimeout,
                            sizeof(sendtimeout))) {
            platform_err("setsockopt SO_SNDTIMEO error");
        }

        platform_info("setsockopt SO_SNDTIMEO  SO_RECVTIMEO  SO_SNDTIMEO timeout: %ds",
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
        platform_err(" failed ! ssl_client_init returned -0x%04x", -ret);
        return ret;
    }

    /*
     * 1. Start the connection
     */
    platform_info("Connecting to /%s/%s...", addr, port);
#if defined(_PLATFORM_IS_LINUX_)
    if (0 != (ret = mbedtls_net_connect_timeout(&(pTlsData->fd), addr, port,
                    MBEDTLS_NET_PROTO_TCP,
                    SEND_TIMEOUT_SECONDS))) {
        platform_err(" failed ! net_connect returned -0x%04x", -ret);
        return ret;
    }
#else
    if (0 != (ret = mbedtls_net_connect(&(pTlsData->fd), addr, port,
                                        MBEDTLS_NET_PROTO_TCP))) {
        platform_err(" failed ! net_connect returned -0x%04x", -ret);
        return ret;
    }
#endif
    platform_info(" ok");

    /*
     * 2. Setup stuff
     */
    platform_info("  . Setting up the SSL/TLS structure...");
    if ((ret = mbedtls_ssl_config_defaults(
                           &(pTlsData->conf), MBEDTLS_SSL_IS_CLIENT,
                           MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        platform_err(" failed! mbedtls_ssl_config_defaults returned %d", ret);
        return ret;
    }

    mbedtls_ssl_conf_max_version(&pTlsData->conf, MBEDTLS_SSL_MAJOR_VERSION_3,
                                 MBEDTLS_SSL_MINOR_VERSION_3);
    mbedtls_ssl_conf_min_version(&pTlsData->conf, MBEDTLS_SSL_MAJOR_VERSION_3,
                                 MBEDTLS_SSL_MINOR_VERSION_3);

    platform_info(" ok");

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
        platform_err(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n",
                     ret);
        return ret;
    }
#endif
    mbedtls_ssl_conf_rng(&(pTlsData->conf), _ssl_random, NULL);
    mbedtls_ssl_conf_dbg(&(pTlsData->conf), _ssl_debug, NULL);
    /* mbedtls_ssl_conf_dbg( &(pTlsData->conf), _ssl_debug, stdout ); */

    if ((ret = mbedtls_ssl_setup(&(pTlsData->ssl), &(pTlsData->conf))) != 0) {
        platform_err("failed! mbedtls_ssl_setup returned %d", ret);
        return ret;
    }
#if defined(ON_PRE) || defined(ON_DAILY)
    platform_err("SKIPPING mbedtls_ssl_set_hostname() when ON_PRE or ON_DAILY defined!");
#else
    mbedtls_ssl_set_hostname(&(pTlsData->ssl), addr);
#endif
    mbedtls_ssl_set_bio(&(pTlsData->ssl), &(pTlsData->fd), mbedtls_net_send,
                        mbedtls_net_recv, mbedtls_net_recv_timeout);

#ifdef TLS_SAVE_TICKET
    if (NULL == saved_session) {
        do {
            int len = TLS_MAX_SESSION_BUF;
            char device_key[PRODUCT_KEY_MAXLEN + DEVICE_NAME_MAXLEN + 6] = {0};
            char _product_key[PRODUCT_KEY_LEN + 1] = {0};
            char _device_name[DEVICE_NAME_LEN + 1] = {0};
            unsigned char *save_buf = aos_malloc(TLS_MAX_SESSION_BUF);
            if (save_buf ==  NULL) {
                platform_err(" malloc failed");
                break;
            }

            saved_session = aos_malloc(sizeof(mbedtls_ssl_session));

            if (saved_session == NULL) {
                platform_err(" malloc failed");
                aos_free(save_buf);
                save_buf =  NULL;
                break;
            }


            memset(save_buf, 0x00, TLS_MAX_SESSION_BUF);
            memset(saved_session, 0x00, sizeof(mbedtls_ssl_session));

            HAL_GetProductKey(_product_key);
            HAL_GetDeviceName(_device_name);
            HAL_Snprintf(device_key, PRODUCT_KEY_MAXLEN + DEVICE_NAME_MAXLEN + 5, KV_SESSION_KEY, _product_key, _device_name);
            ret = HAL_Kv_Get(device_key, save_buf, &len);

            if (ret != 0 || len == 0) {
                platform_info(" kv get failed len=%d,ret = %d", len, ret);
                aos_free(saved_session);
                aos_free(save_buf);
                save_buf = NULL;
                saved_session = NULL;
                break;
            }
            ret = ssl_deserialize_session(saved_session, save_buf, len);
            if (ret < 0) {
                platform_err("ssl_deserialize_session err,ret = %d", ret);
                aos_free(saved_session);
                aos_free(save_buf);
                save_buf = NULL;
                saved_session = NULL;
                break;
            }
            aos_free(save_buf);
        } while (0);
    }

    if (NULL != saved_session) {
        saved_session->peer_cert = NULL;//for test
        mbedtls_ssl_set_session(&(pTlsData->ssl), saved_session);
        platform_info("use saved session!!");
    }
#endif
    /*
     * 4. Handshake
     */
    mbedtls_ssl_conf_read_timeout(&(pTlsData->conf), 10000);
    platform_info("Performing the SSL/TLS handshake...");

    while ((ret = mbedtls_ssl_handshake(&(pTlsData->ssl))) != 0) {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) &&
            (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            platform_err("failed  ! mbedtls_ssl_handshake returned -0x%04x",
                         -ret);
            return ret;
        }
    }
    platform_info(" ok");

#ifdef TLS_SAVE_TICKET
    if (NULL == saved_session) {
        do {
            size_t real_session_len = 0;
            unsigned char *save_buf = aos_malloc(TLS_MAX_SESSION_BUF); //for test
            if (save_buf ==  NULL) {
                break;
            }

            saved_session = aos_malloc(sizeof(mbedtls_ssl_session));
            if (NULL == saved_session) {
                aos_free(save_buf);
                break;
            }
            memset(save_buf, 0x00, sizeof(TLS_MAX_SESSION_BUF));
            memset(saved_session, 0x00, sizeof(mbedtls_ssl_session));

            ret = mbedtls_ssl_get_session(&(pTlsData->ssl), saved_session);
            if (ret != 0) {
                aos_free(save_buf);
                aos_free(saved_session);
                saved_session = NULL;
                break;
            }
            ret = ssl_serialize_session(saved_session, save_buf, TLS_MAX_SESSION_BUF, &real_session_len);
            platform_info("mbedtls_ssl_get_session_session return 0x%04x real_len=%d\r\n", ret, real_session_len);
            if (ret == 0) {
                char device_key[PRODUCT_KEY_MAXLEN + DEVICE_NAME_MAXLEN + 6] = {0};
                char _product_key[PRODUCT_KEY_LEN + 1] = {0};
                char _device_name[DEVICE_NAME_LEN + 1] = {0};

                HAL_GetProductKey(_product_key);
                HAL_GetDeviceName(_device_name);
                HAL_Snprintf(device_key, PRODUCT_KEY_MAXLEN + DEVICE_NAME_MAXLEN +5, KV_SESSION_KEY, _product_key, _device_name);
                HAL_Kv_Set(device_key, (void *)save_buf, real_session_len, 1);
            }
            aos_free(save_buf);
        } while (0);
    }
#endif
    /*
     * 5. Verify the server certificate
     */
    platform_info("  . Verifying peer X.509 certificate..");
    if (0 != (ret = _real_confirm(
                                mbedtls_ssl_get_verify_result(&(pTlsData->ssl))))) {
        platform_err(" failed  ! verify result not confirmed.");
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
                platform_err("ssl recv error: code = %d", ret);
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
                platform_err("ssl recv error: code = %d", ret);
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
            platform_err("ssl write timeout");
            return 0;
        } else {
            // mbedtls_strerror(ret, err_str, sizeof(err_str));
            platform_err("ssl write fail, code=%d", ret);
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
        platform_info("need release client crt&key");
#if defined(MBEDTLS_CERTS_C)
        mbedtls_x509_crt_free(&(pTlsData->clicert));
        mbedtls_pk_free(&(pTlsData->pkey));
#endif
    }
#endif
    mbedtls_ssl_free(&(pTlsData->ssl));
    mbedtls_ssl_config_free(&(pTlsData->conf));
    platform_info("ssl_disconnect");
}

int32_t HAL_SSL_Read(uintptr_t handle, char *buf, int len, int timeout_ms)
{
    return _network_ssl_read((TLSDataParams_t *)handle, buf, len, timeout_ms);
    ;
}

int32_t HAL_SSL_Write(uintptr_t handle, const char *buf, int len,
                      int timeout_ms)
{
    return _network_ssl_write((TLSDataParams_t *)handle, buf, len, timeout_ms);
}

int32_t HAL_SSL_Destroy(uintptr_t handle)
{
    if ((uintptr_t)NULL == handle) {
        platform_err("handle is NULL");
        return 0;
    }

    _network_ssl_disconnect((TLSDataParams_t *)handle);
    HAL_Free((void *)handle);
    return 0;
}

int HAL_SSLHooks_set(ssl_hooks_t *hooks)
{
    if (hooks == NULL || hooks->malloc == NULL || hooks->free == NULL) {
        return -1;
    }

    g_ssl_hooks.malloc = hooks->malloc;
    g_ssl_hooks.free = hooks->free;

    return 0;
}

uintptr_t HAL_SSL_Establish(const char *host, uint16_t port, const char *ca_crt,
                            size_t ca_crt_len)
{
    char             port_str[6];
    TLSDataParams_pt pTlsData;

    pTlsData = HAL_Malloc(sizeof(TLSDataParams_t));
    if (NULL == pTlsData) {
        return (uintptr_t)NULL;
    }
    memset(pTlsData, 0x0, sizeof(TLSDataParams_t));

    sprintf(port_str, "%u", port);

    if (0 != _TLSConnectNetwork(pTlsData, host, port_str, ca_crt, ca_crt_len,
                                NULL, 0, NULL, 0, NULL, 0)) {
        _network_ssl_disconnect(pTlsData);
        HAL_Free((void *)pTlsData);
        return (uintptr_t)NULL;
    }

    return (uintptr_t)pTlsData;
}
