/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "httpclient.h"
#include "http_opts.h"
#include "http_wrappers.h"

#include "network/network.h"

#if CONFIG_HTTP_SECURE
#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/certs.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

/** @brief   This structure defines the httpclient ssl structure.  */
typedef struct {
    mbedtls_ssl_context ssl_ctx;        /**< mbedtls ssl context */
    mbedtls_net_context net_ctx;        /**< fill in socket id   */
    mbedtls_ssl_config ssl_conf;        /**< entropy context     */
    mbedtls_entropy_context entropy;    /**< ssl configuration   */
    mbedtls_ctr_drbg_context ctr_drbg;  /**< ctr drbg context    */
    mbedtls_x509_crt_profile profile;   /**< x509 cacert profile */
    mbedtls_x509_crt cacert;            /**< x509 cacert         */
    mbedtls_x509_crt clicert;           /**< x509 client cacert  */
    mbedtls_pk_context pkey;            /**< pkey context        */
} httpclient_ssl_t;


#if defined(MBEDTLS_DEBUG_C)
#define DEBUG_LEVEL 2
#endif
#endif


/*  
 *  Conncection wrapper function for HTTP
 *  @param[in] client    pointer to the httpclient.
 *  @param[in] host      host url
 *  @return    0 success other failed
 */
int http_tcp_conn_wrapper(httpclient_t *client, const char *host)
{
    struct addrinfo hints, *addr_list, *cur;
    int ret = 0;
    char port[10] = {0};

    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    snprintf(port, sizeof(port), "%d", client->remote_port) ;
    if ( getaddrinfo( host, port , &hints, &addr_list ) != 0 ) {
        http_err("getaddrinfo != 0, return EDNS");
        return HTTP_EDNS;
    }

    /* Try the sockaddrs until a connection succeeds */
    ret = HTTP_EDNS;
    for ( cur = addr_list; cur != NULL; cur = cur->ai_next ) {
        client->socket = (int) socket( cur->ai_family, cur->ai_socktype,
                                        cur->ai_protocol );
        if ( client->socket < 0 ) {
            ret = HTTP_ECONN;
            continue;
        }

        if ( connect( client->socket, cur->ai_addr, (int)cur->ai_addrlen ) == 0 ) {
            ret = 0;
            break;
        }

        close(client->socket);
        ret = HTTP_ECONN;
    }

    freeaddrinfo( addr_list );

    return ret;
}

int http_tcp_close_wrapper(httpclient_t *client)
{
    close(client->socket);
    client->socket = -1;
    return 0;
}

int http_tcp_send_wrapper(httpclient_t *client, const char *data, int length)
{
    int written_len = 0;

    while (written_len < length) {
        int ret = send(client->socket, data + written_len, length - written_len, 0);
        if (ret > 0) {
            written_len += ret;
            continue;
        } else if (ret == 0) {
            return written_len;
        } else {
            http_err("Connection err ret=%d errno=%d\n", ret, errno);
            return -1; /* Connnection error */
        }
    }

    return written_len;
}

int http_tcp_recv_wrapper(httpclient_t *client, char *buf, int buflen, int timeout_ms, int *p_read_len)
{
    int ret = 0, select_ret;
    size_t readLen = 0;
    struct timeval timeout;
    fd_set         sets;
    int err_record;
    int sockfd = client->socket;

    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = 0;

    while (readLen < buflen) {
        FD_ZERO(&sets);
        FD_SET(sockfd, &sets);

        select_ret = select(sockfd + 1, &sets, NULL, NULL, &timeout);
        err_record = errno;
        if (select_ret > 0) {
            if (0 == FD_ISSET(sockfd, &sets)) {
                ret = 0;
                http_debug("select continue");
                continue;
            }

            ret = recv(sockfd, buf + readLen, buflen - readLen, 0);
            err_record = errno;
            if (ret < 0 ) {
                if ((EINTR == err_record) || (EAGAIN == err_record) || (EWOULDBLOCK == err_record) ||
                    (EPROTOTYPE == err_record) || (EALREADY == err_record) || (EINPROGRESS == err_record)) {
                    http_debug("recv continue %d ret %d", err_record, ret);
                    continue;
                }
            } else if (ret == 0) {
                    http_debug("recv return 0 disconnected");
                    ret = HTTP_ECLSD;
            }
        } else if (select_ret == 0) {
            http_info("select return 0 may disconnected");
            ret = HTTP_ECLSD;
        } else {
            http_debug("select return %d errno %d", select_ret, err_record);
            if (err_record == EINTR) {
                continue;
            }

            ret = select_ret;
        }

        if (ret > 0) {
            readLen += ret;
        } else if (ret == 0) {
            break;
        } else if (ret == HTTP_ECLSD) {
            break;
        } else {
            http_err("Connection error (recv returned %d readLen:%d)", ret,readLen);
            *p_read_len = readLen;
            return HTTP_ECONN;
        }
    }

    *p_read_len = readLen;
    if (ret == HTTP_ECLSD) {
        return ret;
    } else {
        return HTTP_SUCCESS;
    }
}

#if CONFIG_HTTP_SECURE

int http_ssl_send_wrapper(httpclient_t *client, const char *data, size_t length)
{
    size_t written_len = 0;

    httpclient_ssl_t *ssl = (httpclient_ssl_t *) client->ssl;
    if (!ssl) {
    	return -1;
    }

    while (written_len < length) {
        int ret = mbedtls_ssl_write(&ssl->ssl_ctx, (unsigned char *)(data + written_len), (length - written_len));
        if (ret > 0) {
            written_len += ret;
            continue;
        } else if (ret == 0) {
            return written_len;
        } else {
            return -1; /* Connnection error */
        }
    }

    return written_len;
}

static void httpclient_debug( void *ctx, int level, const char *file, int line, const char *str )
{
    http_debug("%s", str);
}

static int httpclient_random(void *prng, unsigned char *output, size_t output_len)
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

int http_ssl_conn_wrapper(httpclient_t *client, const char *host)
{
    int authmode = MBEDTLS_SSL_VERIFY_NONE;
#ifdef MBEDTLS_ENTROPY_C
    const char *pers = "https";
#endif
    int value, ret = 0;
    uint32_t flags;
    char port[10] = {0};
    httpclient_ssl_t *ssl;

    client->ssl = (httpclient_ssl_t *)malloc(sizeof(httpclient_ssl_t));
    if (!client->ssl) {
        http_err("Memory malloc error.");
        ret = -1;
        goto exit;
    }
    ssl = (httpclient_ssl_t *)client->ssl;

    if (client->server_cert)
        authmode = MBEDTLS_SSL_VERIFY_REQUIRED;

    /*
     * Initialize the RNG and the session data
     */
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif
    mbedtls_net_init(&ssl->net_ctx);
    mbedtls_ssl_init(&ssl->ssl_ctx);
    mbedtls_ssl_config_init(&ssl->ssl_conf);
#ifdef MBEDTLS_X509_CRT_PARSE_C
    mbedtls_x509_crt_init(&ssl->cacert);
    mbedtls_x509_crt_init(&ssl->clicert);
#endif
    mbedtls_pk_init(&ssl->pkey);
#ifdef MBEDTLS_ENTROPY_C
    mbedtls_ctr_drbg_init(&ssl->ctr_drbg);
    mbedtls_entropy_init(&ssl->entropy);
    if ((value = mbedtls_ctr_drbg_seed(&ssl->ctr_drbg,
                               mbedtls_entropy_func,
                               &ssl->entropy,
                               (const unsigned char*)pers,
                               strlen(pers))) != 0) {
        http_err("mbedtls_ctr_drbg_seed() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }
#endif
    /*
    * Load the Client certificate
    */
    if (client->client_cert && client->client_pk) {
#ifdef MBEDTLS_X509_CRT_PARSE_C
        ret = mbedtls_x509_crt_parse(&ssl->clicert, (const unsigned char *)client->client_cert, client->client_cert_len);
        if (ret < 0) {
            http_err("Loading cli_cert failed! mbedtls_x509_crt_parse returned -0x%x.", -ret);
            ret = -1;
            goto exit;
        }
#endif
        ret = mbedtls_pk_parse_key(&ssl->pkey, (const unsigned char *)client->client_pk, client->client_pk_len, NULL, 0);
        if (ret != 0) {
            http_err("failed! mbedtls_pk_parse_key returned -0x%x.", -ret);
            ret = -1;
            goto exit;
        }
    }

    /*
    * Load the trusted CA
    */
    /* cert_len passed in is gotten from sizeof not strlen */
#ifdef MBEDTLS_X509_CRT_PARSE_C
    if (client->server_cert && ((value = mbedtls_x509_crt_parse(&ssl->cacert,
                                        (const unsigned char *)client->server_cert,
                                        client->server_cert_len)) < 0)) {
        http_err("mbedtls_x509_crt_parse() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }
#endif

    /*
     * Start the connection
     */
    snprintf(port, sizeof(port), "%d", client->remote_port) ;
    if ((ret = mbedtls_net_connect(&ssl->net_ctx, host, port, MBEDTLS_NET_PROTO_TCP)) != 0) {
        http_err("failed! mbedtls_net_connect returned %d, port:%s.", ret, port);
        ret = -1;
        goto exit;
    }

    client->socket = ssl->net_ctx.fd;

    /*
     * Setup stuff
     */
    if ((value = mbedtls_ssl_config_defaults(&ssl->ssl_conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        http_err("mbedtls_ssl_config_defaults() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }

    // TODO: add customerization encryption algorithm
#ifdef MBEDTLS_X509_CRT_PARSE_C
    memcpy(&ssl->profile, ssl->ssl_conf.cert_profile, sizeof(mbedtls_x509_crt_profile));
    ssl->profile.allowed_mds = ssl->profile.allowed_mds | MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_MD5);
    mbedtls_ssl_conf_cert_profile(&ssl->ssl_conf, &ssl->profile);

    mbedtls_ssl_conf_authmode(&ssl->ssl_conf, authmode);
    mbedtls_ssl_conf_ca_chain(&ssl->ssl_conf, &ssl->cacert, NULL);

    if (client->client_cert && (ret = mbedtls_ssl_conf_own_cert(&ssl->ssl_conf, &ssl->clicert, &ssl->pkey)) != 0) {
        http_err(" failed! mbedtls_ssl_conf_own_cert returned %d.", ret );
        ret = -1;
        goto exit;
    }
#endif

    mbedtls_ssl_conf_rng(&ssl->ssl_conf, httpclient_random, &ssl->ctr_drbg);
    mbedtls_ssl_conf_dbg(&ssl->ssl_conf, httpclient_debug, NULL);

    if ((value = mbedtls_ssl_setup(&ssl->ssl_ctx, &ssl->ssl_conf)) != 0) {
        http_err("mbedtls_ssl_setup() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }

    mbedtls_ssl_set_bio(&ssl->ssl_ctx, &ssl->net_ctx, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);
    mbedtls_ssl_conf_read_timeout(&ssl->ssl_conf, 10000);

    /*
    * Handshake
    */
    while ((ret = mbedtls_ssl_handshake(&ssl->ssl_ctx)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            http_err("mbedtls_ssl_handshake() failed, ret:-0x%x.", -ret);
            ret = -1;
            goto exit;
        }
    }

    /*
     * Verify the server certificate
     */
    if ((flags = mbedtls_ssl_get_verify_result(&ssl->ssl_ctx)) != 0) {
        char vrfy_buf[512];
        http_err("svr_cert varification failed.");
        mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);
        http_debug("%s", vrfy_buf);
        ret = -1;
    } else {
        http_info("svr_cert varification ok.");
    }

exit:
    if (ret != 0) {
        http_debug("ret=%d.", ret);
        http_ssl_close_wrapper(client);
    }
    return ret;
}

int http_ssl_close_wrapper(httpclient_t *client)
{
    httpclient_ssl_t *ssl = (httpclient_ssl_t *)client->ssl;

    if (!ssl)
        return -1;

    client->ssl = NULL;
    client->client_cert = NULL;
    client->server_cert = NULL;
    client->client_pk = NULL;
    client->socket = -1;

    mbedtls_ssl_close_notify(&ssl->ssl_ctx);
    mbedtls_net_free(&ssl->net_ctx);
#ifdef MBEDTLS_X509_CRT_PARSE_C
    mbedtls_x509_crt_free(&ssl->cacert);
    mbedtls_x509_crt_free(&ssl->clicert);
#endif
    mbedtls_pk_free(&ssl->pkey);
    mbedtls_ssl_free(&ssl->ssl_ctx);
    mbedtls_ssl_config_free(&ssl->ssl_conf);
#ifdef MBEDTLS_ENTROPY_C
    mbedtls_ctr_drbg_free(&ssl->ctr_drbg);
    mbedtls_entropy_free(&ssl->entropy);
#endif
    free(ssl);
    return 0;
}

int http_ssl_recv_wrapper(httpclient_t *client, char *buf, int buflen, int timeout_ms, int *p_read_len)
{
    int ret = 0;
    size_t readLen = 0;

    httpclient_ssl_t *ssl = (httpclient_ssl_t *) client->ssl;
    if (!ssl) {
    	return HTTP_ECONN;
    }

    mbedtls_ssl_conf_read_timeout(&ssl->ssl_conf, timeout_ms);
    
    while (readLen < buflen) {
        ret = mbedtls_ssl_read(&ssl->ssl_ctx, (unsigned char *)buf + readLen, buflen - readLen);
        if (ret == 0) {
            ret = HTTP_ECLSD;
        } else if (ret < 0) {
            http_debug("mbedtls_ssl_read, return:%d", ret);
            if ((MBEDTLS_ERR_SSL_TIMEOUT == ret)
                 || (MBEDTLS_ERR_SSL_CONN_EOF == ret)
                 || (MBEDTLS_ERR_SSL_SESSION_TICKET_EXPIRED == ret)
                 || (MBEDTLS_ERR_SSL_NON_FATAL == ret)) {
                /* read already complete */
                /* if call mbedtls_ssl_read again, it will return 0 (means EOF) */
            }

            if (MBEDTLS_ERR_SSL_WANT_READ == ret) {
                continue;
            }

            if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
                ret = HTTP_ECLSD;
            }
        }

        if (ret > 0) {
            readLen += ret;
        } else if (ret == 0) {
            break;
        } else if (ret == HTTP_ECLSD) {
            break;
        } else {
            http_err("Connection error (recv returned %d readLen:%d)", ret,readLen);
            *p_read_len = readLen;
            return HTTP_ECONN;
        }
    }

    *p_read_len = readLen;

    if (ret == HTTP_ECLSD) {
        return ret;
    } else {
        return HTTP_SUCCESS;
    }
}

#endif
