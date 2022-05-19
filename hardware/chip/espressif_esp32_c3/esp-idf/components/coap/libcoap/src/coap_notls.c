/*
* coap_notls.c -- Stub Datagram Transport Layer Support for libcoap
*
* Copyright (C) 2016 Olaf Bergmann <bergmann@tzi.org>
*
 * SPDX-License-Identifier: BSD-2-Clause
 *
* This file is part of the CoAP library libcoap. Please see README for terms
* of use.
*/

#include "coap3/coap_internal.h"

#if !defined(HAVE_LIBTINYDTLS) && !defined(HAVE_OPENSSL) && !defined(HAVE_LIBGNUTLS) && !defined(HAVE_MBEDTLS)

int
coap_dtls_is_supported(void) {
  return 0;
}

int
coap_tls_is_supported(void) {
  return 0;
}

coap_tls_version_t *
coap_get_tls_library_version(void) {
  static coap_tls_version_t version;
  version.version = 0;
  version.type = COAP_TLS_LIBRARY_NOTLS;
  return &version;
}

int
coap_dtls_context_set_pki(coap_context_t *ctx COAP_UNUSED,
                          const coap_dtls_pki_t* setup_data COAP_UNUSED,
                          const coap_dtls_role_t role COAP_UNUSED
) {
  return 0;
}

int
coap_dtls_context_set_pki_root_cas(coap_context_t *ctx COAP_UNUSED,
                                   const char *ca_file COAP_UNUSED,
                                   const char *ca_path COAP_UNUSED
) {
  return 0;
}

int
coap_dtls_context_set_cpsk(coap_context_t *ctx COAP_UNUSED,
                          coap_dtls_cpsk_t* setup_data COAP_UNUSED
) {
  return 0;
}

int
coap_dtls_context_set_spsk(coap_context_t *ctx COAP_UNUSED,
                          coap_dtls_spsk_t* setup_data COAP_UNUSED
) {
  return 0;
}

int
coap_dtls_context_check_keys_enabled(coap_context_t *ctx COAP_UNUSED)
{
  return 0;
}

static int dtls_log_level = 0;

void coap_dtls_startup(void) {
}

void *
coap_dtls_get_tls(const coap_session_t *c_session COAP_UNUSED,
                  coap_tls_library_t *tls_lib) {
  if (tls_lib)
    *tls_lib = COAP_TLS_LIBRARY_NOTLS;
  return NULL;
}

void coap_dtls_shutdown(void) {
}

void
coap_dtls_set_log_level(int level) {
  dtls_log_level = level;
}

int
coap_dtls_get_log_level(void) {
  return dtls_log_level;
}

void *
coap_dtls_new_context(coap_context_t *coap_context COAP_UNUSED) {
  return NULL;
}

void
coap_dtls_free_context(void *handle COAP_UNUSED) {
}

void *coap_dtls_new_server_session(coap_session_t *session COAP_UNUSED) {
  return NULL;
}

void *coap_dtls_new_client_session(coap_session_t *session COAP_UNUSED) {
  return NULL;
}

void coap_dtls_free_session(coap_session_t *coap_session COAP_UNUSED) {
}

void coap_dtls_session_update_mtu(coap_session_t *session COAP_UNUSED) {
}

int
coap_dtls_send(coap_session_t *session COAP_UNUSED,
  const uint8_t *data COAP_UNUSED,
  size_t data_len COAP_UNUSED
) {
  return -1;
}

int coap_dtls_is_context_timeout(void) {
  return 1;
}

coap_tick_t coap_dtls_get_context_timeout(void *dtls_context COAP_UNUSED) {
  return 0;
}

coap_tick_t
coap_dtls_get_timeout(coap_session_t *session COAP_UNUSED, coap_tick_t now COAP_UNUSED) {
  return 0;
}

void coap_dtls_handle_timeout(coap_session_t *session COAP_UNUSED) {
}

int
coap_dtls_receive(coap_session_t *session COAP_UNUSED,
  const uint8_t *data COAP_UNUSED,
  size_t data_len COAP_UNUSED
) {
  return -1;
}

int
coap_dtls_hello(coap_session_t *session COAP_UNUSED,
  const uint8_t *data COAP_UNUSED,
  size_t data_len COAP_UNUSED
) {
  return 0;
}

unsigned int coap_dtls_get_overhead(coap_session_t *session COAP_UNUSED) {
  return 0;
}

void *coap_tls_new_client_session(coap_session_t *session COAP_UNUSED, int *connected COAP_UNUSED) {
  return NULL;
}

void *coap_tls_new_server_session(coap_session_t *session COAP_UNUSED, int *connected COAP_UNUSED) {
  return NULL;
}

void coap_tls_free_session(coap_session_t *coap_session COAP_UNUSED) {
}

ssize_t coap_tls_write(coap_session_t *session COAP_UNUSED,
                       const uint8_t *data COAP_UNUSED,
                       size_t data_len COAP_UNUSED
) {
  return -1;
}

ssize_t coap_tls_read(coap_session_t *session COAP_UNUSED,
                      uint8_t *data COAP_UNUSED,
                      size_t data_len COAP_UNUSED
) {
  return -1;
}

typedef struct coap_local_hash_t {
  size_t ofs;
  coap_key_t key[8];   /* 32 bytes in total */
} coap_local_hash_t;

coap_digest_ctx_t *
coap_digest_setup(void) {
  coap_key_t *digest_ctx = coap_malloc(sizeof(coap_local_hash_t));

  if (digest_ctx) {
    memset(digest_ctx, 0, sizeof(coap_local_hash_t));
  }

  return digest_ctx;
}

void
coap_digest_free(coap_digest_ctx_t *digest_ctx) {
  coap_free(digest_ctx);
}

int
coap_digest_update(coap_digest_ctx_t *digest_ctx,
                   const uint8_t *data,
                   size_t data_len) {
  coap_local_hash_t *local = (coap_local_hash_t*)digest_ctx;

  coap_hash(data, data_len, local->key[local->ofs]);

  local->ofs = (local->ofs + 1) % 7;
  return 1;
}

int
coap_digest_final(coap_digest_ctx_t *digest_ctx,
                  coap_digest_t *digest_buffer) {
  coap_local_hash_t *local = (coap_local_hash_t*)digest_ctx;

  memcpy(digest_buffer, local->key, sizeof(coap_digest_t));

  coap_digest_free(digest_ctx);
  return 1;
}

#else /* !HAVE_LIBTINYDTLS && !HAVE_OPENSSL && !HAVE_LIBGNUTLS */

#ifdef __clang__
/* Make compilers happy that do not like empty modules. As this function is
 * never used, we ignore -Wunused-function at the end of compiling this file
 */
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
static inline void dummy(void) {
}

#endif /* !HAVE_LIBTINYDTLS && !HAVE_OPENSSL && !HAVE_LIBGNUTLS && !HAVE_MBEDTLS */
