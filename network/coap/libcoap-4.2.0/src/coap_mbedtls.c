/*
 * coap_mbedtls.c -- Datagram Transport Layer Support for libcoap
 *
 * Copyright (C) 2016 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */
#include "coap_config.h"

#ifdef HAVE_MBEDTLS

#include <errno.h>

#include "coap_net.h"
#include "coap_mem.h"
#include "coap_debug.h"

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/error.h"
#include "mbedtls/timing.h"
#ifdef MBEDTLS_ENTROPY_C
#include "mbedtls/entropy.h"
#endif
#include "mbedtls/ssl_cookie.h"

mbedtls_ssl_session *saved_session = NULL;

typedef struct coap_mbedtls_st {
  coap_session_t *session;
  const void *pdu;
  unsigned pdu_len;
  unsigned peekmode;
  coap_tick_t timeout;
} coap_mbedtls_data;

typedef struct coap_mbedtls_context_t {
    char *root_ca_file;
    char *root_ca_path;
} coap_mbedtls_context_t;

typedef struct coap_mbedtls_session_t {
    mbedtls_ssl_context          context;
    mbedtls_ssl_config           config;
#ifdef MBEDTLS_ENTROPY_C
    mbedtls_ctr_drbg_context     ctr_drbg;
    mbedtls_entropy_context      entropy;
#endif
#ifdef MBEDTLS_X509_CRT_PARSE_C
    mbedtls_x509_crt             cacert;
#endif
    mbedtls_net_context          fd;
    mbedtls_timing_delay_context timer;
    mbedtls_ssl_cookie_ctx       cookie_ctx;
    coap_mbedtls_data            coap_ssl_data;
    int                          established;
} coap_mbedtls_session_t;

typedef struct {
    unsigned char             *p_ca_cert_pem;
    char                      *p_host;
    unsigned short             port;
} coap_dtls_options_t;

#define DTLS_ERROR_BASE                (1<<24)
#define DTLS_SUCCESS                   (0)
#define DTLS_INVALID_PARAM             (DTLS_ERROR_BASE | 1)
#define DTLS_INVALID_CA_CERTIFICATE    (DTLS_ERROR_BASE | 2)
#define DTLS_HANDSHAKE_IN_PROGRESS     (DTLS_ERROR_BASE | 3)
#define DTLS_HANDSHAKE_FAILED          (DTLS_ERROR_BASE | 4)
#define DTLS_FATAL_ALERT_MESSAGE       (DTLS_ERROR_BASE | 5)
#define DTLS_PEER_CLOSE_NOTIFY         (DTLS_ERROR_BASE | 6)
#define DTLS_SESSION_CREATE_FAILED     (DTLS_ERROR_BASE | 7)
#define DTLS_READ_DATA_FAILED          (DTLS_ERROR_BASE | 8)

static int ssl_random(void *prng, unsigned char *output, size_t output_len)
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

int
coap_dtls_is_supported(void) {
  return 1;
}

int
coap_tls_is_supported(void) {
  return 0;
}

coap_tls_version_t *
coap_get_tls_library_version(void) {
  static coap_tls_version_t version;
  version.version = 0;
  version.type = COAP_TLS_LIBRARY_MBEDTLS;
  return &version;
}

int
coap_dtls_context_set_pki(struct coap_context_t *coap_context,
                          coap_dtls_pki_t *setup_data,
                          coap_dtls_role_t role)
{
    return 0;
}

int
coap_dtls_context_set_pki_root_cas(struct coap_context_t *ctx,
                                   const char *ca_file,
                                   const char *ca_path
) {
  return 0;
}

int
coap_dtls_context_set_psk(struct coap_context_t *coap_context,
                          const char *identity_hint,
                          coap_dtls_role_t role)
{
  return 0;
}

int
coap_dtls_context_check_keys_enabled(coap_context_t *ctx)
{
  return 0;
}

static int dtls_log_level = 0;

void coap_dtls_startup(void) {
}

void
coap_dtls_set_log_level(int level) {
  dtls_log_level = level;
}

int
coap_dtls_get_log_level(void) {
  return dtls_log_level;
}

static void coap_dtls_log(void  *p_ctx, int level,
                          const char *p_file, int line,
                          const char *p_str)
{
    coap_log(LOG_INFO,"[mbedTLS]:[%s]:[%d]: %s\r\n", p_file, line, p_str);
}

static int coap_dtls_verify_options_set(coap_mbedtls_session_t *p_session,
                                        unsigned char *p_ca_cert_pem, char *host)
{
    int ret;
    int err_code = 0;

#ifdef MBEDTLS_X509_CRT_PARSE_C
    if (p_ca_cert_pem != NULL) {
        mbedtls_ssl_conf_authmode(&p_session->config, MBEDTLS_SSL_VERIFY_REQUIRED);
        if (strstr(host, "pre.iot-as-coap")) {
            coap_log(LOG_INFO, "host = '%s' so verify server OPTIONAL\r\n", host);
            mbedtls_ssl_conf_authmode(&p_session->config, MBEDTLS_SSL_VERIFY_OPTIONAL);
        }
        coap_log(LOG_INFO, "Call mbedtls_ssl_conf_authmode\r\n");

        coap_log(LOG_INFO, "x509 ca cert pem len %d\r\n%s\r\n", (int)strlen((char *)p_ca_cert_pem) + 1, p_ca_cert_pem);
        ret = mbedtls_x509_crt_parse(&p_session->cacert,
                                      p_ca_cert_pem,
                                      strlen((const char *)p_ca_cert_pem) + 1);

        coap_log(LOG_INFO, "mbedtls_x509_crt_parse result 0x%d\r\n", ret);
        if (0 != ret) {
            err_code = -1;
        } else {
            mbedtls_ssl_conf_ca_chain(&p_session->config, &p_session->cacert, NULL);
        }
    } else
#endif
    {
        mbedtls_ssl_conf_authmode(&p_session->config, MBEDTLS_SSL_VERIFY_NONE);
    }

    return err_code;
}

static int coap_dtls_session_config(coap_mbedtls_session_t *p_session)
{
    int ret;

    if (NULL == p_session) {
        coap_log(LOG_ERR, "Invalid input para");
        return -1;
    }

#ifdef MBEDTLS_ENTROPY_C
    ret = mbedtls_ctr_drbg_seed(&p_session->ctr_drbg,
                                mbedtls_entropy_func,
                                &p_session->entropy,
                                "IoTx",
                                strlen("IoTx"));
    if (0 != ret) {
        coap_log(LOG_ERR, "mbedtls_ctr_drbg_seed failed ret %d\n", ret);
        return -1;
    }
#endif

    ret = mbedtls_ssl_config_defaults(&p_session->config,
                                       MBEDTLS_SSL_IS_CLIENT,
                                       MBEDTLS_SSL_TRANSPORT_DATAGRAM,
                                       MBEDTLS_SSL_PRESET_DEFAULT);
    if (0 != ret) {
        coap_log(LOG_ERR, "mbedtls_ssl_config_defaults failed ret %d\n", ret);
        return -1;
    }

    mbedtls_ssl_conf_rng(&p_session->config, ssl_random, NULL);

#ifdef MBEDTLS_ENTROPY_C
#if 0
    ret = mbedtls_ssl_cookie_setup(&p_session->cookie_ctx, mbedtls_ctr_drbg_random,
                                   &p_session->ctr_drbg);
    if (0 != ret) {
        coap_log(LOG_ERR, "mbedtls_ssl_cookie_setup result %d\n", ret);
        return -1;
    }
#endif
#endif

#if defined(MBEDTLS_SSL_DTLS_HELLO_VERIFY) && defined(MBEDTLS_SSL_SRV_C)
    mbedtls_ssl_conf_dtls_cookies(&p_session->config, mbedtls_ssl_cookie_write,
                                  mbedtls_ssl_cookie_check, &p_session->cookie_ctx);
#endif

    return 0;
}

static ssize_t
coap_dgram_read(void *context, unsigned char *buf, size_t len)
{
    ssize_t ret = 0;
    coap_session_t *c_session = (struct coap_session_t *)context;
    coap_mbedtls_data *data = NULL;

    if (!c_session->tls) {
        errno = EAGAIN;
        return -1;
    }

    data = &((coap_mbedtls_session_t *)c_session->tls)->coap_ssl_data;
    if (buf != NULL) {
        if (data != NULL && data->pdu_len > 0) {
            if (len < data->pdu_len) {
                memcpy(buf, data->pdu, len);
                ret = len;
                data->pdu += len;
                data->pdu_len -= len;
        } else {
            memcpy(buf, data->pdu, data->pdu_len);
            ret = data->pdu_len;
            if (!data->peekmode) {
                data->pdu_len = 0;
                data->pdu = NULL;
            }
        }
      } else {
          errno = EAGAIN;
          ret = -1;
      }
    }

    return ret;
}

static ssize_t
coap_dgram_write(void *context, const unsigned char *buf, size_t len) {
  ssize_t ret = -1;
  coap_session_t *c_session = (struct coap_session_t *)context;

  if (c_session) {
    ret = coap_session_send(c_session, buf, len);
    if (ret != (int)len) {
        coap_log(LOG_WARNING, "coap_network_send failed\n");
        ret = 0;
    }
  } else {
    ret = 0;
  }
  return ret;
}

static int
coap_dgram_read_timeout(void *context, unsigned char *buf, size_t len,
                        uint32_t timeout) 
{
    coap_session_t *c_session = (struct coap_session_t *)context;

    if (c_session) {
        fd_set readfds, writefds, exceptfds;
        struct timeval tv;
        int nfds = c_session->sock.fd +1;

        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);
        FD_SET (c_session->sock.fd, &readfds);
        FD_SET (c_session->sock.fd, &writefds);
        FD_SET (c_session->sock.fd, &exceptfds);
        /* Polling */    
        tv.tv_sec  = timeout / 1000;
        tv.tv_usec = ( timeout % 1000 ) * 1000;

        return select(nfds, &readfds, &writefds, &exceptfds, &tv);
     }

    return 1;
}

static int coap_dtls_session_setup(coap_session_t *c_session,
                                   coap_mbedtls_session_t *p_session,
                                   coap_dtls_options_t *p_options)
{
    int ret = 0;

    if (NULL == c_session || NULL == p_session || NULL == p_options) {
        return -1;
    }

    ret = mbedtls_ssl_setup(&p_session->context, &p_session->config);
    coap_log(LOG_INFO, "mbedtls_ssl_setup result 0x%04x\r\n", ret);

    if (ret == 0) {
        if (p_session->config.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
            mbedtls_ssl_set_timer_cb(
              &p_session->context, (void *)&p_session->timer,
              mbedtls_timing_set_delay, mbedtls_timing_get_delay);
        }

#ifdef MBEDTLS_X509_CRT_PARSE_C
        coap_log(LOG_INFO, "mbedtls_ssl_set_hostname %s\r\n", p_options->p_host);
        mbedtls_ssl_set_hostname(&p_session->context, p_options->p_host);
#endif
        mbedtls_ssl_set_bio(&p_session->context,
                            (void *)c_session, coap_dgram_write,
                            coap_dgram_read, NULL);

        if (NULL != saved_session) {
            mbedtls_ssl_set_session(&p_session->context, saved_session);
        }

        ret = mbedtls_ssl_handshake(&p_session->context);
        coap_log(LOG_INFO, "mbedtls_ssl_handshake result 0x%04x\r\n", ret);
        if (0 == ret) {
            if (NULL == saved_session) {
                saved_session = coap_malloc(sizeof(mbedtls_ssl_session));
            }
            if (NULL != saved_session) {
                memset(saved_session, 0x00, sizeof(mbedtls_ssl_session));
                ret = mbedtls_ssl_get_session(&p_session->context,
                                                 saved_session);
                coap_log(LOG_INFO,
                  "mbedtls_ssl_get_session_session return 0x%04x\r\n", ret);
            }
        }
    }

    return ret;
}

void *coap_dtls_new_context(struct coap_context_t *coap_context) {
    coap_mbedtls_context_t  *ctx = NULL;
    (void)coap_context;
    
    ctx = (coap_mbedtls_context_t *) coap_malloc(sizeof(coap_mbedtls_context_t));
    if (NULL == ctx) {
        coap_log(LOG_ERR, "Fail to alloc mbedtls context\n");
    }

    return ctx;
}

void coap_dtls_free_context(void *handle) {
    coap_mbedtls_context_t  *ctx = (coap_mbedtls_context_t *)handle;
    if (ctx) {
        coap_free(ctx);
    }
}

static coap_mbedtls_session_t *coap_dtls_session_init()
{
    coap_mbedtls_session_t *p_session = NULL;
    p_session = coap_malloc(sizeof(coap_mbedtls_session_t));

    if (NULL != p_session) {
        memset(p_session, 0x00, sizeof(coap_mbedtls_session_t));
        mbedtls_net_init(&p_session->fd);
        mbedtls_ssl_init(&p_session->context);
        mbedtls_ssl_config_init(&p_session->config);
        mbedtls_ssl_cookie_init(&p_session->cookie_ctx);
#ifdef MBEDTLS_X509_CRT_PARSE_C
        mbedtls_x509_crt_init(&p_session->cacert);
#endif
#ifdef MBEDTLS_ENTROPY_C
        mbedtls_ctr_drbg_init(&p_session->ctr_drbg);
        mbedtls_entropy_init(&p_session->entropy);
#endif
    }

    return p_session;
}

static void coap_dtls_session_deinit(coap_mbedtls_session_t *p_session)
{
    int ret;
    
    if (p_session != NULL) {
         do {
            ret = mbedtls_ssl_close_notify(&p_session->context);
        } while (ret == MBEDTLS_ERR_SSL_WANT_WRITE);

        mbedtls_net_free(&p_session->fd);
#ifdef MBEDTLS_X509_CRT_PARSE_C
        mbedtls_x509_crt_free(&p_session->cacert);
#endif
        mbedtls_ssl_cookie_free(&p_session->cookie_ctx);
        mbedtls_ssl_config_free(&p_session->config);
        mbedtls_ssl_free(&p_session->context);
#ifdef MBEDTLS_ENTROPY_C
        mbedtls_ctr_drbg_free(&p_session->ctr_drbg);
        mbedtls_entropy_free(&p_session->entropy);
#endif

        coap_free(p_session);
    }
}

void *coap_dtls_new_server_session(coap_session_t *session) {
  return NULL;
}

void *coap_dtls_new_client_session(coap_session_t *c_session) {
    int ret;
    coap_mbedtls_session_t *p_session = NULL;
    //FIXME
    coap_dtls_options_t  options = {NULL, "", 5684 };
    coap_dtls_options_t  *p_options = &options; 

    p_session = coap_dtls_session_init();
    if (NULL == p_session) {
        coap_log(LOG_ERR, "fail to init dtls session\n");
        return NULL;
    }

    ret = coap_dtls_session_config(p_session);
    if (0 != ret) {
        coap_log(LOG_ERR, "fail to config dtls session\n");
        goto err;
    }

    ret = coap_dtls_verify_options_set(p_session, p_options->p_ca_cert_pem, p_options->p_host);
    if (0 != ret) {
        coap_log(LOG_ERR, "dtls_verify_options result %d\r\n", ret);
        goto err;
    }

#ifdef MBEDTLS_SSL_PROTO_DTLS
    if (p_session->config.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
            mbedtls_ssl_conf_min_version(&p_session->config,
                                         MBEDTLS_SSL_MAJOR_VERSION_3,
                                         MBEDTLS_SSL_MINOR_VERSION_3);

            mbedtls_ssl_conf_max_version(&p_session->config,
                                         MBEDTLS_SSL_MAJOR_VERSION_3,
                                         MBEDTLS_SSL_MINOR_VERSION_3);

            mbedtls_ssl_conf_handshake_timeout(&p_session->config,
                                               (MBEDTLS_SSL_DTLS_TIMEOUT_DFL_MIN * 2),
                                               (MBEDTLS_SSL_DTLS_TIMEOUT_DFL_MIN * 2 * 4));
    }
#endif
    ret = coap_dtls_session_setup(c_session, p_session, p_options);
    if (0 == ret) {
        p_session->established = 1;
    }
    
    return p_session;
err:
    coap_dtls_session_deinit(p_session);
    return NULL;
}

void coap_dtls_free_session(coap_session_t *coap_session) {
    coap_mbedtls_session_t *p_session = (coap_mbedtls_session_t *)coap_session->tls;

    coap_dtls_session_deinit(p_session);

    coap_session->tls = NULL;
}

void coap_dtls_session_update_mtu(coap_session_t *session) {
}

int coap_dtls_send(coap_session_t *c_session,
                   const uint8_t *data, size_t data_len) {
    int ret = 0;
    coap_mbedtls_session_t *p_session = NULL;

    if (NULL == c_session || NULL == data || data_len == 0) {
        return -1;
    }
    
    p_session = (coap_mbedtls_session_t *)c_session->tls;
    if (NULL == p_session) {
        return -1;
    }

    c_session->dtls_event = -1;
    if (p_session->established) {
        ret = mbedtls_ssl_write(&p_session->context, data, data_len);

    if (ret <= 0) {
      switch (ret) {
       /* TODO: Add EGAIN handle */
      default:
        ret = -1;
        break;
      }
      if (ret == -1) {
        coap_log(LOG_WARNING, "coap_dtls_send: cannot send PDU\n");
      }
    }
  } else {
    ret = mbedtls_ssl_handshake(&p_session->context);
    if (ret == 0) {
      p_session->established = 1;
      /* Just connected, so send the data */
      return coap_dtls_send(c_session, data, data_len);
    }
    ret = -1;
  }

  if (c_session->dtls_event >= 0) {
      coap_handle_event(c_session->context, c_session->dtls_event, c_session);
      if (c_session->dtls_event == COAP_EVENT_DTLS_ERROR ||
          c_session->dtls_event == COAP_EVENT_DTLS_CLOSED) {
      coap_session_disconnected(c_session, COAP_NACK_TLS_FAILED);
      ret = -1;
    }
  }

  return ret;
}

int coap_dtls_is_context_timeout(void) {
  return 1;
}

coap_tick_t coap_dtls_get_context_timeout(void *dtls_context ) {
  return 0;
}

coap_tick_t coap_dtls_get_timeout(coap_session_t *session) {
  return 0;
}

void coap_dtls_handle_timeout(coap_session_t *session ) {
}

static uint8_t recv_pdu[COAP_RXBUFFER_SIZE];
int coap_dtls_receive(coap_session_t *c_session,
                      const uint8_t *data,
                      size_t data_len)
{
    int ret = 0;

    coap_mbedtls_session_t *p_session = NULL;
    coap_mbedtls_data *ssl_data = NULL;

   if (NULL == c_session || NULL == data || data_len == 0) {
       return -1;
   }
    
    p_session = (coap_mbedtls_session_t *)c_session->tls;
    if (NULL == p_session) {
        return -1;
    }

    ssl_data = &p_session->coap_ssl_data;
    if (NULL == ssl_data) {
        return -1;
    }
 
    if (ssl_data->pdu_len)
        coap_log(LOG_INFO, "** %s: Previous data not read %u bytes\n",
                 coap_session_str(c_session), ssl_data->pdu_len);

    ssl_data->pdu = data;
    ssl_data->pdu_len = (unsigned)data_len;

    c_session->dtls_event = -1;
    if (p_session->established) {
       if (c_session->state == COAP_SESSION_STATE_HANDSHAKE) {
        coap_handle_event(c_session->context, COAP_EVENT_DTLS_CONNECTED,
                          c_session);
        coap_session_connected(c_session);
    }
    ret = mbedtls_ssl_read(&p_session->context, recv_pdu, sizeof(recv_pdu));
    if (ret > 0) {
      return coap_handle_dgram(c_session->context, c_session, recv_pdu, (size_t)ret);
    }
    else if (ret == 0) {
      c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
    }
    else {
      coap_log(LOG_WARNING,
               "coap_dtls_receive: gnutls_record_recv returned %d\n", ret);
      ret = -1;
    }
  }
  else {
        ret = mbedtls_ssl_handshake(&p_session->context);
    if (ret == 0) {
        p_session->established = 1;
        coap_session_connected(c_session);
    }
    else {
        ret = -1;
    }
  }

  if (c_session->dtls_event >= 0) {
      coap_handle_event(c_session->context, c_session->dtls_event, c_session);
      if (c_session->dtls_event == COAP_EVENT_DTLS_ERROR ||
        c_session->dtls_event == COAP_EVENT_DTLS_CLOSED) {
        coap_session_disconnected(c_session, COAP_NACK_TLS_FAILED);
        ret = -1;
      }
  }

  return ret;
}

// for dtls server
int
coap_dtls_hello(coap_session_t *session,
  const uint8_t *data,
  size_t data_len
) {
  return 0;
}

unsigned int coap_dtls_get_overhead(coap_session_t *session) {
  return 0;
}


void *coap_tls_new_client_session(coap_session_t *session, int *connected) {
  return NULL;
}

void *coap_tls_new_server_session(coap_session_t *session, int *connected) {
  return NULL;
}

void coap_tls_free_session(coap_session_t *coap_session) {
 
}

ssize_t coap_tls_write(coap_session_t *session,
                       const uint8_t *data,
                       size_t data_len)
{
    return -1;
}

ssize_t coap_tls_read(coap_session_t *session,
                      uint8_t *data,
                      size_t data_len) 
{
  return -1;
}

#endif /* HAVE_MBEDDTLS */
