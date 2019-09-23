/* net.c -- CoAP network interface
 *
 * Copyright (C) 2010--2016 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap_config.h"

#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#elif HAVE_SYS_UNISTD_H
#include <sys/unistd.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_WS2TCPIP_H
#include <ws2tcpip.h>
#endif

#ifdef WITH_LWIP_LIBCOAP
#include <lwip/pbuf.h>
#include <lwip/udp.h>
#include <lwip/timeouts.h>
#endif

#include "libcoap.h"
#include "utlist.h"
#include "coap_debug.h"
#include "coap_mem.h"
#include "str.h"
#include "async.h"
#include "resource.h"
#include "option.h"
#include "encode.h"
#include "block.h"
#include "coap_net.h"
#include "utlist.h"

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

      /**
       * The number of bits for the fractional part of ACK_TIMEOUT and
       * ACK_RANDOM_FACTOR. Must be less or equal 8.
       */
#define FRAC_BITS 6

       /**
        * The maximum number of bits for fixed point integers that are used
        * for retransmission time calculation. Currently this must be @c 8.
        */
#define MAX_BITS 8

#if FRAC_BITS > 8
#error FRAC_BITS must be less or equal 8
#endif

        /** creates a Qx.frac from fval in coap_fixed_point_t */
#define Q(frac,fval) ((uint16_t)(((1 << (frac)) * fval.integer_part) + \
                      ((1 << (frac)) * fval.fractional_part + 500)/1000))

/** creates a Qx.FRAC_BITS from session's 'ack_random_factor' */
#define ACK_RANDOM_FACTOR                                        \
  Q(FRAC_BITS, session->ack_random_factor)

/** creates a Qx.FRAC_BITS from session's 'ack_timeout' */
#define ACK_TIMEOUT Q(FRAC_BITS, session->ack_timeout)

#if !defined(WITH_LWIP_LIBCOAP) && !defined(WITH_CONTIKI)

COAP_STATIC_INLINE coap_queue_t *
coap_malloc_node(void) {
  return (coap_queue_t *)coap_malloc_type(COAP_NODE, sizeof(coap_queue_t));
}

COAP_STATIC_INLINE void
coap_free_node(coap_queue_t *node) {
  coap_free_type(COAP_NODE, node);
}
#endif /* !defined(WITH_LWIP_LIBCOAP) && !defined(WITH_CONTIKI) */

void coap_free_endpoint(coap_endpoint_t *ep);

#ifdef WITH_LWIP_LIBCOAP

#include <lwip/memp.h>

static void coap_retransmittimer_execute(void *arg);
static void coap_retransmittimer_restart(coap_context_t *ctx);

COAP_STATIC_INLINE coap_queue_t *
coap_malloc_node() {
  return (coap_queue_t *)memp_malloc(MEMP_COAP_NODE);
}

COAP_STATIC_INLINE void
coap_free_node(coap_queue_t *node) {
  memp_free(MEMP_COAP_NODE, node);
}

#endif /* WITH_LWIP_LIBCOAP */
#ifdef WITH_CONTIKI
# ifndef DEBUG
#  define DEBUG DEBUG_PRINT
# endif /* DEBUG */

#include "mem.h"
#include "net/ip/uip-debug.h"

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[UIP_LLIPH_LEN])

void coap_resources_init();

unsigned char initialized = 0;
coap_context_t the_coap_context;

PROCESS(coap_retransmit_process, "message retransmit process");

COAP_STATIC_INLINE coap_queue_t *
coap_malloc_node() {
  return (coap_queue_t *)coap_malloc_type(COAP_NODE, 0);
}

COAP_STATIC_INLINE void
coap_free_node(coap_queue_t *node) {
  coap_free_type(COAP_NODE, node);
}
#endif /* WITH_CONTIKI */

unsigned int
coap_adjust_basetime(coap_context_t *ctx, coap_tick_t now) {
  unsigned int result = 0;
  coap_tick_diff_t delta = now - ctx->sendqueue_basetime;

  if (ctx->sendqueue) {
    /* delta < 0 means that the new time stamp is before the old. */
    if (delta <= 0) {
      ctx->sendqueue->t -= delta;
    } else {
      /* This case is more complex: The time must be advanced forward,
       * thus possibly leading to timed out elements at the queue's
       * start. For every element that has timed out, its relative
       * time is set to zero and the result counter is increased. */

      coap_queue_t *q = ctx->sendqueue;
      coap_tick_t t = 0;
      while (q && (t + q->t < (coap_tick_t)delta)) {
        t += q->t;
        q->t = 0;
        result++;
        q = q->next;
      }

      /* finally adjust the first element that has not expired */
      if (q) {
        q->t = (coap_tick_t)delta - t;
      }
    }
  }

  /* adjust basetime */
  ctx->sendqueue_basetime += delta;

  return result;
}

int
coap_insert_node(coap_queue_t **queue, coap_queue_t *node) {
  coap_queue_t *p, *q;
  if (!queue || !node)
    return 0;

  /* set queue head if empty */
  if (!*queue) {
    *queue = node;
    return 1;
  }

  /* replace queue head if PDU's time is less than head's time */
  q = *queue;
  if (node->t < q->t) {
    node->next = q;
    *queue = node;
    q->t -= node->t;                /* make q->t relative to node->t */
    return 1;
  }

  /* search for right place to insert */
  do {
    node->t -= q->t;                /* make node-> relative to q->t */
    p = q;
    q = q->next;
  } while (q && q->t <= node->t);

  /* insert new item */
  if (q) {
    q->t -= node->t;                /* make q->t relative to node->t */
  }
  node->next = q;
  p->next = node;
  return 1;
}

int
coap_delete_node(coap_queue_t *node) {
  if (!node)
    return 0;

  coap_delete_pdu(node->pdu);
  if ( node->session ) {
    /*
     * Need to remove out of context->sendqueue as added in by coap_wait_ack()
     */
    if (node->session->context->sendqueue) {
      LL_DELETE(node->session->context->sendqueue, node);
    }
    coap_session_release(node->session);
  }
  coap_free_node(node);

  return 1;
}

void
coap_delete_all(coap_queue_t *queue) {
  if (!queue)
    return;

  coap_delete_all(queue->next);
  coap_delete_node(queue);
}

coap_queue_t *
coap_new_node(void) {
  coap_queue_t *node;
  node = coap_malloc_node();

  if (!node) {
#ifndef NDEBUG
    coap_log(LOG_WARNING, "coap_new_node: malloc\n");
#endif
    return NULL;
  }

  memset(node, 0, sizeof(*node));
  return node;
}

coap_queue_t *
coap_peek_next(coap_context_t *context) {
  if (!context || !context->sendqueue)
    return NULL;

  return context->sendqueue;
}

coap_queue_t *
coap_pop_next(coap_context_t *context) {
  coap_queue_t *next;

  if (!context || !context->sendqueue)
    return NULL;

  next = context->sendqueue;
  context->sendqueue = context->sendqueue->next;
  if (context->sendqueue) {
    context->sendqueue->t += next->t;
  }
  next->next = NULL;
  return next;
}

static size_t
coap_get_session_client_psk(
  const coap_session_t *session,
  const uint8_t *hint, size_t hint_len,
  uint8_t *identity, size_t *identity_len, size_t max_identity_len,
  uint8_t *psk, size_t max_psk_len
) {
  (void)hint;
  (void)hint_len;
  if (session->psk_identity && session->psk_identity_len > 0 && session->psk_key && session->psk_key_len > 0) {
    if (session->psk_identity_len <= max_identity_len && session->psk_key_len <= max_psk_len) {
      memcpy(identity, session->psk_identity, session->psk_identity_len);
      memcpy(psk, session->psk_key, session->psk_key_len);
      *identity_len = session->psk_identity_len;
      return session->psk_key_len;
    }
  } else if (session->context && session->context->psk_key && session->context->psk_key_len > 0) {
    if (session->context->psk_key_len <= max_psk_len) {
      *identity_len = 0;
      memcpy(psk, session->context->psk_key, session->context->psk_key_len);
      return session->context->psk_key_len;
    }
  }
  *identity_len = 0;
  return 0;
}

static size_t
coap_get_context_server_psk(
  const coap_session_t *session,
  const uint8_t *identity, size_t identity_len,
  uint8_t *psk, size_t max_psk_len
) {
  (void)identity;
  (void)identity_len;
  const coap_context_t *ctx = session->context;
  if (ctx && ctx->psk_key && ctx->psk_key_len > 0 && ctx->psk_key_len <= max_psk_len) {
    memcpy(psk, ctx->psk_key, ctx->psk_key_len);
    return ctx->psk_key_len;
  }
  return 0;
}

static size_t
coap_get_context_server_hint(
  const coap_session_t *session,
  uint8_t *hint, size_t max_hint_len
) {
  const coap_context_t *ctx = session->context;
  if (ctx && ctx->psk_hint && ctx->psk_hint_len > 0 && ctx->psk_hint_len <= max_hint_len) {
    memcpy(hint, ctx->psk_hint, ctx->psk_hint_len);
    return ctx->psk_hint_len;
  }
  return 0;
}

int coap_context_set_psk(coap_context_t *ctx,
  const char *hint,
  const uint8_t *key, size_t key_len
) {

  if (ctx->psk_hint)
    coap_free(ctx->psk_hint);
  ctx->psk_hint = NULL;
  ctx->psk_hint_len = 0;

  if (hint) {
    size_t hint_len = strlen(hint);
    ctx->psk_hint = (uint8_t*)coap_malloc(hint_len);
    if (ctx->psk_hint) {
      memcpy(ctx->psk_hint, hint, hint_len);
      ctx->psk_hint_len = hint_len;
    } else {
      coap_log(LOG_ERR, "No memory to store PSK hint\n");
      return 0;
    }
  }

  if (ctx->psk_key)
    coap_free(ctx->psk_key);
  ctx->psk_key = NULL;
  ctx->psk_key_len = 0;

  if (key && key_len > 0) {
    ctx->psk_key = (uint8_t *)coap_malloc(key_len);
    if (ctx->psk_key) {
      memcpy(ctx->psk_key, key, key_len);
      ctx->psk_key_len = key_len;
    } else {
      coap_log(LOG_ERR, "No memory to store PSK key\n");
      return 0;
    }
  }
  if (coap_dtls_is_supported()) {
    return coap_dtls_context_set_psk(ctx, hint, COAP_DTLS_ROLE_SERVER);
  }
  return 0;
}

int coap_context_set_pki(coap_context_t *ctx,
  coap_dtls_pki_t* setup_data
) {
  if (!setup_data)
    return 0;
  if (setup_data->version != COAP_DTLS_PKI_SETUP_VERSION) {
    coap_log(LOG_ERR, "coap_context_set_pki: Wrong version of setup_data\n");
    return 0;
  }
  if (coap_dtls_is_supported()) {
    return coap_dtls_context_set_pki(ctx, setup_data, COAP_DTLS_ROLE_SERVER);
  }
  return 0;
}

int coap_context_set_pki_root_cas(coap_context_t *ctx,
  const char *ca_file,
  const char *ca_dir
) {
  if (coap_dtls_is_supported()) {
    return coap_dtls_context_set_pki_root_cas(ctx, ca_file, ca_dir);
  }
  return 0;
}

void coap_context_set_keepalive(coap_context_t *context, unsigned int seconds) {
  context->ping_timeout = seconds;
}

coap_context_t *
coap_new_context(
  const coap_address_t *listen_addr) {
  coap_context_t *c;

#ifdef WITH_CONTIKI
  if (initialized)
    return NULL;
#endif /* WITH_CONTIKI */

  coap_startup();

#ifndef WITH_CONTIKI
  c = coap_malloc_type(COAP_CONTEXT, sizeof(coap_context_t));
#endif /* not WITH_CONTIKI */

#ifndef WITH_CONTIKI
  if (!c) {
#ifndef NDEBUG
    coap_log(LOG_EMERG, "coap_init: malloc:\n");
#endif
    return NULL;
  }
#endif /* not WITH_CONTIKI */
#ifdef WITH_CONTIKI
  coap_resources_init();
  coap_memory_init();

  c = &the_coap_context;
  initialized = 1;
#endif /* WITH_CONTIKI */

  memset(c, 0, sizeof(coap_context_t));

  if (coap_dtls_is_supported()) {
    c->dtls_context = coap_dtls_new_context(c);
    if (!c->dtls_context) {
      coap_log(LOG_EMERG, "coap_init: no DTLS context available\n");
      coap_free_context(c);
      return NULL;
    }
  }

  /* set default CSM timeout */
  c->csm_timeout = 30;

  /* initialize message id */
  prng((unsigned char *)&c->message_id, sizeof(uint16_t));

#ifdef LIBCOAP_SERVER_SUPPORT
  if (listen_addr) {
    coap_endpoint_t *endpoint = coap_new_endpoint(c, listen_addr, COAP_PROTO_UDP);
    if (endpoint == NULL) {
      goto onerror;
    }
  }
#endif

#if !defined(WITH_LWIP_LIBCOAP)
  c->network_send = coap_network_send;
  c->network_read = coap_network_read;
#endif

  c->get_client_psk = coap_get_session_client_psk;
  c->get_server_psk = coap_get_context_server_psk;
  c->get_server_hint = coap_get_context_server_hint;

#ifdef WITH_CONTIKI
  process_start(&coap_retransmit_process, (char *)c);

  PROCESS_CONTEXT_BEGIN(&coap_retransmit_process);
#ifndef WITHOUT_OBSERVE
  etimer_set(&c->notify_timer, COAP_RESOURCE_CHECK_TIME * COAP_TICKS_PER_SECOND);
#endif /* WITHOUT_OBSERVE */
  /* the retransmit timer must be initialized to some large value */
  etimer_set(&the_coap_context.retransmit_timer, 0xFFFF);
  PROCESS_CONTEXT_END(&coap_retransmit_process);
#endif /* WITH_CONTIKI */

  return c;

#ifdef LIBCOAP_SERVER_SUPPORT
onerror:
  coap_free_type(COAP_CONTEXT, c);
  return NULL;
#endif
}

void
coap_set_app_data(coap_context_t *ctx, void *app_data) {
  assert(ctx);
  ctx->app = app_data;
}

void *
coap_get_app_data(const coap_context_t *ctx) {
  assert(ctx);
  return ctx->app;
}

void
coap_free_context(coap_context_t *context) {
#ifdef LIBCOAP_SERVER_SUPPORT
  coap_endpoint_t *ep, *tmp;
#endif
  coap_session_t *sp, *stmp;

  if (!context)
    return;

  coap_delete_all(context->sendqueue);

#ifdef WITH_LWIP_LIBCOAP
  context->sendqueue = NULL;
  coap_retransmittimer_restart(context);
#endif

  coap_delete_all_resources(context);

#ifdef LIBCOAP_SERVER_SUPPORT
  LL_FOREACH_SAFE(context->endpoint, ep, tmp) {
    coap_free_endpoint(ep);
  }
#endif

  LL_FOREACH_SAFE(context->sessions, sp, stmp) {
    coap_session_release(sp);
  }

  if (context->dtls_context)
    coap_dtls_free_context(context->dtls_context);

  if (context->psk_hint)
    coap_free(context->psk_hint);

  if (context->psk_key)
    coap_free(context->psk_key);

#ifndef WITH_CONTIKI
  coap_free_type(COAP_CONTEXT, context);
#endif/* not WITH_CONTIKI */
#ifdef WITH_CONTIKI
  memset(&the_coap_context, 0, sizeof(coap_context_t));
  initialized = 0;
#endif /* WITH_CONTIKI */
}

int
coap_option_check_critical(coap_context_t *ctx,
  coap_pdu_t *pdu,
  coap_opt_filter_t unknown) {

  coap_opt_iterator_t opt_iter;
  int ok = 1;

  coap_option_iterator_init(pdu, &opt_iter, COAP_OPT_ALL);

  while (coap_option_next(&opt_iter)) {

    /* The following condition makes use of the fact that
     * coap_option_getb() returns -1 if type exceeds the bit-vector
     * filter. As the vector is supposed to be large enough to hold
     * the largest known option, we know that everything beyond is
     * bad.
     */
    if (opt_iter.type & 0x01) {
      /* first check the built-in critical options */
      switch (opt_iter.type) {
      case COAP_OPTION_IF_MATCH:
      case COAP_OPTION_URI_HOST:
      case COAP_OPTION_IF_NONE_MATCH:
      case COAP_OPTION_URI_PORT:
      case COAP_OPTION_URI_PATH:
      case COAP_OPTION_URI_QUERY:
      case COAP_OPTION_ACCEPT:
      case COAP_OPTION_PROXY_URI:
      case COAP_OPTION_PROXY_SCHEME:
      case COAP_OPTION_BLOCK2:
      case COAP_OPTION_BLOCK1:
        break;
      default:
        if (coap_option_filter_get(ctx->known_options, opt_iter.type) <= 0) {
          coap_log(LOG_DEBUG, "unknown critical option %d\n", opt_iter.type);
          ok = 0;

          /* When opt_iter.type is beyond our known option range,
           * coap_option_filter_set() will return -1 and we are safe to leave
           * this loop. */
          if (coap_option_filter_set(unknown, opt_iter.type) == -1) {
            break;
          }
        }
      }
    }
  }

  return ok;
}

coap_tid_t
coap_send_ack(coap_session_t *session, coap_pdu_t *request) {
  coap_pdu_t *response;
  coap_tid_t result = COAP_INVALID_TID;

  if (request && request->type == COAP_MESSAGE_CON &&
    COAP_PROTO_NOT_RELIABLE(session->proto)) {
    response = coap_pdu_init(COAP_MESSAGE_ACK, 0, request->tid, 0);
    if (response)
      result = coap_send(session, response);
  }
  return result;
}

ssize_t
coap_session_send_pdu(coap_session_t *session, coap_pdu_t *pdu) {
  ssize_t bytes_written = -1;
  assert(pdu->hdr_size > 0);
  switch(session->proto) {
    case COAP_PROTO_UDP:
      bytes_written = coap_session_send(session, pdu->token - pdu->hdr_size,
                                        pdu->used_size + pdu->hdr_size);
      break;
    case COAP_PROTO_DTLS:
      bytes_written = coap_dtls_send(session, pdu->token - pdu->hdr_size,
                                     pdu->used_size + pdu->hdr_size);
      break;
#ifdef LIBCOAP_RELIABLE_CONNECT
    case COAP_PROTO_TCP:
      bytes_written = coap_session_write(session, pdu->token - pdu->hdr_size,
                                         pdu->used_size + pdu->hdr_size);
      break;
    case COAP_PROTO_TLS:
      bytes_written = coap_tls_write(session, pdu->token - pdu->hdr_size,
                                     pdu->used_size + pdu->hdr_size);
      break;
#endif
    default:
      break;
  }
  coap_show_pdu(LOG_DEBUG, pdu);
  return bytes_written;
}

static ssize_t
coap_send_pdu(coap_session_t *session, coap_pdu_t *pdu, coap_queue_t *node) {
  ssize_t bytes_written;

#ifdef WITH_LWIP_LIBCOAP

  coap_socket_t *sock = &session->sock;
  if (sock->flags == COAP_SOCKET_EMPTY) {
    assert(session->endpoint != NULL);
    sock = &session->endpoint->sock;
  }
  if (pdu->type == COAP_MESSAGE_CON && COAP_PROTO_NOT_RELIABLE(session->proto))
    session->con_active++;

  bytes_written = coap_socket_send_pdu(sock, session, pdu);
  if (LOG_DEBUG <= coap_get_log_level()) {
    coap_show_pdu(LOG_DEBUG, pdu);
  }
  coap_ticks(&session->last_rx_tx);

#else

  /* Do not send error responses for requests that were received via
  * IP multicast.
  * FIXME: If No-Response option indicates interest, these responses
  *        must not be dropped. */
  if (coap_is_mcast(&session->local_addr) &&
    COAP_RESPONSE_CLASS(pdu->code) > 2) {
    return COAP_DROPPED_RESPONSE;
  }

  if (session->state == COAP_SESSION_STATE_NONE) {
    if (session->proto == COAP_PROTO_DTLS && !session->tls) {
      session->tls = coap_dtls_new_client_session(session);
      if (session->tls) {
        session->state = COAP_SESSION_STATE_HANDSHAKE;
        return coap_session_delay_pdu(session, pdu, node);
      }
      coap_handle_event(session->context, COAP_EVENT_DTLS_ERROR, session);
      return -1;
    }
#ifdef LIBCOAP_RELIABLE_CONNECT
    else if(COAP_PROTO_RELIABLE(session->proto)) {
      if (!coap_socket_connect_tcp1(
        &session->sock, &session->local_if, &session->remote_addr,
        session->proto == COAP_PROTO_TLS ? COAPS_DEFAULT_PORT : COAP_DEFAULT_PORT,
        &session->local_addr, &session->remote_addr
      )) {
        coap_handle_event(session->context, COAP_EVENT_TCP_FAILED, session);
        return -1;
      }
      session->last_ping = 0;
      session->last_pong = 0;
      session->csm_tx = 0;
      coap_ticks( &session->last_rx_tx );
      if ((session->sock.flags & COAP_SOCKET_WANT_CONNECT) != 0) {
        session->state = COAP_SESSION_STATE_CONNECTING;
        return coap_session_delay_pdu(session, pdu, node);
      }
      coap_handle_event(session->context, COAP_EVENT_TCP_CONNECTED, session);
      if (session->proto == COAP_PROTO_TLS) {
        int connected = 0;
        session->state = COAP_SESSION_STATE_HANDSHAKE;
        session->tls = coap_tls_new_client_session(session, &connected);
        if (session->tls) {
          if (connected) {
            coap_handle_event(session->context, COAP_EVENT_DTLS_CONNECTED, session);
            coap_session_send_csm(session);
          }
          return coap_session_delay_pdu(session, pdu, node);
        }
        coap_handle_event(session->context, COAP_EVENT_DTLS_ERROR, session);
        coap_session_disconnected(session, COAP_NACK_TLS_FAILED);
        return -1;
      } else {
        coap_session_send_csm(session);
      }
    }
#endif
    else {
      return -1;
    }
  }

  if (session->state != COAP_SESSION_STATE_ESTABLISHED ||
      (pdu->type == COAP_MESSAGE_CON && session->con_active >= COAP_DEFAULT_NSTART)) {
    return coap_session_delay_pdu(session, pdu, node);
  }

  if ((session->sock.flags & COAP_SOCKET_NOT_EMPTY) &&
    (session->sock.flags & COAP_SOCKET_WANT_WRITE))
    return coap_session_delay_pdu(session, pdu, node);

  if (pdu->type == COAP_MESSAGE_CON && COAP_PROTO_NOT_RELIABLE(session->proto))
    session->con_active++;

  bytes_written = coap_session_send_pdu(session, pdu);
#endif /* WITH_LWIP_LIBCOAP */

  return bytes_written;
}

coap_tid_t
coap_send_error(coap_session_t *session,
  coap_pdu_t *request,
  unsigned char code,
  coap_opt_filter_t opts) {
  coap_pdu_t *response;
  coap_tid_t result = COAP_INVALID_TID;

  assert(request);
  assert(session);

  response = coap_new_error_response(request, code, opts);
  if (response)
    result = coap_send(session, response);

  return result;
}

coap_tid_t
coap_send_message_type(coap_session_t *session, coap_pdu_t *request, unsigned char type) {
  coap_pdu_t *response;
  coap_tid_t result = COAP_INVALID_TID;

  if (request) {
    response = coap_pdu_init(type, 0, request->tid, 0);
    if (response)
      result = coap_send(session, response);
  }
  return result;
}

/**
 * Calculates the initial timeout based on the session CoAP transmission
 * parameters 'ack_timeout', 'ack_random_factor', and COAP_TICKS_PER_SECOND.
 * The calculation requires 'ack_timeout' and 'ack_random_factor' to be in
 * Qx.FRAC_BITS fixed point notation, whereas the passed parameter @p r
 * is interpreted as the fractional part of a Q0.MAX_BITS random value.
 *
 * @param session session timeout is associated with
 * @param r  random value as fractional part of a Q0.MAX_BITS fixed point
 *           value
 * @return   COAP_TICKS_PER_SECOND * 'ack_timeout' *
 *           (1 + ('ack_random_factor' - 1) * r)
 */
unsigned int
coap_calc_timeout(coap_session_t *session, unsigned char r) {
  unsigned int result;

  /* The integer 1.0 as a Qx.FRAC_BITS */
#define FP1 Q(FRAC_BITS, ((coap_fixed_point_t){1,0}))

  /* rounds val up and right shifts by frac positions */
#define SHR_FP(val,frac) (((val) + (1 << ((frac) - 1))) >> (frac))

  /* Inner term: multiply ACK_RANDOM_FACTOR by Q0.MAX_BITS[r] and
   * make the result a rounded Qx.FRAC_BITS */
  result = SHR_FP((ACK_RANDOM_FACTOR - FP1) * r, MAX_BITS);

  /* Add 1 to the inner term and multiply with ACK_TIMEOUT, then
   * make the result a rounded Qx.FRAC_BITS */
  result = SHR_FP(((result + FP1) * ACK_TIMEOUT), FRAC_BITS);

  /* Multiply with COAP_TICKS_PER_SECOND to yield system ticks
   * (yields a Qx.FRAC_BITS) and shift to get an integer */
  return SHR_FP((COAP_TICKS_PER_SECOND * result), FRAC_BITS);

#undef FP1
#undef SHR_FP
}

coap_tid_t
coap_wait_ack(coap_context_t *context, coap_session_t *session,
              coap_queue_t *node) {
  coap_tick_t now;

  node->session = coap_session_reference(session);

  /* Set timer for pdu retransmission. If this is the first element in
  * the retransmission queue, the base time is set to the current
  * time and the retransmission time is node->timeout. If there is
  * already an entry in the sendqueue, we must check if this node is
  * to be retransmitted earlier. Therefore, node->timeout is first
  * normalized to the base time and then inserted into the queue with
  * an adjusted relative time.
  */
  coap_ticks(&now);
  if (context->sendqueue == NULL) {
    node->t = node->timeout;
    context->sendqueue_basetime = now;
  } else {
    /* make node->t relative to context->sendqueue_basetime */
    node->t = (now - context->sendqueue_basetime) + node->timeout;
  }

  coap_insert_node(&context->sendqueue, node);

#ifdef WITH_LWIP_LIBCOAP
  if (node == context->sendqueue) /* don't bother with timer stuff if there are earlier retransmits */
    coap_retransmittimer_restart(context);
#endif

#ifdef WITH_CONTIKI
  {                            /* (re-)initialize retransmission timer */
    coap_queue_t *nextpdu;

    nextpdu = coap_peek_next(context);
    assert(nextpdu);                /* we have just inserted a node */

                                /* must set timer within the context of the retransmit process */
    PROCESS_CONTEXT_BEGIN(&coap_retransmit_process);
    etimer_set(&context->retransmit_timer, nextpdu->t);
    PROCESS_CONTEXT_END(&coap_retransmit_process);
  }
#endif /* WITH_CONTIKI */

  coap_log(LOG_DEBUG, "** %s: tid=%d added to retransmit queue (%ums)\n",
    coap_session_str(node->session), node->id,
    (unsigned)(node->t * 1000 / COAP_TICKS_PER_SECOND));

  return node->id;
}

coap_tid_t
coap_send(coap_session_t *session, coap_pdu_t *pdu) {
  uint8_t r;
  ssize_t bytes_written;

  if (!coap_pdu_encode_header(pdu, session->proto)) {
    goto error;
  }

  bytes_written = coap_send_pdu( session, pdu, NULL );

  if (bytes_written == COAP_PDU_DELAYED) {
    /* do not free pdu as it is stored with session for later use */
    return pdu->tid;
  }

  if (bytes_written < 0) {
    coap_delete_pdu(pdu);
    return (coap_tid_t)bytes_written;
  }

#ifdef LIBCOAP_RELIABLE_CONNECT
  if (COAP_PROTO_RELIABLE(session->proto) &&
    (size_t)bytes_written < pdu->used_size + pdu->hdr_size) {
    if (coap_session_delay_pdu(session, pdu, NULL) == COAP_PDU_DELAYED) {
      session->partial_write = (size_t)bytes_written;
      /* do not free pdu as it is stored with session for later use */
      return pdu->tid;
    } else {
      goto error;
    }
  }
#endif

  if (pdu->type != COAP_MESSAGE_CON || COAP_PROTO_RELIABLE(session->proto)) {
    coap_tid_t id = pdu->tid;
    coap_delete_pdu(pdu);
    return id;
  }

  coap_queue_t *node = coap_new_node();
  if (!node) {
    coap_log(LOG_DEBUG, "coap_wait_ack: insufficient memory\n");
    goto error;
  }

  node->id = pdu->tid;
  node->pdu = pdu;
  prng(&r, sizeof(r));
  /* add timeout in range [ACK_TIMEOUT...ACK_TIMEOUT * ACK_RANDOM_FACTOR] */
  node->timeout = coap_calc_timeout(session, r);
  return coap_wait_ack(session->context, session, node);
 error:
  coap_delete_pdu(pdu);
  return COAP_INVALID_TID;
}

coap_tid_t
coap_retransmit(coap_context_t *context, coap_queue_t *node) {
  if (!context || !node)
    return COAP_INVALID_TID;

  /* re-initialize timeout when maximum number of retransmissions are not reached yet */
  if (node->retransmit_cnt < node->session->max_retransmit) {
    ssize_t bytes_written;
    coap_tick_t now;

    node->retransmit_cnt++;
    coap_ticks(&now);
    if (context->sendqueue == NULL) {
      node->t = node->timeout << node->retransmit_cnt;
      context->sendqueue_basetime = now;
    } else {
      /* make node->t relative to context->sendqueue_basetime */
      node->t = (now - context->sendqueue_basetime) + (node->timeout << node->retransmit_cnt);
    }
    coap_insert_node(&context->sendqueue, node);
#ifdef WITH_LWIP_LIBCOAP
    if (node == context->sendqueue) /* don't bother with timer stuff if there are earlier retransmits */
      coap_retransmittimer_restart(context);
#endif

    coap_log(LOG_DEBUG, "** %s: tid=%d: retransmission #%d\n",
             coap_session_str(node->session), node->id, node->retransmit_cnt);

    if (node->session->con_active)
      node->session->con_active--;
    bytes_written = coap_send_pdu(node->session, node->pdu, node);

    if (bytes_written == COAP_PDU_DELAYED) {
      /* PDU was not retransmitted immediately because a new handshake is
         in progress. node was moved to the send queue of the session. */
      return node->id;
    }

    if (bytes_written < 0)
      return (int)bytes_written;

    return node->id;
  }

  /* no more retransmissions, remove node from system */

#ifndef WITH_CONTIKI
  coap_log(LOG_DEBUG, "** %s: tid=%d: give up after %d attempts\n",
           coap_session_str(node->session), node->id, node->retransmit_cnt);
#endif

#ifndef WITHOUT_OBSERVE
  /* Check if subscriptions exist that should be canceled after
     COAP_MAX_NOTIFY_FAILURES */
  if (node->pdu->code >= 64) {
    coap_binary_t token = { 0, NULL };

    token.length = node->pdu->token_length;
    token.s = node->pdu->token;

    coap_handle_failed_notify(context, node->session, &token);
  }
#endif /* WITHOUT_OBSERVE */
  if (node->session->con_active) {
    node->session->con_active--;
    if (node->session->state == COAP_SESSION_STATE_ESTABLISHED) {
      /*
       * As there may be another CON in a different queue entry on the same
       * session that needs to be immediately released,
       * coap_session_connected() is called.
       * However, there is the possibility coap_wait_ack() may be called for
       * this node (queue) and re-added to context->sendqueue.
       * coap_delete_node(node) called shortly will handle this and remove it.
       */
      coap_session_connected(node->session);
    }
 }

  /* And finally delete the node */
  if (node->pdu->type == COAP_MESSAGE_CON && context->nack_handler)
    context->nack_handler(context, node->session, node->pdu, COAP_NACK_TOO_MANY_RETRIES, node->id);
  coap_delete_node(node);
  return COAP_INVALID_TID;
}

#ifdef WITH_LWIP_LIBCOAP
/* WITH_LWIP_LIBCOAP, this is handled by coap_recv in a different way */
void
coap_read(coap_context_t *ctx, coap_tick_t now) {
  return;
}
#else /* WITH_LWIP_LIBCOAP */

static int
coap_handle_dgram_for_proto(coap_context_t *ctx, coap_session_t *session, coap_packet_t *packet) {
  uint8_t *data;
  size_t data_len;
  int result = -1;

  coap_packet_get_memmapped(packet, &data, &data_len);

  if (session->proto == COAP_PROTO_DTLS) {
    if (session->type == COAP_SESSION_TYPE_HELLO)
      result = coap_dtls_hello(session, data, data_len);
    else if (session->tls)
      result = coap_dtls_receive(session, data, data_len);
  } else if (session->proto == COAP_PROTO_UDP) {
    result = coap_handle_dgram(ctx, session, data, data_len);
  }
  return result;
}

#ifdef LIBCOAP_RELIABLE_CONNECT
static void
coap_connect_session(coap_context_t *ctx, coap_session_t *session, coap_tick_t now) {
  (void)ctx;
  if (coap_socket_connect_tcp2(&session->sock, &session->local_addr, &session->remote_addr)) {
    session->last_rx_tx = now;
    coap_handle_event(session->context, COAP_EVENT_TCP_CONNECTED, session);
    if (session->proto == COAP_PROTO_TCP) {
      coap_session_send_csm(session);
    } else if (session->proto == COAP_PROTO_TLS) {
      int connected = 0;
      session->state = COAP_SESSION_STATE_HANDSHAKE;
      session->tls = coap_tls_new_client_session(session, &connected);
      if (session->tls) {
        if (connected) {
          coap_handle_event(session->context, COAP_EVENT_DTLS_CONNECTED, session);
          coap_session_send_csm(session);
        }
      } else {
        coap_handle_event(session->context, COAP_EVENT_DTLS_ERROR, session);
        coap_session_disconnected(session, COAP_NACK_TLS_FAILED);
      }
    }
  } else {
    coap_handle_event(session->context, COAP_EVENT_TCP_FAILED, session);
    coap_session_disconnected(session, COAP_NACK_NOT_DELIVERABLE);
  }
}
#endif

#if defined(LIBCOAP_SERVER_SUPPORT) || defined(LIBCOAP_RELIABLE_CONNECT)
static void
coap_write_session(coap_context_t *ctx, coap_session_t *session, coap_tick_t now) {
  (void)ctx;
  assert(session->sock.flags & COAP_SOCKET_CONNECTED);

  while (session->delayqueue) {
    ssize_t bytes_written;
    coap_queue_t *q = session->delayqueue;
    coap_log(LOG_DEBUG, "** %s: tid=%d: transmitted after delay\n",
             coap_session_str(session), (int)q->pdu->tid);
    assert(session->partial_write < q->pdu->used_size + q->pdu->hdr_size);
    switch (session->proto) {
      case COAP_PROTO_TCP:
        bytes_written = coap_session_write(
          session,
          q->pdu->token - q->pdu->hdr_size - session->partial_write,
          q->pdu->used_size + q->pdu->hdr_size - session->partial_write
        );
        break;
      case COAP_PROTO_TLS:
        bytes_written = coap_tls_write(
          session,
          q->pdu->token - q->pdu->hdr_size - session->partial_write,
          q->pdu->used_size + q->pdu->hdr_size - session->partial_write
        );
        break;
      default:
        bytes_written = -1;
        break;
    }
    if (bytes_written > 0)
      session->last_rx_tx = now;
    if (bytes_written <= 0 || (size_t)bytes_written < q->pdu->used_size + q->pdu->hdr_size - session->partial_write) {
      if (bytes_written > 0)
        session->partial_write += (size_t)bytes_written;
      break;
    }
    session->delayqueue = q->next;
    session->partial_write = 0;
    coap_delete_node(q);
  }
}
#endif

#ifdef WITH_CONTIKI
COAP_STATIC_INLINE coap_packet_t *
coap_malloc_packet(void) {
  return (coap_packet_t *)coap_malloc_type(COAP_PACKET, 0);
}

void
coap_free_packet(coap_packet_t *packet) {
  coap_free_type(COAP_PACKET, packet);
}
#endif /* WITH_CONTIKI */

static void
coap_read_session(coap_context_t *ctx, coap_session_t *session, coap_tick_t now) {
#ifdef WITH_CONTIKI
  coap_packet_t *packet = coap_malloc_packet();
  if ( !packet )
    return;
#else /* WITH_CONTIKI */
  coap_packet_t s_packet;
  coap_packet_t *packet = &s_packet;
#endif /* WITH_CONTIKI */

  assert(session->sock.flags & (COAP_SOCKET_CONNECTED | COAP_SOCKET_MULTICAST));

  if (COAP_PROTO_NOT_RELIABLE(session->proto)) {
    ssize_t bytes_read;
    coap_address_copy(&packet->src, &session->remote_addr);
    coap_address_copy(&packet->dst, &session->local_addr);
    bytes_read = ctx->network_read(&session->sock, packet);

    if (bytes_read < 0) {
      if (bytes_read == -2)
        coap_session_disconnected(session, COAP_NACK_RST);
      else
        coap_log(LOG_WARNING, "*  %s: read error\n",
                 coap_session_str(session));
    } else if (bytes_read > 0) {
      coap_log(LOG_DEBUG, "*  %s: received %zd bytes\n",
               coap_session_str(session), bytes_read);
      session->last_rx_tx = now;
      coap_packet_set_addr(packet, &session->remote_addr, &session->local_addr);
      coap_handle_dgram_for_proto(ctx, session, packet);
    }
  }
#ifdef LIBCOAP_RELIABLE_CONNECT
  else {
    ssize_t bytes_read = 0;
    const uint8_t *p;
    int retry;
    /* adjust for LWIP */
    uint8_t *buf = packet->payload;
    size_t buf_len = sizeof(packet->payload);

    do {
      if (session->proto == COAP_PROTO_TCP)
        bytes_read = coap_socket_read(&session->sock, buf, buf_len);
      else if (session->proto == COAP_PROTO_TLS)
        bytes_read = coap_tls_read(session, buf, buf_len);
      if (bytes_read > 0) {
        coap_log(LOG_DEBUG, "*  %s: received %zd bytes\n",
                 coap_session_str(session), bytes_read);
        session->last_rx_tx = now;
      }
      p = buf;
      retry = bytes_read == (ssize_t)buf_len;
      while (bytes_read > 0) {
        if (session->partial_pdu) {
          size_t len = session->partial_pdu->used_size
                     + session->partial_pdu->hdr_size
                     - session->partial_read;
          size_t n = min(len, (size_t)bytes_read);
          memcpy(session->partial_pdu->token - session->partial_pdu->hdr_size
                 + session->partial_read, p, n);
          p += n;
          bytes_read -= n;
          if (n == len) {
            if (coap_pdu_parse_header(session->partial_pdu, session->proto)
              && coap_pdu_parse_opt(session->partial_pdu)) {
              coap_dispatch(ctx, session, session->partial_pdu);
            }
            coap_delete_pdu(session->partial_pdu);
            session->partial_pdu = NULL;
            session->partial_read = 0;
          } else {
            session->partial_read += n;
          }
        } else if (session->partial_read > 0) {
          size_t hdr_size = coap_pdu_parse_header_size(session->proto,
            session->read_header);
          size_t len = hdr_size - session->partial_read;
          size_t n = min(len, (size_t)bytes_read);
          memcpy(session->read_header + session->partial_read, p, n);
          p += n;
          bytes_read -= n;
          if (n == len) {
            size_t size = coap_pdu_parse_size(session->proto, session->read_header,
              hdr_size);
            session->partial_pdu = coap_pdu_init(0, 0, 0, size);
            if (session->partial_pdu == NULL) {
              bytes_read = -1;
              break;
            }
            if (session->partial_pdu->alloc_size < size && !coap_pdu_resize(session->partial_pdu, size)) {
              bytes_read = -1;
              break;
            }
            session->partial_pdu->hdr_size = (uint8_t)hdr_size;
            session->partial_pdu->used_size = size;
            memcpy(session->partial_pdu->token - hdr_size, session->read_header, hdr_size);
            session->partial_read = hdr_size;
            if (size == 0) {
              if (coap_pdu_parse_header(session->partial_pdu, session->proto)) {
                coap_dispatch(ctx, session, session->partial_pdu);
              }
              coap_delete_pdu(session->partial_pdu);
              session->partial_pdu = NULL;
              session->partial_read = 0;
            }
          } else {
            session->partial_read += bytes_read;
          }
        } else {
          session->read_header[0] = *p++;
          bytes_read -= 1;
          if (!coap_pdu_parse_header_size(session->proto,
            session->read_header)) {
            bytes_read = -1;
            break;
          }
          session->partial_read = 1;
        }
      }
    } while (bytes_read == 0 && retry);
    if (bytes_read < 0)
      coap_session_disconnected(session, COAP_NACK_NOT_DELIVERABLE);
  }
#endif

#ifdef WITH_CONTIKI
  if ( packet )
    coap_free_packet( packet );
#endif
}

#ifdef LIBCOAP_SERVER_SUPPORT
static int
coap_read_endpoint(coap_context_t *ctx, coap_endpoint_t *endpoint, coap_tick_t now) {
  ssize_t bytes_read = -1;
  int result = -1;                /* the value to be returned */
#ifdef WITH_CONTIKI
  coap_packet_t *packet = coap_malloc_packet();
#else /* WITH_CONTIKI */
  coap_packet_t s_packet;
  coap_packet_t *packet = &s_packet;
#endif /* WITH_CONTIKI */

  assert(COAP_PROTO_NOT_RELIABLE(endpoint->proto));
  assert(endpoint->sock.flags & COAP_SOCKET_BOUND);

  if (packet) {
    coap_address_init(&packet->src);
    coap_address_copy(&packet->dst, &endpoint->bind_addr);
    bytes_read = ctx->network_read(&endpoint->sock, packet);
  }
  else {
    coap_log(LOG_WARNING, "*  %s: Packet allocation failed\n",
             coap_endpoint_str(endpoint));
    return -1;
  }

  if (bytes_read < 0) {
    coap_log(LOG_WARNING, "*  %s: read failed\n", coap_endpoint_str(endpoint));
  } else if (bytes_read > 0) {
    coap_session_t *session = coap_endpoint_get_session(endpoint, packet, now);
    if (session) {
      coap_log(LOG_DEBUG, "*  %s: received %zd bytes\n",
               coap_session_str(session), bytes_read);
      result = coap_handle_dgram_for_proto(ctx, session, packet);
      if (endpoint->proto == COAP_PROTO_DTLS && session->type == COAP_SESSION_TYPE_HELLO && result == 1)
        coap_endpoint_new_dtls_session(endpoint, packet, now);
    }
  }

#ifdef WITH_CONTIKI
  if (packet)
    coap_free_packet(packet);
#endif

  return result;
}

static int
coap_write_endpoint(coap_context_t *ctx, coap_endpoint_t *endpoint, coap_tick_t now) {
  (void)ctx;
  (void)endpoint;
  (void)now;
  return 0;
}

static int
coap_accept_endpoint(coap_context_t *ctx, coap_endpoint_t *endpoint,
  coap_tick_t now) {
  coap_session_t *session = coap_new_server_session(ctx, endpoint);
  if (session)
    session->last_rx_tx = now;
  return session != NULL;
}
#endif

void
coap_read(coap_context_t *ctx, coap_tick_t now) {
  coap_session_t *s, *tmp_s;

#ifdef LIBCOAP_SERVER_SUPPORT
  coap_endpoint_t *ep, *tmp;
  LL_FOREACH_SAFE(ctx->endpoint, ep, tmp) {
    if ((ep->sock.flags & COAP_SOCKET_CAN_READ) != 0)
      coap_read_endpoint(ctx, ep, now);
    if ((ep->sock.flags & COAP_SOCKET_CAN_WRITE) != 0)
      coap_write_endpoint(ctx, ep, now);
    if ((ep->sock.flags & COAP_SOCKET_CAN_ACCEPT) != 0)
      coap_accept_endpoint(ctx, ep, now);
    LL_FOREACH_SAFE(ep->sessions, s, tmp_s) {
      if ((s->sock.flags & COAP_SOCKET_CAN_READ) != 0) {
        /* Make sure the session object is not deleted in one of the callbacks  */
        coap_session_reference(s);
        coap_read_session(ctx, s, now);
        coap_session_release(s);
      }
      if ((s->sock.flags & COAP_SOCKET_CAN_WRITE) != 0) {
        /* Make sure the session object is not deleted in one of the callbacks  */
        coap_session_reference(s);
        coap_write_session(ctx, s, now);
        coap_session_release(s);
      }
    }
  }
#endif

  LL_FOREACH_SAFE(ctx->sessions, s, tmp_s) {
#ifdef LIBCOAP_RELIABLE_CONNECT
    if ((s->sock.flags & COAP_SOCKET_CAN_CONNECT) != 0) {
      /* Make sure the session object is not deleted in one of the callbacks  */
      coap_session_reference(s);
      coap_connect_session(ctx, s, now);
      coap_session_release( s );
    }
#endif
    if ((s->sock.flags & COAP_SOCKET_CAN_READ) != 0) {
      /* Make sure the session object is not deleted in one of the callbacks  */
      coap_session_reference(s);
      coap_read_session(ctx, s, now);
      coap_session_release(s);
    }
#ifdef LIBCOAP_RELIABLE_CONNECT
    if ((s->sock.flags & COAP_SOCKET_CAN_WRITE) != 0) {
      /* Make sure the session object is not deleted in one of the callbacks  */
      coap_session_reference(s);
      coap_write_session(ctx, s, now);
      coap_session_release( s );
    }
#endif
  }
}

int
coap_handle_dgram(coap_context_t *ctx, coap_session_t *session,
  uint8_t *msg, size_t msg_len) {

  coap_pdu_t *pdu = NULL;

  assert(COAP_PROTO_NOT_RELIABLE(session->proto));

  /* Reserved more payload for decrypt data.
     Change 'msg_len - 4' to 2*(msg_len - 4) */
  pdu = coap_pdu_init(0, 0, 0, (msg_len - 4)*2);
  if (!pdu)
    goto error;

  if (!coap_pdu_parse(session->proto, msg, msg_len, pdu)) {
    coap_log(LOG_WARNING, "discard malformed PDU\n");
    goto error;
  }

  coap_dispatch(ctx, session, pdu);
  coap_delete_pdu(pdu);
  return 0;

error:
  /* FIXME: send back RST? */
  coap_delete_pdu(pdu);
  return -1;
}
#endif /* not WITH_LWIP_LIBCOAP */

int
coap_remove_from_queue(coap_queue_t **queue, coap_session_t *session, coap_tid_t id, coap_queue_t **node) {
  coap_queue_t *p, *q;

  if (!queue || !*queue)
    return 0;

  /* replace queue head if PDU's time is less than head's time */

  if (session == (*queue)->session && id == (*queue)->id) { /* found transaction */
    *node = *queue;
    *queue = (*queue)->next;
    if (*queue) {          /* adjust relative time of new queue head */
      (*queue)->t += (*node)->t;
    }
    (*node)->next = NULL;
    coap_log(LOG_DEBUG, "** %s: tid=%d: removed\n",
             coap_session_str(session), id);
    return 1;
  }

  /* search transaction to remove (only first occurence will be removed) */
  q = *queue;
  do {
    p = q;
    q = q->next;
  } while (q && (session != q->session || id != q->id));

  if (q) {                        /* found transaction */
    p->next = q->next;
    if (p->next) {                /* must update relative time of p->next */
      p->next->t += q->t;
    }
    q->next = NULL;
    *node = q;
    coap_log(LOG_DEBUG, "** %s: tid=%d: removed\n",
             coap_session_str(session), id);
    return 1;
  }

  return 0;

}

COAP_STATIC_INLINE int
token_match(const uint8_t *a, size_t alen,
  const uint8_t *b, size_t blen) {
  return alen == blen && (alen == 0 || memcmp(a, b, alen) == 0);
}

void
coap_cancel_session_messages(coap_context_t *context, coap_session_t *session,
  coap_nack_reason_t reason) {
  coap_queue_t *p, *q;

  while (context->sendqueue && context->sendqueue->session == session) {
    q = context->sendqueue;
    context->sendqueue = q->next;
    coap_log(LOG_DEBUG, "** %s: tid=%d: removed\n",
             coap_session_str(session), q->id);
    if (q->pdu->type == COAP_MESSAGE_CON && context->nack_handler)
      context->nack_handler(context, session, q->pdu, reason, q->id);
    coap_delete_node(q);
  }

  if (!context->sendqueue)
    return;

  p = context->sendqueue;
  q = p->next;

  while (q) {
    if (q->session == session) {
      p->next = q->next;
      coap_log(LOG_DEBUG, "** %s: tid=%d: removed\n",
               coap_session_str(session), q->id);
      if (q->pdu->type == COAP_MESSAGE_CON && context->nack_handler)
        context->nack_handler(context, session, q->pdu, reason, q->id);
      coap_delete_node(q);
      q = p->next;
    } else {
      p = q;
      q = q->next;
    }
  }
}

void
coap_cancel_all_messages(coap_context_t *context, coap_session_t *session,
  const uint8_t *token, size_t token_length) {
  /* cancel all messages in sendqueue that belong to session
   * and use the specified token */
  coap_queue_t *p, *q;

  while (context->sendqueue && context->sendqueue->session == session &&
    token_match(token, token_length,
      context->sendqueue->pdu->token,
      context->sendqueue->pdu->token_length)) {
    q = context->sendqueue;
    context->sendqueue = q->next;
    coap_log(LOG_DEBUG, "** %s: tid=%d: removed\n",
             coap_session_str(session), q->id);
    coap_delete_node(q);
  }

  if (!context->sendqueue)
    return;

  p = context->sendqueue;
  q = p->next;

  /* when q is not NULL, it does not match (dst, token), so we can skip it */
  while (q) {
    if (q->session == session &&
      token_match(token, token_length,
        q->pdu->token, q->pdu->token_length)) {
      p->next = q->next;
      coap_log(LOG_DEBUG, "** %s: tid=%d: removed\n",
               coap_session_str(session), q->id);
      coap_delete_node(q);
      q = p->next;
    } else {
      p = q;
      q = q->next;
    }
  }
}

coap_queue_t *
coap_find_transaction(coap_queue_t *queue, coap_session_t *session, coap_tid_t id) {
  while (queue && queue->session != session && queue->id != id)
    queue = queue->next;

  return queue;
}

coap_pdu_t *
coap_new_error_response(coap_pdu_t *request, unsigned char code,
  coap_opt_filter_t opts) {
  coap_opt_iterator_t opt_iter;
  coap_pdu_t *response;
  size_t size = request->token_length;
  unsigned char type;
  coap_opt_t *option;
  uint16_t opt_type = 0;        /* used for calculating delta-storage */

#if COAP_ERROR_PHRASE_LENGTH > 0
  const char *phrase = coap_response_phrase(code);

  /* Need some more space for the error phrase and payload start marker */
  if (phrase)
    size += strlen(phrase) + 1;
#endif

  assert(request);

  /* cannot send ACK if original request was not confirmable */
  type = request->type == COAP_MESSAGE_CON
    ? COAP_MESSAGE_ACK
    : COAP_MESSAGE_NON;

  /* Estimate how much space we need for options to copy from
   * request. We always need the Token, for 4.02 the unknown critical
   * options must be included as well. */
  coap_option_clrb(opts, COAP_OPTION_CONTENT_TYPE); /* we do not want this */

  coap_option_iterator_init(request, &opt_iter, opts);

  /* Add size of each unknown critical option. As known critical
     options as well as elective options are not copied, the delta
     value might grow.
   */
  while ((option = coap_option_next(&opt_iter))) {
    uint16_t delta = opt_iter.type - opt_type;
    /* calculate space required to encode (opt_iter.type - opt_type) */
    if (delta < 13) {
      size++;
    } else if (delta < 269) {
      size += 2;
    } else {
      size += 3;
    }

    /* add coap_opt_length(option) and the number of additional bytes
     * required to encode the option length */

    size += coap_opt_length(option);
    switch (*option & 0x0f) {
    case 0x0e:
      size++;
      /* fall through */
    case 0x0d:
      size++;
      break;
    default:
      ;
    }

    opt_type = opt_iter.type;
  }

  /* Now create the response and fill with options and payload data. */
  response = coap_pdu_init(type, code, request->tid, size);
  if (response) {
    /* copy token */
    if (!coap_add_token(response, request->token_length,
      request->token)) {
      coap_log(LOG_DEBUG, "cannot add token to error response\n");
      coap_delete_pdu(response);
      return NULL;
    }

    /* copy all options */
    coap_option_iterator_init(request, &opt_iter, opts);
    while ((option = coap_option_next(&opt_iter))) {
      coap_add_option(response, opt_iter.type,
        coap_opt_length(option),
        coap_opt_value(option));
    }

#if COAP_ERROR_PHRASE_LENGTH > 0
    /* note that diagnostic messages do not need a Content-Format option. */
    if (phrase){
      coap_add_data(response, (size_t)strlen(phrase), (const uint8_t *)phrase);
    }
#endif
  }

  return response;
}

/**
 * Quick hack to determine the size of the resource description for
 * .well-known/core.
 */
COAP_STATIC_INLINE size_t
get_wkc_len(coap_context_t *context, coap_opt_t *query_filter) {
  unsigned char buf[1];
  size_t len = 0;

  if (coap_print_wellknown(context, buf, &len, UINT_MAX, query_filter)
    & COAP_PRINT_STATUS_ERROR) {
    coap_log(LOG_WARNING, "cannot determine length of /.well-known/core\n");
    return 0;
  }

  coap_log(LOG_DEBUG, "get_wkc_len: coap_print_wellknown() returned %zu\n", len);

  return len;
}

#define SZX_TO_BYTES(SZX) ((size_t)(1 << ((SZX) + 4)))

coap_pdu_t *
coap_wellknown_response(coap_context_t *context, coap_session_t *session,
  coap_pdu_t *request) {
  coap_pdu_t *resp;
  coap_opt_iterator_t opt_iter;
  size_t len, wkc_len;
  uint8_t buf[2];
  int result = 0;
  int need_block2 = 0;           /* set to 1 if Block2 option is required */
  coap_block_t block;
  coap_opt_t *query_filter;
  size_t offset = 0;
  uint8_t *data;

  resp = coap_pdu_init(request->type == COAP_MESSAGE_CON
    ? COAP_MESSAGE_ACK
    : COAP_MESSAGE_NON,
    COAP_RESPONSE_CODE(205),
    request->tid, coap_session_max_pdu_size(session));
  if (!resp) {
    coap_log(LOG_DEBUG, "coap_wellknown_response: cannot create PDU\n");
    return NULL;
  }

  if (!coap_add_token(resp, request->token_length, request->token)) {
    coap_log(LOG_DEBUG, "coap_wellknown_response: cannot add token\n");
    goto error;
  }

  query_filter = coap_check_option(request, COAP_OPTION_URI_QUERY, &opt_iter);
  wkc_len = get_wkc_len(context, query_filter);

  /* The value of some resources is undefined and get_wkc_len will return 0.*/
  if (wkc_len == 0) {
    coap_log(LOG_DEBUG, "coap_wellknown_response: undefined resource\n");
    /* set error code 4.00 Bad Request*/
    resp->code = COAP_RESPONSE_CODE(400);
    resp->used_size = resp->token_length;
    return resp;
  }

  /* check whether the request contains the Block2 option */
  if (coap_get_block(request, COAP_OPTION_BLOCK2, &block)) {
    coap_log(LOG_DEBUG, "create block\n");
    offset = block.num << (block.szx + 4);
    if (block.szx > 6) {  /* invalid, MUST lead to 4.00 Bad Request */
      resp->code = COAP_RESPONSE_CODE(400);
      return resp;
    } else if (block.szx > COAP_MAX_BLOCK_SZX) {
      block.szx = COAP_MAX_BLOCK_SZX;
      block.num = (unsigned int)(offset >> (block.szx + 4));
    }

    need_block2 = 1;
  }

  /* Check if there is sufficient space to add Content-Format option
   * and data. We do this before adding the Content-Format option to
   * avoid sending error responses with that option but no actual
   * content. */
  if (resp->max_size && resp->max_size <= resp->used_size + 3) {
    coap_log(LOG_DEBUG, "coap_wellknown_response: insufficient storage space\n");
    goto error;
  }

  /* Add Content-Format. As we have checked for available storage,
   * nothing should go wrong here. */
  assert(coap_encode_var_safe(buf, sizeof(buf),
    COAP_MEDIATYPE_APPLICATION_LINK_FORMAT) == 1);
  coap_add_option(resp, COAP_OPTION_CONTENT_FORMAT,
    coap_encode_var_safe(buf, sizeof(buf),
      COAP_MEDIATYPE_APPLICATION_LINK_FORMAT), buf);

  /* check if Block2 option is required even if not requested */
  if (!need_block2 && resp->max_size && resp->max_size - resp->used_size < wkc_len + 1) {
    assert(resp->used_size <= resp->max_size);
    const size_t payloadlen = resp->max_size - resp->used_size;
    /* yes, need block-wise transfer */
    block.num = 0;
    block.m = 0;      /* the M bit is set by coap_write_block_opt() */
    block.szx = COAP_MAX_BLOCK_SZX;
    while (payloadlen < SZX_TO_BYTES(block.szx) + 6) {
      if (block.szx == 0) {
        coap_log(LOG_DEBUG,
             "coap_wellknown_response: message to small even for szx == 0\n");
        goto error;
      } else {
        block.szx--;
      }
    }

    need_block2 = 1;
  }

  /* write Block2 option if necessary */
  if (need_block2) {
    if (coap_write_block_opt(&block, COAP_OPTION_BLOCK2, resp, wkc_len) < 0) {
      coap_log(LOG_DEBUG,
               "coap_wellknown_response: cannot add Block2 option\n");
      goto error;
    }
  }

  len = need_block2 ? SZX_TO_BYTES( block.szx ) :
        resp->max_size && resp->used_size + wkc_len + 1 > resp->max_size ?
        resp->max_size - resp->used_size - 1 : wkc_len;
  data = coap_add_data_after(resp, len);
  if (!data) {
    coap_log(LOG_DEBUG, "coap_wellknown_response: coap_add_data failed\n" );
    goto error;
  }

  result = coap_print_wellknown(context, data, &len, offset, query_filter);
  if ((result & COAP_PRINT_STATUS_ERROR) != 0) {
    coap_log(LOG_DEBUG, "coap_print_wellknown failed\n");
    goto error;
  }

  return resp;

error:
  /* set error code 5.03 and remove all options and data from response */
  resp->code = COAP_RESPONSE_CODE(503);
  resp->used_size = resp->token_length;
  return resp;
}

/**
 * This function cancels outstanding messages for the session and
 * token specified in @p sent. Any observation relationship for
 * sent->session and the token are removed. Calling this function is
 * required when receiving an RST message (usually in response to a
 * notification) or a GET request with the Observe option set to 1.
 *
 * This function returns @c 0 when the token is unknown with this
 * peer, or a value greater than zero otherwise.
 */
static int
coap_cancel(coap_context_t *context, const coap_queue_t *sent) {
#ifndef WITHOUT_OBSERVE
  coap_binary_t token = { 0, NULL };
  int num_cancelled = 0;    /* the number of observers cancelled */

  /* remove observer for this resource, if any
   * get token from sent and try to find a matching resource. Uh!
   */

  COAP_SET_STR(&token, sent->pdu->token_length, sent->pdu->token);

  RESOURCES_ITER(context->resources, r) {
    num_cancelled += coap_delete_observer(r, sent->session, &token);
    coap_cancel_all_messages(context, sent->session, token.s, token.length);
  }

  return num_cancelled;
#else /* WITOUT_OBSERVE */
  return 0;
#endif /* WITOUT_OBSERVE */
}

/**
 * Internal flags to control the treatment of responses (specifically
 * in presence of the No-Response option).
 */
enum respond_t { RESPONSE_DEFAULT, RESPONSE_DROP, RESPONSE_SEND };

#ifdef LIBCOAP_SERVER_SUPPORT
/**
 * Checks for No-Response option in given @p request and
 * returns @c 1 if @p response should be suppressed
 * according to RFC 7967.
 *
 * The value of the No-Response option is encoded as
 * follows:
 *
 * @verbatim
 *  +-------+-----------------------+-----------------------------------+
 *  | Value | Binary Representation |          Description              |
 *  +-------+-----------------------+-----------------------------------+
 *  |   0   |      <empty>          | Interested in all responses.      |
 *  +-------+-----------------------+-----------------------------------+
 *  |   2   |      00000010         | Not interested in 2.xx responses. |
 *  +-------+-----------------------+-----------------------------------+
 *  |   8   |      00001000         | Not interested in 4.xx responses. |
 *  +-------+-----------------------+-----------------------------------+
 *  |  16   |      00010000         | Not interested in 5.xx responses. |
 *  +-------+-----------------------+-----------------------------------+
 * @endverbatim
 *
 * @param request  The CoAP request to check for the No-Response option.
 *                 This parameter must not be NULL.
 * @param response The response that is potentially suppressed.
 *                 This parameter must not be NULL.
 * @return RESPONSE_DEFAULT when no special treatment is requested,
 *         RESPONSE_DROP    when the response must be discarded, or
 *         RESPONSE_SEND    when the response must be sent.
 */
static enum respond_t
no_response(coap_pdu_t *request, coap_pdu_t *response) {
  coap_opt_t *nores;
  coap_opt_iterator_t opt_iter;
  unsigned int val = 0;

  assert(request);
  assert(response);

  if (COAP_RESPONSE_CLASS(response->code) > 0) {
    nores = coap_check_option(request, COAP_OPTION_NORESPONSE, &opt_iter);

    if (nores) {
      val = coap_decode_var_bytes(coap_opt_value(nores), coap_opt_length(nores));

      /* The response should be dropped when the bit corresponding to
       * the response class is set (cf. table in function
       * documentation). When a No-Response option is present and the
       * bit is not set, the sender explicitly indicates interest in
       * this response. */
      if (((1 << (COAP_RESPONSE_CLASS(response->code) - 1)) & val) > 0) {
        return RESPONSE_DROP;
      } else {
        return RESPONSE_SEND;
      }
    }
  }

  /* Default behavior applies when we are not dealing with a response
   * (class == 0) or the request did not contain a No-Response option.
   */
  return RESPONSE_DEFAULT;
}

static coap_str_const_t coap_default_uri_wellknown =
          { sizeof(COAP_DEFAULT_URI_WELLKNOWN)-1,
           (const uint8_t *)COAP_DEFAULT_URI_WELLKNOWN };

static void
handle_request(coap_context_t *context, coap_session_t *session, coap_pdu_t *pdu) {
  coap_method_handler_t h = NULL;
  coap_pdu_t *response = NULL;
  coap_opt_filter_t opt_filter;
  coap_resource_t *resource;
  /* The respond field indicates whether a response must be treated
   * specially due to a No-Response option that declares disinterest
   * or interest in a specific response class. DEFAULT indicates that
   * No-Response has not been specified. */
  enum respond_t respond = RESPONSE_DEFAULT;

  coap_option_filter_clear(opt_filter);

  /* try to find the resource from the request URI */
  coap_string_t *uri_path = coap_get_uri_path(pdu);
  if (!uri_path)
    return;
  coap_str_const_t uri_path_c = { uri_path->length, uri_path->s };
  resource = coap_get_resource_from_uri_path(context, &uri_path_c);

  if ((resource == NULL) || (resource->is_unknown == 1)) {
    /* The resource was not found or there is an unexpected match against the
     * resource defined for handling unknown URIs.
     * Check if the request URI happens to be the well-known URI, or if the
     * unknown resource handler is defined, a PUT or optionally other methods,
     * if configured, for the unknown handler.
     *
     * if well-known URI generate a default response
     *
     * else if unknown URI handler defined, call the unknown
     *  URI handler (to allow for potential generation of resource
     *  [RFC7272 5.8.3]) if the appropriate method is defined.
     *
     * else if DELETE return 2.02 (RFC7252: 5.8.4.  DELETE)
     *
     * else return 4.04 */

    if (coap_string_equal(uri_path, &coap_default_uri_wellknown)) {
      /* request for .well-known/core */
      if (pdu->code == COAP_REQUEST_GET) { /* GET */
        coap_log(LOG_INFO, "create default response for %s\n",
                 COAP_DEFAULT_URI_WELLKNOWN);
        response = coap_wellknown_response(context, session, pdu);
      } else {
        coap_log(LOG_DEBUG, "method not allowed for .well-known/core\n");
        response = coap_new_error_response(pdu, COAP_RESPONSE_CODE(405),
          opt_filter);
      }
    } else if ((context->unknown_resource != NULL) &&
               ((size_t)pdu->code - 1 <
                (sizeof(resource->handler) / sizeof(coap_method_handler_t))) &&
               (context->unknown_resource->handler[pdu->code - 1])) {
      /*
       * The unknown_resource can be used to handle undefined resources
       * for a PUT request and can support any other registered handler
       * defined for it
       * Example set up code:-
       *   r = coap_resource_unknown_init(hnd_put_unknown);
       *   coap_register_handler(r, COAP_REQUEST_POST, hnd_post_unknown);
       *   coap_register_handler(r, COAP_REQUEST_GET, hnd_get_unknown);
       *   coap_register_handler(r, COAP_REQUEST_DELETE, hnd_delete_unknown);
       *   coap_add_resource(ctx, r);
       *
       * Note: It is not possible to observe the unknown_resource, a separate
       *       resource must be created (by PUT or POST) which has a GET
       *       handler to be observed
       */
      resource = context->unknown_resource;
    } else if (pdu->code == COAP_REQUEST_DELETE) {
      /*
       * Request for DELETE on non-existant resource (RFC7252: 5.8.4.  DELETE)
       */
      coap_log(LOG_DEBUG, "request for unknown resource '%*.*s',"
                          " return 2.02\n",
                          (int)uri_path->length,
                          (int)uri_path->length,
                          uri_path->s);
      response =
        coap_new_error_response(pdu, COAP_RESPONSE_CODE(202),
          opt_filter);
    } else { /* request for any another resource, return 4.04 */

      coap_log(LOG_DEBUG, "request for unknown resource '%*.*s', return 4.04\n",
               (int)uri_path->length, (int)uri_path->length, uri_path->s);
      response =
        coap_new_error_response(pdu, COAP_RESPONSE_CODE(404),
          opt_filter);
    }

    if (!resource) {
      if (response && (no_response(pdu, response) != RESPONSE_DROP)) {
        if (coap_send(session, response) == COAP_INVALID_TID)
          coap_log(LOG_WARNING, "cannot send response for transaction %u\n",
                   pdu->tid);
      } else {
        coap_delete_pdu(response);
      }

      response = NULL;

      coap_delete_string(uri_path);
      return;
    } else {
      if (response) {
        /* Need to delete unused response - it will get re-created further on */
        coap_delete_pdu(response);
      }
    }
  }

  /* the resource was found, check if there is a registered handler */
  if ((size_t)pdu->code - 1 <
    sizeof(resource->handler) / sizeof(coap_method_handler_t))
    h = resource->handler[pdu->code - 1];

  if (h) {
    coap_string_t *query = coap_get_query(pdu);
    int owns_query = 1;
     coap_log(LOG_DEBUG, "call custom handler for resource '%*.*s'\n",
              (int)resource->uri_path->length, (int)resource->uri_path->length,
              resource->uri_path->s);
    response = coap_pdu_init(pdu->type == COAP_MESSAGE_CON
      ? COAP_MESSAGE_ACK
      : COAP_MESSAGE_NON,
      0, pdu->tid, coap_session_max_pdu_size(session));

    /* Implementation detail: coap_add_token() immediately returns 0
       if response == NULL */
    if (coap_add_token(response, pdu->token_length, pdu->token)) {
      coap_binary_t token = { pdu->token_length, pdu->token };
      coap_opt_iterator_t opt_iter;
      coap_opt_t *observe = NULL;
      int observe_action = COAP_OBSERVE_CANCEL;

      /* check for Observe option */
      if (resource->observable) {
        observe = coap_check_option(pdu, COAP_OPTION_OBSERVE, &opt_iter);
        if (observe) {
          observe_action =
            coap_decode_var_bytes(coap_opt_value(observe),
              coap_opt_length(observe));

          if ((observe_action & COAP_OBSERVE_CANCEL) == 0) {
            coap_subscription_t *subscription;
            coap_block_t block2;
            int has_block2 = 0;

            if (coap_get_block(pdu, COAP_OPTION_BLOCK2, &block2)) {
              has_block2 = 1;
            }
            subscription = coap_add_observer(resource, session, &token, query, has_block2, block2);
            owns_query = 0;
            if (subscription) {
              coap_touch_observer(context, session, &token);
            }
          } else {
            coap_delete_observer(resource, session, &token);
          }
        }
      }

      h(context, resource, session, pdu, &token, query, response);

      if (query && owns_query)
        coap_delete_string(query);

      respond = no_response(pdu, response);
      if (respond != RESPONSE_DROP) {
        if (observe && (COAP_RESPONSE_CLASS(response->code) > 2)) {
          coap_delete_observer(resource, session, &token);
        }

        /* If original request contained a token, and the registered
         * application handler made no changes to the response, then
         * this is an empty ACK with a token, which is a malformed
         * PDU */
        if ((response->type == COAP_MESSAGE_ACK)
          && (response->code == 0)) {
          /* Remove token from otherwise-empty acknowledgment PDU */
          response->token_length = 0;
          response->used_size = 0;
        }

        if ((respond == RESPONSE_SEND)
          || /* RESPOND_DEFAULT */
          (response->type != COAP_MESSAGE_NON ||
          (response->code >= 64
            && !coap_mcast_interface(&node->local_if)))) {

          if (coap_send(session, response) == COAP_INVALID_TID)
            coap_log(LOG_DEBUG, "cannot send response for message %d\n",
                     pdu->tid);
        } else {
          coap_delete_pdu(response);
        }
      } else {
        coap_delete_pdu(response);
      }
      response = NULL;
    } else {
      coap_log(LOG_WARNING, "cannot generate response\r\n");
    }
  } else {
    if (coap_string_equal(uri_path, &coap_default_uri_wellknown)) {
      /* request for .well-known/core */
      coap_log(LOG_DEBUG, "create default response for %s\n",
               COAP_DEFAULT_URI_WELLKNOWN);
      response = coap_wellknown_response(context, session, pdu);
      coap_log(LOG_DEBUG, "have wellknown response %p\n", (void *)response);
    } else
      response = coap_new_error_response(pdu, COAP_RESPONSE_CODE(405),
        opt_filter);

    if (response && (no_response(pdu, response) != RESPONSE_DROP)) {
      if (coap_send(session, response) == COAP_INVALID_TID)
        coap_log(LOG_DEBUG, "cannot send response for transaction %d\n",
                 pdu->tid);
    } else {
      coap_delete_pdu(response);
    }
    response = NULL;
  }

  assert(response == NULL);
  coap_delete_string(uri_path);
}
#endif

static void
handle_response(coap_context_t *context, coap_session_t *session,
  coap_pdu_t *sent, coap_pdu_t *rcvd) {

  coap_send_ack(session, rcvd);

  /* In a lossy context, the ACK of a separate response may have
   * been lost, so we need to stop retransmitting requests with the
   * same token.
   */
  coap_cancel_all_messages(context, session, rcvd->token, rcvd->token_length);

  /* Call application-specific response handler when available. */
  if (context->response_handler) {
    context->response_handler(context, session, sent, rcvd, rcvd->tid);
  }
}

#ifdef LIBCOAP_SERVER_SUPPORT
static void
handle_signaling(coap_context_t *context, coap_session_t *session,
  coap_pdu_t *pdu) {
  coap_opt_iterator_t opt_iter;
  coap_opt_t *option;
  (void)context;

  coap_option_iterator_init(pdu, &opt_iter, COAP_OPT_ALL);

  if (pdu->code == COAP_SIGNALING_CSM) {
    while ((option = coap_option_next(&opt_iter))) {
      if (opt_iter.type == COAP_SIGNALING_OPTION_MAX_MESSAGE_SIZE) {
        coap_session_set_mtu(session, coap_decode_var_bytes(coap_opt_value(option),
          coap_opt_length(option)));
      } else if (opt_iter.type == COAP_SIGNALING_OPTION_BLOCK_WISE_TRANSFER) {
        /* ... */
      }
    }
    if (session->state == COAP_SESSION_STATE_CSM)
      coap_session_connected(session);
  } else if (pdu->code == COAP_SIGNALING_PING) {
    coap_pdu_t *pong = coap_pdu_init(COAP_MESSAGE_CON, COAP_SIGNALING_PONG, 0, 1);
    if (context->ping_handler) {
      context->ping_handler(context, session, pdu, pdu->tid);
    }
    if (pong) {
      coap_add_option(pong, COAP_SIGNALING_OPTION_CUSTODY, 0, NULL);
      coap_send(session, pong);
    }
  } else if (pdu->code == COAP_SIGNALING_PONG) {
    session->last_pong = session->last_rx_tx;
    if (context->pong_handler) {
      context->pong_handler(context, session, pdu, pdu->tid);
    }
  } else if (pdu->code == COAP_SIGNALING_RELEASE
          || pdu->code == COAP_SIGNALING_ABORT) {
    coap_session_disconnected(session, COAP_NACK_RST);
  }
}
#endif

void
coap_dispatch(coap_context_t *context, coap_session_t *session,
  coap_pdu_t *pdu) {
  coap_queue_t *sent = NULL;
  coap_opt_filter_t opt_filter;
#ifdef LIBCOAP_SERVER_SUPPORT
  coap_pdu_t *response;
#endif

#ifndef NDEBUG
  if (LOG_DEBUG <= coap_get_log_level()) {
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 40
#endif
    /* FIXME: get debug to work again **
    unsigned char addr[INET6_ADDRSTRLEN+8], localaddr[INET6_ADDRSTRLEN+8];
    if (coap_print_addr(remote, addr, INET6_ADDRSTRLEN+8) &&
        coap_print_addr(&packet->dst, localaddr, INET6_ADDRSTRLEN+8) )
      coap_log(LOG_DEBUG, "** received %d bytes from %s on interface %s:\n",
            (int)msg_len, addr, localaddr);

            */
    coap_show_pdu(LOG_DEBUG, pdu);
  }
#endif

  memset(opt_filter, 0, sizeof(coap_opt_filter_t));

  switch (pdu->type) {
    case COAP_MESSAGE_ACK:
      /* find transaction in sendqueue to stop retransmission */
      coap_remove_from_queue(&context->sendqueue, session, pdu->tid, &sent);

      if (session->con_active) {
        session->con_active--;
        if (session->state == COAP_SESSION_STATE_ESTABLISHED)
          /* Flush out any entries on session->delayqueue */
          coap_session_connected(session);
      }
      if (pdu->code == 0)
        goto cleanup;

#ifndef WITHOUT_OBSERVE
      /* if sent code was >= 64 the message might have been a
       * notification. Then, we must flag the observer to be alive
       * by setting obs->fail_cnt = 0. */
      if (sent && COAP_RESPONSE_CLASS(sent->pdu->code) == 2) {
        const coap_binary_t token =
        { sent->pdu->token_length, sent->pdu->token };
        coap_touch_observer(context, sent->session, &token);
      }
#endif
      break;

    case COAP_MESSAGE_RST:
      /* We have sent something the receiver disliked, so we remove
       * not only the transaction but also the subscriptions we might
       * have. */

      coap_log(LOG_ALERT, "got RST for message %d\n", pdu->tid);

      if (session->con_active) {
        session->con_active--;
        if (session->state == COAP_SESSION_STATE_ESTABLISHED)
          /* Flush out any entries on session->delayqueue */
          coap_session_connected(session);
      }

      /* find transaction in sendqueue to stop retransmission */
      coap_remove_from_queue(&context->sendqueue, session, pdu->tid, &sent);

      if (sent) {
        coap_cancel(context, sent);

        if(sent->pdu->type==COAP_MESSAGE_CON && context->nack_handler)
          context->nack_handler(context, sent->session, sent->pdu, COAP_NACK_RST, sent->id);
      }
      goto cleanup;

#ifdef LIBCOAP_SERVER_SUPPORT
    case COAP_MESSAGE_NON:        /* check for unknown critical options */
      if (coap_option_check_critical(context, pdu, opt_filter) == 0)
        goto cleanup;
      break;

    case COAP_MESSAGE_CON:        /* check for unknown critical options */
      if (coap_option_check_critical(context, pdu, opt_filter) == 0) {

        /* FIXME: send response only if we have received a request. Otherwise,
         * send RST. */
        response =
          coap_new_error_response(pdu, COAP_RESPONSE_CODE(402), opt_filter);

        if (!response) {
          coap_log(LOG_WARNING,
                   "coap_dispatch: cannot create error response\n");
        } else {
          if (coap_send(session, response) == COAP_INVALID_TID)
            coap_log(LOG_WARNING, "coap_dispatch: error sending response\n");
        }

        goto cleanup;
      }
#endif
    default: break;
  }

  /* Pass message to upper layer if a specific handler was
    * registered for a request that should be handled locally. */
#ifdef LIBCOAP_SERVER_SUPPORT
  if (COAP_PDU_IS_SIGNALING(pdu))
    handle_signaling(context, session, pdu);
  else if (COAP_PDU_IS_REQUEST(pdu))
    handle_request(context, session, pdu);
  else 
#endif
  if (COAP_PDU_IS_RESPONSE(pdu))
    handle_response(context, session, sent ? sent->pdu : NULL, pdu);
  else {
    if (COAP_PDU_IS_EMPTY(pdu)) {
      if (context->ping_handler) {
        context->ping_handler(context, session,
          pdu, pdu->tid);
      }
    }
    coap_log(LOG_DEBUG, "dropped message with invalid code (%d.%02d)\n",
             COAP_RESPONSE_CLASS(pdu->code),
      pdu->code & 0x1f);

    if (!coap_is_mcast(&session->local_addr)) {
      if (COAP_PDU_IS_EMPTY(pdu)) {
        if (session->proto != COAP_PROTO_TCP && session->proto != COAP_PROTO_TLS) {
          coap_tick_t now;
          coap_ticks(&now);
          if (session->last_tx_rst + COAP_TICKS_PER_SECOND/4 < now) {
            coap_send_message_type(session, pdu, COAP_MESSAGE_RST);
            session->last_tx_rst = now;
          }
        }
      }
      else {
        coap_send_message_type(session, pdu, COAP_MESSAGE_RST);
      }
    }
  }

cleanup:
  coap_delete_node(sent);
}

int
coap_handle_event(coap_context_t *context, coap_event_t event, coap_session_t *session) {
  coap_log(LOG_DEBUG, "***EVENT: 0x%04x\n", event);

  if (context->handle_event) {
    return context->handle_event(context, event, session);
  } else {
    return 0;
  }
}

int
coap_can_exit(coap_context_t *context) {
#ifdef LIBCOAP_SERVER_SUPPORT
  coap_endpoint_t *ep;
#endif
  coap_session_t *s;
  if (!context)
    return 1;
  if (context->sendqueue)
    return 0;
#ifdef LIBCOAP_SERVER_SUPPORT
  LL_FOREACH(context->endpoint, ep) {
    LL_FOREACH(ep->sessions, s) {
      if (s->delayqueue)
        return 0;
    }
  }
#endif
  LL_FOREACH(context->sessions, s) {
    if (s->delayqueue)
      return 0;
  }
  return 1;
}

static int coap_started = 0;

void coap_startup(void) {
  if (coap_started)
    return;
  coap_started = 1;
#if defined(HAVE_WINSOCK2_H)
  WORD wVersionRequested = MAKEWORD(2, 2);
  WSADATA wsaData;
  WSAStartup(wVersionRequested, &wsaData);
#endif
  coap_clock_init();
#if defined(WITH_LWIP_LIBCOAP)
  prng_init(LWIP_RAND());
#elif defined(WITH_CONTIKI)
  prng_init(0);
#elif !defined(_WIN32)
  prng_init(0);
#endif
  coap_dtls_startup();
}

void coap_cleanup(void) {
#if defined(HAVE_WINSOCK2_H)
  WSACleanup();
#endif
}

#ifdef WITH_CONTIKI

/*---------------------------------------------------------------------------*/
/* CoAP message retransmission */
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(coap_retransmit_process, ev, data) {
  coap_tick_t now;
  coap_queue_t *nextpdu;

  PROCESS_BEGIN();

  coap_log(LOG_DEBUG, "Started retransmit process\n");

  while (1) {
    PROCESS_YIELD();
    if (ev == PROCESS_EVENT_TIMER) {
      if (etimer_expired(&the_coap_context.retransmit_timer)) {

        nextpdu = coap_peek_next(&the_coap_context);

        coap_ticks(&now);
        while (nextpdu && nextpdu->t <= now) {
          coap_retransmit(&the_coap_context, coap_pop_next(&the_coap_context));
          nextpdu = coap_peek_next(&the_coap_context);
        }

        /* need to set timer to some value even if no nextpdu is available */
        etimer_set(&the_coap_context.retransmit_timer,
          nextpdu ? nextpdu->t - now : 0xFFFF);
      }
#ifndef WITHOUT_OBSERVE
      if (etimer_expired(&the_coap_context.notify_timer)) {
        coap_check_notify(&the_coap_context);
        etimer_reset(&the_coap_context.notify_timer);
      }
#endif /* WITHOUT_OBSERVE */
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

#endif /* WITH_CONTIKI */

#ifdef WITH_LWIP_LIBCOAP
/* FIXME: retransmits that are not required any more due to incoming packages
 * do *not* get cleared at the moment, the wakeup when the transmission is due
 * is silently accepted. this is mainly due to the fact that the required
 * checks are similar in two places in the code (when receiving ACK and RST)
 * and that they cause more than one patch chunk, as it must be first checked
 * whether the sendqueue item to be dropped is the next one pending, and later
 * the restart function has to be called. nothing insurmountable, but it can
 * also be implemented when things have stabilized, and the performance
 * penality is minimal
 *
 * also, this completely ignores COAP_RESOURCE_CHECK_TIME.
 * */

static void coap_retransmittimer_execute(void *arg) {
  coap_context_t *ctx = (coap_context_t*)arg;
  coap_tick_t now;
  coap_tick_t elapsed;
  coap_queue_t *nextinqueue;

  ctx->timer_configured = 0;

  coap_ticks(&now);

  elapsed = now - ctx->sendqueue_basetime; /* that's positive for sure, and unless we haven't been called for a complete wrapping cycle, did not wrap */

  nextinqueue = coap_peek_next(ctx);
  while (nextinqueue != NULL) {
    if (nextinqueue->t > elapsed) {
      nextinqueue->t -= elapsed;
      break;
    } else {
      elapsed -= nextinqueue->t;
      coap_retransmit(ctx, coap_pop_next(ctx));
      nextinqueue = coap_peek_next(ctx);
    }
  }

  ctx->sendqueue_basetime = now;

  coap_retransmittimer_restart(ctx);
}

static void coap_retransmittimer_restart(coap_context_t *ctx) {
  coap_tick_t now, elapsed, delay;

  if (ctx->timer_configured) {
    printf("clearing\n");
    sys_untimeout(coap_retransmittimer_execute, (void*)ctx);
    ctx->timer_configured = 0;
  }
  if (ctx->sendqueue != NULL) {
    coap_ticks(&now);
    elapsed = now - ctx->sendqueue_basetime;
    if (ctx->sendqueue->t >= elapsed) {
      delay = ctx->sendqueue->t - elapsed;
    } else {
      /* a strange situation, but not completely impossible.
       *
       * this happens, for example, right after
       * coap_retransmittimer_execute, when a retransmission
       * was *just not yet* due, and the clock ticked before
       * our coap_ticks was called.
       *
       * not trying to retransmit anything now, as it might
       * cause uncontrollable recursion; let's just try again
       * with the next main loop run.
       * */
      delay = 0;
    }

    printf("scheduling for %d ticks\n", delay);
    sys_timeout(delay, coap_retransmittimer_execute, (void*)ctx);
    ctx->timer_configured = 1;
  }
}
#endif
