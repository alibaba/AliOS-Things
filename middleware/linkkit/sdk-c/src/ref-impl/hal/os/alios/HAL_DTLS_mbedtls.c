/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "iot_import.h"
#ifdef COAP_DTLS_SUPPORT

#include "ali_crypto.h"
#include "mbedtls/config.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/error.h"
#include "mbedtls/timing.h"
#ifdef MBEDTLS_ENTROPY_C
#include "mbedtls/entropy.h"
#endif
#include "mbedtls/ssl_cookie.h"


#define LOG_TAG "HAL_DTLS"
#define platform_trace(format, ...) // LOGW(LOG_TAG, format,##__VA_ARGS__)
#define platform_info(format, ...)  // LOGI(LOG_TAG, format,##__VA_ARGS__)
#define platform_err(format, ...) LOGE(LOG_TAG, format, ##__VA_ARGS__)

static dtls_hooks_t g_dtls_hooks = {HAL_Malloc, HAL_Free};

#if defined(MBEDTLS_DEBUG_C)
extern int  csp_printf(const char *fmt, ...);
static void ssl_debug(void *ctx, int level, const char *file, int line,
                      const char *str)
{
    (void)ctx;
    (void)level;

    csp_printf("%s, line: %d: %s", file ? file : NULL_STR, line,
               str ? str : NULL_STR);

    return;
}
#endif

typedef void DTLSContext;

mbedtls_ssl_session *saved_session = NULL;

typedef struct
{
    mbedtls_ssl_context context;
    mbedtls_ssl_config  conf;
#ifdef MBEDTLS_ENTROPY_C
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_context  entropy;
#endif
#ifdef MBEDTLS_X509_CRT_PARSE_C
    mbedtls_x509_crt cacert;
#endif
    mbedtls_net_context          fd;
    mbedtls_timing_delay_context timer;
    mbedtls_ssl_cookie_ctx       cookie_ctx;
} dtls_session_t;

#define MBEDTLS_MEM_TEST 1

#ifdef MBEDTLS_MEM_TEST

#define MBEDTLS_MEM_INFO_MAGIC 0x12345678

static unsigned int mbedtls_mem_used     = 0;
static unsigned int mbedtls_max_mem_used = 0;

typedef struct
{
    int magic;
    int size;
} mbedtls_mem_info_t;


static int ssl_random(void *prng, unsigned char *output, size_t output_len)
{
    struct timeval tv;

    (void)prng;

    gettimeofday(&tv, NULL);
    ali_seed((uint8_t *)&tv.tv_usec, sizeof(suseconds_t));
    ali_rand_gen(output, output_len);

    return 0;
}

void *_DTLSCalloc_wrapper(size_t n, size_t size)
{
    void *              buf      = NULL;
    mbedtls_mem_info_t *mem_info = NULL;

    if (n == 0 || size == 0) {
        return NULL;
    }

    buf = malloc(n * size + sizeof(mbedtls_mem_info_t));
    if (NULL == buf) {
        return NULL;
    } else {
        memset(buf, 0, n * size + sizeof(mbedtls_mem_info_t));
    }

    mem_info        = (mbedtls_mem_info_t *)buf;
    mem_info->magic = MBEDTLS_MEM_INFO_MAGIC;
    mem_info->size  = n * size;
    buf += sizeof(mbedtls_mem_info_t);

    mbedtls_mem_used += mem_info->size;
    if (mbedtls_mem_used > mbedtls_max_mem_used) {
        mbedtls_max_mem_used = mbedtls_mem_used;
    }

    /* platform_trace("INFO -- mbedtls malloc: %p %d  total used: %d  max used:
       %d\r\n", buf, (int)size, mbedtls_mem_used, mbedtls_max_mem_used); */

    return buf;
}

void _DTLSFree_wrapper(void *ptr)
{
    mbedtls_mem_info_t *mem_info = NULL;
    if (NULL == ptr) {
        return;
    }

    mem_info = ptr - sizeof(mbedtls_mem_info_t);
    if (mem_info->magic != MBEDTLS_MEM_INFO_MAGIC) {
        platform_trace("Warning - invalid mem info magic: 0x%x\r\n",
                       mem_info->magic);
        return;
    }

    mbedtls_mem_used -= mem_info->size;
    /* platform_trace("INFO mbedtls free: %p %d  total used: %d  max used:
       %d\r\n", ptr, mem_info->size, mbedtls_mem_used, mbedtls_max_mem_used);*/

    free(mem_info);
}

#else
static void *_DTLSCalloc_wrapper(size_t n, size_t s)
{
    void * ptr = NULL;
    size_t len = n * s;
    ptr        = aos_malloc(len);
    if (NULL != ptr) {
        memset(ptr, 0x00, len);
    }
    return ptr;
}

static void _DTLSFree_wrapper(void *ptr)
{
    if (NULL != ptr) {
        aos_free(ptr);
        ptr = NULL;
    }
}
#endif

#ifdef DTLS_SESSION_SAVE
static int _DTLSSession_save(const mbedtls_ssl_session *session,
                             unsigned char *buf, size_t buf_len, size_t *olen)
{
    unsigned char *p    = buf;
    size_t         left = buf_len;
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    size_t cert_len;
#endif /* MBEDTLS_X509_CRT_PARSE_C */

    if (left < sizeof(mbedtls_ssl_session)) {
        return (MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL);
    }

    memcpy(p, session, sizeof(mbedtls_ssl_session));
    p += sizeof(mbedtls_ssl_session);
    left -= sizeof(mbedtls_ssl_session);

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if (session->peer_cert == NULL) {
        cert_len = 0;
    } else {
        cert_len = session->peer_cert->raw.len;
    }

    if (left < 3 + cert_len) {
        return (MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL);
    }

    *p++ = (unsigned char)(cert_len >> 16 & 0xFF);
    *p++ = (unsigned char)(cert_len >> 8 & 0xFF);
    *p++ = (unsigned char)(cert_len & 0xFF);

    if (session->peer_cert != NULL) {
        memcpy(p, session->peer_cert->raw.p, cert_len);
    }

    p += cert_len;
#endif /* MBEDTLS_X509_CRT_PARSE_C */

    *olen = p - buf;

    return (0);
}
#endif

static unsigned int _DTLSVerifyOptions_set(dtls_session_t *p_dtls_session,
                                           unsigned char * p_ca_cert_pem)
{
    int          result;
    unsigned int err_code = DTLS_SUCCESS;

#ifdef MBEDTLS_X509_CRT_PARSE_C
    if (p_ca_cert_pem != NULL) {
#ifndef TEST_COAP_DAILY
        mbedtls_ssl_conf_authmode(&p_dtls_session->conf,
                                  MBEDTLS_SSL_VERIFY_REQUIRED);
#else
        mbedtls_ssl_conf_authmode(&p_dtls_session->conf,
                                  MBEDTLS_SSL_VERIFY_OPTIONAL);
#endif
        platform_trace("Call mbedtls_ssl_conf_authmode\r\n");

        platform_trace("x509 ca cert pem len %d\r\n%s\r\n",
                       (int)strlen((char *)p_ca_cert_pem) + 1, p_ca_cert_pem);
        result =
          mbedtls_x509_crt_parse(&p_dtls_session->cacert, p_ca_cert_pem,
                                 strlen((const char *)p_ca_cert_pem) + 1);

        platform_trace("mbedtls_x509_crt_parse result 0x%04x\r\n", result);
        if (0 != result) {
            err_code = DTLS_INVALID_CA_CERTIFICATE;
        } else {
            mbedtls_ssl_conf_ca_chain(&p_dtls_session->conf,
                                      &p_dtls_session->cacert, NULL);
        }
    } else
#endif
    {
        mbedtls_ssl_conf_authmode(&p_dtls_session->conf,
                                  MBEDTLS_SSL_VERIFY_NONE);
    }

    return err_code;
}

static unsigned int _DTLSContext_setup(dtls_session_t *     p_dtls_session,
                                       coap_dtls_options_t *p_options)
{
    int result = 0;

    mbedtls_ssl_init(&p_dtls_session->context);

    result = mbedtls_ssl_setup(&p_dtls_session->context, &p_dtls_session->conf);
    platform_trace("mbedtls_ssl_setup result 0x%04x\r\n", result);

    if (result == 0) {
        if (p_dtls_session->conf.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
            mbedtls_ssl_set_timer_cb(
              &p_dtls_session->context, (void *)&p_dtls_session->timer,
              mbedtls_timing_set_delay, mbedtls_timing_get_delay);
        }

#ifdef MBEDTLS_X509_CRT_PARSE_C
        platform_trace("mbedtls_ssl_set_hostname %s\r\n", p_options->p_host);
        mbedtls_ssl_set_hostname(&p_dtls_session->context, p_options->p_host);
#endif
        mbedtls_ssl_set_bio(&p_dtls_session->context,
                            (void *)&p_dtls_session->fd, mbedtls_net_send,
                            mbedtls_net_recv, mbedtls_net_recv_timeout);
        platform_trace("mbedtls_ssl_set_bio result 0x%04x\r\n", result);

        if (NULL != saved_session) {
            mbedtls_ssl_set_session(&p_dtls_session->context, saved_session);
        }

        do {
            result = mbedtls_ssl_handshake(&p_dtls_session->context);
        } while (result == MBEDTLS_ERR_SSL_WANT_READ ||
                 result == MBEDTLS_ERR_SSL_WANT_WRITE);
        platform_trace("mbedtls_ssl_handshake result 0x%04x\r\n", result);
#ifdef MBEDTLS_MEM_TEST
        platform_trace(
          "mbedtls handshake memory total used: %d  max used: %d\r\n",
          mbedtls_mem_used, mbedtls_max_mem_used);
#endif
        if (0 == result) {
            if (NULL == saved_session) {
                saved_session = aos_malloc(sizeof(mbedtls_ssl_session));
            }
            if (NULL != saved_session) {
                memset(saved_session, 0x00, sizeof(mbedtls_ssl_session));
                result = mbedtls_ssl_get_session(&p_dtls_session->context,
                                                 saved_session);
                platform_trace(
                  "mbedtls_ssl_get_session_session return 0x%04x\r\n", result);
            }
        }
    }

    return (result ? DTLS_HANDSHAKE_FAILED : DTLS_SUCCESS);
}

dtls_session_t *_DTLSSession_init()
{
    dtls_session_t *p_dtls_session = NULL;
    p_dtls_session                 = aos_malloc(sizeof(dtls_session_t));

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(0);
#endif

#ifdef MBEDTLS_MEM_TEST
    mbedtls_mem_used     = 0;
    mbedtls_max_mem_used = 0;
#endif
    // mbedtls_platform_set_calloc_free(_DTLSCalloc_wrapper, _DTLSFree_wrapper);
    if (NULL != p_dtls_session) {
        memset(p_dtls_session, 0x00, sizeof(dtls_session_t));
        // mbedtls_net_init(&p_dtls_session->fd);
        mbedtls_ssl_init(&p_dtls_session->context);
        mbedtls_ssl_config_init(&p_dtls_session->conf);
        mbedtls_net_init(&p_dtls_session->fd);

        mbedtls_ssl_cookie_init(&p_dtls_session->cookie_ctx);

#ifdef MBEDTLS_X509_CRT_PARSE_C
        mbedtls_x509_crt_init(&p_dtls_session->cacert);
#endif
#ifdef MBEDTLS_ENTROPY_C
        mbedtls_ctr_drbg_init(&p_dtls_session->ctr_drbg);
        mbedtls_entropy_init(&p_dtls_session->entropy);
#endif
        platform_info("HAL_DTLSSession_init success\r\n");
    }

    return p_dtls_session;
}

unsigned int _DTLSSession_deinit(dtls_session_t *p_dtls_session)
{
    int ret;
    if (p_dtls_session != NULL) {
        do {
            ret = mbedtls_ssl_close_notify(&p_dtls_session->context);
        } while (ret == MBEDTLS_ERR_SSL_WANT_WRITE);

        mbedtls_net_free(&p_dtls_session->fd);
#ifdef MBEDTLS_X509_CRT_PARSE_C
        mbedtls_x509_crt_free(&p_dtls_session->cacert);
#endif
        mbedtls_ssl_cookie_free(&p_dtls_session->cookie_ctx);

        mbedtls_ssl_config_free(&p_dtls_session->conf);
        mbedtls_ssl_free(&p_dtls_session->context);

#ifdef MBEDTLS_ENTROPY_C
        mbedtls_ctr_drbg_free(&p_dtls_session->ctr_drbg);
        mbedtls_entropy_free(&p_dtls_session->entropy);
#endif
        aos_free(p_dtls_session);
    }

    return DTLS_SUCCESS;
}

DLL_HAL_API int HAL_DTLSHooks_set(dtls_hooks_t *hooks)
{
    if (hooks == NULL || hooks->malloc == NULL || hooks->free == NULL) {
        return DTLS_INVALID_PARAM;
    }

    g_dtls_hooks.malloc = hooks->malloc;
    g_dtls_hooks.free = hooks->free;

    return DTLS_SUCCESS;
}

DTLSContext *HAL_DTLSSession_create(coap_dtls_options_t *p_options)
{
    char            port[6]        = { 0 };
    int             result         = 0;
    dtls_session_t *p_dtls_session = NULL;

    p_dtls_session = _DTLSSession_init();
    if (NULL != p_dtls_session) {
        mbedtls_ssl_config_init(&p_dtls_session->conf);

#ifdef MBEDTLS_ENTROPY_C
        result =
          mbedtls_ctr_drbg_seed(&p_dtls_session->ctr_drbg, mbedtls_entropy_func,
                                &p_dtls_session->entropy,
                                (const unsigned char *)"IoTx", strlen("IoTx"));
        if (0 != result) {
            platform_err("mbedtls_ctr_drbg_seed result 0x%04x\r\n", result);
            goto error;
        }
#endif
        result = mbedtls_ssl_config_defaults(
          &p_dtls_session->conf, MBEDTLS_SSL_IS_CLIENT,
          MBEDTLS_SSL_TRANSPORT_DATAGRAM, MBEDTLS_SSL_PRESET_DEFAULT);
        if (0 != result) {
            platform_err("mbedtls_ssl_config_defaults result 0x%04x\r\n",
                         result);
            goto error;
        }
        mbedtls_ssl_conf_rng(&p_dtls_session->conf, ssl_random, NULL);
#if defined(MBEDTLS_DEBUG_C)
        mbedtls_ssl_conf_dbg(&p_dtls_session->conf, ssl_debug, NULL);
#endif
        // mbedtls_ssl_conf_dbg(&p_dtls_session->conf, _DTLSLog_wrapper, NULL);

#ifdef MBEDTLS_ENTROPY_C
        result = mbedtls_ssl_cookie_setup(&p_dtls_session->cookie_ctx,
                                          mbedtls_ctr_drbg_random,
                                          &p_dtls_session->ctr_drbg);
        if (0 != result) {
            platform_err("mbedtls_ssl_cookie_setup result 0x%04x\r\n", result);
            goto error;
        }
#endif
#if defined(MBEDTLS_SSL_DTLS_HELLO_VERIFY) && defined(MBEDTLS_SSL_SRV_C)
        mbedtls_ssl_conf_dtls_cookies(
          &p_dtls_session->conf, mbedtls_ssl_cookie_write,
          mbedtls_ssl_cookie_check, &p_dtls_session->cookie_ctx);
#endif

        result =
          _DTLSVerifyOptions_set(p_dtls_session, p_options->p_ca_cert_pem);

        if (DTLS_SUCCESS != result) {
            platform_err("DTLSVerifyOptions_set result 0x%04x\r\n", result);
            goto error;
        }
        sprintf(port, "%u", p_options->port);
        result = mbedtls_net_connect(&p_dtls_session->fd, p_options->p_host,
                                     port, MBEDTLS_NET_PROTO_UDP);
        if (0 != result) {
            platform_err("mbedtls_net_connect result 0x%04x\r\n", result);
            goto error;
        }

#ifdef MBEDTLS_SSL_PROTO_DTLS
        if (p_dtls_session->conf.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
            mbedtls_ssl_conf_min_version(&p_dtls_session->conf,
                                         MBEDTLS_SSL_MAJOR_VERSION_3,
                                         MBEDTLS_SSL_MINOR_VERSION_3);

            mbedtls_ssl_conf_max_version(&p_dtls_session->conf,
                                         MBEDTLS_SSL_MAJOR_VERSION_3,
                                         MBEDTLS_SSL_MINOR_VERSION_3);

            mbedtls_ssl_conf_handshake_timeout(
              &p_dtls_session->conf, (MBEDTLS_SSL_DTLS_TIMEOUT_DFL_MIN * 2),
              (MBEDTLS_SSL_DTLS_TIMEOUT_DFL_MIN * 2 * 4));
        }
#endif
        result = _DTLSContext_setup(p_dtls_session, p_options);
        if (DTLS_SUCCESS != result) {
            platform_err("DTLSVerifyOptions_set result 0x%04x\r\n", result);
            goto error;
        }

        return (DTLSContext *)p_dtls_session;
    }

error:
    if (NULL != p_dtls_session) {
        _DTLSSession_deinit(p_dtls_session);
    }
    return NULL;
}

unsigned int HAL_DTLSSession_write(DTLSContext *        context,
                                   const unsigned char *p_data,
                                   unsigned int *       p_datalen)
{
    int             len            = 0;
    unsigned int    err_code       = DTLS_SUCCESS;
    dtls_session_t *p_dtls_session = (dtls_session_t *)context;

    if (NULL != p_dtls_session && NULL != p_data && p_datalen != NULL) {
        len = (*p_datalen);
        len = mbedtls_ssl_write(&p_dtls_session->context, p_data, len);

        if (len < 0) {
            if (len == MBEDTLS_ERR_SSL_CONN_EOF) {
                if (p_dtls_session->context.state <
                    MBEDTLS_SSL_HANDSHAKE_OVER) {
                    err_code = DTLS_HANDSHAKE_IN_PROGRESS;
                }
            }
        } else {
            (*p_datalen) = len;
            err_code     = DTLS_SUCCESS;
        }
    }

    return err_code;
}

unsigned int HAL_DTLSSession_read(DTLSContext *context, unsigned char *p_data,
                                  unsigned int *p_datalen, unsigned int timeout)
{
    int             len            = 0;
    unsigned int    err_code       = DTLS_READ_DATA_FAILED;
    dtls_session_t *p_dtls_session = (dtls_session_t *)context;

    if (NULL != p_dtls_session && NULL != p_data && p_datalen != NULL) {
        mbedtls_ssl_conf_read_timeout(&(p_dtls_session->conf), timeout);
        len = mbedtls_ssl_read(&p_dtls_session->context, p_data, *p_datalen);

        if (0 < len) {
            *p_datalen = len;
            err_code   = DTLS_SUCCESS;
            platform_trace("mbedtls_ssl_read len %d bytes\r\n", len);
        } else {
            *p_datalen = 0;
            if (MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE == len) {
                err_code = DTLS_FATAL_ALERT_MESSAGE;
                platform_err("Recv peer fatal alert message\r\n");
            } else if (MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY == len) {
                err_code = DTLS_PEER_CLOSE_NOTIFY;
                platform_err("The DTLS session was closed by peer\r\n");
            } else if (MBEDTLS_ERR_SSL_TIMEOUT == len) {
                err_code = DTLS_SUCCESS;
                platform_err("DTLS recv timeout\r\n");
            }
            // else {
            //     platform_trace("mbedtls_ssl_read error result (-0x%04x)\r\n",
            //     len);
            // }
        }
    }
    return err_code;
}

unsigned int HAL_DTLSSession_free(DTLSContext *context)
{
    dtls_session_t *p_dtls_session = NULL;
    if (NULL != context) {
        p_dtls_session = (dtls_session_t *)context;
        return _DTLSSession_deinit(p_dtls_session);
    }

    return DTLS_INVALID_PARAM;
}


#endif
