/*
 * coap_tinydtls.c -- Datagram Transport Layer Support for libcoap with tinydtls
 *
 * Copyright (C) 2016-2020 Olaf Bergmann <bergmann@tzi.org>
 * Copyright (C) 2020 Jon Shallow <supjps-libcoap@jpshallow.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */

#include "coap3/coap_internal.h"

#ifdef HAVE_LIBTINYDTLS

/* We want TinyDTLS versions of these, not libcoap versions */
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_URL
#undef PACKAGE_VERSION

#include <tinydtls.h>
#include <dtls.h>
#include <dtls_debug.h>

typedef struct coap_tiny_context_t {
  struct dtls_context_t *dtls_context;
  coap_context_t *coap_context;
#ifdef DTLS_ECC
  coap_dtls_pki_t setup_data;
  coap_binary_t *priv_key;
  coap_binary_t *pub_key;
#endif /* DTLS_ECC */
} coap_tiny_context_t;

static dtls_tick_t dtls_tick_0 = 0;
static coap_tick_t coap_tick_0 = 0;

int
coap_dtls_is_supported(void) {
  return 1;
}

void coap_dtls_startup(void) {
  dtls_init();
  dtls_ticks(&dtls_tick_0);
  coap_ticks(&coap_tick_0);
}

void coap_dtls_shutdown(void) {
}

void *
coap_dtls_get_tls(const coap_session_t *c_session,
                  coap_tls_library_t *tls_lib) {
  if (tls_lib)
    *tls_lib = COAP_TLS_LIBRARY_TINYDTLS;
  if (c_session && c_session->context && c_session->context->dtls_context) {
    const coap_tiny_context_t *t_context =
                  (const coap_tiny_context_t *)c_session->context->dtls_context;

    return t_context->dtls_context;
  }
  return NULL;
}

void
coap_dtls_set_log_level(int level) {
  dtls_set_log_level(level);
}

int
coap_dtls_get_log_level(void) {
  return dtls_get_log_level();
}

static void get_session_addr(const session_t *s, coap_address_t *a) {
#ifdef WITH_CONTIKI
  a->addr = s->addr;
  a->port = s->port;
#else
  if (s->addr.sa.sa_family == AF_INET6) {
    a->size = (socklen_t)sizeof(a->addr.sin6);
    a->addr.sin6 = s->addr.sin6;
  } else if (s->addr.sa.sa_family == AF_INET) {
    a->size = (socklen_t)sizeof(a->addr.sin);
    a->addr.sin = s->addr.sin;
  } else {
    a->size = (socklen_t)s->size;
    a->addr.sa = s->addr.sa;
  }
#endif
}

static void put_session_addr(const coap_address_t *a, session_t *s) {
#ifdef WITH_CONTIKI
  s->size = (unsigned char)sizeof(s->addr);
  s->addr = a->addr;
  s->port = a->port;
#else
  if (a->addr.sa.sa_family == AF_INET6) {
    s->size = (socklen_t)sizeof(s->addr.sin6);
    s->addr.sin6 = a->addr.sin6;
  } else if (a->addr.sa.sa_family == AF_INET) {
    s->size = (socklen_t)sizeof(s->addr.sin);
    s->addr.sin = a->addr.sin;
  } else {
    s->size = (socklen_t)a->size;
    s->addr.sa = a->addr.sa;
  }
#endif
}

static int
dtls_send_to_peer(struct dtls_context_t *dtls_context,
  session_t *dtls_session, uint8 *data, size_t len) {
  coap_tiny_context_t *t_context =
                  (coap_tiny_context_t *)dtls_get_app_data(dtls_context);
  coap_context_t *coap_context = t_context ? t_context->coap_context : NULL;
  coap_session_t *coap_session;
  coap_address_t remote_addr;

  assert(coap_context);
  get_session_addr(dtls_session, &remote_addr);
  coap_session = coap_session_get_by_peer(coap_context, &remote_addr, dtls_session->ifindex);
  if (!coap_session) {
    coap_log(LOG_WARNING, "dtls_send_to_peer: cannot find local interface\n");
    return -3;
  }
  return (int)coap_session_send(coap_session, data, len);
}

static int
dtls_application_data(struct dtls_context_t *dtls_context,
  session_t *dtls_session, uint8 *data, size_t len) {
  coap_tiny_context_t *t_context =
                  (coap_tiny_context_t *)dtls_get_app_data(dtls_context);
  coap_context_t *coap_context = t_context ? t_context->coap_context : NULL;
  coap_session_t *coap_session;
  coap_address_t remote_addr;

  assert(coap_context);
  get_session_addr(dtls_session, &remote_addr);
  coap_session = coap_session_get_by_peer(coap_context, &remote_addr, dtls_session->ifindex);
  if (!coap_session) {
    coap_log(LOG_DEBUG,
             "dropped message that was received on invalid interface\n");
    return -1;
  }

  return coap_handle_dgram(coap_context, coap_session, data, len);
}

static int coap_event_dtls = 0;

static int
dtls_event(struct dtls_context_t *dtls_context,
  session_t *dtls_session,
  dtls_alert_level_t level,
  uint16_t code) {
  (void)dtls_context;
  (void)dtls_session;

  if (level == DTLS_ALERT_LEVEL_FATAL)
    coap_event_dtls = COAP_EVENT_DTLS_ERROR;

  /* handle DTLS events */
  switch (code) {
  case DTLS_ALERT_CLOSE_NOTIFY:
  {
    coap_event_dtls = COAP_EVENT_DTLS_CLOSED;
    break;
  }
  case DTLS_EVENT_CONNECTED:
  {
    coap_event_dtls = COAP_EVENT_DTLS_CONNECTED;
    break;
  }
  case DTLS_EVENT_RENEGOTIATE:
  {
    coap_event_dtls = COAP_EVENT_DTLS_RENEGOTIATE;
    break;
  }
  default:
    ;
  }

  return 0;
}

/* This function is the "key store" for tinyDTLS. It is called to
 * retrieve a key for the given identity within this particular
 * session. */
static int
get_psk_info(struct dtls_context_t *dtls_context,
  const session_t *dtls_session,
  dtls_credentials_type_t type,
  const uint8_t *id, size_t id_len,
  unsigned char *result, size_t result_length) {

  coap_tiny_context_t *t_context =
                  (coap_tiny_context_t *)dtls_get_app_data(dtls_context);
  coap_context_t *coap_context = t_context ? t_context->coap_context : NULL;
  coap_session_t *coap_session;
  int fatal_error = DTLS_ALERT_INTERNAL_ERROR;
  size_t identity_length;
  uint8_t psk[COAP_DTLS_MAX_PSK];
  size_t psk_len = 0;
  coap_address_t remote_addr;
  coap_dtls_cpsk_t *setup_cdata;
  coap_dtls_spsk_t *setup_sdata;
  coap_bin_const_t temp;

  assert(coap_context);
  get_session_addr(dtls_session, &remote_addr);
  coap_session = coap_session_get_by_peer(coap_context, &remote_addr, dtls_session->ifindex);
  if (!coap_session) {
    coap_log(LOG_DEBUG, "cannot get PSK, session not found\n");
    goto error;
  }

  switch (type) {
  case DTLS_PSK_IDENTITY:

    if (!coap_context || !coap_context->get_client_psk ||
        coap_session->type != COAP_SESSION_TYPE_CLIENT)
      goto error;

    setup_cdata = &coap_session->cpsk_setup_data;

    temp.s = id;
    temp.length = id_len;
    coap_session_refresh_psk_hint(coap_session, &temp);

    coap_log(LOG_DEBUG, "got psk_identity_hint: '%.*s'\n", (int)id_len, id ? (const char*)id : "");

    if (setup_cdata->validate_ih_call_back) {
      coap_str_const_t lhint;
      lhint.length = id_len;
      lhint.s = id;
      const coap_dtls_cpsk_info_t *psk_info =
               setup_cdata->validate_ih_call_back(&lhint,
                                                  coap_session,
                                                  setup_cdata->ih_call_back_arg);

      if (psk_info == NULL)
        return 0;
      if (psk_info->identity.length >= result_length)
        return 0;
      if (psk_info->key.length > sizeof(psk))
        return 0;

      if (coap_session->psk_identity) {
        coap_delete_bin_const(coap_session->psk_identity);
      }
      identity_length = psk_info->identity.length;
      coap_session->psk_identity = coap_new_bin_const(psk_info->identity.s, identity_length);
      memcpy(result, psk_info->identity.s, identity_length);
      result[identity_length] = '\000';

      coap_session_refresh_psk_key(coap_session, &psk_info->key);

      return identity_length;
    }

    identity_length = 0;
    /* result_length is max size of the returned identity */
    psk_len = coap_context->get_client_psk(coap_session, (const uint8_t*)id, id_len, (uint8_t*)result, &identity_length, result_length, psk, sizeof(psk));
    if (!psk_len) {
      coap_log(LOG_WARNING, "no PSK identity for given realm or buffer too small\n");
      fatal_error = DTLS_ALERT_CLOSE_NOTIFY;
      goto error;
    }
    return (int)identity_length;

  case DTLS_PSK_KEY:
    if (coap_session->type == COAP_SESSION_TYPE_CLIENT) {
      if (!coap_context || !coap_context->get_client_psk)
        goto error;
      identity_length = 0;
      /* Use psk[] as a scratch area for returning the unused identity */
      psk_len = coap_context->get_client_psk(coap_session, (const uint8_t*)id, id_len, psk, &identity_length, sizeof(psk), result, result_length);
      if (!psk_len) {
        coap_log(LOG_WARNING, "no pre-shared key for given realm or buffer too small\n");
        fatal_error = DTLS_ALERT_CLOSE_NOTIFY;
        goto error;
      }
      return (int)psk_len;
    }
    if (coap_context->get_server_psk) {
      setup_sdata = &coap_session->context->spsk_setup_data;

      if (!id)
        id = (const uint8_t *)"";

      /* Track the Identity being used */
      if (coap_session->psk_identity)
        coap_delete_bin_const(coap_session->psk_identity);
      coap_session->psk_identity = coap_new_bin_const(id, id_len);

      coap_log(LOG_DEBUG, "got psk_identity: '%.*s'\n",
               (int)id_len, id);

      if (setup_sdata->validate_id_call_back) {
        coap_bin_const_t lidentity;
        lidentity.length = id_len;
        lidentity.s = (const uint8_t*)id;
        const coap_bin_const_t *psk_key =
                 setup_sdata->validate_id_call_back(&lidentity,
                                                    coap_session,
                                                    setup_sdata->id_call_back_arg);

        if (psk_key == NULL)
          return 0;
        if (psk_key->length > result_length)
          return 0;
        memcpy(result, psk_key->s, psk_key->length);
        coap_session_refresh_psk_key(coap_session, psk_key);
        return psk_key->length;
      }

      return (int)coap_context->get_server_psk(coap_session, (const uint8_t*)id, id_len, (uint8_t*)result, result_length);
    }
    return 0;

  case DTLS_PSK_HINT:
    if (coap_context->get_server_hint)
      return (int)coap_context->get_server_hint(coap_session, (uint8_t *)result, result_length);
    return 0;

  default:
    coap_log(LOG_WARNING, "unsupported request type: %d\n", type);
  }

error:
  return dtls_alert_fatal_create(fatal_error);
}

#ifdef DTLS_ECC
static int
get_ecdsa_key(struct dtls_context_t *dtls_context,
              const session_t *dtls_session COAP_UNUSED,
              const dtls_ecdsa_key_t **result) {
  static dtls_ecdsa_key_t ecdsa_key;
  coap_tiny_context_t *t_context =
                  (coap_tiny_context_t *)dtls_get_app_data(dtls_context);

  ecdsa_key.curve = DTLS_ECDH_CURVE_SECP256R1;
  ecdsa_key.priv_key = t_context->priv_key->s;
  ecdsa_key.pub_key_x = t_context->pub_key->s;
  ecdsa_key.pub_key_y = &t_context->pub_key->s[DTLS_EC_KEY_SIZE];

  *result = &ecdsa_key;
  return 0;
}

/* first part of Raw public key, the is the start of the Subject Public Key */
static const unsigned char cert_asn1_header[] = {
  0x30, 0x59, /* SEQUENCE, length 89 bytes */
    0x30, 0x13, /* SEQUENCE, length 19 bytes */
      0x06, 0x07, /* OBJECT IDENTIFIER ecPublicKey (1 2 840 10045 2 1) */
        0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01,
      0x06, 0x08, /* OBJECT IDENTIFIER prime256v1 (1 2 840 10045 3 1 7) */
        0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07,
      0x03, 0x42, 0x00, /* BIT STRING, length 66 bytes, 0 bits unused */
         0x04 /* uncompressed, followed by the r and s values of the public key */
};
#define DTLS_CE_LENGTH (sizeof(cert_asn1_header) + key_size + key_size)

static int
verify_ecdsa_key(struct dtls_context_t *dtls_context COAP_UNUSED,
                 const session_t *dtls_session COAP_UNUSED,
                 const uint8_t *other_pub_x,
                 const uint8_t *other_pub_y,
                 size_t key_size) {
  coap_tiny_context_t *t_context =
                  (coap_tiny_context_t *)dtls_get_app_data(dtls_context);
  if (t_context && t_context->setup_data.validate_cn_call_back) {
    /* Need to build asn.1 certificate - code taken from tinydtls */
    uint8 *p;
    uint8 buf[DTLS_CE_LENGTH];
    coap_session_t *c_session;
    coap_address_t remote_addr;

    /* Certificate
     *
     * Start message construction at beginning of buffer. */
    p = buf;

    memcpy(p, &cert_asn1_header, sizeof(cert_asn1_header));
    p += sizeof(cert_asn1_header);

    memcpy(p, other_pub_x, key_size);
    p += key_size;

    memcpy(p, other_pub_y, key_size);
    p += key_size;

    assert(p <= (buf + sizeof(buf)));

    get_session_addr(dtls_session, &remote_addr);
    c_session = coap_session_get_by_peer(t_context->coap_context,
                                         &remote_addr, dtls_session->ifindex);
    if (!c_session)
      return -3;
    if (!t_context->setup_data.validate_cn_call_back(COAP_DTLS_RPK_CERT_CN,
        buf, p-buf, c_session, 0, 1, t_context->setup_data.cn_call_back_arg)) {
      return -1;
    }
  }
  return 0;
}
static dtls_handler_t ec_cb = {
  .write = dtls_send_to_peer,
  .read = dtls_application_data,
  .event = dtls_event,
  .get_psk_info = NULL,
  .get_ecdsa_key = get_ecdsa_key,
  .verify_ecdsa_key = verify_ecdsa_key
};
#endif /* DTLS_ECC */

static dtls_handler_t psk_cb = {
  .write = dtls_send_to_peer,
  .read = dtls_application_data,
  .event = dtls_event,
  .get_psk_info = get_psk_info,
#ifdef DTLS_ECC
  .get_ecdsa_key = NULL,
  .verify_ecdsa_key = NULL
#endif
};

void *
coap_dtls_new_context(coap_context_t *coap_context) {
  coap_tiny_context_t *t_context = coap_malloc(sizeof(coap_tiny_context_t));
  struct dtls_context_t *dtls_context = t_context ? dtls_new_context(t_context) : NULL;
  if (!dtls_context)
    goto error;
  memset(t_context, 0, sizeof(coap_tiny_context_t));
  t_context->coap_context = coap_context;
  t_context->dtls_context = dtls_context;
  dtls_set_handler(dtls_context, &psk_cb);
  return t_context;
error:
  if (t_context)
    coap_free(t_context);
  if (dtls_context)
    coap_dtls_free_context(dtls_context);
  return NULL;
}

void
coap_dtls_free_context(void *handle) {
  if (handle) {
    coap_tiny_context_t *t_context = (coap_tiny_context_t *)handle;
#ifdef DTLS_ECC
    if (t_context->priv_key) {
      coap_delete_binary(t_context->priv_key);
      t_context->priv_key = NULL;
    }
    if (t_context->pub_key) {
      coap_delete_binary(t_context->pub_key);
      t_context->pub_key = NULL;
    }
#endif /* DTLS_ECC */
    if (t_context->dtls_context)
      dtls_free_context(t_context->dtls_context);
    coap_free(t_context);
  }
}

static session_t *
coap_dtls_new_session(coap_session_t *session) {
  session_t *dtls_session = coap_malloc_type(COAP_DTLS_SESSION, sizeof(session_t));

  if (dtls_session) {
    /* create tinydtls session object from remote address and local
    * endpoint handle */
    dtls_session_init(dtls_session);
    put_session_addr(&session->addr_info.remote, dtls_session);
    dtls_session->ifindex = session->ifindex;
    coap_log(LOG_DEBUG, "***new session %p\n", (void *)dtls_session);
  }

  return dtls_session;
}

void *coap_dtls_new_server_session(coap_session_t *session) {
  return coap_dtls_new_session(session);
}

void *coap_dtls_new_client_session(coap_session_t *session) {
  dtls_peer_t *peer;
  coap_tiny_context_t *t_context = (coap_tiny_context_t *)session->context->dtls_context;
  dtls_context_t *dtls_context = t_context ? t_context->dtls_context : NULL;
  session_t *dtls_session = dtls_context ? coap_dtls_new_session(session) : NULL;

  if (!dtls_session)
    return NULL;
  peer =
    dtls_get_peer(dtls_context, dtls_session);

  if (!peer) {
    /* The peer connection does not yet exist. */
    /* dtls_connect() returns a value greater than zero if a new
    * connection attempt is made, 0 for session reuse. */
    if (dtls_connect(dtls_context, dtls_session) >= 0) {
      peer =
        dtls_get_peer(dtls_context, dtls_session);
    }
  }

  if (!peer) {
    /* delete existing session because the peer object has been invalidated */
    coap_free_type(COAP_DTLS_SESSION, dtls_session);
    dtls_session = NULL;
  }

  return dtls_session;
}

void
coap_dtls_session_update_mtu(coap_session_t *session) {
  (void)session;
}

void
coap_dtls_free_session(coap_session_t *coap_session) {
  coap_tiny_context_t *t_context =
                 (coap_tiny_context_t *)coap_session->context->dtls_context;
  dtls_context_t *dtls_context = t_context ? t_context->dtls_context : NULL;

  if (dtls_context == NULL)
    return;
  if (coap_session->tls && dtls_context) {
    dtls_peer_t *peer = dtls_get_peer(dtls_context, (session_t *)coap_session->tls);
    if ( peer )
      dtls_reset_peer(dtls_context, peer);
    else
      dtls_close(dtls_context, (session_t *)coap_session->tls);
    coap_log(LOG_DEBUG, "***removed session %p\n", coap_session->tls);
    coap_free_type(COAP_DTLS_SESSION, coap_session->tls);
    coap_session->tls = NULL;
    coap_handle_event(coap_session->context, COAP_EVENT_DTLS_CLOSED, coap_session);
  }
}

int
coap_dtls_send(coap_session_t *session,
  const uint8_t *data,
  size_t data_len
) {
  int res;
  uint8_t *data_rw;
  coap_tiny_context_t *t_context = (coap_tiny_context_t *)session->context->dtls_context;
  dtls_context_t *dtls_context = t_context ? t_context->dtls_context : NULL;

  assert(dtls_context);
  coap_log(LOG_DEBUG, "call dtls_write\n");

  coap_event_dtls = -1;
  /* Need to do this to not get a compiler warning about const parameters */
  memcpy (&data_rw, &data, sizeof(data_rw));
  res = dtls_write(dtls_context,
    (session_t *)session->tls, data_rw, data_len);

  if (res < 0)
    coap_log(LOG_WARNING, "coap_dtls_send: cannot send PDU\n");

  if (coap_event_dtls >= 0) {
    /* COAP_EVENT_DTLS_CLOSED event reported in coap_session_disconnected() */
    if (coap_event_dtls != COAP_EVENT_DTLS_CLOSED)
      coap_handle_event(session->context, coap_event_dtls, session);
    if (coap_event_dtls == COAP_EVENT_DTLS_CONNECTED)
      coap_session_connected(session);
    else if (coap_event_dtls == COAP_EVENT_DTLS_CLOSED || coap_event_dtls == COAP_EVENT_DTLS_ERROR)
      coap_session_disconnected(session, COAP_NACK_TLS_FAILED);
  }

  return res;
}

int coap_dtls_is_context_timeout(void) {
  return 1;
}

coap_tick_t coap_dtls_get_context_timeout(void *tiny_context) {
  clock_time_t next = 0;
  coap_tiny_context_t *t_context = (coap_tiny_context_t *)tiny_context;
  dtls_context_t *dtls_context = t_context ? t_context->dtls_context : NULL;
  if (tiny_context)
    dtls_check_retransmit(dtls_context, &next);
  if (next > 0)
    return ((coap_tick_t)(next - dtls_tick_0)) * COAP_TICKS_PER_SECOND / DTLS_TICKS_PER_SECOND + coap_tick_0;
  return 0;
}

coap_tick_t coap_dtls_get_timeout(coap_session_t *session, coap_tick_t now) {
  (void)session;
  (void)now;
  return 0;
}

void coap_dtls_handle_timeout(coap_session_t *session) {
  (void)session;
  return;
}

int
coap_dtls_receive(coap_session_t *session,
  const uint8_t *data,
  size_t data_len
) {
  session_t *dtls_session = (session_t *)session->tls;
  int err;
  uint8_t *data_rw;
  coap_tiny_context_t *t_context = (coap_tiny_context_t *)session->context->dtls_context;
  dtls_context_t *dtls_context = t_context ? t_context->dtls_context : NULL;

  assert(dtls_context);
  coap_event_dtls = -1;
  /* Need to do this to not get a compiler warning about const parameters */
  memcpy (&data_rw, &data, sizeof(data_rw));
  err = dtls_handle_message(dtls_context, dtls_session, data_rw, (int)data_len);

  if (err){
    coap_event_dtls = COAP_EVENT_DTLS_ERROR;
  }

  if (coap_event_dtls >= 0) {
    /* COAP_EVENT_DTLS_CLOSED event reported in coap_session_disconnected() */
    if (coap_event_dtls != COAP_EVENT_DTLS_CLOSED)
      coap_handle_event(session->context, coap_event_dtls, session);
    if (coap_event_dtls == COAP_EVENT_DTLS_CONNECTED)
      coap_session_connected(session);
    else if (coap_event_dtls == COAP_EVENT_DTLS_CLOSED || coap_event_dtls == COAP_EVENT_DTLS_ERROR)
      coap_session_disconnected(session, COAP_NACK_TLS_FAILED);
  }

  return err;
}

int
coap_dtls_hello(coap_session_t *session,
  const uint8_t *data,
  size_t data_len
) {
  session_t dtls_session;
  coap_tiny_context_t *t_context = (coap_tiny_context_t *)session->context->dtls_context;
  dtls_context_t *dtls_context = t_context ? t_context->dtls_context : NULL;
  uint8_t *data_rw;

  assert(dtls_context);
  dtls_session_init(&dtls_session);
  put_session_addr(&session->addr_info.remote, &dtls_session);
  dtls_session.ifindex = session->ifindex;
  /* Need to do this to not get a compiler warning about const parameters */
  memcpy (&data_rw, &data, sizeof(data_rw));
  int res = dtls_handle_message(dtls_context, &dtls_session,
    data_rw, (int)data_len);
  if (res >= 0) {
    if (dtls_get_peer(dtls_context, &dtls_session))
      res = 1;
    else
      res = 0;
  }
  return res;
}

unsigned int coap_dtls_get_overhead(coap_session_t *session) {
  (void)session;
  return 13 + 8 + 8;
}

int coap_tls_is_supported(void) {
  return 0;
}

coap_tls_version_t *
coap_get_tls_library_version(void) {
  static coap_tls_version_t version;
  const char *vers = dtls_package_version();

  version.version = 0;
  if (vers) {
    long int p1, p2 = 0, p3 = 0;
    char* endptr;

    p1 = strtol(vers, &endptr, 10);
    if (*endptr == '.') {
      p2 = strtol(endptr+1, &endptr, 10);
      if (*endptr == '.') {
        p3 = strtol(endptr+1, &endptr, 10);
      }
    }
    version.version = (p1 << 16) | (p2 << 8) | p3;
  }
  version.built_version = version.version;
  version.type = COAP_TLS_LIBRARY_TINYDTLS;
  return &version;
}

#ifdef DTLS_ECC
static const uint8_t b64_6[256] =
  {
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
/*                                             +               / */
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
/* 0   1   2   3   4   5   6   7   8   9               =         */
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
/*     A   B   C   D   E   F   G   H   I   J   K   L   M   N   O */
  64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
/* P   Q   R   S   T   U   V   W   X   Y   Z                     */
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
/*     a   b   c   d   e   f   g   h   i   j   k   l   m   n   o */
  64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
/* p   q   r   s   t   u   v   w   x   y   z                     */
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
  };

/* caller must free off returned coap_binary_t* */
static coap_binary_t *
pem_base64_decode (const uint8_t *data, size_t size)
{
  uint8_t *tbuf = coap_malloc(size);
  size_t nbytesdecoded;
  size_t i;
  coap_binary_t *decoded;
  uint8_t *ptr;
  uint8_t *out;
  size_t nb64bytes = 0;

  for (i = 0; i < size; i++) {
    switch (data[i]) {
    case ' ':
    case '\r':
    case '\n':
    case '\t':
      break;
    default:
      if (b64_6[data[i]] == 64)
        goto end;
      tbuf[nb64bytes++] = data[i];
      break;
    }
  }

end:
  nbytesdecoded = ((nb64bytes + 3) / 4) * 3;
  decoded = coap_new_binary(nbytesdecoded + 1);
  if (!decoded)
    return NULL;

  out = decoded->s;
  ptr = tbuf;

  while (nb64bytes > 4) {
    *(out++) = b64_6[ptr[0]] << 2 | b64_6[ptr[1]] >> 4;
    *(out++) = b64_6[ptr[1]] << 4 | b64_6[ptr[2]] >> 2;
    *(out++) = b64_6[ptr[2]] << 6 | b64_6[ptr[3]];
    ptr += 4;
    nb64bytes -= 4;
  }

  /* Note: (nb64bytes == 1) is an error */
  if (nb64bytes > 1) {
    *(out++) = b64_6[ptr[0]] << 2 | b64_6[ptr[1]] >> 4;
  }
  if (nb64bytes > 2) {
    *(out++) = b64_6[ptr[1]] << 4 | b64_6[ptr[2]] >> 2;
  }
  if (nb64bytes > 3) {
    *(out++) = b64_6[ptr[2]] << 6 | b64_6[ptr[3]];
  }

  decoded->length = nbytesdecoded - ((4 - nb64bytes) & 3);
  coap_free(tbuf);
  return decoded;
}

typedef coap_binary_t * (*asn1_callback)(const uint8_t *data, size_t size);

static int
asn1_verify_privkey(const uint8_t *data, size_t size)
{
  /* Check if we have the private key (with optional leading 0x00) */
  /* skip leading 0x00 */
  if (size - 1 == DTLS_EC_KEY_SIZE && *data == '\000') {
        --size;
        ++data;
  }

  /* Check if we have the private key */
  if (size != DTLS_EC_KEY_SIZE)
    return 0;

  return 1;
}

static int
asn1_verify_pubkey(const uint8_t *data, size_t size)
{
  (void)data;

  /* We have the public key
     (with a leading 0x00 (no unused bits) 0x04 (not compressed() */
  if (size - 2 != 2 * DTLS_EC_KEY_SIZE)
    return 0;

  return 1;
}

static int
asn1_verify_curve(const uint8_t *data, size_t size)
{
  static uint8_t prime256v1_oid[] =
         /* OID 1.2.840.10045.3.1.7 */
         { 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07 };

  /* Check that we have the correct EC (only one supported) */
  if (size != sizeof(prime256v1_oid) ||
      memcmp(data, prime256v1_oid, size) != 0)
    return 0;

  return 1;
}

static int
asn1_verify_pkcs8_version(const uint8_t *data, size_t size)
{
  /* Check that we have the version */
  if (size != 1 || *data != 0)
    return 0;

  return 1;
}

static int
asn1_verify_ec_identifier(const uint8_t *data, size_t size)
{
  static uint8_t ec_public_key_oid[] =
         /* OID 1.2.840.10045.2.1 */
         { 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01 };

  /* Check that we have the correct ecPublicKey */
  if (size != sizeof(ec_public_key_oid) ||
      memcmp(data, ec_public_key_oid, size) != 0)
    return 0;

  return 1;
}

static int
asn1_verify_ec_key(const uint8_t *data, size_t size)
{
  (void)data;

  if (size == 0)
    return 0;

  return 1;
}

static int
asn1_derive_keys(coap_tiny_context_t *t_context,
                 const uint8_t *priv_data, size_t priv_len,
                 const uint8_t *pub_data, size_t pub_len,
                 int is_pkcs8)
{
  coap_binary_t *test;

  t_context->priv_key = get_asn1_tag(COAP_ASN1_OCTETSTRING, priv_data,
                                     priv_len, asn1_verify_privkey);
  if (!t_context->priv_key) {
    coap_log(LOG_INFO, "EC Private Key (RPK) invalid\n");
    return 0;
  }
  /* skip leading 0x00 */
  if (t_context->priv_key->length - 1 == DTLS_EC_KEY_SIZE &&
      t_context->priv_key->s[0] == '\000') {
        t_context->priv_key->length--;
        t_context->priv_key->s++;
  }

  if (!is_pkcs8) {
    /* pkcs8 abstraction tested for valid eliptic curve */
    test = get_asn1_tag(COAP_ASN1_IDENTIFIER, priv_data, priv_len,
                                    asn1_verify_curve);
    if (!test) {
      coap_log(LOG_INFO, "EC Private Key (RPK) invalid elliptic curve\n");
      coap_delete_binary(t_context->priv_key);
      t_context->priv_key = NULL;
      return 0;
    }
    coap_delete_binary(test);
  }

  t_context->pub_key = get_asn1_tag(COAP_ASN1_BITSTRING, pub_data, pub_len,
                                              asn1_verify_pubkey);
  if (!t_context->pub_key) {
    coap_log(LOG_INFO, "EC Public Key (RPK) invalid\n");
    coap_delete_binary(t_context->priv_key);
    t_context->priv_key = NULL;
    return 0;
  }
  /* Drop leading 0x00 and 0x04 */
  t_context->pub_key->s += 2;
  t_context->pub_key->length -= 2;
  dtls_set_handler(t_context->dtls_context, &ec_cb);
  return 1;
}

static coap_binary_t *
ec_abstract_pkcs8_asn1(const uint8_t *asn1_ptr, size_t asn1_length)
{
  coap_binary_t *test;

  test = get_asn1_tag(COAP_ASN1_INTEGER, asn1_ptr, asn1_length,
                               asn1_verify_pkcs8_version);
  if (!test)
    return 0;

  coap_delete_binary(test);

  test = get_asn1_tag(COAP_ASN1_IDENTIFIER, asn1_ptr, asn1_length,
                               asn1_verify_ec_identifier);
  if (!test)
    return 0;
  coap_delete_binary(test);

  test = get_asn1_tag(COAP_ASN1_IDENTIFIER, asn1_ptr, asn1_length,
                               asn1_verify_curve);
  if (!test) {
    coap_log(LOG_INFO, "EC Private Key (RPK) invalid elliptic curve\n");
    return 0;
  }
  coap_delete_binary(test);

  test = get_asn1_tag(COAP_ASN1_OCTETSTRING, asn1_ptr, asn1_length,
                               asn1_verify_ec_key);
  return test;
}

static coap_binary_t *
pem_decode_mem_asn1(const char *begstr, const uint8_t *str)
{
  char *bcp = str ? strstr((const char*)str, begstr) : NULL;
  char *tcp = bcp ? strstr(bcp, "-----END ") : NULL;

  if (bcp && tcp) {
    bcp += strlen(begstr);
    return pem_base64_decode ((const uint8_t *)bcp, tcp - bcp);
  }
  return NULL;
}

#endif /* DTLS_ECC */

int
coap_dtls_context_set_pki(coap_context_t *ctx,
                          const coap_dtls_pki_t* setup_data,
                          const coap_dtls_role_t role COAP_UNUSED
) {
#ifdef DTLS_ECC
  coap_tiny_context_t *t_context;
  coap_binary_t *asn1_priv;
  coap_binary_t *asn1_pub;
  coap_binary_t *asn1_temp;
  int is_pkcs8 = 0;

  if (!setup_data)
    return 0;
  if (setup_data->version != COAP_DTLS_PKI_SETUP_VERSION)
    return 0;
  if (!setup_data->is_rpk_not_cert) {
    coap_log(LOG_WARNING, "Only RPK, not full PKI is supported\n");
    return 0;
  }
  if (!ctx)
    return 0;
  t_context = (coap_tiny_context_t *)ctx->dtls_context;
  if (!t_context)
    return 0;
  if (t_context->priv_key) {
    coap_delete_binary(t_context->priv_key);
    t_context->priv_key = NULL;
  }
  if (t_context->pub_key) {
    coap_delete_binary(t_context->pub_key);
    t_context->pub_key = NULL;
  }
  t_context->setup_data = *setup_data;

  /* All should be RPK only now */
  switch (setup_data->pki_key.key_type) {
  case COAP_PKI_KEY_PEM:
    coap_log(LOG_WARNING, "RPK keys cannot be in COAP_PKI_KEY_PEM format\n");
    break;
  case COAP_PKI_KEY_PEM_BUF:
    if (setup_data->pki_key.key.pem_buf.public_cert &&
        setup_data->pki_key.key.pem_buf.public_cert[0] &&
        setup_data->pki_key.key.pem_buf.private_key &&
        setup_data->pki_key.key.pem_buf.private_key[0]) {
      /* Need to take PEM memory information and convert to binary */
      asn1_priv = pem_decode_mem_asn1("-----BEGIN EC PRIVATE KEY-----",
                                 setup_data->pki_key.key.pem_buf.private_key);
      if (!asn1_priv) {
        asn1_priv = pem_decode_mem_asn1("-----BEGIN PRIVATE KEY-----",
                                 setup_data->pki_key.key.pem_buf.private_key);
        if (!asn1_priv) {
          coap_log(LOG_INFO, "Private Key (RPK) invalid\n");
          return 0;
        }
        asn1_temp = ec_abstract_pkcs8_asn1(asn1_priv->s, asn1_priv->length);
        if (!asn1_temp) {
          coap_log(LOG_INFO, "PKCS#8 Private Key (RPK) invalid\n");
          coap_delete_binary(asn1_priv);
          return 0;
        }
        coap_delete_binary(asn1_priv);
        asn1_priv = asn1_temp;
        is_pkcs8 = 1;
      }
      asn1_pub = pem_decode_mem_asn1(
                                "-----BEGIN PUBLIC KEY-----",
                                 setup_data->pki_key.key.pem_buf.public_cert);
      if (!asn1_pub) {
        asn1_pub = pem_decode_mem_asn1("-----BEGIN EC PRIVATE KEY-----",
                                 setup_data->pki_key.key.pem_buf.private_key);
        if (!asn1_pub) {
          asn1_pub = pem_decode_mem_asn1("-----BEGIN PRIVATE KEY-----",
                                 setup_data->pki_key.key.pem_buf.private_key);
          if (!asn1_pub) {
            coap_log(LOG_INFO, "Public Key (RPK) invalid\n");
            coap_delete_binary(asn1_priv);
            return 0;
          }
          asn1_temp = ec_abstract_pkcs8_asn1(asn1_pub->s, asn1_pub->length);
          if (!asn1_temp) {
            coap_log(LOG_INFO, "PKCS#8 Private Key (RPK) invalid\n");
            coap_delete_binary(asn1_priv);
            coap_delete_binary(asn1_pub);
            return 0;
          }
          coap_delete_binary(asn1_pub);
          asn1_pub = asn1_temp;
          is_pkcs8 = 1;
        }
      }
      if (!asn1_derive_keys(t_context, asn1_priv->s, asn1_priv->length,
                            asn1_pub->s, asn1_pub->length, is_pkcs8)) {
        coap_log(LOG_INFO, "Unable to derive Public/Private Keys\n");
        coap_delete_binary(asn1_priv);
        coap_delete_binary(asn1_pub);
        return 0;
      }
      coap_delete_binary(asn1_priv);
      coap_delete_binary(asn1_pub);
      return 1;
    }
    break;
  case COAP_PKI_KEY_ASN1:
    if (setup_data->pki_key.key.asn1.private_key &&
        setup_data->pki_key.key.asn1.private_key_len &&
         setup_data->pki_key.key.asn1.private_key_type == COAP_ASN1_PKEY_EC) {
      const uint8_t* private_key = setup_data->pki_key.key.asn1.private_key;
      size_t private_key_len = setup_data->pki_key.key.asn1.private_key_len;

      /* Check to see whether this is in pkcs8 format or not */
      asn1_temp = ec_abstract_pkcs8_asn1(
                       setup_data->pki_key.key.asn1.private_key,
                       setup_data->pki_key.key.asn1.private_key_len);
      if (asn1_temp) {
        private_key = asn1_temp->s;
        private_key_len = asn1_temp->length;
        is_pkcs8 = 1;
      }
      /* Need to take ASN1 memory information and convert to binary */
      if (setup_data->pki_key.key.asn1.public_cert &&
          setup_data->pki_key.key.asn1.public_cert_len) {
        if (!asn1_derive_keys(t_context,
                              private_key,
                              private_key_len,
                              setup_data->pki_key.key.asn1.public_cert,
                              setup_data->pki_key.key.asn1.public_cert_len,
                              is_pkcs8)) {
          coap_log(LOG_INFO, "Unable to derive Public/Private Keys\n");
          if (asn1_temp) coap_delete_binary(asn1_temp);
          return 0;
        }
      }
      else {
        if (!asn1_derive_keys(t_context,
                              private_key,
                              private_key_len,
                              private_key,
                              private_key_len,
                              is_pkcs8)) {
          coap_log(LOG_INFO, "Unable to derive Public/Private Keys\n");
          if (asn1_temp) coap_delete_binary(asn1_temp);
          return 0;
        }
      }
      return 1;
    }
    break;
  case COAP_PKI_KEY_PKCS11:
    coap_log(LOG_WARNING, "RPK keys cannot be in COAP_PKI_KEY_PCKS11 format\n");
    break;
  default:
    break;
  }
#else /* ! DTLS_ECC */
  (void)ctx;
  (void)setup_data;
#endif /* ! DTLS_ECC */
  return 0;
}

int
coap_dtls_context_set_pki_root_cas(coap_context_t *ctx COAP_UNUSED,
  const char *ca_file COAP_UNUSED,
  const char *ca_path COAP_UNUSED
) {
  coap_log(LOG_WARNING, "Root CAs PKI not supported\n");
  return 0;
}

int
coap_dtls_context_set_cpsk(coap_context_t *coap_context COAP_UNUSED,
  coap_dtls_cpsk_t *setup_data
) {
  if (!setup_data)
    return 0;

  return 1;
}

int
coap_dtls_context_set_spsk(coap_context_t *coap_context COAP_UNUSED,
  coap_dtls_spsk_t *setup_data
) {
  if (!setup_data)
    return 0;

  if (setup_data->validate_sni_call_back) {
    coap_log(LOG_WARNING,
        "CoAP Server with TinyDTLS does not support SNI selection\n");
  }

  return 1;
}

int
coap_dtls_context_check_keys_enabled(coap_context_t *ctx COAP_UNUSED)
{
  return 1;
}

#if !COAP_DISABLE_TCP
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
#endif /* !COAP_DISABLE_TCP */

coap_digest_ctx_t *
coap_digest_setup(void) {
  dtls_sha256_ctx *digest_ctx = coap_malloc(sizeof(dtls_sha256_ctx));

  if (digest_ctx) {
    dtls_sha256_init(digest_ctx);
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
  dtls_sha256_update(digest_ctx, data, data_len);

  return 1;
}

int
coap_digest_final(coap_digest_ctx_t *digest_ctx,
                  coap_digest_t *digest_buffer) {
  dtls_sha256_final((uint8_t*)digest_buffer, digest_ctx);

  coap_digest_free(digest_ctx);
  return 1;
}

#else /* !HAVE_LIBTINYDTLS */

#ifdef __clang__
/* Make compilers happy that do not like empty modules. As this function is
 * never used, we ignore -Wunused-function at the end of compiling this file
 */
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
static inline void dummy(void) {
}

#endif /* HAVE_LIBTINYDTLS */
