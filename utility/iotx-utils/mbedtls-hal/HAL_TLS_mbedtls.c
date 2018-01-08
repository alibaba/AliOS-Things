/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
//#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "mbedtls/error.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/pk.h"
#include "mbedtls/debug.h"
#include "mbedtls/platform.h"
#include "aos/aos.h"

#include "iot_import.h"

#define NULL_STR "NULL"
typedef struct _TLSDataParams {
    mbedtls_ssl_context ssl;          /**< mbed TLS control context. */
    mbedtls_net_context fd;           /**< mbed TLS network context. */
    mbedtls_ssl_config conf;          /**< mbed TLS configuration context. */
    mbedtls_x509_crt cacertl;         /**< mbed TLS CA certification. */
    mbedtls_x509_crt clicert;         /**< mbed TLS Client certification. */
    mbedtls_pk_context pkey;          /**< mbed TLS Client key. */
} TLSDataParams_t, *TLSDataParams_pt;

#define TAG "HAL_TLS"

#define TLS_DEBUG(fmt,args...)  LOGD(TAG, fmt,##args)
#define TLS_INFO(fmt, args...)  LOGI(TAG, fmt,##args)
#define TLS_WARN(fmt, args...)  LOGW(TAG, fmt,##args)
#define TLS_ERR(fmt, args...)   LOGE(TAG, fmt,##args)

#define DEBUG_LEVEL 10
//#define MBEDTLS_DEBUG_C
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
        fprintf((FILE *) ctx, "%s:%04d: %s", file?file:NULL_STR, line, str?str:NULL_STR);
        fflush((FILE *) ctx);
    }
}

static int _real_confirm(int verify_result)
{
    TLS_DEBUG("certificate verification result: 0x%02x", verify_result);

#if defined(FORCE_SSL_VERIFY)
    if ((verify_result & MBEDTLS_X509_BADCERT_EXPIRED) != 0) {
        TLS_ERR("! fail ! ERROR_CERTIFICATE_EXPIRED");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_REVOKED) != 0) {
        TLS_ERR("! fail ! server certificate has been revoked");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_CN_MISMATCH) != 0) {
        TLS_ERR("! fail ! CN mismatch");
        return -1;
    }

    if ((verify_result & MBEDTLS_X509_BADCERT_NOT_TRUSTED) != 0) {
        TLS_ERR("! fail ! self-signed or not signed by a trusted CA");
        return -1;
    }
#endif

    return 0;
}

//static int _ssl_parse_crt(mbedtls_x509_crt *crt)
//{
//    char buf[1024];
//    mbedtls_x509_crt *local_crt = crt;
//    int i = 0;
//    while (local_crt) {
//        mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", local_crt);
//        {
//            char str[512];
//            const char *start, *cur;
//            start = buf;
//            for (cur = buf; *cur != '\0'; cur++) {
//                if (*cur == '\n') {
//                    size_t len = cur - start + 1;
//                    if (len > 511) {
//                        len = 511;
//                    }
//                    memcpy(str, start, len);
//                    str[len] = '\0';
//                    start = cur + 1;
//                    printf("%s", str);
//                }
//            }
//        }
//        SSL_LOG("crt content:%u", (uint32_t)strlen(buf));
//        local_crt = local_crt->next;
//        i++;
//    }
//    return i;
//}

#if defined(MBEDTLS_DEBUG_C)
static void ssl_debug(void *ctx, int level,
                      const char *file, int line, const char *str)
{
    (void)ctx;
    (void) level;

    TLS_DEBUG("%s, line: %d: %s", file?file:NULL_STR, line, str?str:NULL_STR);

    return;
}
#endif

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
    mbedtls_ssl_conf_dbg(conf, ssl_debug, NULL);
#endif
    mbedtls_net_init(tcp_fd);
    mbedtls_ssl_init(ssl);
    mbedtls_ssl_config_init(conf);
    mbedtls_x509_crt_init(crt509_ca);

    /*verify_source->trusted_ca_crt==NULL
     * 0. Initialize certificates
     */

    TLS_DEBUG("Loading the CA root certificate ...");
    if (NULL != ca_crt) {
        if (0 != (ret = mbedtls_x509_crt_parse(crt509_ca, (const unsigned char *)ca_crt, ca_len))) {
            TLS_ERR(" failed ! x509parse_crt returned -0x%04x", -ret);
            return ret;
        }
    }
//    _ssl_parse_crt(crt509_ca);
    TLS_DEBUG(" ok (%d skipped)", ret);


    /* Setup Client Cert/Key */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_CERTS_C)
    mbedtls_x509_crt_init(crt509_cli);
    mbedtls_pk_init(pk_cli);
#endif
    if (cli_crt != NULL && cli_key != NULL) {
#if defined(MBEDTLS_CERTS_C)
        TLS_DEBUG("start prepare client cert .");
        ret = mbedtls_x509_crt_parse(crt509_cli, (const unsigned char *) cli_crt, cli_len);
#else
        {
            ret = 1;
            TLS_DEBUG("MBEDTLS_CERTS_C not defined.");
        }
#endif
        if (ret != 0) {
            TLS_ERR(" failed!  mbedtls_x509_crt_parse returned -0x%x\n", -ret);
            return ret;
        }

#if defined(MBEDTLS_CERTS_C)
        TLS_DEBUG("start mbedtls_pk_parse_key[%s]", cli_pwd?cli_pwd:NULL_STR);
        ret = mbedtls_pk_parse_key(pk_cli, (const unsigned char *) cli_key, key_len, (const unsigned char *) cli_pwd, pwd_len);
#else
        {
            ret = 1;
            TLS_INFO("MBEDTLS_CERTS_C not defined.");
        }
#endif

        if (ret != 0) {
            TLS_ERR(" failed\n  !  mbedtls_pk_parse_key returned -0x%x\n", -ret);
            return ret;
        }
    }
#endif /* MBEDTLS_X509_CRT_PARSE_C */

    return 0;
}


int utils_network_ssl_read(TLSDataParams_t *pTlsData, char *buffer, int len, int timeout_ms)
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
                TLS_ERR("ssl recv error: code = %d, err_str = '%s'", ret, err_str);
                net_status = -2; // connection is closed
                break;
            } else if ((MBEDTLS_ERR_SSL_TIMEOUT == ret)
                       || (MBEDTLS_ERR_SSL_CONN_EOF == ret)
                       || (MBEDTLS_ERR_SSL_SESSION_TICKET_EXPIRED == ret)
                       || (MBEDTLS_ERR_SSL_NON_FATAL == ret)) {
                // read already complete
                // if call mbedtls_ssl_read again, it will return 0 (means EOF)

                return readLen;
            } else {
		if (MBEDTLS_ERR_SSL_WANT_READ == ret && errno == EINTR) {
		    continue;
		}
                //mbedtls_strerror(ret, err_str, sizeof(err_str));
                TLS_ERR("ssl recv error: code = %d, err_str = '%s'", ret, err_str);
                net_status = -1;
                return -1; // Connection error
            }
        }
    }

    return (readLen > 0) ? readLen : net_status;
}

int utils_network_ssl_write(TLSDataParams_t *pTlsData, const char *buffer, int len, int timeout_ms)
{
    uint32_t writtenLen = 0;
    int ret = -1;

    while (writtenLen < len) {
        ret = mbedtls_ssl_write(&(pTlsData->ssl), (unsigned char *)(buffer + writtenLen), (len - writtenLen));
        if (ret > 0) {
            writtenLen += ret;
            continue;
        } else if (ret == 0) {
            TLS_DEBUG("ssl write timeout");
            return 0;
        } else {
            char err_str[33];
            //mbedtls_strerror(ret, err_str, sizeof(err_str));
            TLS_ERR("ssl write fail, code=%d, str=%s", ret, err_str);
            return -1; // Connnection error
        }
    }

    return writtenLen;
}

void utils_network_ssl_disconnect(TLSDataParams_t *pTlsData)
{
    mbedtls_ssl_close_notify(&(pTlsData->ssl));
    mbedtls_net_free(&(pTlsData->fd));
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_free(&(pTlsData->cacertl));
#if defined(MBEDTLS_CERTS_C)
    if ((pTlsData->pkey).pk_info != NULL) {
        TLS_DEBUG("need free client crt&key");
        mbedtls_x509_crt_free(&(pTlsData->clicert));
        mbedtls_pk_free(&(pTlsData->pkey));
    }
#endif
#endif
    mbedtls_ssl_free(&(pTlsData->ssl));
    mbedtls_ssl_config_free(&(pTlsData->conf));
    TLS_DEBUG("ssl_disconnect");
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
int TLSConnectNetwork(TLSDataParams_t *pTlsData, const char *addr, const char *port,
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
        TLS_ERR(" failed ! ssl_client_init returned -0x%04x", -ret);
        return ret;
    }

    /*
     * 1. Start the connection
     */
    TLS_DEBUG("Connecting to /%s/%s...", addr?addr:NULL_STR, port?port:NULL_STR);
    if (0 != (ret = mbedtls_net_connect(&(pTlsData->fd), addr, port, MBEDTLS_NET_PROTO_TCP))) {
        TLS_ERR(" failed ! net_connect returned -0x%04x", -ret);
        return ret;
    }
    TLS_DEBUG(" ok");

    /*
     * 2. Setup stuff
     */
    TLS_DEBUG("  . Setting up the SSL/TLS structure...");
    if ((ret = mbedtls_ssl_config_defaults(&(pTlsData->conf), MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        TLS_ERR(" failed! mbedtls_ssl_config_defaults returned %d", ret);
        return ret;
    }

    mbedtls_ssl_conf_max_version(&pTlsData->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
    mbedtls_ssl_conf_min_version(&pTlsData->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);

    TLS_DEBUG(" ok");

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
#if defined(MBEDTLS_CERTS_C)
    if ((ret = mbedtls_ssl_conf_own_cert(&(pTlsData->conf), &(pTlsData->clicert), &(pTlsData->pkey))) != 0) {
        TLS_ERR(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n", ret);
        return ret;
    }
#endif
#endif
    mbedtls_ssl_conf_rng(&(pTlsData->conf), _ssl_random, NULL);
    mbedtls_ssl_conf_dbg(&(pTlsData->conf), _ssl_debug, NULL);
    // mbedtls_ssl_conf_dbg( &(pTlsData->conf), _ssl_debug, stdout );

    if ((ret = mbedtls_ssl_setup(&(pTlsData->ssl), &(pTlsData->conf))) != 0) {
        TLS_ERR("failed! mbedtls_ssl_setup returned %d", ret);
        return ret;
    }
    mbedtls_ssl_set_hostname(&(pTlsData->ssl), addr);
    mbedtls_ssl_set_bio(&(pTlsData->ssl), &(pTlsData->fd), mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    /*
      * 4. Handshake
      */
    TLS_DEBUG("Performing the SSL/TLS handshake...");

    while ((ret = mbedtls_ssl_handshake(&(pTlsData->ssl))) != 0) {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            TLS_ERR("failed  ! mbedtls_ssl_handshake returned -0x%04x", -ret);
            return ret;
        }
    }
    TLS_DEBUG(" ok");
    /*
     * 5. Verify the server certificate
     */
    TLS_DEBUG("  . Verifying peer X.509 certificate..");
    if (0 != (ret = _real_confirm(mbedtls_ssl_get_verify_result(&(pTlsData->ssl))))) {
        TLS_ERR(" failed  ! verify result not confirmed.");
        return ret;
    }
    // n->my_socket = (int)((n->tlsdataparams.fd).fd);
    // WRITE_IOT_DEBUG_LOG("my_socket=%d", n->my_socket);

    return 0;
}

int utils_network_ssl_connect(TLSDataParams_t *pTlsData, const char *addr, const char *port, const char *ca_crt,
                              size_t ca_crt_len)
{
    return TLSConnectNetwork(pTlsData, addr, port, NULL, 0, NULL, 0, NULL, 0, NULL, 0);
}

int HAL_SSL_Read(uintptr_t handle, char *buf, int len, int timeout_ms)
{
    return utils_network_ssl_read((TLSDataParams_t *)handle, buf, len, timeout_ms);;
}

int HAL_SSL_Write(uintptr_t handle, const char *buf, int len, int timeout_ms)
{
    return utils_network_ssl_write((TLSDataParams_t *)handle, buf, len, timeout_ms);
}

int32_t HAL_SSL_Destroy(uintptr_t handle)
{
    if ((uintptr_t)NULL == handle) {
        TLS_WARN("handle is NULL");
        return 0;
    }

    utils_network_ssl_disconnect((TLSDataParams_t *)handle);
    free((void *)handle);
    return 0;
}

int ssl_fd = -1;

int get_ssl_fd()
{
   return ssl_fd;
}

uintptr_t HAL_SSL_Establish(const char *host,
                                      uint16_t port,
                                      const char *ca_crt,
                                      size_t ca_crt_len)
{
    char port_str[6];
    TLSDataParams_pt pTlsData;

    pTlsData = malloc(sizeof(TLSDataParams_t));
    if (NULL == pTlsData) {
        return (uintptr_t)NULL;
    }

    sprintf(port_str, "%u", port);

    if (0 != TLSConnectNetwork(pTlsData, host, port_str, ca_crt, ca_crt_len, NULL, 0, NULL, 0, NULL, 0)) {
        mbedtls_x509_crt_free(&(pTlsData->cacertl));
#if defined(MBEDTLS_CERTS_C)
        mbedtls_x509_crt_free(&(pTlsData->clicert));
#endif
        if (pTlsData->ssl.hostname) {
            mbedtls_free(pTlsData->ssl.hostname);
            pTlsData->ssl.hostname = NULL;
        }
        utils_network_ssl_disconnect(pTlsData);
        free(pTlsData);
        return 0;
    }
    ssl_fd = pTlsData->fd.fd;
    //SSL_LOG(" ssl_fd %d \n ", ssl_fd);
    return (uintptr_t)pTlsData;
}
