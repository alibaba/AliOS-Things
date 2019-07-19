/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
//#include <memory.h>
#include <stdlib.h>
#include <string.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "itls/ssl.h"
#include "itls/net.h"
#include "itls/debug.h"
#include "itls/platform.h"
#include "iot_import.h"

#define LOG_TAG "HAL_iTLS"

#define platform_info(format, ...) LOGI(LOG_TAG, format, ##__VA_ARGS__)
#define platform_err(format, ...) LOGE(LOG_TAG, format, ##__VA_ARGS__)


#define CONFIG_ITLS_TIME_TEST

#define SEND_TIMEOUT_SECONDS (10)

typedef struct _TLSDataParams {
    mbedtls_ssl_context ssl;          /**< iTLS control context. */
    mbedtls_net_context fd;           /**< iTLS network context. */
    mbedtls_ssl_config conf;          /**< iTLS configuration context. */
} TLSDataParams_t, *TLSDataParams_pt;

/**< set debug log level, 0 close*/
#define DEBUG_LEVEL 0

#if defined(CONFIG_ITLS_TIME_TEST)
#include <sys/time.h>
static struct timeval tv1, tv2;
#endif

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
    ((void) ctx);
    ((void) level);

    platform_info("%s:%04d: %s", file, line, str);
}

#if defined(_PLATFORM_IS_LINUX_)
#include "aos/errno.h"
extern int errno;

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
            platform_err("setsockopt error");
        }

        platform_info("setsockopt SO_SNDTIMEO timeout: %ds", sendtimeout.tv_sec);

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
 * @brief This function connects to the specific SSL server with TLS.
 * @param[in] n is the the network structure pointer.
 * @param[in] addr is the Server Host name or IP address.
 * @param[in] port is the Server Port.
 * @param[in] product_key is the product name.
 * @param[in] product_secret is the product secret.
 *
 * @retval       0 : success.
 * @retval     < 0 : fail.
 */
static int _TLSConnectNetwork(TLSDataParams_t *pTlsData,
                              const char *addr,
                              const char *port,
                              const char *product_key,
                              const char *product_secret)
{
    int ret = -1;

    /*
     * 0. Initialize the RNG and the session data
     */
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif
    mbedtls_ssl_init(&(pTlsData->ssl));
    mbedtls_net_init(&(pTlsData->fd));
    mbedtls_ssl_config_init(&(pTlsData->conf));

    /*
     * 1. Start the connection
     */
    platform_info("Connecting to /%s/%s...", addr, port);
#if defined(_PLATFORM_IS_LINUX_)
    if (0 != (ret = mbedtls_net_connect_timeout(&(pTlsData->fd),
                    addr, port, MBEDTLS_NET_PROTO_TCP, SEND_TIMEOUT_SECONDS))) {
        platform_err(" failed ! net_connect returned -0x%04x", -ret);
        return ret;
    }
#else
    if (0 != (ret = mbedtls_net_connect(&(pTlsData->fd), addr, port, MBEDTLS_NET_PROTO_TCP))) {
        platform_err(" failed ! net_connect returned -0x%04x", -ret);
        return ret;
    }
#endif
    platform_info(" ok");

    /*
     * 2. Setup stuff
     */
    platform_info("  . Setting up the SSL/TLS structure...");
    if ((ret = mbedtls_ssl_config_defaults(&(pTlsData->conf),
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        platform_err(" failed! mbedtls_ssl_config_defaults returned %d", ret);
        goto _out;
    }

    mbedtls_ssl_conf_max_version(&pTlsData->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
    mbedtls_ssl_conf_min_version(&pTlsData->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);

    platform_info(" ok");

    mbedtls_ssl_conf_rng(&(pTlsData->conf), _ssl_random, NULL);
    mbedtls_ssl_conf_dbg(&(pTlsData->conf), _ssl_debug, NULL);

    /* "OPTIONAL", set extra data for client authentication */
    if ((ret = mbedtls_ssl_conf_auth_extra(&(pTlsData->conf), product_key, strlen(product_key))) != 0) {
        platform_err(" failed! mbedtls_ssl_config_auth_extra returned %d", ret);
        goto _out;
    }

    /* "OPTIONAL", token for id2 one-time provisioning */
    if ((ret = mbedtls_ssl_conf_auth_token(&(pTlsData->conf), product_secret, strlen(product_secret))) != 0) {
        platform_err( " failed\n  ! mbedtls_ssl_conf_auth_token returned %d\n\n", ret );
        goto _out;
    }

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    if ((ret = mbedtls_ssl_conf_max_frag_len(&(pTlsData->conf), MBEDTLS_SSL_MAX_FRAG_LEN_1024)) != 0) {
        platform_err( " failed\n  ! mbedtls_ssl_conf_max_frag_len returned %d\n\n", ret );
        goto _out;
    }
#endif

    if ((ret = mbedtls_ssl_setup(&(pTlsData->ssl), &(pTlsData->conf))) != 0) {
        platform_err(" failed! mbedtls_ssl_setup returned %d", ret);
        goto _out;
    }

    mbedtls_ssl_set_bio(&(pTlsData->ssl), &(pTlsData->fd), mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    /*
      * 3. Handshake
      */
    platform_info("Performing the SSL/TLS handshake...");
#if defined(CONFIG_ITLS_TIME_TEST)
    gettimeofday(&tv1, NULL);
#endif

    while ((ret = mbedtls_ssl_handshake(&(pTlsData->ssl))) != 0) {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            platform_err("failed  ! mbedtls_ssl_handshake returned -0x%04x", -ret);
            goto _out;
        }
    }

#if defined(CONFIG_ITLS_TIME_TEST)
    gettimeofday(&tv2, NULL);
    platform_info("=========================== iTLS handshake used time(usec): %d\n",
                   (int)((tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec)));
#endif

    platform_info(" ok");

_out:
    if (ret != 0) {
        mbedtls_net_free(&(pTlsData->fd));
        mbedtls_ssl_free(&(pTlsData->ssl));
        mbedtls_ssl_config_free(&(pTlsData->conf));
    }

    return ret;
}

static int _network_ssl_read(TLSDataParams_t *pTlsData, char *buffer, int len, int timeout_ms)
{
    uint32_t        readLen = 0;
    static int      net_status = 0;
    int             ret = -1;

#if defined(CONFIG_ITLS_TIME_TEST)
    gettimeofday(&tv1, NULL);
#endif

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
                platform_err("ssl recv peer close notify");
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
#ifdef _PLATFORM_IS_LINUX_
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

#if defined(CONFIG_ITLS_TIME_TEST)
    gettimeofday(&tv2, NULL);
    platform_info("=========================== iTLS receive data(%d bytes) used time(usec): %d\n",
                   readLen, (int)((tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec)));
#endif

    return (readLen > 0) ? readLen : net_status;
}

static int _network_ssl_write(TLSDataParams_t *pTlsData, const char *buffer, int len, int timeout_ms)
{
    uint32_t writtenLen = 0;
    int ret = -1;

#if defined(CONFIG_ITLS_TIME_TEST)
    gettimeofday(&tv1, NULL);
#endif

    while (writtenLen < len) {
        ret = mbedtls_ssl_write(&(pTlsData->ssl), (unsigned char *)(buffer + writtenLen), (len - writtenLen));
        if (ret > 0) {
            writtenLen += ret;
            continue;
        } else if (ret == 0) {
            platform_err("ssl write timeout");
            return 0;
        } else {
            platform_err("ssl write error, code = %d", ret);
            return -1;
        }
    }

#if defined(CONFIG_ITLS_TIME_TEST)
    gettimeofday(&tv2, NULL);
    platform_info("iTLS send data(%d bytes) used time(usec): %d\n",
                   writtenLen,  (int)((tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec)));
#endif

    return writtenLen;
}

static void _network_ssl_disconnect(TLSDataParams_t *pTlsData)
{
    mbedtls_ssl_close_notify(&(pTlsData->ssl));
    mbedtls_net_free(&(pTlsData->fd));
    mbedtls_ssl_free(&(pTlsData->ssl));
    mbedtls_ssl_config_free(&(pTlsData->conf));
    platform_info("ssl_disconnect");
}

uintptr_t HAL_SSL_Establish(const char *host,
                             uint16_t port,
                             const char *ca_crt,
                             size_t ca_crt_len)
{
    char port_str[6];
    TLSDataParams_pt pTlsData;
    const char *product_key = NULL;
    const char *product_secret = NULL;

    if (host == NULL || ca_crt == NULL) {
        platform_err("input params are NULL");
        return 0;
    }

    product_key = ca_crt;
    product_secret = ca_crt + strlen(product_key) + 1;

    pTlsData = HAL_Malloc(sizeof(TLSDataParams_t));
    if (NULL == pTlsData) {
        return (uintptr_t)NULL;
    }

    memset(pTlsData, 0x0, sizeof(TLSDataParams_t));

    sprintf(port_str, "%u", port);

    if (0 != _TLSConnectNetwork(pTlsData, host, port_str, product_key, product_secret)) {
        HAL_Free((void *)pTlsData);
        return (uintptr_t)NULL; 
    }

    return (uintptr_t)pTlsData;
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

int32_t HAL_SSL_Write(uintptr_t handle, const char *buf, int len, int timeout_ms)
{
    return _network_ssl_write((TLSDataParams_t *)handle, buf, len, timeout_ms);
}

int32_t HAL_SSL_Read(uintptr_t handle, char *buf, int len, int timeout_ms)
{
    return _network_ssl_read((TLSDataParams_t *)handle, buf, len, timeout_ms);;
}

