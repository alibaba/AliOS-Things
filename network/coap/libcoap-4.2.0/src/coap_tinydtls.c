/*
 * coap_tinydtls.c -- Datagram Transport Layer Support for libcoap with tinydtls
 *
 * Copyright (C) 2016 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */

#include "coap_config.h"

#ifdef HAVE_LIBTINYDTLS

#include "net.h"
#include "address.h"
#include "coap_debug.h"
#include "mem.h"

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
  coap_context_t *coap_context = (coap_context_t *)dtls_get_app_data(dtls_context);
  coap_session_t *coap_session;
  coap_address_t remote_addr;

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
  coap_context_t *coap_context = (coap_context_t *)dtls_get_app_data(dtls_context);
  coap_session_t *coap_session;
  coap_address_t remote_addr;

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
  coap_context_t *coap_context;
  coap_session_t *coap_session;
  int fatal_error = DTLS_ALERT_INTERNAL_ERROR;
  size_t identity_length;
  static int client = 0;
  static uint8_t psk[128];
  static size_t psk_len = 0;
  coap_address_t remote_addr;


  if (type == DTLS_PSK_KEY && client) {
    if (psk_len > result_length) {
      coap_log(LOG_WARNING, "cannot set psk -- buffer too small\n");
      goto error;
    }
    memcpy(result, psk, psk_len);
    client = 0;
    return (int)psk_len;
  }

  client = 0;
  coap_context = (coap_context_t *)dtls_get_app_data(dtls_context);
  get_session_addr(dtls_session, &remote_addr);
  coap_session = coap_session_get_by_peer(coap_context, &remote_addr, dtls_session->ifindex);
  if (!coap_session) {
    coap_log(LOG_DEBUG, "cannot get PSK, session not found\n");
    goto error;
  }

  switch (type) {
  case DTLS_PSK_IDENTITY:

    if (id_len)
      coap_log(LOG_DEBUG, "got psk_identity_hint: '%.*s'\n", (int)id_len, id);

    if (!coap_context || !coap_context->get_client_psk)
      goto error;

    identity_length = 0;
    psk_len = coap_context->get_client_psk(coap_session, (const uint8_t*)id, id_len, (uint8_t*)result, &identity_length, result_length, psk, sizeof(psk));
    if (!psk_len) {
      coap_log(LOG_WARNING, "no PSK identity for given realm\n");
      fatal_error = DTLS_ALERT_CLOSE_NOTIFY;
      goto error;
    }
    client = 1;
    return (int)identity_length;

  case DTLS_PSK_KEY:
    if (coap_context->get_server_psk)
      return (int)coap_context->get_server_psk(coap_session, (const uint8_t*)id, id_len, (uint8_t*)result, result_length);
    return 0;
    break;

  case DTLS_PSK_HINT:
    client = 0;
    if (coap_context->get_server_hint)
      return (int)coap_context->get_server_hint(coap_session, (uint8_t *)result, result_length);
    return 0;

  default:
    coap_log(LOG_WARNING, "unsupported request type: %d\n", type);
  }

error:
  client = 0;
  return dtls_alert_fatal_create(fatal_error);
}

static dtls_handler_t cb = {
  .write = dtls_send_to_peer,
  .read = dtls_application_data,
  .event = dtls_event,
  .get_psk_info = get_psk_info,
#ifdef WITH_ECC
  .get_ecdsa_key = NULL,
  .verify_ecdsa_key = NULL
#endif
};

void *
coap_dtls_new_context(struct coap_context_t *coap_context) {
  struct dtls_context_t *dtls_context = dtls_new_context(coap_context);
  if (!dtls_context)
    goto error;
  dtls_set_handler(dtls_context, &cb);
  return dtls_context;
error:
  coap_dtls_free_context(dtls_context);
  return NULL;
}

void
coap_dtls_free_context(void *handle) {
  if (handle) {
    struct dtls_context_t *dtls_context = (struct dtls_context_t *)handle;
    dtls_free_context(dtls_context);
  }
}

static session_t *
coap_dtls_new_session(coap_session_t *session) {
  session_t *dtls_session = coap_malloc_type(COAP_DTLS_SESSION, sizeof(session_t));

  if (dtls_session) {
    /* create tinydtls session object from remote address and local
    * endpoint handle */
    dtls_session_init(dtls_session);
    put_session_addr(&session->remote_addr, dtls_session);
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
  session_t *dtls_session = coap_dtls_new_session(session);
  if (!dtls_session)
    return NULL;
  peer =
    dtls_get_peer((struct dtls_context_t *)session->context->dtls_context,
      dtls_session);

  if (!peer) {
    /* The peer connection does not yet exist. */
    /* dtls_connect() returns a value greater than zero if a new
    * connection attempt is made, 0 for session reuse. */
    if (dtls_connect((struct dtls_context_t *)session->context->dtls_context,
      dtls_session) >= 0) {
      peer =
        dtls_get_peer((struct dtls_context_t *)session->context->dtls_context,
          dtls_session);
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
  struct dtls_context_t *ctx = (struct dtls_context_t *)coap_session->context->dtls_context;
  if (coap_session->tls) {
    dtls_peer_t *peer = dtls_get_peer(ctx, (session_t *)coap_session->tls);
    if ( peer )
      dtls_reset_peer(ctx, peer);
    else
      dtls_close(ctx, (session_t *)coap_session->tls);
    coap_log(LOG_DEBUG, "***removed session %p\n", coap_session->tls);
    coap_free_type(COAP_DTLS_SESSION, coap_session->tls);
    coap_session->tls = NULL;
  }
}

int
coap_dtls_send(coap_session_t *session,
  const uint8_t *data,
  size_t data_len
) {
  int res;
  uint8_t *data_rw;

  coap_log(LOG_DEBUG, "call dtls_write\n");

  coap_event_dtls = -1;
  /* Need to do this to not get a compiler warning about const parameters */
  memcpy (&data_rw, &data, sizeof(data_rw));
  res = dtls_write((struct dtls_context_t *)session->context->dtls_context,
    (session_t *)session->tls, data_rw, data_len);

  if (res < 0)
    coap_log(LOG_WARNING, "coap_dtls_send: cannot send PDU\n");

  if (coap_event_dtls >= 0) {
    coap_handle_event(session->context, coap_event_dtls, session);
    if (coap_event_dtls == COAP_EVENT_DTLS_CONNECTED)
      coap_session_connected(session);
    else if (coap_event_dtls == DTLS_ALERT_CLOSE_NOTIFY || coap_event_dtls == COAP_EVENT_DTLS_ERROR)
      coap_session_disconnected(session, COAP_NACK_TLS_FAILED);
  }

  return res;
}

int coap_dtls_is_context_timeout(void) {
  return 1;
}

coap_tick_t coap_dtls_get_context_timeout(void *dtls_context) {
  clock_time_t next = 0;
  dtls_check_retransmit((struct dtls_context_t *)dtls_context, &next);
  if (next > 0)
    return ((coap_tick_t)(next - dtls_tick_0)) * COAP_TICKS_PER_SECOND / DTLS_TICKS_PER_SECOND + coap_tick_0;
  return 0;
}

coap_tick_t coap_dtls_get_timeout(coap_session_t *session) {
  (void)session;
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

  coap_event_dtls = -1;
  /* Need to do this to not get a compiler warning about const parameters */
  memcpy (&data_rw, &data, sizeof(data_rw));
  err = dtls_handle_message(
    (struct dtls_context_t *)session->context->dtls_context,
    dtls_session, data_rw, (int)data_len);

  if (err){
    coap_event_dtls = COAP_EVENT_DTLS_ERROR;
  }

  if (coap_event_dtls >= 0) {
    coap_handle_event(session->context, coap_event_dtls, session);
    if (coap_event_dtls == COAP_EVENT_DTLS_CONNECTED)
      coap_session_connected(session);
    else if (coap_event_dtls == DTLS_ALERT_CLOSE_NOTIFY || coap_event_dtls == COAP_EVENT_DTLS_ERROR)
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
  struct dtls_context_t *dtls_context =
    (struct dtls_context_t *)session->context->dtls_context;
  uint8_t *data_rw;

  dtls_session_init(&dtls_session);
  put_session_addr(&session->remote_addr, &dtls_session);
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

#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else /* __GNUC__ */
#define UNUSED
#endif /* __GNUC__ */

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

int
coap_dtls_context_set_pki(coap_context_t *ctx UNUSED,
  coap_dtls_pki_t* setup_data UNUSED,
  coap_dtls_role_t role UNUSED
) {
  return 0;
}

int
coap_dtls_context_set_pki_root_cas(struct coap_context_t *ctx UNUSED,
  const char *ca_file UNUSED,
  const char *ca_path UNUSED
) {
  return 0;
}

int
coap_dtls_context_set_psk(coap_context_t *ctx UNUSED,
  const char *hint UNUSED,
  coap_dtls_role_t role UNUSED
) {
  return 1;
}

int
coap_dtls_context_check_keys_enabled(coap_context_t *ctx UNUSED)
{
  return 1;
}

void *coap_tls_new_client_session(coap_session_t *session UNUSED, int *connected UNUSED) {
  return NULL;
}

void *coap_tls_new_server_session(coap_session_t *session UNUSED, int *connected UNUSED) {
  return NULL;
}

void coap_tls_free_session(coap_session_t *coap_session UNUSED) {
}

ssize_t coap_tls_write(coap_session_t *session UNUSED,
                       const uint8_t *data UNUSED,
                       size_t data_len UNUSED
) {
  return -1;
}

ssize_t coap_tls_read(coap_session_t *session UNUSED,
                      uint8_t *data UNUSED,
                      size_t data_len UNUSED
) {
  return -1;
}

#undef UNUSED

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
