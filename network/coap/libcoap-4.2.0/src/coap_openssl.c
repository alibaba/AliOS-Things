/*
* coap_openssl.c -- Datagram Transport Layer Support for libcoap with openssl
*
* Copyright (C) 2017 Jean-Claude Michelou <jcm@spinetix.com>
* Copyright (C) 2018 Jon Shallow <supjps-libcoap@jpshallow.com>
*
* This file is part of the CoAP library libcoap. Please see README for terms
* of use.
*/

#include "coap_config.h"

#ifdef HAVE_OPENSSL

/*
 * OpenSSL 1.1.0 has support for making decisions during receipt of
 * the Client Hello - the call back function is set up using
 * SSL_CTX_set_tlsext_servername_callback() which is called later in the
 * Client Hello processing - but called every Client Hello.
 * Certificates and Preshared Keys have to be set up in the SSL CTX before
 * SSL_Accept() is called, making the code messy to decide whether this is a
 * PKI or PSK incoming request to handle things accordingly if both are
 * defined.  SNI has to create a new SSL CTX to handle different server names
 * with different crtificates.
 *
 * OpenSSL 1.1.1 introduces a new function SSL_CTX_set_client_hello_cb().
 * The call back is invoked early on in the Client Hello processing giving
 * the ability to easily use different Preshared Keys, Certificates etc.
 * Certificates do not have to be set up in the SSL CTX before SSL_Accept is
 * called.
 * Later in the Client Hello code, the callback for
 * SSL_CTX_set_tlsext_servername_callback() is still called, but only if SNI
 * is being used by the client, so cannot be used for doing things the
 * OpenSSL 1.1.0 way.
 *
 * OpenSSL 1.1.1 supports TLS1.3.
 *
 * Consequently, this code has to have compile time options to include /
 * exclude code based on whether compiled against 1.1.0 or 1.1.1, as well as
 * have additional run time checks.
 *
 */
#include "net.h"
#include "mem.h"
#include "coap_debug.h"
#include "prng.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>
#include <openssl/x509v3.h>

#if OPENSSL_VERSION_NUMBER < 0x10100000L
#error Must be compiled against OpenSSL 1.1.0 or later
#endif

#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif /* __GNUC__ */

/* RFC6091/RFC7250 */
#ifndef TLSEXT_TYPE_client_certificate_type
#define TLSEXT_TYPE_client_certificate_type 19
#endif
#ifndef TLSEXT_TYPE_server_certificate_type
#define TLSEXT_TYPE_server_certificate_type 20
#endif

/* This structure encapsulates the OpenSSL context object. */
typedef struct coap_dtls_context_t {
  SSL_CTX *ctx;
  SSL *ssl;        /* OpenSSL object for listening to connection requests */
  HMAC_CTX *cookie_hmac;
  BIO_METHOD *meth;
  BIO_ADDR *bio_addr;
} coap_dtls_context_t;

typedef struct coap_tls_context_t {
  SSL_CTX *ctx;
  BIO_METHOD *meth;
} coap_tls_context_t;

#define IS_PSK 0x1
#define IS_PKI 0x2

typedef struct sni_entry {
  char *sni;
#if OPENSSL_VERSION_NUMBER < 0x10101000L
  SSL_CTX *ctx;
#else /* OPENSSL_VERSION_NUMBER >= 0x10101000L */
  coap_dtls_key_t pki_key;
#endif /* OPENSSL_VERSION_NUMBER >= 0x10101000L */
} sni_entry;

typedef struct coap_openssl_context_t {
  coap_dtls_context_t dtls;
  coap_tls_context_t tls;
  coap_dtls_pki_t setup_data;
  int psk_pki_enabled;
  size_t sni_count;
  sni_entry *sni_entry_list;
} coap_openssl_context_t;

int coap_dtls_is_supported(void) {
  if (SSLeay() < 0x10100000L) {
    coap_log(LOG_WARNING, "OpenSSL version 1.1.0 or later is required\n");
    return 0;
  }
#if OPENSSL_VERSION_NUMBER >= 0x10101000L
  /*
   * For 1.1.1, we need to use SSL_CTX_set_client_hello_cb()
   * which is not in 1.1.0 instead of SSL_CTX_set_tlsext_servername_callback()
   *
   * However, there could be a runtime undefined external reference error
   * as SSL_CTX_set_client_hello_cb() is not there in 1.1.0.
   */
  if (SSLeay() < 0x10101000L) {
    coap_log(LOG_WARNING, "OpenSSL version 1.1.1 or later is required\n");
    return 0;
  }
#endif /* OPENSSL_VERSION_NUMBER >= 0x10101000L */
  return 1;
}

int coap_tls_is_supported(void) {
  if (SSLeay() < 0x10100000L) {
    coap_log(LOG_WARNING, "OpenSSL version 1.1.0 or later is required\n");
    return 0;
  }
#if OPENSSL_VERSION_NUMBER >= 0x10101000L
  if (SSLeay() < 0x10101000L) {
    coap_log(LOG_WARNING, "OpenSSL version 1.1.1 or later is required\n");
    return 0;
  }
#endif /* OPENSSL_VERSION_NUMBER >= 0x10101000L */
  return 1;
}

coap_tls_version_t *
coap_get_tls_library_version(void) {
  static coap_tls_version_t version;
  version.version = SSLeay();
  version.built_version = OPENSSL_VERSION_NUMBER;
  version.type = COAP_TLS_LIBRARY_OPENSSL;
  return &version;
}

void coap_dtls_startup(void) {
  SSL_load_error_strings();
  SSL_library_init();
}

static int dtls_log_level = 0;

void coap_dtls_set_log_level(int level) {
  dtls_log_level = level;
}

int coap_dtls_get_log_level(void) {
  return dtls_log_level;
}

typedef struct coap_ssl_st {
  coap_session_t *session;
  const void *pdu;
  unsigned pdu_len;
  unsigned peekmode;
  coap_tick_t timeout;
} coap_ssl_data;

static int coap_dgram_create(BIO *a) {
  coap_ssl_data *data = NULL;
  data = malloc(sizeof(coap_ssl_data));
  if (data == NULL)
    return 0;
  BIO_set_init(a, 1);
  BIO_set_data(a, data);
  memset(data, 0x00, sizeof(coap_ssl_data));
  return 1;
}

static int coap_dgram_destroy(BIO *a) {
  coap_ssl_data *data;
  if (a == NULL)
    return 0;
  data = (coap_ssl_data *)BIO_get_data(a);
  if (data != NULL)
    free(data);
  return 1;
}

static int coap_dgram_read(BIO *a, char *out, int outl) {
  int ret = 0;
  coap_ssl_data *data = (coap_ssl_data *)BIO_get_data(a);

  if (out != NULL) {
    if (data != NULL && data->pdu_len > 0) {
      if (outl < (int)data->pdu_len) {
        memcpy(out, data->pdu, outl);
        ret = outl;
      } else {
        memcpy(out, data->pdu, data->pdu_len);
        ret = (int)data->pdu_len;
      }
      if (!data->peekmode) {
        data->pdu_len = 0;
        data->pdu = NULL;
      }
    } else {
      ret = -1;
    }
    BIO_clear_retry_flags(a);
    if (ret < 0)
      BIO_set_retry_read(a);
  }
  return ret;
}

static int coap_dgram_write(BIO *a, const char *in, int inl) {
  int ret = 0;
  coap_ssl_data *data = (coap_ssl_data *)BIO_get_data(a);

  if (data->session) {
    if (data->session->sock.flags == COAP_SOCKET_EMPTY && data->session->endpoint == NULL) {
      /* socket was closed on client due to error */
      BIO_clear_retry_flags(a);
      return -1;
    }
    ret = (int)coap_session_send(data->session, (const uint8_t *)in, (size_t)inl);
    BIO_clear_retry_flags(a);
    if (ret <= 0)
      BIO_set_retry_write(a);
  } else {
    BIO_clear_retry_flags(a);
    ret = -1;
  }
  return ret;
}

static int coap_dgram_puts(BIO *a, const char *pstr) {
  return coap_dgram_write(a, pstr, (int)strlen(pstr));
}

static long coap_dgram_ctrl(BIO *a, int cmd, long num, void *ptr) {
  long ret = 1;
  coap_ssl_data *data = BIO_get_data(a);

  (void)ptr;

  switch (cmd) {
  case BIO_CTRL_GET_CLOSE:
    ret = BIO_get_shutdown(a);
    break;
  case BIO_CTRL_SET_CLOSE:
    BIO_set_shutdown(a, (int)num);
    ret = 1;
    break;
  case BIO_CTRL_DGRAM_SET_PEEK_MODE:
    data->peekmode = (unsigned)num;
    break;
  case BIO_CTRL_DGRAM_CONNECT:
  case BIO_C_SET_FD:
  case BIO_C_GET_FD:
  case BIO_CTRL_DGRAM_SET_DONT_FRAG:
  case BIO_CTRL_DGRAM_GET_MTU:
  case BIO_CTRL_DGRAM_SET_MTU:
  case BIO_CTRL_DGRAM_QUERY_MTU:
  case BIO_CTRL_DGRAM_GET_FALLBACK_MTU:
    ret = -1;
    break;
  case BIO_CTRL_DUP:
  case BIO_CTRL_FLUSH:
  case BIO_CTRL_DGRAM_MTU_DISCOVER:
  case BIO_CTRL_DGRAM_SET_CONNECTED:
    ret = 1;
    break;
  case BIO_CTRL_DGRAM_SET_NEXT_TIMEOUT:
    data->timeout = coap_ticks_from_rt_us((uint64_t)((struct timeval*)ptr)->tv_sec * 1000000 + ((struct timeval*)ptr)->tv_usec);
    ret = 1;
    break;
  case BIO_CTRL_RESET:
  case BIO_C_FILE_SEEK:
  case BIO_C_FILE_TELL:
  case BIO_CTRL_INFO:
  case BIO_CTRL_PENDING:
  case BIO_CTRL_WPENDING:
  case BIO_CTRL_DGRAM_GET_PEER:
  case BIO_CTRL_DGRAM_SET_PEER:
  case BIO_CTRL_DGRAM_SET_RECV_TIMEOUT:
  case BIO_CTRL_DGRAM_GET_RECV_TIMEOUT:
  case BIO_CTRL_DGRAM_SET_SEND_TIMEOUT:
  case BIO_CTRL_DGRAM_GET_SEND_TIMEOUT:
  case BIO_CTRL_DGRAM_GET_SEND_TIMER_EXP:
  case BIO_CTRL_DGRAM_GET_RECV_TIMER_EXP:
  case BIO_CTRL_DGRAM_MTU_EXCEEDED:
  case BIO_CTRL_DGRAM_GET_MTU_OVERHEAD:
  default:
    ret = 0;
    break;
  }
  return ret;
}

static int coap_dtls_generate_cookie(SSL *ssl, unsigned char *cookie, unsigned int *cookie_len) {
  coap_dtls_context_t *dtls = (coap_dtls_context_t *)SSL_CTX_get_app_data(SSL_get_SSL_CTX(ssl));
  coap_ssl_data *data = (coap_ssl_data*)BIO_get_data(SSL_get_rbio(ssl));
  int r = HMAC_Init_ex(dtls->cookie_hmac, NULL, 0, NULL, NULL);
  r &= HMAC_Update(dtls->cookie_hmac, (const uint8_t*)&data->session->local_addr.addr, (size_t)data->session->local_addr.size);
  r &= HMAC_Update(dtls->cookie_hmac, (const uint8_t*)&data->session->remote_addr.addr, (size_t)data->session->remote_addr.size);
  r &= HMAC_Final(dtls->cookie_hmac, cookie, cookie_len);
  return r;
}

static int coap_dtls_verify_cookie(SSL *ssl, const uint8_t *cookie, unsigned int cookie_len) {
  uint8_t hmac[32];
  unsigned len = 32;
  if (coap_dtls_generate_cookie(ssl, hmac, &len) && cookie_len == len && memcmp(cookie, hmac, len) == 0)
    return 1;
  else
    return 0;
}

static unsigned coap_dtls_psk_client_callback(SSL *ssl, const char *hint, char *identity, unsigned int max_identity_len, unsigned char *buf, unsigned max_len) {
  size_t hint_len = 0, identity_len = 0, psk_len;
  coap_session_t *session = (coap_session_t*)SSL_get_app_data(ssl);

  if (hint)
    hint_len = strlen(hint);
  else
    hint = "";

  coap_log(LOG_DEBUG, "got psk_identity_hint: '%.*s'\n", (int)hint_len, hint);

  if (session == NULL || session->context == NULL || session->context->get_client_psk == NULL)
    return 0;

  psk_len = session->context->get_client_psk(session, (const uint8_t*)hint, hint_len, (uint8_t*)identity, &identity_len, max_identity_len - 1, (uint8_t*)buf, max_len);
  if (identity_len < max_identity_len)
    identity[identity_len] = 0;
  return (unsigned)psk_len;
}

static unsigned coap_dtls_psk_server_callback(SSL *ssl, const char *identity, unsigned char *buf, unsigned max_len) {
  size_t identity_len = 0;
  coap_session_t *session = (coap_session_t*)SSL_get_app_data(ssl);

  if (identity)
    identity_len = strlen(identity);
  else
    identity = "";

  coap_log(LOG_DEBUG, "got psk_identity: '%.*s'\n",
           (int)identity_len, identity);

  if (session == NULL || session->context == NULL || session->context->get_server_psk == NULL)
    return 0;

  return (unsigned)session->context->get_server_psk(session, (const uint8_t*)identity, identity_len, (uint8_t*)buf, max_len);
}

static void coap_dtls_info_callback(const SSL *ssl, int where, int ret) {
  coap_session_t *session = (coap_session_t*)SSL_get_app_data(ssl);
  const char *pstr;
  int w = where &~SSL_ST_MASK;

  if (w & SSL_ST_CONNECT)
    pstr = "SSL_connect";
  else if (w & SSL_ST_ACCEPT)
    pstr = "SSL_accept";
  else
    pstr = "undefined";

  if (where & SSL_CB_LOOP) {
    if (dtls_log_level >= LOG_DEBUG)
      coap_log(LOG_DEBUG, "*  %s: %s:%s\n",
               coap_session_str(session), pstr, SSL_state_string_long(ssl));
  } else if (where & SSL_CB_ALERT) {
    pstr = (where & SSL_CB_READ) ? "read" : "write";
    if (dtls_log_level >= LOG_INFO)
      coap_log(LOG_INFO, "*  %s: SSL3 alert %s:%s:%s\n",
               coap_session_str(session),
               pstr,
               SSL_alert_type_string_long(ret),
               SSL_alert_desc_string_long(ret));
    if ((where & (SSL_CB_WRITE|SSL_CB_READ)) && (ret >> 8) == SSL3_AL_FATAL)
      session->dtls_event = COAP_EVENT_DTLS_ERROR;
  } else if (where & SSL_CB_EXIT) {
    if (ret == 0) {
      if (dtls_log_level >= LOG_WARNING) {
        unsigned long e;
        coap_log(LOG_WARNING, "*  %s: %s:failed in %s\n",
                 coap_session_str(session), pstr, SSL_state_string_long(ssl));
        while ((e = ERR_get_error()))
          coap_log(LOG_WARNING, "*  %s:   %s at %s:%s\n",
                   coap_session_str(session), ERR_reason_error_string(e),
                   ERR_lib_error_string(e), ERR_func_error_string(e));
      }
    } else if (ret < 0) {
      if (dtls_log_level >= LOG_WARNING) {
        int err = SSL_get_error(ssl, ret);
        if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE && err != SSL_ERROR_WANT_CONNECT && err != SSL_ERROR_WANT_ACCEPT && err != SSL_ERROR_WANT_X509_LOOKUP) {
          long e;
          coap_log(LOG_WARNING, "*  %s: %s:error in %s\n",
                   coap_session_str(session), pstr, SSL_state_string_long(ssl));
          while ((e = ERR_get_error()))
            coap_log(LOG_WARNING, "*  %s: %s at %s:%s\n",
                     coap_session_str(session), ERR_reason_error_string(e),
                     ERR_lib_error_string(e), ERR_func_error_string(e));
        }
      }
    }
  }

  if (where == SSL_CB_HANDSHAKE_START && SSL_get_state(ssl) == TLS_ST_OK)
    session->dtls_event = COAP_EVENT_DTLS_RENEGOTIATE;
}

static int coap_sock_create(BIO *a) {
  BIO_set_init(a, 1);
  return 1;
}

static int coap_sock_destroy(BIO *a) {
  (void)a;
  return 1;
}

static int coap_sock_read(BIO *a, char *out, int outl) {
  int ret = 0;
  coap_session_t *session = (coap_session_t *)BIO_get_data(a);

  if (out != NULL) {
    ret = (int)coap_socket_read(&session->sock, (uint8_t*)out, (size_t)outl);
    if (ret == 0) {
      BIO_set_retry_read(a);
      ret = -1;
    } else {
      BIO_clear_retry_flags(a);
    }
  }
  return ret;
}

static int coap_sock_write(BIO *a, const char *in, int inl) {
  int ret = 0;
  coap_session_t *session = (coap_session_t *)BIO_get_data(a);

  ret = (int)coap_socket_write(&session->sock, (const uint8_t*)in, (size_t)inl);
  BIO_clear_retry_flags(a);
  if (ret == 0) {
    BIO_set_retry_read(a);
    ret = -1;
  } else {
    BIO_clear_retry_flags(a);
  }
  return ret;
}

static int coap_sock_puts(BIO *a, const char *pstr) {
  return coap_sock_write(a, pstr, (int)strlen(pstr));
}

static long coap_sock_ctrl(BIO *a, int cmd, long num, void *ptr) {
  int r = 1;
  (void)a;
  (void)ptr;
  (void)num;

  switch (cmd) {
  case BIO_C_SET_FD:
  case BIO_C_GET_FD:
    r = -1;
    break;
  case BIO_CTRL_SET_CLOSE:
  case BIO_CTRL_DUP:
  case BIO_CTRL_FLUSH:
    r = 1;
    break;
  default:
  case BIO_CTRL_GET_CLOSE:
    r = 0;
    break;
  }
  return r;
}

void *coap_dtls_new_context(struct coap_context_t *coap_context) {
  coap_openssl_context_t *context;
  (void)coap_context;

  context = (coap_openssl_context_t *)coap_malloc(sizeof(coap_openssl_context_t));
  if (context) {
    uint8_t cookie_secret[32];

    memset(context, 0, sizeof(coap_openssl_context_t));

    /* Set up DTLS context */
    context->dtls.ctx = SSL_CTX_new(DTLS_method());
    if (!context->dtls.ctx)
      goto error;
    SSL_CTX_set_min_proto_version(context->dtls.ctx, DTLS1_2_VERSION);
    SSL_CTX_set_app_data(context->dtls.ctx, &context->dtls);
    SSL_CTX_set_read_ahead(context->dtls.ctx, 1);
    SSL_CTX_set_cipher_list(context->dtls.ctx, "TLSv1.2:TLSv1.0");
    if (!RAND_bytes(cookie_secret, (int)sizeof(cookie_secret))) {
      if (dtls_log_level >= LOG_WARNING)
        coap_log(LOG_WARNING,
                 "Insufficient entropy for random cookie generation");
      prng(cookie_secret, sizeof(cookie_secret));
    }
    context->dtls.cookie_hmac = HMAC_CTX_new();
    if (!HMAC_Init_ex(context->dtls.cookie_hmac, cookie_secret, (int)sizeof(cookie_secret), EVP_sha256(), NULL))
      goto error;
    SSL_CTX_set_cookie_generate_cb(context->dtls.ctx, coap_dtls_generate_cookie);
    SSL_CTX_set_cookie_verify_cb(context->dtls.ctx, coap_dtls_verify_cookie);
    SSL_CTX_set_info_callback(context->dtls.ctx, coap_dtls_info_callback);
    SSL_CTX_set_options(context->dtls.ctx, SSL_OP_NO_QUERY_MTU);
    context->dtls.meth = BIO_meth_new(BIO_TYPE_DGRAM, "coapdgram");
    if (!context->dtls.meth)
      goto error;
    context->dtls.bio_addr = BIO_ADDR_new();
    if (!context->dtls.bio_addr)
      goto error;
    BIO_meth_set_write(context->dtls.meth, coap_dgram_write);
    BIO_meth_set_read(context->dtls.meth, coap_dgram_read);
    BIO_meth_set_puts(context->dtls.meth, coap_dgram_puts);
    BIO_meth_set_ctrl(context->dtls.meth, coap_dgram_ctrl);
    BIO_meth_set_create(context->dtls.meth, coap_dgram_create);
    BIO_meth_set_destroy(context->dtls.meth, coap_dgram_destroy);

    /* Set up TLS context */
    context->tls.ctx = SSL_CTX_new(TLS_method());
    if (!context->tls.ctx)
      goto error;
    SSL_CTX_set_app_data(context->tls.ctx, &context->tls);
    SSL_CTX_set_min_proto_version(context->tls.ctx, TLS1_VERSION);
    SSL_CTX_set_cipher_list(context->tls.ctx, "TLSv1.2:TLSv1.0");
    SSL_CTX_set_info_callback(context->tls.ctx, coap_dtls_info_callback);
    context->tls.meth = BIO_meth_new(BIO_TYPE_SOCKET, "coapsock");
    if (!context->tls.meth)
      goto error;
    BIO_meth_set_write(context->tls.meth, coap_sock_write);
    BIO_meth_set_read(context->tls.meth, coap_sock_read);
    BIO_meth_set_puts(context->tls.meth, coap_sock_puts);
    BIO_meth_set_ctrl(context->tls.meth, coap_sock_ctrl);
    BIO_meth_set_create(context->tls.meth, coap_sock_create);
    BIO_meth_set_destroy(context->tls.meth, coap_sock_destroy);
  }

  return context;

error:
  coap_dtls_free_context(context);
  return NULL;
}

int
coap_dtls_context_set_psk(coap_context_t *ctx,
                          const char *identity_hint,
                          coap_dtls_role_t role
) {
  coap_openssl_context_t *context = ((coap_openssl_context_t *)ctx->dtls_context);
  BIO *bio;

  if (role == COAP_DTLS_ROLE_SERVER) {
    SSL_CTX_set_psk_server_callback(context->dtls.ctx, coap_dtls_psk_server_callback);
    SSL_CTX_set_psk_server_callback(context->tls.ctx, coap_dtls_psk_server_callback);
    SSL_CTX_use_psk_identity_hint(context->dtls.ctx, identity_hint ? identity_hint : "");
    SSL_CTX_use_psk_identity_hint(context->tls.ctx, identity_hint ? identity_hint : "");
  }
  if (!context->dtls.ssl) {
    /* This is set up to handle new incoming sessions to a server */
    context->dtls.ssl = SSL_new(context->dtls.ctx);
    if (!context->dtls.ssl)
      return 0;
    bio = BIO_new(context->dtls.meth);
    if (!bio) {
      SSL_free (context->dtls.ssl);
      context->dtls.ssl = NULL;
      return 0;
    }
    SSL_set_bio(context->dtls.ssl, bio, bio);
    SSL_set_app_data(context->dtls.ssl, NULL);
    SSL_set_options(context->dtls.ssl, SSL_OP_COOKIE_EXCHANGE);
    SSL_set_mtu(context->dtls.ssl, COAP_DEFAULT_MTU);
  }
  context->psk_pki_enabled |= IS_PSK;
  return 1;
}

static int
map_key_type(int asn1_private_key_type
) {
  switch (asn1_private_key_type) {
  case COAP_ASN1_PKEY_NONE: return EVP_PKEY_NONE;
  case COAP_ASN1_PKEY_RSA: return EVP_PKEY_RSA;
  case COAP_ASN1_PKEY_RSA2: return EVP_PKEY_RSA2;
  case COAP_ASN1_PKEY_DSA: return EVP_PKEY_DSA;
  case COAP_ASN1_PKEY_DSA1: return EVP_PKEY_DSA1;
  case COAP_ASN1_PKEY_DSA2: return EVP_PKEY_DSA2;
  case COAP_ASN1_PKEY_DSA3: return EVP_PKEY_DSA3;
  case COAP_ASN1_PKEY_DSA4: return EVP_PKEY_DSA4;
  case COAP_ASN1_PKEY_DH: return EVP_PKEY_DH;
  case COAP_ASN1_PKEY_DHX: return EVP_PKEY_DHX;
  case COAP_ASN1_PKEY_EC: return EVP_PKEY_EC;
  case COAP_ASN1_PKEY_HMAC: return EVP_PKEY_HMAC;
  case COAP_ASN1_PKEY_CMAC: return EVP_PKEY_CMAC;
  case COAP_ASN1_PKEY_TLS1_PRF: return EVP_PKEY_TLS1_PRF;
  case COAP_ASN1_PKEY_HKDF: return EVP_PKEY_HKDF;
  default:
    coap_log(LOG_WARNING,
             "*** setup_pki: DTLS: Unknown Private Key type %d for ASN1\n",
             asn1_private_key_type);
    break;
  }
  return 0;
}
static uint8_t coap_alpn[] = { 4, 'c', 'o', 'a', 'p' };

static int
server_alpn_callback (SSL *ssl UNUSED,
                      const unsigned char **out,
                      unsigned char *outlen,
                      const unsigned char *in,
                      unsigned int inlen,
                      void *arg UNUSED
) {
  unsigned char *tout = NULL;
  int ret;
  if (inlen == 0)
    return SSL_TLSEXT_ERR_NOACK;
  ret = SSL_select_next_proto(&tout,
                              outlen,
                              coap_alpn,
                              sizeof(coap_alpn),
                              in,
                              inlen);
  *out = tout;
  return (ret != OPENSSL_NPN_NEGOTIATED) ? SSL_TLSEXT_ERR_NOACK : SSL_TLSEXT_ERR_OK;
}

static void
add_ca_to_cert_store(X509_STORE *st, X509 *x509)
{
  long e;

  /* Flush out existing errors */
  while ((e = ERR_get_error()) != 0) {
  }

  if (!X509_STORE_add_cert(st, x509)) {
    while ((e = ERR_get_error()) != 0) {
      int r = ERR_GET_REASON(e);
      if (r != X509_R_CERT_ALREADY_IN_HASH_TABLE) {
        /* Not already added */
        coap_log(LOG_WARNING, "***setup_pki: (D)TLS: %s at %s:%s\n",
                 ERR_reason_error_string(e),
                 ERR_lib_error_string(e),
                 ERR_func_error_string(e));
      }
    }
  }
}

#if OPENSSL_VERSION_NUMBER < 0x10101000L
static int
setup_pki_server(SSL_CTX *ctx,
                 coap_dtls_pki_t* setup_data
) {
  switch (setup_data->pki_key.key_type) {
  case COAP_PKI_KEY_PEM:
    if (setup_data->pki_key.key.pem.public_cert &&
        setup_data->pki_key.key.pem.public_cert[0]) {
      if (!(SSL_CTX_use_certificate_file(ctx,
                                        setup_data->pki_key.key.pem.public_cert,
                                        SSL_FILETYPE_PEM))) {
        coap_log(LOG_WARNING,
                 "*** setup_pki: (D)TLS: %s: Unable to configure "
                 "Server Certificate\n",
                 setup_data->pki_key.key.pem.public_cert);
        return 0;
      }
    }
    else {
      coap_log(LOG_ERR,
             "*** setup_pki: (D)TLS: No Server Certificate defined\n");
      return 0;
    }

    if (setup_data->pki_key.key.pem.private_key &&
        setup_data->pki_key.key.pem.private_key[0]) {
      if (!(SSL_CTX_use_PrivateKey_file(ctx,
                                        setup_data->pki_key.key.pem.private_key,
                                        SSL_FILETYPE_PEM))) {
        coap_log(LOG_WARNING,
                 "*** setup_pki: (D)TLS: %s: Unable to configure "
                 "Server Private Key\n",
                  setup_data->pki_key.key.pem.private_key);
        return 0;
      }
    }
    else {
      coap_log(LOG_ERR,
           "*** setup_pki: (D)TLS: No Server Private Key defined\n");
      return 0;
    }

    if (setup_data->pki_key.key.pem.ca_file &&
        setup_data->pki_key.key.pem.ca_file[0]) {
      STACK_OF(X509_NAME) *cert_names;
      X509_STORE *st;
      BIO *in;
      X509 *x = NULL;
      char *rw_var = NULL;
      cert_names = SSL_load_client_CA_file(setup_data->pki_key.key.pem.ca_file);
      if (cert_names != NULL)
        SSL_CTX_set_client_CA_list(ctx, cert_names);
      else {
        coap_log(LOG_WARNING,
                 "*** setup_pki: (D)TLS: %s: Unable to configure "
                 "client CA File\n",
                  setup_data->pki_key.key.pem.ca_file);
        return 0;
      }
      st = SSL_CTX_get_cert_store(ctx);
      in = BIO_new(BIO_s_file());
      /* Need to do this to not get a compiler warning about const parameters */
      memcpy(&rw_var, &setup_data->pki_key.key.pem.ca_file, sizeof (rw_var));
      if (!BIO_read_filename(in, rw_var)) {
        BIO_free(in);
        X509_free(x);
        break;
      }

      for (;;) {
        if (PEM_read_bio_X509(in, &x, NULL, NULL) == NULL)
            break;
        add_ca_to_cert_store(st, x);
      }
      BIO_free(in);
      X509_free(x);
    }
    break;

  case COAP_PKI_KEY_ASN1:
    if (setup_data->pki_key.key.asn1.public_cert &&
        setup_data->pki_key.key.asn1.public_cert_len > 0) {
      if (!(SSL_CTX_use_certificate_ASN1(ctx,
                                 setup_data->pki_key.key.asn1.public_cert_len,
                                 setup_data->pki_key.key.asn1.public_cert))) {
        coap_log(LOG_WARNING,
                 "*** setup_pki: (D)TLS: %s: Unable to configure "
                 "Server Certificate\n",
                 "ASN1");
        return 0;
      }
    }
    else {
      coap_log(LOG_ERR,
             "*** setup_pki: (D)TLS: No Server Certificate defined\n");
      return 0;
    }

    if (setup_data->pki_key.key.asn1.private_key &&
             setup_data->pki_key.key.asn1.private_key_len > 0) {
      int pkey_type = map_key_type(setup_data->pki_key.key.asn1.private_key_type);
      if (!(SSL_CTX_use_PrivateKey_ASN1(pkey_type, ctx,
                             setup_data->pki_key.key.asn1.private_key,
                             setup_data->pki_key.key.asn1.private_key_len))) {
        coap_log(LOG_WARNING,
                 "*** setup_pki: (D)TLS: %s: Unable to configure "
                 "Server Private Key\n",
                 "ASN1");
        return 0;
      }
    }
    else {
      coap_log(LOG_ERR,
             "*** setup_pki: (D)TLS: No Server Private Key defined\n");
      return 0;
    }

    if (setup_data->pki_key.key.asn1.ca_cert &&
        setup_data->pki_key.key.asn1.ca_cert_len > 0) {
      /* Need to use a temp variable as it gets incremented*/
      const uint8_t *p = setup_data->pki_key.key.asn1.ca_cert;
      X509* x509 = d2i_X509(NULL, &p, setup_data->pki_key.key.asn1.ca_cert_len);
      X509_STORE *st;
      if (!x509 || !SSL_CTX_add_client_CA(ctx, x509)) {
        coap_log(LOG_WARNING,
                 "*** setup_pki: (D)TLS: %s: Unable to configure "
                 "client CA File\n",
                  "ASN1");
        X509_free(x509);
        return 0;
      }
      st = SSL_CTX_get_cert_store(ctx);
      add_ca_to_cert_store(st, x509);
      X509_free(x509);
    }
    break;
  default:
    coap_log(LOG_ERR,
             "*** setup_pki: (D)TLS: Unknown key type %d\n",
             setup_data->pki_key.key_type);
    return 0;
  }

  return 1;
}
#endif /* OPENSSL_VERSION_NUMBER < 0x10101000L */

static int
setup_pki_ssl(SSL *ssl,
                 coap_dtls_pki_t* setup_data, coap_dtls_role_t role
) {
  switch (setup_data->pki_key.key_type) {
  case COAP_PKI_KEY_PEM:
    if (setup_data->pki_key.key.pem.public_cert &&
        setup_data->pki_key.key.pem.public_cert[0]) {
      if (!(SSL_use_certificate_file(ssl,
                                   setup_data->pki_key.key.pem.public_cert,
                                   SSL_FILETYPE_PEM))) {
        coap_log(LOG_WARNING,
                 "*** setup_pki: (D)TLS: %s: Unable to configure "
                 "%s Certificate\n",
                 setup_data->pki_key.key.pem.public_cert,
                 role == COAP_DTLS_ROLE_SERVER ? "Server" : "Client");
        return 0;
      }
    }
    else if (role == COAP_DTLS_ROLE_SERVER ||
             (setup_data->pki_key.key.pem.private_key &&
              setup_data->pki_key.key.pem.private_key[0])) {
      coap_log(LOG_ERR,
             "*** setup_pki: (D)TLS: No %s Certificate defined\n",
             role == COAP_DTLS_ROLE_SERVER ? "Server" : "Client");
      return 0;
    }
    if (setup_data->pki_key.key.pem.private_key &&
        setup_data->pki_key.key.pem.private_key[0]) {
      if (!(SSL_use_PrivateKey_file(ssl,
                                  setup_data->pki_key.key.pem.private_key,
                                  SSL_FILETYPE_PEM))) {
        coap_log(LOG_WARNING,
                 "*** setup_pki: (D)TLS: %s: Unable to configure "
                 "Client Private Key\n",
                  setup_data->pki_key.key.pem.private_key);
        return 0;
      }
    }
    else if (role == COAP_DTLS_ROLE_SERVER ||
             (setup_data->pki_key.key.pem.public_cert &&
              setup_data->pki_key.key.pem.public_cert[0])) {
      coap_log(LOG_ERR,
             "*** setup_pki: (D)TLS: No %s Private Key defined\n",
             role == COAP_DTLS_ROLE_SERVER ? "Server" : "Client");
      return 0;
    }
    if (setup_data->pki_key.key.pem.ca_file &&
        setup_data->pki_key.key.pem.ca_file[0]) {
      X509_STORE *st;
      BIO *in;
      X509 *x = NULL;
      char *rw_var = NULL;
      SSL_CTX *ctx = SSL_get_SSL_CTX(ssl);

      if (role == COAP_DTLS_ROLE_SERVER) {
        STACK_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(setup_data->pki_key.key.pem.ca_file);

        if (cert_names != NULL)
          SSL_set_client_CA_list(ssl, cert_names);
        else {
          coap_log(LOG_WARNING,
                   "*** setup_pki: (D)TLS: %s: Unable to configure "
                   "%s CA File\n",
                    setup_data->pki_key.key.pem.ca_file,
                    role == COAP_DTLS_ROLE_SERVER ? "Server" : "Client");
          return 0;
        }
      }

      /* Add CA to the trusted root CA store */
      in = BIO_new(BIO_s_file());
      /* Need to do this to not get a compiler warning about const parameters */
      memcpy(&rw_var, &setup_data->pki_key.key.pem.ca_file, sizeof (rw_var));
      if (!BIO_read_filename(in, rw_var)) {
        BIO_free(in);
        X509_free(x);
        break;
      }
      st = SSL_CTX_get_cert_store(ctx);
      for (;;) {
        if (PEM_read_bio_X509(in, &x, NULL, NULL) == NULL)
            break;
        add_ca_to_cert_store(st, x);
      }
      BIO_free(in);
      X509_free(x);
    }
    break;

  case COAP_PKI_KEY_ASN1:
    if (setup_data->pki_key.key.asn1.public_cert &&
        setup_data->pki_key.key.asn1.public_cert_len > 0) {
      if (!(SSL_use_certificate_ASN1(ssl,
                           setup_data->pki_key.key.asn1.public_cert,
                           setup_data->pki_key.key.asn1.public_cert_len))) {
        coap_log(LOG_WARNING,
                 "*** setup_pki: (D)TLS: %s: Unable to configure "
                 "%s Certificate\n",
                 role == COAP_DTLS_ROLE_SERVER ? "Server" : "Client",
                 "ASN1");
        return 0;
      }
    }
    else if (role == COAP_DTLS_ROLE_SERVER ||
             (setup_data->pki_key.key.asn1.private_key &&
              setup_data->pki_key.key.asn1.private_key[0])) {
      coap_log(LOG_ERR,
             "*** setup_pki: (D)TLS: No %s Certificate defined\n",
             role == COAP_DTLS_ROLE_SERVER ? "Server" : "Client");
      return 0;
    }
    if (setup_data->pki_key.key.asn1.private_key &&
             setup_data->pki_key.key.asn1.private_key_len > 0) {
      int pkey_type = map_key_type(setup_data->pki_key.key.asn1.private_key_type);
      if (!(SSL_use_PrivateKey_ASN1(pkey_type, ssl,
                        setup_data->pki_key.key.asn1.private_key,
                        setup_data->pki_key.key.asn1.private_key_len))) {
        coap_log(LOG_WARNING,
                 "*** setup_pki: (D)TLS: %s: Unable to configure "
                 "%s Private Key\n",
                 role == COAP_DTLS_ROLE_SERVER ? "Server" : "Client",
                 "ASN1");
        return 0;
      }
    }
    else if (role == COAP_DTLS_ROLE_SERVER ||
             (setup_data->pki_key.key.asn1.public_cert &&
              setup_data->pki_key.key.asn1.public_cert_len > 0)) {
      coap_log(LOG_ERR,
             "*** setup_pki: (D)TLS: No %s Private Key defined",
             role == COAP_DTLS_ROLE_SERVER ? "Server" : "Client");
      return 0;
    }
    if (setup_data->pki_key.key.asn1.ca_cert &&
        setup_data->pki_key.key.asn1.ca_cert_len > 0) {
      /* Need to use a temp variable as it gets incremented*/
      const uint8_t *p = setup_data->pki_key.key.asn1.ca_cert;
      X509* x509 = d2i_X509(NULL, &p, setup_data->pki_key.key.asn1.ca_cert_len);
      X509_STORE *st;
      SSL_CTX *ctx = SSL_get_SSL_CTX(ssl);

      if (role == COAP_DTLS_ROLE_SERVER) {
        if (!x509 || !SSL_add_client_CA(ssl, x509)) {
          coap_log(LOG_WARNING,
                   "*** setup_pki: (D)TLS: %s: Unable to configure "
                   "client CA File\n",
                    "ASN1");
          X509_free(x509);
          return 0;
        }
      }

      /* Add CA to the trusted root CA store */
      st = SSL_CTX_get_cert_store(ctx);
      add_ca_to_cert_store(st, x509);
      X509_free(x509);
    }
    break;
  default:
    coap_log(LOG_ERR,
             "*** setup_pki: (D)TLS: Unknown key type %d\n",
             setup_data->pki_key.key_type);
    return 0;
  }
  return 1;
}

static char*
get_common_name_from_cert(X509* x509) {
  if (x509) {
    char *cn;
    int n;
    STACK_OF(GENERAL_NAME) *san_list;
    char buffer[256];

    san_list = X509_get_ext_d2i(x509, NID_subject_alt_name, NULL, NULL);
    if (san_list) {
      int san_count = sk_GENERAL_NAME_num(san_list);

      for (n = 0; n < san_count; n++) {
        const GENERAL_NAME * name = sk_GENERAL_NAME_value (san_list, n);

        if (name->type == GEN_DNS) {
          const char *dns_name = (const char *)ASN1_STRING_get0_data(name->d.dNSName);

          /* Make sure that there is not an embedded NUL in the dns_name */
          if (ASN1_STRING_length(name->d.dNSName) != (int)strlen (dns_name))
            continue;
          cn = OPENSSL_strdup(dns_name);
          sk_GENERAL_NAME_pop_free(san_list, GENERAL_NAME_free);
          return cn;
        }
      }
      sk_GENERAL_NAME_pop_free(san_list, GENERAL_NAME_free);
    }
    /* Otherwise look for the CN= field */
    X509_NAME_oneline(X509_get_subject_name(x509), buffer, sizeof(buffer));

    /* Need to emulate strcasestr() here.  Looking for CN= */
    n = strlen(buffer) - 3;
    cn = buffer;
    while (n > 0) {
      if (((cn[0] == 'C') || (cn[0] == 'c')) &&
          ((cn[1] == 'N') || (cn[1] == 'n')) &&
          (cn[2] == '=')) {
        cn += 3;
        break;
      }
      cn++;
      n--;
    }
    if (n > 0) {
      char * ecn = strchr(cn, '/');
      if (ecn) {
        return OPENSSL_strndup(cn, ecn-cn);
      }
      else {
        return OPENSSL_strdup(cn);
      }
    }
  }
  return NULL;
}

static int
tls_verify_call_back(int preverify_ok, X509_STORE_CTX *ctx) {
  SSL *ssl = X509_STORE_CTX_get_ex_data(ctx,
                              SSL_get_ex_data_X509_STORE_CTX_idx());
  coap_session_t *session = SSL_get_app_data(ssl);
  coap_openssl_context_t *context =
           ((coap_openssl_context_t *)session->context->dtls_context);
  coap_dtls_pki_t *setup_data = &context->setup_data;
  int depth = X509_STORE_CTX_get_error_depth(ctx);
  int err = X509_STORE_CTX_get_error(ctx);
  X509 *x509 = X509_STORE_CTX_get_current_cert(ctx);
  char *cn = get_common_name_from_cert(x509);
  int keep_preverify_ok = preverify_ok;

  if (!preverify_ok) {
    switch (err) {
    case X509_V_ERR_CERT_NOT_YET_VALID:
    case X509_V_ERR_CERT_HAS_EXPIRED:
      if (setup_data->allow_expired_certs)
        preverify_ok = 1;
      break;
    case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
      if (setup_data->allow_self_signed)
        preverify_ok = 1;
      break;
    case X509_V_ERR_UNABLE_TO_GET_CRL:
      if (setup_data->allow_no_crl)
        preverify_ok = 1;
      break;
    case X509_V_ERR_CRL_NOT_YET_VALID:
    case X509_V_ERR_CRL_HAS_EXPIRED:
      if (setup_data->allow_expired_crl)
        preverify_ok = 1;
      break;
    default:
      break;
    }
    if (!preverify_ok) {
        coap_log(LOG_WARNING,
               "    %s: %s: '%s' depth=%d\n",
               coap_session_str(session),
               X509_verify_cert_error_string(err), cn ? cn : "?", depth);
        /* Invoke the CN callback function for this failure */
        keep_preverify_ok = 1;
    }
    else {
        coap_log(LOG_WARNING,
               "    %s: %s: overridden: '%s' depth=%d\n",
               coap_session_str(session),
               X509_verify_cert_error_string(err), cn ? cn : "?", depth);
    }
  }
  /* Certificate - depth == 0 is the Client Cert */
  if (setup_data->validate_cn_call_back && keep_preverify_ok) {
    int length = i2d_X509(x509, NULL);
    uint8_t *base_buf;
    uint8_t *base_buf2 = base_buf = OPENSSL_malloc(length);

    /* base_buf2 gets moved to the end */
    i2d_X509(x509, &base_buf2);
    if (!setup_data->validate_cn_call_back(cn, base_buf, length, session,
                                           depth, preverify_ok,
                                           setup_data->cn_call_back_arg)) {
      if (depth == 0) {
        X509_STORE_CTX_set_error(ctx, X509_V_ERR_CERT_REJECTED);
      }
      else {
        X509_STORE_CTX_set_error(ctx, X509_V_ERR_INVALID_CA);
      }
      preverify_ok = 0;
    }
    OPENSSL_free(base_buf);
  }
  OPENSSL_free(cn);
  return preverify_ok;
}

#if OPENSSL_VERSION_NUMBER < 0x10101000L
/*
 * During the SSL/TLS initial negotiations, tls_secret_call_back() is called so
 * it is possible to determine whether this is a PKI or PSK incoming
 * request and adjust the Ciphers if necessary
 *
 * Set up by SSL_set_session_secret_cb() in tls_server_name_call_back()
 */
static int
tls_secret_call_back(SSL *ssl,
  void *secret UNUSED,
  int *secretlen UNUSED,
  STACK_OF(SSL_CIPHER) *peer_ciphers,
  const SSL_CIPHER **cipher UNUSED,
  void *arg
) {
  int     ii;
  int     psk_requested = 0;
  coap_session_t *session = SSL_get_app_data(ssl);
  coap_dtls_pki_t *setup_data = (coap_dtls_pki_t*)arg;

  if (session && session->context->psk_key && session->context->psk_key_len) {
    /* Is PSK being requested - if so, we need to change algorithms */
    for (ii = 0; ii < sk_SSL_CIPHER_num (peer_ciphers); ii++) {
      const SSL_CIPHER *peer_cipher = sk_SSL_CIPHER_value(peer_ciphers, ii);

      if (strstr (SSL_CIPHER_get_name (peer_cipher), "PSK")) {
        psk_requested = 1;
        break;
      }
    }
  }
  if (!psk_requested) {
    if (session) {
      coap_log(LOG_DEBUG, "   %s: Using PKI ciphers\n",
                coap_session_str(session));
    }
    else {
      coap_log(LOG_DEBUG, "Using PKI ciphers\n");
    }
    if (setup_data->verify_peer_cert) {
      if (setup_data->require_peer_cert) {
        SSL_set_verify(ssl,
                       SSL_VERIFY_PEER |
                       SSL_VERIFY_CLIENT_ONCE |
                       SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
                       tls_verify_call_back);
      }
      else {
        SSL_set_verify(ssl,
                       SSL_VERIFY_PEER |
                       SSL_VERIFY_CLIENT_ONCE,
                       tls_verify_call_back);
      }
    }
    else {
      SSL_set_verify(ssl, SSL_VERIFY_NONE, NULL);
    }

    /* Check CA Chain */
    if (setup_data->cert_chain_validation)
      SSL_set_verify_depth(ssl, setup_data->cert_chain_verify_depth);

    /* Certificate Revocation */
    if (setup_data->check_cert_revocation) {
       X509_VERIFY_PARAM *param;

       param = X509_VERIFY_PARAM_new();
       X509_VERIFY_PARAM_set_flags(param, X509_V_FLAG_CRL_CHECK);
       SSL_set1_param(ssl, param);
       X509_VERIFY_PARAM_free(param);
    }
  }
  else {
    if (session) {
      if (session->context->psk_key && session->context->psk_key_len) {
        memcpy(secret, session->context->psk_key, session->context->psk_key_len);
        *secretlen = session->context->psk_key_len;
      }
      coap_log(LOG_DEBUG, "   %s: Setting PSK ciphers\n",
               coap_session_str(session));
    }
    else {
      coap_log(LOG_DEBUG, "Setting PSK ciphers\n");
    }
    /*
     * Force a PSK algorithm to be used, so we do PSK
     */
    SSL_set_cipher_list (ssl, "PSK:!NULL");
    SSL_set_psk_server_callback(ssl, coap_dtls_psk_server_callback);
  }
  if (setup_data->additional_tls_setup_call_back) {
    /* Additional application setup wanted */
    if (!setup_data->additional_tls_setup_call_back(ssl, setup_data))
     return 0;
  }
  return 0;
}

/*
 * During the SSL/TLS initial negotiations, tls_server_name_call_back() is called
 * so it is possible to set up an extra callback to determine whether this is
 * a PKI or PSK incoming request and adjust the Ciphers if necessary
 *
 * Set up by SSL_CTX_set_tlsext_servername_callback() in coap_dtls_context_set_pki()
 */
static int
tls_server_name_call_back(SSL *ssl,
                          int *sd UNUSED,
                          void *arg
) {
  coap_dtls_pki_t *setup_data = (coap_dtls_pki_t*)arg;

  if (!ssl) {
    return SSL_TLSEXT_ERR_NOACK;
  }

  if (setup_data->validate_sni_call_back) {
    /* SNI checking requested */
    coap_session_t *session = (coap_session_t*)SSL_get_app_data(ssl);
    coap_openssl_context_t *context =
                  ((coap_openssl_context_t *)session->context->dtls_context);
    const char *sni = SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name);
    size_t i;

    if (!sni || !sni[0]) {
      sni = "";
    }
    for (i = 0; i < context->sni_count; i++) {
      if (!strcmp(sni, context->sni_entry_list[i].sni)) {
        break;
      }
    }
    if (i == context->sni_count) {
      SSL_CTX *ctx;
      coap_dtls_pki_t sni_setup_data;
      coap_dtls_key_t *new_entry = setup_data->validate_sni_call_back(sni,
                                               setup_data->sni_call_back_arg);
      if (!new_entry) {
        return SSL_TLSEXT_ERR_ALERT_FATAL;
      }
      /* Need to set up a new SSL_CTX to switch to */
      if (session->proto == COAP_PROTO_DTLS) {
        /* Set up DTLS context */
        ctx = SSL_CTX_new(DTLS_method());
        if (!ctx)
          goto error;
        SSL_CTX_set_min_proto_version(ctx, DTLS1_2_VERSION);
        SSL_CTX_set_app_data(ctx, &context->dtls);
        SSL_CTX_set_read_ahead(ctx, 1);
        SSL_CTX_set_cipher_list(ctx, "TLSv1.2:TLSv1.0");
        SSL_CTX_set_cookie_generate_cb(ctx, coap_dtls_generate_cookie);
        SSL_CTX_set_cookie_verify_cb(ctx, coap_dtls_verify_cookie);
        SSL_CTX_set_info_callback(ctx, coap_dtls_info_callback);
        SSL_CTX_set_options(ctx, SSL_OP_NO_QUERY_MTU);
      }
      else {
        /* Set up TLS context */
        ctx = SSL_CTX_new(TLS_method());
        if (!ctx)
          goto error;
        SSL_CTX_set_app_data(ctx, &context->tls);
        SSL_CTX_set_min_proto_version(ctx, TLS1_VERSION);
        SSL_CTX_set_cipher_list(ctx, "TLSv1.2:TLSv1.0");
        SSL_CTX_set_info_callback(ctx, coap_dtls_info_callback);
        SSL_CTX_set_alpn_select_cb(ctx, server_alpn_callback, NULL);
      }
      memset(&sni_setup_data, 0, sizeof(sni_setup_data));
      sni_setup_data.pki_key.key_type = new_entry->key_type;
      sni_setup_data.pki_key.key.pem = new_entry->key.pem;
      sni_setup_data.pki_key.key.asn1 = new_entry->key.asn1;
      setup_pki_server(ctx, &sni_setup_data);

      context->sni_entry_list = OPENSSL_realloc(context->sni_entry_list,
                                     (context->sni_count+1)*sizeof(sni_entry));
      context->sni_entry_list[context->sni_count].sni = OPENSSL_strdup(sni);
      context->sni_entry_list[context->sni_count].ctx = ctx;
      context->sni_count++;
    }
    SSL_set_SSL_CTX (ssl, context->sni_entry_list[i].ctx);
    SSL_clear_options (ssl, 0xFFFFFFFFL);
    SSL_set_options (ssl, SSL_CTX_get_options (context->sni_entry_list[i].ctx));
  }

  /*
   * Have to do extra call back next to get client algorithms
   * SSL_get_client_ciphers() does not work this early on
   */
  SSL_set_session_secret_cb(ssl, tls_secret_call_back, arg);
  return SSL_TLSEXT_ERR_OK;

error:
  return SSL_TLSEXT_ERR_ALERT_WARNING;
}
#else /* OPENSSL_VERSION_NUMBER >= 0x10101000L */
/*
 * During the SSL/TLS initial negotiations, tls_client_hello_call_back() is
 * called early in the Client Hello processing so it is possible to determine
 * whether this is a PKI or PSK incoming request and adjust the Ciphers if
 * necessary.
 *
 * Set up by SSL_CTX_set_client_hello_cb().
 */
static int
tls_client_hello_call_back(SSL *ssl,
                          int *al,
                          void *arg UNUSED
) {
  coap_session_t *session = (coap_session_t *)SSL_get_app_data(ssl);
  coap_openssl_context_t *dtls_context = (coap_openssl_context_t *)session->context->dtls_context;
  coap_dtls_pki_t *setup_data = &dtls_context->setup_data;
  int psk_requested = 0;
  const unsigned char *out;
  size_t outlen;

  if (!ssl) {
    *al = SSL_AD_INTERNAL_ERROR;
    return SSL_CLIENT_HELLO_ERROR;
  }

  /*
   * See if PSK being requested
   */
  if (session && session->context->psk_key && session->context->psk_key_len) {
    int len = SSL_client_hello_get0_ciphers(ssl, &out);
    STACK_OF(SSL_CIPHER) *peer_ciphers = NULL;
    STACK_OF(SSL_CIPHER) *scsvc = NULL;

    if (len && SSL_bytes_to_cipher_list(ssl, out, len,
                                        SSL_client_hello_isv2(ssl),
                                        &peer_ciphers, &scsvc)) {
      int ii;
      for (ii = 0; ii < sk_SSL_CIPHER_num (peer_ciphers); ii++) {
        const SSL_CIPHER *peer_cipher = sk_SSL_CIPHER_value(peer_ciphers, ii);

        if (strstr (SSL_CIPHER_get_name (peer_cipher), "PSK")) {
          psk_requested = 1;
          break;
        }
      }
    }
    sk_SSL_CIPHER_free(peer_ciphers);
    sk_SSL_CIPHER_free(scsvc);
  }

  if (psk_requested) {
    /*
     * Client has requested PSK and it is supported
     */
    if (session) {
      coap_log(LOG_DEBUG, "   %s: PSK request\n",
               coap_session_str(session));
    }
    else {
      coap_log(LOG_DEBUG, "PSK request\n");
    }
    SSL_set_psk_server_callback(ssl, coap_dtls_psk_server_callback);
    if (setup_data->additional_tls_setup_call_back) {
      /* Additional application setup wanted */
      if (!setup_data->additional_tls_setup_call_back(ssl, setup_data))
       return 0;
    }
    return SSL_CLIENT_HELLO_SUCCESS;
  }

  /*
   * Handle Certificate requests
   */

  /*
   * Determine what type of certificate is being requested
   */
  if (SSL_client_hello_get0_ext(ssl, TLSEXT_TYPE_client_certificate_type,
                                &out, &outlen)) {
    size_t ii;
    for (ii = 0; ii < outlen; ii++) {
      switch (out[ii]) {
      case 0:
        /* RFC6091 X.509 */
        if (outlen >= 2) {
          /* X.509 cannot be the singular entry. RFC6091 3.1. Client Hello */
          goto is_x509;
        }
        break;
      case 2:
        /* RFC7250 RPK - not yet supported */
        break;
      default:
        break;
      }
    }
    *al = SSL_AD_UNSUPPORTED_EXTENSION;
    return SSL_CLIENT_HELLO_ERROR;
  }

is_x509:
  if (setup_data->validate_sni_call_back) {
    /*
     * SNI checking requested
     */
    coap_dtls_pki_t sni_setup_data;
    coap_openssl_context_t *context =
                  ((coap_openssl_context_t *)session->context->dtls_context);
    const char *sni = "";
    char *sni_tmp = NULL;
    size_t i;

    if (SSL_client_hello_get0_ext (ssl, TLSEXT_TYPE_server_name, &out, &outlen) &&
        outlen > 5 &&
        (((out[0]<<8) + out[1] +2) == (int)outlen) &&
        out[2] == TLSEXT_NAMETYPE_host_name &&
        (((out[3]<<8) + out[4] +2 +3) == (int)outlen)) {
      /* Skip over length, type and length */
      out += 5;
      outlen -= 5;
      sni_tmp = OPENSSL_malloc(outlen+1);
      sni_tmp[outlen] = '\000';
      memcpy(sni_tmp, out, outlen);
      sni = sni_tmp;
    }
    /* Is this a cached entry? */
    for (i = 0; i < context->sni_count; i++) {
      if (!strcmp(sni, context->sni_entry_list[i].sni)) {
        break;
      }
    }
    if (i == context->sni_count) {
      /*
       * New SNI request
       */
      coap_dtls_key_t *new_entry = setup_data->validate_sni_call_back(sni,
                                               setup_data->sni_call_back_arg);
      if (!new_entry) {
        *al = SSL_AD_UNRECOGNIZED_NAME;
        return SSL_CLIENT_HELLO_ERROR;
      }


      context->sni_entry_list = OPENSSL_realloc(context->sni_entry_list,
                                     (context->sni_count+1)*sizeof(sni_entry));
      context->sni_entry_list[context->sni_count].sni = OPENSSL_strdup(sni);
      context->sni_entry_list[context->sni_count].pki_key = *new_entry;
      context->sni_count++;
    }
    if (sni_tmp) {
      OPENSSL_free(sni_tmp);
    }
    memset(&sni_setup_data, 0, sizeof(sni_setup_data));
    sni_setup_data.pki_key = context->sni_entry_list[i].pki_key;
    setup_pki_ssl(ssl, &sni_setup_data, 1);
  }
  else {
    setup_pki_ssl(ssl, setup_data, 1);
  }

  if (session) {
    coap_log(LOG_DEBUG, "   %s: Using PKI ciphers\n",
              coap_session_str(session));
  }
  else {
    coap_log(LOG_DEBUG, "Using PKI ciphers\n");
  }
  if (setup_data->verify_peer_cert) {
    if (setup_data->require_peer_cert) {
      SSL_set_verify(ssl,
                     SSL_VERIFY_PEER |
                     SSL_VERIFY_CLIENT_ONCE |
                     SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
                     tls_verify_call_back);
    }
    else {
      SSL_set_verify(ssl,
                     SSL_VERIFY_PEER |
                     SSL_VERIFY_CLIENT_ONCE,
                     tls_verify_call_back);
    }
  }
  else {
    SSL_set_verify(ssl, SSL_VERIFY_NONE, NULL);
  }

  /* Check CA Chain */
  if (setup_data->cert_chain_validation)
    SSL_set_verify_depth(ssl, setup_data->cert_chain_verify_depth);

  /* Certificate Revocation */
  if (setup_data->check_cert_revocation) {
     X509_VERIFY_PARAM *param;

     param = X509_VERIFY_PARAM_new();
     X509_VERIFY_PARAM_set_flags(param, X509_V_FLAG_CRL_CHECK);
     SSL_set1_param(ssl, param);
     X509_VERIFY_PARAM_free(param);
  }
  if (setup_data->additional_tls_setup_call_back) {
    /* Additional application setup wanted */
    if (!setup_data->additional_tls_setup_call_back(ssl, setup_data))
     return 0;
  }
  return SSL_CLIENT_HELLO_SUCCESS;
}
#endif /* OPENSSL_VERSION_NUMBER >= 0x10101000L */

int
coap_dtls_context_set_pki(coap_context_t *ctx,
                          coap_dtls_pki_t *setup_data,
                          coap_dtls_role_t role
) {
  coap_openssl_context_t *context =
                                ((coap_openssl_context_t *)ctx->dtls_context);
  BIO *bio;
  if (!setup_data)
    return 0;
  context->setup_data = *setup_data;
  if (role == COAP_DTLS_ROLE_SERVER) {
    if (context->dtls.ctx) {
      /* SERVER DTLS */
#if OPENSSL_VERSION_NUMBER < 0x10101000L
      if (!setup_pki_server(context->dtls.ctx, setup_data))
        return 0;
#endif /* OPENSSL_VERSION_NUMBER < 0x10101000L */
      /* libcoap is managing TLS connection based on setup_data options */
      /* Need to set up logic to differentiate between a PSK or PKI session */
      /*
       * For OpenSSL 1.1.1, we need to use SSL_CTX_set_client_hello_cb()
       * which is not in 1.1.0
       */
#if OPENSSL_VERSION_NUMBER < 0x10101000L
      if (SSLeay() >= 0x10101000L) {
        coap_log(LOG_WARNING,
                 "OpenSSL compiled with %lux, linked with %lux, so "
                 "no certificate checking\n",
                 OPENSSL_VERSION_NUMBER, SSLeay());
      }
      SSL_CTX_set_tlsext_servername_arg(context->dtls.ctx, &context->setup_data);
      SSL_CTX_set_tlsext_servername_callback(context->dtls.ctx,
                                             tls_server_name_call_back);
#else /* OPENSSL_VERSION_NUMBER >= 0x10101000L */
      SSL_CTX_set_client_hello_cb(context->dtls.ctx,
                                    tls_client_hello_call_back,
                                    NULL);
#endif /* OPENSSL_VERSION_NUMBER >= 0x10101000L */
    }
    if (context->tls.ctx) {
      /* SERVER TLS */
#if OPENSSL_VERSION_NUMBER < 0x10101000L
      if (!setup_pki_server(context->tls.ctx, setup_data))
        return 0;
#endif /* OPENSSL_VERSION_NUMBER < 0x10101000L */
      /* libcoap is managing TLS connection based on setup_data options */
      /* Need to set up logic to differentiate between a PSK or PKI session */
      /*
       * For OpenSSL 1.1.1, we need to use SSL_CTX_set_client_hello_cb()
       * which is not in 1.1.0
       */
#if OPENSSL_VERSION_NUMBER < 0x10101000L
      if (SSLeay() >= 0x10101000L) {
        coap_log(LOG_WARNING,
                 "OpenSSL compiled with %lux, linked with %lux, so "
                 "no certificate checking\n",
                 OPENSSL_VERSION_NUMBER, SSLeay());
      }
      SSL_CTX_set_tlsext_servername_arg(context->tls.ctx, &context->setup_data);
      SSL_CTX_set_tlsext_servername_callback(context->tls.ctx,
                                             tls_server_name_call_back);
#else /* OPENSSL_VERSION_NUMBER >= 0x10101000L */
      SSL_CTX_set_client_hello_cb(context->tls.ctx,
                                    tls_client_hello_call_back,
                                      NULL);
#endif /* OPENSSL_VERSION_NUMBER >= 0x10101000L */
      /* TLS Only */
      SSL_CTX_set_alpn_select_cb(context->tls.ctx, server_alpn_callback, NULL);
    }
  }

  if (!context->dtls.ssl) {
    /* This is set up to handle new incoming sessions to a server */
    context->dtls.ssl = SSL_new(context->dtls.ctx);
    if (!context->dtls.ssl)
      return 0;
    bio = BIO_new(context->dtls.meth);
    if (!bio) {
      SSL_free (context->dtls.ssl);
      context->dtls.ssl = NULL;
      return 0;
    }
    SSL_set_bio(context->dtls.ssl, bio, bio);
    SSL_set_app_data(context->dtls.ssl, NULL);
    SSL_set_options(context->dtls.ssl, SSL_OP_COOKIE_EXCHANGE);
    SSL_set_mtu(context->dtls.ssl, COAP_DEFAULT_MTU);
  }
  context->psk_pki_enabled |= IS_PKI;
  return 1;
}

int
coap_dtls_context_set_pki_root_cas(struct coap_context_t *ctx,
                                   const char *ca_file,
                                   const char *ca_dir
) {
  coap_openssl_context_t *context =
                                ((coap_openssl_context_t *)ctx->dtls_context);
  if (context->dtls.ctx) {
    if (!SSL_CTX_load_verify_locations(context->dtls.ctx, ca_file, ca_dir)) {
      coap_log(LOG_WARNING, "Unable to install root CAs (%s/%s)\n",
               ca_file ? ca_file : "NULL", ca_dir ? ca_dir : "NULL");
      return 0;
    }
  }
  if (context->tls.ctx) {
    if (!SSL_CTX_load_verify_locations(context->tls.ctx, ca_file, ca_dir)) {
      coap_log(LOG_WARNING, "Unable to install root CAs (%s/%s)\n",
               ca_file ? ca_file : "NULL", ca_dir ? ca_dir : "NULL");
      return 0;
    }
  }
  return 1;
}

int
coap_dtls_context_check_keys_enabled(coap_context_t *ctx)
{
  coap_openssl_context_t *context =
                                 ((coap_openssl_context_t *)ctx->dtls_context);
  return context->psk_pki_enabled ? 1 : 0;
}


void coap_dtls_free_context(void *handle) {
  size_t i;
  coap_openssl_context_t *context = (coap_openssl_context_t *)handle;

  if (context->dtls.ssl)
    SSL_free(context->dtls.ssl);
  if (context->dtls.ctx)
    SSL_CTX_free(context->dtls.ctx);
  if (context->dtls.cookie_hmac)
    HMAC_CTX_free(context->dtls.cookie_hmac);
  if (context->dtls.meth)
    BIO_meth_free(context->dtls.meth);
  if (context->dtls.bio_addr)
    BIO_ADDR_free(context->dtls.bio_addr);
  if ( context->tls.ctx )
      SSL_CTX_free( context->tls.ctx );
  if ( context->tls.meth )
      BIO_meth_free( context->tls.meth );
  for (i = 0; i < context->sni_count; i++) {
    OPENSSL_free(context->sni_entry_list[i].sni);
#if OPENSSL_VERSION_NUMBER < 0x10101000L
    SSL_CTX_free(context->sni_entry_list[i].ctx);
#endif /* OPENSSL_VERSION_NUMBER < 0x10101000L */
  }
  if (context->sni_count)
    OPENSSL_free(context->sni_entry_list);
  coap_free(context);
}

void * coap_dtls_new_server_session(coap_session_t *session) {
  BIO *nbio = NULL;
  SSL *nssl = NULL, *ssl = NULL;
  coap_ssl_data *data;
  coap_dtls_context_t *dtls = &((coap_openssl_context_t *)session->context->dtls_context)->dtls;
  int r;

  nssl = SSL_new(dtls->ctx);
  if (!nssl)
    goto error;
  nbio = BIO_new(dtls->meth);
  if (!nbio)
    goto error;
  SSL_set_bio(nssl, nbio, nbio);
  SSL_set_app_data(nssl, NULL);
  SSL_set_options(nssl, SSL_OP_COOKIE_EXCHANGE);
  SSL_set_mtu(nssl, session->mtu);
  ssl = dtls->ssl;
  dtls->ssl = nssl;
  nssl = NULL;
  SSL_set_app_data(ssl, session);

  data = (coap_ssl_data*)BIO_get_data(SSL_get_rbio(ssl));
  data->session = session;

  if (session->context->get_server_hint) {
    char hint[128] = "";
    size_t hint_len = session->context->get_server_hint(session, (uint8_t*)hint, sizeof(hint) - 1);
    if (hint_len > 0 && hint_len < sizeof(hint)) {
      hint[hint_len] = 0;
      SSL_use_psk_identity_hint(ssl, hint);
    }
  }

  r = SSL_accept(ssl);
  if (r == -1) {
    int err = SSL_get_error(ssl, r);
    if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE)
      r = 0;
  }

  if (r == 0) {
    SSL_free(ssl);
    return NULL;
  }

  return ssl;

error:
  if (nssl)
    SSL_free(nssl);
  return NULL;
}

static int
setup_client_ssl_session(coap_session_t *session, SSL *ssl
) {
  coap_openssl_context_t *context = ((coap_openssl_context_t *)session->context->dtls_context);

  if (context->psk_pki_enabled & IS_PSK) {
    SSL_set_psk_client_callback(ssl, coap_dtls_psk_client_callback);
    SSL_set_psk_server_callback(ssl, coap_dtls_psk_server_callback);
    SSL_set_cipher_list(ssl, "PSK:!NULL");
  }
  if (context->psk_pki_enabled & IS_PKI) {
    coap_dtls_pki_t *setup_data = &context->setup_data;
    if (!setup_pki_ssl(ssl, setup_data, 0))
      return 0;
    /* libcoap is managing (D)TLS connection based on setup_data options */
    if (session->proto == COAP_PROTO_TLS)
      SSL_set_alpn_protos(ssl, coap_alpn, sizeof(coap_alpn));

    /* Issue SNI if requested */
    if (setup_data->client_sni &&
        SSL_set_tlsext_host_name (ssl, setup_data->client_sni) != 1) {
          coap_log(LOG_WARNING, "SSL_set_tlsext_host_name: set '%s' failed",
                   setup_data->client_sni);
    }
    /* Certificate Revocation */
    if (setup_data->check_cert_revocation) {
       X509_VERIFY_PARAM *param;

       param = X509_VERIFY_PARAM_new();
       X509_VERIFY_PARAM_set_flags(param, X509_V_FLAG_CRL_CHECK);
       SSL_set1_param(ssl, param);
       X509_VERIFY_PARAM_free(param);
    }

    /* Verify Peer */
    if (setup_data->verify_peer_cert)
      SSL_set_verify(ssl, SSL_VERIFY_PEER, tls_verify_call_back);
    else
      SSL_set_verify(ssl, SSL_VERIFY_NONE, NULL);

    /* Check CA Chain */
    if (setup_data->cert_chain_validation)
      SSL_set_verify_depth(ssl, setup_data->cert_chain_verify_depth);

  }
  return 1;
}

void *coap_dtls_new_client_session(coap_session_t *session) {
  BIO *bio = NULL;
  SSL *ssl = NULL;
  coap_ssl_data *data;
  int r;
  coap_openssl_context_t *context = ((coap_openssl_context_t *)session->context->dtls_context);
  coap_dtls_context_t *dtls = &context->dtls;

  ssl = SSL_new(dtls->ctx);
  if (!ssl)
    goto error;
  bio = BIO_new(dtls->meth);
  if (!bio)
    goto error;
  data = (coap_ssl_data *)BIO_get_data(bio);
  data->session = session;
  SSL_set_bio(ssl, bio, bio);
  SSL_set_app_data(ssl, session);
  SSL_set_options(ssl, SSL_OP_COOKIE_EXCHANGE);
  SSL_set_mtu(ssl, session->mtu);

  if (!setup_client_ssl_session(session, ssl))
    goto error;

  session->dtls_timeout_count = 0;

  r = SSL_connect(ssl);
  if (r == -1) {
    int ret = SSL_get_error(ssl, r);
    if (ret != SSL_ERROR_WANT_READ && ret != SSL_ERROR_WANT_WRITE)
      r = 0;
  }

  if (r == 0)
    goto error;

  return ssl;

error:
  if (ssl)
    SSL_free(ssl);
  return NULL;
}

void coap_dtls_session_update_mtu(coap_session_t *session) {
  SSL *ssl = (SSL *)session->tls;
  if (ssl)
    SSL_set_mtu(ssl, session->mtu);
}

void coap_dtls_free_session(coap_session_t *session) {
  SSL *ssl = (SSL *)session->tls;
  if (ssl) {
    if (!SSL_in_init(ssl) && !(SSL_get_shutdown(ssl) & SSL_SENT_SHUTDOWN)) {
      int r = SSL_shutdown(ssl);
      if (r == 0) r = SSL_shutdown(ssl);
    }
    SSL_free(ssl);
    session->tls = NULL;
  }
}

int coap_dtls_send(coap_session_t *session,
  const uint8_t *data, size_t data_len) {
  int r;
  SSL *ssl = (SSL *)session->tls;

  assert(ssl != NULL);

  session->dtls_event = -1;
  r = SSL_write(ssl, data, (int)data_len);

  if (r <= 0) {
    int err = SSL_get_error(ssl, r);
    if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
      r = 0;
    } else {
      coap_log(LOG_WARNING, "coap_dtls_send: cannot send PDU\n");
      if (err == SSL_ERROR_ZERO_RETURN)
        session->dtls_event = COAP_EVENT_DTLS_CLOSED;
      else if (err == SSL_ERROR_SSL)
        session->dtls_event = COAP_EVENT_DTLS_ERROR;
      r = -1;
    }
  }

  if (session->dtls_event >= 0) {
    coap_handle_event(session->context, session->dtls_event, session);
    if (session->dtls_event == COAP_EVENT_DTLS_ERROR ||
        session->dtls_event == COAP_EVENT_DTLS_CLOSED) {
      coap_session_disconnected(session, COAP_NACK_TLS_FAILED);
      r = -1;
    }
  }

  return r;
}

int coap_dtls_is_context_timeout(void) {
  return 0;
}

coap_tick_t coap_dtls_get_context_timeout(void *dtls_context) {
  (void)dtls_context;
  return 0;
}

coap_tick_t coap_dtls_get_timeout(coap_session_t *session) {
  SSL *ssl = (SSL *)session->tls;
  coap_ssl_data *ssl_data;

  assert(ssl != NULL);
  ssl_data = (coap_ssl_data*)BIO_get_data(SSL_get_rbio(ssl));
  return ssl_data->timeout;
}

void coap_dtls_handle_timeout(coap_session_t *session) {
  SSL *ssl = (SSL *)session->tls;

  assert(ssl != NULL);
  if (((session->state == COAP_SESSION_STATE_HANDSHAKE) &&
       (++session->dtls_timeout_count > session->max_retransmit)) ||
      (DTLSv1_handle_timeout(ssl) < 0)) {
    /* Too many retries */
    coap_session_disconnected(session, COAP_NACK_TLS_FAILED);
  }
}

int coap_dtls_hello(coap_session_t *session,
  const uint8_t *data, size_t data_len) {
  coap_dtls_context_t *dtls = &((coap_openssl_context_t *)session->context->dtls_context)->dtls;
  coap_ssl_data *ssl_data;
  int r;

  SSL_set_mtu(dtls->ssl, session->mtu);
  ssl_data = (coap_ssl_data*)BIO_get_data(SSL_get_rbio(dtls->ssl));
  ssl_data->session = session;
  ssl_data->pdu = data;
  ssl_data->pdu_len = (unsigned)data_len;
  r = DTLSv1_listen(dtls->ssl, dtls->bio_addr);
  if (r <= 0) {
    int err = SSL_get_error(dtls->ssl, r);
    if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
      /* Got a ClientHello, sent-out a VerifyRequest */
      r = 0;
    }
  } else {
    /* Got a valid answer to a VerifyRequest */
    r = 1;
  }

  return r;
}

int coap_dtls_receive(coap_session_t *session,
  const uint8_t *data, size_t data_len) {
  coap_ssl_data *ssl_data;
  SSL *ssl = (SSL *)session->tls;
  int r;

  assert(ssl != NULL);

  int in_init = SSL_in_init(ssl);
  uint8_t pdu[COAP_RXBUFFER_SIZE];
  ssl_data = (coap_ssl_data*)BIO_get_data(SSL_get_rbio(ssl));
  ssl_data->pdu = data;
  ssl_data->pdu_len = (unsigned)data_len;

  session->dtls_event = -1;
  r = SSL_read(ssl, pdu, (int)sizeof(pdu));
  if (r > 0) {
    return coap_handle_dgram(session->context, session, pdu, (size_t)r);
  } else {
    int err = SSL_get_error(ssl, r);
    if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
      if (in_init && SSL_is_init_finished(ssl)) {
        coap_handle_event(session->context, COAP_EVENT_DTLS_CONNECTED, session);
        coap_session_connected(session);
      }
      r = 0;
    } else {
      if (err == SSL_ERROR_ZERO_RETURN)        /* Got a close notify alert from the remote side */
        session->dtls_event = COAP_EVENT_DTLS_CLOSED;
      else if (err == SSL_ERROR_SSL)
        session->dtls_event = COAP_EVENT_DTLS_ERROR;
      r = -1;
    }
    if (session->dtls_event >= 0) {
      coap_handle_event(session->context, session->dtls_event, session);
      if (session->dtls_event == COAP_EVENT_DTLS_ERROR ||
          session->dtls_event == COAP_EVENT_DTLS_CLOSED) {
        coap_session_disconnected(session, COAP_NACK_TLS_FAILED);
        r = -1;
      }
    }
  }

  return r;
}

unsigned int coap_dtls_get_overhead(coap_session_t *session) {
  unsigned int overhead = 37;
  const SSL_CIPHER *s_ciph = NULL;
  if (session->tls != NULL)
    s_ciph = SSL_get_current_cipher(session->tls);
  if ( s_ciph ) {
    unsigned int ivlen, maclen, blocksize = 1, pad = 0;

    const EVP_CIPHER *e_ciph;
    const EVP_MD *e_md;
    char cipher[128];

    e_ciph = EVP_get_cipherbynid(SSL_CIPHER_get_cipher_nid(s_ciph));

    switch (EVP_CIPHER_mode(e_ciph)) {
    case EVP_CIPH_GCM_MODE:
      ivlen = EVP_GCM_TLS_EXPLICIT_IV_LEN;
      maclen = EVP_GCM_TLS_TAG_LEN;
      break;

    case EVP_CIPH_CCM_MODE:
      ivlen = EVP_CCM_TLS_EXPLICIT_IV_LEN;
      SSL_CIPHER_description(s_ciph, cipher, sizeof(cipher));
      if (strstr(cipher, "CCM8"))
        maclen = 8;
      else
        maclen = 16;
      break;

    case EVP_CIPH_CBC_MODE:
      e_md = EVP_get_digestbynid(SSL_CIPHER_get_digest_nid(s_ciph));
      blocksize = EVP_CIPHER_block_size(e_ciph);
      ivlen = EVP_CIPHER_iv_length(e_ciph);
      pad = 1;
      maclen = EVP_MD_size(e_md);
      break;

    case EVP_CIPH_STREAM_CIPHER:
      /* Seen with PSK-CHACHA20-POLY1305 */
      ivlen = 8;
      maclen = 8;
      break;

    default:
      SSL_CIPHER_description(s_ciph, cipher, sizeof(cipher));
      coap_log(LOG_WARNING, "Unknown overhead for DTLS with cipher %s\n",
               cipher);
      ivlen = 8;
      maclen = 16;
      break;
    }
    overhead = DTLS1_RT_HEADER_LENGTH + ivlen + maclen + blocksize - 1 + pad;
  }
  return overhead;
}

void *coap_tls_new_client_session(coap_session_t *session, int *connected) {
  BIO *bio = NULL;
  SSL *ssl = NULL;
  int r;
  coap_openssl_context_t *context = ((coap_openssl_context_t *)session->context->dtls_context);
  coap_tls_context_t *tls = &context->tls;

  *connected = 0;
  ssl = SSL_new(tls->ctx);
  if (!ssl)
    goto error;
  bio = BIO_new(tls->meth);
  if (!bio)
    goto error;
  BIO_set_data(bio, session);
  SSL_set_bio(ssl, bio, bio);
  SSL_set_app_data(ssl, session);

  if (!setup_client_ssl_session(session, ssl))
    return 0;

  r = SSL_connect(ssl);
  if (r == -1) {
    int ret = SSL_get_error(ssl, r);
    if (ret != SSL_ERROR_WANT_READ && ret != SSL_ERROR_WANT_WRITE)
      r = 0;
    if (ret == SSL_ERROR_WANT_READ)
      session->sock.flags |= COAP_SOCKET_WANT_READ;
    if (ret == SSL_ERROR_WANT_WRITE)
      session->sock.flags |= COAP_SOCKET_WANT_WRITE;
  }

  if (r == 0)
    goto error;

  *connected = SSL_is_init_finished(ssl);

  return ssl;

error:
  if (ssl)
    SSL_free(ssl);
  return NULL;
}

void *coap_tls_new_server_session(coap_session_t *session, int *connected) {
  BIO *bio = NULL;
  SSL *ssl = NULL;
  coap_tls_context_t *tls = &((coap_openssl_context_t *)session->context->dtls_context)->tls;
  int r;

  *connected = 0;
  ssl = SSL_new(tls->ctx);
  if (!ssl)
    goto error;
  bio = BIO_new(tls->meth);
  if (!bio)
    goto error;
  BIO_set_data(bio, session);
  SSL_set_bio(ssl, bio, bio);
  SSL_set_app_data(ssl, session);

  if (session->context->get_server_hint) {
    char hint[128] = "";
    size_t hint_len = session->context->get_server_hint(session, (uint8_t*)hint, sizeof(hint) - 1);
    if (hint_len > 0 && hint_len < sizeof(hint)) {
      hint[hint_len] = 0;
      SSL_use_psk_identity_hint(ssl, hint);
    }
  }

  r = SSL_accept(ssl);
  if (r == -1) {
    int err = SSL_get_error(ssl, r);
    if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE)
      r = 0;
    if (err == SSL_ERROR_WANT_READ)
      session->sock.flags |= COAP_SOCKET_WANT_READ;
    if (err == SSL_ERROR_WANT_WRITE)
      session->sock.flags |= COAP_SOCKET_WANT_WRITE;
  }

  if (r == 0)
    goto error;

  *connected = SSL_is_init_finished(ssl);

  return ssl;

error:
  if (ssl)
    SSL_free(ssl);
  return NULL;
}

void coap_tls_free_session(coap_session_t *session) {
  SSL *ssl = (SSL *)session->tls;
  if (ssl) {
    if (!SSL_in_init(ssl) && !(SSL_get_shutdown(ssl) & SSL_SENT_SHUTDOWN)) {
      int r = SSL_shutdown(ssl);
      if (r == 0) r = SSL_shutdown(ssl);
    }
    SSL_free(ssl);
    session->tls = NULL;
  }
}

ssize_t coap_tls_write(coap_session_t *session,
                       const uint8_t *data,
                       size_t data_len
) {
  SSL *ssl = (SSL *)session->tls;
  int r, in_init;

  if (ssl == NULL)
    return -1;

  in_init = !SSL_is_init_finished(ssl);
  session->dtls_event = -1;
  r = SSL_write(ssl, data, (int)data_len);

  if (r <= 0) {
    int err = SSL_get_error(ssl, r);
    if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
      if (in_init && SSL_is_init_finished(ssl)) {
        coap_handle_event(session->context, COAP_EVENT_DTLS_CONNECTED, session);
        coap_session_send_csm(session);
      }
      if (err == SSL_ERROR_WANT_READ)
        session->sock.flags |= COAP_SOCKET_WANT_READ;
      if (err == SSL_ERROR_WANT_WRITE)
        session->sock.flags |= COAP_SOCKET_WANT_WRITE;
      r = 0;
    } else {
      coap_log(LOG_WARNING, "***%s: coap_tls_write: cannot send PDU\n",
               coap_session_str(session));
      if (err == SSL_ERROR_ZERO_RETURN)
        session->dtls_event = COAP_EVENT_DTLS_CLOSED;
      else if (err == SSL_ERROR_SSL)
        session->dtls_event = COAP_EVENT_DTLS_ERROR;
      r = -1;
    }
  } else if (in_init && SSL_is_init_finished(ssl)) {
    coap_handle_event(session->context, COAP_EVENT_DTLS_CONNECTED, session);
    coap_session_send_csm(session);
  }

  if (session->dtls_event >= 0) {
    coap_handle_event(session->context, session->dtls_event, session);
    if (session->dtls_event == COAP_EVENT_DTLS_ERROR ||
        session->dtls_event == COAP_EVENT_DTLS_CLOSED) {
      coap_session_disconnected(session, COAP_NACK_TLS_FAILED);
      r = -1;
    }
  }

  return r;
}

ssize_t coap_tls_read(coap_session_t *session,
                      uint8_t *data,
                      size_t data_len
) {
  SSL *ssl = (SSL *)session->tls;
  int r, in_init;

  if (ssl == NULL)
    return -1;

  in_init = !SSL_is_init_finished(ssl);
  session->dtls_event = -1;
  r = SSL_read(ssl, data, (int)data_len);
  if (r <= 0) {
    int err = SSL_get_error(ssl, r);
    if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
      if (in_init && SSL_is_init_finished(ssl)) {
        coap_handle_event(session->context, COAP_EVENT_DTLS_CONNECTED, session);
        coap_session_send_csm(session);
      }
      if (err == SSL_ERROR_WANT_READ)
        session->sock.flags |= COAP_SOCKET_WANT_READ;
      if (err == SSL_ERROR_WANT_WRITE)
        session->sock.flags |= COAP_SOCKET_WANT_WRITE;
      r = 0;
    } else {
      if (err == SSL_ERROR_ZERO_RETURN)        /* Got a close notify alert from the remote side */
        session->dtls_event = COAP_EVENT_DTLS_CLOSED;
      else if (err == SSL_ERROR_SSL)
        session->dtls_event = COAP_EVENT_DTLS_ERROR;
      r = -1;
    }
  } else if (in_init && SSL_is_init_finished(ssl)) {
    coap_handle_event(session->context, COAP_EVENT_DTLS_CONNECTED, session);
    coap_session_send_csm(session);
  }

  if (session->dtls_event >= 0) {
    coap_handle_event(session->context, session->dtls_event, session);
    if (session->dtls_event == COAP_EVENT_DTLS_ERROR ||
        session->dtls_event == COAP_EVENT_DTLS_CLOSED) {
      coap_session_disconnected(session, COAP_NACK_TLS_FAILED);
      r = -1;
    }
  }

  return r;
}

#else /* !HAVE_OPENSSL */

#ifdef __clang__
/* Make compilers happy that do not like empty modules. As this function is
 * never used, we ignore -Wunused-function at the end of compiling this file
 */
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
static inline void dummy(void) {
}

#endif /* HAVE_OPENSSL */
