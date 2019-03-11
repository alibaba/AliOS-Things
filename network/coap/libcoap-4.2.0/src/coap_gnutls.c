/*
 * coap_gnutls.c -- GnuTLS Datagram Transport Layer Support for libcoap
 *
 * Copyright (C) 2017 Dag Bjorklund <dag.bjorklund@comsel.fi>
 * Copyright (C) 2018-2019 Jon Shallow <supjps-libcoap@jpshallow.com>
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */

/*
 * Naming used to prevent confusion between coap sessions, gnutls sessions etc.
 * when reading the code.
 *
 * c_context  A coap_context_t *
 * c_session  A coap_session_t *
 * g_context  A coap_gnutls_context_t * (held in c_context->dtls_context)
 * g_session  A gnutls_session_t (which has the * in the typedef)
 * g_env      A coap_gnutls_env_t * (held in c_session->tls)
 */

#include "coap_config.h"

#ifdef HAVE_LIBGNUTLS

#define MIN_GNUTLS_VERSION "3.3.0"

#include "net.h"
#include "mem.h"
#include "coap_debug.h"
#include "prng.h"
#include <inttypes.h>
#include <stdio.h>
#include <errno.h>
#include <gnutls/gnutls.h>
#include <gnutls/x509.h>
#include <gnutls/dtls.h>
#include <unistd.h>

#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else /* __GNUC__ */
#define UNUSED
#endif /* __GNUC__ */

typedef struct coap_ssl_t {
  const uint8_t *pdu;
  unsigned pdu_len;
  unsigned peekmode;
  coap_tick_t timeout;
} coap_ssl_t;

/*
 * This structure encapsulates the GnuTLS session object.
 * It handles both TLS and DTLS.
 * c_session->tls points to this.
 */
typedef struct coap_gnutls_env_t {
  gnutls_session_t g_session;
  gnutls_psk_client_credentials_t psk_cl_credentials;
  gnutls_psk_server_credentials_t psk_sv_credentials;
  gnutls_certificate_credentials_t pki_credentials;
  coap_ssl_t coap_ssl_data;
  /* If not set, need to do gnutls_handshake */
  int established;
  int seen_client_hello;
} coap_gnutls_env_t;

#define IS_PSK (1 << 0)
#define IS_PKI (1 << 1)
#define IS_CLIENT (1 << 6)
#define IS_SERVER (1 << 7)

typedef struct sni_entry {
  char *sni;
  coap_dtls_key_t pki_key;
  gnutls_certificate_credentials_t pki_credentials;
} sni_entry;

typedef struct coap_gnutls_context_t {
  coap_dtls_pki_t setup_data;
  int psk_pki_enabled;
  size_t sni_count;
  sni_entry *sni_entry_list;
  gnutls_datum_t alpn_proto;    /* Will be "coap", but that is a const */
  char *root_ca_file;
  char *root_ca_path;
  gnutls_priority_t priority_cache;
} coap_gnutls_context_t;

typedef enum coap_free_bye_t {
  COAP_FREE_BYE_AS_TCP,  /**< call gnutls_bye() with GNUTLS_SHUT_RDWR */
  COAP_FREE_BYE_AS_UDP,  /**< call gnutls_bye() with GNUTLS_SHUT_WR */
  COAP_FREE_BYE_NONE     /**< do not call gnutls_bye() */
} coap_free_bye_t;

#if (GNUTLS_VERSION_NUMBER >= 0x030505)
#define VARIANTS "NORMAL:+ECDHE-PSK:+PSK:+ECDHE-ECDSA:+AES-128-CCM-8"
#else
#define VARIANTS "NORMAL:+ECDHE-PSK:+PSK"
#endif

#define G_ACTION(xx) do { \
  ret = (xx); \
} while (ret == GNUTLS_E_AGAIN || ret == GNUTLS_E_INTERRUPTED)

#define G_CHECK(xx,func) do { \
  if ((ret = (xx)) < 0) { \
    coap_log(LOG_WARNING, "%s: '%s'\n", func, gnutls_strerror(ret)); \
    goto fail; \
  } \
} while (0)

#define G_ACTION_CHECK(xx,func) do { \
  G_ACTION(xx); \
  G_CHECK(xx, func); \
} while 0

static int dtls_log_level = 0;

static int post_client_hello_gnutls_pki(gnutls_session_t g_session);
static int post_client_hello_gnutls_psk(gnutls_session_t g_session);

/*
 * return 0 failed
 *        1 passed
 */
int
coap_dtls_is_supported(void) {
  if (gnutls_check_version(MIN_GNUTLS_VERSION) == NULL) {
    coap_log(LOG_ERR, "GnuTLS " MIN_GNUTLS_VERSION " or later is required\n");
    return 0;
  }
  return 1;
}

/*
 * return 0 failed
 *        1 passed
 */
int
coap_tls_is_supported(void) {
  if (gnutls_check_version(MIN_GNUTLS_VERSION) == NULL) {
    coap_log(LOG_ERR, "GnuTLS " MIN_GNUTLS_VERSION " or later is required\n");
    return 0;
  }
  return 1;
}

coap_tls_version_t *
coap_get_tls_library_version(void) {
  static coap_tls_version_t version;
  const char *vers = gnutls_check_version(NULL);

  version.version = 0;
  if (vers) {
    int p1, p2, p3;

    sscanf (vers, "%d.%d.%d", &p1, &p2, &p3);
    version.version = (p1 << 16) | (p2 << 8) | p3;
  }
  version.built_version = GNUTLS_VERSION_NUMBER;
  version.type = COAP_TLS_LIBRARY_GNUTLS;
  return &version;
}

static void
coap_gnutls_audit_log_func(gnutls_session_t g_session, const char* text)
{
  if (g_session) {
    coap_session_t *c_session =
      (coap_session_t *)gnutls_transport_get_ptr(g_session);
    coap_log(LOG_WARNING, "** %s: %s",
             coap_session_str(c_session), text);
  } else {
    coap_log(LOG_WARNING, "** (null): %s", text);
  }
}

static void
coap_gnutls_log_func(int level, const char* text)
{
  /* debug logging in gnutls starts at 2 */
  if (level > 2)
    level = 2;
  coap_log(LOG_DEBUG + level - 2, "%s", text);
}

/*
 * return 0 failed
 *        1 passed
 */
int
coap_dtls_context_set_pki(coap_context_t *c_context,
                          coap_dtls_pki_t* setup_data,
                          coap_dtls_role_t role UNUSED)
{
  coap_gnutls_context_t *g_context =
                         ((coap_gnutls_context_t *)c_context->dtls_context);

  if (!g_context || !setup_data)
    return 0;

  g_context->setup_data = *setup_data;
  g_context->psk_pki_enabled |= IS_PKI;
  return 1;
}

/*
 * return 0 failed
 *        1 passed
 */
int
coap_dtls_context_set_pki_root_cas(struct coap_context_t *c_context,
                                   const char *ca_file,
                                   const char *ca_path)
{
  coap_gnutls_context_t *g_context =
                         ((coap_gnutls_context_t *)c_context->dtls_context);
  if (!g_context) {
    coap_log(LOG_WARNING,
             "coap_context_set_pki_root_cas: (D)TLS environment "
             "not set up\n");
    return 0;
  }

  if (ca_file == NULL && ca_path == NULL) {
    coap_log(LOG_WARNING,
             "coap_context_set_pki_root_cas: ca_file and/or ca_path "
             "not defined\n");
    return 0;
  }
  if (g_context->root_ca_file) {
    gnutls_free(g_context->root_ca_file);
    g_context->root_ca_file = NULL;
  }
  if (ca_file) {
    g_context->root_ca_file = gnutls_strdup(ca_file);
  }
  if (g_context->root_ca_path) {
    gnutls_free(g_context->root_ca_path);
    g_context->root_ca_path = NULL;
  }
  if (ca_path) {
#if (GNUTLS_VERSION_NUMBER >= 0x030306)
    g_context->root_ca_path = gnutls_strdup(ca_path);
#else
    coap_log(LOG_ERR, "ca_path not supported in GnuTLS < 3.3.6\n");
#endif
  }
  return 1;
}

/*
 * return 0 failed
 *        1 passed
 */
int
coap_dtls_context_set_psk(coap_context_t *c_context,
                          const char *identity_hint UNUSED,
                          coap_dtls_role_t role UNUSED
) {
  coap_gnutls_context_t *g_context =
                         ((coap_gnutls_context_t *)c_context->dtls_context);

  g_context->psk_pki_enabled |= IS_PSK;
  return 1;
}

/*
 * return 0 failed
 *        1 passed
 */
int
coap_dtls_context_check_keys_enabled(coap_context_t *c_context)
{
  coap_gnutls_context_t *g_context =
                         ((coap_gnutls_context_t *)c_context->dtls_context);
  return g_context->psk_pki_enabled ? 1 : 0;
}

void coap_dtls_startup(void) {
  gnutls_global_set_audit_log_function(coap_gnutls_audit_log_func);
  gnutls_global_set_log_function(coap_gnutls_log_func);
}

void
coap_dtls_set_log_level(int level) {
  dtls_log_level = level;
  if (level - LOG_DEBUG >= -2) {
    /* debug logging in gnutls starts at 2 */
    gnutls_global_set_log_level(2 + level - LOG_DEBUG);
  }
  else {
    gnutls_global_set_log_level(0);
  }
}

/*
 * return current logging level
 */
int
coap_dtls_get_log_level(void) {
  return dtls_log_level;
}

/*
 * return +ve  new g_context
 *        NULL failure
 */
void *
coap_dtls_new_context(struct coap_context_t *c_context UNUSED) {
  const char *err;
  int ret;
  struct coap_gnutls_context_t *g_context =
                                (struct coap_gnutls_context_t *)
                                gnutls_malloc(sizeof(coap_gnutls_context_t));

  if (g_context) {
    G_CHECK(gnutls_global_init(), "gnutls_global_init");
    memset(g_context, 0, sizeof(struct coap_gnutls_context_t));
    g_context->alpn_proto.data = gnutls_malloc(4);
    if (g_context->alpn_proto.data) {
      memcpy(g_context->alpn_proto.data, "coap", 4);
      g_context->alpn_proto.size = 4;
    }
    G_CHECK(gnutls_priority_init(&g_context->priority_cache, VARIANTS, &err),
            "gnutls_priority_init");
  }
  return g_context;

fail:
  if (g_context)
    coap_dtls_free_context(g_context);
  return NULL;
}

void
coap_dtls_free_context(void *handle) {
  size_t i;
  coap_gnutls_context_t *g_context = (coap_gnutls_context_t *)handle;

  gnutls_free(g_context->alpn_proto.data);
  gnutls_free(g_context->root_ca_file);
  gnutls_free(g_context->root_ca_path);
  for (i = 0; i < g_context->sni_count; i++) {
    gnutls_free(g_context->sni_entry_list[i].sni);
    if (g_context->psk_pki_enabled & IS_PKI) {
      gnutls_certificate_free_credentials(
          g_context->sni_entry_list[i].pki_credentials);
    }
  }
  if (g_context->sni_count)
    gnutls_free(g_context->sni_entry_list);

  gnutls_priority_deinit(g_context->priority_cache);

  gnutls_global_deinit();
  gnutls_free(g_context);
}

/*
 * gnutls_psk_client_credentials_function return values
 * (see gnutls_psk_set_client_credentials_function())
 *
 * return -1 failed
 *         0 passed
 */
static int
psk_client_callback(gnutls_session_t g_session,
                    char **username, gnutls_datum_t *key) {
  coap_session_t *c_session =
                  (coap_session_t *)gnutls_transport_get_ptr(g_session);
  uint8_t identity[64];
  size_t identity_len;
  uint8_t psk_key[64];
  size_t psk_len;

  /* Constant passed to get_client_psk callback. The final byte is
   * reserved for a terminating 0. */
  const size_t max_identity_len = sizeof(identity) - 1;

  /* Initialize result parameters. */
  *username = NULL;
  key->data = NULL;

  if (c_session == NULL || c_session->context == NULL ||
      c_session->context->get_client_psk == NULL) {
    return -1;
  }

  psk_len = c_session->context->get_client_psk(c_session,
                                               NULL,
                                               0,
                                               identity,
                                               &identity_len,
                                               max_identity_len,
                                               psk_key,
                                               sizeof(psk_key));
  assert(identity_len < sizeof(identity));

  /* Reserve dynamic memory to hold the identity and a terminating
   * zero. */
  *username = gnutls_malloc(identity_len+1);
  if (*username) {
    memcpy(*username, identity, identity_len);
    *username[identity_len] = '\0';
  }

  key->data = gnutls_malloc(psk_len);
  if (key->data) {
    memcpy(key->data, psk_key, psk_len);
    key->size = psk_len;
  }

  return (*username && key->data) ? 0 : -1;
}

/*
 * return +ve  SAN or CN derived from certificate
 *        NULL failed
 */
static char* get_san_or_cn(gnutls_session_t g_session)
{
  unsigned int cert_list_size = 0;
  const gnutls_datum_t *cert_list;
  gnutls_x509_crt_t cert;
  char dn[256];
  size_t size;
  int n;
  char *cn;
  int ret;

  if (gnutls_certificate_type_get(g_session) != GNUTLS_CRT_X509)
    return NULL;

  cert_list = gnutls_certificate_get_peers(g_session, &cert_list_size);
  if (cert_list_size == 0) {
    return NULL;
  }

  G_CHECK(gnutls_x509_crt_init(&cert), "gnutls_x509_crt_init");

  /* Interested only in first cert in chain */
  G_CHECK(gnutls_x509_crt_import(cert, &cert_list[0], GNUTLS_X509_FMT_DER),
          "gnutls_x509_crt_import");

  size = sizeof(dn) -1;
  /* See if there is a Subject Alt Name first */
  ret = gnutls_x509_crt_get_subject_alt_name(cert, 0, dn, &size, NULL);
  if (ret >= 0) {
    dn[size] = '\000';
    gnutls_x509_crt_deinit(cert);
    return gnutls_strdup(dn);
  }

  size = sizeof(dn);
  G_CHECK(gnutls_x509_crt_get_dn(cert, dn, &size), "gnutls_x509_crt_get_dn");

  gnutls_x509_crt_deinit(cert);

  /* Need to emulate strcasestr() here.  Looking for CN= */
  n = strlen(dn) - 3;
  cn = dn;
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
    char *ecn = strchr(cn, ',');
    if (ecn) {
      cn[ecn-cn] = '\000';
    }
    return gnutls_strdup(cn);
  }
  return NULL;

fail:
  return NULL;
}

/*
 * return 0 failed
 *        1 passed
 */
static int cert_verify_gnutls(gnutls_session_t g_session)
{
  unsigned int status = 0;
  coap_session_t *c_session =
                (coap_session_t *)gnutls_transport_get_ptr(g_session);
  coap_gnutls_context_t *g_context =
             (coap_gnutls_context_t *)c_session->context->dtls_context;
  char *cn = NULL;
  int alert = GNUTLS_A_BAD_CERTIFICATE;
  int ret;

  G_CHECK(gnutls_certificate_verify_peers(g_session, NULL, 0, &status),
          "gnutls_certificate_verify_peers");

  cn = get_san_or_cn(g_session);

  if (status) {
    status &= ~(GNUTLS_CERT_INVALID);
    if (status & (GNUTLS_CERT_NOT_ACTIVATED|GNUTLS_CERT_EXPIRED)) {
      if (g_context->setup_data.allow_expired_certs) {
        status &= ~(GNUTLS_CERT_NOT_ACTIVATED|GNUTLS_CERT_EXPIRED);
        coap_log(LOG_WARNING,
                 "   %s: %s: overridden: '%s'\n",
                 coap_session_str(c_session),
                 "The certificate has an invalid usage date", cn ? cn : "?");
      }
    }
    if (status & (GNUTLS_CERT_REVOCATION_DATA_SUPERSEDED|
                  GNUTLS_CERT_REVOCATION_DATA_ISSUED_IN_FUTURE)) {
      if (g_context->setup_data.allow_expired_crl) {
        status &= ~(GNUTLS_CERT_REVOCATION_DATA_SUPERSEDED|
                    GNUTLS_CERT_REVOCATION_DATA_ISSUED_IN_FUTURE);
        coap_log(LOG_WARNING,
                 "   %s: %s: overridden: '%s'\n",
                 coap_session_str(c_session),
                 "The certificate's CRL entry has an invalid usage date",
                 cn ? cn : "?");
      }
    }

    if (status) {
        coap_log(LOG_WARNING,
                 "   %s: status 0x%x: '%s'\n",
                 coap_session_str(c_session),
                 status, cn ? cn : "?");
    }
  }

  if (status)
    goto fail;

  if (g_context->setup_data.validate_cn_call_back) {
    unsigned int cert_list_size = 0;
    const gnutls_datum_t *cert_list;
    gnutls_x509_crt_t cert;
    uint8_t der[2048];
    size_t size;
    /* status == 0 indicates that the certificate passed to
     *  setup_data.validate_cn_call_back has been validated. */
    const int cert_is_trusted = !status;

    cert_list = gnutls_certificate_get_peers(g_session, &cert_list_size);
    if (cert_list_size == 0) {
      /* get_san_or_cn() should have caught this */
      goto fail;
    }

    G_CHECK(gnutls_x509_crt_init(&cert), "gnutls_x509_crt_init");

    /* Interested only in first cert in chain */
    G_CHECK(gnutls_x509_crt_import(cert, &cert_list[0], GNUTLS_X509_FMT_DER),
            "gnutls_x509_crt_import");

    size = sizeof(der);
    G_CHECK(gnutls_x509_crt_export(cert, GNUTLS_X509_FMT_DER, der, &size),
            "gnutls_x509_crt_export");
    gnutls_x509_crt_deinit(cert);
    if (!g_context->setup_data.validate_cn_call_back(cn,
           der,
           size,
           c_session,
           0,
           cert_is_trusted,
           g_context->setup_data.cn_call_back_arg)) {
      alert = GNUTLS_A_ACCESS_DENIED;
      goto fail;
    }
  }

  if (g_context->setup_data.additional_tls_setup_call_back) {
    /* Additional application setup wanted */
    if (!g_context->setup_data.additional_tls_setup_call_back(g_session,
            &g_context->setup_data)) {
      goto fail;
    }
  }

  if (cn)
    gnutls_free(cn);

  return 1;

fail:
  if (cn)
    gnutls_free(cn);

  G_ACTION(gnutls_alert_send(g_session, GNUTLS_AL_FATAL, alert));
  return 0;
}

/*
 * gnutls_certificate_verify_function return values
 * (see gnutls_certificate_set_verify_function())
 *
 * return -1 failed
 *         0 passed
 */
static int cert_verify_callback_gnutls(gnutls_session_t g_session)
{
  int ret;

  if (gnutls_auth_get_type(g_session) == GNUTLS_CRD_CERTIFICATE) {
    if (cert_verify_gnutls(g_session) == 0) {
      G_ACTION(gnutls_alert_send(g_session,
                                 GNUTLS_AL_FATAL,
                                 GNUTLS_A_ACCESS_DENIED));
      return -1;
    }
  }
  return 0;
}

/*
 * return 0   Success (GNUTLS_E_SUCCESS)
 *        neg GNUTLS_E_* error code
 */
static int
setup_pki_credentials(gnutls_certificate_credentials_t *pki_credentials,
                      coap_gnutls_context_t *g_context,
                      coap_dtls_pki_t *setup_data, coap_dtls_role_t role)
{
  int ret;

  G_CHECK(gnutls_certificate_allocate_credentials(pki_credentials),
          "gnutls_certificate_allocate_credentials");

  switch (setup_data->pki_key.key_type) {
  case COAP_PKI_KEY_PEM:
    if (setup_data->pki_key.key.pem.public_cert &&
        setup_data->pki_key.key.pem.public_cert[0] &&
        setup_data->pki_key.key.pem.private_key &&
        setup_data->pki_key.key.pem.private_key[0]) {
      G_CHECK(gnutls_certificate_set_x509_key_file(*pki_credentials,
                                   setup_data->pki_key.key.pem.public_cert,
                                   setup_data->pki_key.key.pem.private_key,
                                   GNUTLS_X509_FMT_PEM),
                 "gnutls_certificate_set_x509_key_file");
    }
    else if (role == COAP_DTLS_ROLE_SERVER) {
      coap_log(LOG_ERR,
               "***setup_pki: (D)TLS: No %s Certificate + Private "
               "Key defined\n",
               role == COAP_DTLS_ROLE_SERVER ? "Server" : "Client");
      return GNUTLS_E_INSUFFICIENT_CREDENTIALS;
    }
    if (setup_data->pki_key.key.pem.ca_file &&
        setup_data->pki_key.key.pem.ca_file[0]) {
      G_CHECK(gnutls_certificate_set_x509_trust_file(*pki_credentials,
                           setup_data->pki_key.key.pem.ca_file,
                           GNUTLS_X509_FMT_PEM),
              "gnutls_certificate_set_x509_trust_file");
    }
    break;

  case COAP_PKI_KEY_ASN1:
    if (setup_data->pki_key.key.asn1.public_cert &&
        setup_data->pki_key.key.asn1.public_cert_len &&
        setup_data->pki_key.key.asn1.private_key &&
        setup_data->pki_key.key.asn1.private_key_len > 0) {
      gnutls_datum_t cert;
      gnutls_datum_t key;

      /* Kludge to get around const parameters */
      memcpy(&cert.data, &setup_data->pki_key.key.asn1.public_cert,
                         sizeof(cert.data));
      cert.size = setup_data->pki_key.key.asn1.public_cert_len;
      memcpy(&key.data, &setup_data->pki_key.key.asn1.private_key,
                        sizeof(key.data));
      key.size = setup_data->pki_key.key.asn1.private_key_len;
      G_CHECK(gnutls_certificate_set_x509_key_mem(*pki_credentials,
                           &cert,
                           &key,
                           GNUTLS_X509_FMT_DER),
              "gnutls_certificate_set_x509_key_mem");
    }
    else if (role == COAP_DTLS_ROLE_SERVER) {
      coap_log(LOG_ERR,
               "***setup_pki: (D)TLS: No %s Certificate + Private "
               "Key defined\n",
               role == COAP_DTLS_ROLE_SERVER ? "Server" : "Client");
      return GNUTLS_E_INSUFFICIENT_CREDENTIALS;
    }
    if (setup_data->pki_key.key.asn1.ca_cert &&
        setup_data->pki_key.key.asn1.ca_cert_len > 0) {
      gnutls_datum_t ca_cert;

      /* Kludge to get around const parameters */
      memcpy(&ca_cert.data, &setup_data->pki_key.key.asn1.ca_cert,
                            sizeof(ca_cert.data));
      ca_cert.size = setup_data->pki_key.key.asn1.ca_cert_len;
      G_CHECK(gnutls_certificate_set_x509_trust_mem(*pki_credentials,
                           &ca_cert,
                           GNUTLS_X509_FMT_DER),
              "gnutls_certificate_set_x509_trust_mem");
    }
    break;
  default:
    coap_log(LOG_ERR,
             "***setup_pki: (D)TLS: Unknown key type %d\n",
             setup_data->pki_key.key_type);
    return GNUTLS_E_INSUFFICIENT_CREDENTIALS;
  }

  if (g_context->root_ca_file) {
    G_CHECK(gnutls_certificate_set_x509_trust_file(*pki_credentials,
                         g_context->root_ca_file,
                         GNUTLS_X509_FMT_PEM),
            "gnutls_certificate_set_x509_trust_file");
  }
  if (g_context->root_ca_path) {
#if (GNUTLS_VERSION_NUMBER >= 0x030306)
    G_CHECK(gnutls_certificate_set_x509_trust_dir(*pki_credentials,
                         g_context->root_ca_path,
                         GNUTLS_X509_FMT_PEM),
            "gnutls_certificate_set_x509_trust_dir");
#endif
  }
  if (!(g_context->psk_pki_enabled & IS_PKI)) {
    /* No PKI defined at all - still need a trust set up for 3.6.0 or later */
    G_CHECK(gnutls_certificate_set_x509_system_trust(*pki_credentials),
            "gnutls_certificate_set_x509_system_trust");
  }

  /* Verify Peer */
  if (setup_data->verify_peer_cert) {
    gnutls_certificate_set_verify_function(*pki_credentials,
                                           cert_verify_callback_gnutls);
  }

  /* Cert chain checking (can raise GNUTLS_E_CONSTRAINT_ERROR) */
  if (setup_data->cert_chain_validation) {
    gnutls_certificate_set_verify_limits(*pki_credentials,
                                         0,
                                         setup_data->cert_chain_verify_depth);
  }

  /* Check for self signed */
  gnutls_certificate_set_verify_flags(*pki_credentials,
                                      GNUTLS_VERIFY_DO_NOT_ALLOW_SAME);

  /* CRL checking (can raise GNUTLS_CERT_MISSING_OCSP_STATUS) */
  if (setup_data->check_cert_revocation == 0) {
    gnutls_certificate_set_verify_flags(*pki_credentials,
                                        GNUTLS_VERIFY_DO_NOT_ALLOW_SAME |
                                        GNUTLS_VERIFY_DISABLE_CRL_CHECKS);
  }

  return GNUTLS_E_SUCCESS;

fail:
  return ret;
}

/*
 * return 0   Success (GNUTLS_E_SUCCESS)
 *        neg GNUTLS_E_* error code
 */
static int
post_client_hello_gnutls_pki(gnutls_session_t g_session)
{
  coap_session_t *c_session =
                (coap_session_t *)gnutls_transport_get_ptr(g_session);
  coap_gnutls_context_t *g_context =
             (coap_gnutls_context_t *)c_session->context->dtls_context;
  coap_gnutls_env_t *g_env = (coap_gnutls_env_t *)c_session->tls;
  int ret = GNUTLS_E_SUCCESS;
  char *name = NULL;

  g_env->seen_client_hello = 1;

  if (g_context->setup_data.validate_sni_call_back) {
    /* DNS names (only type supported) may be at most 256 byte long */
    size_t len = 256;
    unsigned int type;
    unsigned int i;
    coap_dtls_pki_t sni_setup_data;

    name = gnutls_malloc(len);
    if (name == NULL)
      return GNUTLS_E_MEMORY_ERROR;

    for (i=0; ; ) {
      ret = gnutls_server_name_get(g_session, name, &len, &type, i);
      if (ret == GNUTLS_E_SHORT_MEMORY_BUFFER) {
        char *new_name;
        new_name = gnutls_realloc(name, len);
        if (new_name == NULL) {
          ret = GNUTLS_E_MEMORY_ERROR;
          goto end;
        }
        name = new_name;
        continue; /* retry call with same index */
      }

      /* check if it is the last entry in list */
      if (ret == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
        break;
      i++;
      if (ret != GNUTLS_E_SUCCESS)
        goto end;
      /* unknown types need to be ignored */
      if (type != GNUTLS_NAME_DNS)
        continue;

    }
    /* If no extension provided, make it a dummy entry */
    if (i == 0) {
      name[0] = '\000';
      len = 0;
    }

    /* Is this a cached entry? */
    for (i = 0; i < g_context->sni_count; i++) {
      if (strcmp(name, g_context->sni_entry_list[i].sni) == 0) {
        break;
      }
    }
    if (i == g_context->sni_count) {
      /*
       * New SNI request
       */
      coap_dtls_key_t *new_entry =
        g_context->setup_data.validate_sni_call_back(name,
                                   g_context->setup_data.sni_call_back_arg);
      if (!new_entry) {
        G_ACTION(gnutls_alert_send(g_session, GNUTLS_AL_FATAL,
                                   GNUTLS_A_UNRECOGNIZED_NAME));
        ret = GNUTLS_E_NO_CERTIFICATE_FOUND;
        goto end;
      }

      g_context->sni_entry_list = gnutls_realloc(g_context->sni_entry_list,
                                     (i+1)*sizeof(sni_entry));
      g_context->sni_entry_list[i].sni = gnutls_strdup(name);
      g_context->sni_entry_list[i].pki_key = *new_entry;
      sni_setup_data = g_context->setup_data;
      sni_setup_data.pki_key = *new_entry;
      if ((ret = setup_pki_credentials(
                           &g_context->sni_entry_list[i].pki_credentials,
                           g_context,
                           &sni_setup_data, COAP_DTLS_ROLE_CLIENT)) < 0) {
        int keep_ret = ret;
        G_ACTION(gnutls_alert_send(g_session, GNUTLS_AL_FATAL,
                                   GNUTLS_A_BAD_CERTIFICATE));
        ret = keep_ret;
        goto end;
      }
      g_context->sni_count++;
    }
    G_CHECK(gnutls_credentials_set(g_env->g_session, GNUTLS_CRD_CERTIFICATE,
                               g_context->sni_entry_list[i].pki_credentials),
            "gnutls_credentials_set");
  }

end:
  free(name);
  return ret;

fail:
  return ret;
}

/*
 * return 0   Success (GNUTLS_E_SUCCESS)
 *        neg GNUTLS_E_* error code
 */
static int
post_client_hello_gnutls_psk(gnutls_session_t g_session)
{
  coap_session_t *c_session =
                (coap_session_t *)gnutls_transport_get_ptr(g_session);
  coap_gnutls_env_t *g_env = (coap_gnutls_env_t *)c_session->tls;

  g_env->seen_client_hello = 1;
  return GNUTLS_E_SUCCESS;
}

/*
 * return 0   Success (GNUTLS_E_SUCCESS)
 *        neg GNUTLS_E_* error code
 */
static int
setup_client_ssl_session(coap_session_t *c_session, coap_gnutls_env_t *g_env)
{
  coap_gnutls_context_t *g_context =
             (coap_gnutls_context_t *)c_session->context->dtls_context;
  int ret;

  g_context->psk_pki_enabled |= IS_CLIENT;
  if (g_context->psk_pki_enabled & IS_PSK) {
    char *identity = NULL;
    gnutls_datum_t psk_key;

    G_CHECK(gnutls_psk_allocate_client_credentials(&g_env->psk_cl_credentials),
            "gnutls_psk_allocate_client_credentials");
    psk_client_callback(g_env->g_session, &identity, &psk_key);
    G_CHECK(gnutls_psk_set_client_credentials(g_env->psk_cl_credentials,
                                              identity,
                                              &psk_key,
                                              GNUTLS_PSK_KEY_RAW),
            "gnutls_psk_set_client_credentials");
    G_CHECK(gnutls_credentials_set(g_env->g_session, GNUTLS_CRD_PSK,
                                   g_env->psk_cl_credentials),
            "gnutls_credentials_set");
    gnutls_free(identity);
    gnutls_free(psk_key.data);
  }

  if ((g_context->psk_pki_enabled & IS_PKI) ||
      (g_context->psk_pki_enabled & (IS_PSK | IS_PKI)) == 0) {
    /*
     * If neither PSK or PKI have been set up, use PKI basics.
     * This works providing COAP_PKI_KEY_PEM has a value of 0.
     */
    coap_dtls_pki_t *setup_data = &g_context->setup_data;
    G_CHECK(setup_pki_credentials(&g_env->pki_credentials, g_context,
                                  setup_data, COAP_DTLS_ROLE_CLIENT),
            "setup_pki_credentials");

    G_CHECK(gnutls_credentials_set(g_env->g_session, GNUTLS_CRD_CERTIFICATE,
                                   g_env->pki_credentials),
            "gnutls_credentials_set");

    if (c_session->proto == COAP_PROTO_TLS)
      G_CHECK(gnutls_alpn_set_protocols(g_env->g_session,
                                        &g_context->alpn_proto, 1, 0),
              "gnutls_alpn_set_protocols");

    /* Issue SNI if requested (only happens if PKI defined) */
    if (setup_data->client_sni) {
      G_CHECK(gnutls_server_name_set(g_env->g_session, GNUTLS_NAME_DNS,
                                     setup_data->client_sni,
                                     strlen(setup_data->client_sni)),
              "gnutls_server_name_set");
    }
  }
  return GNUTLS_E_SUCCESS;

fail:
  return ret;
}

/*
 * gnutls_psk_server_credentials_function return values
 * (see gnutls_psk_set_server_credentials_function())
 *
 * return -1 failed
 *         0 passed
 */
static int
psk_server_callback(gnutls_session_t g_session,
                    const char *identity,
                    gnutls_datum_t *key)
{
  coap_session_t *c_session =
                (coap_session_t *)gnutls_transport_get_ptr(g_session);
  size_t identity_len = 0;
  uint8_t buf[64];
  size_t psk_len;

  if (identity)
    identity_len = strlen(identity);
  else
    identity = "";

  coap_log(LOG_DEBUG, "got psk_identity: '%.*s'\n",
                      (int)identity_len, identity);

  if (c_session == NULL || c_session->context == NULL ||
      c_session->context->get_server_psk == NULL)
    return -1;

  psk_len = c_session->context->get_server_psk(c_session,
                               (const uint8_t*)identity,
                               identity_len,
                               (uint8_t*)buf, sizeof(buf));
  key->data = gnutls_malloc(psk_len);
  memcpy(key->data, buf, psk_len);
  key->size = psk_len;
  return 0;
}

/*
 * return 0   Success (GNUTLS_E_SUCCESS)
 *        neg GNUTLS_E_* error code
 */
static int
setup_server_ssl_session(coap_session_t *c_session, coap_gnutls_env_t *g_env)
{
  coap_gnutls_context_t *g_context =
             (coap_gnutls_context_t *)c_session->context->dtls_context;
  int ret = GNUTLS_E_SUCCESS;

  g_context->psk_pki_enabled |= IS_SERVER;
  if (g_context->psk_pki_enabled & IS_PSK) {
    G_CHECK(gnutls_psk_allocate_server_credentials(&g_env->psk_sv_credentials),
            "gnutls_psk_allocate_server_credentials");
    gnutls_psk_set_server_credentials_function(g_env->psk_sv_credentials,
                                                      psk_server_callback);

    gnutls_handshake_set_post_client_hello_function(g_env->g_session,
                                                 post_client_hello_gnutls_psk);

    G_CHECK(gnutls_credentials_set(g_env->g_session,
                                   GNUTLS_CRD_PSK,
                                   g_env->psk_sv_credentials),
            "gnutls_credentials_set\n");
  }

  if (g_context->psk_pki_enabled & IS_PKI) {
    coap_dtls_pki_t *setup_data = &g_context->setup_data;
    G_CHECK(setup_pki_credentials(&g_env->pki_credentials, g_context,
                                  setup_data, COAP_DTLS_ROLE_SERVER),
            "setup_pki_credentials");

    if (setup_data->require_peer_cert) {
      gnutls_certificate_server_set_request(g_env->g_session,
                                            GNUTLS_CERT_REQUIRE);
    }
    else {
      gnutls_certificate_server_set_request(g_env->g_session, GNUTLS_CERT_IGNORE);
    }

    gnutls_handshake_set_post_client_hello_function(g_env->g_session,
                                                 post_client_hello_gnutls_pki);

    G_CHECK(gnutls_credentials_set(g_env->g_session, GNUTLS_CRD_CERTIFICATE,
                                   g_env->pki_credentials),
            "gnutls_credentials_set\n");
  }
  return GNUTLS_E_SUCCESS;

fail:
  return ret;
}

/*
 * return +ve data amount
 *        0   no more
 *        -1  error (error in errno)
 */
static ssize_t
coap_dgram_read(gnutls_transport_ptr_t context, void *out, size_t outl)
{
  ssize_t ret = 0;
  coap_session_t *c_session = (struct coap_session_t *)context;
  coap_ssl_t *data = &((coap_gnutls_env_t *)c_session->tls)->coap_ssl_data;

  if (!c_session->tls) {
    errno = EAGAIN;
    return -1;
  }

  if (out != NULL) {
    if (data != NULL && data->pdu_len > 0) {
      if (outl < data->pdu_len) {
        memcpy(out, data->pdu, outl);
        ret = outl;
        data->pdu += outl;
        data->pdu_len -= outl;
      } else {
        memcpy(out, data->pdu, data->pdu_len);
        ret = data->pdu_len;
        if (!data->peekmode) {
          data->pdu_len = 0;
          data->pdu = NULL;
        }
      }
    }
    else {
      errno = EAGAIN;
      ret = -1;
    }
  }
  return ret;
}

/*
 * return +ve data amount
 *        0   no more
 *        -1  error (error in errno)
 */
/* callback function given to gnutls for sending data over socket */
static ssize_t
coap_dgram_write(gnutls_transport_ptr_t context, const void *send_buffer,
                  size_t send_buffer_length) {
  ssize_t result = -1;
  coap_session_t *c_session = (struct coap_session_t *)context;

  if (c_session) {
    result = coap_session_send(c_session, send_buffer, send_buffer_length);
    if (result != (int)send_buffer_length) {
      coap_log(LOG_WARNING, "coap_network_send failed\n");
      result = 0;
    }
  } else {
    result = 0;
  }
  return result;
}

/*
 * return 1  fd has activity
 *        0  timeout
 *        -1 error (error in errno)
 */
static int
receive_timeout(gnutls_transport_ptr_t context, unsigned int ms UNUSED) {
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
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    return select(nfds, &readfds, &writefds, &exceptfds, &tv);
  }
  return 1;
}

static coap_gnutls_env_t *
coap_dtls_new_gnutls_env(coap_session_t *c_session, int type)
{
  coap_gnutls_context_t *g_context =
          ((coap_gnutls_context_t *)c_session->context->dtls_context);
  coap_gnutls_env_t *g_env = (coap_gnutls_env_t *)c_session->tls;
  int flags = type | GNUTLS_DATAGRAM | GNUTLS_NONBLOCK;
  int ret;

  if (g_env)
    return g_env;

  g_env = gnutls_malloc(sizeof(coap_gnutls_env_t));
  if (!g_env)
    return NULL;

  memset(g_env, 0, sizeof(struct coap_gnutls_env_t));

  G_CHECK(gnutls_init(&g_env->g_session, flags), "gnutls_init");

  gnutls_transport_set_pull_function(g_env->g_session, coap_dgram_read);
  gnutls_transport_set_push_function(g_env->g_session, coap_dgram_write);
  gnutls_transport_set_pull_timeout_function(g_env->g_session, receive_timeout);
  /* So we can track the coap_session_t in callbacks */
  gnutls_transport_set_ptr(g_env->g_session, c_session);

  if (type == GNUTLS_SERVER) {
    G_CHECK(setup_server_ssl_session(c_session, g_env),
            "setup_server_ssl_session");
  }
  else {
    G_CHECK(setup_client_ssl_session(c_session, g_env),
            "setup_client_ssl_session");
  }

  G_CHECK(gnutls_priority_set(g_env->g_session, g_context->priority_cache),
          "gnutls_priority_set");
  gnutls_handshake_set_timeout(g_env->g_session,
                               GNUTLS_DEFAULT_HANDSHAKE_TIMEOUT);

  return g_env;

fail:
  if (g_env)
    gnutls_free(g_env);
  return NULL;
}

static void
coap_dtls_free_gnutls_env(coap_gnutls_context_t *g_context,
                          coap_gnutls_env_t *g_env,
                          coap_free_bye_t free_bye)
{
  if (g_env) {
    /* It is suggested not to use GNUTLS_SHUT_RDWR in DTLS
     * connections because the peer's closure message might
     * be lost */
    if (free_bye != COAP_FREE_BYE_NONE) {
      /* Only do this if appropriate */
      gnutls_bye(g_env->g_session, free_bye == COAP_FREE_BYE_AS_UDP ?
                                       GNUTLS_SHUT_WR : GNUTLS_SHUT_RDWR);
    }
    gnutls_deinit(g_env->g_session);
    g_env->g_session = NULL;
    if (g_context->psk_pki_enabled & IS_PSK) {
      if (g_context->psk_pki_enabled & IS_CLIENT) {
        gnutls_psk_free_client_credentials(g_env->psk_cl_credentials);
        g_env->psk_cl_credentials = NULL;
      }
      else {
        gnutls_psk_free_server_credentials(g_env->psk_sv_credentials);
        g_env->psk_sv_credentials = NULL;
      }
    }
    if (g_context->psk_pki_enabled & IS_PKI) {
      gnutls_certificate_free_credentials(g_env->pki_credentials);
      g_env->pki_credentials = NULL;
    }
    gnutls_free(g_env);
  }
}

void *coap_dtls_new_server_session(coap_session_t *c_session) {
  coap_gnutls_env_t *g_env =
         (coap_gnutls_env_t *)c_session->endpoint->hello.tls;

  gnutls_transport_set_ptr(g_env->g_session, c_session);
  /* For the next one */
  c_session->endpoint->hello.tls = NULL;

  return g_env;
}

static void log_last_alert(gnutls_session_t g_session) {
  int last_alert = gnutls_alert_get(g_session);

  coap_log(LOG_WARNING, "Received alert '%d': '%s'\n",
                        last_alert, gnutls_alert_get_name(last_alert));
}

/*
 * return -1  failure
 *         0  not completed
 *         1  established
 */
static int
do_gnutls_handshake(coap_session_t *c_session, coap_gnutls_env_t *g_env) {
  int ret;

  ret = gnutls_handshake(g_env->g_session);
  switch (ret) {
  case GNUTLS_E_SUCCESS:
    g_env->established = 1;
    coap_log(LOG_DEBUG, "*  %s: GnuTLS established\n",
                                            coap_session_str(c_session));
    ret = 1;
    break;
  case GNUTLS_E_INTERRUPTED:
    errno = EINTR;
    ret = 0;
    break;
  case GNUTLS_E_AGAIN:
    errno = EAGAIN;
    ret = 0;
    break;
  case GNUTLS_E_INSUFFICIENT_CREDENTIALS:
    coap_log(LOG_WARNING,
             "Insufficient credentials provided.\n");
    ret = -1;
    break;
  case GNUTLS_E_UNEXPECTED_HANDSHAKE_PACKET:
  case GNUTLS_E_FATAL_ALERT_RECEIVED:
    log_last_alert(g_env->g_session);
    c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
    ret = -1;
    break;
  case GNUTLS_E_WARNING_ALERT_RECEIVED:
    log_last_alert(g_env->g_session);
    c_session->dtls_event = COAP_EVENT_DTLS_ERROR;
    ret = 0;
    break;
  case GNUTLS_E_NO_CERTIFICATE_FOUND:
    coap_log(LOG_WARNING,
             "Client Certificate requested and required, but not provided\n"
             );
    G_ACTION(gnutls_alert_send(g_env->g_session, GNUTLS_AL_FATAL,
                                                 GNUTLS_A_BAD_CERTIFICATE));
    c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
    ret = -1;
    break;
  case GNUTLS_E_DECRYPTION_FAILED:
    coap_log(LOG_WARNING,
             "do_gnutls_handshake: session establish "
             "returned %d: '%s'\n",
             ret, gnutls_strerror(ret));
    G_ACTION(gnutls_alert_send(g_env->g_session, GNUTLS_AL_FATAL,
                                                 GNUTLS_A_DECRYPT_ERROR));
    c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
    ret = -1;
    break;
  case GNUTLS_E_UNKNOWN_CIPHER_SUITE:
  /* fall through */
  case GNUTLS_E_TIMEDOUT:
    c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
    ret = -1;
    break;
  default:
    coap_log(LOG_WARNING,
             "do_gnutls_handshake: session establish "
             "returned %d: '%s'\n",
             ret, gnutls_strerror(ret));
    ret = -1;
    break;
  }
  return ret;
}

void *coap_dtls_new_client_session(coap_session_t *c_session) {
  coap_gnutls_env_t *g_env = coap_dtls_new_gnutls_env(c_session, GNUTLS_CLIENT);
  int ret;

  if (g_env) {
    ret = do_gnutls_handshake(c_session, g_env);
    if (ret == -1) {
      coap_dtls_free_gnutls_env(c_session->context->dtls_context,
                                g_env,
                                COAP_PROTO_NOT_RELIABLE(c_session->proto) ?
                                 COAP_FREE_BYE_AS_UDP : COAP_FREE_BYE_AS_TCP);
      return NULL;
    }
  }
  return g_env;
}

void coap_dtls_free_session(coap_session_t *c_session) {
  if (c_session && c_session->context) {
    coap_dtls_free_gnutls_env(c_session->context->dtls_context,
                c_session->tls,
                COAP_PROTO_NOT_RELIABLE(c_session->proto) ?
                 COAP_FREE_BYE_AS_UDP : COAP_FREE_BYE_AS_TCP);
    c_session->tls = NULL;
  }
}

void coap_dtls_session_update_mtu(coap_session_t *c_session) {
  coap_gnutls_env_t *g_env = (coap_gnutls_env_t *)c_session->tls;
  int ret;

  if (g_env)
    G_CHECK(gnutls_dtls_set_data_mtu(g_env->g_session, c_session->mtu),
            "gnutls_dtls_set_data_mtu");
fail:
  ;;
}

/*
 * return +ve data amount
 *        0   no more
 *        -1  error
 */
int coap_dtls_send(coap_session_t *c_session,
  const uint8_t *data, size_t data_len) {
  int ret;
  coap_gnutls_env_t *g_env = (coap_gnutls_env_t *)c_session->tls;

  assert(g_env != NULL);

  c_session->dtls_event = -1;
  if (g_env->established) {
    ret = gnutls_record_send(g_env->g_session, data, data_len);

    if (ret <= 0) {
      switch (ret) {
      case GNUTLS_E_AGAIN:
        ret = 0;
        break;
      case GNUTLS_E_FATAL_ALERT_RECEIVED:
        log_last_alert(g_env->g_session);
        c_session->dtls_event = COAP_EVENT_DTLS_ERROR;
        ret = -1;
        break;
      default:
        ret = -1;
        break;
      }
      if (ret == -1) {
        coap_log(LOG_WARNING, "coap_dtls_send: cannot send PDU\n");
      }
    }
  }
  else {
    ret = do_gnutls_handshake(c_session, g_env);
    if (ret == 1) {
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

coap_tick_t coap_dtls_get_context_timeout(void *dtls_context UNUSED) {
  return 0;
}

coap_tick_t coap_dtls_get_timeout(coap_session_t *c_session UNUSED) {
  return 0;
}

void coap_dtls_handle_timeout(coap_session_t *c_session UNUSED) {
}

/*
 * return +ve data amount
 *        0   no more
 *        -1  error
 */
int
coap_dtls_receive(coap_session_t *c_session,
  const uint8_t *data,
  size_t data_len
) {
  coap_gnutls_env_t *g_env = (coap_gnutls_env_t *)c_session->tls;
  int ret = 0;
  coap_ssl_t *ssl_data = &g_env->coap_ssl_data;

  uint8_t pdu[COAP_RXBUFFER_SIZE];

  assert(g_env != NULL);

  if (ssl_data->pdu_len)
    coap_log(LOG_INFO, "** %s: Previous data not read %u bytes\n",
             coap_session_str(c_session), ssl_data->pdu_len);
  ssl_data->pdu = data;
  ssl_data->pdu_len = (unsigned)data_len;

  c_session->dtls_event = -1;
  if (g_env->established) {
    if (c_session->state == COAP_SESSION_STATE_HANDSHAKE) {
      coap_handle_event(c_session->context, COAP_EVENT_DTLS_CONNECTED,
                        c_session);
      gnutls_transport_set_ptr(g_env->g_session, c_session);
      coap_session_connected(c_session);
    }
    ret = gnutls_record_recv(g_env->g_session, pdu, (int)sizeof(pdu));
    if (ret > 0) {
      return coap_handle_dgram(c_session->context, c_session, pdu, (size_t)ret);
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
    ret = do_gnutls_handshake(c_session, g_env);
    if (ret == 1) {
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

#define DTLS_CT_HANDSHAKE          22
#define DTLS_HT_CLIENT_HELLO        1

/** Generic header structure of the DTLS record layer. */
typedef struct __attribute__((__packed__)) {
  uint8_t content_type;           /**< content type of the included message */
  uint16_t version;               /**< Protocol version */
  uint16_t epoch;                 /**< counter for cipher state changes */
  uint8_t sequence_number[6];     /**< sequence number */
  uint16_t length;                /**< length of the following fragment */
  uint8_t handshake;              /**< If content_type == DTLS_CT_HANDSHAKE */
} dtls_record_handshake_t;

#define OFF_CONTENT_TYPE 0     /* offset of content_type in dtls_record_handshake_t */
#define OFF_HANDSHAKE_TYPE 13  /* offset of handshake in dtls_record_handshake_t */

/*
 * return 0 failed
 *        1 passed
 */
int
coap_dtls_hello(coap_session_t *c_session,
  const uint8_t *data,
  size_t data_len
) {
  coap_gnutls_env_t *g_env = (coap_gnutls_env_t *)c_session->tls;
  coap_ssl_t *ssl_data = g_env ? &g_env->coap_ssl_data : NULL;
  int ret;

  if (!g_env) {
    /*
     * Need to check that this actually is a Client Hello before wasting
     * time allocating and then freeing off g_env.
     */
    if (data_len < (OFF_HANDSHAKE_TYPE + 1)) {
      coap_log(LOG_DEBUG,
         "coap_dtls_hello: ContentType %d Short Packet (%ld < %d) dropped\n",
         data[OFF_CONTENT_TYPE], data_len, OFF_HANDSHAKE_TYPE + 1);
      return 0;
    }
    if (data[OFF_CONTENT_TYPE] != DTLS_CT_HANDSHAKE ||
        data[OFF_HANDSHAKE_TYPE] != DTLS_HT_CLIENT_HELLO) {
      coap_log(LOG_DEBUG,
         "coap_dtls_hello: ContentType %d Handshake %d dropped\n",
         data[OFF_CONTENT_TYPE], data[OFF_HANDSHAKE_TYPE]);
      return 0;
    }

    g_env = coap_dtls_new_gnutls_env(c_session, GNUTLS_SERVER);
    if (g_env) {
      c_session->tls = g_env;
      ssl_data = &g_env->coap_ssl_data;
      ssl_data->pdu = data;
      ssl_data->pdu_len = (unsigned)data_len;
      gnutls_dtls_set_data_mtu(g_env->g_session, c_session->mtu);
      ret = do_gnutls_handshake(c_session, g_env);
      if (ret == 1 || g_env->seen_client_hello) {
        /* The test for seen_client_hello gives the ability to setup a new
           coap_session to continue the gnutls_handshake past the client hello
           and safely allow updating of the g_env & g_session and separately
           letting a new session cleanly start up using endpoint->hello.
         */
        g_env->seen_client_hello = 0;
        return 1;
      }
      /*
       * as the above failed, need to remove g_env to clean up any
       * pollution of the information
       */
      coap_dtls_free_gnutls_env(
              ((coap_gnutls_context_t *)c_session->context->dtls_context),
              g_env, COAP_FREE_BYE_NONE);
      c_session->tls = NULL;
    }
    return 0;
  }

  ssl_data->pdu = data;
  ssl_data->pdu_len = (unsigned)data_len;

  ret = do_gnutls_handshake(c_session, g_env);
  if (ret == 1 || g_env->seen_client_hello) {
    /* The test for seen_client_hello gives the ability to setup a new
       coap_session to continue the gnutls_handshake past the client hello
       and safely allow updating of the g_env & g_session and separately
       letting a new session cleanly start up using endpoint->hello.
     */
    g_env->seen_client_hello = 0;
    return 1;
  }
  return 0;
}

unsigned int coap_dtls_get_overhead(coap_session_t *c_session UNUSED) {
  return 37;
}

/*
 * return +ve data amount
 *        0   no more
 *        -1  error (error in errno)
 */
static ssize_t
coap_sock_read(gnutls_transport_ptr_t context, void *out, size_t outl) {
  int ret = 0;
  coap_session_t *c_session = (struct coap_session_t *)context;

  if (out != NULL) {
#ifdef _WIN32
    ret = recv(c_session->sock.fd, (char *)out, (int)outl, 0);
#else
    ret = recv(c_session->sock.fd, out, outl, 0);
#endif
    if (ret == 0) {
      /* graceful shutdown */
      c_session->sock.flags &= ~COAP_SOCKET_CAN_READ;
      return 0;
    } else if (ret == COAP_SOCKET_ERROR)
      c_session->sock.flags &= ~COAP_SOCKET_CAN_READ;
    else if (ret < (ssize_t)outl)
      c_session->sock.flags &= ~COAP_SOCKET_CAN_READ;
    return ret;
  }
  return ret;
}

/*
 * return +ve data amount
 *        0   no more
 *        -1  error (error in errno)
 */
static ssize_t
coap_sock_write(gnutls_transport_ptr_t context, const void *in, size_t inl) {
  int ret = 0;
  coap_session_t *c_session = (struct coap_session_t *)context;

  ret = (int)coap_socket_write(&c_session->sock, in, inl);
  if (ret == 0) {
    errno = EAGAIN;
    ret = -1;
  }
  return ret;
}

void *coap_tls_new_client_session(coap_session_t *c_session, int *connected) {
  coap_gnutls_env_t *g_env = gnutls_malloc(sizeof(coap_gnutls_env_t));
  coap_gnutls_context_t *g_context =
                ((coap_gnutls_context_t *)c_session->context->dtls_context);
  int flags = GNUTLS_CLIENT;
  int ret;

  if (!g_env) {
    return NULL;
  }
  memset(g_env, 0, sizeof(struct coap_gnutls_env_t));

  *connected = 0;
  G_CHECK(gnutls_init(&g_env->g_session, flags), "gnutls_init");

  gnutls_transport_set_pull_function(g_env->g_session, coap_sock_read);
  gnutls_transport_set_push_function(g_env->g_session, coap_sock_write);
  gnutls_transport_set_pull_timeout_function(g_env->g_session, receive_timeout);
  /* So we can track the coap_session_t in callbacks */
  gnutls_transport_set_ptr(g_env->g_session, c_session);

  setup_client_ssl_session(c_session, g_env);

  gnutls_priority_set(g_env->g_session, g_context->priority_cache);
  gnutls_handshake_set_timeout(g_env->g_session, GNUTLS_DEFAULT_HANDSHAKE_TIMEOUT);

  ret = do_gnutls_handshake(c_session, g_env);
  if (ret == 1) {
    *connected = 1;
    coap_handle_event(c_session->context, COAP_EVENT_DTLS_CONNECTED, c_session);
    coap_session_connected(c_session);
  }
  return g_env;

fail:
  if (g_env)
    gnutls_free(g_env);
  return NULL;
}

void *coap_tls_new_server_session(coap_session_t *c_session, int *connected) {
  coap_gnutls_env_t *g_env = gnutls_malloc(sizeof(coap_gnutls_env_t));
  coap_gnutls_context_t *g_context =
             ((coap_gnutls_context_t *)c_session->context->dtls_context);
  int flags = GNUTLS_SERVER;
  int ret;

  if (!g_env)
    return NULL;
  memset(g_env, 0, sizeof(struct coap_gnutls_env_t));

  *connected = 0;
  G_CHECK(gnutls_init(&g_env->g_session, flags), "gnutls_init");

  gnutls_transport_set_pull_function(g_env->g_session, coap_sock_read);
  gnutls_transport_set_push_function(g_env->g_session, coap_sock_write);
  gnutls_transport_set_pull_timeout_function(g_env->g_session, receive_timeout);
  /* So we can track the coap_session_t in callbacks */
  gnutls_transport_set_ptr(g_env->g_session, c_session);

  setup_server_ssl_session(c_session, g_env);

  gnutls_priority_set(g_env->g_session, g_context->priority_cache);
  gnutls_handshake_set_timeout(g_env->g_session,
                                     GNUTLS_DEFAULT_HANDSHAKE_TIMEOUT);

  c_session->tls = g_env;
  ret = do_gnutls_handshake(c_session, g_env);
  if (ret == 1) {
    *connected = 1;
  }
  return g_env;

fail:
  return NULL;
}

void coap_tls_free_session(coap_session_t *c_session) {
  coap_dtls_free_session(c_session);
  return;
}

/*
 * return +ve data amount
 *        0   no more
 *        -1  error (error in errno)
 */
ssize_t coap_tls_write(coap_session_t *c_session,
                       const uint8_t *data,
                       size_t data_len
) {
  int ret;
  coap_gnutls_env_t *g_env = (coap_gnutls_env_t *)c_session->tls;

  assert(g_env != NULL);

  c_session->dtls_event = -1;
  if (g_env->established) {
    ret = gnutls_record_send(g_env->g_session, data, data_len);

    if (ret <= 0) {
      switch (ret) {
      case GNUTLS_E_AGAIN:
        ret = 0;
        break;
      case GNUTLS_E_FATAL_ALERT_RECEIVED:
        log_last_alert(g_env->g_session);
        c_session->dtls_event = COAP_EVENT_DTLS_ERROR;
        ret = -1;
        break;
      default:
        coap_log(LOG_WARNING,
                 "coap_tls_write: gnutls_record_send "
                 "returned %d: '%s'\n",
                 ret, gnutls_strerror(ret));
        ret = -1;
        break;
      }
      if (ret == -1) {
        coap_log(LOG_WARNING, "coap_dtls_send: cannot send PDU\n");
      }
    }
  }
  else {
    ret = do_gnutls_handshake(c_session, g_env);
    if (ret == 1) {
      coap_handle_event(c_session->context, COAP_EVENT_DTLS_CONNECTED,
                                     c_session);
      coap_session_send_csm(c_session);
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

/*
 * return +ve data amount
 *        0   no more
 *        -1  error (error in errno)
 */
ssize_t coap_tls_read(coap_session_t *c_session,
                      uint8_t *data,
                      size_t data_len
) {
  coap_gnutls_env_t *g_env = (coap_gnutls_env_t *)c_session->tls;
  int ret;

  if (!g_env)
    return -1;

  c_session->dtls_event = -1;
  if (!g_env->established) {
    ret = do_gnutls_handshake(c_session, g_env);
    if (ret == 1) {
      coap_handle_event(c_session->context, COAP_EVENT_DTLS_CONNECTED,
                                                               c_session);
      coap_session_send_csm(c_session);
    }
  }
  if (g_env->established) {
    ret = gnutls_record_recv(g_env->g_session, data, (int)data_len);
    if (ret <= 0) {
      switch (ret) {
      case 0:
        c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
        break;
      case GNUTLS_E_AGAIN:
        errno = EAGAIN;
        ret = 0;
        break;
      case GNUTLS_E_PULL_ERROR:
        c_session->dtls_event = COAP_EVENT_DTLS_ERROR;
        break;
      default:
        coap_log(LOG_WARNING,
                 "coap_tls_read: gnutls_record_recv "
                 "returned %d: '%s'\n",
                 ret, gnutls_strerror(ret));
        ret = -1;
        break;
      }
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

#else /* !HAVE_LIBGNUTLS */

#ifdef __clang__
/* Make compilers happy that do not like empty modules. As this function is
 * never used, we ignore -Wunused-function at the end of compiling this file
 */
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
static inline void dummy(void) {
}

#endif /* !HAVE_LIBGNUTLS */
