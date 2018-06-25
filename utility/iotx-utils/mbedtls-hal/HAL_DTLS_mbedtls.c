/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include "iot_import_dtls.h"

#include "ali_crypto.h"
#include "mbedtls/config.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/error.h"
#include "mbedtls/timing.h"

#define COAP_DTLS_SUPPORT
#ifdef COAP_DTLS_SUPPORT

#define NULL_STR "NULL"
static int ssl_random(void *prng, unsigned char *output, size_t output_len)
{
    struct timeval tv;

    (void)prng;

    gettimeofday(&tv, NULL);
    ali_seed((uint8_t *)&tv.tv_usec, sizeof(suseconds_t));
    ali_rand_gen(output, output_len);

    return 0;
}

#if defined(MBEDTLS_DEBUG_C)
extern int csp_printf(const char *fmt, ...);
static void ssl_debug(void *ctx, int level,
                      const char *file, int line, const char *str)
{
    (void)ctx;
    (void) level;

    csp_printf("%s, line: %d: %s", file ? file : NULL_STR, line, str ? str : NULL_STR);

    return;
}
#endif

typedef struct {
    dtls_network_t         network;
    mbedtls_ssl_send_t     *send_fn;
    mbedtls_ssl_recv_t     *recv_fn;
    mbedtls_ssl_recv_timeout_t *recv_timeout_fn;
    mbedtls_ssl_context    context;
    mbedtls_ssl_config     conf;
    mbedtls_ctr_drbg_context ctr_drbg;
    //    mbedtls_entropy_context entropy;
#ifdef MBEDTLS_X509_CRT_PARSE_C
    mbedtls_x509_crt       cacert;
#endif

    mbedtls_timing_delay_context timer;
    mbedtls_ssl_cookie_ctx cookie_ctx;
} dtls_session_t;


static  void *DTLSCalloc_wrapper(size_t n, size_t s)
{
    void *ptr = NULL;
    size_t len = n * s;
    ptr = coap_malloc(len);
    if (NULL != ptr) {
        memset(ptr, 0x00, len);
    }
    return ptr;
}

static  void DTLSFree_wrapper(void *ptr)
{
    if (NULL != ptr) {
        coap_free(ptr);
        ptr = NULL;
    }
}

static unsigned int DTLSVerifyOptions_set(dtls_session_t *p_dtls_session,
                                          unsigned char    *p_ca_cert_pem)
{
    unsigned int err_code = DTLS_SUCCESS;

#ifdef MBEDTLS_X509_CRT_PARSE_C
    if (p_ca_cert_pem != NULL) {
        mbedtls_ssl_conf_authmode(&p_dtls_session->conf, MBEDTLS_SSL_VERIFY_REQUIRED );
        DTLS_TRC("Call mbedtls_ssl_conf_authmode\r\n");

        DTLS_TRC("x509 ca cert pem len %d\r\n%s\r\n", (int)strlen((char *)p_ca_cert_pem) + 1, p_ca_cert_pem);
        int result = mbedtls_x509_crt_parse(&p_dtls_session->cacert,
                                            p_ca_cert_pem,
                                            strlen((const char *)p_ca_cert_pem) + 1);

        DTLS_TRC("mbedtls_x509_crt_parse result 0x%04x\r\n", result);
        if ( result < 0 ) {
            err_code = DTLS_INVALID_CA_CERTIFICATE;
        } else {
            mbedtls_ssl_conf_ca_chain(&p_dtls_session->conf, &p_dtls_session->cacert, NULL);
        }
    } else
#endif
    {
        mbedtls_ssl_conf_authmode(&p_dtls_session->conf, MBEDTLS_SSL_VERIFY_NONE );
    }

    return err_code;
}

static void DTLSLog_wrapper(void        *p_ctx, int level,
                            const char *p_file, int line,   const char *p_str)
{
    DTLS_INFO("[mbedTLS]:[%s]:[%d]: %s\r\n", p_file ? p_file : NULL_STR, line, p_str ? p_str : NULL_STR);
}


static void DTLSTimer_set(void *data, unsigned int int_ms, unsigned int fin_ms)
{
    mbedtls_timing_delay_context *ctx = (mbedtls_timing_delay_context *) data;

    ctx->int_ms = int_ms;
    ctx->fin_ms = fin_ms;

    if ( fin_ms != 0 ) {
        (void) mbedtls_timing_get_timer(&ctx->timer, 1);
    }
}

static int DTLSTimer_get(void *data)
{
    mbedtls_timing_delay_context *ctx = (mbedtls_timing_delay_context *) data;
    unsigned long elapsed_ms;

    if ( ctx->fin_ms == 0 ) {
        return ( -1 );
    }

    elapsed_ms = mbedtls_timing_get_timer( &ctx->timer, 0 );

    if ( elapsed_ms >= ctx->fin_ms ) {
        return ( 2 );
    }

    if ( elapsed_ms >= ctx->int_ms ) {
        return ( 1 );
    }

    return ( 0 );
}

static unsigned int DTLSContext_setup(dtls_session_t *p_dtls_session, coap_dtls_options_t  *p_options)
{
    int   result = 0;

    mbedtls_ssl_init(&p_dtls_session->context);

    result = mbedtls_ssl_setup(&p_dtls_session->context, &p_dtls_session->conf);
    DTLS_TRC("mbedtls_ssl_setup result 0x%04x\r\n", result);

    if (result == 0) {
        if (p_dtls_session->conf.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
            mbedtls_ssl_set_timer_cb(&p_dtls_session->context,
                                     (void *)&p_dtls_session->timer,
                                     DTLSTimer_set,
                                     DTLSTimer_get);
        }

#ifdef MBEDTLS_X509_CRT_PARSE_C
        DTLS_TRC("mbedtls_ssl_set_hostname %s\r\n", p_options->p_host ? p_options->p_host : NULL_STR);
        mbedtls_ssl_set_hostname(&p_dtls_session->context, p_options->p_host);
#endif
        mbedtls_ssl_set_bio(&p_dtls_session->context,
                            (void *)&p_dtls_session->network,
                            p_dtls_session->send_fn,
                            p_dtls_session->recv_fn,
                            p_dtls_session->recv_timeout_fn);
        DTLS_TRC("mbedtls_ssl_set_bio result 0x%04x\r\n", result);

        do {
            result = mbedtls_ssl_handshake(&p_dtls_session->context);
        } while ( result == MBEDTLS_ERR_SSL_WANT_READ ||
                  result == MBEDTLS_ERR_SSL_WANT_WRITE );
        DTLS_TRC("mbedtls_ssl_handshake result 0x%04x\r\n", result);
    }

    return (result ? DTLS_HANDSHAKE_FAILED : DTLS_SUCCESS);
}

unsigned int HAL_DTLSSession_free(DTLSContext *context)
{
    int ret;
    dtls_session_t *p_dtls_session = NULL;
    if (context != NULL) {
        p_dtls_session = (dtls_session_t *)context;
        do {
            ret = mbedtls_ssl_close_notify(&p_dtls_session->context);
        } while (ret == MBEDTLS_ERR_SSL_WANT_WRITE);
        p_dtls_session->network.socket_id = -1;
        memset(p_dtls_session->network.remote_addr, 0x00, NETWORK_ADDR_LEN);
        p_dtls_session->network.remote_port = 0;
        p_dtls_session->recv_fn = NULL;
        p_dtls_session->send_fn = NULL;
        p_dtls_session->recv_timeout_fn = NULL;

#ifdef MBEDTLS_X509_CRT_PARSE_C
        mbedtls_x509_crt_free(&p_dtls_session->cacert);
#endif
        mbedtls_ssl_cookie_free(&p_dtls_session->cookie_ctx);

        mbedtls_ssl_config_free(&p_dtls_session->conf);
        mbedtls_ssl_free(&p_dtls_session->context);

        //        mbedtls_ctr_drbg_free(&p_dtls_session->ctr_drbg);
        //        mbedtls_entropy_free(&p_dtls_session->entropy);
        coap_free(context);
    }

    return DTLS_SUCCESS;
}

DTLSContext *HAL_DTLSSession_init()
{
    dtls_session_t *p_dtls_session = NULL;
    p_dtls_session = coap_malloc(sizeof(dtls_session_t));
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(0);
#endif
    //    mbedtls_platform_set_calloc_free(DTLSCalloc_wrapper, DTLSFree_wrapper);
    if (NULL != p_dtls_session) {
        /*p_dtls_session->cookie_ctx.mutex.is_valid must init to zero! */
        memset(p_dtls_session, 0x00, sizeof(dtls_session_t));
        p_dtls_session->network.socket_id = -1;
        // memset(p_dtls_session->network.remote_addr, 0x00, NETWORK_ADDR_LEN);
        // p_dtls_session->network.remote_port = 0;
        // p_dtls_session->recv_fn = NULL;
        // p_dtls_session->send_fn = NULL;
        // p_dtls_session->recv_timeout_fn = NULL;

        mbedtls_ssl_init(&p_dtls_session->context);
        mbedtls_ssl_config_init(&p_dtls_session->conf);

        mbedtls_ssl_cookie_init(&p_dtls_session->cookie_ctx);

#ifdef MBEDTLS_X509_CRT_PARSE_C
        mbedtls_x509_crt_init(&p_dtls_session->cacert);
#endif
        //        mbedtls_ctr_drbg_init(&p_dtls_session->ctr_drbg );
        //        mbedtls_entropy_init( &p_dtls_session->entropy );
        DTLS_INFO("HAL_DTLSSession_init success\r\n");

    }

    return (DTLSContext *)p_dtls_session;
}


unsigned int HAL_DTLSSession_create(DTLSContext *context, coap_dtls_options_t  *p_options)
{
    unsigned int result = DTLS_SUCCESS;
    dtls_session_t *p_dtls_session = (dtls_session_t *)context;

    if (NULL != p_dtls_session) {
        p_dtls_session->network.socket_id = p_options->network.socket_id;
        memcpy(p_dtls_session->network.remote_addr, p_options->network.remote_addr, NETWORK_ADDR_LEN);
        p_dtls_session->network.remote_port = p_options->network.remote_port;
        p_dtls_session->recv_fn = p_options->recv_fn;
        p_dtls_session->send_fn = p_options->send_fn;
        p_dtls_session->recv_timeout_fn = p_options->recv_timeout_fn;

        mbedtls_ssl_config_init(&p_dtls_session->conf);
        //        result = mbedtls_ctr_drbg_seed(&p_dtls_session->ctr_drbg, mbedtls_entropy_func, &p_dtls_session->entropy,
        //                                       (const unsigned char *)"IoTx",
        //                                       strlen("IoTx"));
        //        DTLS_TRC("mbedtls_ctr_drbg_seed result 0x%04x\r\n", result);

        result = mbedtls_ssl_config_defaults(&p_dtls_session->conf,
                                             MBEDTLS_SSL_IS_CLIENT,
                                             MBEDTLS_SSL_TRANSPORT_DATAGRAM,
                                             MBEDTLS_SSL_PRESET_DEFAULT);

        DTLS_TRC("mbedtls_ssl_config_defaults result 0x%04x\r\n", result);

        //        mbedtls_ssl_conf_rng(&p_dtls_session->conf, mbedtls_ctr_drbg_random, &p_dtls_session->ctr_drbg);
        mbedtls_ssl_conf_rng(&p_dtls_session->conf, ssl_random, NULL/*&p_dtls_session->ctr_drbg*/);
        //        mbedtls_ssl_conf_dbg(&p_dtls_session->conf, DTLSLog_wrapper, NULL);
#if defined(MBEDTLS_DEBUG_C)
        mbedtls_ssl_conf_dbg(&p_dtls_session->conf, ssl_debug, NULL);
#endif

        //        result = mbedtls_ssl_cookie_setup(&p_dtls_session->cookie_ctx,
        //                                          mbedtls_ctr_drbg_random, &p_dtls_session->ctr_drbg);
        //        DTLS_TRC("mbedtls_ssl_cookie_setup result 0x%04x\r\n", result);

        //        mbedtls_ssl_conf_dtls_cookies(&p_dtls_session->conf, mbedtls_ssl_cookie_write,
        //                                      mbedtls_ssl_cookie_check, &p_dtls_session->cookie_ctx);


        if (result == DTLS_SUCCESS) {
            result = DTLSVerifyOptions_set(p_dtls_session, p_options->p_ca_cert_pem);

            DTLS_TRC("DTLSVerifyOptions_set result 0x%04x\r\n", result);
        }

#ifdef MBEDTLS_SSL_PROTO_DTLS
        if (result == DTLS_SUCCESS) {
            if (p_dtls_session->conf.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
                mbedtls_ssl_conf_min_version(&p_dtls_session->conf,
                                             MBEDTLS_SSL_MAJOR_VERSION_3,
                                             MBEDTLS_SSL_MINOR_VERSION_3);

                mbedtls_ssl_conf_max_version(&p_dtls_session->conf,
                                             MBEDTLS_SSL_MAJOR_VERSION_3,
                                             MBEDTLS_SSL_MINOR_VERSION_3);

                mbedtls_ssl_conf_handshake_timeout(&p_dtls_session->conf,
                                                   (MBEDTLS_SSL_DTLS_TIMEOUT_DFL_MIN * 2),
                                                   (MBEDTLS_SSL_DTLS_TIMEOUT_DFL_MIN * 2 * 4));
            }
        }
#endif
        if (DTLS_SUCCESS == result) {
            result = DTLSContext_setup(p_dtls_session, p_options);
        }
        if (DTLS_SUCCESS != result) {
            //HAL_DTLSSession_free(p_dtls_session);
        }

        return result;
    } else {
        return DTLS_INVALID_PARAM;
    }
}

unsigned int HAL_DTLSSession_write(DTLSContext *context,
                                   const unsigned char   *p_data,
                                   unsigned int    *p_datalen)
{
    unsigned int err_code = DTLS_SUCCESS;
    dtls_session_t *p_dtls_session = (dtls_session_t *)context;

    if (NULL != p_dtls_session && NULL != p_data && p_datalen != NULL) {
        int actual_len = (*p_datalen);

        actual_len = mbedtls_ssl_write(&p_dtls_session->context, p_data, actual_len);

        if (actual_len < 0) {
            if (actual_len == MBEDTLS_ERR_SSL_CONN_EOF) {
                if (p_dtls_session->context.state < MBEDTLS_SSL_HANDSHAKE_OVER) {
                    err_code = DTLS_HANDSHAKE_IN_PROGRESS;
                }
            }
        } else {
            (* p_datalen) = actual_len;
            err_code      = DTLS_SUCCESS;
        }
    }

    return err_code;
}

unsigned int HAL_DTLSSession_read(DTLSContext *context,
                                  unsigned char   *p_data,
                                  unsigned int    *p_datalen,
                                  unsigned int     timeout)
{
    int len = 0;
    unsigned int err_code = DTLS_READ_DATA_FAILED;
    dtls_session_t *p_dtls_session = (dtls_session_t *)context;

    if (NULL != p_dtls_session && NULL != p_data && p_datalen != NULL) {
        mbedtls_ssl_conf_read_timeout(&(p_dtls_session->conf), timeout);
        len = mbedtls_ssl_read(&p_dtls_session->context, p_data, *p_datalen);

        if (0  <  len) {
            *p_datalen = len;
            err_code = DTLS_SUCCESS;
            DTLS_TRC("mbedtls_ssl_read len %d bytes\r\n", len);
        } else {
            *p_datalen = 0;
            if (MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE == len) {
                err_code = DTLS_FATAL_ALERT_MESSAGE;
                DTLS_INFO("Recv peer fatal alert message\r\n");
            }
            if (MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY == len) {
                err_code = DTLS_PEER_CLOSE_NOTIFY;
                DTLS_INFO("The DTLS session was closed by peer\r\n");
            }
            if (MBEDTLS_ERR_SSL_TIMEOUT == len) {
                err_code = DTLS_SUCCESS;
            }
            DTLS_TRC("mbedtls_ssl_read result(len) (-0x%04x)\r\n", len);
        }
    }
    return err_code;
}
#endif
