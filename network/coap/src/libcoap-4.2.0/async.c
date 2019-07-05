/* async.c -- state management for asynchronous messages
 *
 * Copyright (C) 2010,2011 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

/**
 * @file async.c
 * @brief state management for asynchronous messages
 */

#ifndef WITHOUT_ASYNC

#include "coap_config.h"
#include "coap.h"
#include "async.h"
#include "coap_debug.h"
#include "coap_mem.h"
#include "utlist.h"

/* utlist-style macros for searching pairs in linked lists */
#define SEARCH_PAIR(head,out,field1,val1,field2,val2)   \
  SEARCH_PAIR2(head,out,field1,val1,field2,val2,next)

#define SEARCH_PAIR2(head,out,field1,val1,field2,val2,next)             \
  do {                                                                  \
    LL_FOREACH2(head,out,next) {                                        \
      if ((out)->field1 == (val1) && (out)->field2 == (val2)) break;    \
    }                                                                   \
} while(0)

coap_async_state_t *
coap_register_async(coap_context_t *context, coap_session_t *session,
                    coap_pdu_t *request, unsigned char flags, void *data) {
  coap_async_state_t *s;
  coap_tid_t id = request->tid;

  SEARCH_PAIR(context->async_state,s,session,session,id,id);

  if (s != NULL) {
    /* We must return NULL here as the caller must know that he is
     * responsible for releasing @p data. */
    coap_log(LOG_DEBUG,
         "asynchronous state for transaction %d already registered\n", id);
    return NULL;
  }

  /* store information for handling the asynchronous task */
  s = (coap_async_state_t *)coap_malloc(sizeof(coap_async_state_t));
  if (!s) {
    coap_log(LOG_CRIT, "coap_register_async: insufficient memory\n");
    return NULL;
  }

  memset(s, 0, sizeof(coap_async_state_t));

  /* set COAP_ASYNC_CONFIRM according to request's type */
  s->flags = flags & ~COAP_ASYNC_CONFIRM;
  if (request->type == COAP_MESSAGE_CON)
    s->flags |= COAP_ASYNC_CONFIRM;

  s->appdata = data;
  s->session = coap_session_reference( session );
  s->id = id;

  if (request->token_length) {
    /* A token can be up to 8 bytes */
    s->tokenlen = (request->token_length > 8) ? 8 : request->token_length;
    memcpy(s->token, request->token, s->tokenlen);
  }

  coap_touch_async(s);

  LL_PREPEND(context->async_state, s);

  return s;
}

coap_async_state_t *
coap_find_async(coap_context_t *context, coap_session_t *session, coap_tid_t id) {
  coap_async_state_t *tmp;
  SEARCH_PAIR(context->async_state,tmp,session,session,id,id);
  return tmp;
}

int
coap_remove_async(coap_context_t *context, coap_session_t *session,
                  coap_tid_t id, coap_async_state_t **s) {
  coap_async_state_t *tmp = coap_find_async(context, session, id);

  if (tmp)
    LL_DELETE(context->async_state,tmp);

  *s = tmp;
  return tmp != NULL;
}

void
coap_free_async(coap_async_state_t *s) {
  if (s) {
    if (s->session) {
      coap_session_release(s->session);
    }
    if ((s->flags & COAP_ASYNC_RELEASE_DATA) != 0) {
      coap_free(s->appdata);
    }
    coap_free(s);
  }
}

#else
void does_not_exist(void);        /* make some compilers happy */
#endif /* WITHOUT_ASYNC */
