/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#include "aos/errno.h"
#include "aos/kernel.h"
#include "aos/kv.h"
#include "ulog/ulog.h"
#include <network/network.h>

#ifdef FEATURE_UND_SUPPORT
    #include "und/und.h"
#endif

#include "mbedtls/error.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/pk.h"
#include "mbedtls/debug.h"
#include "mbedtls/platform.h"
#include "linkkit/wrappers/wrappers_defs.h"
#include "linkkit/wrappers/wrappers_os.h"
#ifdef  HAL_TLS_DNS_ENHANCE
    #include "dns.h"
#endif
#define LOG_TAG "HAL_TLS"



#define platform_info(format, ...) LOGI(LOG_TAG, format, ##__VA_ARGS__)
#define platform_err(format, ...) LOGE(LOG_TAG, format, ##__VA_ARGS__)

#define SEND_TIMEOUT_SECONDS (10)

#define DEBUG_LEVEL 10

typedef struct _TLSDataParams {
    mbedtls_ssl_context ssl;     /**< mbed TLS control context. */
    mbedtls_net_context fd;      /**< mbed TLS network context. */
    mbedtls_ssl_config  conf;    /**< mbed TLS configuration context. */
    mbedtls_x509_crt    cacertl; /**< mbed TLS CA certification. */
    mbedtls_x509_crt    clicert; /**< mbed TLS Client certification. */
    mbedtls_pk_context  pkey;    /**< mbed TLS Client key. */
} TLSDataParams_t, *TLSDataParams_pt;

#ifdef HAL_TLS_DNS_ENHANCE
static char *g_fixed_list[3] = {
    "106.15.83.29",
    "106.15.100.2",
    "139.196.135.135"
};
#endif

#if defined(TLS_SAVE_TICKET)

#define KEY_MAX_LEN          64
#define TLS_MAX_SESSION_BUF  384
#define KV_SESSION_KEY_FMT   "TLS_%s"

extern int HAL_Kv_Set(const char *key, const void *val, int len, int sync);

extern int HAL_Kv_Get(const char *key, void *val, int *buffer_len);

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
    if (session->ticket_len > 0) {
        if (session->ticket_len > (size_t)(end - p)) {
            return (MBEDTLS_ERR_SSL_BAD_INPUT_DATA);
        }
        session->ticket = HAL_Malloc(session->ticket_len);
        if (session->ticket == NULL) {
            return (MBEDTLS_ERR_SSL_ALLOC_FAILED);
        }
        memcpy(session->ticket, p, session->ticket_len);
        p += session->ticket_len;
        printf("saved ticket len = %d \r\n", (int)session->ticket_len);
    }
#endif

    if (p != end) {
        return (MBEDTLS_ERR_SSL_BAD_INPUT_DATA);
    }

    return (0);
}
#endif




#ifdef HAL_TLS_DNS_ENHANCE
static void utils_str2uint(char *input, uint8_t input_len, uint32_t *output)
{
    uint8_t index = 0;
    uint32_t temp = 0;

    for (index = 0; index < input_len; index++) {
        if (input[index] < '0' || input[index] > '9') {
            return;
        }
        temp = temp * 10 + input[index] - '0';
    }
    *output = temp;
}

static int mbedtls_net_connect_timeout_backup(mbedtls_net_context *ctx, const char *host,
        const char *port, int proto, unsigned int timeout)
{
    int ret;
    struct sockaddr_in address;
    size_t addr_len;
    struct timeval sendtimeout;
    uint8_t dns_retry = 0, dns_count = 0;
    uint32_t uint_port = 0;
    char *ip[DNS_RESULT_COUNT] = {0};

    memset(&address, 0, sizeof(struct sockaddr_in));
    utils_str2uint((char *)port, strlen(port), &uint_port);

    while (dns_retry++ < 8) {
        ret = dns_getaddrinfo((char *)host, ip);
        if (ret != 0) {
            platform_info("getaddrinfo error[%d], res: %d, host: %s, port: %s\n", dns_retry, ret, host, port);
            HAL_SleepMs(1000);
            continue;
        } else {
            break;
        }
    }
    if (ret != 0) {
        //try fixed ip:
        if (strstr(host, "iot-as-mqtt.cn-shanghai.aliyuncs.com") != NULL) {
            int i;
            for (i = 0; i < 3; i++) {
                ip[i] = g_fixed_list[i];
            }
            platform_info("using fixed ip\n");
        } else {
            return (MBEDTLS_ERR_NET_UNKNOWN_HOST);
        }
    }

    /* Try the sockaddrs until a connection succeeds */
    ret = MBEDTLS_ERR_NET_UNKNOWN_HOST;
    for (dns_count = 0; dns_count < DNS_RESULT_COUNT; dns_count++) {
        if (ip[dns_count] == NULL || strlen(ip[dns_count]) == 0) {
            continue;
        }
        platform_info("ip address: %s\n", ip[dns_count]);

        ctx->fd = (int) socket(AF_INET, SOCK_STREAM, 0);
        if (ctx->fd < 0) {
            ret = MBEDTLS_ERR_NET_SOCKET_FAILED;
            continue;
        }

        sendtimeout.tv_sec = timeout;
        sendtimeout.tv_usec = 0;

        if (0 != setsockopt(ctx->fd, SOL_SOCKET, SO_SNDTIMEO, &sendtimeout, sizeof(sendtimeout))) {
            platform_err("setsockopt error");
        }
        platform_info("setsockopt SO_SNDTIMEO timeout: %ds", sendtimeout.tv_sec);

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr(ip[dns_count]);
        address.sin_port = htons(uint_port);

        addr_len = sizeof(address);

        if (connect(ctx->fd, (struct sockaddr *)&address, addr_len) == 0) {
            ret = 0;
            break;
        }

        close(ctx->fd);
        ret = MBEDTLS_ERR_NET_CONNECT_FAILED;
    }

    return (ret);
}
#endif

static unsigned int _avRandom()
{
    return (((unsigned int)aos_rand() << 16) + aos_rand());
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
    aos_srand((uint32_t)aos_now_ms());

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
    do {
        ret = mbedtls_net_connect(&(pTlsData->fd), addr, port, MBEDTLS_NET_PROTO_TCP);
        if (ret == 0) {
            break;
        }

        platform_err(" connect failed ! net_connect returned -0x%04x", -ret);
#if defined(HAL_TLS_DNS_ENHANCE)
        ret = mbedtls_net_connect_timeout_backup(&(pTlsData->fd), addr, port, MBEDTLS_NET_PROTO_TCP,
                SEND_TIMEOUT_SECONDS);
        if (ret == 0) {
            break;
        }
        platform_err(" backup failed ! net_connect returned -0x%04x", -ret);
#endif

#ifdef FEATURE_UND_SUPPORT
        if (ret == MBEDTLS_ERR_NET_UNKNOWN_HOST) {
            und_update_statis(UND_STATIS_NETWORK_FAIL_IDX, UND_STATIS_NETWORK_DNS_FIAL_REASON);
        } else {
            und_update_statis(UND_STATIS_NETWORK_FAIL_IDX, UND_STATIS_NETWORK_TCP_FAIL_REASON);
        }
#endif
        return ret;
    } while (0);


    platform_info(" ok");

    /*
     * 2. Setup stuff
     */
    platform_info("  . Setting up the SSL/TLS structure...");
    if ((ret = mbedtls_ssl_config_defaults(
                           &(pTlsData->conf), MBEDTLS_SSL_IS_CLIENT,
                           MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        platform_err(" failed! mbedtls_ssl_config_defaults returned %d", ret);
#ifdef FEATURE_UND_SUPPORT
        und_update_statis(UND_STATIS_NETWORK_FAIL_IDX, UND_STATIS_NETWORK_TLS_FAIL_REASON);
#endif
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
#ifdef FEATURE_UND_SUPPORT
        und_update_statis(UND_STATIS_NETWORK_FAIL_IDX, UND_STATIS_NETWORK_TLS_FAIL_REASON);
#endif
        return ret;
    }
#endif
    mbedtls_ssl_conf_rng(&(pTlsData->conf), _ssl_random, NULL);
    mbedtls_ssl_conf_dbg(&(pTlsData->conf), _ssl_debug, NULL);
    /* mbedtls_ssl_conf_dbg( &(pTlsData->conf), _ssl_debug, stdout ); */

    if ((ret = mbedtls_ssl_setup(&(pTlsData->ssl), &(pTlsData->conf))) != 0) {
        platform_err("failed! mbedtls_ssl_setup returned %d", ret);
#ifdef FEATURE_UND_SUPPORT
        und_update_statis(UND_STATIS_NETWORK_FAIL_IDX, UND_STATIS_NETWORK_TLS_FAIL_REASON);
#endif
        return ret;
    }
#if defined(ON_PRE) || defined(ON_DAILY)
    platform_err("SKIPPING mbedtls_ssl_set_hostname() when ON_PRE or ON_DAILY defined!");
#else
    mbedtls_ssl_set_hostname(&(pTlsData->ssl), addr);
#endif
    mbedtls_ssl_set_bio(&(pTlsData->ssl), &(pTlsData->fd), mbedtls_net_send,
                        mbedtls_net_recv, mbedtls_net_recv_timeout);

#if defined(TLS_SAVE_TICKET)
    if (NULL == saved_session) {
        do {
            int len = TLS_MAX_SESSION_BUF;
            unsigned char *save_buf = HAL_Malloc(TLS_MAX_SESSION_BUF);
            char key_buf[KEY_MAX_LEN] = {0};
            if (save_buf ==  NULL) {
                printf(" malloc failed\r\n");
                break;
            }

            saved_session = HAL_Malloc(sizeof(mbedtls_ssl_session));

            if (saved_session == NULL) {
                printf(" malloc failed\r\n");
                HAL_Free(save_buf);
                save_buf =  NULL;
                break;
            }


            memset(save_buf, 0x00, TLS_MAX_SESSION_BUF);
            memset(saved_session, 0x00, sizeof(mbedtls_ssl_session));
            HAL_Snprintf(key_buf, KEY_MAX_LEN - 1, KV_SESSION_KEY_FMT, addr);
            ret = HAL_Kv_Get(key_buf, save_buf, &len);

            if (ret != 0 || len == 0) {
                printf(" kv get failed len=%d,ret = %d\r\n", len, ret);
                HAL_Free(saved_session);
                HAL_Free(save_buf);
                save_buf = NULL;
                saved_session = NULL;
                break;
            }
            ret = ssl_deserialize_session(saved_session, save_buf, len);
            if (ret < 0) {
                printf("ssl_deserialize_session err,ret = %d\r\n", ret);
                HAL_Free(saved_session);
                HAL_Free(save_buf);
                save_buf = NULL;
                saved_session = NULL;
                break;
            }
            HAL_Free(save_buf);
        } while (0);
    }

    if (NULL != saved_session) {
        mbedtls_ssl_set_session(&(pTlsData->ssl), saved_session);
        printf("use saved session!!\r\n");
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
#ifdef FEATURE_UND_SUPPORT
            und_update_statis(UND_STATIS_NETWORK_FAIL_IDX, UND_STATIS_NETWORK_TLS_FAIL_REASON);
#endif
#if defined(TLS_SAVE_TICKET)
            if (saved_session != NULL) {
                mbedtls_ssl_session_free(saved_session);
                HAL_Free(saved_session);
                saved_session = NULL;
            }
#endif
            return ret;
        }
    }
    platform_info(" ok");

#if defined(TLS_SAVE_TICKET)
    do {
        size_t real_session_len = 0;
        mbedtls_ssl_session *new_session = NULL;

        new_session = HAL_Malloc(sizeof(mbedtls_ssl_session));
        if (NULL == new_session) {
            break;
        }

        memset(new_session, 0x00, sizeof(mbedtls_ssl_session));

        ret = mbedtls_ssl_get_session(&(pTlsData->ssl), new_session);
        if (ret != 0) {
            HAL_Free(new_session);
            break;
        }
        if (saved_session == NULL) {
            ret = 1;
        } else if (new_session->ticket_len != saved_session->ticket_len) {
            ret = 1;
        } else {
            ret = memcmp(new_session->ticket, saved_session->ticket, new_session->ticket_len);
        }
        if (ret != 0) {
            unsigned char *save_buf = HAL_Malloc(TLS_MAX_SESSION_BUF);
            if (save_buf ==  NULL) {
                mbedtls_ssl_session_free(new_session);
                HAL_Free(new_session);
                new_session = NULL;
                break;
            }
            memset(save_buf, 0x00, sizeof(TLS_MAX_SESSION_BUF));
            ret = ssl_serialize_session(new_session, save_buf, TLS_MAX_SESSION_BUF, &real_session_len);
            printf("mbedtls_ssl_get_session_session return 0x%04x real_len=%d\r\n", ret, (int)real_session_len);
            if (ret == 0) {
                char key_buf[KEY_MAX_LEN] = {0};
                HAL_Snprintf(key_buf, KEY_MAX_LEN - 1, KV_SESSION_KEY_FMT, addr);
                ret = HAL_Kv_Set(key_buf, (void *)save_buf, real_session_len, 1);
                if (ret < 0) {
                    printf("save ticket to kv failed ret =%d ,len = %d\r\n", ret, (int)real_session_len);
                }
            }
            HAL_Free(save_buf);
        }
        mbedtls_ssl_session_free(new_session);
        HAL_Free(new_session);
    } while (0);
    if (saved_session != NULL) {
        mbedtls_ssl_session_free(saved_session);
        HAL_Free(saved_session);
        saved_session = NULL;
    }
#endif
    /*
     * 5. Verify the server certificate
     */
    platform_info("  . Verifying peer X.509 certificate..");
    if (0 != (ret = _real_confirm(
                                mbedtls_ssl_get_verify_result(&(pTlsData->ssl))))) {
        platform_err(" failed  ! verify result not confirmed.");
#ifdef FEATURE_UND_SUPPORT
        und_update_statis(UND_STATIS_NETWORK_FAIL_IDX, UND_STATIS_NETWORK_TLS_FAIL_REASON);
#endif
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

    uint32_t cur_time = HAL_UptimeMs();
    uint32_t expire_time = cur_time + timeout_ms;
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
            if (net_status == -2) {
#ifdef FEATURE_UND_SUPPORT
                und_update_statis(UND_STATIS_NETWORK_EXCEPTION_IDX,
                                  UND_STATIS_NETWORK_RD_EXCEPTION_REASON);
#endif
            }
            return (net_status == -2) ? net_status : readLen;
        } else {
            if (MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY == ret) {
                // mbedtls_strerror(ret, err_str, sizeof(err_str));
                platform_err("ssl recv error: code = %d", ret);
                net_status = -2; /* connection is closed */
                break;
#ifdef CSP_LINUXHOST
            } else if (MBEDTLS_ERR_SSL_WANT_READ == ret && errno == EINTR) {
                cur_time = HAL_UptimeMs();
                if ((cur_time - expire_time) < (UINT32_MAX / 2)) {
                    return readLen;
                }
                HAL_SleepMs(10);
                continue;

#endif
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
                platform_err("ssl recv error: code = %d", ret);
#ifdef FEATURE_UND_SUPPORT
                und_update_statis(UND_STATIS_NETWORK_EXCEPTION_IDX,
                                  UND_STATIS_NETWORK_RD_EXCEPTION_REASON);
#endif
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
#ifdef FEATURE_UND_SUPPORT
            und_update_statis(UND_STATIS_NETWORK_EXCEPTION_IDX,
                              UND_STATIS_NETWORK_WR_EXCEPTION_REASON);
#endif
            platform_err("ssl write timeout");
            return 0;
        } else {
            // mbedtls_strerror(ret, err_str, sizeof(err_str));
#ifdef FEATURE_UND_SUPPORT
            und_update_statis(UND_STATIS_NETWORK_EXCEPTION_IDX,
                              UND_STATIS_NETWORK_WR_EXCEPTION_REASON);
#endif
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

#ifdef FEATURE_UND_SUPPORT
    und_update_statis(UND_STATIS_NETWORK_EXCEPTION_IDX, UND_STATIS_NETWORK_PING_EXCEPTION_REASON);
#endif
    _network_ssl_disconnect((TLSDataParams_t *)handle);
    aos_free((void *)handle);
    return 0;
}

uintptr_t HAL_SSL_Establish(const char *host, uint16_t port, const char *ca_crt,
                            size_t ca_crt_len)
{
    char             port_str[6];
    TLSDataParams_pt pTlsData;

    pTlsData = aos_malloc(sizeof(TLSDataParams_t));
    if (NULL == pTlsData) {
        return (uintptr_t)NULL;
    }
    memset(pTlsData, 0x0, sizeof(TLSDataParams_t));

    sprintf(port_str, "%u", port);

    if (0 != _TLSConnectNetwork(pTlsData, host, port_str, ca_crt, ca_crt_len,
                                NULL, 0, NULL, 0, NULL, 0)) {
        _network_ssl_disconnect(pTlsData);
        aos_free((void *)pTlsData);
        return (uintptr_t)NULL;
    }

    return (uintptr_t)pTlsData;
}
