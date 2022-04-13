/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#if defined(_PLATFORM_IS_LINUX_)
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <sys/types.h>
    #include <netdb.h>
    #include <signal.h>
    #include <unistd.h>
    #include <sys/time.h>
    #include <arpa/nameser.h>
    #include <resolv.h>
#endif
#include "infra_config.h"
#include "infra_compat.h"
#include "mbedtls/error.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/pk.h"
#include "mbedtls/debug.h"
#include "mbedtls/platform.h"
#include "wrappers.h"
#include "infra_sha256.h"
#include "infra_string.h"
#include <errno.h>

#include "fibocom.h"
#if defined(CONFIG_FIBOCOM_ALIOS)
#include "amp_system.h"

#define printf  HAL_Printf
#endif

#define SEND_TIMEOUT_SECONDS                (10)

#ifndef CONFIG_MBEDTLS_DEBUG_LEVEL
    #define CONFIG_MBEDTLS_DEBUG_LEVEL 0
#endif

typedef struct _TLSDataParams {
    mbedtls_ssl_context ssl;          /**< mbed TLS control context. */
    mbedtls_net_context fd;           /**< mbed TLS network context. */
    mbedtls_ssl_config conf;          /**< mbed TLS configuration context. */
    mbedtls_x509_crt cacertl;         /**< mbed TLS CA certification. */
    mbedtls_x509_crt clicert;         /**< mbed TLS Client certification. */
    mbedtls_pk_context pkey;          /**< mbed TLS Client key. */
} TLSDataParams_t, *TLSDataParams_pt;

static unsigned int mbedtls_mem_used = 0;
static unsigned int mbedtls_max_mem_used = 0;
static ssl_hooks_t g_ssl_hooks = {HAL_Malloc, HAL_Free};

#define MBEDTLS_MEM_INFO_MAGIC   0x12345678

typedef struct {
    int magic;
    int size;
} mbedtls_mem_info_t;

#define TLS_AUTH_MODE_CA        0
#define TLS_AUTH_MODE_PSK       1

#ifndef TLS_AUTH_MODE
#define TLS_AUTH_MODE           TLS_AUTH_MODE_CA
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
#if (TLS_AUTH_MODE == TLS_AUTH_MODE_CA)
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

static void _srand(unsigned int seed)
{
#ifdef HAL_KV
#define SEED_MAGIC 0x123
    int           ret        = 0;
    int           seed_len   = 0;
    unsigned int  seed_val   = 0;
    static char  *g_seed_key = "seed_key";

    seed_len = sizeof(seed_val);
    ret = HAL_Kv_Get(g_seed_key, &seed_val, &seed_len);
    if (ret) {
        seed_val = SEED_MAGIC;
    }
    seed_val += seed;
    HAL_Srandom(seed_val);
    seed_val = rand();
    HAL_Kv_Set(g_seed_key, &seed_val, sizeof(seed_val), 1);
#else
    HAL_Srandom(seed);
#endif
}


static int _ssl_random(void *p_rng, unsigned char *output, size_t output_len)
{
    uint32_t rnglen = output_len;
    uint8_t   rngoffset = 0;

    while (rnglen > 0) {
        *(output + rngoffset) = (unsigned char)HAL_Random(0);
        rngoffset++;
        rnglen--;
    }
    return 0;
}

static void _ssl_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    ((void) level);
    if (NULL != ctx) {
#if 0
        fprintf((FILE *) ctx, "%s:%04d: %s", file, line, str);
        fflush((FILE *) ctx);
#endif
        printf("%s\n", str);
    }
}

static int _real_confirm(int verify_result)
{
    printf("certificate verification result: 0x%02x\n", verify_result);

#if defined(FORCE_SSL_VERIFY)
    if ((verify_result & MBEDTLS_X509_BADCERT_EXPIRED) != 0) {
        printf("! fail ! ERROR_CERTIFICATE_EXPIRED\n");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_REVOKED) != 0) {
        printf("! fail ! server certificate has been revoked\n");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_CN_MISMATCH) != 0) {
        printf("! fail ! CN mismatch\n");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_NOT_TRUSTED) != 0) {
        printf("! fail ! self-signed or not signed by a trusted CA\n");
        return -1;
    }
#endif

    return 0;
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
    mbedtls_debug_set_threshold((int)CONFIG_MBEDTLS_DEBUG_LEVEL);
#endif
    mbedtls_net_init(tcp_fd);
    mbedtls_ssl_init(ssl);
    mbedtls_ssl_config_init(conf);

    _srand(HAL_UptimeMs());
    /* Setup Client Cert/Key */
#if (TLS_AUTH_MODE == TLS_AUTH_MODE_CA)
    mbedtls_x509_crt_init(crt509_ca);

    /*verify_source->trusted_ca_crt==NULL
     * 0. Initialize certificates
     */

    printf("Loading the CA root certificate ...\n");
    if (NULL != ca_crt) {
        if (0 != (ret = mbedtls_x509_crt_parse(crt509_ca, (const unsigned char *)ca_crt, ca_len))) {
            printf(" failed ! x509parse_crt returned -0x%04x\n", -ret);
            return ret;
        }
    }
    printf(" ok (%d skipped)\n", ret);

#if defined(MBEDTLS_CERTS_C)
    mbedtls_x509_crt_init(crt509_cli);
    mbedtls_pk_init(pk_cli);
#endif
    if (cli_crt != NULL && cli_key != NULL) {
#if defined(MBEDTLS_CERTS_C)
        printf("start prepare client cert .\n");
        ret = mbedtls_x509_crt_parse(crt509_cli, (const unsigned char *) cli_crt, cli_len);
#else
        {
            ret = 1;
            printf("MBEDTLS_CERTS_C not defined.\n");
        }
#endif
        if (ret != 0) {
            printf(" failed!  mbedtls_x509_crt_parse returned -0x%x\n", -ret);
            return ret;
        }

#if defined(MBEDTLS_CERTS_C)
        printf("start mbedtls_pk_parse_key[%s]\n", cli_pwd);
        ret = mbedtls_pk_parse_key(pk_cli, (const unsigned char *) cli_key, key_len, (const unsigned char *) cli_pwd, pwd_len);
#else
        {
            ret = 1;
            printf("MBEDTLS_CERTS_C not defined.\n");
        }
#endif

        if (ret != 0) {
            printf(" failed\n  !  mbedtls_pk_parse_key returned -0x%x\n", -ret);
            return ret;
        }
    }
#endif /* if (TLS_AUTH_MODE == TLS_AUTH_MODE_CA) */

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
    uint8_t dns_retry = 0;

    if ((ret = net_prepare()) != 0) {
        return (ret);
    }

    /* Do name resolution with both IPv6 and IPv4 */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = proto == MBEDTLS_NET_PROTO_UDP ? SOCK_DGRAM : SOCK_STREAM;
    hints.ai_protocol = proto == MBEDTLS_NET_PROTO_UDP ? IPPROTO_UDP : IPPROTO_TCP;

    while (dns_retry++ < 8) {
        ret = getaddrinfo(host, port, &hints, &addr_list);
        if (ret != 0) {
#if defined(_PLATFORM_IS_LINUX_)
            if (ret == EAI_AGAIN) {
                int rc = res_init();
                printf("getaddrinfo res_init, rc is %d, errno is %d\n", rc, errno);
            }
#endif

            printf("getaddrinfo error[%d], res: %s, host: %s, port: %s\n", dns_retry, gai_strerror(ret), host, port);
            sleep(1);
            continue;
        } else {
            break;
        }
    }

    if (ret != 0) {
        return (MBEDTLS_ERR_NET_UNKNOWN_HOST);
    }


    /* Try the sockaddrs until a connection succeeds */
    ret = MBEDTLS_ERR_NET_UNKNOWN_HOST;
    for (cur = addr_list; cur != NULL; cur = cur->ai_next) {
        char ip4_str[INET_ADDRSTRLEN];

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
            printf("setsockopt error\n");
        }
        printf("setsockopt SO_SNDTIMEO timeout: %ds\n", (int)sendtimeout.tv_sec);

        inet_ntop(AF_INET, &((const struct sockaddr_in *)cur->ai_addr)->sin_addr, ip4_str, INET_ADDRSTRLEN);
        printf("connecting IP_ADDRESS: %s\n", ip4_str);

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

void *_SSLCalloc_wrapper(size_t n, size_t size)
{
    unsigned char *buf = NULL;
    mbedtls_mem_info_t *mem_info = NULL;

    if (n == 0 || size == 0) {
        return NULL;
    }

    buf = (unsigned char *)(g_ssl_hooks.malloc(n * size + sizeof(mbedtls_mem_info_t)));
    if (NULL == buf) {
        return NULL;
    } else {
        memset(buf, 0, n * size + sizeof(mbedtls_mem_info_t));
    }

    mem_info = (mbedtls_mem_info_t *)buf;
    mem_info->magic = MBEDTLS_MEM_INFO_MAGIC;
    mem_info->size = n * size;
    buf += sizeof(mbedtls_mem_info_t);

    mbedtls_mem_used += mem_info->size;
    if (mbedtls_mem_used > mbedtls_max_mem_used) {
        mbedtls_max_mem_used = mbedtls_mem_used;
    }

    /* printf("INFO -- mbedtls malloc: %p %d  total used: %d  max used: %d\r\n",
                       buf, (int)size, mbedtls_mem_used, mbedtls_max_mem_used); */

    return buf;
}

void _SSLFree_wrapper(void *ptr)
{
    mbedtls_mem_info_t *mem_info = NULL;
    if (NULL == ptr) {
        return;
    }

    mem_info = (mbedtls_mem_info_t *)((unsigned char *)ptr - sizeof(mbedtls_mem_info_t));
    if (mem_info->magic != MBEDTLS_MEM_INFO_MAGIC) {
        printf("Warning - invalid mem info magic: 0x%x\r\n", mem_info->magic);
        return;
    }

    mbedtls_mem_used -= mem_info->size;
    /* printf("INFO mbedtls free: %p %d  total used: %d  max used: %d\r\n",
                       ptr, mem_info->size, mbedtls_mem_used, mbedtls_max_mem_used);*/

    g_ssl_hooks.free(mem_info);
}

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
#if defined(_PLATFORM_IS_LINUX_)
    struct in_addr in;
#endif /* #if defined(_PLATFORM_IS_LINUX_) */
    int ret = -1;

    /*
     * 0. Init
     */
    if (0 != (ret = _ssl_client_init(&(pTlsData->ssl), &(pTlsData->fd), &(pTlsData->conf),
                                     &(pTlsData->cacertl), ca_crt, ca_crt_len,
                                     &(pTlsData->clicert), client_crt, client_crt_len,
                                     &(pTlsData->pkey), client_key, client_key_len, client_pwd, client_pwd_len))) {
        printf(" failed ! ssl_client_init returned -0x%04x\n", -ret);
        return ret;
    }

    /*
     * 1. Start the connection
     */
    printf("Connecting to /%s/%s...\n", addr, port);
#if defined(_PLATFORM_IS_LINUX_)
    if (0 != (ret = mbedtls_net_connect_timeout(&(pTlsData->fd), addr, port, MBEDTLS_NET_PROTO_TCP,
                    SEND_TIMEOUT_SECONDS))) {
        printf(" failed ! net_connect returned -0x%04x\n", -ret);
        return ret;
    }
#else
    if (0 != (ret = mbedtls_net_connect(&(pTlsData->fd), addr, port, MBEDTLS_NET_PROTO_TCP))) {
        printf(" failed ! net_connect returned -0x%04x\n", -ret);
        return ret;
    }
#endif
    printf(" ok\n");

    /*
     * 2. Setup stuff
     */
    printf("  . Setting up the SSL/TLS structure...\n");
    if ((ret = mbedtls_ssl_config_defaults(&(pTlsData->conf), MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        printf(" failed! mbedtls_ssl_config_defaults returned %d\n", ret);
        return ret;
    }

    mbedtls_ssl_conf_max_version(&pTlsData->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
    mbedtls_ssl_conf_min_version(&pTlsData->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);

    printf(" ok\n");

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

#if (TLS_AUTH_MODE == TLS_AUTH_MODE_CA)
    mbedtls_ssl_conf_ca_chain(&(pTlsData->conf), &(pTlsData->cacertl), NULL);

    if ((ret = mbedtls_ssl_conf_own_cert(&(pTlsData->conf), &(pTlsData->clicert), &(pTlsData->pkey))) != 0) {
        printf(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n", ret);
        return ret;
    }
#elif (TLS_AUTH_MODE == TLS_AUTH_MODE_PSK)
    {
        static const int ciphersuites[1] = {MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA};
        char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
        char device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
        char device_secret[IOTX_DEVICE_SECRET_LEN + 1] = {0};
        char *auth_type = "devicename";
        char *sign_method = "hmacsha256";
        char *timestamp = "2524608000000";
        char *psk_identity = NULL, string_to_sign[IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 33] = {0};
        uint32_t psk_identity_len = 0;
        uint8_t sign_hex[32] = {0};
        char sign_string[65] = {0};

        IOT_Ioctl(IOTX_IOCTL_GET_PRODUCT_KEY, product_key);
        IOT_Ioctl(IOTX_IOCTL_GET_DEVICE_NAME, device_name);
        IOT_Ioctl(IOTX_IOCTL_GET_DEVICE_SECRET, device_secret);

        /* psk identity length */
        psk_identity_len = strlen(auth_type) + strlen(sign_method) + strlen(product_key) + strlen(device_name) + strlen(
                                       timestamp) + 5;
        psk_identity = HAL_Malloc(psk_identity_len);
        if (psk_identity == NULL) {
            printf("psk_identity malloc failed\n");
            return -1;
        }
        memset(psk_identity, 0, psk_identity_len);
        memcpy(psk_identity, auth_type, strlen(auth_type));
        memcpy(psk_identity + strlen(psk_identity), "|", strlen("|"));
        memcpy(psk_identity + strlen(psk_identity), sign_method, strlen(sign_method));
        memcpy(psk_identity + strlen(psk_identity), "|", strlen("|"));
        memcpy(psk_identity + strlen(psk_identity), product_key, strlen(product_key));
        memcpy(psk_identity + strlen(psk_identity), "&", strlen("&"));
        memcpy(psk_identity + strlen(psk_identity), device_name, strlen(device_name));
        memcpy(psk_identity + strlen(psk_identity), "|", strlen("|"));
        memcpy(psk_identity + strlen(psk_identity), timestamp, strlen(timestamp));

        /* string to sign */
        memcpy(string_to_sign, "id", strlen("id"));
        memcpy(string_to_sign + strlen(string_to_sign), product_key, strlen(product_key));
        memcpy(string_to_sign + strlen(string_to_sign), "&", strlen("&"));
        memcpy(string_to_sign + strlen(string_to_sign), device_name, strlen(device_name));
        memcpy(string_to_sign + strlen(string_to_sign), "timestamp", strlen("timestamp"));
        memcpy(string_to_sign + strlen(string_to_sign), timestamp, strlen(timestamp));

        utils_hmac_sha256((uint8_t *)string_to_sign, strlen(string_to_sign), (uint8_t *)device_secret,
                          strlen(device_secret), sign_hex);
        infra_hex2str(sign_hex, 32, sign_string);

        /* printf("psk_identity: %s\n",psk_identity);
        printf("psk         : %s\n",sign_string); */

        mbedtls_ssl_conf_psk(&(pTlsData->conf), (const unsigned char *)sign_string, strlen(sign_string),
                             (const unsigned char *)psk_identity, strlen(psk_identity));
        mbedtls_ssl_conf_ciphersuites(&(pTlsData->conf), ciphersuites);

        HAL_Free(psk_identity);

        printf("mbedtls psk config finished\n");
    }
#endif /* #elif (TLS_AUTH_MODE == TLS_AUTH_MODE_PSK) */

    mbedtls_ssl_conf_rng(&(pTlsData->conf), _ssl_random, NULL);
    mbedtls_ssl_conf_dbg(&(pTlsData->conf), _ssl_debug, NULL);
    mbedtls_ssl_conf_dbg(&(pTlsData->conf), _ssl_debug, stdout);

    if ((ret = mbedtls_ssl_setup(&(pTlsData->ssl), &(pTlsData->conf))) != 0) {
        printf("failed! mbedtls_ssl_setup returned %d\n", ret);
        return ret;
    }

#if (TLS_AUTH_MODE == TLS_AUTH_MODE_CA)
#if defined(_PLATFORM_IS_LINUX_)
    /* only set hostname when addr isn't ip string and hostname isn't preauth_shanghai */
    if (inet_aton(addr, &in) == 0 && strcmp("iot-auth-pre.cn-shanghai.aliyuncs.com", addr)) {
        mbedtls_ssl_set_hostname(&(pTlsData->ssl), addr);
    }
#else
    if (strcmp("iot-auth-pre.cn-shanghai.aliyuncs.com", addr)) {
        mbedtls_ssl_set_hostname(&(pTlsData->ssl), addr);
    }
#endif /* #if defined(_PLATFORM_IS_LINUX_) */
#endif /* #if (TLS_AUTH_MODE == TLS_AUTH_MODE_CA) */
    mbedtls_ssl_set_bio(&(pTlsData->ssl), &(pTlsData->fd), mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

#if defined(TLS_SAVE_TICKET)
    if (NULL == saved_session) {
        do {
            int len = TLS_MAX_SESSION_BUF;
            char key_buf[KEY_MAX_LEN] = {0};
            unsigned char *save_buf = HAL_Malloc(TLS_MAX_SESSION_BUF);
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

            HAL_Snprintf(key_buf,KEY_MAX_LEN -1, KV_SESSION_KEY_FMT, addr);
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
    printf("Performing the SSL/TLS handshake...\n");

    while ((ret = mbedtls_ssl_handshake(&(pTlsData->ssl))) != 0) {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            printf("failed  ! mbedtls_ssl_handshake returned -0x%04x\n", -ret);
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
    printf(" ok\n");

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
                HAL_Snprintf(key_buf,KEY_MAX_LEN -1, KV_SESSION_KEY_FMT, addr);
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
    printf("  . Verifying peer X.509 certificate..\n");
    if (0 != (ret = _real_confirm(mbedtls_ssl_get_verify_result(&(pTlsData->ssl))))) {
        printf(" failed  ! verify result not confirmed.\n");
        return ret;
    }

    /* n->my_socket = (int)((n->tlsdataparams.fd).fd); */
    /* WRITE_IOT_DEBUG_LOG("my_socket=%d", n->my_socket); */

    return 0;

}

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
                mbedtls_strerror(ret, err_str, sizeof(err_str));
                printf("ssl recv error: code = -0x%04X, err_str = '%s'\n", -ret, err_str);
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
                mbedtls_strerror(ret, err_str, sizeof(err_str));
                printf("ssl recv error: code = -0x%04X, err_str = '%s'\n", -ret, err_str);
                net_status = -1;
                return -1; /* Connection error */
            }
        }
    }

    return (readLen > 0) ? readLen : net_status;
}

static int _network_ssl_write(TLSDataParams_t *pTlsData, const char *buffer, int len, int timeout_ms)
{
#if defined(_PLATFORM_IS_LINUX_)
    int32_t res = 0;
    int32_t write_bytes = 0;
    uint64_t timestart_ms = 0, timenow_ms = 0;
    struct timeval timestart, timenow, timeout;

    if (pTlsData == NULL) {
        return -1;
    }

    /* timeout */
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    /* Start Time */
    gettimeofday(&timestart, NULL);
    timestart_ms = timestart.tv_sec * 1000 + timestart.tv_usec / 1000;
    timenow_ms = timestart_ms;

    res = setsockopt(pTlsData->fd.fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    if (res < 0) {
        return -1;
    }

    do {
        gettimeofday(&timenow, NULL);
        timenow_ms = timenow.tv_sec * 1000 + timenow.tv_usec / 1000;

        if (timenow_ms - timestart_ms >= timenow_ms ||
            timeout_ms - (timenow_ms - timestart_ms) > timeout_ms) {
            break;
        }

        res = mbedtls_ssl_write(&(pTlsData->ssl), (unsigned char *)buffer + write_bytes, len - write_bytes);
        if (res < 0) {
            if (res != MBEDTLS_ERR_SSL_WANT_READ &&
                res != MBEDTLS_ERR_SSL_WANT_WRITE) {
                if (write_bytes == 0) {
                    return -1;
                }
                break;
            }
        } else if (res == 0) {
            break;
        } else {
            write_bytes += res;
        }
    } while (((timenow_ms - timestart_ms) < timeout_ms) && (write_bytes < len));

    return write_bytes;
#else
    uint32_t writtenLen = 0;
    int ret = -1;

    if (pTlsData == NULL) {
        return -1;
    }

    while (writtenLen < len) {
        ret = mbedtls_ssl_write(&(pTlsData->ssl), (unsigned char *)(buffer + writtenLen), (len - writtenLen));
        if (ret > 0) {
            writtenLen += ret;
            continue;
        } else if (ret == 0) {
            printf("ssl write timeout\n");
            return 0;
        } else {
            char err_str[33];
            mbedtls_strerror(ret, err_str, sizeof(err_str));
            printf("ssl write fail, code=%d, str=%s\n", ret, err_str);
            return -1; /* Connnection error */
        }
    }

    return writtenLen;
#endif
}

static void _network_ssl_disconnect(TLSDataParams_t *pTlsData)
{
    mbedtls_ssl_close_notify(&(pTlsData->ssl));
    mbedtls_net_free(&(pTlsData->fd));
#if (TLS_AUTH_MODE == TLS_AUTH_MODE_CA)
    mbedtls_x509_crt_free(&(pTlsData->cacertl));
    if ((pTlsData->pkey).pk_info != NULL) {
        printf("need release client crt&key\n");
#if defined(MBEDTLS_CERTS_C)
        mbedtls_x509_crt_free(&(pTlsData->clicert));
        mbedtls_pk_free(&(pTlsData->pkey));
#endif
    }
#endif /* #if (TLS_AUTH_MODE == TLS_AUTH_MODE_CA) */
    mbedtls_ssl_free(&(pTlsData->ssl));
    mbedtls_ssl_config_free(&(pTlsData->conf));
    printf("ssl_disconnect\n");
}

int ssl_hooks_set(ssl_hooks_t *hooks)
{
    if (hooks == NULL || hooks->malloc == NULL || hooks->free == NULL) {
        return -1;
    }

    g_ssl_hooks.malloc = hooks->malloc;
    g_ssl_hooks.free = hooks->free;

    return 0;
}

const char *g_cli_crt = \
{
    \
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDhzCCAm+gAwIBAgIHNHa/qHDMmTANBgkqhkiG9w0BAQsFADBTMSgwJgYDVQQD\r\n"
    "DB9BbGliYWJhIENsb3VkIElvVCBPcGVyYXRpb24gQ0ExMRowGAYDVQQKDBFBbGli\r\n"
    "YWJhIENsb3VkIElvVDELMAkGA1UEBhMCQ04wIBcNMTkxMDE3MDExODE3WhgPMjEx\r\n"
    "OTEwMTcwMTE4MTdaMFExJjAkBgNVBAMMHUFsaWJhYmEgQ2xvdWQgSW9UIENlcnRp\r\n"
    "ZmljYXRlMRowGAYDVQQKDBFBbGliYWJhIENsb3VkIElvVDELMAkGA1UEBhMCQ04w\r\n"
    "ggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCsNkQ57z1lpe/dSQqHmrfm\r\n"
    "2Hml0YPhac8yXdx1brUqzWg+robUsK8bxyr1yqlTL/aNNT6Kjeq8m8pZv/fEy42o\r\n"
    "WmQogrJ8nBYCdXGOYmfsdDrcvInkde8jiRLRduW+/aSeGqkWiKoInqvvSFRelGNv\r\n"
    "N9wZ7uRcL16jJGQFpvlowps/+Lkg6vNiJ0mjKLQRy3bjrwEazLBijfI5oyDQAcXj\r\n"
    "J4GEdA1vw/ZxQJkrI2ZXp2bfc3q1QciWb8mQDqNIG9DldfRddS2Vj4iDI3TNoFFf\r\n"
    "PF7eoSbycjaVfXnJY+cflapnwm/1LoCUNEi7ut25PzqP5JJbhV1XYg4A/OED1NRT\r\n"
    "AgMBAAGjYDBeMB8GA1UdIwQYMBaAFIo3m6hwzdX5SMiXfiGfWW9JjiQRMB0GA1Ud\r\n"
    "DgQWBBRtrOWad4BnTKm9l5n2scfjaUEoTjAOBgNVHQ8BAf8EBAMCA/gwDAYDVR0T\r\n"
    "AQH/BAIwADANBgkqhkiG9w0BAQsFAAOCAQEAiug/d0unpbHHtsg6RKrmIK/VLvAd\r\n"
    "F8Mjkohv8XBxu+8xqIb5JXsUEtdHWDHBXubmQvvh4OIVxByx967fUFT8u/mZFRwl\r\n"
    "banwK9wQgua3Y5M84k3jnWo3PfKERCOZ0DqY4ARjv8gZnZ1UEzDeslbft9SXwK5F\r\n"
    "1OtDmwAez+Kr+8Zw/r2bKu70eq5A+KAIF9KJEnLnxNrtAU7cw7BjfNGCYa3DswlC\r\n"
    "5cEdQe3llFl8RZJtznWdttQt5yJwweMEz9D4ac+aS8Kjiav51MC/DyBc7u4IeDYe\r\n"
    "1R1ufWK+qRKZKanTqWA3PS0pF54jmSwiRzQanmCST7nyKCIqoZXZJg476g==\r\n"
    "-----END CERTIFICATE-----\r\n"
};

const char *g_cli_key = \
{
    \
    "-----BEGIN RSA PRIVATE KEY-----\r\n"
    "MIIEpAIBAAKCAQEArDZEOe89ZaXv3UkKh5q35th5pdGD4WnPMl3cdW61Ks1oPq6G\r\n"
    "1LCvG8cq9cqpUy/2jTU+io3qvJvKWb/3xMuNqFpkKIKyfJwWAnVxjmJn7HQ63LyJ\r\n"
    "5HXvI4kS0Xblvv2knhqpFoiqCJ6r70hUXpRjbzfcGe7kXC9eoyRkBab5aMKbP/i5\r\n"
    "IOrzYidJoyi0Ect2468BGsywYo3yOaMg0AHF4yeBhHQNb8P2cUCZKyNmV6dm33N6\r\n"
    "tUHIlm/JkA6jSBvQ5XX0XXUtlY+IgyN0zaBRXzxe3qEm8nI2lX15yWPnH5WqZ8Jv\r\n"
    "9S6AlDRIu7rduT86j+SSW4VdV2IOAPzhA9TUUwIDAQABAoIBAEs5LRYtZaFIGX2M\r\n"
    "kTfooloIxyJiYXwOLv5uN4VoRWSb8uQmcijnwKWemypgSxRWSBkoX9g/44BdgMcx\r\n"
    "bnYdABTA3y0+4d5Cg+6xBFhQrdqAbtHK3WTZCwt3raunhvWlb8nMFX4NVlCLlsjf\r\n"
    "zFK/+Ks1OERvsHqACpOyqqo5ZIqYdiJ/onjW2FmR6v6midB2qKch9pyCTZGoxhJb\r\n"
    "8KgVYUzUoTxLgwwLzOdaZSCieHLJA67GtUDVyLPN0eD8apNlhdmCXCVUMgixNGuE\r\n"
    "Mz71GHnPi+uLLUUige7m1Qk9qTN/deilRgJ1nOp4AWu7sHbkilVXY7O47C6vFLWv\r\n"
    "NuGqplECgYEA7xkOfdS1TWbV/wL0QWCQWSagx+EjnpuxxIikyopFB8EOb3GlAN81\r\n"
    "DQzhmBWbrR8mqfGdjkqGy/XZyQjI7Apyyv1HJlwiPqsihULMs0vHZGxK7aQ7C06/\r\n"
    "yOgsiS3REF4paTUKqilCB+fpCdT8w1r+sT8APAb5XDP+iWSAVeO7LgsCgYEAuGLH\r\n"
    "Ye84tH3puN172b80OGqJX9ytDFcML08N0QcBKwjMx9dw6oh3cBNG4bjcjWZPzMSt\r\n"
    "owUkXuImAfhLmFGp+kyWqs9Soom1m5dTt6/Twwbii9u/243OtpmuKH1Jpg194H3n\r\n"
    "2ebl4uI1qMQpGXpHNukyKhKCVFNtEvrLwIOJh9kCgYEA1Ri167wqooOm8pOFcsdH\r\n"
    "n81nuv2Tq4cEVy+Z/9JgV9rerMDS2QYW2DzEy7FmgFOlOA3Z6LnPjfTeV+h2ermN\r\n"
    "OhGm2i9/be9E4xR4MMy3N2+1EDwxS5CyKbOUDedBz568tcHKcYI4EafF8Tdc2Bg/\r\n"
    "q2WFJfrpQB4vG/Dw2JwRdaUCgYEAtiV2e0ubg8ni6il54KgeySCNZrKUDF2tfsqk\r\n"
    "hnpZ8A44Za5xMG7xNjkEy5UX42NPr6QXN7SrskPLciS7YNRDiyy/pis6obBSRErg\r\n"
    "e9Po6H9Tqv/Bbv2OWBpGzl6+M5ieYfde44ZSUKhQg9CMHUwLL7Z8RM2WDAGB/b7d\r\n"
    "KueGJDkCgYBJzldXgBqOVDgoWLpcuMSZD8bvHHRsY0RSj3Otx4utKOw4ILEXAIyb\r\n"
    "d6BpbrZAinsNWOFXfpIeOobgs+VGOYClni4ULmW4XzRtb1n+9AWP33sMjr4xt8Qs\r\n"
    "fMLnYxXHzu/+uCVEpR3XLL0+bwMDdSqYRL9tc9EkQbvLzvZALl4flA==\r\n"
    "-----END RSA PRIVATE KEY-----\r\n"
};

uintptr_t HAL_SSL_Establish(const char *host,
                            uint16_t port,
                            const char *ca_crt,
                            uint32_t ca_crt_len)
{
    char                port_str[6];
    const char         *alter = host;
    TLSDataParams_pt    pTlsData;

    if (host == NULL || ca_crt == NULL) {
        printf("input params are NULL, abort\n");
        return 0;
    }

    if (!strlen(host) || (strlen(host) < 8)) {
        printf("invalid host: '%s'(len=%d), abort\n", host, (int)strlen(host));
        return 0;
    }

    pTlsData = g_ssl_hooks.malloc(sizeof(TLSDataParams_t));
    if (NULL == pTlsData) {
        return (uintptr_t)NULL;
    }
    memset(pTlsData, 0x0, sizeof(TLSDataParams_t));

    sprintf(port_str, "%u", port);

    mbedtls_platform_set_calloc_free(_SSLCalloc_wrapper, _SSLFree_wrapper);

    if (0 != _TLSConnectNetwork(pTlsData, alter, port_str, ca_crt, ca_crt_len, g_cli_crt, strlen(g_cli_crt) + 1, g_cli_key, strlen(g_cli_key) + 1, NULL, 0)) {
        _network_ssl_disconnect(pTlsData);
        g_ssl_hooks.free((void *)pTlsData);
        return (uintptr_t)NULL;
    }

    return (uintptr_t)pTlsData;
}

int HAL_SSL_Read(uintptr_t handle, char *buf, int len, int timeout_ms)
{
    return _network_ssl_read((TLSDataParams_t *)handle, buf, len, timeout_ms);;
}

int HAL_SSL_Write(uintptr_t handle, const char *buf, int len, int timeout_ms)
{
    return _network_ssl_write((TLSDataParams_t *)handle, buf, len, timeout_ms);
}

int32_t HAL_SSL_Destroy(uintptr_t handle)
{
    if ((uintptr_t)NULL == handle) {
        printf("handle is NULL\n");
        return 0;
    }

    _network_ssl_disconnect((TLSDataParams_t *)handle);
    g_ssl_hooks.free((void *)handle);
    return 0;
}
