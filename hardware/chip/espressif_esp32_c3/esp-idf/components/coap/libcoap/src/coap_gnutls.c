/*
 * coap_gnutls.c -- GnuTLS Datagram Transport Layer Support for libcoap
 *
 * Copyright (C) 2017 Dag Bjorklund <dag.bjorklund@comsel.fi>
 * Copyright (C) 2018-2021 Jon Shallow <supjps-libcoap@jpshallow.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
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

/*
 * Notes
 *
 * There is a memory leak in GnuTLS prior to 3.3.26 when hint is not freed off
 * when server psk credentials are freed off.
 *
 * ca_path in coap_dtls_context_set_pki_root_cas() is not supported until 3.3.6
 *
 * Identity Hint is not provided if using DH and versions prior to 3.4.4
 *
 * 3.5.5 or later is required to interoperate with TinyDTLS as CCM algorithm
 * support is required.
 *
 * TLS 1.3 is properly supported from 3.6.5 onwards
 * (but is not enabled by default in 3.6.4)
 *
 * Starting with 3.6.3, fixed in 3.6.13, Client Hellos may fail with some
 * server implementations (e.g. Californium) as random value is all zeros
 * - CVE-2020-11501 - a security weakness.
 * 3.6.6 or later is required to support Raw Public Key(RPK)
 */

#include "coap3/coap_internal.h"

#ifdef HAVE_LIBGNUTLS

#define MIN_GNUTLS_VERSION "3.3.0"

#include <inttypes.h>
#include <stdio.h>
#include <errno.h>
#include <gnutls/gnutls.h>
#include <gnutls/x509.h>
#include <gnutls/dtls.h>
#include <gnutls/pkcs11.h>
#include <gnutls/crypto.h>
#include <gnutls/abstract.h>
#include <unistd.h>
#if (GNUTLS_VERSION_NUMBER >= 0x030606)
#define COAP_GNUTLS_KEY_RPK GNUTLS_KEY_DIGITAL_SIGNATURE | \
                            GNUTLS_KEY_NON_REPUDIATION | \
                            GNUTLS_KEY_KEY_ENCIPHERMENT | \
                            GNUTLS_KEY_DATA_ENCIPHERMENT | \
                            GNUTLS_KEY_KEY_AGREEMENT | \
                            GNUTLS_KEY_KEY_CERT_SIGN
#endif /* GNUTLS_VERSION_NUMBER >= 0x030606 */

#ifndef GNUTLS_CRT_RAW
#define GNUTLS_CRT_RAW GNUTLS_CRT_RAWPK
#endif /* GNUTLS_CRT_RAW */

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

typedef struct coap_ssl_t {
  const uint8_t *pdu;
  unsigned pdu_len;
  unsigned peekmode;
  gnutls_datum_t cookie_key;
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
  int doing_dtls_timeout;
  coap_tick_t last_timeout;
  int sent_alert;
} coap_gnutls_env_t;

#define IS_PSK (1 << 0)
#define IS_PKI (1 << 1)
#define IS_CLIENT (1 << 6)
#define IS_SERVER (1 << 7)

typedef struct pki_sni_entry {
  char *sni;
  coap_dtls_key_t pki_key;
  gnutls_certificate_credentials_t pki_credentials;
} pki_sni_entry;

typedef struct psk_sni_entry {
  char *sni;
  coap_dtls_spsk_info_t psk_info;
  gnutls_psk_server_credentials_t psk_credentials;
} psk_sni_entry;

typedef struct coap_gnutls_context_t {
  coap_dtls_pki_t setup_data;
  int psk_pki_enabled;
  size_t pki_sni_count;
  pki_sni_entry *pki_sni_entry_list;
  size_t psk_sni_count;
  psk_sni_entry *psk_sni_entry_list;
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

#define VARIANTS_3_6_6 "NORMAL:+ECDHE-PSK:+PSK:+ECDHE-ECDSA:+AES-128-CCM-8:+CTYPE-CLI-ALL:+CTYPE-SRV-ALL:+SHA256"
#define VARIANTS_3_5_5 "NORMAL:+ECDHE-PSK:+PSK:+ECDHE-ECDSA:+AES-128-CCM-8"
#define VARIANTS_BASE "NORMAL:+ECDHE-PSK:+PSK"

#define VARIANTS_NO_TLS13_3_6_6 VARIANTS_3_6_6 ":-VERS-TLS1.3"
#define VARIANTS_NO_TLS13_3_6_4 VARIANTS_3_5_5 ":-VERS-TLS1.3"

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
static int psk_server_callback(gnutls_session_t g_session,
                               const char *identity,
                               gnutls_datum_t *key);

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
#if !COAP_DISABLE_TCP
  if (gnutls_check_version(MIN_GNUTLS_VERSION) == NULL) {
    coap_log(LOG_ERR, "GnuTLS " MIN_GNUTLS_VERSION " or later is required\n");
    return 0;
  }
  return 1;
#else /* COAP_DISABLE_TCP */
  return 0;
#endif /* COAP_DISABLE_TCP */
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
                          const coap_dtls_pki_t* setup_data,
                          const coap_dtls_role_t role COAP_UNUSED)
{
  coap_gnutls_context_t *g_context =
                         ((coap_gnutls_context_t *)c_context->dtls_context);

  if (!g_context || !setup_data)
    return 0;

  g_context->setup_data = *setup_data;
  if (!g_context->setup_data.verify_peer_cert) {
    /* Needs to be clear so that no CA DNs are transmitted */
    g_context->setup_data.check_common_ca = 0;
    if (g_context->setup_data.is_rpk_not_cert) {
      /* Disable all of these as they cannot be checked */
      g_context->setup_data.allow_self_signed = 0;
      g_context->setup_data.allow_expired_certs = 0;
      g_context->setup_data.cert_chain_validation = 0;
      g_context->setup_data.cert_chain_verify_depth = 0;
      g_context->setup_data.check_cert_revocation = 0;
      g_context->setup_data.allow_no_crl = 0;
      g_context->setup_data.allow_expired_crl = 0;
      g_context->setup_data.allow_bad_md_hash = 0;
      g_context->setup_data.allow_short_rsa_length = 0;
    }
    else {
      /* Allow all of these but warn if issue */
      g_context->setup_data.allow_self_signed = 1;
      g_context->setup_data.allow_expired_certs = 1;
      g_context->setup_data.cert_chain_validation = 1;
      g_context->setup_data.cert_chain_verify_depth = 10;
      g_context->setup_data.check_cert_revocation = 1;
      g_context->setup_data.allow_no_crl = 1;
      g_context->setup_data.allow_expired_crl = 1;
      g_context->setup_data.allow_bad_md_hash = 1;
      g_context->setup_data.allow_short_rsa_length = 1;
    }
  }
  g_context->psk_pki_enabled |= IS_PKI;
  return 1;
}

/*
 * return 0 failed
 *        1 passed
 */
int
coap_dtls_context_set_pki_root_cas(coap_context_t *c_context,
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
coap_dtls_context_set_spsk(coap_context_t *c_context,
                              coap_dtls_spsk_t *setup_data
) {
  coap_gnutls_context_t *g_context =
                         ((coap_gnutls_context_t *)c_context->dtls_context);

  if (!g_context || !setup_data)
    return 0;

  g_context->psk_pki_enabled |= IS_PSK;
  return 1;
}

/*
 * return 0 failed
 *        1 passed
 */
int
coap_dtls_context_set_cpsk(coap_context_t *c_context,
                          coap_dtls_cpsk_t *setup_data
) {
  coap_gnutls_context_t *g_context =
                         ((coap_gnutls_context_t *)c_context->dtls_context);

  if (!g_context || !setup_data)
    return 0;

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

void coap_dtls_shutdown(void) {
}

void *
coap_dtls_get_tls(const coap_session_t *c_session,
                  coap_tls_library_t *tls_lib) {
  if (tls_lib)
    *tls_lib = COAP_TLS_LIBRARY_GNUTLS;
  if (c_session && c_session->tls) {
    const coap_gnutls_env_t *g_env = (const coap_gnutls_env_t *)c_session->tls;

    return g_env->g_session;
  }
  return NULL;
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
coap_dtls_new_context(coap_context_t *c_context COAP_UNUSED) {
  const char *err;
  int ret;
  coap_gnutls_context_t *g_context =
                                (coap_gnutls_context_t *)
                                gnutls_malloc(sizeof(coap_gnutls_context_t));

  if (g_context) {
    coap_tls_version_t* tls_version = coap_get_tls_library_version();
    const char *priority;

    G_CHECK(gnutls_global_init(), "gnutls_global_init");
    memset(g_context, 0, sizeof(coap_gnutls_context_t));
    g_context->alpn_proto.data = gnutls_malloc(4);
    if (g_context->alpn_proto.data) {
      memcpy(g_context->alpn_proto.data, "coap", 4);
      g_context->alpn_proto.size = 4;
    }

    if (tls_version->version >= 0x030606) {
      priority = VARIANTS_3_6_6;
    }
    else if (tls_version->version >= 0x030505) {
      priority = VARIANTS_3_5_5;
    }
    else {
      priority = VARIANTS_BASE;
    }
    ret = gnutls_priority_init(&g_context->priority_cache, priority, &err);
    if (ret != GNUTLS_E_SUCCESS) {
      if (ret == GNUTLS_E_INVALID_REQUEST)
        coap_log(LOG_WARNING,
                 "gnutls_priority_init: Syntax error at: %s\n", err);
      else
        coap_log(LOG_WARNING,
                 "gnutls_priority_init: %s\n", gnutls_strerror(ret));
      goto fail;
    }
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
  for (i = 0; i < g_context->pki_sni_count; i++) {
    gnutls_free(g_context->pki_sni_entry_list[i].sni);
    gnutls_certificate_free_credentials(
        g_context->pki_sni_entry_list[i].pki_credentials);
  }
  if (g_context->pki_sni_entry_list)
    gnutls_free(g_context->pki_sni_entry_list);

  for (i = 0; i < g_context->psk_sni_count; i++) {
    gnutls_free(g_context->psk_sni_entry_list[i].sni);
    /* YUK - A memory leak in 3.3.0 (fixed by 3.3.26) of hint */
    gnutls_psk_free_server_credentials(
          g_context->psk_sni_entry_list[i].psk_credentials);
  }
  if (g_context->psk_sni_entry_list)
    gnutls_free(g_context->psk_sni_entry_list);

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
  coap_gnutls_context_t *g_context;
  coap_dtls_cpsk_t *setup_data;
  uint8_t identity[COAP_DTLS_MAX_PSK_IDENTITY];
  size_t identity_len;
  uint8_t psk[COAP_DTLS_MAX_PSK];
  size_t psk_len;
  const char *hint = gnutls_psk_client_get_hint(g_session);
  size_t hint_len = 0;

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

  g_context = (coap_gnutls_context_t *)c_session->context->dtls_context;
  if (g_context == NULL)
    return -1;

  setup_data = &c_session->cpsk_setup_data;

  if (hint)
    hint_len = strlen(hint);
  else
    hint = "";

  coap_log(LOG_DEBUG, "got psk_identity_hint: '%.*s'\n", (int)hint_len, hint);

  if (setup_data->validate_ih_call_back) {
    coap_str_const_t lhint;
    lhint.length = hint_len;
    lhint.s = (const uint8_t*)hint;
    const coap_dtls_cpsk_info_t *psk_info =
             setup_data->validate_ih_call_back(&lhint,
                                               c_session,
                                               setup_data->ih_call_back_arg);

    if (psk_info == NULL)
      return -1;

    *username = gnutls_malloc(psk_info->identity.length+1);
    if (*username == NULL)
      return -1;
    memcpy(*username, psk_info->identity.s, psk_info->identity.length);
    (*username)[psk_info->identity.length] = '\000';

    key->data = gnutls_malloc(psk_info->key.length);
    if (key->data == NULL) {
      gnutls_free(*username);
      *username = NULL;
      return -1;
    }
    memcpy(key->data, psk_info->key.s, psk_info->key.length);
    key->size = psk_info->key.length;
    return 0;
  }

  psk_len = c_session->context->get_client_psk(c_session,
                                               NULL,
                                               0,
                                               identity,
                                               &identity_len,
                                               max_identity_len,
                                               psk,
                                               sizeof(psk));
  assert(identity_len < sizeof(identity));

  /* Reserve dynamic memory to hold the identity and a terminating
   * zero. */
  *username = gnutls_malloc(identity_len+1);
  if (*username == NULL)
    return -1;
  memcpy(*username, identity, identity_len);
  (*username)[identity_len] = '\0';

  key->data = gnutls_malloc(psk_len);
  if (key->data == NULL) {
    gnutls_free(*username);
    *username = NULL;
    return -1;
  }
  memcpy(key->data, psk, psk_len);
  key->size = psk_len;

  return 0;
}

typedef struct {
  gnutls_certificate_type_t certificate_type;
  char *san_or_cn;
  const gnutls_datum_t *cert_list;
  unsigned int cert_list_size;
  int self_signed; /* 1 if cert self-signed, 0 otherwise */
} coap_gnutls_certificate_info_t;

/*
 * return Type of certificate and SAN or CN if appropriate derived from
 *        certificate. GNUTLS_CRT_UNKNOWN if failure.
 */
static gnutls_certificate_type_t get_san_or_cn(gnutls_session_t g_session,
                                     coap_gnutls_certificate_info_t *cert_info)
{
  gnutls_x509_crt_t cert;
  char dn[256];
  size_t size;
  int n;
  char *cn;
  int ret;

#if (GNUTLS_VERSION_NUMBER >= 0x030606)
  cert_info->certificate_type = gnutls_certificate_type_get2(g_session,
                                                           GNUTLS_CTYPE_PEERS);
#else /* < 3.6.6 */
  cert_info->certificate_type = gnutls_certificate_type_get(g_session);
#endif /* < 3.6.6 */

  cert_info->san_or_cn = NULL;

  cert_info->cert_list = gnutls_certificate_get_peers(g_session,
                                                   &cert_info->cert_list_size);
  if (cert_info->cert_list_size == 0) {
    return GNUTLS_CRT_UNKNOWN;
  }

  if (cert_info->certificate_type != GNUTLS_CRT_X509)
    return cert_info->certificate_type;

  G_CHECK(gnutls_x509_crt_init(&cert), "gnutls_x509_crt_init");

  /* Interested only in first cert in chain */
  G_CHECK(gnutls_x509_crt_import(cert, &cert_info->cert_list[0],
          GNUTLS_X509_FMT_DER), "gnutls_x509_crt_import");

  cert_info->self_signed = gnutls_x509_crt_check_issuer(cert, cert);

  size = sizeof(dn) -1;
  /* See if there is a Subject Alt Name first */
  ret = gnutls_x509_crt_get_subject_alt_name(cert, 0, dn, &size, NULL);
  if (ret >= 0) {
    dn[size] = '\000';
    gnutls_x509_crt_deinit(cert);
    cert_info->san_or_cn = gnutls_strdup(dn);
    return cert_info->certificate_type;
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
    cert_info->san_or_cn = gnutls_strdup(cn);
    return cert_info->certificate_type;
  }
  return GNUTLS_CRT_UNKNOWN;

fail:
  return GNUTLS_CRT_UNKNOWN;
}

#if (GNUTLS_VERSION_NUMBER >= 0x030606)
#define OUTPUT_CERT_NAME (cert_type == GNUTLS_CRT_X509 ? \
                          cert_info.san_or_cn : \
                          cert_type == GNUTLS_CRT_RAW ? \
                          COAP_DTLS_RPK_CERT_CN : "?")
#else /* GNUTLS_VERSION_NUMBER < 0x030606 */
#define OUTPUT_CERT_NAME (cert_type == GNUTLS_CRT_X509 ? \
                          cert_info.san_or_cn : "?")
#endif /* GNUTLS_VERSION_NUMBER < 0x030606 */

#if (GNUTLS_VERSION_NUMBER >= 0x030606)
static int
check_rpk_cert(coap_gnutls_context_t *g_context,
               coap_gnutls_certificate_info_t *cert_info,
               coap_session_t *c_session) {
  int ret;

  if (g_context->setup_data.validate_cn_call_back) {
    gnutls_pcert_st pcert;
    uint8_t der[2048];
    size_t size;

    G_CHECK(gnutls_pcert_import_rawpk_raw(&pcert, &cert_info->cert_list[0],
                                          GNUTLS_X509_FMT_DER, 0, 0),
            "gnutls_pcert_import_rawpk_raw");

    size = sizeof(der);
    G_CHECK(gnutls_pubkey_export(pcert.pubkey, GNUTLS_X509_FMT_DER, der, &size),
            "gnutls_pubkey_export");
    gnutls_pcert_deinit(&pcert);
    if (!g_context->setup_data.validate_cn_call_back(COAP_DTLS_RPK_CERT_CN,
           der,
           size,
           c_session,
           0,
           1,
           g_context->setup_data.cn_call_back_arg)) {
      return 0;
    }
  }
  return 1;
fail:
  return 0;
}
#endif /* >= 3.6.6 */

/*
 * return 0 failed
 *        1 passed
 */
static int cert_verify_gnutls(gnutls_session_t g_session)
{
  unsigned int status = 0;
  unsigned int fail = 0;
  coap_session_t *c_session =
                (coap_session_t *)gnutls_transport_get_ptr(g_session);
  coap_gnutls_context_t *g_context =
             (coap_gnutls_context_t *)c_session->context->dtls_context;
  coap_gnutls_env_t *g_env = (coap_gnutls_env_t *)c_session->tls;
  int alert = GNUTLS_A_BAD_CERTIFICATE;
  int ret;
  coap_gnutls_certificate_info_t cert_info;
  gnutls_certificate_type_t cert_type;

  memset(&cert_info, 0, sizeof(cert_info));
  cert_type = get_san_or_cn(g_session, &cert_info);
#if (GNUTLS_VERSION_NUMBER >= 0x030606)
  if (cert_type == GNUTLS_CRT_RAW) {
    if (!check_rpk_cert(g_context, &cert_info, c_session)) {
      alert = GNUTLS_A_ACCESS_DENIED;
      goto fail;
    }
    goto ok;
  }
#endif /* >= 3.6.6 */

  if (cert_info.cert_list_size == 0 && !g_context->setup_data.verify_peer_cert)
    goto ok;

  G_CHECK(gnutls_certificate_verify_peers(g_session, NULL, 0, &status),
          "gnutls_certificate_verify_peers");

  if (status) {
    status &= ~(GNUTLS_CERT_INVALID);
    if (status & (GNUTLS_CERT_NOT_ACTIVATED|GNUTLS_CERT_EXPIRED)) {
      status &= ~(GNUTLS_CERT_NOT_ACTIVATED|GNUTLS_CERT_EXPIRED);
      if (g_context->setup_data.allow_expired_certs) {
        coap_log(LOG_INFO,
                 "   %s: %s: overridden: '%s'\n",
                 coap_session_str(c_session),
                 "The certificate has an invalid usage date",
                 OUTPUT_CERT_NAME);
      }
      else {
        fail = 1;
        coap_log(LOG_WARNING,
                 "   %s: %s: '%s'\n",
                 coap_session_str(c_session),
                 "The certificate has an invalid usage date",
                 OUTPUT_CERT_NAME);
      }
    }
    if (status & (GNUTLS_CERT_REVOCATION_DATA_SUPERSEDED|
                  GNUTLS_CERT_REVOCATION_DATA_ISSUED_IN_FUTURE)) {
      status &= ~(GNUTLS_CERT_REVOCATION_DATA_SUPERSEDED|
                  GNUTLS_CERT_REVOCATION_DATA_ISSUED_IN_FUTURE);
      if (g_context->setup_data.allow_expired_crl) {
        coap_log(LOG_INFO,
                 "   %s: %s: overridden: '%s'\n",
                 coap_session_str(c_session),
                 "The certificate's CRL entry has an invalid usage date",
                 OUTPUT_CERT_NAME);
      }
      else {
        fail = 1;
        coap_log(LOG_WARNING,
                 "   %s: %s: '%s'\n",
                 coap_session_str(c_session),
                 "The certificate's CRL entry has an invalid usage date",
                 OUTPUT_CERT_NAME);
      }
    }
    if (status & (GNUTLS_CERT_SIGNER_NOT_FOUND)) {
      status &= ~(GNUTLS_CERT_SIGNER_NOT_FOUND);
      if (cert_info.self_signed) {
        if (g_context->setup_data.allow_self_signed &&
            !g_context->setup_data.check_common_ca) {
          coap_log(LOG_INFO,
                   "   %s: %s: overridden: '%s'\n",
                   coap_session_str(c_session),
                   "Self-signed",
                   OUTPUT_CERT_NAME);
        }
        else {
          fail = 1;
          alert = GNUTLS_A_UNKNOWN_CA;
          coap_log(LOG_WARNING,
                   "   %s: %s: '%s'\n",
                   coap_session_str(c_session),
                   "Self-signed",
                   OUTPUT_CERT_NAME);
        }
      }
      else {
        if (!g_context->setup_data.verify_peer_cert) {
          coap_log(LOG_INFO,
                   "   %s: %s: overridden: '%s'\n",
                   coap_session_str(c_session),
                   "The peer certificate's CA is unknown",
                   OUTPUT_CERT_NAME);
        }
        else {
          fail = 1;
          alert = GNUTLS_A_UNKNOWN_CA;
          coap_log(LOG_WARNING,
                   "   %s: %s: '%s'\n",
                   coap_session_str(c_session),
                   "The peer certificate's CA is unknown",
                   OUTPUT_CERT_NAME);
        }
      }
    }

    if (status) {
        fail = 1;
        coap_log(LOG_WARNING,
                 "   %s: gnutls_certificate_verify_peers() status 0x%x: '%s'\n",
                 coap_session_str(c_session),
                 status, OUTPUT_CERT_NAME);
    }
  }

  if (fail)
    goto fail;

  if (g_context->setup_data.validate_cn_call_back) {
    gnutls_x509_crt_t cert;
    uint8_t der[2048];
    size_t size;
    /* status == 0 indicates that the certificate passed to
     *  setup_data.validate_cn_call_back has been validated. */
    const int cert_is_trusted = !status;

    G_CHECK(gnutls_x509_crt_init(&cert), "gnutls_x509_crt_init");

    /* Interested only in first cert in chain */
    G_CHECK(gnutls_x509_crt_import(cert, &cert_info.cert_list[0],
            GNUTLS_X509_FMT_DER), "gnutls_x509_crt_import");

    size = sizeof(der);
    G_CHECK(gnutls_x509_crt_export(cert, GNUTLS_X509_FMT_DER, der, &size),
            "gnutls_x509_crt_export");
    gnutls_x509_crt_deinit(cert);
    if (!g_context->setup_data.validate_cn_call_back(OUTPUT_CERT_NAME,
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

ok:
  if (cert_info.san_or_cn)
    gnutls_free(cert_info.san_or_cn);

  return 1;

fail:
  if (cert_info.san_or_cn)
    gnutls_free(cert_info.san_or_cn);

  if (!g_env->sent_alert) {
    G_ACTION(gnutls_alert_send(g_session, GNUTLS_AL_FATAL, alert));
    g_env->sent_alert = 1;
  }
  c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
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
  if (gnutls_auth_get_type(g_session) == GNUTLS_CRD_CERTIFICATE) {
    if (cert_verify_gnutls(g_session) == 0) {
      return -1;
    }
  }
  return 0;
}

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

static int
pin_callback(void *user_data, int attempt,
             const char *token_url COAP_UNUSED,
             const char *token_label COAP_UNUSED,
             unsigned int flags COAP_UNUSED,
             char *pin,
             size_t pin_max)
{
  coap_dtls_pki_t *setup_data = (coap_dtls_pki_t *)user_data;

  /* Only do this on first attempt to prevent token lockout */
  if (attempt == 0 && setup_data && setup_data->pki_key.key.pkcs11.user_pin) {
    int len = min(pin_max - 1, strlen(setup_data->pki_key.key.pkcs11.user_pin));
    memcpy(pin, setup_data->pki_key.key.pkcs11.user_pin, len);
    pin[len] = 0;
    return 0;
  }
  return -1;
}
#if (GNUTLS_VERSION_NUMBER >= 0x030606)
/* first part of Raw public key, this is the start of the Subject Public Key */
static const unsigned char cert_asn1_header1[] = {
  0x30, 0x59, /* SEQUENCE, length 89 bytes */
    0x30, 0x13, /* SEQUENCE, length 19 bytes */
      0x06, 0x07, /* OBJECT IDENTIFIER ecPublicKey (1 2 840 10045 2 1) */
        0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01,
};
/* PrimeX will get inserted */
#if 0
      0x06, 0x08, /* OBJECT IDENTIFIER prime256v1 (1 2 840 10045 3 1 7) */
        0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07,
#endif
static const unsigned char cert_asn1_header2[] = {
      0x03, 0x42, /* BIT STRING, length 66 bytes */
/* Note: 0 bits (0x00) and no compression (0x04) are already in the certificate */
};

static gnutls_datum_t *
get_asn1_spki(const uint8_t *data, size_t size)
{
  coap_binary_t *pub_key = get_asn1_tag(COAP_ASN1_BITSTRING, data, size, NULL);
  coap_binary_t *prime = get_asn1_tag(COAP_ASN1_IDENTIFIER, data, size, NULL);
  gnutls_datum_t *spki = NULL;

  if (pub_key && prime) {
    size_t header_size = sizeof(cert_asn1_header1) +
                         2 +
                         prime->length +
                         sizeof(cert_asn1_header2);
    uint8_t *tmp = gnutls_malloc(sizeof(gnutls_datum_t) +
                                 header_size +
                                 pub_key->length);

    if (tmp) {
      spki = (gnutls_datum_t *)tmp;
      spki->data = &tmp[sizeof(gnutls_datum_t)];
      memcpy(&spki->data[header_size], pub_key->s, pub_key->length);
      memcpy(spki->data, cert_asn1_header1, sizeof(cert_asn1_header1));
      spki->data[sizeof(cert_asn1_header1)] = COAP_ASN1_IDENTIFIER;
      spki->data[sizeof(cert_asn1_header1)+1] = prime->length;
      memcpy(&spki->data[sizeof(cert_asn1_header1)+2],
             prime->s, prime->length);
      memcpy(&spki->data[sizeof(cert_asn1_header1)+2+prime->length],
             cert_asn1_header2, sizeof(cert_asn1_header2));
      spki->size = header_size + pub_key->length;
    }
  }
  if (pub_key) coap_delete_binary(pub_key);
  if (prime) coap_delete_binary(prime);
  return spki;
}
#endif /* GNUTLS_VERSION_NUMBER >= 0x030606 */

/*
 * return 0   Success (GNUTLS_E_SUCCESS)
 *        neg GNUTLS_E_* error code
 */
static int
setup_pki_credentials(gnutls_certificate_credentials_t *pki_credentials,
                      gnutls_session_t g_session,
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
      if (setup_data->is_rpk_not_cert) {
        coap_log(LOG_WARNING,
                 "RPK keys cannot be in COAP_PKI_KEY_PEM format\n");
        return GNUTLS_E_INSUFFICIENT_CREDENTIALS;
      }
      else {
        G_CHECK(gnutls_certificate_set_x509_key_file(*pki_credentials,
                                   setup_data->pki_key.key.pem.public_cert,
                                   setup_data->pki_key.key.pem.private_key,
                                   GNUTLS_X509_FMT_PEM),
                 "gnutls_certificate_set_x509_key_file");
      }
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
      ret = gnutls_certificate_set_x509_trust_file(*pki_credentials,
                           setup_data->pki_key.key.pem.ca_file,
                           GNUTLS_X509_FMT_PEM);
      if (ret == 0) {
        coap_log(LOG_WARNING,
         "gnutls_certificate_set_x509_trust_file: No certificates found\n");
      }
      else if (ret < 0) {
        coap_log(LOG_WARNING, "%s: '%s'\n",
                 "gnutls_certificate_set_x509_trust_file",
                 gnutls_strerror(ret));
        goto fail;
      }
    }
    break;

  case COAP_PKI_KEY_PEM_BUF:
    if (setup_data->pki_key.key.pem_buf.public_cert &&
        setup_data->pki_key.key.pem_buf.public_cert_len &&
        setup_data->pki_key.key.pem_buf.private_key &&
        setup_data->pki_key.key.pem_buf.private_key_len) {
      gnutls_datum_t cert;
      gnutls_datum_t key;
      int alloced_cert_memory = 0;
      int alloced_key_memory = 0;

      cert.size = setup_data->pki_key.key.pem_buf.public_cert_len;
      if (setup_data->pki_key.key.pem_buf.public_cert[cert.size-1] != '\000') {
        /* Need to allocate memory, rather than just copying pointers across */
        alloced_cert_memory = 1;
        cert.data = gnutls_malloc(cert.size + 1);
        if (!cert.data) {
          coap_log(LOG_ERR, "gnutls_malloc failure\n");
          return GNUTLS_E_MEMORY_ERROR;
        }
        memcpy(cert.data, setup_data->pki_key.key.pem_buf.public_cert,
               cert.size);
        cert.data[cert.size] = '\000';
        cert.size++;
      }
      else {
        /* To get around const issue */
        memcpy(&cert.data,
             &setup_data->pki_key.key.pem_buf.public_cert, sizeof(cert.data));
      }
      key.size = setup_data->pki_key.key.pem_buf.private_key_len;
      if (setup_data->pki_key.key.pem_buf.private_key[key.size-1] != '\000') {
        /* Need to allocate memory, rather than just copying pointers across */
        alloced_key_memory = 1;
        key.data = gnutls_malloc(key.size + 1);
        if (!key.data) {
          coap_log(LOG_ERR, "gnutls_malloc failure\n");
          if (alloced_cert_memory) gnutls_free(cert.data);
          return GNUTLS_E_MEMORY_ERROR;
        }
        memcpy(key.data, setup_data->pki_key.key.pem_buf.private_key, key.size);
        key.data[key.size] = '\000';
        key.size++;
      }
      else {
        /* To get around const issue */
        memcpy(&key.data,
               &setup_data->pki_key.key.pem_buf.private_key, sizeof(key.data));
      }
      if (setup_data->is_rpk_not_cert) {
#if (GNUTLS_VERSION_NUMBER >= 0x030606)
        int have_done_key = 0;
        if (strstr ((char*)key.data, "-----BEGIN EC PRIVATE KEY-----")) {
          gnutls_datum_t der_private;

          if (gnutls_pem_base64_decode2("EC PRIVATE KEY", &key,
                                        &der_private) == 0) {
            gnutls_datum_t *spki = get_asn1_spki(der_private.data,
                                                der_private.size);

            if (spki) {
              ret = gnutls_certificate_set_rawpk_key_mem(*pki_credentials,
                                         spki,
                                         &der_private,
                                         GNUTLS_X509_FMT_DER, NULL,
                                         COAP_GNUTLS_KEY_RPK,
                                         NULL, 0, 0);
              if (ret >= 0) {
                have_done_key = 1;
              }
              gnutls_free(spki);
            }
            gnutls_free(der_private.data);
          }
        }
        if (!have_done_key) {
          G_CHECK(gnutls_certificate_set_rawpk_key_mem(*pki_credentials,
                                     &cert,
                                     &key,
                                     GNUTLS_X509_FMT_PEM, NULL,
                                     COAP_GNUTLS_KEY_RPK,
                                     NULL, 0, 0),
                   "gnutls_certificate_set_rawpk_key_mem");
        }
#else /* GNUTLS_VERSION_NUMBER < 0x030606 */
        coap_log(LOG_ERR,
              "RPK Support not available (needs gnutls 3.6.6 or later)\n");
        if (alloced_cert_memory) gnutls_free(cert.data);
        if (alloced_key_memory) gnutls_free(key.data);
        return GNUTLS_E_INSUFFICIENT_CREDENTIALS;
#endif /* GNUTLS_VERSION_NUMBER < 0x030606 */
      }
      else {
        G_CHECK(gnutls_certificate_set_x509_key_mem(*pki_credentials,
                                   &cert,
                                   &key,
                                   GNUTLS_X509_FMT_PEM),
                 "gnutls_certificate_set_x509_key_mem");
      }
      if (alloced_cert_memory) gnutls_free(cert.data);
      if (alloced_key_memory) gnutls_free(key.data);
    }
    else if (role == COAP_DTLS_ROLE_SERVER) {
      coap_log(LOG_ERR,
               "***setup_pki: (D)TLS: No Server Certificate + Private "
               "Key defined\n");
      return GNUTLS_E_INSUFFICIENT_CREDENTIALS;
    }
    if (setup_data->pki_key.key.pem_buf.ca_cert &&
        setup_data->pki_key.key.pem_buf.ca_cert_len) {
      gnutls_datum_t ca;
      int alloced_ca_memory = 0;

      ca.size = setup_data->pki_key.key.pem_buf.ca_cert_len;
      if (setup_data->pki_key.key.pem_buf.ca_cert[ca.size-1] != '\000') {
        /* Need to allocate memory, rather than just copying pointers across */
        alloced_ca_memory = 1;
        ca.data = gnutls_malloc(ca.size + 1);
        if (!ca.data) {
          coap_log(LOG_ERR, "gnutls_malloc failure\n");
          return GNUTLS_E_MEMORY_ERROR;
        }
        memcpy(ca.data, setup_data->pki_key.key.pem_buf.ca_cert, ca.size);
        ca.data[ca.size] = '\000';
        ca.size++;
      }
      else {
        /* To get around const issue */
        memcpy(&ca.data,
               &setup_data->pki_key.key.pem_buf.ca_cert, sizeof(ca.data));
      }
      ret = gnutls_certificate_set_x509_trust_mem(*pki_credentials,
                           &ca,
                           GNUTLS_X509_FMT_PEM);
      if (ret == 0) {
        coap_log(LOG_WARNING,
         "gnutls_certificate_set_x509_trust_mem: No certificates found\n");
      }
      else if (ret < 0) {
        coap_log(LOG_WARNING, "%s: '%s'\n",
                 "gnutls_certificate_set_x509_trust_mem",
                 gnutls_strerror(ret));
        if (alloced_ca_memory) gnutls_free(ca.data);
        goto fail;
      }
      if (alloced_ca_memory) gnutls_free(ca.data);
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
      if (setup_data->is_rpk_not_cert) {
#if (GNUTLS_VERSION_NUMBER >= 0x030606)
        int have_done_key = 0;
        if (setup_data->pki_key.key.asn1.private_key_type ==
                                              COAP_ASN1_PKEY_EC) {
          gnutls_datum_t *spki = get_asn1_spki(key.data,
                                               key.size);

          if (spki) {
            ret = gnutls_certificate_set_rawpk_key_mem(*pki_credentials,
                                       spki,
                                       &key,
                                       GNUTLS_X509_FMT_DER, NULL,
                                       COAP_GNUTLS_KEY_RPK,
                                       NULL, 0, 0);
            if (ret >= 0) {
              have_done_key = 1;
            }
            gnutls_free(spki);
          }
        }
        if (!have_done_key) {
          G_CHECK(gnutls_certificate_set_rawpk_key_mem(*pki_credentials,
                           &cert,
                           &key,
                           GNUTLS_X509_FMT_DER, NULL,
                           COAP_GNUTLS_KEY_RPK,
                           NULL, 0, 0),
                 "gnutls_certificate_set_rawpk_key_mem");
        }
#else /* GNUTLS_VERSION_NUMBER < 0x030606 */
        coap_log(LOG_ERR,
              "RPK Support not available (needs gnutls 3.6.6 or later)\n");
      return GNUTLS_E_INSUFFICIENT_CREDENTIALS;
#endif /* GNUTLS_VERSION_NUMBER < 0x030606 */
      }
      else {
        G_CHECK(gnutls_certificate_set_x509_key_mem(*pki_credentials,
                           &cert,
                           &key,
                           GNUTLS_X509_FMT_DER),
              "gnutls_certificate_set_x509_key_mem");
      }
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
      ret = gnutls_certificate_set_x509_trust_mem(*pki_credentials,
                           &ca_cert,
                           GNUTLS_X509_FMT_DER);
      if (ret == 0) {
        coap_log(LOG_WARNING,
         "gnutls_certificate_set_x509_trust_mem: No certificates found\n");
      }
      else if (ret < 0) {
        coap_log(LOG_WARNING, "%s: '%s'\n",
                 "gnutls_certificate_set_x509_trust_mem",
                 gnutls_strerror(ret));
        goto fail;
      }
    }
    break;

  case COAP_PKI_KEY_PKCS11:
    if (setup_data->pki_key.key.pkcs11.public_cert &&
        setup_data->pki_key.key.pkcs11.public_cert[0] &&
        setup_data->pki_key.key.pkcs11.private_key &&
        setup_data->pki_key.key.pkcs11.private_key[0]) {

      gnutls_pkcs11_set_pin_function(pin_callback, setup_data);
      if (setup_data->is_rpk_not_cert) {
#if (GNUTLS_VERSION_NUMBER >= 0x030606)
        G_CHECK(gnutls_certificate_set_rawpk_key_file(*pki_credentials,
                                   setup_data->pki_key.key.pkcs11.public_cert,
                                   setup_data->pki_key.key.pkcs11.private_key,
                                   GNUTLS_X509_FMT_PEM, NULL,
                                   COAP_GNUTLS_KEY_RPK,
                                   NULL, 0, GNUTLS_PKCS_PLAIN, 0),
                 "gnutls_certificate_set_rawpk_key_file");
#else /* GNUTLS_VERSION_NUMBER < 0x030606 */
        coap_log(LOG_ERR,
              "RPK Support not available (needs gnutls 3.6.6 or later)\n");
      return GNUTLS_E_INSUFFICIENT_CREDENTIALS;
#endif /* GNUTLS_VERSION_NUMBER < 0x030606 */
      }
      else {
        G_CHECK(gnutls_certificate_set_x509_key_file(*pki_credentials,
                                   setup_data->pki_key.key.pkcs11.public_cert,
                                   setup_data->pki_key.key.pkcs11.private_key,
                                   GNUTLS_X509_FMT_DER),
                 "gnutls_certificate_set_x509_key_file");
      }
    }
    else if (role == COAP_DTLS_ROLE_SERVER) {
      coap_log(LOG_ERR,
               "***setup_pki: (D)TLS: No %s Certificate + Private "
               "Key defined\n",
               role == COAP_DTLS_ROLE_SERVER ? "Server" : "Client");
      return GNUTLS_E_INSUFFICIENT_CREDENTIALS;
    }
    if (setup_data->pki_key.key.pkcs11.ca &&
        setup_data->pki_key.key.pkcs11.ca[0]) {
      ret = gnutls_certificate_set_x509_trust_file(*pki_credentials,
                           setup_data->pki_key.key.pkcs11.ca,
                           GNUTLS_X509_FMT_DER);
      if (ret == 0) {
        coap_log(LOG_WARNING,
         "gnutls_certificate_set_x509_trust_file: No certificates found\n");
      }
      else if (ret < 0) {
        coap_log(LOG_WARNING, "%s: '%s'\n",
                 "gnutls_certificate_set_x509_trust_file",
                 gnutls_strerror(ret));
        goto fail;
      }
    }
    break;

  default:
    coap_log(LOG_ERR,
             "***setup_pki: (D)TLS: Unknown key type %d\n",
             setup_data->pki_key.key_type);
    return GNUTLS_E_INSUFFICIENT_CREDENTIALS;
  }

  if (g_context->root_ca_file) {
    ret = gnutls_certificate_set_x509_trust_file(*pki_credentials,
                         g_context->root_ca_file,
                         GNUTLS_X509_FMT_PEM);
    if (ret == 0) {
      coap_log(LOG_WARNING,
       "gnutls_certificate_set_x509_trust_file: Root CA: No certificates found\n");
    }
  }
  if (g_context->root_ca_path) {
#if (GNUTLS_VERSION_NUMBER >= 0x030306)
    G_CHECK(gnutls_certificate_set_x509_trust_dir(*pki_credentials,
                         g_context->root_ca_path,
                         GNUTLS_X509_FMT_PEM),
            "gnutls_certificate_set_x509_trust_dir");
#endif
  }
  gnutls_certificate_send_x509_rdn_sequence(g_session,
                                      setup_data->check_common_ca ? 0 : 1);
  if (!(g_context->psk_pki_enabled & IS_PKI)) {
    /* No PKI defined at all - still need a trust set up for 3.6.0 or later */
    G_CHECK(gnutls_certificate_set_x509_system_trust(*pki_credentials),
            "gnutls_certificate_set_x509_system_trust");
  }

  /* Verify Peer */
  gnutls_certificate_set_verify_function(*pki_credentials,
                                         cert_verify_callback_gnutls);

  /* Cert chain checking (can raise GNUTLS_E_CONSTRAINT_ERROR) */
  if (setup_data->cert_chain_validation) {
    gnutls_certificate_set_verify_limits(*pki_credentials,
                                         0,
                                      setup_data->cert_chain_verify_depth + 2);
  }

  /*
   * Check for self signed
   *           CRL checking (can raise GNUTLS_CERT_MISSING_OCSP_STATUS)
   */
  gnutls_certificate_set_verify_flags(*pki_credentials,
            (setup_data->check_cert_revocation == 0 ?
             GNUTLS_VERIFY_DISABLE_CRL_CHECKS : 0)
            );

  return GNUTLS_E_SUCCESS;

fail:
  return ret;
}

/*
 * return 0   Success (GNUTLS_E_SUCCESS)
 *        neg GNUTLS_E_* error code
 */
static int
setup_psk_credentials(gnutls_psk_server_credentials_t *psk_credentials,
                      coap_gnutls_context_t *g_context COAP_UNUSED,
                      coap_dtls_spsk_t *setup_data)
{
  int ret;
  char hint[COAP_DTLS_HINT_LENGTH];

  G_CHECK(gnutls_psk_allocate_server_credentials(psk_credentials),
          "gnutls_psk_allocate_server_credentials");
  gnutls_psk_set_server_credentials_function(*psk_credentials,
                                                    psk_server_callback);
  if (setup_data->psk_info.hint.s) {
    snprintf(hint, sizeof(hint), "%.*s", (int)setup_data->psk_info.hint.length,
             setup_data->psk_info.hint.s);
    G_CHECK(gnutls_psk_set_server_credentials_hint(*psk_credentials, hint),
          "gnutls_psk_set_server_credentials_hint");
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
post_client_hello_gnutls_psk(gnutls_session_t g_session)
{
  coap_session_t *c_session =
                (coap_session_t *)gnutls_transport_get_ptr(g_session);
  coap_gnutls_context_t *g_context =
             (coap_gnutls_context_t *)c_session->context->dtls_context;
  coap_gnutls_env_t *g_env = (coap_gnutls_env_t *)c_session->tls;
  int ret = GNUTLS_E_SUCCESS;
  char *name = NULL;

  if (c_session->context->spsk_setup_data.validate_sni_call_back) {
    coap_dtls_spsk_t sni_setup_data;
    /* DNS names (only type supported) may be at most 256 byte long */
    size_t len = 256;
    unsigned int type;
    unsigned int i;

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
    for (i = 0; i < g_context->psk_sni_count; i++) {
      if (strcasecmp(name, g_context->psk_sni_entry_list[i].sni) == 0) {
        break;
      }
    }
    if (i == g_context->psk_sni_count) {
      /*
       * New SNI request
       */
      const coap_dtls_spsk_info_t *new_entry =
        c_session->context->spsk_setup_data.validate_sni_call_back(name,
                        c_session,
                        c_session->context->spsk_setup_data.sni_call_back_arg);
      if (!new_entry) {
        G_ACTION(gnutls_alert_send(g_session, GNUTLS_AL_FATAL,
                                   GNUTLS_A_UNRECOGNIZED_NAME));
        g_env->sent_alert = 1;
        ret = GNUTLS_E_NO_CERTIFICATE_FOUND;
        goto end;
      }

      g_context->psk_sni_entry_list =
                            gnutls_realloc(g_context->psk_sni_entry_list,
                                           (i+1)*sizeof(psk_sni_entry));
      g_context->psk_sni_entry_list[i].sni = gnutls_strdup(name);
      g_context->psk_sni_entry_list[i].psk_info = *new_entry;
      sni_setup_data = c_session->context->spsk_setup_data;
      sni_setup_data.psk_info = *new_entry;
      if ((ret = setup_psk_credentials(
                           &g_context->psk_sni_entry_list[i].psk_credentials,
                           g_context,
                           &sni_setup_data)) < 0) {
        int keep_ret = ret;
        G_ACTION(gnutls_alert_send(g_session, GNUTLS_AL_FATAL,
                                   GNUTLS_A_BAD_CERTIFICATE));
        g_env->sent_alert = 1;
        ret = keep_ret;
        goto end;
      }
      g_context->psk_sni_count++;
    }
    G_CHECK(gnutls_credentials_set(g_env->g_session, GNUTLS_CRD_PSK,
                             g_context->psk_sni_entry_list[i].psk_credentials),
            "gnutls_credentials_set");
    coap_session_refresh_psk_hint(c_session,
                                 &g_context->psk_sni_entry_list[i].psk_info.hint);
    coap_session_refresh_psk_key(c_session,
                                 &g_context->psk_sni_entry_list[i].psk_info.key);
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
post_client_hello_gnutls_pki(gnutls_session_t g_session)
{
  coap_session_t *c_session =
                (coap_session_t *)gnutls_transport_get_ptr(g_session);
  coap_gnutls_context_t *g_context =
             (coap_gnutls_context_t *)c_session->context->dtls_context;
  coap_gnutls_env_t *g_env = (coap_gnutls_env_t *)c_session->tls;
  int ret = GNUTLS_E_SUCCESS;
  char *name = NULL;

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
    for (i = 0; i < g_context->pki_sni_count; i++) {
      if (strcasecmp(name, g_context->pki_sni_entry_list[i].sni) == 0) {
        break;
      }
    }
    if (i == g_context->pki_sni_count) {
      /*
       * New SNI request
       */
      coap_dtls_key_t *new_entry =
        g_context->setup_data.validate_sni_call_back(name,
                                   g_context->setup_data.sni_call_back_arg);
      if (!new_entry) {
        G_ACTION(gnutls_alert_send(g_session, GNUTLS_AL_FATAL,
                                   GNUTLS_A_UNRECOGNIZED_NAME));
        g_env->sent_alert = 1;
        ret = GNUTLS_E_NO_CERTIFICATE_FOUND;
        goto end;
      }

      g_context->pki_sni_entry_list = gnutls_realloc(
                                            g_context->pki_sni_entry_list,
                                            (i+1)*sizeof(pki_sni_entry));
      g_context->pki_sni_entry_list[i].sni = gnutls_strdup(name);
      g_context->pki_sni_entry_list[i].pki_key = *new_entry;
      sni_setup_data = g_context->setup_data;
      sni_setup_data.pki_key = *new_entry;
      if ((ret = setup_pki_credentials(
                           &g_context->pki_sni_entry_list[i].pki_credentials,
                           g_session,
                           g_context,
                           &sni_setup_data, COAP_DTLS_ROLE_SERVER)) < 0) {
        int keep_ret = ret;
        G_ACTION(gnutls_alert_send(g_session, GNUTLS_AL_FATAL,
                                   GNUTLS_A_BAD_CERTIFICATE));
        g_env->sent_alert = 1;
        ret = keep_ret;
        goto end;
      }
      g_context->pki_sni_count++;
    }
    G_CHECK(gnutls_credentials_set(g_env->g_session, GNUTLS_CRD_CERTIFICATE,
                               g_context->pki_sni_entry_list[i].pki_credentials),
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
setup_client_ssl_session(coap_session_t *c_session, coap_gnutls_env_t *g_env)
{
  coap_gnutls_context_t *g_context =
             (coap_gnutls_context_t *)c_session->context->dtls_context;
  int ret;

  g_context->psk_pki_enabled |= IS_CLIENT;
  if (g_context->psk_pki_enabled & IS_PSK) {
    coap_dtls_cpsk_t *setup_data = &c_session->cpsk_setup_data;
    G_CHECK(gnutls_psk_allocate_client_credentials(&g_env->psk_cl_credentials),
            "gnutls_psk_allocate_client_credentials");
    gnutls_psk_set_client_credentials_function(g_env->psk_cl_credentials,
                                               psk_client_callback);
    G_CHECK(gnutls_credentials_set(g_env->g_session, GNUTLS_CRD_PSK,
                                   g_env->psk_cl_credentials),
            "gnutls_credentials_set");
    /* Issue SNI if requested */
    if (setup_data->client_sni) {
      G_CHECK(gnutls_server_name_set(g_env->g_session, GNUTLS_NAME_DNS,
                                     setup_data->client_sni,
                                     strlen(setup_data->client_sni)),
              "gnutls_server_name_set");
    }
    if (setup_data->validate_ih_call_back) {
      const char *err;
      coap_tls_version_t* tls_version = coap_get_tls_library_version();

      if (tls_version->version >= 0x030604) {
        /* Disable TLS1.3 if Identity Hint Callback set */
        const char *priority;

        if (tls_version->version >= 0x030606) {
          priority = VARIANTS_NO_TLS13_3_6_6;
        }
        else {
          priority = VARIANTS_NO_TLS13_3_6_4;
        }
        ret = gnutls_priority_set_direct(g_env->g_session,
              priority, &err);
        if (ret < 0) {
          if (ret == GNUTLS_E_INVALID_REQUEST)
            coap_log(LOG_WARNING,
                     "gnutls_priority_set_direct: Syntax error at: %s\n", err);
          else
            coap_log(LOG_WARNING,
                     "gnutls_priority_set_direct: %s\n", gnutls_strerror(ret));
          goto fail;
        }
      }
    }
  }

  if ((g_context->psk_pki_enabled & IS_PKI) ||
      (g_context->psk_pki_enabled & (IS_PSK | IS_PKI)) == 0) {
    /*
     * If neither PSK or PKI have been set up, use PKI basics.
     * This works providing COAP_PKI_KEY_PEM has a value of 0.
     */
    coap_dtls_pki_t *setup_data = &g_context->setup_data;
    G_CHECK(setup_pki_credentials(&g_env->pki_credentials, g_env->g_session,
                                  g_context, setup_data,
                                  COAP_DTLS_ROLE_CLIENT),
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
  coap_gnutls_context_t *g_context;
  coap_dtls_spsk_t *setup_data;
  size_t identity_len = 0;
  uint8_t buf[COAP_DTLS_MAX_PSK];
  size_t psk_len;

  if (c_session == NULL || c_session->context == NULL ||
      c_session->context->get_server_psk == NULL)
    return -1;

  g_context = (coap_gnutls_context_t *)c_session->context->dtls_context;
  if (g_context == NULL)
    return -1;
  setup_data = &c_session->context->spsk_setup_data;

  if (identity)
    identity_len = strlen(identity);
  else
    identity = "";

  /* Track the Identity being used */
  if (c_session->psk_identity)
    coap_delete_bin_const(c_session->psk_identity);
  c_session->psk_identity = coap_new_bin_const((const uint8_t *)identity,
                                               identity_len);

  coap_log(LOG_DEBUG, "got psk_identity: '%.*s'\n",
                      (int)identity_len, identity);

  if (setup_data->validate_id_call_back) {
    coap_bin_const_t lidentity;
    lidentity.length = identity_len;
    lidentity.s = (const uint8_t*)identity;
    const coap_bin_const_t *psk_key =
             setup_data->validate_id_call_back(&lidentity,
                                               c_session,
                                               setup_data->id_call_back_arg);

    if (psk_key == NULL)
      return -1;
    key->data = gnutls_malloc(psk_key->length);
    if (key->data == NULL)
      return -1;
    memcpy(key->data, psk_key->s, psk_key->length);
    key->size = psk_key->length;
    coap_session_refresh_psk_key(c_session, psk_key);
    return 0;
  }

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
    G_CHECK(setup_psk_credentials(
                             &g_env->psk_sv_credentials,
                             g_context,
                             &c_session->context->spsk_setup_data),
            "setup_psk_credentials\n");
    G_CHECK(gnutls_credentials_set(g_env->g_session,
                                   GNUTLS_CRD_PSK,
                                   g_env->psk_sv_credentials),
            "gnutls_credentials_set\n");
    gnutls_handshake_set_post_client_hello_function(g_env->g_session,
                                                post_client_hello_gnutls_psk);
  }

  if (g_context->psk_pki_enabled & IS_PKI) {
    coap_dtls_pki_t *setup_data = &g_context->setup_data;
    G_CHECK(setup_pki_credentials(&g_env->pki_credentials, g_env->g_session,
                                  g_context, setup_data,
                                  COAP_DTLS_ROLE_SERVER),
            "setup_pki_credentials");

    if (setup_data->verify_peer_cert) {
      gnutls_certificate_server_set_request(g_env->g_session,
                                            GNUTLS_CERT_REQUIRE);
    }
    else if (setup_data->is_rpk_not_cert) {
      gnutls_certificate_server_set_request(g_env->g_session,
                                            GNUTLS_CERT_REQUEST);
    }
    else {
      gnutls_certificate_server_set_request(g_env->g_session,
                                            GNUTLS_CERT_IGNORE);
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
  coap_session_t *c_session = (coap_session_t *)context;
  coap_ssl_t *data;

  if (!c_session->tls) {
    errno = EAGAIN;
    return -1;
  }
  data = &((coap_gnutls_env_t *)c_session->tls)->coap_ssl_data;

  if (out != NULL) {
    if (data != NULL && data->pdu_len > 0) {
      if (outl < data->pdu_len) {
        memcpy(out, data->pdu, outl);
        ret = outl;
        if (!data->peekmode) {
          data->pdu += outl;
          data->pdu_len -= outl;
        }
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
  coap_session_t *c_session = (coap_session_t *)context;

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
receive_timeout(gnutls_transport_ptr_t context, unsigned int ms COAP_UNUSED) {
  coap_session_t *c_session = (coap_session_t *)context;

  if (c_session) {
    fd_set readfds, writefds, exceptfds;
    struct timeval tv;
    int nfds = c_session->sock.fd +1;
    coap_gnutls_env_t *g_env = (coap_gnutls_env_t *)c_session->tls;

    /* If data has been read in by libcoap ahead of GnuTLS, say it is there */
    if (c_session->proto == COAP_PROTO_DTLS && g_env &&
        g_env->coap_ssl_data.pdu_len > 0) {
      return 1;
    }

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    FD_SET (c_session->sock.fd, &readfds);
    if (!(g_env && g_env->doing_dtls_timeout)) {
      FD_SET (c_session->sock.fd, &writefds);
      FD_SET (c_session->sock.fd, &exceptfds);
    }
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
#if (GNUTLS_VERSION_NUMBER >= 0x030606)
  int flags = type | GNUTLS_DATAGRAM | GNUTLS_NONBLOCK | GNUTLS_ENABLE_RAWPK;
#else /* < 3.6.6 */
  int flags = type | GNUTLS_DATAGRAM | GNUTLS_NONBLOCK;
#endif /* < 3.6.6 */
  int ret;

  if (g_env)
    return g_env;

  g_env = gnutls_malloc(sizeof(coap_gnutls_env_t));
  if (!g_env)
    return NULL;

  memset(g_env, 0, sizeof(coap_gnutls_env_t));

  G_CHECK(gnutls_init(&g_env->g_session, flags), "gnutls_init");

  gnutls_transport_set_pull_function(g_env->g_session, coap_dgram_read);
  gnutls_transport_set_push_function(g_env->g_session, coap_dgram_write);
  gnutls_transport_set_pull_timeout_function(g_env->g_session, receive_timeout);
  /* So we can track the coap_session_t in callbacks */
  gnutls_transport_set_ptr(g_env->g_session, c_session);

  G_CHECK(gnutls_priority_set(g_env->g_session, g_context->priority_cache),
          "gnutls_priority_set");

  if (type == GNUTLS_SERVER) {
    G_CHECK(setup_server_ssl_session(c_session, g_env),
            "setup_server_ssl_session");
  }
  else {
    G_CHECK(setup_client_ssl_session(c_session, g_env),
            "setup_client_ssl_session");
  }

  gnutls_handshake_set_timeout(g_env->g_session,
                               GNUTLS_DEFAULT_HANDSHAKE_TIMEOUT);
  gnutls_dtls_set_timeouts(g_env->g_session, COAP_DTLS_RETRANSMIT_MS,
                                             COAP_DTLS_RETRANSMIT_TOTAL_MS);

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
    if (free_bye != COAP_FREE_BYE_NONE && !g_env->sent_alert) {
      /* Only do this if appropriate */
      gnutls_bye(g_env->g_session, free_bye == COAP_FREE_BYE_AS_UDP ?
                                       GNUTLS_SHUT_WR : GNUTLS_SHUT_RDWR);
    }
    gnutls_deinit(g_env->g_session);
    g_env->g_session = NULL;
    if (g_context->psk_pki_enabled & IS_PSK) {
      if ((g_context->psk_pki_enabled & IS_CLIENT) &&
          g_env->psk_cl_credentials != NULL) {
        gnutls_psk_free_client_credentials(g_env->psk_cl_credentials);
        g_env->psk_cl_credentials = NULL;
      }
      else {
        /* YUK - A memory leak in 3.3.0 (fixed by 3.3.26) of hint */
        if (g_env->psk_sv_credentials != NULL)
          gnutls_psk_free_server_credentials(g_env->psk_sv_credentials);
        g_env->psk_sv_credentials = NULL;
      }
    }
    if ((g_context->psk_pki_enabled & IS_PKI) ||
        (g_context->psk_pki_enabled &
         (IS_PSK | IS_PKI | IS_CLIENT)) == IS_CLIENT) {
      gnutls_certificate_free_credentials(g_env->pki_credentials);
      g_env->pki_credentials = NULL;
    }
    gnutls_free(g_env->coap_ssl_data.cookie_key.data);
    gnutls_free(g_env);
  }
}

void *coap_dtls_new_server_session(coap_session_t *c_session) {
  coap_gnutls_env_t *g_env =
         (coap_gnutls_env_t *)c_session->tls;

  gnutls_transport_set_ptr(g_env->g_session, c_session);

  return g_env;
}

static void log_last_alert(coap_session_t *c_session,
                           gnutls_session_t g_session) {
  int last_alert = gnutls_alert_get(g_session);

  if (last_alert == GNUTLS_A_CLOSE_NOTIFY)
    coap_log(LOG_DEBUG, "***%s: Alert '%d': %s\n",
                         coap_session_str(c_session),
                         last_alert, gnutls_alert_get_name(last_alert));
  else
    coap_log(LOG_WARNING, "***%s: Alert '%d': %s\n",
                          coap_session_str(c_session),
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
  case GNUTLS_E_FATAL_ALERT_RECEIVED:
    /* Stop the sending of an alert on closedown */
    g_env->sent_alert = 1;
    log_last_alert(c_session, g_env->g_session);
    /* Fall through */
  case GNUTLS_E_UNEXPECTED_HANDSHAKE_PACKET:
    c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
    ret = -1;
    break;
  case GNUTLS_E_WARNING_ALERT_RECEIVED:
    log_last_alert(c_session, g_env->g_session);
    c_session->dtls_event = COAP_EVENT_DTLS_ERROR;
    ret = 0;
    break;
  case GNUTLS_E_NO_CERTIFICATE_FOUND:
#if (GNUTLS_VERSION_NUMBER > 0x030606)
  case GNUTLS_E_CERTIFICATE_REQUIRED:
#endif /* GNUTLS_VERSION_NUMBER > 0x030606 */
    coap_log(LOG_WARNING,
             "Client Certificate requested and required, but not provided\n"
             );
    G_ACTION(gnutls_alert_send(g_env->g_session, GNUTLS_AL_FATAL,
                                                 GNUTLS_A_BAD_CERTIFICATE));
    g_env->sent_alert = 1;
    c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
    ret = -1;
    break;
  case GNUTLS_E_DECRYPTION_FAILED:
    coap_log(LOG_WARNING,
             "do_gnutls_handshake: session establish "
             "returned '%s'\n",
             gnutls_strerror(ret));
    G_ACTION(gnutls_alert_send(g_env->g_session, GNUTLS_AL_FATAL,
                                                 GNUTLS_A_DECRYPT_ERROR));
    g_env->sent_alert = 1;
    c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
    ret = -1;
    break;
  case GNUTLS_E_CERTIFICATE_ERROR:
    if (g_env->sent_alert) {
      c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
      ret = -1;
      break;
    }
    /* Fall through */
  case GNUTLS_E_UNKNOWN_CIPHER_SUITE:
  case GNUTLS_E_NO_CIPHER_SUITES:
  case GNUTLS_E_INVALID_SESSION:
    coap_log(LOG_WARNING,
             "do_gnutls_handshake: session establish "
             "returned '%s'\n",
             gnutls_strerror(ret));
    if (!g_env->sent_alert) {
      G_ACTION(gnutls_alert_send(g_env->g_session, GNUTLS_AL_FATAL,
                                                   GNUTLS_A_HANDSHAKE_FAILURE));
      g_env->sent_alert = 1;
    }
    c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
    ret = -1;
    break;
  case GNUTLS_E_SESSION_EOF:
  case GNUTLS_E_TIMEDOUT:
  case GNUTLS_E_PULL_ERROR:
  case GNUTLS_E_PUSH_ERROR:
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
  if (c_session && c_session->context && c_session->tls) {
    coap_dtls_free_gnutls_env(c_session->context->dtls_context,
                c_session->tls,
                COAP_PROTO_NOT_RELIABLE(c_session->proto) ?
                 COAP_FREE_BYE_AS_UDP : COAP_FREE_BYE_AS_TCP);
    c_session->tls = NULL;
    coap_handle_event(c_session->context, COAP_EVENT_DTLS_CLOSED, c_session);
  }
}

void coap_dtls_session_update_mtu(coap_session_t *c_session) {
  coap_gnutls_env_t *g_env = (coap_gnutls_env_t *)c_session->tls;
  int ret;

  if (g_env)
    G_CHECK(gnutls_dtls_set_data_mtu(g_env->g_session,
                                     (unsigned int)c_session->mtu),
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
        /* Stop the sending of an alert on closedown */
        g_env->sent_alert = 1;
        log_last_alert(c_session, g_env->g_session);
        c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
        ret = -1;
        break;
      default:
        coap_log(LOG_DEBUG,
                 "coap_dtls_send: gnutls_record_send "
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
  return 0;
}

coap_tick_t coap_dtls_get_context_timeout(void *dtls_context COAP_UNUSED) {
  return 0;
}

coap_tick_t coap_dtls_get_timeout(coap_session_t *c_session, coap_tick_t now) {
  coap_gnutls_env_t *g_env = (coap_gnutls_env_t *)c_session->tls;

  assert(c_session->state == COAP_SESSION_STATE_HANDSHAKE);
  if (g_env && g_env->g_session) {
    unsigned int rem_ms = gnutls_dtls_get_timeout(g_env->g_session);

    if (rem_ms == 0) {
      /*
       * Need to make sure that we do not do this too frequently as some
       * versions of gnutls reset retransmit if a spurious packet is received
       * (e.g. duplicate Client Hello), but last_transmit does not get updated
       * when gnutls_handshake() is called and there is 'nothing' to resend.
       */
      if (g_env->last_timeout + COAP_DTLS_RETRANSMIT_COAP_TICKS > now)
        return g_env->last_timeout + COAP_DTLS_RETRANSMIT_COAP_TICKS;
    }
    /* Reset for the next time */
    g_env->last_timeout = now;
    return now + rem_ms;
  }

  return 0;
}

void coap_dtls_handle_timeout(coap_session_t *c_session) {
  coap_gnutls_env_t *g_env = (coap_gnutls_env_t *)c_session->tls;

  assert(g_env != NULL && c_session->state == COAP_SESSION_STATE_HANDSHAKE);
  g_env->doing_dtls_timeout = 1;
  if ((++c_session->dtls_timeout_count > c_session->max_retransmit) ||
      (do_gnutls_handshake(c_session, g_env) < 0)) {
    /* Too many retries */
    g_env->doing_dtls_timeout = 0;
    coap_session_disconnected(c_session, COAP_NACK_TLS_FAILED);
  }
  else {
    g_env->doing_dtls_timeout = 0;
  }
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
    coap_log(LOG_ERR, "** %s: Previous data not read %u bytes\n",
             coap_session_str(c_session), ssl_data->pdu_len);
  ssl_data->pdu = data;
  ssl_data->pdu_len = data_len;

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
      switch (ret) {
      case GNUTLS_E_FATAL_ALERT_RECEIVED:
        /* Stop the sending of an alert on closedown */
        g_env->sent_alert = 1;
        log_last_alert(c_session, g_env->g_session);
        c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
        ret = -1;
        break;
      case GNUTLS_E_WARNING_ALERT_RECEIVED:
        log_last_alert(c_session, g_env->g_session);
        c_session->dtls_event = COAP_EVENT_DTLS_ERROR;
        ret = 0;
        break;
      default:
        coap_log(LOG_WARNING,
                 "coap_dtls_receive: gnutls_record_recv returned %d\n", ret);
        ret = -1;
        break;
      }
    }
  }
  else {
    ret = do_gnutls_handshake(c_session, g_env);
    if (ret == 1) {
      coap_session_connected(c_session);
    }
    else {
      ret = -1;
      if (ssl_data->pdu_len && !g_env->sent_alert) {
        /* Do the handshake again incase of internal timeout */
        ret = do_gnutls_handshake(c_session, g_env);
        if (ret == 1) {
          /* Just connected, so send the data */
           coap_session_connected(c_session);
        }
      }
    }
  }

  if (c_session->dtls_event >= 0) {
    /* COAP_EVENT_DTLS_CLOSED event reported in coap_session_disconnected() */
    if (c_session->dtls_event != COAP_EVENT_DTLS_CLOSED)
      coap_handle_event(c_session->context, c_session->dtls_event, c_session);
    if (c_session->dtls_event == COAP_EVENT_DTLS_ERROR ||
        c_session->dtls_event == COAP_EVENT_DTLS_CLOSED) {
      coap_session_disconnected(c_session, COAP_NACK_TLS_FAILED);
      ssl_data = NULL;
      ret = -1;
    }
  }
  if (ssl_data && ssl_data->pdu_len) {
    /* pdu data is held on stack which will not stay there */
    coap_log(LOG_DEBUG, "coap_dtls_receive: ret %d: remaining data %u\n", ret, ssl_data->pdu_len);
    ssl_data->pdu_len = 0;
    ssl_data->pdu = NULL;
  }
  return ret;
}

/*
 * return -1  failure
 *         0  not completed
 *         1  client hello seen
 */
int
coap_dtls_hello(coap_session_t *c_session,
  const uint8_t *data,
  size_t data_len
) {
  coap_gnutls_env_t *g_env = (coap_gnutls_env_t *)c_session->tls;
  coap_ssl_t *ssl_data;
  int ret;

  if (!g_env) {
    g_env = coap_dtls_new_gnutls_env(c_session, GNUTLS_SERVER);
    if (g_env) {
      c_session->tls = g_env;
      gnutls_key_generate(&g_env->coap_ssl_data.cookie_key,
                          GNUTLS_COOKIE_KEY_SIZE);
    }
    else {
      /* error should have already been reported */
      return -1;
    }
  }
  if (data_len > 0) {
    gnutls_dtls_prestate_st prestate;
    uint8_t *data_rw;

    memset(&prestate, 0, sizeof(prestate));
    /* Need to do this to not get a compiler warning about const parameters */
    memcpy (&data_rw, &data, sizeof(data_rw));
    ret = gnutls_dtls_cookie_verify(&g_env->coap_ssl_data.cookie_key,
                                     &c_session->addr_info,
                                     sizeof(c_session->addr_info),
                                     data_rw, data_len,
                                     &prestate);
    if (ret < 0) {  /* cookie not valid */
      coap_log(LOG_DEBUG, "Invalid Cookie - sending Hello Verify\n");
      gnutls_dtls_cookie_send(&g_env->coap_ssl_data.cookie_key,
                              &c_session->addr_info,
                              sizeof(c_session->addr_info),
                              &prestate,
                              c_session,
                              coap_dgram_write);
      return 0;
    }
    gnutls_dtls_prestate_set(g_env->g_session, &prestate);
  }

  ssl_data = &g_env->coap_ssl_data;
  ssl_data->pdu = data;
  ssl_data->pdu_len = data_len;

  ret = do_gnutls_handshake(c_session, g_env);
  if (ret < 0) {
    /*
     * as the above failed, need to remove g_env to clean up any
     * pollution of the information
     */
    coap_dtls_free_gnutls_env(
            ((coap_gnutls_context_t *)c_session->context->dtls_context),
            g_env, COAP_FREE_BYE_NONE);
    c_session->tls = NULL;
    ssl_data = NULL;
    ret = -1;
  }
  else {
    /* Client Hello has been seen */
    ret = 1;
  }

  if (ssl_data && ssl_data->pdu_len) {
    /* pdu data is held on stack which will not stay there */
    coap_log(LOG_DEBUG, "coap_dtls_hello: ret %d: remaining data %u\n", ret, ssl_data->pdu_len);
     ssl_data->pdu_len = 0;
     ssl_data->pdu = NULL;
  }
  return ret;
}

unsigned int coap_dtls_get_overhead(coap_session_t *c_session COAP_UNUSED) {
  return 37;
}

#if !COAP_DISABLE_TCP
/*
 * return +ve data amount
 *        0   no more
 *        -1  error (error in errno)
 */
static ssize_t
coap_sock_read(gnutls_transport_ptr_t context, void *out, size_t outl) {
  int ret = 0;
  coap_session_t *c_session = (coap_session_t *)context;

  if (out != NULL) {
#ifdef _WIN32
    ret = recv(c_session->sock.fd, (char *)out, (int)outl, 0);
#else
    ret = recv(c_session->sock.fd, out, outl, 0);
#endif
    if (ret > 0) {
      coap_log(LOG_DEBUG, "*  %s: received %d bytes\n",
               coap_session_str(c_session), ret);
    } else if (ret < 0 && errno != EAGAIN) {
      coap_log(LOG_DEBUG,  "*  %s: failed to receive any bytes (%s)\n",
               coap_session_str(c_session), coap_socket_strerror());
    }
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
  coap_session_t *c_session = (coap_session_t *)context;

  ret = (int)coap_socket_write(&c_session->sock, in, inl);
  if (ret > 0) {
    coap_log(LOG_DEBUG, "*  %s: sent %d bytes\n",
             coap_session_str(c_session), ret);
  } else if (ret < 0) {
    if ((c_session->state == COAP_SESSION_STATE_CSM ||
         c_session->state == COAP_SESSION_STATE_HANDSHAKE) &&
        (errno == EPIPE || errno == ECONNRESET)) {
      /*
       * Need to handle a TCP timing window where an agent continues with
       * the sending of the next handshake or a CSM.
       * However, the peer does not like a certificate and so sends a
       * fatal alert and closes the TCP session.
       * The sending of the next handshake or CSM may get terminated because
       * of the closed TCP session, but there is still an outstanding alert
       * to be read in and reported on.
       * In this case, pretend that sending the info was fine so that the
       * alert can be read (which effectively is what happens with DTLS).
       */
      ret = inl;
    }
    else {
      coap_log(LOG_DEBUG,  "*  %s: failed to send %zd bytes (%s) state %d\n",
               coap_session_str(c_session), inl, coap_socket_strerror(),
               c_session->state);
    }
  }
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
#if (GNUTLS_VERSION_NUMBER >= 0x030606)
  int flags = GNUTLS_CLIENT | GNUTLS_NONBLOCK | GNUTLS_ENABLE_RAWPK;
#else /* < 3.6.6 */
  int flags = GNUTLS_CLIENT | GNUTLS_NONBLOCK;
#endif /* < 3.6.6 */
  int ret;

  if (!g_env) {
    return NULL;
  }
  memset(g_env, 0, sizeof(coap_gnutls_env_t));

  *connected = 0;
  G_CHECK(gnutls_init(&g_env->g_session, flags), "gnutls_init");

  gnutls_transport_set_pull_function(g_env->g_session, coap_sock_read);
  gnutls_transport_set_push_function(g_env->g_session, coap_sock_write);
  gnutls_transport_set_pull_timeout_function(g_env->g_session, receive_timeout);
  /* So we can track the coap_session_t in callbacks */
  gnutls_transport_set_ptr(g_env->g_session, c_session);

  gnutls_priority_set(g_env->g_session, g_context->priority_cache);
  setup_client_ssl_session(c_session, g_env);

  gnutls_handshake_set_timeout(g_env->g_session, GNUTLS_DEFAULT_HANDSHAKE_TIMEOUT);

  c_session->tls = g_env;
  ret = do_gnutls_handshake(c_session, g_env);
  if (ret == 1) {
    *connected = 1;
    coap_handle_event(c_session->context, COAP_EVENT_DTLS_CONNECTED, c_session);
    coap_session_send_csm(c_session);
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
#if (GNUTLS_VERSION_NUMBER >= 0x030606)
  int flags = GNUTLS_SERVER | GNUTLS_NONBLOCK | GNUTLS_ENABLE_RAWPK;
#else /* < 3.6.6 */
  int flags = GNUTLS_SERVER | GNUTLS_NONBLOCK;
#endif /* < 3.6.6 */
  int ret;

  if (!g_env)
    return NULL;
  memset(g_env, 0, sizeof(coap_gnutls_env_t));

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
      case GNUTLS_E_PUSH_ERROR:
      case GNUTLS_E_PULL_ERROR:
      case GNUTLS_E_PREMATURE_TERMINATION:
        c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
        ret = -1;
        break;
      case GNUTLS_E_FATAL_ALERT_RECEIVED:
        /* Stop the sending of an alert on closedown */
        g_env->sent_alert = 1;
        log_last_alert(c_session, g_env->g_session);
        c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
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
        coap_log(LOG_INFO, "coap_tls_write: cannot send PDU\n");
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
    /* COAP_EVENT_DTLS_CLOSED event reported in coap_session_disconnected() */
    if (c_session->dtls_event != COAP_EVENT_DTLS_CLOSED)
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
  int ret = -1;

  if (!g_env)
    return -1;

  c_session->dtls_event = -1;
  if (!g_env->established && !g_env->sent_alert) {
    ret = do_gnutls_handshake(c_session, g_env);
    if (ret == 1) {
      coap_handle_event(c_session->context, COAP_EVENT_DTLS_CONNECTED,
                                                               c_session);
      coap_session_send_csm(c_session);
    }
  }
  if (c_session->state != COAP_SESSION_STATE_NONE && g_env->established) {
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
      case GNUTLS_E_FATAL_ALERT_RECEIVED:
        /* Stop the sending of an alert on closedown */
        g_env->sent_alert = 1;
        log_last_alert(c_session, g_env->g_session);
        c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
        ret = -1;
        break;
      case GNUTLS_E_WARNING_ALERT_RECEIVED:
        log_last_alert(c_session, g_env->g_session);
        c_session->dtls_event = COAP_EVENT_DTLS_ERROR;
        ret = 0;
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
    /* COAP_EVENT_DTLS_CLOSED event reported in coap_session_disconnected() */
    if (c_session->dtls_event != COAP_EVENT_DTLS_CLOSED)
      coap_handle_event(c_session->context, c_session->dtls_event, c_session);
    if (c_session->dtls_event == COAP_EVENT_DTLS_ERROR ||
        c_session->dtls_event == COAP_EVENT_DTLS_CLOSED) {
      coap_session_disconnected(c_session, COAP_NACK_TLS_FAILED);
      ret = -1;
    }
  }
  return ret;
}
#endif /* !COAP_DISABLE_TCP */

coap_digest_ctx_t *
coap_digest_setup(void) {
  gnutls_hash_hd_t digest_ctx;

  if (gnutls_hash_init(&digest_ctx, GNUTLS_DIG_SHA256)) {
    return NULL;
  }
  return digest_ctx;
}

void
coap_digest_free(coap_digest_ctx_t *digest_ctx) {
  gnutls_hash_deinit(digest_ctx, NULL);
}

int
coap_digest_update(coap_digest_ctx_t *digest_ctx,
                   const uint8_t *data,
                   size_t data_len) {
  int ret = gnutls_hash(digest_ctx, data, data_len);

  return ret == 0;
}

int
coap_digest_final(coap_digest_ctx_t *digest_ctx,
                  coap_digest_t *digest_buffer) {
  gnutls_hash_output(digest_ctx, (uint8_t*)digest_buffer);

  coap_digest_free(digest_ctx);
  return 1;
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
