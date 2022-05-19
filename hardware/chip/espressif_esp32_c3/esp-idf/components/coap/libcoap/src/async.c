/* async.c -- state management for asynchronous messages
 *
 * Copyright (C) 2010,2011,2021 Olaf Bergmann <bergmann@tzi.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

/**
 * @file async.c
 * @brief state management for asynchronous messages
 */

#include "coap3/coap_internal.h"

#ifndef WITHOUT_ASYNC

/* utlist-style macros for searching pairs in linked lists */
#define SEARCH_PAIR(head,out,field1,val1,field2,val2,field3,val3)   \
  SEARCH_PAIR3(head,out,field1,val1,field2,val2,field3,val3,next)

#define SEARCH_PAIR3(head,out,field1,val1,field2,val2,field3,val3,next) \
  do {                                                                  \
    LL_FOREACH2(head,out,next) {                                        \
      if ((out)->field1 == (val1) && (out)->field2 == (val2) &&         \
          ((val2) == 0 || memcmp((out)->field3, (val3), (val2)) == 0)) break; \
    }                                                                   \
} while(0)

int
coap_async_is_supported(void) {
  return 1;
}

coap_async_t *
coap_register_async(coap_session_t *session,
                    const coap_pdu_t *request, coap_tick_t delay) {
  coap_async_t *s;
  size_t len;
  const uint8_t *data;

  if (!COAP_PDU_IS_REQUEST(request))
    return NULL;

  SEARCH_PAIR(session->context->async_state, s,
              session, session,
              pdu->token_length, request->token_length,
              pdu->token, request->token);

  if (s != NULL) {
    coap_log(LOG_DEBUG,
         "asynchronous state for mid=0x%x already registered\n", request->mid);
    return NULL;
  }

  /* store information for handling the asynchronous task */
  s = (coap_async_t *)coap_malloc(sizeof(coap_async_t));
  if (!s) {
    coap_log(LOG_CRIT, "coap_register_async: insufficient memory\n");
    return NULL;
  }

  memset(s, 0, sizeof(coap_async_t));

  /* Note that this generates a new MID */
  s->pdu = coap_pdu_duplicate(request, session, request->token_length,
                              request->token, NULL);
  if (s->pdu == NULL) {
    coap_free_async(session, s);
    coap_log(LOG_CRIT, "coap_register_async: insufficient memory\n");
    return NULL;
  }

  if (coap_get_data(request, &len, &data)) {
    coap_add_data(s->pdu, len, data);
  }

  s->session = coap_session_reference( session );

  coap_async_set_delay(s, delay);

  LL_PREPEND(session->context->async_state, s);

  return s;
}

void
coap_async_set_delay(coap_async_t *async, coap_tick_t delay) {
  assert(async != NULL);

  if (delay) {
    coap_ticks(&async->delay);
    async->delay += delay;
  }
  else
    async->delay = 0;
  coap_log(LOG_DEBUG, "   %s: Request for delayed for %u.%03u secs\n",
           coap_session_str(async->session),
           (unsigned int)(delay / COAP_TICKS_PER_SECOND),
           (unsigned int)((delay % COAP_TICKS_PER_SECOND) *
               1000 / COAP_TICKS_PER_SECOND));
}


coap_async_t *
coap_find_async(coap_session_t *session, coap_bin_const_t token) {
  coap_async_t *tmp;
  SEARCH_PAIR(session->context->async_state, tmp,
              session, session,
              pdu->token_length, token.length,
              pdu->token, token.s);
  return tmp;
}

static void
coap_free_async_sub(coap_context_t *context, coap_async_t *s) {
  if (s) {
    LL_DELETE(context->async_state,s);
    if (s->session) {
      coap_session_release(s->session);
    }
    if (s->pdu) {
      coap_delete_pdu(s->pdu);
      s->pdu = NULL;
    }
    coap_free(s);
  }
}

void
coap_free_async(coap_session_t *session, coap_async_t *s) {
  coap_free_async_sub(session->context, s);
}

void
coap_delete_all_async(coap_context_t *context) {
  coap_async_t *astate, *tmp;

  LL_FOREACH_SAFE(context->async_state, astate, tmp) {
    coap_free_async_sub(context, astate);
  }
  context->async_state = NULL;
}

void
coap_async_set_app_data(coap_async_t *async, void *app_data) {
  async->appdata = app_data;
}

void *
coap_async_get_app_data(const coap_async_t *async) {
  return async->appdata;
}

#else

int
coap_async_is_supported(void) {
  return 0;
}

coap_async_t *
coap_register_async(coap_session_t *session,
                    const coap_pdu_t *request,
                    coap_tick_t delay) {
  (void)session;
  (void)request;
  (void)delay;
  return NULL;
}

void
coap_async_set_delay(coap_async_t *async, coap_tick_t delay) {
  (void)async;
  (void)delay;
}

void
coap_free_async(coap_session_t *session, coap_async_t *async) {
  (void)session;
  (void)async;
}

coap_async_t *
coap_find_async(coap_session_t *session,
                coap_bin_const_t token) {
  (void)session;
  (void)token;
  return NULL;
}

void
coap_async_set_app_data(coap_async_t *async, void *app_data) {
  (void)async;
  (void)app_data;
}

void *
coap_async_get_app_data(const coap_async_t *async) {
  (void)async;
  return NULL;
}

#endif /* WITHOUT_ASYNC */
